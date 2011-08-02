/**
 *  Asymmetry Analysis of RHIC pC Polarimeter
 *
 *  Authors: I. Nakagawa
 *           Dmitri Smirnov
 *
 * Oct 18, 2010 - Dmitri Smirnov
 *    - Minor code clean-up
 *
 * Feb 10, 2011 - Dmitri Smirnov
 *    - Significantly simplified the code. Removed unnecessary copies of global
 *      variables
 *
 */


#include "AsymProcess.h"

#include "TH1F.h"

#include "AnaInfo.h"
#include "AsymRoot.h"
#include "Kinema.h"
#include "RunInfo.h"

using namespace std;

// A manual switch operation is required for following routine.
// This routine calls special text output routine for spin tune measurements
//
// SpinTuneOutput(event->bid,si);

extern void HHF1(int, float, float);
extern void HHF2(int, float, float, float);

int init=0;
//float EnergyBin[NTBIN+1]={320,400,480,600,800,1000,1200}; // 6+1
float EnergyBin[NTBIN+1]={320,360,400,440,480,520,600,700,800,900,1000,1100,1200,1300,1400}; // 14+1
//float EnergyBin[NTBIN+1]={200,220,240,260,280,300,340,380,440,500,600,750,900,1100,1400,1700,2000,2300,2600,2900,3100,3300}; // 21+1
//float EnergyBin[NTBIN+1]={100,150,200,250,300,350,400,500,600,800,1100,1500,2000,2500,3000,3300,3600,4000,4250,4500,4750}; // 21+1
//float EnergyBin[NTBIN+1]={100,120,140,160,180,200,230,260,290,320,350,400,450,500,600,700,800,900,1000,1100,1200,1300,1400}; // 22+1
//float EnergyBin[NTBIN+1]={500,600,700,800,900,1000,1100,1200,1400,1600,1800,2000,2200,2400,2600,2800,3000,3300,3600,3900,4500,5000,6000,7000,8000,9000,10000}; // 26+1


/**
 * Main process event routine. This routine is called event by event basis
 * Input       : processEvent *event
 */
