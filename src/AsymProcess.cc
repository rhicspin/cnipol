#include "AsymProcess.h"

#include "TH1F.h"

#include "AnaInfo.h"
#include "AsymRoot.h"
#include "Kinema.h"
#include "MeasInfo.h"

using namespace std;

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
 */
void event_process(ChannelEvent &chEvent)
{
   //int   st    = chEvent.GetChannelId() - 1;        // 0 - 71
   //int   si    = (int) (st/12);     // 0 - 5
   //float Emin  = (float) gAsymAnaInfo->enel;
   //float Emax  = (float) gAsymAnaInfo->eneu;

   Ntotal[chEvent.GetBunchId()-1]++;

   // write out t-dependence energy bin into log file (only once)
   if (!init) {
      fprintf(stdout, "EnergyBin:");
      for (int k=1; k<=NTBIN; k++) fprintf(stdout, "%6.0f", (EnergyBin[k]+EnergyBin[k-1])/2.);

      fprintf(stdout, "\nEnergydBin:");
      for (int k=1; k<=NTBIN; k++) fprintf(stdout, "%6.0f", (EnergyBin[k]-EnergyBin[k-1])/2.);

      fprintf(stdout,"\n");

      init = 1;
   }

   /*
   // tdc > 6 ns, effectively...
   if ((chEvent.tdc > 2*gConfigInfo->data.chan[st].Window.split.Beg)) {

      // random numbers in order to smear for the integer reading
      int vlen = 1;
      float rand1, rand2;

      hhrammar_(&rand1, &vlen);
      hhrammar_(&rand2, &vlen);

      float edepo = gConfigInfo->data.chan[st].acoef * (chEvent.amp+rand2-0.5);
      float e = ekin(edepo, gConfigInfo->data.chan[st].dwidth);
      //e = gConfigInfo->data.chan[st].edead + gConfigInfo->data.chan[st].ecoef *
      //  (chEvent.amp + rand2 - 0.5);

      float t = gRunConsts[st+1].Ct * (chEvent.tdc + rand1 - 0.5) - gConfigInfo->data.chan[st].t0 - gAsymAnaInfo->tshift;

      float Mass = t*t*e* gRunConsts[st+1].T2M * k2G;

      //float delt = t - gRunConsts[st+1].E2T/sqrt(e);

      if (e > Emin && e < Emax && Mass > gAsymAnaInfo->MassLimit && Mass < 20.) {
         mass_feedback[st]->Fill(Mass);
         mass_feedback_all->Fill(Mass);
      }
   }
   */

   // Nomal Process Mode
   // fill profile histograms at the 1st visit
   /*
   if (Nevtot==1) {

      // Wall Current Monitor Operation
      average.total   = 0;
      average.counter = 0;

      for (int j=0; j<NBUNCH; j++) {
         wall_current_monitor->Fill(j, gMeasInfo->fWallCurMon[j+1]);
         HHF1(10030, (float) j, (float) gMeasInfo->fWallCurMon[j+1]);

         if ( (fabs(gMeasInfo->fWallCurMonAve-gMeasInfo->fWallCurMon[j+1])/gMeasInfo->fWallCurMonAve < gAsymAnaInfo->WCMRANGE) && gFillPattern[j]) {
            average.total+=gMeasInfo->fWallCurMon[j+1];
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

            }
         }
      }

      cout << "Finished first event initialization" << endl;
   }
   */

   // Main Process Routine event by event starts here

   // TDC Dists without Cut
   //HHF1(11200+st+1, (float)chEvent.tdc, 1.);

   // Target Histogram
   //if (st >= 72 && st <= 75) {
   //   HHF2(25060, cntr.revolution/RHIC_REVOLUTION_FREQ, tgt.x, 1);
   //}

   // Fill target histograms
   if (gAsymAnaInfo->HasTargetBit()) {

      //if (chEvent.GetChannelId() > NSTRIP)
      //   printf("channel1: %d, %d\n", chEvent.GetChannelId(), gConfigInfo->data.NumChannels);

      if (chEvent.PassCutTargetChannel()) {
         //printf("channel: %d\n", chEvent.GetChannelId());
         gAsymRoot->fHists->d["targets"]->Fill(&chEvent);
      }
   }

   if (gAsymAnaInfo->HasNormalBit())
   {
      if (!chEvent.PassCutSiliconChannel()) return;

      //if (fabs(gFillPattern[chEvent.bid]) != 1)

      //printf("channel %d, %d, %d, %d\n", chEvent.GetChannelId(), chEvent.PassCutNoise(), chEvent.PassCutKinEnergyAEDepend(), chEvent.PassCutEnabledChannel());

      //if (chEvent.PassCutPulser() && chEvent.PassCutNoise() && chEvent.PassCutKinEnergyADLCorrEstimate())
      //if (chEvent.PassCutNoise() && chEvent.PassCutKinEnergyADLCorrEstimate())
      //if (chEvent.PassCutNoise() && chEvent.PassCutKinEnergyAEDepend() && chEvent.PassCutEnabledChannel())
      //if ( !chEvent.PassCutNoise() || !chEvent.PassCutKinEnergyAEDepend() || !chEvent.PassCutEnabledChannel() ) return;
      if ( !chEvent.PassCutKinEnergyAEDepend() || !chEvent.PassCutEnabledChannel() ) return;

		gAsymRoot->Fill(kCUT_NOISE);

      if (!chEvent.PassCutCarbonMass()) return;

		gAsymRoot->Fill(kCUT_CARBON);

      //if ( chEvent.PassCutEmptyBunch() ) {
		//	gAsymRoot->Fill(kCUT_CARBON_EB);
      //} else {
		//   gAsymRoot->Fill(kCUT_CARBON);
      //}

      //((CnipolRunHists*) gAsymRoot->fHists)->Fill(chEvent);
      //((TH1*) gAsymRoot->fHists->o["hTargetSteps"])->Fill(chEvent->GetDelimiterId());

      //if ( chEvent.PassCutNoise() && chEvent.PassCutKinEnergyAEDependAverage() &&
      //     chEvent.PassCutCarbonMassEstimate())
      //{
	   //   gAsymRoot->fHists->Fill(chEvent, "_cut2");
      //}
   }
}


