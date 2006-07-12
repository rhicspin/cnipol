#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>
#include "rhicpol.h"
#include "rpoldata.h"
#include "Asym.h"
#include "AsymROOT.h"

extern void HHF1(int, float, float);
extern void HHF2(int, float, float, float);
int KinemaReconstruction(int Mode, processEvent *event, recordConfigRhicStruct *cfginfo, 
			 int st, float &edepo, float &e, float &t, float &delt, float &Mass);


int init=0;
//float EnergyBin[NTBIN+1]={320,400,480,600,800,1000,1200}; // 6+1
float EnergyBin[NTBIN+1]={320,360,400,440,480,520,600,700,800,900,1000,1100,1200,1300,1400}; // 14+1
//float EnergyBin[NTBIN+1]={200,220,240,260,280,300,340,380,440,500,600,750,900,1100,1400,1700,2000,2300,2600,2900,3100,3300}; // 21+1
//float EnergyBin[NTBIN+1]={100,150,200,250,300,350,400,500,600,800,1100,1500,2000,2500,3000,3300,3600,4000,4250,4500,4750}; // 21+1
//float EnergyBin[NTBIN+1]={100,120,140,160,180,200,230,260,290,320,350,400,450,500,600,700,800,900,1000,1100,1200,1300,1400}; // 22+1
//float EnergyBin[NTBIN+1]={500,600,700,800,900,1000,1100,1200,1400,1600,1800,2000,2200,2400,2600,2800,3000,3300,3600,3900,4500,5000,6000,7000,8000,9000,10000}; // 26+1