void event_process(processEvent *event)
{
   int   delim = event->delim ;
   int   st    = event->stN;        // 0 - 71
   int   si    = (int) (st/12);     // 0 - 5
   float Emin  = (float) gAnaInfo->enel;
   float Emax  = (float) gAnaInfo->eneu;

   Ntotal[event->bid]++;

   // write out t-dependence energy bin into log file (only once)
   if (!init) {
      fprintf(stdout, "EnergyBin:");
      for (int k=1; k<=NTBIN; k++) fprintf(stdout, "%6.0f", (EnergyBin[k]+EnergyBin[k-1])/2.);

      fprintf(stdout, "\nEnergydBin:");
      for (int k=1; k<=NTBIN; k++) fprintf(stdout, "%6.0f", (EnergyBin[k]-EnergyBin[k-1])/2.);

      fprintf(stdout,"\n");

      //ds: XXX
      gAsymRoot->PreProcess();

      init = 1;
   }

   // Feedback Mode
   //if (Flag.feedback){

   // tdc > 6 ns, effectively...
   /*
   if ((event->tdc > 2*gConfigInfo->data.chan[st].Window.split.Beg)) {

      // random numbers in order to smear for the integer reading
      int vlen = 1;
      float rand1, rand2;

      hhrammar_(&rand1, &vlen);
      hhrammar_(&rand2, &vlen);

      float edepo = gConfigInfo->data.chan[st].acoef * (event->amp+rand2-0.5);
      float e = ekin(edepo, gConfigInfo->data.chan[st].dwidth);
      //e = gConfigInfo->data.chan[st].edead + gConfigInfo->data.chan[st].ecoef *
      //  (event->amp + rand2 - 0.5);

      float t = gRunConsts[st+1].Ct * (event->tdc + rand1 - 0.5) - gConfigInfo->data.chan[st].t0 - gAnaInfo->tshift;

      float Mass = t*t*e* gRunConsts[st+1].T2M * k2G;

      //float delt = t - gRunConsts[st+1].E2T/sqrt(e);

      if (e > Emin && e < Emax && Mass > gAnaInfo->MassLimit && Mass < 20.) {
         mass_feedback[st]->Fill(Mass);
         mass_feedback_all->Fill(Mass);
      }
   }
   */

   //return;
   //}

   // DeadLayer Mode
   if (gAnaInfo->DMODE) {

      // 2*gConfigInfo->data.chan[st].Window.split.Beg = 6
      // ds: A prelim cut on tdc? 6 ns?
      if (event->tdc > 2*gConfigInfo->data.chan[st].Window.split.Beg) {
     
         float edepo, e, t, delt, Mass;

         KinemaReconstruction(1, event, gConfigInfo, st, edepo, e, t, delt, Mass);

         // Get rid of bunch zero due to laser event after Run09
         if (event->bid) HHF2(15000+st+1, edepo, t + gConfigInfo->data.chan[st].t0, 1.);

         if (fabs(delt) < gRunConsts[st+1].M2T*feedback.RMS[st]*gAnaInfo->MassSigma/sqrt(e))
         {
            HHF2(15100+st+1, edepo, t + gConfigInfo->data.chan[st].t0, 1.);
            if ( e > Emin && e < Emax) Ngood[event->bid]++;
         }
      }

      return;
   }

   // Calibration hists
   if ( gAnaInfo->HasAlphaBit() ) {
      //int vlen = 1;
      //float rand1, rand2;
      //hhrammar_(&rand1, &vlen);
      //hhrammar_(&rand2, &vlen);

      // reducing the background
      //if ((event->tdc>50) && (event->tdc <200) && (event->amp<215))
         //ds: HHF1(12000+st+1, event->amp+rand2-0.5, 1.);
         //ds: HHF1(12000+st+1, event->amp, 1.);
      //}

      // Get address of the histogram container
      ChannelEvent *ch = gAsymRoot->fChannelEvent;

      if (!ch->PassCutSiliconChannel()) return;

      gAsymRoot->fHists->Fill(ch);

      if (ch->PassCutRawAlpha()) {
	      gAsymRoot->fHists->Fill(ch, "cut1");
      }
      
      return;
   }

   // Nomal Process Mode
   // fill profile histograms at the 1st visit
   /*
   if (Nevtot==1) {

      // Wall Current Monitor Operation
      average.total   = 0;
      average.counter = 0;

      for (int j=0; j<NBUNCH; j++) {
         wall_current_monitor->Fill(j, gRunInfo->fWallCurMon[j+1]);
         HHF1(10030, (float) j, (float) gRunInfo->fWallCurMon[j+1]);

         if ( (fabs(gRunInfo->fWallCurMonAve-gRunInfo->fWallCurMon[j+1])/gRunInfo->fWallCurMonAve < gAnaInfo->WCMRANGE) && gFillPattern[j]) {
            average.total+=gRunInfo->fWallCurMon[j+1];
            wcmfillpat[j]=1;
            ++average.counter;
         }
      }

      average.average = average.total/float(average.counter);

      // ---------------------------------------------------- //
      //   Root Histogram Booking using feedback results      //
      // ---------------------------------------------------- //
      gAsymRoot->BookHists2(feedback);

      // Online Banana Cut
      for (int strip=0; strip<72; strip++) {

         for (int j=0; j<HENEBIN; j++) {

            float e = ((float) j + 0.5) * (HMAXENE/float(HENEBIN));

            // Outside Of energy cuts
            if (e < Emin) {
                HHF1(13500+strip+1, e, 0.);
                HHF1(13600+strip+1, e, 0.);
            } else if (e > Emax){
                HHF1(13500+strip+1, e, 0.);
                HHF1(13600+strip+1, e, 0.);
            } else {

               if (gAnaInfo->CBANANA == 0){
                   HHF1(13500+strip+1, e, gRunConsts[st+1].E2T/sqrt(e)
                        -gConfigInfo->data.chan[strip].ETCutW);
                   HHF1(13600+strip+1, e, gRunConsts[st+1].E2T/sqrt(e)
                        +gConfigInfo->data.chan[strip].ETCutW);
               } else if (gAnaInfo->CBANANA == 1) {
                   HHF1(13500+strip+1, e, gRunConsts[st+1].E2T/sqrt(e)
                        +(float)(gAnaInfo->widthl));
                   HHF1(13600+strip+1, e, gRunConsts[st+1].E2T/sqrt(e)
                        +(float)(gAnaInfo->widthu));
               } else if (gAnaInfo->CBANANA == 2) {
                   HHF1(13500+strip+1, e, gRunConsts[st+1].E2T/sqrt(e)
                        + gRunConsts[st+1].M2T*feedback.RMS[strip]*gAnaInfo->MassSigma/sqrt(e)) ;
                   HHF1(13600+strip+1, e, gRunConsts[st+1].E2T/sqrt(e)
                        - gRunConsts[st+1].M2T*feedback.RMS[strip]*gAnaInfo->MassSigma/sqrt(e)) ;
               }
            }
         }
      }

      cout << "Finished first event initialization" << endl;
   }
   */

   // Main Process Routine event by event starts here

   // TDC Dists without Cut
   //HHF1(11200+st+1, (float)event->tdc, 1.);

   // Target Histogram
   //if (st >= 72 && st <= 75) {
   //   HHF2(25060, cntr.revolution/RHIC_REVOLUTION_FREQ, tgt.x, 1);
   //}

   // Get address of the histogram container
   ChannelEvent *ch = gAsymRoot->fChannelEvent;

   // Fill target histograms
   if (gAnaInfo->HasTargetBit()) {

      //if (ch->GetChannelId() > NSTRIP)
      //   printf("channel1: %d, %d\n", ch->GetChannelId(), gConfigInfo->data.NumChannels);

      if (ch->PassCutTargetChannel()) {
         //printf("channel: %d\n", ch->GetChannelId());
         gAsymRoot->fHists->d["targets"]->Fill(ch);
      }
   }

   if (gAnaInfo->HasNormalBit()) {

      if (!ch->PassCutSiliconChannel()) return;

      //if () {

         gAsymRoot->fHists->Fill(ch);

         //if (fabs(gFillPattern[event.bid]) != 1)

         //printf("channel %d, %d, %d, %d\n", ch->GetChannelId(), ch->PassCutNoise(), ch->PassCutKinEnergyAEDepend(), ch->PassCutEnabledChannel());

         //if (ch->PassCutPulser() && ch->PassCutNoise() && ch->PassCutKinEnergyADLCorrEstimate())
         //if (ch->PassCutNoise() && ch->PassCutKinEnergyADLCorrEstimate())
         //if (ch->PassCutNoise() && ch->PassCutKinEnergyAEDepend() && ch->PassCutEnabledChannel())
         if ( ch->PassCutNoise() && ch->PassCutKinEnergyAEDepend() && ch->PassCutEnabledChannel() )
         {
	         gAsymRoot->fHists->Fill(ch, "_cut1");

            if (ch->PassCutCarbonMass()) {

	            gAsymRoot->fHists->Fill(ch, "_cut2");

               //((CnipolRunHists*) gAsymRoot->fHists)->Fill(ch);
               //((TH1*) gAsymRoot->fHists->o["hTargetSteps"])->Fill(ch->GetDelimiterId());
            }
         }
      //} else {
      //   if ( ch->PassCutNoise() && ch->PassCutKinEnergyAEDependAverage() &&
      //        ch->PassCutCarbonMassEstimate())
      //   {
	   //      gAsymRoot->fHists->Fill(ch, "_cut2");
      //   }
      //}

      //if ( ch->PassCutEmptyBunch() ) {
      //   gAsymRoot->fHists->Fill(ch, "empty_bunch");
      //}
   }

   //ds
   return;

   // 2*gConfigInfo->data.chan[st].Window.split.Beg = 6
   //if ((event->tdc > 2*gConfigInfo->data.chan[st].Window.split.Beg))
   //ds: Proceed only if the time value is greater than...
   if ((event->tdc <= 2*gConfigInfo->data.chan[st].Window.split.Beg))
      return;

   // random numbers in order to smear for the integer reading
   int   vlen = 1;
   float rand1, rand2;
   hhrammar_(&rand1, &vlen);
   hhrammar_(&rand2, &vlen);

   // Strip distribution
   //HHF1(10300+si+1, (float)(st-si*12)+1, 1.);

   // bunch distribution
   //HHF1(10000, (float)event->bid, 1.);
   //gAsymRoot->bunch_dist->Fill(event->bid);

   // Integral
   float Integ = (event->intg) << (2+gConfigInfo->data.CSR.split.iDiv);

   //ds: Some manipulations with ADC counts...
   float amp_int = (Integ - gConfigInfo->data.chan[st].A0) / gConfigInfo->data.chan[st].A1;
    
   // Energy corrected with Dead layer info (keV)

   // energy deposit in Si strip
   //ds: Are we using the same acoef??? for amp and int?
   float edepo = gConfigInfo->data.chan[st].acoef * (event->amp+rand2-0.5);
   //ds: edepo_int = deposited energy in keV?
   //float edepo_int = gConfigInfo->data.chan[st].acoef * (amp_int+rand2-0.5);
   float edepo_int = gConfigInfo->data.chan[st].ecoef * (amp_int+rand2-0.5);

   // === NEW float dwidth = gConfigInfo->data.chan[st].IACutW; // new entry
   float e      = ekin(edepo, gConfigInfo->data.chan[st].dwidth);
   float e_int  = ekin(edepo_int, gConfigInfo->data.chan[st].dwidth);
   float sqrt_e = sqrt(e);

   /*
   e = gConfigInfo->data.chan[st].edead + gConfigInfo->data.chan[st].ecoef *
     (event->amp + rand2 - 0.5);
   e_int = gConfigInfo->data.chan[st].edead +
           gConfigInfo->data.chan[st].ecoef * (amp_int + rand2 - 0.5);
   */

   // For A_N Calculation (Cross section)
   //if (e > Emin && e < Emax) {
   //    HHF1(10040, e, 1.);
   //}

   //HHF1(10400+si+1, e, 1.);

   //if (gSpinPattern[event->bid] > 0) {
   //    HHF1(10500+st+1, e, 1.);
   //} else {
   //    HHF1(10600+st+1, e, 1.);
   //}

   // Time Of Flight
   float t;

   if (gAnaInfo->RAMPMODE == 1) {

      t = gRunConsts[st+1].Ct * (event->tdc + rand1 - 0.5)
          - gConfigInfo->data.chan[st].t0
          - (ramptshift[(int)event->delim/20]-ramptshift[0]) - gAnaInfo->tshift;

   } else if (gAnaInfo->ZMODE == 0) {   // normal runs

      t = gRunConsts[st+1].Ct * (event->tdc + rand1 - 0.5) - gConfigInfo->data.chan[st].t0
           - gAnaInfo->tshift - feedback.tedev[st]/sqrt_e;
   } else  {

      t = gRunConsts[st+1].Ct * (event->tdc + rand1 - 0.5) - gAnaInfo->tshift ;
   }

   //ds
   //printf("%10.3f, %10d, %10d, %10.3f, %10.3f, %10.3f\n", ch->GetTime(), ch->fChannel.fTdc, event->tdc, t, gAnaInfo->tshift, gRunConsts[st+1].Ct);

   float delt = t - gRunConsts[st+1].E2T/sqrt_e;

   // Invariant Mass
   float Mass = t * t * e * gRunConsts[st+1].T2M * k2G;

   mass_nocut[st]->Fill(Mass);
   mass_nocut_all->Fill(Mass);

   // Mass mode
   //if (gAnaInfo->MMODE == 1) {
   //   if (e > Emin && e < Emax) {

   //      HHF1(16000+st+1, Mass, 1.);

   //      if (Mass > gAnaInfo->MassLimit && Mass < 14.)
   //         HHF1(17200+st+1, Mass, 1.);
   //   }
   //}

   // for T0 (cable length dependence)
   //if (gAnaInfo->TMODE == 1 && edepo!=0.){
   //   HHF2(12100+st+1, (float)(1./sqrt_e), t, 1.);
   //}

   //// Banana Plots (E-T)
   //if (gAnaInfo->BMODE == 1) {

   //   HHF2(13000+st+1, e, t, 1.);

   //   if (fabs(Mass - 3.726) < 2.5)  HHF2(13700+st+1, e, t, 1.);
   //   if (fabs(Mass - 3.726) > 2.5)  HHF2(13800+st+1, e, t, 1.);

   //   HHF2(13100+st+1, event->amp, event->tdc, 1.);
   //   HHF2(13200+st+1, event->amp, event->tdcmax, 1.);
   //   HHF1(14000+st+1, event->bid, 1.);
   //}

   /*
   if (gAnaInfo->RAMPMODE==1) {0
       // total RAMPTIME sec
       // 1sec for each bin, delimiters are 20Hz rate
       int rbin = (int)(event->delim/20);
       if (e>600 && e<650) {
           HHF1(20000+rbin,t,1.);
       }
   }
   */

   //// integral vs. amplitede
   //if (gAnaInfo->AMODE == 1) {
   //   HHF2(12200+st+1, event->amp, Integ, 1.);
   //   HHF2(12300+st+1, Integ, t, 1.);
   //}

   //// -t slope
   //if ( (Mass < 11.18 + 5.0) && (Mass > 11.18 - 5.0) ) {
   //   HHF1(10450+si+1, 2*11.18*e/1000000., 1.);
   //   HHF1(10460+si+1, 2*11.18*e_int/1000000., 1.);
   //}

   // t vs. E (banana with no cut)
   t_vs_e[st]->Fill(e, t);
   //ds:
   //t_vs_e[st]->Fill(e_int, t);

   // Fill ntuple
   /*
   if ((Nevtot<NTLIMIT)&&(gAnaInfo->NTMODE==1)&&(fmod(float(Nevtot),10)==0)) {
       atdata.ia = (long)event->amp;
       atdata.is = (long)event->intg;
       atdata.it = (long)event->tdc;
       atdata.ib = (long)event->bid;
       atdata.id = (long)event->delim;
       atdata.strip = (int)st+1;
       atdata.e = (float)e;
       atdata.tof = (float)t;

       atdata.spin = gSpinPattern[event->bid];

       //HHFNT(1);
   }
   */

   // background estimation
   if ( (delt <= -2*gConfigInfo->data.chan[st].ETCutW) && e>Emin && e<Emax)
   {
      Nback[event->bid]++;
   }

   // Bunch distribution (only -t cut)
   //if (e > Emin && e < Emax) {
   //    HHF1(10020,(float)event->bid,1.);
   //}

   //------------------------------------------------------
   //                Banana Curve Cut
   //------------------------------------------------------
   //if ( ((delt> -1. * gConfigInfo->data.chan[st].ETCutW ) &&
   //      (delt<  1. * gConfigInfo->data.chan[st].ETCutW ) &&
   //      (gAnaInfo->CBANANA == 0))
   //     ||
   //     ((delt > (float)(gAnaInfo->widthl) ) &&
   //      (delt < (float)(gAnaInfo->widthu) ) &&
   //      (gAnaInfo->CBANANA == 1))
   //     ||
   //     ((fabs(delt) < gRunConsts[st+1].M2T*feedback.RMS[st]*gAnaInfo->MassSigma/sqrt_e)
   //       && (gAnaInfo->CBANANA == 2)) // default 
   //     )

   if (ch->PassCutCarbonMass())
   {
      // -t dependence
      float minus_t = 2 * e * MASS_12C * k2G * k2G;
      //printf("UUU0: %d, %f\n", si, minus_t);

      if (e > EnergyBin[0]) {
         for (int k=0; k<NTBIN; k++) {
            if (e >= EnergyBin[k] && e < EnergyBin[k+1]) {
               NTcounts[(int) (st/12)][event->bid][k]++;
            }
         }
      }

      // fine -t bins
      int spbit = 2;

      if (gSpinPattern[event->bid] == 1) {
         spbit = 0;
      } else if (gSpinPattern[event->bid] == -1) {
         spbit = 1;
      }

      int   iebin = 11;
      float eRegion[11] = {380., 420., 460., 520., 580., 640., 700., 760., 840., 920., 1000.};

      for (int ebin=0; ebin<10; ebin++) {
         if ( e >= eRegion[ebin] && e < eRegion[ebin+1]) {
            iebin = ebin;   // if found
         }
      }

      //ds HHF1(35000+st+100*spbit, iebin, 1.);

      // Strip distribution (time cut, before Energy Cut)
      //ds HHF1(10310+si+1, (float) (st-si*12)+1, 1.);

      // Energy Cut
      if (e > Emin && e < Emax) {

         // t vs. E (banana with cut)
         t_vs_e_yescut[st]->Fill(e, t);

         // mass_with energy & mass sigma cut
         mass_yescut[st]->Fill(Mass);
         mass_yescut_all->Fill(Mass);

         // Timing info for Each bunch
         //if (e > 600. && e < 650.) {
         //   HHF1(11000+(int)event->bid, t, 1.);
         //}

         // delimiter distribution
         //ds HHF1(10200,(float)event->delim,1.);

         // Fill event in memory
         Nevcut++;
         Ngood[event->bid]++;

         // bunch distribution (time + -t cut)
         //ds HHF1(10010,(float)event->bid,1.);
         //ds if (fmod((float)event->bid,2)==0) {
         //ds    HHF1(10100+(si+1)*10,(float)event->bid,1.);
         //ds }

         // energy distribution after carbon cut
         //ds HHF1(10050, e, 1.);
	      //((TH1F*) gAsymRoot->fHists->o["hKinEnergyA_oo"])->Fill(e);

         //ds HHF1(10410+si+1, e, 1.);
         //ds HHF1(10420+si+1, e_int, 1.);   // Integral

         //printf("UUU: %d, %f\n", si, minus_t);
         energy_spectrum[si]->Fill(minus_t);
         energy_spectrum_all->Fill(minus_t);

         //ds if (st == 14) HHF1(10470+si+1,e*2.234e-5,1.); // -t

         // Strip distribution (time + -t cut )
         //ds HHF1(10320+si+1, (float) (st-si*12)+1, 1.);
         good_carbon_events_strip->Fill(st+1);

         // Mass vs. Energy plots
         mass_vs_e_ecut[st]->Fill(e, Mass);

         Ncounts[(int)(st/12)][event->bid]++;
         int time = 0;

         if (gRunInfo->Run == 5) {
            time = delim;
            ++cntr.good[delim];
            NDcounts[(int)(st/12)][event->bid][TgtIndex[delim]]++;
         } else {
            time = (int) (cntr.revolution/RHIC_REVOLUTION_FREQ);

            if (time < MAXDELIM) {
               ++cntr.good[TgtIndex[time]];
               NDcounts[(int)(st/12)][event->bid][TgtIndex[time]]++;
            } else if (!gAnaInfo->HasAlphaBit()) {
               cerr << "ERROR: time constructed from revolution # " << time
                    << "exeeds MAXDELIM=" << MAXDELIM << " defined" << endl;
               cerr << "Perhaps calibration data? Try running with -C option" << endl;
            }
         }

         // Following function call is for special text output routine of spin tune measurements
         // This routine is commented out by default. Activate this upon necessity.
         //              SpinTuneOutput(event->bid,si);

         //ds if ((int)(st/12)==1) HHF1(38010, TgtIndex[time], gSpinPattern[event->bid] ==  1 ? 1 : 0);
         //ds if ((int)(st/12)==1) HHF1(38020, TgtIndex[time], gSpinPattern[event->bid] == -1 ? 1 : 0);
         //ds if ((int)(st/12)==4) HHF1(38030, TgtIndex[time], gSpinPattern[event->bid] ==  1 ? 1 : 0);
         //ds if ((int)(st/12)==4) HHF1(38040, TgtIndex[time], gSpinPattern[event->bid] == -1 ? 1 : 0);

         //ds HHF1(38050, TgtIndex[time], 1);
         //ds HHF1(38060, time, 1);

         // counters
         cntr.reg.NStrip[spbit][st]++;

         //ds: printf("t, sp, st: %d, %d, %d, %d\n", TgtIndex[time], spbit, st);
         cntr_tgt.reg.NStrip[(int) TgtIndex[time] ][spbit][st]++;

         if (fabs(delt) < gRunConsts[st+1].M2T*feedback.RMS[st]*gAnaInfo->MassSigmaAlt/sqrt_e)
            cntr.alt.NStrip[spbit][st]++;

         if (phx.bunchpat[event->bid]) cntr.phx.NStrip[spbit][st]++;
         if (str.bunchpat[event->bid]) cntr.str.NStrip[spbit][st]++;

         // Ramp measurements binning
         // 20 Hz delimiters
         /*
         if (gAnaInfo->RAMPMODE==1) {
            int rbin = (int)((event->delim)/20.);
            //NRcounts[(int)(st/12)][event->bid][rbin]++;

            // Plus Spin 21000+Si
            // Minus Spin 21100+Si
            HHF1(21000+spbit*100+(int)(st/12), rbin, 1.);
         }
         */

         // Spin Tune
         //ds if (gAnaInfo->STUDYMODE == 1) {
         //ds    HHF1(40000+(int)(st/12), (float)event->bid/2. + (float)event->rev0 * 60., 1.);
         //ds }
      }
   }
}


