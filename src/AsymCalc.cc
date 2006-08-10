//  Asymmetry Analysis of RHIC pC Polarimeter
//  End-of-Run routine
//  file name :   AsymCalc.cc
// 
//  Author    :   Itaru Nakagawa
//  Creation  :   01/21/2006
//                

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <iostream.h>
#include "TMinuit.h"
#include "TString.h"
#include "TMath.h"
#include "rhicpol.h"
#include "rpoldata.h"
#include "Asym.h"
#include "WeightedMean.h"
#include "AsymErrorDetector.h"
#include "AsymCalc.h"

BunchAsym basym;
float RawP[72], dRawP[72]; // Raw Polarization (Not corrected for phi)


// =========================
// End of data process
// =========================
int end_process(recordConfigRhicStruct *cfginfo)
{
  StructHistStat hstat;

  if (Flag.feedback){

    //-------------------------------------------------------
    //                Feedback Mode
    //-------------------------------------------------------
    int FeedBackLevel=2; // 1: no fit just max and mean  
                         // 2: gaussian fit
    anal.TshiftAve = TshiftFinder(Flag.feedback, FeedBackLevel);

    // reset counters
    Nevtot = Nread = 0;
    for (int i=0;i<120;i++) Ntotal[i] = 0;

    return 0;

  }else if (!dproc.DMODE) {

    //-------------------------------------------------------
    //    Energy Yeild Weighted Average Analyzing Power
    //-------------------------------------------------------
      //    anal.A_N[0]=WeightAnalyzingPower(10040); // no cut in energy spectra
    anal.A_N[1]=WeightAnalyzingPower(10050); // banana cut in energy spectra


    //-------------------------------------------------------
    //              CumulativeAsymmetry()    
    //-------------------------------------------------------
    CumulativeAsymmetry();

    //-------------------------------------------------------
    // Bunch Asymmetries
    //-------------------------------------------------------
    calcBunchAsymmetry();

    //-------------------------------------------------------
    //  Check for bunch anomaly 
    //-------------------------------------------------------
    BunchAnomalyDetector();

    //-------------------------------------------------------
    // Strip-by-Strip Asymmetries
    //-------------------------------------------------------
    if (dproc.RECONFMODE) CalcStripAsymmetry(anal.A_N[1]);

    //-------------------------------------------------------
    //  Check 12C Invariant Mass Possition
    //-------------------------------------------------------
    //TshiftFinder(Flag.feedback, 2);


    //-------------------------------------------------------
    // Specific Luminosity 
    //-------------------------------------------------------
    SpecificLuminosity(hstat.mean, hstat.RMS, hstat.RMSnorm);

  }//end-of-if(!dproc.DMODE)


  //-------------------------------------------------------
  // Run Informations
  //-------------------------------------------------------
  PrintWarning();
  PrintRunResults(hstat);

  return(0);


}