/*
   // 2*gConfigInfo->data.chan[st].Window.split.Beg = 6
   //if ((chEvent.tdc > 2*gConfigInfo->data.chan[st].Window.split.Beg))
   //ds: Proceed only if the time value is greater than...
   if ((chEvent.tdc <= 2*gConfigInfo->data.chan[st].Window.split.Beg))
      return;

   // random numbers in order to smear for the integer reading
   int   vlen = 1;
   float rand1, rand2;
   //hhrammar_(&rand1, &vlen);
   //hhrammar_(&rand2, &vlen);

   // Strip distribution
   //HHF1(10300+si+1, (float)(st-si*12)+1, 1.);

   // bunch distribution
   //HHF1(10000, (float)chEvent.bid, 1.);
   //gAsymRoot->bunch_dist->Fill(chEvent.bid);

   // Integral
   float Integ = (chEvent.intg) << (2+gConfigInfo->data.CSR.split.iDiv);

   //ds: Some manipulations with ADC counts...
   float amp_int = (Integ - gConfigInfo->data.chan[st].A0) / gConfigInfo->data.chan[st].A1;
    
   // Energy corrected with Dead layer info (keV)

   // energy deposit in Si strip
   //ds: Are we using the same acoef??? for amp and int?
   float edepo = gConfigInfo->data.chan[st].acoef * (chEvent.amp+rand2-0.5);
   //ds: edepo_int = deposited energy in keV?
   //float edepo_int = gConfigInfo->data.chan[st].acoef * (amp_int+rand2-0.5);
   float edepo_int = gConfigInfo->data.chan[st].ecoef * (amp_int+rand2-0.5);

   // === NEW float dwidth = gConfigInfo->data.chan[st].IACutW; // new entry
   float e      = ekin(edepo, gConfigInfo->data.chan[st].dwidth);
   float e_int  = ekin(edepo_int, gConfigInfo->data.chan[st].dwidth);
   float sqrt_e = sqrt(e);

   //e = gConfigInfo->data.chan[st].edead + gConfigInfo->data.chan[st].ecoef *
   //  (chEvent.amp + rand2 - 0.5);
   //e_int = gConfigInfo->data.chan[st].edead +
   //        gConfigInfo->data.chan[st].ecoef * (amp_int + rand2 - 0.5);

   // For A_N Calculation (Cross section)
   //if (e > Emin && e < Emax) {
   //    HHF1(10040, e, 1.);
   //}

   //HHF1(10400+si+1, e, 1.);

   //if (gSpinPattern[chEvent.bid] > 0) {
   //    HHF1(10500+st+1, e, 1.);
   //} else {
   //    HHF1(10600+st+1, e, 1.);
   //}

   // Time Of Flight
   float t;

   //ds
   //printf("%10.3f, %10d, %10d, %10.3f, %10.3f, %10.3f\n", chEvent.GetTime(), chEvent.fChannel.fTdc, chEvent.tdc, t, gAsymAnaInfo->tshift, gRunConsts[st+1].Ct);

   float delt = t - gRunConsts[st+1].E2T/sqrt_e;

   // Invariant Mass
   float Mass = t * t * e * gRunConsts[st+1].T2M * k2G;

   mass_nocut[st]->Fill(Mass);
   mass_nocut_all->Fill(Mass);

   // Mass mode
   //if (gAsymAnaInfo->MMODE == 1) {
   //   if (e > Emin && e < Emax) {

   //      HHF1(16000+st+1, Mass, 1.);

   //      if (Mass > gAsymAnaInfo->MassLimit && Mass < 14.)
   //         HHF1(17200+st+1, Mass, 1.);
   //   }
   //}

   //// Banana Plots (E-T)
   //if (gAsymAnaInfo->BMODE == 1) {

   //   HHF2(13000+st+1, e, t, 1.);

   //   if (fabs(Mass - 3.726) < 2.5)  HHF2(13700+st+1, e, t, 1.);
   //   if (fabs(Mass - 3.726) > 2.5)  HHF2(13800+st+1, e, t, 1.);

   //   HHF2(13100+st+1, chEvent.amp, chEvent.tdc, 1.);
   //   HHF2(13200+st+1, chEvent.amp, chEvent.tdcmax, 1.);
   //   HHF1(14000+st+1, chEvent.bid, 1.);
   //}

   //// integral vs. amplitede
   //if (gAsymAnaInfo->AMODE == 1) {
   //   HHF2(12200+st+1, chEvent.amp, Integ, 1.);
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
   //if ((Nevtot<NTLIMIT)&&(gAsymAnaInfo->NTMODE==1)&&(fmod(float(Nevtot),10)==0)) {
   //    atdata.ia = (long)chEvent.amp;
   //    atdata.is = (long)chEvent.intg;
   //    atdata.it = (long)chEvent.tdc;
   //    atdata.ib = (long)chEvent.bid;
   //    atdata.id = (long)chEvent.GetDelimiterId();
   //    atdata.strip = (int)st+1;
   //    atdata.e = (float)e;
   //    atdata.tof = (float)t;
   //    atdata.spin = gSpinPattern[chEvent.bid];
   //    //HHFNT(1);
   //}

   // background estimation
   //if ( (delt <= -2*gConfigInfo->data.chan[st].ETCutW) && e>Emin && e<Emax)
   //{
   //   Nback[chEvent.bid]++;
   //}

   // Bunch distribution (only -t cut)
   //if (e > Emin && e < Emax) {
   //    HHF1(10020,(float)chEvent.bid,1.);
   //}

   if (chEvent.PassCutCarbonMass())
   {
      // -t dependence
      float minus_t = 2 * e * MASS_12C * k2G * k2G;
      //printf("UUU0: %d, %f\n", si, minus_t);

      if (e > EnergyBin[0]) {
         for (int k=0; k<NTBIN; k++) {
            if (e >= EnergyBin[k] && e < EnergyBin[k+1]) {
               NTcounts[(int) (st/12)][chEvent.bid][k]++;
            }
         }
      }

      // fine -t bins
      int spbit = 2;

      if (gMeasInfo->GetBunchSpin(chEvent.bid + 1) == 1) {
         spbit = 0;
      } else if (gMeasInfo->GetBunchSpin(chEvent.bid + 1) == -1) {
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
         //   HHF1(11000+(int)chEvent.bid, t, 1.);
         //}

         // delimiter distribution
         //ds HHF1(10200,(float)chEvent.GetDelimiterId(),1.);

         // Fill chEvent in memory
         Ngood[chEvent.bid]++;

         // bunch distribution (time + -t cut)
         //ds HHF1(10010,(float)chEvent.bid,1.);
         //ds if (fmod((float)chEvent.bid,2)==0) {
         //ds    HHF1(10100+(si+1)*10,(float)chEvent.bid,1.);
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

         Ncounts[(int)(st/12)][chEvent.bid]++;
         int time = 0;

         if (gMeasInfo->fRunId == 5) {
            time = chEvent.GetDelimiterId();
            ++cntr.good[chEvent.GetDelimiterId()];
            NDcounts[(int)(st/12)][chEvent.bid][TgtIndex[chEvent.GetDelimiterId()]]++;
         } else {
            time = (int) (cntr.revolution/RHIC_REVOLUTION_FREQ);

            if (time < MAXDELIM) {
               ++cntr.good[TgtIndex[time]];
               NDcounts[(int)(st/12)][chEvent.bid][TgtIndex[time]]++;
            } else if (!gAsymAnaInfo->HasAlphaBit()) {
               cerr << "ERROR: time constructed from revolution # " << time
                    << "exeeds MAXDELIM=" << MAXDELIM << " defined" << endl;
               cerr << "Perhaps calibration data? Try running with -C option" << endl;
            }
         }

         // Following function call is for special text output routine of spin tune measurements
         // This routine is commented out by default. Activate this upon necessity.
         //              SpinTuneOutput(chEvent.bid,si);

         //ds if ((int)(st/12)==1) HHF1(38010, TgtIndex[time], gSpinPattern[chEvent.bid] ==  1 ? 1 : 0);
         //ds if ((int)(st/12)==1) HHF1(38020, TgtIndex[time], gSpinPattern[chEvent.bid] == -1 ? 1 : 0);
         //ds if ((int)(st/12)==4) HHF1(38030, TgtIndex[time], gSpinPattern[chEvent.bid] ==  1 ? 1 : 0);
         //ds if ((int)(st/12)==4) HHF1(38040, TgtIndex[time], gSpinPattern[chEvent.bid] == -1 ? 1 : 0);

         //ds HHF1(38050, TgtIndex[time], 1);
         //ds HHF1(38060, time, 1);

         // counters
         cntr.reg.NStrip[spbit][st]++;

         //ds: printf("t, sp, st: %d, %d, %d, %d\n", TgtIndex[time], spbit, st);
         cntr_tgt.reg.NStrip[(int) TgtIndex[time] ][spbit][st]++;

         if (fabs(delt) < gRunConsts[st+1].M2T*feedback.RMS[st]*gAsymAnaInfo->MassSigmaAlt/sqrt_e)
            cntr.alt.NStrip[spbit][st]++;

         if (phx.bunchpat[chEvent.bid]) cntr.phx.NStrip[spbit][st]++;
         if (str.bunchpat[chEvent.bid]) cntr.str.NStrip[spbit][st]++;
      }
   }
*/


// Description : calculate kinematics from ADC and TDC
// Input       : int Mode, processEvent *event, recordConfigRhicStruct *gConfigInfo, int st
// Return      : float &edepo, float &e, float &t, float &delt, float &Mass
void KinemaReconstruction(int Mode, processEvent *event, recordConfigRhicStruct *gConfigInfo,
                     int st, float &edepo, float &e, float &t, float &delt, float &Mass)
{
  float rand1 = (float)rand()/(float)RAND_MAX;
  float rand2 = (float)rand()/(float)RAND_MAX;

  // random numbers in order to smear for the integer reading
  //int vlen = 1;
  //hhrammar_(&rand1, &vlen);
  //hhrammar_(&rand2, &vlen);

  // Energy deposit
  edepo = gConfigInfo->data.chan[st].acoef * (event->amp+rand2-0.5);

  // ToF in [ns]
  t = gRunConsts[st+1].Ct * (event->tdc + rand1 - 0.5) - gConfigInfo->data.chan[st].t0 - gAsymAnaInfo->tshift;

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
   fprintf(stderr,   "%5d", gMeasInfo->GetBunchSpin(bid + 1) );
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