// Description : calculate kinematics from ADC and TDC
// Input       : int Mode, processEvent *event, recordConfigRhicStruct *gConfigInfo, int st
// Return      : float &edepo, float &e, float &t, float &delt, float &Mass
void KinemaReconstruction(int Mode, processEvent *event, recordConfigRhicStruct *gConfigInfo,
                     int st, float &edepo, float &e, float &t, float &delt, float &Mass)
{
  float rand1, rand2;

  // random numbers in order to smear for the integer reading
  int vlen = 1;
  hhrammar_(&rand1, &vlen);
  hhrammar_(&rand2, &vlen);

  // Energy deposit
  edepo = gConfigInfo->data.chan[st].acoef * (event->amp+rand2-0.5);

  // ToF in [ns]
  t = gRunConsts[st+1].Ct * (event->tdc + rand1 - 0.5) - gConfigInfo->data.chan[st].t0 - gAnaInfo->tshift;

  // Kinetic energy assuming Carbon
  e = ekin(edepo, gConfigInfo->data.chan[st].dwidth);

  // Decrepancy between observed ToF and calculated t from energy
  delt = t - gRunConsts[st+1].E2T/sqrt(e);
}


// Description : output routine for Mei Bai's spin tune measurements
//             : The output should be stderr. Following two commands
//             : creat data file without unnecessary Warnings.
//             : 1) Asym -f 9961.002 -b 2> tmp.dat
//             : 2) grep -v "Warning" tmp.dat > SpinTune9961.002.dat
//             :
// Input       : int bid, double si
// Ouput       : revolusion#, bid, si
//             : output Bunch ID starts from 1, not zero
// Return      : 0
//
void SpinTuneOutput(int bid, double si)
{
   fprintf(stderr, "%10ld", cntr.revolution);
   fprintf(stderr,   "%5d", bid+1);
   fprintf(stderr,   "%5d", gSpinPattern[bid]);
   fprintf(stderr,   "%5d", int(si)+1);
 
   /*
   si==0 ? fprintf(stderr,"%5d",1) : fprintf(stderr,"%5d",0);
   si==1 ? fprintf(stderr,"%5d",1) : fprintf(stderr,"%5d",0);
   si==2 ? fprintf(stderr,"%5d",1) : fprintf(stderr,"%5d",0);
   si==3 ? fprintf(stderr,"%5d",1) : fprintf(stderr,"%5d",0);
   si==4 ? fprintf(stderr,"%5d",1) : fprintf(stderr,"%5d",0);
   si==5 ? fprintf(stderr,"%5d",1) : fprintf(stderr,"%5d",0);
   */
 
   fprintf(stderr, "\n");
}