//
// Class name  :
// Method name : CumulativeAsymmetry(){
//
// Description : Caluclate bunch cumulative asymmetries
// Input       : 
// Return      : 
//
int
CumulativeAsymmetry(){


    int bid;
    asymStruct x90[120];  // x90[119] is total
    asymStruct x45[120];
    asymStruct y45[120];
    asymStruct cr45[120];
    asymStruct tx90[120][6];  
    asymStruct tx45[120][6];
    asymStruct ty45[120][6];
    float RL90[120],RL90E[120];
    float RL45[120],RL45E[120];
    float BT45[120],BT45E[120];
    float NL,NR;
    float tmpasym,tmpasyme;
    FILE *fp;
    float RU[120],RD[120],LU[120],LD[120];
    long SIU[6],SID[6];
    int tr,si;
    int gbid[120];    // if 1:good and used 0: be discarded
    float gtmin,gtmax,btmin,btmax;
    float X[HENEBIN],Y[HENEBIN],EX[HENEBIN],EY[HENEBIN];
    float fspinpat[120];
    int i,j,k;
    long Nsi[6]={0,0,0,0,0,0};

    
    //====================================================
    // Right-Left asymmetry
    //====================================================
    for (bid=0;bid<120;bid++){
        
        fspinpat[bid] = (float) spinpat[bid];
        
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

    // *** GOOD/BAD BUNCH CRITERIA 1

    gtmin = 0.0;
    gtmax = 1.0;
    btmin = 0.0;
    btmax = 1.00;

    for (bid=0;bid<120;bid++){
        gbid[bid] = 1;
        // good/total event rate
        if (Ntotal[bid]!=0){
            if (((float)Ngood[bid]/Ntotal[bid])<gtmin){
                fprintf(stdout,"BID: %d discarded (GOOD/TOTAL) %f \n",
                        bid,(float)Ngood[bid]/Ntotal[bid]);
                gbid[bid] = 0;
            }
            if (((float)Ngood[bid]/Ntotal[bid])>gtmax){
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
    // Counts for each detector
    for (bid=0;bid<120;bid++){
        Nsi[0]+=Ncounts[0][bid];
        Nsi[1]+=Ncounts[1][bid];
        Nsi[2]+=Ncounts[2][bid];
        Nsi[3]+=Ncounts[3][bid];
        Nsi[4]+=Ncounts[4][bid];
        Nsi[5]+=Ncounts[5][bid];
    }


    HHPAK(31010, (float*)Ncounts[0]);
    HHPAK(31020, (float*)Ncounts[1]);
    HHPAK(31030, (float*)Ncounts[2]);
    HHPAK(31040, (float*)Ncounts[3]);
    HHPAK(31050, (float*)Ncounts[4]);
    HHPAK(31060, (float*)Ncounts[5]);


    float x90phys[2][120], x90acpt[2][120], x90lumi[2][120];
    float x45phys[2][120], x45acpt[2][120], x45lumi[2][120];
    float y45phys[2][120], y45acpt[2][120], y45lumi[2][120];
    float c45phys[2][120], c45acpt[2][120], c45lumi[2][120];


    // X90 (2-5) (C:1-4)
    for (bid=0;bid<120;bid++){
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
        x90phys[0][bid] = tmpasym;
        x90phys[1][bid] = tmpasyme;
        sqass(RU[bid],RD[bid],LD[bid],LU[bid],&tmpasym,&tmpasyme);
        x90[bid].acpt = tmpasym; x90[bid].acptE = tmpasyme; 
        x90acpt[0][bid] = tmpasym;
        x90acpt[1][bid] = tmpasyme;
        sqass(RU[bid],LU[bid],RD[bid],LD[bid],&tmpasym,&tmpasyme);
        x90[bid].lumi = tmpasym; x90[bid].lumiE = tmpasyme; 
        x90lumi[0][bid] = tmpasym;
        x90lumi[1][bid] = tmpasyme;
        //        printf("%d : %d %f %f %f %f \n",bid,spinpat[bid],
        //       RU[bid],RD[bid],LU[bid],LD[bid]);
    }
    fprintf(stdout,"si2 up :%10.0f down :%10.0f\n",RU[119],RD[119]);
    fprintf(stdout,"si5 up :%10.0f down :%10.0f\n",LU[119],LD[119]);

    // X45 (13-46) (C:02-35)
    for (bid=0;bid<120;bid++){
        RU[bid] = ((bid==0)?0:RU[bid-1])
            + (Ncounts[1-1][bid]+Ncounts[3-1][bid])*((spinpat[bid]==1)?1:0);
        RD[bid] = ((bid==0)?0:RD[bid-1])
            + (Ncounts[1-1][bid]+Ncounts[3-1][bid])*((spinpat[bid]==-1)?1:0);
        LU[bid] = ((bid==0)?0:LU[bid-1])
            + (Ncounts[4-1][bid]+Ncounts[6-1][bid])*((spinpat[bid]==1)?1:0);
        LD[bid] = ((bid==0)?0:LD[bid-1])
            + (Ncounts[4-1][bid]+Ncounts[6-1][bid])*((spinpat[bid]==-1)?1:0);

        sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);
        x45[bid].phys = tmpasym; x45[bid].physE = tmpasyme; 
        x45phys[0][bid] = tmpasym;
        x45phys[1][bid] = tmpasyme;
        sqass(RU[bid],RD[bid],LD[bid],LU[bid],&tmpasym,&tmpasyme);
        x45[bid].acpt = tmpasym; x45[bid].acptE = tmpasyme; 
        x45acpt[0][bid] = tmpasym;
        x45acpt[1][bid] = tmpasyme;
        sqass(RU[bid],LU[bid],RD[bid],LD[bid],&tmpasym,&tmpasyme);
        x45[bid].lumi = tmpasym; x45[bid].lumiE = tmpasyme; 
        x45lumi[0][bid] = tmpasym;
        x45lumi[1][bid] = tmpasyme;
    }
    fprintf(stdout,"si1,3 up :%10.0f down :%10.0f\n",RU[119],RD[119]);
    fprintf(stdout,"si4,6 up :%10.0f down :%10.0f\n",LU[119],LD[119]);

    // Y45 (34-16) (C:23-05)
    for (bid=0;bid<120;bid++){
        RU[bid] = ((bid==0)?0:RU[bid-1])
            + (Ncounts[3-1][bid]+Ncounts[4-1][bid])*((spinpat[bid]==1)?1:0);
        RD[bid] = ((bid==0)?0:RD[bid-1])
            + (Ncounts[3-1][bid]+Ncounts[4-1][bid])*((spinpat[bid]==-1)?1:0);
        LU[bid] = ((bid==0)?0:LU[bid-1])
            + (Ncounts[1-1][bid]+Ncounts[6-1][bid])*((spinpat[bid]==1)?1:0);
        LD[bid] = ((bid==0)?0:LD[bid-1])
            + (Ncounts[1-1][bid]+Ncounts[6-1][bid])*((spinpat[bid]==-1)?1:0);

        sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);
        y45[bid].phys = tmpasym; y45[bid].physE = tmpasyme; 
        y45phys[0][bid] = tmpasym;
        y45phys[1][bid] = tmpasyme;
        sqass(RU[bid],RD[bid],LD[bid],LU[bid],&tmpasym,&tmpasyme);
        y45[bid].acpt = tmpasym; y45[bid].acptE = tmpasyme; 
        y45acpt[0][bid] = tmpasym;
        y45acpt[1][bid] = tmpasyme;
        sqass(RU[bid],LU[bid],RD[bid],LD[bid],&tmpasym,&tmpasyme);
        y45[bid].lumi = tmpasym; y45[bid].lumiE = tmpasyme; 
        y45lumi[0][bid] = tmpasym;
        y45lumi[1][bid] = tmpasyme;
    }
    // CR45 (14-36) (C:03-25)
    for (bid=1;bid<120;bid++){
        RU[bid] = ((bid==0)?0:RU[bid-1])
            + (Ncounts[1-1][bid]+Ncounts[4-1][bid])*((spinpat[bid]==1)?1:0);
        RD[bid] = ((bid==0)?0:RD[bid-1])
            + (Ncounts[1-1][bid]+Ncounts[4-1][bid])*((spinpat[bid]==-1)?1:0);
        LU[bid] = ((bid==0)?0:LU[bid-1])
            + (Ncounts[3-1][bid]+Ncounts[6-1][bid])*((spinpat[bid]==1)?1:0);
        LD[bid] = ((bid==0)?0:LD[bid-1])
            + (Ncounts[3-1][bid]+Ncounts[6-1][bid])*((spinpat[bid]==-1)?1:0);
        
        sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);
        cr45[bid].phys = tmpasym; cr45[bid].physE = tmpasyme; 
        c45phys[0][bid]  = tmpasym; 
        c45phys[1][bid] = tmpasyme; 
        
        sqass(RU[bid],RD[bid],LD[bid],LU[bid],&tmpasym,&tmpasyme);
        cr45[bid].acpt = tmpasym; cr45[bid].acptE = tmpasyme; 
        c45acpt[0][bid]  = tmpasym; 
        c45acpt[1][bid] = tmpasyme; 

        sqass(RU[bid],LU[bid],RD[bid],LD[bid],&tmpasym,&tmpasyme);
        cr45[bid].lumi = tmpasym; cr45[bid].lumiE = tmpasyme; 
        c45lumi[0][bid]  = tmpasym; 
        c45lumi[1][bid] = tmpasyme; 
    }
    
    HHPAK(30000, x90phys[0]); HHPAKE(30000, x90phys[1]);
    HHPAK(30010, x90acpt[0]); HHPAKE(30010, x90acpt[1]);
    HHPAK(30020, x90lumi[0]); HHPAKE(30020, x90lumi[1]);

    HHPAK(30100, x45phys[0]); HHPAKE(30100, x45phys[1]);
    HHPAK(30110, x45acpt[0]); HHPAKE(30110, x45acpt[1]);
    HHPAK(30120, x45lumi[0]); HHPAKE(30120, x45lumi[1]);

    HHPAK(30200, y45phys[0]); HHPAKE(30200, y45phys[1]);
    HHPAK(30210, y45acpt[0]); HHPAKE(30210, y45acpt[1]);
    HHPAK(30220, y45lumi[0]); HHPAKE(30220, y45lumi[1]);

    HHPAK(30300, c45phys[0]); HHPAKE(30300, c45phys[1]);
    HHPAK(30310, c45acpt[0]); HHPAKE(30310, c45acpt[1]);
    HHPAK(30320, c45lumi[0]); HHPAKE(30320, c45lumi[1]);




    printf("*************** RESULT *******************\n");
    printf("        physics                luminosity             acceptance\n");
    printf("X90  :%10.6f+-%10.6f %10.6f+-%10.6f %10.6f+-%10.6f\n",
            x90[119].phys,x90[119].physE,
            x90[119].lumi,x90[119].lumiE,
            x90[119].acpt,x90[119].acptE);
    printf("X45  :%10.6f+-%10.6f %10.6f+-%10.6f %10.6f+-%10.6f\n",
            x45[119].phys,x45[119].physE,
            x45[119].lumi,x45[119].lumiE,
            x45[119].acpt,x45[119].acptE);
    printf("Y45  :%10.6f+-%10.6f %10.6f+-%10.6f %10.6f+-%10.6f\n",
            y45[119].phys,y45[119].physE,
            y45[119].lumi,y45[119].lumiE,
            y45[119].acpt,y45[119].acptE);
    printf("CR45 :%10.6f+-%10.6f %10.6f+-%10.6f %10.6f+-%10.6f\n",
            cr45[119].phys,cr45[119].physE,
            cr45[119].lumi,cr45[119].lumiE,
            cr45[119].acpt,cr45[119].acptE);

    printf("*************** RESULT *******************\n");





    // Target Position Loop
    for (int i=0; i<=nTgtIndex; i++) {

    // X90 (2-5) (C:1-4)
      for (bid=0;bid<120;bid++){
        RU[bid] = ((bid==0)?0:RU[bid-1])
            + NDcounts[2-1][bid][i]*((spinpat[bid]==1)?1:0);
        RD[bid] = ((bid==0)?0:RD[bid-1])
            + NDcounts[2-1][bid][i]*((spinpat[bid]==-1)?1:0);
        LU[bid] = ((bid==0)?0:LU[bid-1])
            + NDcounts[5-1][bid][i]*((spinpat[bid]==1)?1:0);
        LD[bid] = ((bid==0)?0:LD[bid-1])
            + NDcounts[5-1][bid][i]*((spinpat[bid]==-1)?1:0);
        sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);
        x90[bid].phys = tmpasym; x90[bid].physE = tmpasyme; 
        x90phys[0][bid] = tmpasym;
        x90phys[1][bid] = tmpasyme;

      }

     HHPAK(37000+i, x90phys[0]); HHPAKE(37000+i, x90phys[1]);

    // X45 (13-46) (C:02-35)
    for (bid=0;bid<120;bid++){
        RU[bid] = ((bid==0)?0:RU[bid-1])
            + (NDcounts[1-1][bid][i]+NDcounts[3-1][bid][i])*((spinpat[bid]==1)?1:0);
        RD[bid] = ((bid==0)?0:RD[bid-1])
            + (NDcounts[1-1][bid][i]+NDcounts[3-1][bid][i])*((spinpat[bid]==-1)?1:0);
        LU[bid] = ((bid==0)?0:LU[bid-1])
            + (NDcounts[4-1][bid][i]+NDcounts[6-1][bid][i])*((spinpat[bid]==1)?1:0);
        LD[bid] = ((bid==0)?0:LD[bid-1])
            + (NDcounts[4-1][bid][i]+NDcounts[6-1][bid][i])*((spinpat[bid]==-1)?1:0);

        sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);
        x45[bid].phys = tmpasym; x45[bid].physE = tmpasyme; 
        x45phys[0][bid] = tmpasym;
        x45phys[1][bid] = tmpasyme;
    }

     HHPAK(37500+i, x45phys[0]); HHPAKE(37500+i, x45phys[1]);

    } // end-of-nTgtIndex loop



    // **** for different t range ****

    float txasym90[6][120], txasym90E[6][120];
    float txasym45[6][120], txasym45E[6][120];
    float tyasym45[6][120], tyasym45E[6][120];
    float tcasym45[6][120], tcasym45E[6][120];


    for (tr=0;tr<NTBIN;tr++){
      float SUM=0;
        // X90 (2-5)
        for (bid=0;bid<120;bid++){
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
            txasym90[tr][bid] = tmpasym; txasym90E[tr][bid] = tmpasyme; 
	    SUM+=NTcounts[2-1][bid][tr];
        }

        // X45 (13-46)
        for (bid=0;bid<120;bid++){
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
            txasym45[tr][bid] = tmpasym; txasym45E[tr][bid] = tmpasyme; 
        }

        // Y45 (34-16)
        for (bid=0;bid<120;bid++){
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
            tyasym45[tr][bid] = tmpasym; tyasym45E[tr][bid] = tmpasyme; 
        }
        // CROSS 45 (14-36)
        for (bid=0;bid<120;bid++){
            RU[bid] = ((bid==0)?0:RU[bid-1])
                + (NTcounts[1-1][bid][tr]+NTcounts[4-1][bid][tr])
                *((spinpat[bid]==1)?1:0)*gbid[bid];
            RD[bid] = ((bid==0)?0:RD[bid-1])
                + (NTcounts[1-1][bid][tr]+NTcounts[4-1][bid][tr])
                *((spinpat[bid]==-1)?1:0)*gbid[bid];
            LU[bid] = ((bid==0)?0:LU[bid-1])
                + (NTcounts[3-1][bid][tr]+NTcounts[6-1][bid][tr])
                *((spinpat[bid]==1)?1:0)*gbid[bid];
            LD[bid] = ((bid==0)?0:LD[bid-1])
                + (NTcounts[3-1][bid][tr]+NTcounts[6-1][bid][tr])
                *((spinpat[bid]==-1)?1:0)*gbid[bid];
            sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);
            tcasym45[tr][bid] = tmpasym; tcasym45E[tr][bid] = tmpasyme; 
        }
        // Fill Histograms
        HHPAK(32000+tr+1, txasym90[tr]);  HHPAKE(32000+tr+1, txasym90E[tr]);
        HHPAK(32100+tr+1, txasym45[tr]);  HHPAKE(32100+tr+1, txasym45E[tr]);
        HHPAK(32200+tr+1, tyasym45[tr]);  HHPAKE(32200+tr+1, tyasym45E[tr]);
        HHPAK(32300+tr+1, tcasym45[tr]);  HHPAKE(32300+tr+1, tcasym45E[tr]);
    }   
    
    // Fill Histograms
    HHPAK(31000, fspinpat);
    HHPAK(31100, RL90);    HHPAKE(31100, RL90E);
    HHPAK(31110, RL45);    HHPAKE(31110, RL45E);
    HHPAK(31120, BT45);    HHPAKE(31120, BT45E);

    HHPAK(33000, (float*)Ngood);
    HHPAK(33010, (float*)Nback);
    HHPAK(33020, (float*)Ntotal);

    // Spin Sorted Strip Distribution  
    HHPAK(36000, (float*)NStrip[0]);
    HHPAK(36100, (float*)NStrip[1]);


    return 0;

} // end-of-CumulativeAsymmetry()






