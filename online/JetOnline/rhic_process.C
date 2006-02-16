// Event mode data analysis tool
//  file name :   rhic_process.C
// 
//  Author    :   Osamu Jinnouchi
//  Creation  :   12/9/2003
//  Comment   :   optimized for the detailed analysis for the event mode data
//                

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <iostream.h>

#include "TROOT.h"
#include "TFile.h"
#include "TRandom.h"
#include "TTree.h"
#include "TBranch.h"
#include "TClonesArray.h"
#include "TStopwatch.h"
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>

#include "rhicpol.h"
#include "rpoldata.h"
#include "rhic_offline.h"
#include "rhic_class.h"

// ======================================================== 
// Processing one event 
// ======================================================== 
int TAsym::EventProcess(processEvent *event, recordConfigRhicStruct *cfginfo) {
    int bunchx = event->bid;

    Nevtot++; 
    Ntotal[bunchx]++;

    int st = event->stN;       // st=0...71  
    int si = (int)(st/16);   // si=0...5
    
    float Emin = (float)dproc.enel;
    float Emax = (float)dproc.eneu;
    
    
    // Fill the profile Histograms
    if (Nevtot==1) {  // Only at the first event
        
        cout << "The  first event of Event data  " << endl;
        
        // WCM distribution
        for (int j=0;j<120;j++){
            hwcm -> Fill((float)j, (float)wcmdist[j]);
        }
/* 
        // Banana Cut
        if (dproc.BMODE==1) {
            for (int strip=0;strip<72;strip++){
                for (int bin=0; bin<80; bin++){
                    float e = ((float)bin+0.5)*(1200/60.);
                    
                    float udev, ldev;
                    if (dproc.CBANANA == 0){
                        udev = cfginfo->data.chan[strip].ETCutW+tshift;
                        ldev = cfginfo->data.chan[strip].ETCutW+tshift;
                    } else {
                        udev = (float)(dproc.widthu)+tshift;
                        ldev = (float)(dproc.widthl)+tshift;
                    }
                    
                    hlowedg[strip] -> Fill(e, KINC/sqrt(e) - ldev);
                    huppedg[strip] -> Fill(e, KINC/sqrt(e) + udev);
                    
                    if (e<Emin) {
                        hlowedg[strip] -> Fill(e, 200.);
                        huppedg[strip] -> Fill(e, 0.);
                    } else if (e>Emax){
                        hlowedg[strip] -> Fill(e, 200.);
                        huppedg[strip] -> Fill(e, 0.);
                    } 
                }
            }
        }
*/
    }
    
/*
     // Calibration hists
       if (dproc.CMODE == 1) {
            // reducing the background 
            if ((event->tdc>20)&&(event->amp<215)) { 
            //printf(" TEST rhic_process CALIBRATION st=%d\n",st);
                hcalib[st] -> Fill(event->amp0);
            }
        }
        
        // Strip Distribution (Total)
        hstall[si] -> Fill((float)(st+1-si*12));
                
        // Random numbers in order to smear for the integer reading

        float rand1 = gRandom->Rndm();
        float rand2 = gRandom->Rndm();
        
        // =================================
        //  Time Of Flight
        // =================================

        // Determine the TDC count unit (ns/channel)
        float Ct = cfginfo->data.WFDTUnit/2.;
        
        float t;
        if (dproc.ZMODE == 0) {
            t =  Ct * (event->tdc + rand1 - 0.5) 
                - kint0[st]; 
        } else {
            t =  Ct * (event->tdc + rand1 - 0.5); 
        }        
        
        // =================================
        //  Integral
        // =================================

        // Integral vs. Amplitude 
        float Integ = (event->intg) << (2+cfginfo->data.CSR.split.iDiv); 

        //        cout << cfginfo->data.CSR.split.iDiv << endl;
        float Expint = cfginfo->data.chan[st].A0 + 
            cfginfo->data.chan[st].A1 * event->amp;
        
        float amp_int = (Integ - iapar0[st])/iapar1[st];

        // =================================
        //  Energy
        // =================================

        // Deposit Energy
        
        float edepo = cfginfo->data.chan[st].acoef * (event->amp+rand2-0.5);
        float edepo_int = cfginfo->data.chan[st].acoef * (amp_int+rand2-0.5);

        // Kintic Energy
        
        float e, e_int;
        if (dthick[si]!=0.0) {

            // Non-linear dead layer correction

            e = ekin(edepo, dthick[si]);
            e_int = ekin(edepo_int, dthick[si]);

        } else {

            // linear approximation

            e = cfginfo->data.chan[st].edead + 
                cfginfo->data.chan[st].ecoef * (event->amp + rand2 - 0.5);
            e_int = cfginfo->data.chan[st].edead + 
                cfginfo->data.chan[st].ecoef * (amp_int + rand2 - 0.5);
        }

        // Energy Spectrum after Carbon Cut 
        heneall[si]->Fill(e);
        heneallitg[si]->Fill(e_int);
        
        // Energy Spectrum for eacf strip (spin sorted)
        if (spinpat[bunchx]>0) {
            henestpl[st] -> Fill(e);
        } else {
            henestng[st] -> Fill(e);
        }
        
        
        // =================================
        //  Invariant Mass
        // =================================

        // Mass of Carbon
        // c = 3.0x10^8 m/s

        float Mshift;
        if ((bunchx/2)<55) {
            Mshift = mrelcent[st][bunchx/2];
        } else {
            Mshift = 0.;
        }

        // ====================================
        // fluctuation in cut
        // ====================================

        float Mass_int = 8.*0.000001*t*t*e;
        float Mass = 8.*0.000001*t*t*e_int;
        
        //Mass -= Mshift;
        //Mass_int -= Mshift;

        Mass -= Mshift * sqrt(Mass/11.17);
        Mass_int -= Mshift * sqrt(Mass_int/11.17);

        //e -= 700.*Mshift/11.17;    // use 700keV for average
         
        // MASS mode (Before Cut)
        if (dproc.MMODE == 1) {
            if (e>600. && e<1000.) {
                hmassall[st] -> Fill(Mass);
            } 
            
            hmassenea[st] -> Fill(Mass, e);
            hmassenei[st] -> Fill(Mass_int, e_int);
        }
        
        // Cross section (for A_N)
        if ((e>Emin)&&(e<Emax)) {
            henetotal -> Fill(e);
        }

        // Timing shift study 
        if ((e>600.) && (e<650.)){
            htshift[(int)bunchx/2] -> Fill(t);
        }
        
        // for T0 (cable length dependence)
        if ((dproc.TMODE == 1)&&(e != 0.)){
            ht0[st] -> Fill((float)(1./sqrt(e)),t);
        }

        // Banana Plots No Cut
        if (dproc.BMODE == 1){
            htofene[st] -> Fill(e,t);
            if (spinpat[bunchx]==1) {
                htdcadcpos[st] -> Fill(event->amp, event->tdc);
                htdcintpos[st] -> Fill(event->intg, event->tdc);
            } else if (spinpat[bunchx]==-1) {
                htdcadcneg[st] -> Fill(event->amp, event->tdc);
                htdcintneg[st] -> Fill(event->intg, event->tdc);
            }
        }

        int ebin = (int)((e-550.)/50.);
        
        if (e <= 550.) ebin = 0;
        if (e >= 1050.) ebin = 9;
        
        Float_t Mcenter, Mwidth;
        
        Mcenter = mcenter[st][ebin];
        Mwidth =  msigma[st][ebin];
        
        // Bunch 
        if ((dproc.BUNCHMODE == 1)&&(e<Emax)&&(e>Emin)) {
            

            // bunch by bunch mass plot
            if ((bunchx/2)<55) {
                
                hmass_bunch[st][bunchx/2] -> Fill(Mass);

            }


            hbunch_noc[st] -> Fill(bunchx);  // no cut

            if (Mass > Mcenter-3.*Mwidth && 
                Mass < Mcenter-2.*Mwidth) {
                
                hbunch_m30s[st] -> Fill(bunchx);  // -3 -> -2 sigma
            }
            
            if (Mass > Mcenter-2.*Mwidth && 
                Mass < Mcenter-1.*Mwidth) {
                
                hbunch_m20s[st] -> Fill(bunchx);  // -2 -> -1 sigma
            }

            if (Mass > Mcenter-1.*Mwidth && 
                Mass < Mcenter+1.*Mwidth) {

                hbunch_10s[st] -> Fill(bunchx);  // +- 1sigma
            }
            
            if (Mass > Mcenter+1.*Mwidth && 
                Mass < Mcenter+2.*Mwidth) {

                hbunch_p20s[st] -> Fill(bunchx);  // +1 +2 sigma
            }
            
            if (Mass > Mcenter+2.*Mwidth && 
                Mass < Mcenter+3.*Mwidth) {

                hbunch_p30s[st] -> Fill(bunchx);  // +1 +2 sigma
            }
            
        }

        if (dproc.RAMPMODE==1) {
            // total RAMPTIME sec
            // 10sec for each bin, delimiters are 20Hz rate 
            int rbin = (int)(event->delim/200);
            hramp[rbin] -> Fill(e,t);
        }

        // Dead Layer estimation
        if (dproc.DMODE == 1){
            htofdep[st] -> Fill(edepo, t);
        }

        // Integral vs. Amplitede
        if (dproc.AMODE ==1){
            hia[st] -> Fill(event->amp, Integ);
            hti[st] -> Fill(Integ, t);
        }
*/        
        
        // =========================================
        // Tree fill
        // =========================================
        if (dproc.NTMODE==1) {
            atdata.amp = (int)event->amp;
            atdata.itg = (int)event->intg;
            atdata.tdc = (int)event->tdc;
            atdata.tmax = (int)event->tmax;
            atdata.bid = (int)event->bid;
            atdata.dlm = (int)event->delim;
            atdata.stp = (int)st+1;
            atdata.ene = (float)0;
            atdata.tof = (float)0;
            atdata.spn = (int)spinpat[bunchx];
            atdata.rev = (int)event->rev;
            atdata.rev0 = (int)event->rev0;
            atdata.Mx = event->Mx;
            atdata.Mp = event->Mp;
            atdata.geo = event->geo;
            
            Event -> Fill();
        }
/*        
        // Deviation from Kinetic Curve
        float delt = t - KINC/sqrt(e);  
        
        // background estimation
        if ((delt<=-2*cfginfo->data.chan[st].ETCutW+tshift)&&
            (e>Emin)&&(e<Emax)){
            Nback[bunchx]++;
        }
        
        
        // TDC vs. ADC  (LUT boundaris)

        int ltva, utva;   
        ltva = cfginfo->data.chan[st].LookUp[event->amp] & 0xFF;
        utva = (cfginfo->data.chan[st].LookUp[event->amp]>>8) & 0xFF;
        
        // Integral vs. ADC (LUT boundaries)

        int liva, uiva; 
        liva = cfginfo->data.chan[st].LookUp[event->amp+256] & 0xFF;
        uiva = (cfginfo->data.chan[st].LookUp[event->amp+256]>>8) & 0xFF;

        // ========================================
        // Cut with Upper and Lower Boundary
        // ========================================

        if (
            // cut with LUP table
            
            ((event->tdc < utva)&&(event->tdc > ltva)&&
             (event->intg< uiva)&&(event->intg> liva)&&
             (dproc.LMODE==1))
            ||
            
            // cut with smooth curves
            
            ((delt> -1. * cfginfo->data.chan[st].ETCutW + tshift) &&
             (delt<  1. * cfginfo->data.chan[st].ETCutW + tshift) && 
             (dproc.CBANANA==0)&&(dproc.LMODE==0))
            ||
            ((delt > (float)(dproc.widthl) + tshift)&&
             (delt < (float)(dproc.widthu) + tshift)&&
             (dproc.CBANANA==1)&&(dproc.LMODE==0))
            ||
            ((Mass > Mcenter - 2.0 * Mwidth)&&
             (Mass < Mcenter + 2.0 * Mwidth)&&
             (dproc.CBANANA==2)&&(dproc.LMODE==0))
            ){
            
            // MASS mode (CARBON)
            if (dproc.MMODE == 1) {
                if (e>600. && e<1000.) {
                    hmasscbn[st] -> Fill(Mass);
                } 
            }
            
            // =======================================
            //   -t dependence
            // =======================================

           int tranges[7] = {
                320, 400, 480, 600, 800, 1000, 1200};

            for (int tbin=0; tbin<6; tbin++) {
                if (e> (float)tranges[tbin] && e< (float)tranges[tbin+1]) {
                    NTcounts[si][bunchx][tbin]++;
                    break;
                }
            }

            // for spin sorted fine bins
            
            int spbit;
            if (spinpat[bunchx]==1) {
                spbit = 0;
            } else if (spinpat[bunchx]==-1){
                spbit = 1;
            } else {
                spbit = 2;
            }
            
            int tfranges[11] = {
                380, 420, 460, 520, 580, 640, 700, 760, 840, 920, 1000};  
            for (int tbin=0; tbin<10 ; tbin++) {
                if (e> (float)tfranges[tbin] && e< (float)tfranges[tbin+1]) {
                    Tcounts[st][spbit][tbin]++;
                    break;
                }
            }
            
            // =======================================
            //   At Last select Carbon Event
            // =======================================

            // Energy Spectrum after Carbon Cut 
            henecbn[si]->Fill(e);
            henecbnitg[si]->Fill(e_int);

            if ((dproc.LMODE==1)  // LMODE ==1 use LUT
                ||
                (dproc.LMODE==0)&&  // use Energy Threshold 
                (e>(Emin))&&(e<Emax)&&(Mass>0.)) {
                
                // Delimiter Distribution
                hdelim -> Fill((float)event->delim);

                
                // Fill event in memory
                Nevcut++;
                Ngood[bunchx]++;
                
                // bunch distribution
                hbunch[si]->Fill((float)bunchx);
                
                
                // Strip Distribution (Carbon Cut)
                hstcbn[si]->Fill((float)(st+1-si*12));
                
                
                Ncounts[(int)(st/12)][bunchx]++;
                if (spinpat[bunchx]==0) {NSTcounts[st][0]++;}
                if (spinpat[bunchx]==1) {NSTcounts[st][1]++;}
                if (spinpat[bunchx]==-1) {NSTcounts[st][2]++;}

                // Ramp measurements binning
                // 20 Hz delimiters
                if (dproc.RAMPMODE==1) {
                    int rbin = (int)((event->delim)/20.);
                    NRcounts[(int)(st/12)][bunchx][rbin]++;
                }
                
            }
        }
    }
*/
    return(0);
}







