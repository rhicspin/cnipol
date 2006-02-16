// Event mode data analysis tool
//  file name :   rhic_asymmetry.C
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

// ==============================================
// End of data process (ASYMMETRY calculation)
// ==============================================
int TAsym::EndProcess(recordConfigRhicStruct *cfginfo)
{

    // ===============================================
    //  Analyzing Power
    // ===============================================
    
    // A_N values from L. Trueman
    //     data anth/0.034445, 0.026340, 0.020389, 0.016051, 0.012707,
    //               0.010014, 0.007777, 0.005875, 0.004233, 0.002799/
    //     t values: 0.0030,   0.0055,   0.0080,   0.0105,   0.0130, 
    //               0.0155,   0.0180,   0.0205,   0.0230,   0.0255
    
    //      t = e * 22.18 / 1000000.
    //      Emin = (0.0030-0.0025/2.)*1e6/22.18
    //      Emax = (0.0255+0.0025/2.)*1e6/22.18

    float anth[10] = { 0.034445, 0.026340, 0.020389, 0.016051, 0.012707,
                       0.010014, 0.007777, 0.005875, 0.004233, 0.002799};
    
    float Emin = LWAN; // 78.9
    float Emax = UPAN; //1206.0
    float DeltaE = (Emax - Emin)/10.;
    
    float sum = 0.;
    float suma = 0.;
    for (int i=0; i<BINAN; i++){
        int j = (int)((henetotal->GetBinCenter(i) - Emin)/DeltaE);
        sum  += henetotal->GetBinContent(i);
        suma += henetotal->GetBinContent(i) * anth[j];
    }

    TreeFill.an = suma/sum;
    TreeFill.enel = dproc.enel;
    TreeFill.eneu = dproc.eneu;
    A_N -> Fill();

    cout << "ANALYZING POWER (FIT) : " << suma/sum <<endl;
    
    // ==============================================
    // Right-Left asymmetry
    // ==============================================
    float NR, NL;
    float RL90[120],RL90E[120];
    float RL45[120],RL45E[120];
    float BT45[120],BT45E[120];

    for (int bid=0; bid<120; bid++){
        // R-L X90
        if (Ncounts[2-1][bid]+Ncounts[5-1][bid]!=0) {

            NR = Ncounts[2-1][bid];
            NL = Ncounts[5-1][bid];
            RL90[bid] = (float) (NR-NL)/(NR+NL);
            RL90E[bid] = (float) 2*NL*NR*sqrt((1./NR)+(1./NL))/(NL+NR)/(NL+NR);

        } else {
            RL90[bid] = 0.;
            RL90E[bid] = 0.;
        }
        // R-L X45 
        if (Ncounts[1-1][bid]+Ncounts[3-1][bid]+
            Ncounts[4-1][bid]+Ncounts[6-1][bid]!=0){
            
            NR = Ncounts[1-1][bid]+Ncounts[3-1][bid];
            NL = Ncounts[4-1][bid]+Ncounts[6-1][bid];
            RL45[bid] = (float) (NR-NL)/(NR+NL);
            RL45E[bid] = (float) 2*NL*NR*sqrt((1./NR)+(1./NL))/(NL+NR)/(NL+NR);

        } else {
            RL45[bid] = 0.;
            RL45E[bid] = 0.;
        }  
        // B-T Y45
        if (Ncounts[3-1][bid]+Ncounts[4-1][bid]+
            Ncounts[1-1][bid]+Ncounts[6-1][bid]!=0){
            
            NR = Ncounts[3-1][bid]+Ncounts[4-1][bid];
            NL = Ncounts[1-1][bid]+Ncounts[6-1][bid];

            BT45[bid] = (float) (NR-NL)/(NR+NL);
            BT45E[bid] = (float) 2*NL*NR*sqrt((1./NR)+(1./NL))/(NL+NR)/(NL+NR);

        } else {
            BT45[bid] = 0.;
            BT45E[bid] = 0.;
        }  
    }

    // ===================================================
    //  GOOD/BAD BUNCH CRITERIA PART 1
    // ===================================================

    float gtmin = 0.0;
    float gtmax = 1.0;
    float btmin = 0.0;
    float btmax = 1.00;
    int gbid[120];    // if 1:good and used 0: be discarded

    for (int bid=0; bid<120; bid++){
        gbid[bid] = 1;

        // good/total event rate
        if (Ntotal[bid]!=0){
            if ( ((float)Ngood[bid]/Ntotal[bid]) < gtmin) {
                fprintf(stdout,"BID: %d discarded (GOOD/TOTAL) %f \n",
                        bid,(float)Ngood[bid]/Ntotal[bid]);
                gbid[bid] = 0;
            }
            if ( ((float)Ngood[bid]/Ntotal[bid]) > gtmax) {
                fprintf(stdout,"BID: %d discarded (GOOD/TOTAL) %f \n",
                        bid,(float)Ngood[bid]/Ntotal[bid]);
                gbid[bid] = 0;
            }
        }
        // background / carbon event rate
        if (Ngood[bid]!=0){
            if (((float)Nback[bid]/Ngood[bid])<btmin){
                fprintf(stdout,"BID: %d discarded (BG/GOOD) %f \n",
                        bid,(float)Nback[bid]/Ngood[bid]);
                gbid[bid] = 0;
            }
            if (((float)Nback[bid]/Ngood[bid])>btmax) {
                fprintf(stdout,"BID: %d discarded (BG/GOOD) %f \n",
                        bid,(float)Nback[bid]/Ngood[bid]);
                gbid[bid] = 0;
            }
        }
    }   
    cout << "Finshed Bad/Good banch estimation "<<endl;
    
    // =========================================
    // Counts for each detector
    // =========================================
    long Nsi[6] = {0};
    int Nsiz[6] = {0};
    int Nsiu[6] = {0};
    int Nsid[6] = {0};
    
    for (int bid=0; bid<120; bid++){
        for (int si=0; si<6; si++) {
            Nsi[si]+=Ncounts[si][bid];
            TreeFill.counts[si] = Ncounts[si][bid];
        }
        TreeFill.spin = spinpat[bid]; 
        Counts -> Fill();
    }

    for (int si=0; si<6; si++) {
        TreeFill.counts[si] = Nsi[si];
    }
    TreeFill.spin = 0;
    Counts -> Fill();
    
    for (int bid=0; bid<120; bid++) {
        for (int si=0; si<6; si++){
            Nsiz[si] += Ncounts[si][bid]*((spinpat[bid]==0)?1:0);
            Nsiu[si] += Ncounts[si][bid]*((spinpat[bid]==1)?1:0);
            Nsid[si] += Ncounts[si][bid]*((spinpat[bid]==-1)?1:0);
        }
    }
    fprintf(stdout,"Ncounts 0  : %d %d %d %d %d %d\n",
            Nsiz[0],Nsiz[1],Nsiz[2],Nsiz[3],Nsiz[4],Nsiz[5]);
    fprintf(stdout,"Ncounts Up : %d %d %d %d %d %d\n",
            Nsiu[0],Nsiu[1],Nsiu[2],Nsiu[3],Nsiu[4],Nsiu[5]);
    fprintf(stdout,"Ncounts Dn : %d %d %d %d %d %d\n",
            Nsid[0],Nsid[1],Nsid[2],Nsid[3],Nsid[4],Nsid[5]);

    float TotalUp, TotalDown;
    float TotalRatio;
    TotalUp = (float)(Nsiu[0]+Nsiu[1]+Nsiu[2]+Nsiu[3]+Nsiu[4]+Nsiu[5]);
    TotalDown = (float)(Nsid[0]+Nsid[1]+Nsid[2]+Nsid[3]+Nsid[4]+Nsid[5]);
    TotalRatio = TotalUp/TotalDown;
    

    for (int i=1; i<72; i++) {
        fprintf(stdout,"St-%02d : (+)%8d (-)%8d (0)%8d : %8.5f \n",
                i+1,NSTcounts[i][1],NSTcounts[i][2],NSTcounts[i][0],
                ((float)NSTcounts[i][1]-(float)NSTcounts[i][2]*TotalRatio)/
                ((float)NSTcounts[i][1]+(float)NSTcounts[i][2]*TotalRatio)
                );
    }

    cout << "Fishined counting each detector "<<endl;

    // ===========================================
    // Asymmetry Calculation by Square Root Formula
    // ===========================================
    
    asymStruct x90[120];  // x90[119] is total
    asymStruct x45[120];
    asymStruct y45[120];
    asymStruct cr45[120];
    float RU[120],RD[120],LU[120],LD[120];
    float tmpasym,tmpasyme;
    
    // X90 (2-5) (C:1-4)
    for (int bid=0;bid<120;bid++){
        RU[bid] = ((bid==0)?0:RU[bid-1])
            + Ncounts[2-1][bid]*((spinpat[bid]==1)?1:0);
        RD[bid] = ((bid==0)?0:RD[bid-1])
            + Ncounts[2-1][bid]*((spinpat[bid]==-1)?1:0);
        LU[bid] = ((bid==0)?0:LU[bid-1])
            + Ncounts[5-1][bid]*((spinpat[bid]==1)?1:0);
        LD[bid] = ((bid==0)?0:LD[bid-1])
            + Ncounts[5-1][bid]*((spinpat[bid]==-1)?1:0);

        sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);
        x90[bid].phys = tmpasym; x90[bid].physE = tmpasyme; 
        sqass(RU[bid],RD[bid],LD[bid],LU[bid],&tmpasym,&tmpasyme);
        x90[bid].acpt = tmpasym; x90[bid].acptE = tmpasyme; 
        sqass(RU[bid],LU[bid],RD[bid],LD[bid],&tmpasym,&tmpasyme);
        x90[bid].lumi = tmpasym; x90[bid].lumiE = tmpasyme; 
    }
    
    // X45 (13-46) (C:02-35)
    for (int bid=0;bid<120;bid++){
        RU[bid] = ((bid==0)?0:RU[bid-1])
            + (Ncounts[1-1][bid]+Ncounts[3-1][bid])*
            ((spinpat[bid]==1)?1:0);
        RD[bid] = ((bid==0)?0:RD[bid-1])
            + (Ncounts[1-1][bid]+Ncounts[3-1][bid])*
            ((spinpat[bid]==-1)?1:0);
        LU[bid] = ((bid==0)?0:LU[bid-1])
            + (Ncounts[4-1][bid]+Ncounts[6-1][bid])*
            ((spinpat[bid]==1)?1:0);
        LD[bid] = ((bid==0)?0:LD[bid-1])
            + (Ncounts[4-1][bid]+Ncounts[6-1][bid])*
            ((spinpat[bid]==-1)?1:0);

        sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);
        x45[bid].phys = tmpasym; x45[bid].physE = tmpasyme; 
        sqass(RU[bid],RD[bid],LD[bid],LU[bid],&tmpasym,&tmpasyme);
        x45[bid].acpt = tmpasym; x45[bid].acptE = tmpasyme; 
        sqass(RU[bid],LU[bid],RD[bid],LD[bid],&tmpasym,&tmpasyme);
        x45[bid].lumi = tmpasym; x45[bid].lumiE = tmpasyme; 
    }
    
    // Y45 (34-16) (C:23-05)
    for (int bid=0;bid<120;bid++){
        RU[bid] = ((bid==0)?0:RU[bid-1])
            + (Ncounts[3-1][bid]+Ncounts[4-1][bid])*
            ((spinpat[bid]==1)?1:0);
        RD[bid] = ((bid==0)?0:RD[bid-1])
            + (Ncounts[3-1][bid]+Ncounts[4-1][bid])*
            ((spinpat[bid]==-1)?1:0);
        LU[bid] = ((bid==0)?0:LU[bid-1])
            + (Ncounts[1-1][bid]+Ncounts[6-1][bid])*
            ((spinpat[bid]==1)?1:0);
        LD[bid] = ((bid==0)?0:LD[bid-1])
            + (Ncounts[1-1][bid]+Ncounts[6-1][bid])*
            ((spinpat[bid]==-1)?1:0);

        sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);
        y45[bid].phys = tmpasym; y45[bid].physE = tmpasyme; 
        sqass(RU[bid],RD[bid],LD[bid],LU[bid],&tmpasym,&tmpasyme);
        y45[bid].acpt = tmpasym; y45[bid].acptE = tmpasyme; 
        sqass(RU[bid],LU[bid],RD[bid],LD[bid],&tmpasym,&tmpasyme);
        y45[bid].lumi = tmpasym; y45[bid].lumiE = tmpasyme; 
    }

    // CR45 (14-36) (C:03-25)
    for (int bid=0;bid<120;bid++){
        RU[bid] = ((bid==0)?0:RU[bid-1])
            + (Ncounts[1-1][bid]+Ncounts[4-1][bid])*
            ((spinpat[bid]==1)?1:0);

        RD[bid] = ((bid==0)?0:RD[bid-1])
            + (Ncounts[1-1][bid]+Ncounts[4-1][bid])*
            ((spinpat[bid]==-1)?1:0);


        LU[bid] = ((bid==0)?0:LU[bid-1])
            + (Ncounts[3-1][bid]+Ncounts[6-1][bid])*
            ((spinpat[bid]==1)?1:0);

        LD[bid] = ((bid==0)?0:LD[bid-1])
            + (Ncounts[3-1][bid]+Ncounts[6-1][bid])*
            ((spinpat[bid]==-1)?1:0);

        sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);
        cr45[bid].phys = tmpasym; cr45[bid].physE = tmpasyme; 
        
        sqass(RU[bid],RD[bid],LD[bid],LU[bid],&tmpasym,&tmpasyme);
        cr45[bid].acpt = tmpasym; cr45[bid].acptE = tmpasyme; 

        sqass(RU[bid],LU[bid],RD[bid],LD[bid],&tmpasym,&tmpasyme);
        cr45[bid].lumi = tmpasym; cr45[bid].lumiE = tmpasyme; 
    }
    
    for (int bid=0; bid<120; bid++) {
        TreeFill.x90p = x90[bid].phys; TreeFill.x90pe = x90[bid].physE;
        TreeFill.x90a = x90[bid].acpt; TreeFill.x90ae = x90[bid].acptE;
        TreeFill.x90l = x90[bid].lumi; TreeFill.x90le = x90[bid].lumiE;
    
        TreeFill.x45p = x45[bid].phys; TreeFill.x45pe = x45[bid].physE;
        TreeFill.x45a = x45[bid].acpt; TreeFill.x45ae = x45[bid].acptE;
        TreeFill.x45l = x45[bid].lumi; TreeFill.x45le = x45[bid].lumiE;
    
        TreeFill.y45p = y45[bid].phys; TreeFill.y45pe = y45[bid].physE;
        TreeFill.y45a = y45[bid].acpt; TreeFill.y45ae = y45[bid].acptE;
        TreeFill.y45l = y45[bid].lumi; TreeFill.y45le = y45[bid].lumiE;
    
        TreeFill.c45p = cr45[bid].phys; TreeFill.c45pe = cr45[bid].physE;
        TreeFill.c45a = cr45[bid].acpt; TreeFill.c45ae = cr45[bid].acptE;
        TreeFill.c45l = cr45[bid].lumi; TreeFill.c45le = cr45[bid].lumiE;

        TreeFill.rl90 = RL90[bid]; TreeFill.rl90e = RL90E[bid];
        TreeFill.rl45 = RL45[bid]; TreeFill.rl45e = RL45E[bid];
        TreeFill.bt45 = BT45[bid]; TreeFill.bt45e = BT45E[bid];

        TreeFill.Ngood = Ngood[bid];
        TreeFill.Nback = Nback[bid];
        TreeFill.Ntotal = Ntotal[bid];

        Asym -> Fill();
    }

    cout << "*************** RESULT *******************" <<endl;
    cout << 
        "        physics                luminosity             acceptance"
         <<endl;
    fprintf(stdout,"X90  :%10.6f+-%10.6f %10.6f+-%10.6f %10.6f+-%10.6f\n",
            x90[119].phys,x90[119].physE,
            x90[119].lumi,x90[119].lumiE,
            x90[119].acpt,x90[119].acptE);
    fprintf(stdout,"X45  :%10.6f+-%10.6f %10.6f+-%10.6f %10.6f+-%10.6f\n",
            x45[119].phys,x45[119].physE,
            x45[119].lumi,x45[119].lumiE,
            x45[119].acpt,x45[119].acptE);
    fprintf(stdout,"Y45  :%10.6f+-%10.6f %10.6f+-%10.6f %10.6f+-%10.6f\n",
            y45[119].phys,y45[119].physE,
            y45[119].lumi,y45[119].lumiE,
            y45[119].acpt,y45[119].acptE);
    fprintf(stdout,"CR45 :%10.6f+-%10.6f %10.6f+-%10.6f %10.6f+-%10.6f\n",
            cr45[119].phys,cr45[119].physE,
            cr45[119].lumi,cr45[119].lumiE,
            cr45[119].acpt,cr45[119].acptE);

    cout << "*************** RESULT *******************" <<endl;

    // ============================================
    // **** for different t range ****
    // ============================================

    asymStruct tx90[120][6];  // x90[119] is total
    asymStruct tx45[120][6];
    asymStruct ty45[120][6];

    for (int tr=0; tr<6; tr++){
        // X90 (2-5)
        for (int bid=0; bid<120; bid++){
            RU[bid] = ((bid==0)?0:RU[bid-1])
                + NTcounts[2-1][bid][tr]*((spinpat[bid]==1)?1:0)*gbid[bid];
            RD[bid] = ((bid==0)?0:RD[bid-1])
                + NTcounts[2-1][bid][tr]*((spinpat[bid]==-1)?1:0)*gbid[bid];
            LU[bid] = ((bid==0)?0:LU[bid-1])
                + NTcounts[5-1][bid][tr]*((spinpat[bid]==1)?1:0)*gbid[bid];
            LD[bid] = ((bid==0)?0:LD[bid-1])
                + NTcounts[5-1][bid][tr]*((spinpat[bid]==-1)?1:0)*gbid[bid];
            sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);
            tx90[bid][tr].phys = tmpasym; tx90[bid][tr].physE = tmpasyme; 
        }
        // X45 (13-46)
        for (int bid=0; bid<120; bid++){
            RU[bid] = ((bid==0)?0:RU[bid-1])
                + (NTcounts[1-1][bid][tr]+NTcounts[3-1][bid][tr])
                *((spinpat[bid]==1)?1:0)*gbid[bid];
            RD[bid] = ((bid==0)?0:RD[bid-1])
                + (NTcounts[1-1][bid][tr]+NTcounts[3-1][bid][tr])
                *((spinpat[bid]==-1)?1:0)*gbid[bid];
            LU[bid] = ((bid==0)?0:LU[bid-1])
                + (NTcounts[4-1][bid][tr]+NTcounts[6-1][bid][tr])
                *((spinpat[bid]==1)?1:0)*gbid[bid];
            LD[bid] = ((bid==0)?0:LD[bid-1])
                + (NTcounts[4-1][bid][tr]+NTcounts[6-1][bid][tr])
                *((spinpat[bid]==-1)?1:0)*gbid[bid];
            sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);
            tx45[bid][tr].phys = tmpasym; tx45[bid][tr].physE = tmpasyme; 
        }
        // Y45 (34-16)
        for (int bid=0; bid<120; bid++){
            RU[bid] = ((bid==0)?0:RU[bid-1])
                + (NTcounts[3-1][bid][tr]+NTcounts[4-1][bid][tr])
                *((spinpat[bid]==1)?1:0)*gbid[bid];
            RD[bid] = ((bid==0)?0:RD[bid-1])
                + (NTcounts[3-1][bid][tr]+NTcounts[4-1][bid][tr])
                *((spinpat[bid]==-1)?1:0)*gbid[bid];
            LU[bid] = ((bid==0)?0:LU[bid-1])
                + (NTcounts[1-1][bid][tr]+NTcounts[6-1][bid][tr])
                *((spinpat[bid]==1)?1:0)*gbid[bid];
            LD[bid] = ((bid==0)?0:LD[bid-1])
                + (NTcounts[1-1][bid][tr]+NTcounts[6-1][bid][tr])
                *((spinpat[bid]==-1)?1:0)*gbid[bid];
            sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);
            ty45[bid][tr].phys = tmpasym; ty45[bid][tr].physE = tmpasyme; 
        }
    }   

    for (int bid=0; bid<120; bid++){
        for (int tr=0; tr< 6 ;tr++) {
            TreeFill.tx90p[tr]  = tx90[bid][tr].phys;
            TreeFill.tx90pe[tr] = tx90[bid][tr].physE;
            TreeFill.tx45p[tr]  = tx45[bid][tr].phys;
            TreeFill.tx45pe[tr] = tx45[bid][tr].physE;
            TreeFill.ty45p[tr]  = ty45[bid][tr].phys;
            TreeFill.ty45pe[tr] = ty45[bid][tr].physE;
        }
        Tbasym -> Fill();
    }

    // -----------------------------------------------------
    // output for fine -t bins for every strips
    // -----------------------------------------------------

    for (int i=0;i<10;i++){
        for (int st=0; st<72; st++){
            TreeFill.tstripup[st] = Tcounts[st][0][i];
            TreeFill.tstripdw[st] = Tcounts[st][1][i];
        }
        Tstrip -> Fill();
    }

    // --------------------------------------------------
    //  Various Calibration Coefficients 
    // --------------------------------------------------
    for (int st=0; st<72; st++){
        TreeFill.acoef = cfginfo->data.chan[st].acoef;
        TreeFill.ecoef = cfginfo->data.chan[st].ecoef;
        TreeFill.edead = cfginfo->data.chan[st].edead;
        TreeFill.t0 = cfginfo->data.chan[st].t0;
        TreeFill.dwidth = dthick[st]; 
        TreeFill.kint0 = kint0[st];
        Coefs -> Fill();
    }

    //-------------------------------------------------------
    // RAMP MEASUREMENT 
    //-------------------------------------------------------
    if (dproc.RAMPMODE==1) {

        long SIU[6],SID[6];
        
        for (int dlm=0;dlm<RAMPTIME;dlm++){
            // not need for initialization for RUN,RD,LU,LD
            // they are initialized at bid=0
            //memset(SIU,0,sizeof(SIU));
            //memset(SID,0,sizeof(SID));
            
            for (int bid=0;bid<120;bid++){
                for (int si=0;si<6;si++){
                    SIU[si] += (NRcounts[si][bid][dlm])
                        *((spinpat[bid]==1)?1:0)*gbid[bid];
                    SID[si] += (NRcounts[si][bid][dlm])
                        *((spinpat[bid]==-1)?1:0)*gbid[bid];
                }
            }
            for (int si=0; si<6; si++) {
                TreeFill.rampup[si] = SIU[si];
                TreeFill.rampdw[si] = SID[si];
                Tramp -> Fill();
            }
        }            
    }

    cout << "Asymmetry Calculatioin Finished " <<endl;

    return(0);
}