//
// Class name  :
// Method name : PrintWarning()
//
// Description : print warnings
// Input       : 
// Return      : 
//
void
PrintWarning(){

  extern StructStripCheck strpchk;
  extern StructBunchCheck bnchchk;

    printf("-----------------------------------------------------------------------------------------\n");
    printf("------------------------------  Error Detector Results ----------------------------------\n");
    printf("-----------------------------------------------------------------------------------------\n");
    printf("===> Bunch distribution \n");
    printf(" Good Bunch Rate Sigma Allowance     : %6.1f\n",errdet.BUNCH_RATE_SIGMA_ALLOWANCE);
    printf(" Good Bunch Asymmetry Sigma Allowance: %6.1f\n",errdet.BUNCH_ASYM_SIGMA_ALLOWANCE);
    printf(" Number of Problemeatic Bunches      : %d \n", anal.anomaly.nbunch);
    printf(" Problemeatic Bunch ID's             : ");
    for (int i=0; i<anal.anomaly.nbunch; i++) printf("%d ",anal.anomaly.bunch[i]+1) ; 
    printf("\n");
    printf(" Unrecognized Problematic Strips     : ");
    for (int i=0; i<anal.unrecog.anomaly.nbunch; i++) printf("%d ",anal.unrecog.anomaly.bunch[i]+1) ; 
    printf("\n");
    printf("===> Invariant Mass / strip \n");
    printf(" Maximum Mass Deviation [GeV]        : %6.2f   (%d)\n", strpchk.dev.max,  strpchk.dev.st);
    printf(" Maximum Mass fit chi-2              : %6.2f   (%d)\n", strpchk.chi2.max, strpchk.chi2.st);
    printf(" Maximum Mass-Energy Correlation     : %8.4f (%d)\n", strpchk.p1.max, strpchk.p1.st);
    printf(" Weighted Mean InvMass Sigma         : %6.2f \n", strpchk.average[0]);
    printf(" Good strip Mass-Energy Correlation  : %8.4f \n", strpchk.p1.allowance);
    printf(" Good Strip Mass Sigma Allowance[GeV]: %6.2f \n", strpchk.dev.allowance);
    printf(" Good Strip Mass Fit chi2 Allowance  : %6.2f \n", strpchk.chi2.allowance);
    printf(" Number of Problematic Strips        : %d \n", anal.anomaly.nstrip); 
    printf(" Problematic Strips                  : ");
    for (int i=0; i<anal.anomaly.nstrip; i++) printf("%d ", anal.anomaly.st[i]+1);
    printf("\n");
    printf(" Unrecognized Problematic Strips     : ");
    for (int i=0; i<anal.unrecog.anomaly.nstrip; i++) printf("%d ",anal.unrecog.anomaly.st[i]+1) ; 
    printf("\n");
    printf("-----------------------------------------------------------------------------------------\n");
    printf("\n\n");

    return;
}