// ==================== 
// Processing one event 
// ==================== 
int event_process(processEvent *event, recordConfigRhicStruct *cfginfo) {
    
    int i,j,st;     // st : c like 0-23
    int hid, ben, bid;
    int region;
    int delim;
    float edepo, rtof;
    int si;
    float pbeam;
    float t, e, e_int, Integ, Mass;
    float t0f, t0w, Expint, delt;
    float rand1, rand2;
    int Htgt, Vtgt;

    //----------------------------------------------------

    Nevtot++; 
    Ntotal[event->bid]++;
    delim = event->delim ;
    st = event->stN;       // 0 - 71
    si = (int)(st/12);     // 0 - 5
    
    float Emin = (float)dproc.enel;
    float Emax = (float)dproc.eneu;


    // write out t-dependence energy bin into log file (only once)
    if (!init) {
      fprintf(stdout,"EnergyBin:");
      for (int k=1; k<=NTBIN; k++) fprintf(stdout,"%6.0f",(EnergyBin[k]+EnergyBin[k-1])/2.);
      fprintf(stdout,"\nEnergydBin:");
      for (int k=1; k<=NTBIN; k++) fprintf(stdout,"%6.0f",(EnergyBin[k]-EnergyBin[k-1])/2.);
      fprintf(stdout,"\n");
      init=1;
    }


    //------------------------------------------------------------------//
    //                           Feedback Mode                          //
    //------------------------------------------------------------------//
    if (Flag.feedback){

      if ((event->tdc > 2*cfginfo->data.chan[st].Window.split.Beg)){
        
        // random numbers in order to smear for the integer reading
        int vlen = 1;
        hhrammar_(&rand1, &vlen);
        hhrammar_(&rand2, &vlen);

        float edepo = cfginfo->data.chan[st].acoef * (event->amp+rand2-0.5);
        e = ekin(edepo, cfginfo->data.chan[st].dwidth);
	//e = cfginfo->data.chan[st].edead + cfginfo->data.chan[st].ecoef * 
	//  (event->amp + rand2 - 0.5);

	t =  runconst.Ct * (event->tdc + rand1 - 0.5) - cfginfo->data.chan[st].t0 - dproc.tshift; 

	Mass = t*t*e*runconst.T2M * k2G;

	delt = t - runconst.E2T/sqrt(e);  

	if ((e>Emin && e< Emax)&&(Mass>dproc.MassLimit)&&(Mass<14.))  HHF1(16200+st+1, Mass, 1.);

      } // tdc>6ns

      return (0);
    }// end-of-Flag.feedback


    //------------------------------------------------------------------//
    //                          DeadLayer Mode                          //
    //------------------------------------------------------------------//
    if (dproc.DMODE) {

	// 2*cfginfo->data.chan[st].Window.split.Beg = 6
	if ((event->tdc > 2*cfginfo->data.chan[st].Window.split.Beg)){

	  KinemaReconstruction(1, event, cfginfo, st, edepo, e, t, delt, Mass);
	  HHF2(15000+st+1, edepo, t + cfginfo->data.chan[st].t0, 1.);

	  if (fabs(delt) < runconst.M2T*feedback.RMS[st]*dproc.MassSigma/sqrt(e)) {
	    
	    HHF2(15100+st+1, edepo, t + cfginfo->data.chan[st].t0, 1.);
	    if ((e>Emin) && (e<Emax)) Ngood[event->bid]++;
	    
	  } //end-of-if (abs(delt))
	  
	} // end-of-if (event->tdc)
	
	return (0);

    } // end-of-DMODE



    //------------------------------------------------------------------//
    //                           Nomal Process Mode                     //
    //------------------------------------------------------------------//
    // fill profile histograms at the 1st visit
    if (Nevtot==1) {
        
        // Wall Current Monitor Operation
      average.total   = 0;
      average.counter = 0;
        for (j=0;j<120;j++){
            HHF1(10030,(float)j, (float)wcmdist[j]);
	    if (( fabs(runinfo.WcmAve-wcmdist[j])/runinfo.WcmAve<dproc.WCMRANGE )&&(fillpat[j])) {
	      average.total+=wcmdist[j];
	      wcmfillpat[j]=1;
	      ++average.counter;
	    }
	}
	average.average = average.total/float(average.counter);



        // Online Banana Cut 
        for (int strip=0;strip<72;strip++){
            for (int j=0; j<HENEBIN; j++){
                float e = ((float)j+0.5)*(HMAXENE/float(HENEBIN));

                // Outside Of energy cuts
                if (e<Emin) {
                    HHF1(13500+strip+1, e, 0.);
                    HHF1(13600+strip+1, e, 0.);
                } else if (e>Emax){
                    HHF1(13500+strip+1, e, 0.);
                    HHF1(13600+strip+1, e, 0.);
                } else {
		  
                    if (dproc.CBANANA == 0){
                        HHF1(13500+strip+1, e, runconst.E2T/sqrt(e)
                             -cfginfo->data.chan[strip].ETCutW);
                        HHF1(13600+strip+1, e, runconst.E2T/sqrt(e)
                             +cfginfo->data.chan[strip].ETCutW);
                    } else if (dproc.CBANANA == 1) {
                        HHF1(13500+strip+1, e, runconst.E2T/sqrt(e)
                             +(float)(dproc.widthl));
                        HHF1(13600+strip+1, e, runconst.E2T/sqrt(e)
                             +(float)(dproc.widthu));
                    } else if (dproc.CBANANA == 2) {
                        HHF1(13500+strip+1, e, runconst.E2T/sqrt(e)
                             + runconst.M2T*feedback.RMS[strip]*dproc.MassSigma/sqrt(e)) ;
                        HHF1(13600+strip+1, e, runconst.E2T/sqrt(e)
                             - runconst.M2T*feedback.RMS[strip]*dproc.MassSigma/sqrt(e)) ;
                    }
                }
            }

        }
        cout << "finished first event initialization "<<endl;

    } // end-of-if(Nevent==1)
    

    // TDC Dists without Cut
    HHF1(11200+st+1, (float)event->tdc, 1.);

    // 2*cfginfo->data.chan[st].Window.split.Beg = 6
    if ((event->tdc > 2*cfginfo->data.chan[st].Window.split.Beg)){
        
        // random numbers in order to smear for the integer reading
        int vlen = 1;
	hhrammar_(&rand1, &vlen);
        hhrammar_(&rand2, &vlen);

        // Calibration hists
        if (dproc.CMODE == 1) {
            // reducing the background 
            if ((event->tdc>50)&&(event->tdc <200)&&(event->amp<215)) { 
                
                HHF1(12000+st+1, event->amp+rand2-0.5, 1.);
                
            }
        }
        
        // Strip distribution
        HHF1(10300+si+1,(float)(st-si*12)+1,1.);
        
        // bunch distribution
	HHF1(10000,(float)event->bid,1.);

        
        // ========================================
        //              Integral   
        // ========================================
        Integ = (event->intg) << (2+cfginfo->data.CSR.split.iDiv); 

        float amp_int = (Integ - cfginfo->data.chan[st].A0)/ 
            cfginfo->data.chan[st].A1;
        
        // ========================================
        //              Energy  
        // ========================================

        // Energy corrected with Dead layer info (keV)
        float e, e_int;

        // energy deposit on Si strip
        float edepo = cfginfo->data.chan[st].acoef * (event->amp+rand2-0.5);
        float edepo_int = cfginfo->data.chan[st].acoef * (amp_int+rand2-0.5);

        // === NEW float dwidth = cfginfo->data.chan[st].IACutW; // new entry 


        e = ekin(edepo, cfginfo->data.chan[st].dwidth);
        e_int = ekin(edepo_int, cfginfo->data.chan[st].dwidth);

        /*
        e = cfginfo->data.chan[st].edead + cfginfo->data.chan[st].ecoef * 
          (event->amp + rand2 - 0.5);
	e_int = cfginfo->data.chan[st].edead + 
                cfginfo->data.chan[st].ecoef * (amp_int + rand2 - 0.5);
        */

        // For A_N Calculation (Cross section)
        if ((e>Emin)&&(e<Emax)) {
            HHF1(10040, e, 1.);
        }
        
        HHF1(10400+si+1, e, 1.);
        if (spinpat[event->bid]>0) {
            HHF1(10500+st+1,e,1.);
        } else {
            HHF1(10600+st+1,e,1.);
        }
        

        // ========================================
        //              Time Of Flight
        // ========================================

        if (dproc.RAMPMODE == 1) {

            t =  runconst.Ct * (event->tdc + rand1 - 0.5) 
                - cfginfo->data.chan[st].t0 
                - (ramptshift[(int)event->delim/20]-ramptshift[0]) - dproc.tshift; 

        } else if (dproc.ZMODE == 0) {   // normal runs

            t =  runconst.Ct * (event->tdc + rand1 - 0.5) - cfginfo->data.chan[st].t0 
                - dproc.tshift - feedback.tedev[st]/sqrt(e); 

        } else  { 
            
            t =  runconst.Ct * (event->tdc + rand1 - 0.5) - dproc.tshift ; 

        }        
	

        // ========================================
        //              Invariant Mass
        // ========================================

	Mass = t*t*e*runconst.T2M * k2G;

        // Mass mode
        if (dproc.MMODE == 1) {
            if (e>Emin && e< Emax) {
	      HHF1(16000+st+1, Mass, 1.);
	      if ((Mass>dproc.MassLimit)&&(Mass<14.)) HHF1(17200+st+1, Mass, 1.);
	    }
	}
        
        // for T0 (cable length dependence)
        if ((dproc.TMODE == 1)&&(edepo!=0.)){
            HHF2(12100+st+1, (float)(1./sqrt(e)), t, 1.);
        }
        
        // Banana Plots (E-T)
        if (dproc.BMODE == 1){
            HHF2(13000+st+1, e, t, 1.);
            if (fabs(Mass-3.726)<2.5)  HHF2(13700+st+1, e, t, 1.);
            if (fabs(Mass-3.726)>2.5)  HHF2(13800+st+1, e, t, 1.);
            HHF2(13100+st+1, event->amp, event->tdc, 1.);
            HHF2(13200+st+1, event->amp, event->tdcmax, 1.);
            HHF1(14000+st+1, event->bid, 1.);
        }

	// Mass vs. ToF Scatter plot for Error Check
	mass_vs_t[st]->Fill(t,Mass);

	/*
        if (dproc.RAMPMODE==1) {0
            // total RAMPTIME sec
            // 1sec for each bin, delimiters are 20Hz rate 
            int rbin = (int)(event->delim/20);
            if (e>600 && e<650) {
                HHF1(20000+rbin,t,1.);
            }
        }
        */

        
        // integral vs. amplitede
        if (dproc.AMODE ==1){
            HHF2(12200+st+1,event->amp,Integ,1.);
            HHF2(12300+st+1,Integ,t,1.);
        }
        
        // -t slope
        
        if ((Mass<11.18+5.0)&&(Mass>11.18-5.0)){
            HHF1(10450+si+1, 2*11.18*e/1000000.,1.);
            HHF1(10460+si+1, 2*11.18*e_int/1000000.,1.);
        }
        
	t =  runconst.Ct * (event->tdc + rand1 - 0.5) - cfginfo->data.chan[st].t0 - dproc.tshift; 
        delt = t - runconst.E2T/sqrt(e);  
            
        // =========================================
        // Ntuple fill
        // =========================================
        if ((Nevtot<NTLIMIT)&&(dproc.NTMODE==1)&&(fmod(float(Nevtot),10)==0)) {
            atdata.ia = (long)event->amp;
            atdata.is = (long)event->intg;
            atdata.it = (long)event->tdc;
            atdata.ib = (long)event->bid;
            atdata.id = (long)event->delim;
            atdata.strip = (int)st+1;
            atdata.e = (float)e;
            atdata.tof = (float)t;
            
            atdata.spin = spinpat[event->bid];
            
            //HHFNT(1);
        }
            
        // background estimation
        if ((delt<=-2*cfginfo->data.chan[st].ETCutW)&&
            (e>Emin)&&(e<Emax)){
            Nback[event->bid]++;
        }
        
        // Bunch distribution (only -t cut)
        if ((e>Emin) && (e<Emax)) {
            HHF1(10020,(float)event->bid,1.);
        }

        //------------------------------------------------------
        //                Banana Curve Cut
        //------------------------------------------------------
        if ( ((delt> -1. * cfginfo->data.chan[st].ETCutW ) &&
              (delt<  1. * cfginfo->data.chan[st].ETCutW ) && 
              (dproc.CBANANA==0))
             ||
             ((delt > (float)(dproc.widthl) )&&
              (delt < (float)(dproc.widthu) )&&
              (dproc.CBANANA==1)) 
	     ||
	     ((fabs(delt) < runconst.M2T*feedback.RMS[st]*dproc.MassSigma/sqrt(e)) 
	      && (dproc.CBANANA==2))
	     ){

	    // -t dependence
            if (e>EnergyBin[0]) {
                for (int k=0; k<NTBIN ; k++) {
		  if ((e>=EnergyBin[k])&&(e<EnergyBin[k+1])){
		          NTcounts[(int)(st/12)][event->bid][k]++;
		  }
                }

            }


            // fine -t bins
            int spbit = 2;
            if (spinpat[event->bid]==1) {
                spbit = 0;
            } else if (spinpat[event->bid]==-1){
                spbit = 1;
            } 
            
            float eRegion[11] = {380., 420., 460., 520., 580., 640., 
                                   700., 760., 840., 920., 1000.};
            int iebin = 11;
            for (int ebin =0; ebin<10; ebin++) {
                if ( e >= eRegion[ebin] && e< eRegion[ebin+1]) {
                    iebin = ebin;   // if found 
                }
            }
            HHF1(35000+st+100*spbit, iebin, 1.);
            


            // Strip distribution (time cut, before Energy Cut)
            HHF1(10310+si+1,(float)(st-si*12)+1,1.);

            //  ====================================================
            //  =                Energy Cut                        = 
            //  ====================================================
            
            if ((e>Emin) && (e<Emax)) {
                
        
	      // Timing info for Each bunch 
	      if ((e>600.)&&(e<650.)){
		HHF1(11000+(int)event->bid, t, 1.);
	      }

                // delimiter distribution
                HHF1(10200,(float)event->delim,1.);
                
                // Fill event in memory
                Nevcut++;
                Ngood[event->bid]++;
                
                // bunch distribution (time + -t cut)
                HHF1(10010,(float)event->bid,1.);
                if (fmod((float)event->bid,2)==0) {
                    HHF1(10100+(si+1)*10,(float)event->bid,1.);
                }
                
                // energy distribution after carbon cut 
		HHF1(10050, e, 1.);
                HHF1(10410+si+1,e,1.);
                HHF1(10420+si+1,e_int,1.);   // Integral
                if (st == 14) HHF1(10470+si+1,e*2.234e-5,1.); // -t 
                
                // Strip distribution (time + -t cut )
                HHF1(10320+si+1,(float)(st-si*12)+1,1.);

		// Mass vs. t and vs. Energy plots
		mass_vs_e_ecut[st] -> Fill(e, Mass);
		mass_vs_t_ecut[st] -> Fill(t, Mass);


                Ncounts[(int)(st/12)][event->bid]++;
                int time=0;
		if (runinfo.Run==5){
                    time = delim;
                    NDcounts[(int)(st/12)][event->bid][TgtIndex[delim]]++;
                } else { 
                    time = (int)cntr.revolution/RHIC_REVOLUTION_FREQ;
                    if (time<MAXDELIM) {
                        NDcounts[(int)(st/12)][event->bid][TgtIndex[time]]++;
                    }else{
		      cerr << "ERROR: time constructed from revolution # " << time << "exeeds MAXDELIM=" << MAXDELIM << " defined\n" << endl;
                    }
                }
		if ((int)(st/12)==1) HHF1(38010, TgtIndex[time], spinpat[event->bid]==1?1:0);
		if ((int)(st/12)==1) HHF1(38020, TgtIndex[time], spinpat[event->bid]==-1?1:0);
		if ((int)(st/12)==4) HHF1(38030, TgtIndex[time], spinpat[event->bid]==1?1:0);
		if ((int)(st/12)==4) HHF1(38040, TgtIndex[time], spinpat[event->bid]==-1?1:0);
		HHF1(38050, TgtIndex[time], 1);
		HHF1(38060, time, 1);

		// Disable some detectors
		if (!DisabledDet(st/12)) NStrip[spbit][st]++;

                //lcounts[(int)event->bid][(int)(st/12)][event->gCC]++;
                
                // Ramp measurements binning
                // 20 Hz delimiters
                if (dproc.RAMPMODE==1) {
                    int rbin = (int)((event->delim)/20.);
                    //NRcounts[(int)(st/12)][event->bid][rbin]++;
                    
                    // Plus Spin 21000+Si
                    // Minus Spin 21100+Si
                    HHF1(21000+spbit*100+(int)(st/12), rbin, 1.);
                }

                // Study mode for multiple purpise
                
                // Spin Tune
                if (dproc.STUDYMODE==1) {
                    HHF1(40000+(int)(st/12), 
                         (float)event->bid/2. + (float)event->rev0 * 60., 1.);
                }
                

            } // Emin<e<Emax Cut

        } // Banana Cut

    } // tdc>6ns




    // Target Histogram
    if ((st>=72)&&(st<=75)) {
        HHF2(25060, cntr.revolution/RHIC_REVOLUTION_FREQ, tgt.x, 1);
    }




    return(0);
}