//
// Class name  :
// Method name : PrintRunResults()
//
// Description : print analysis results and run infomation
// Input       : StructHistStat hstat
// Return      : 
//
void
PrintRunResults(StructHistStat hstat){

    runinfo.RunTime = runinfo.StopTime - runinfo.StartTime;
    runinfo.EvntRate = float(Nevtot)/float(runinfo.RunTime);
    runinfo.ReadRate = float(Nread)/float(runinfo.RunTime);

    printf("-----------------------------------------------------------------------------------------\n");
    printf("-----------------------------  Operation Messages  --------------------------------------\n");
    printf("-----------------------------------------------------------------------------------------\n");
    if (Flag.feedback)        printf(" Feedback mode     : On \n");
    if (Flag.spin_pattern>=0) printf(" RHIC Spin Pattern : Recovered.\n");
    if (Flag.mask_bunch)      printf(" Applied a mask to the fill pattern in the data stream.\n");
    printf("-----------------------------------------------------------------------------------------\n");
    printf("\n\n");

    printf("-----------------------------------------------------------------------------------------\n");
    printf("-----------------------------   Analysis Results   --------------------------------------\n");
    printf("-----------------------------------------------------------------------------------------\n");
    printf(" RunTime                 [s] = %10d\n",   runinfo.RunTime);
    printf(" Event Rate             [Hz] = %10.1f\n", runinfo.EvntRate);
    printf(" Read Rate              [Hz] = %10.1f\n", runinfo.ReadRate);
    printf(" Target                      =          %c\n",     runinfo.target);
    printf(" Target Operation            =      %s\n",     runinfo.TgtOperation);
    if (runinfo.Run>=6){
        printf(" Maximum Revolution #        = %10d\n", runinfo.MaxRevolution);
        printf(" Reconstructed Duration  [s] = %10.1f\n",runinfo.MaxRevolution/RHIC_REVOLUTION_FREQ);
        printf(" Target Motion Counter       = %10d\n",cntr.tgtMotion);
    }
    printf(" WCM Average                 = %10.1f\n", runinfo.WcmAve);
    printf(" WCM Average w/in rnge       = %10.1f\n", average.average);
    printf(" Specific Luminosity         = %10.2f%10.2f%10.4f\n",hstat.mean, hstat.RMS, hstat.RMSnorm);
    printf(" # of Filled Bunch           = %10d\n", NFilledBunch);
    printf(" bunch w/in WCM range        = %10d\n", average.counter);
    printf(" process rate                = %10.1f [%]\n",(float)average.counter/(float)NFilledBunch*100);
    printf(" Analyzing Power Average     = %10.4f \n", anal.A_N[1]);
    if (dproc.FEEDBACKMODE) 
      printf(" feedback average tshift     = %10.1f [ns]\n",anal.TshiftAve);
    printf(" Average Polarization        = %10.4f%9.4f\n",anal.P[0],anal.P[1]);
    printf(" Polarization (sinphi)       = %10.4f%9.4f\n",anal.sinphi.P[0],anal.sinphi.P[1]);
    printf(" Phase (sinphi)  [deg.]      = %10.4f%9.4f\n",anal.sinphi.dPhi[0]*R2D,anal.sinphi.dPhi[1]*R2D);
    printf(" chi2/d.o.f (sinphi fit)     = %10.4f\n",anal.sinphi.chi2);
    printf("-----------------------------------------------------------------------------------------\n");

    return;
}


//
// Class name  :
// Method name : WeightAnalysingPower()
//
// Description : Caluclate Energy Yeild weighted Analyzing power
//             : Histogram 34000 is filled only once, which is controled by CallFlag
//             : 
// Input       : int HID
// Return      : A_N
//
float
WeightAnalyzingPower(int HID){

    static int CallFlag=0;

      //----------------------------------------------------------------
    // analyzing power
    //
    // A_N values from L. Trueman (new for Run-04)
    //      t = e * 22.18 / 1000000.
    //      Emin = (0.0010-0.001/2.)*1e6/22.18 = 22.5
    //      Emax = (0.0255+0.001/2.)*1e6/22.18 = 1172.2

    float anth[25] = { // for 25 GeV
        0.0324757, 0.0401093, 0.0377283, 0.0339981, 0.0304917,
        0.0274323, 0.0247887, 0.0224906, 0.020473,  0.0186837,
        0.0170816, 0.0156351, 0.0143192, 0.0131145, 0.0120052,
        0.0109786, 0.0100245, 0.00913435, 0.00830108, 0.00751878,
        0.00678244, 0.00608782, 0.00543127, 0.00480969, 0.00422038};

    float anth100[25] = { // for 100 GeV
        0.0297964, 0.0372334, 0.0345393, 0.0306988, 0.0272192,
        0.0242531, 0.0217307, 0.0195634, 0.017677,  0.0160148,
        0.0145340, 0.0132021, 0.0119941, 0.0108907, 0.00987651,
        0.00893914, 0.00806877, 0.00725722, 0.00649782, 0.00578491,
        0.00511384, 0.00448062, 0.00388186, 0.00331461, 0.00277642};

    if (runinfo.BeamEnergy>50) {
        for (int i=0; i<25; i++) anth[i] = anth100[i] ;
    }


    float Emin = 22.5;
    float Emax = 1172.2;
    float DeltaE = (Emax - Emin)/25.;

    int nbin = HENEBIN;
    int fstbin = 1;
    int lstbin = HENEBIN;

    float X[HENEBIN];
    float Y[HENEBIN];
    float EX[HENEBIN];
    float EY[HENEBIN];

    hhrebin_(&HID, X, Y, EX, EY, &nbin, &fstbin, &lstbin);

    int j=0;
    float sum = 0.;
    float suma = 0.;
    for (int i=0;i<HENEBIN;i++){
        j = (int)((X[i] - Emin)/DeltaE);
        sum += Y[i];
        suma += Y[i]*anth[j];
        //printf("%d: Y %f AN %f X %f\n",i,Y[i],anth[j],X[i]);
    }

    float aveA_N = suma/sum;

    // Fill Analyzing power histogram only once.
    if (!CallFlag) HHF1(34000,0.5, aveA_N);
    CallFlag=1;

  return aveA_N;

}// End-of-WeightAnalyzingPower()




//
// Class name  :
// Method name : ExclusionList(int i, int j)
//
// Description : returns true if strip #j is in exclusion candidate
// Input       : int i, int j
// Return      : true/faulse
//
int
ExclusionList(int k, int j, int RHICBeam){

  int test=0;

  int i = k>35 ? k-36: k;
  if ((j==i)||(j==35-i)||(j==36+i)||(j==71-i)) test = 1;

  // This is because of dead channel 62. (Strip#63) in blue
  if (!RHICBeam) {
    if ((j==26)||(j==9)||(j==62)||(j==45)) test = 1;
  }

  return test;

}



//
// Class name  :
// Method name : calcAsymmetry
//
// Description : calculate Asymmetry
// Input       : int a, int b, int atot, int btot
// Return      : float Asym, float dAsym
//
int
calcAsymmetry(int a, int b, int atot, int btot, float &Asym, float &dAsym){

  float R = 0;
  float A = float(a);
  float B = float(b);
  float Atot = float(atot);
  float Btot = float(btot);
  
  if (Btot) R = Atot/Btot;
  if ((A+R*B)&&(A+B)){
    Asym  = (A-R*B) / (A+R*B);
    dAsym = sqrt(4*B*B*A + 4*A*A*B)/(A+B)/(A+B);
  }else{
    Asym = dAsym = 0;
  }

  return 0;
}



//
// Class name  : 
// Method name : WeightedMean(float A[N], float dA[N], int NDAT)
//
// Description : calculate weighted mean
// Input       : float A[N], float dA[N], int NDAT
// Return      : weighted mean
//
float
WeightedMean(float A[N], float dA[N], int NDAT){

  float sum1, sum2, dA2, WM;
  sum1 = sum2 = dA2 = 0;

  for ( int i=0 ; i<NDAT ; i++ ) {
    if (dA[i]){  // skip dA=0 data
      dA2 = dA[i]*dA[i];
      sum1 += A[i]/dA2 ;
      sum2 += 1/dA2 ;
    }
  }

  WM = dA2 == 0 ? 0 : sum1/sum2 ;
  return WM ;

} // end-of-WeightedMean()


//
// Class name  : 
// Method name : WeightedMeanError(float dA[N], int NDAT)
//
// Description : calculate weighted mean error 
// Input       : float dA[N], int NDAT
// Return      : weighted mean error
//
float
WeightedMeanError(float A[N], float dA[N], float Ave, int NDAT){

  float sum, dA2, dWM;
  sum = dA2 = dWM = 0;

  for ( int i=0 ; i<NDAT ; i++ ) {
    if (dA[i]){
      dA2 = dA[i]*dA[i];
      sum += 1/dA2 ;
    }
  }

  dWM = sum == 0 ? 0 : sqrt(1/sum);
  return dWM ;

} // end-of-WeightedMeanError()




//
// Class name  : 
// Method name : calcWeightedMean(float A[N], float dA[N], int NDAT, float &Ave, float &dAve)
//
// Description : call weighted mean and error 
// Input       : float A[N], float dA[N], float Ave, int NDAT
// Return      : Ave, dAve
//
void
calcWeightedMean(float A[N], float dA[N], int NDAT, float &Ave, float &dAve){

    Ave  = WeightedMean(A, dA, NDAT);
    dAve = WeightedMeanError(A,dA, Ave, NDAT);

  return ;

}




//
// Class name  : 
// Method name : SpecificLuminosity()
//
// Description : Handle Specific Luminosity
// Input       : Histograms 10033, 11033, 10034, 11034
// Return      : float &mean, float &RMS, float &RMS_norm
//
void 
SpecificLuminosity(float &mean, float &RMS, float &RMS_norm){


    //-------------------------------------------------------
    // Specific Luminosity 
    //-------------------------------------------------------
  int bid;
  float SpeLumi[120],dSpeLumi[120];
  float SpeLumi_norm[120], dSpeLumi_norm[120];

  for (bid=0; bid<120; bid++) {
    SpeLumi[bid] = wcmdist[bid] != 0 ? Ngood[bid]/wcmdist[bid] : 0 ;
    dSpeLumi[bid] = sqrt(SpeLumi[bid]);
  }
  HHPAK(10033, SpeLumi);    HHPAKE(11033, dSpeLumi);
  float ave = WeightedMean(SpeLumi,dSpeLumi,120);

  if (ave){
    for (bid=0; bid<120; bid++) {
      SpeLumi_norm[bid] = SpeLumi[bid]/ave;
      dSpeLumi_norm[bid] = dSpeLumi[bid]/ave;
    }
  }
  HHPAK(10034, SpeLumi_norm);    HHPAKE(11034, dSpeLumi_norm);


  // Book and fill histograms
  char hcomment[256];
  sprintf(hcomment,"Specific Luminosity");
  HHBOOK1(10035,hcomment,100,ave-ave/2,ave+ave/2.);
  for (bid=0;bid<120;bid++) HHF1(10035,SpeLumi[bid],1);

  // Get variables
  char CHOICE[5]="HIST";
  mean = HHSTATI(10035, 1, CHOICE, 0) ;
  RMS  = HHSTATI(10035, 2, CHOICE, 0) ;
  RMS_norm = (!mean) ? 0 : RMS/mean ;

  return;

}



//
// Class name  : 
// Method name : TshiftFinder
//
// Description : Find Time shift from 12C mass peak fit. Units are all in [GeV]
//             : ouotputs are converted to [keV]
// Input       : int Mode
//             : int FeedBackLevel 1) no fit, just max and mean
//             :                   2) gaussian fit
// Return      : average tshift [ns] @ 500keV
//
float
TshiftFinder(int Mode, int FeedBackLevel){

  char CHOICE[5]="HIST";
  const int np=3;
  float par[np], step[np], pmin[np], pmax[np], sigpar[np];
  char chfun[3]="G";
  char chopt[2]="Q";
  float chi2, hmax;
  float mdev,adev;
  float ex[NSTRIP];

  for (int st=0; st<NSTRIP; st++){
    feedback.strip[st]=st+1;
    ex[st]=0; feedback.err[st]=1; // initialization
    int hid= Mode ? 16200+st+1 : 17200+st+1 ;

    switch(FeedBackLevel){

    case 1:     // Level 1 Histogram Maximum and Mean

      feedback.mdev[st] = HHSTATI(hid, 1, CHOICE, 0) - MASS_12C*k2G;
      feedback.RMS[st]  = HHSTATI(hid, 2, CHOICE, 0) ;
      break;

    case 2:   // Level 2 Gaussian Fit

      // parameter initialization 
      par[0] = HHMAX(hid);         // amplitude
      par[1] = MASS_12C*k2G;       // mean [GeV]
      par[2] = dproc.OneSigma*k2G; // sigma [GeV]
      if (par[0]) { // Gaussian Fit unless histogram isn't empty
	HHFITHN(hid, chfun, chopt, np, par, step, pmin, pmax, sigpar, chi2); 
      }else{
	par[2] = feedback.err[st] = 0; // set weight 0
      }
      feedback.err[st]  = sigpar[1] * chi2 * chi2;
      feedback.mdev[st] = par[1] - MASS_12C*k2G; 
      feedback.RMS[st]  = par[2]; 
      feedback.chi2[st] = chi2;

      break;

    }//end-of-switch(FinderLevel)

  }//end-of-st loop


  if (Mode) {
    // Fill summary histograms
    HHPAK(16300,feedback.mdev);  
    HHPAKE(16300,feedback.err);


  } else {
    
    // RMS width mapping of 12C mass peak
    mass_sigma_vs_strip = new TGraphErrors(NSTRIP, feedback.strip, feedback.RMS, ex, feedback.err);
    mass_sigma_vs_strip -> SetTitle("Mass sigma vs. strip");
    mass_sigma_vs_strip -> SetMarkerStyle(20);
    mass_sigma_vs_strip -> SetMarkerSize(1.5);
    mass_sigma_vs_strip -> SetMarkerColor(7);
    mass_sigma_vs_strip -> SetDrawOption("P");
    mass_sigma_vs_strip -> GetYaxis() -> SetTitle("RMS Width of 12C Mass Peak[GeV]");
    mass_sigma_vs_strip -> GetXaxis() -> SetTitle("Strip Number");

    // Chi2 mapping of Gaussian fit on 12C mass peak
    mass_chi2_vs_strip = new TGraphErrors(NSTRIP, feedback.strip, feedback.chi2, ex, ex);
    mass_chi2_vs_strip -> SetTitle("Gauss Fit Mass chi2 vs. strip");
    mass_chi2_vs_strip -> SetMarkerStyle(20);
    mass_chi2_vs_strip -> SetMarkerSize(1.5);
    mass_chi2_vs_strip -> SetMarkerColor(7);
    mass_chi2_vs_strip -> SetDrawOption("P");
    mass_chi2_vs_strip -> GetYaxis() -> SetTitle("Chi2 of Gaussian Fit on 12C Mass Peak");
    mass_chi2_vs_strip -> GetXaxis() -> SetTitle("Strip Number");

    StripAnomalyDetector();
    
  }

  mdev = WeightedMean(feedback.mdev,feedback.err,NSTRIP);
  printf("Average Mass Deviation  = %10.2f [GeV/c]\n",mdev);
  adev = mdev*G2k*runconst.M2T/sqrt(400.);
  printf("Tshift Average @ 400keV = %10.2f [ns]\n", adev);

  // Unit Conversion [GeV] -> [keV]
  for (int i=0; i<NSTRIP; i++) {
    feedback.tedev[i] = feedback.mdev[i]*G2k*runconst.M2T;
    feedback.RMS[i] *= G2k;
  }

  return adev ;

}