//
// Class name  : 
// Method name : KinemaReconstruction(
//
// Description : calculate kinematics from ADC and TDC
// Input       : int Mode, processEvent *event, recordConfigRhicStruct *cfginfo, int st
// Return      : float &edepo, float &e, float &t, float &delt, float &Mass
//
int
KinemaReconstruction(int Mode, processEvent *event, recordConfigRhicStruct *cfginfo, 
		     int st, float &edepo, float &e, float &t, float &delt, float &Mass){

  float rand1, rand2;

  // random numbers in order to smear for the integer reading
  int vlen = 1;
  hhrammar_(&rand1, &vlen);
  hhrammar_(&rand2, &vlen);

  // Energy Deosit
  edepo = cfginfo->data.chan[st].acoef * (event->amp+rand2-0.5);

  // ToF in [ns]
  t =  runconst.Ct * (event->tdc + rand1 - 0.5) - cfginfo->data.chan[st].t0 - dproc.tshift; 

  // Kinetic energy assuming Carbon 
  e = ekin(edepo, cfginfo->data.chan[st].dwidth);

  // Decrepancy between observed ToF and calculated t from energy
  delt = t - runconst.E2T/sqrt(e);  

  return 1;

}



// ==============================
// Dead Layer Correction
// ==============================
// Ekin = p0 + p1*Edep + p2*Edep^2 + p3*Edep^3 + p4*Edep^4
float ekin(float edep, float dthick){

    const float cp0[4] = {-0.5174,0.4172,0.3610E-02,-0.1286E-05};
    const float cp1[4] = {1.0000,0.8703E-02,0.1252E-04,0.6948E-07};
    const float cp2[4] = {0.2990E-05,-0.7937E-05,-0.2219E-07,-0.2877E-09};
    const float cp3[4] = {-0.8258E-08,0.4031E-08,0.9673E-12,0.3661E-12};
    const float cp4[4] = {0.3652E-11,-0.8652E-12,0.4059E-14,-0.1294E-15};
    
    float d2 = dthick * dthick;
    float d3 = d2 * dthick;

    float pp[5];
    
    pp[0] = cp0[0] + cp0[1]*dthick + cp0[2]*d2 + cp0[3]*d3;
    pp[1] = cp1[0] + cp1[1]*dthick + cp1[2]*d2 + cp1[3]*d3;
    pp[2] = cp2[0] + cp2[1]*dthick + cp2[2]*d2 + cp2[3]*d3;
    pp[3] = cp3[0] + cp3[1]*dthick + cp3[2]*d2 + cp3[3]*d3;
    pp[4] = cp4[0] + cp4[1]*dthick + cp4[2]*d2 + cp4[3]*d3;
    
    float edep2 = edep * edep;
    float edep3 = edep2 * edep;
    float edep4 = edep3 * edep;
    

    return(pp[0] + pp[1]*edep + pp[2]*edep2 + pp[3]*edep3 + pp[4]*edep4);
    
};