//
// Class name  : 
// Method name : FillAsymmetryHistgram(char Mode[], int sign, int N, float y[])
//
// Description : Fill out bunch by bunch Asymmetry Histograms
//             : asym_bunch_x90, asym_bunch_x45, asym_bunch_y45
//             : These histograms are then applied Gaussian fit to check anomaly bunches
// Input       : char Mode[], int sign, int N, float A[], float bunch[]
// Return      : 
//
void
FillAsymmetryHistgram(char Mode[], int sign, int N, float A[], float bunch[]){

  for (int i=0; i<N; i++) { // loop for bunch number

    // flip the asymmetry sign for spin=-1 to be consistent with spin=+1
    A[i] *= sign; 

    // process only active bunches
    if (bunch[i] != -1){
      if (Mode=="x90") asym_bunch_x90->Fill(A[i]);
      if (Mode=="x45") asym_bunch_x45->Fill(A[i]);
      if (Mode=="y45") asym_bunch_y45->Fill(A[i]);
    }

  }

  return ;

}


//
// Class name  : 
// Method name : AsymmetryGraph()
//
// Description : Define net TGraphErrors object asymgraph for vectors x,y,ex,ey
//             : specifies marker color based on mode
//             : positive spin : blue
//             : negative spin : red
// Input       : int Mode, int N, float x[], float y[], float ex[], float ey[]
// Return      : TGraphErrors * asymgraph
//
TGraphErrors * 
AsymmetryGraph(int Mode, int N, float x[], float y[], float ex[], float ey[]){

  int Color= Mode == 1 ? 4 : 2;
  TGraphErrors * asymgraph = new TGraphErrors(N, x, y, ex, ey);
  asymgraph -> SetMarkerStyle(20);
  asymgraph -> SetMarkerSize(1.2);
  asymgraph -> SetMarkerColor(Color);

  return asymgraph ;

}


//
// Class name  : 
// Method name : calcBunchAsymmetry
//
// Description : call BunchAsymmetry to calculate asymmetries bunch by bunch and
//             : Fill out asym_vs_bunch_x90, x45, y45 histograms
// Input       : 
// Return      : 
//
int
calcBunchAsymmetry(){

  // calculate Bunch Asymmetries for x45, x90, y45
    BunchAsymmetry(0,  basym.Ax90[0], basym.Ax90[1]);
    BunchAsymmetry(1,  basym.Ax45[0], basym.Ax45[1]);
    BunchAsymmetry(2,  basym.Ay45[0], basym.Ay45[1]);

    // Define TH2F histograms first
    Asymmetry->cd();
    char htitle[100];
    float min, max;
    float margin=0.2;
    float prefix=0.028;
    sprintf(htitle,"Run%8.3f : Raw Asymmetry X90", runinfo.RUNID);
    GetMinMaxOption(prefix, NBUNCH, basym.Ax90[0], margin, min, max);
    asym_vs_bunch_x90 = new TH2F("asym_vs_bunch_x90", htitle, 100, 0, NBUNCH+1, 100, min, max);
    DrawLine(asym_vs_bunch_x90, 0, NBUNCH, 0, 1, 1, 1);

    sprintf(htitle,"Run%8.3f : Raw Asymmetry X45", runinfo.RUNID);
    GetMinMaxOption(prefix, NBUNCH, basym.Ax45[0], margin, min, max);
    asym_vs_bunch_x45 = new TH2F("asym_vs_bunch_x45", htitle, 100, 0, NBUNCH+1, 100, min, max);
    DrawLine(asym_vs_bunch_x45, 0, NBUNCH, 0, 1, 1, 1);

    sprintf(htitle,"Run%8.3f : Raw Asymmetry Y45", runinfo.RUNID);
    GetMinMaxOption(prefix, NBUNCH, basym.Ay45[0], margin, min, max);
    asym_vs_bunch_y45 = new TH2F("asym_vs_bunch_y45", htitle, 100, 0, NBUNCH+1, 100, min, max);
    DrawLine(asym_vs_bunch_y45, 0, NBUNCH, 0, 1, 1, 1);

    // fill bunch ID array [1 - NBUNCH], not [0 - NBUNCH-1]
    float bunch[NBUNCH], ex[NBUNCH];
    for (int bid=0; bid<NBUNCH; bid++) { ex[bid]=0; bunch[bid]= bid+1; }

    // Superpose Positive/Negative Bunches arrays into TH2F histograms defined above
    TGraphErrors * asymgraph ;
    for (int spin=1; spin>=-1; spin-=2 ) {

      // Selectively disable bunch ID by matching spin pattern
      for (int bid=0; bid<NBUNCH; bid++) { bunch[bid]= spinpat[bid] == spin ? bid+1 : -1 ;}

      // X90 
      asymgraph = AsymmetryGraph(spin, NBUNCH, bunch, basym.Ax90[0], ex, basym.Ax90[1]);
      FillAsymmetryHistgram("x90", spin, NBUNCH, basym.Ax90[0], bunch);
      asym_vs_bunch_x90 -> GetListOfFunctions() -> Add(asymgraph,"p");
      asym_vs_bunch_x90 -> GetXaxis()->SetTitle("Bunch Number");
      asym_vs_bunch_x90 -> GetYaxis()->SetTitle("Raw Asymmetry ");

      // X45 
      asymgraph = AsymmetryGraph(spin, NBUNCH, bunch, basym.Ax45[0], ex, basym.Ax45[1]);
      FillAsymmetryHistgram("x45", spin, NBUNCH, basym.Ax45[0], bunch);
      asym_vs_bunch_x45 -> GetListOfFunctions() -> Add(asymgraph,"p");
      asym_vs_bunch_x45 -> GetXaxis()->SetTitle("Bunch Number");
      asym_vs_bunch_x45 -> GetYaxis()->SetTitle("Raw Asymmetry ");

      // Y45 
      asymgraph = AsymmetryGraph(spin, NBUNCH, bunch, basym.Ay45[0], ex, basym.Ay45[1]);
      FillAsymmetryHistgram("y45", spin, NBUNCH, basym.Ay45[0], bunch);
      asym_vs_bunch_y45 -> GetListOfFunctions() -> Add(asymgraph,"p");
      asym_vs_bunch_y45 -> GetXaxis()->SetTitle("Bunch Number");
      asym_vs_bunch_y45 -> GetYaxis()->SetTitle("Raw Asymmetry ");

    }// end-of-for(spin) loop


  return 0;

} // end-of-calcBunchAsymmetry()



//
// Class name  : 
// Method name : BunchAsymmetry
//
// Description : calculate asymmetries bunch by bunch
// Input       : int Mode0[Ax90], 1[Ax45], 2[Ay45]
// Return      : Asym[NBUNCH], dA[NBUNCH]
//
int
BunchAsymmetry(int Mode, float A[], float dA[]){


  // Allocate adequate detector IDs involved in X90,X45,Y45, respectively
  int Rdet[2], Ldet[2];
  switch (Mode) {
  case 0:  // Ax90
    Rdet[0]=1; Ldet[0]=4;
    break; 
  case 1:  // Ax45
    Rdet[0]=0; Rdet[1]=2; Ldet[0]=3; Ldet[1]=5;
    break; 
  case 2:  // Ay45
    Rdet[0]=0; Rdet[1]=5; Ldet[0]=2; Ldet[1]=3;
    break;
  default:
    cerr << "BunchAsymmetry: No muching mode is coded in for " << Mode << endl;
    return -1;
  }

  // Calculate detector luminosities by taking sum over R/L detectors and all bunches
  long int LumiR = 0;   long int LumiL = 0;
  for (int i=0;i<2;i++) { // run for 2 for X45 and Y45
    for (int bid=0;bid<NBUNCH;bid++) {
      LumiR += Ncounts[Rdet[i]][bid] ; 
      LumiL += Ncounts[Ldet[i]][bid] ;
    }
    if (!Mode) break; // no R/L detector loop for x90
  }


  // Main bunch loop to calculate asymmetry bunch by bunch
  for (int bid=0;bid<NBUNCH;bid++){

    // Take sum of Up/Down for Right and Left detectors
    int R = 0; int L = 0; A[bid] = dA[bid] = 0;
    for (int i=0; i<2; i++) {
      R += Ncounts[Rdet[i]][bid];
      L += Ncounts[Ldet[i]][bid];
      if (!Mode) break; // no detector loop for X90
    }
    calcAsymmetry(R,L,LumiR,LumiL,A[bid],dA[bid]);

  } // end-of-bid loop


  return 0;

}



//
// Class name  : 
// Method name : CalcStripAsymmetry
//
// Description : calculate asymmetries strip by strip
// Input       : aveA_N
// Return      : 
//
void
CalcStripAsymmetry(float aveA_N){

    //-------------------------------------------------------
    // Strip-by-Strip Asymmetries
    //-------------------------------------------------------

    int LumiSum[2][72]; // Total Luminosity [0]:Spin Up, [1]:Spin Down
    float LumiSum_r[2][72];//Reduced order Total luminosity for histograming
    float LumiRatio[72]; // Luminosity Ratio
    float Asym[72], dAsym[72]; // Raw Asymmetries strip-by-strip
    float P[72], dP[72]; // phi corrected polarization 
    float P_phi[62830],dP_phi[62830]; // phi corrected polarization with extended array
    float Pt[72], dPt[72]; // phi Trancated corrected polarization,

    for (int i=0;i<62830;i++) P_phi[i]=dP_phi[i]=0;


    for (int i=0; i<72; i++) {
      Asym[i] = dAsym[i] = RawP[i] = dRawP[i] = LumiSum_r[0][i] = LumiSum_r[0][i] = LumiRatio[i] = 0;
      LumiSum[0][i] = LumiSum[1][i] = 0;

      // Loop for Total Luminosity
      for (int j=0; j<72; j++) {

	// Calculate Luminosity. Own Strip and ones in cross geometry are excluded.
	if (!ExclusionList(i,j,runinfo.RHICBeam)){
	  for (int k=0; k<=1; k++) LumiSum[k][i]+=NStrip[k][j];
	}

      } // end-of-j-loop. 


      // Luminosity Ratio
      LumiRatio[i] = (float)LumiSum[0][i]/(float)LumiSum[1][i];
      // Calculate Raw Asymmetries for strip-i
      if ((LumiSum[1][i]) && ((NStrip[0][i]+NStrip[1][i])))
	calcAsymmetry(NStrip[0][i], NStrip[1][i], LumiSum[0][i], LumiSum[1][i], Asym[i], dAsym[i]);

      // Reduced Order Luminosity for histograms. Histogram scale is given in float, not double.
      // Cannot accomomdate large entry.
      LumiSum_r[0][i] = LumiSum[0][i]/1e3;
      LumiSum_r[1][i] = LumiSum[1][i]/1e3;

      // Since this is the recoil asymmetries, flip the sign of asymmetry
      Asym[i]*=-1;
      
      // Raw polarization without phi angle weighted A_N
      RawP[i]  =  Asym[i] / aveA_N;
      dRawP[i] = dAsym[i] / aveA_N;

      // Polarization with sin(phi) correction
      P[i]  = RawP[i] / sin(-phi[i]);
      dP[i] = fabs(dRawP[i] / sin(-phi[i]));

      // Dump Polarization to phi array
      int j = int(phi[i]*1e4);
      P_phi[j] = RawP[i];
      dP_phi[j]= dRawP[i];
      // Polarization with trancated sin(phi) correction
      Pt[i]  = RawP[i] / sin(-phit[i]);
      dPt[i] = fabs(dRawP[i] / sin(-phit[i]));



    } // end-of-i-loop


    // printing routine
    if (Flag.VERBOSE){
        printf("*========== strip by strip =============\n");
        for (int i=0;i<NSTRIP; i++){
            printf("%4d",i);
            printf("%7.3f", phi[i]);
            printf("%12.3e%12.3e", Asym[i],dAsym[i]);
            printf("%12.3e%12.3e", RawP[i],dRawP[i]);
            printf("%12.3e%12.4e",    P[i],   dP[i]);
            printf("%12.3e%12.4e",   Pt[i],  dPt[i]);
            printf("\n");
        }
        printf("*=======================================\n");
        printf("\n");
    } //end-of-if(Flag.VERBOSE)



    // Caluclate Weighted Average
    calcWeightedMean(P, dP, 72, anal.P[0], anal.P[1]);


    // Histrograming
    HHPAK(36010, LumiSum_r[0]);  HHPAK(36110, LumiSum_r[1]); 
    HHPAK(36200, LumiRatio); 
    HHPAK(36210, Asym);  HHPAKE(36210, dAsym);
    HHPAK(36220, RawP);  HHPAKE(36220, dRawP);
    HHPAK(36230, P_phi); HHPAKE(36230, dP_phi);
    HHPAK(36240, P);     HHPAKE(36240, dP);
    HHPAK(36250, phi); 

    // Fit phi-distribution
    AsymFit asymfit;
    asymfit.SinPhiFit(anal.P[0], RawP, dRawP, phi, anal.sinphi.P, anal.sinphi.dPhi, anal.sinphi.chi2);
    //asymfit.SinPhiFit(anal.P[0], anal.sinphi.P, anal.sinphi.dPhi, anal.sinphi.chi2);

    return;

}


//
// Class name  : 
// Method name : sin_phi(Float_t x, Double_t *par)
//
// Description : sin(x) fit. Amplitude and phase as parameters
// Input       : Double_t *x, Double_t *par
// Return      : par[0]*sin(x+par[1])
//
Double_t
sin_phi(Double_t *x, Double_t *par)
{
 Double_t value=par[0]*sin(-x[0]+par[1]);
 return value;
}



//
// Class name  : AsymFit
// Method name : SinPhiFit()
//
// Description : Master Routine for sin(phi) root-fit  
// Input       : Float_t p0 (1-par Polarization for par[0] initialization)
//             : Float_t *RawP, Float_t *dRawP, Float_t *phi (vectors to be fit)
// Return      : Float_t *P, Float_t *dphi, Float_t &chi2dof
//
void
AsymFit::SinPhiFit(Float_t p0, Float_t *RawP, Float_t *dRawP, Float_t *phi, 
		   Float_t *P, Float_t *dphi, Float_t &chi2dof)
{
  
  float dx[NSTRIP];
  for ( int i=0; i<NSTRIP; i++) dx[i] = 0;

  asym_sinphi_fit = new TGraphErrors(NSTRIP, phi, RawP, dx, dRawP);
  asym_sinphi_fit->SetTitle("sin(phi) fit");
  asym_sinphi_fit->GetXaxis()->SetTitle("phi [deg.]");
  asym_sinphi_fit->GetYaxis()->SetTitle("Asymmetry * A_N [%]");
  asym_sinphi_fit->SetMarkerStyle(20);
  asym_sinphi_fit->SetMarkerSize(1.5);
  asym_sinphi_fit->SetLineWidth(2);
  asym_sinphi_fit->SetMarkerColor(2);
  asym_sinphi_fit->SetDrawOption("P");

  // define sin(phi) fit function & initialize parmaeters
  TF1 *func = new TF1("sin_phi", sin_phi, 0, 2*M_PI, 2);
  func -> SetParameters(p0,0);
  func -> SetParLimits(0, -1, 1);
  func -> SetParLimits(1, -M_PI, M_PI);
  func -> SetParNames("P","dPhi");
  func -> SetLineColor(2);

  TText * txt = new TText(1,1,"sin(phi) fit");
  asym_sinphi_fit->GetListOfFunctions()->Add(txt);

  // Perform sin(phi) fit
  asym_sinphi_fit -> Fit("sin_phi","R");
    
  // Get fitting results
  P[0] = func->GetParameter(0);
  P[1] = func->GetParError(0);
  dphi[0] = func->GetParameter(1);
  dphi[1] = func->GetParError(1);

  // calculate chi2
  chi2dof = func->GetChisquare()/func->GetNDF();

  return ;

}// end-of-AsymFit::SinPhiFit()



// Return Maximum from array A[N]
float GetMax(int N, float A[]){
  float max = A[0];
  for (int i=1; i<N; i++) max = (A[i])&&(max<A[i]) ? A[i] : max;
  return max;
}

// Return Miminum from array A[N]
float GetMin(int N, float A[]){
  float min = A[0];
  for (int i=1; i<N; i++) min = (A[i])&&(min>A[i]) ? A[i] : min;
  return min;
}

// Return Minimum and Maximum from array A[N]
void GetMinMax(int N, float A[], float margin, float &min, float &max){
  min = GetMin(N,A);
  max = GetMax(N,A);
  min -= fabs(min)*margin;
  max += fabs(max)*margin;
  return ;
}

// Return Minimum and Maximum from array A[N]. Same as GetMinMax() function. But
// GetMinMaxOption takes prefix value which forces min, max to be prefix when the
// absolute min,max are smaller than prefix.
void GetMinMaxOption(float prefix, int N, float A[], float margin, float &min, float &max){
  GetMinMax(N, A, margin, min, max);
  if ( fabs(min)<prefix ) min = -prefix;
  if ( fabs(max)<prefix ) max =  prefix;
  return ;
}





/*       following 3 subroutines are unsuccessful MINUIT sin(phi) fit routines.  

//
// Class name  : 
// Method name : fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
//
// Description : Function returns chi2 for MINUIT
// Input       : 
// Return      : chi2
//
void 
fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{

  AsymFit asymfit;
   //calculate chisquare
   Float_t chisq = 0;
   Float_t delta;
   for (Int_t i=0;i<NSTRIP; i++) {
     if (dRawP[i]) delta  = (RawP[i]-asymfit.sinx(phi[i],par))/dRawP[i];
     chisq += delta*delta;
   }

   FitChi2 = chisq;
   f = chisq;
   
}


//
// Class name  : AsymFit
// Method name : sinx(Float_t x, Double_t *par)
//
// Description : sin(x) fit. Amplitude and phase as parameters
// Input       : Float_t x, Double_t *par
// Return      : par[0]*sin(x+par[1])
//
Float_t
AsymFit::sinx(Float_t x, Double_t *par)
{
 Float_t value=par[0]*sin(-x+par[1]);
 return value;
}

//
// Class name  : AsymFit
// Method name : SinPhiFit()
//
// Description : Master Routine for MINUIT call.
// Input       : Float_t p0 (1-par Polarization for par[0] initialization)
// Return      : Float_t *P, Float_t *phi, Float_t &chi2dof
//
void 
AsymFit::SinPhiFit(Float_t p0, Float_t *P, Float_t *phi, Float_t &chi2dof)
{
  

  const Int_t NPAR=2;
  TMinuit *gMinuit = new TMinuit(NPAR);  
  gMinuit->SetFCN(fcn);


   Double_t arglist[10];
   Int_t ierflg = 0;

   // By default, get rid of MINUIT busy messages
   if (!Flag.VERBOSE) {
     arglist[0]=-1;
     gMinuit->mnexcm("SET PRINT", arglist, 1, ierflg);
     arglist[0]=1;
     gMinuit->mnexcm("SET NOWarnings", arglist, 1, ierflg);
   }

// Initiarize par[0]=p0, par[1]=0
   static Double_t vstart[2] = {p0, 0.};
   static Double_t step[4] = {0.01 , 0.01};
   gMinuit->mnparm(0, "P",   vstart[0], step[0], 0,0,ierflg);
   gMinuit->mnparm(1, "phi", vstart[1], step[1], 0,0,ierflg);

// Now ready for minimization step
   arglist[0] = 1000; // call at least 1000 function calls
   arglist[1] = 0.01;  // tolerance
   gMinuit->mnexcm("MIGRAD", arglist ,2,ierflg);
   gMinuit->mnexcm("MINOS", arglist ,2,ierflg);


// Print results
   Double_t amin,edm,errdef;
   Int_t nvpar,nparx,icstat;
   gMinuit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
   //   if (!Flag.VERBOSE)  
   gMinuit->mnprin(3,amin);


   // Get Fitting Results
   Double_t val,eval,bnd1,bnd2;
   Int_t ivarbl;
   TString CHNAM;
   gMinuit->mnpout(0,CHNAM,val,eval,bnd1,bnd2,ivarbl);
   P[0]=(Float_t)val;
   //   P[1]=(Float_t)eval;
   P[1]=(Float_t)GetFittingErrors(gMinuit, 0);
   gMinuit->mnpout(1,CHNAM,val,eval,bnd1,bnd2,ivarbl);
   phi[0]=(Float_t)val;
   //   phi[1]=(Float_t)eval;
   phi[1]=(Float_t)GetFittingErrors(gMinuit, 1);


   Int_t NDATA=0;
   for (Int_t i=0;i<NSTRIP; i++) NDATA += dRawP[i] ? 1 : 0;
   chi2dof = FitChi2/Float_t(NDATA-NPAR);

   return;

}



//
// Class name  : AsymFit
// Method name : GetFittingErrors(TMinuit *gMinuit, Int_t NUM)
//
// Description : Retrun parameter errors for parameter NUM
// Input       : TMinuit *gMinuit, Int_t NUM
// Return      : error
//
Double_t 
AsymFit::GetFittingErrors(TMinuit *gMinuit, Int_t NUM){

  Double_t error;
  Double_t eplus, eminus, eparab, globcc;
  gMinuit->mnerrs(NUM,eplus,eminus,eparab,globcc);

  Int_t NoAverage = fabs(eplus)*fabs(eminus) ? 0 : 1;
  if (NoAverage) {
    error = eplus ? fabs(eplus) : fabs(eminus);
  }else{
    error =(fabs(eplus)+fabs(eminus))/2.;
  }

  cout << eplus << " " << eminus << " " << eparab << " " << globcc << " " << error << endl;

  return error;

}


*/


/*
//
// Class name  : RAMP
// Method name : CalcRAMP()
//
// Description : Not being use. 
// Input       : 
// Return      : 
//
void 
RAMP::CalcRAMP()
{

  if (dproc.RAMPMODE==1) {

    for (int dlm=0;dlm<RAMPTIME;dlm++){
      // not need for initialization for RUN,RD,LU,LD
      // they are initialized at bid=0
      memset(SIU,0,sizeof(SIU));
      memset(SID,0,sizeof(SID));
            
      for (bid=0;bid<120;bid++){
	for (si=0;si<6;si++){
	  SIU[si] += (NRcounts[si][bid][dlm])
	    *((spinpat[bid]==1)?1:0)*gbid[bid];
	  SID[si] += (NRcounts[si][bid][dlm])
	    *((spinpat[bid]==-1)?1:0)*gbid[bid];
	}
      }

      // fill the histograms
      for (si=0; si<6; si++) {
	HHF1(21000+si, (float)dlm, SIU[si]);
	HHF1(21100+si, (float)dlm, SID[si]);
      }
      
    }            
  }

  return;

}
*/


			
