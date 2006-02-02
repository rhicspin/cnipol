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
#include "rhicpol.h"
#include "rpoldata.h"
#include "Asym.h"
#include "WeightedMean.h"

extern void HHBOOK1(int hid, char* hname, int xnbin, float xmin, float xmax) ;
extern void HHPAK(int, float*);
extern void HHPAKE(int, float*);
extern void HHF1(int, float, float);
//extern void HHKIND(int, int*, char*);
extern float HHMAX(int);
extern float HHSTATI(int hid, int icase, char * choice, int num);
extern void HHFITHN(int hid, char*chfun, char*chopt, int np, float*par, 
	float*step, float*pmin, float*pmax, float*sigpar, float&chi2);


// =========================
// End of data process
// =========================
int end_process(recordConfigRhicStruct *cfginfo)
{
  if (Flag.feedback){

    //-------------------------------------------------------
    //                Feedback Mode
    //-------------------------------------------------------
    int FeedBackLevel=2; // 1: no fit just max and mean  
                         // 2: gaussian fit
    analysis.TshiftAve = TshiftFinder(FeedBackLevel);
    printf("Tshift Average @ 500keV = %10.1f [ns]\n", analysis.TshiftAve);

    // reset counters
    Nevtot = Nread = 0;
    for (int i=0;i<120;i++) Ntotal[i] = 0;
    return 0;

  }else{

    //-------------------------------------------------------
    //                Normal Mode
    //-------------------------------------------------------
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
        for (i=0; i<25; i++) anth[i] = anth100[i] ;
    }


    float Emin = 22.5;
    float Emax = 1172.2;
    float DeltaE = (Emax - Emin)/25.;

    int hid = 10040;
    int nbin = HENEBIN;
    int fstbin = 1;
    int lstbin = HENEBIN;

    hhrebin_(&hid, X, Y, EX, EY, &nbin, &fstbin, &lstbin);
        
    float sum = 0.;
    float suma = 0.;
    for (i=0;i<HENEBIN;i++){
        j = (int)((X[i] - Emin)/DeltaE);
        sum += Y[i];
        suma += Y[i]*anth[j];
        //printf("%d: Y %f AN %f X %f\n",i,Y[i],anth[j],X[i]);
    }
    HHF1(34000,0.5, suma/sum);

    fprintf (stdout,"ANALYZING POWER (FIT) = %f\n",suma/sum);

    float aveA_N = suma/sum;


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



    //-------------------------------------------------------
    // Specific Luminosity 
    //-------------------------------------------------------
    StructHistStat hstat;
    SpecificLuminosity(hstat.mean, hstat.RMS, hstat.RMSnorm);


    //-------------------------------------------------------
    // Strip-by-Strip Asymmetries
    //-------------------------------------------------------

    int LumiSum[2][72]; // Total Luminosity [0]:Spin Up, [1]:Spin Down
    float LumiSum_r[2][72];//Reduced order Total luminosity for histograming
    float LumiRatio[72]; // Luminosity Ratio
    float Asym[72], dAsym[72]; // Raw Asymmetries strip-by-strip
    float RawP[72], dRawP[72]; // Raw Polarization (Not corrected for phi)
    float P[72], dP[72]; // phi corrected polarization 
    float P_phi[62830],dP_phi[62830]; // phi corrected polarization with extended array
    float Pt[72], dPt[72]; // phi Trancated corrected polarization,

    printf("*========== strip by strip =============\n");

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
      P[i]  = RawP[i] / sin(phiRun5[i])*(-1);
      dP[i] = fabs(dRawP[i] / sin(phiRun5[i]));

      // Dump Polarization to phi array
      int j = int(phiRun5[i]*1e4);
      P_phi[j] = RawP[i];
      dP_phi[j]= dRawP[i];

      // Polarization with trancated sin(phi) correction
      Pt[i]  = RawP[i] / sin(phiRun5t[i]);
      dPt[i] = fabs(dRawP[i] / sin(phiRun5t[i]));

      printf("%4d",i);
      printf("%7.3f", phiRun5[i]);
      printf("%12.3e%12.3e", Asym[i],dAsym[i]);
      printf("%12.3e%12.3e", RawP[i],dRawP[i]);
      printf("%12.3e%12.4e",    P[i],   dP[i]);
      printf("%12.3e%12.4e",   Pt[i],  dPt[i]);
      printf("\n");

    } // end-of-i-loop

    printf("*=======================================\n");
    printf("\n");

    // Caluclate Weighted Average
    float P_ave, dP_ave;
    P_ave = dP_ave = 0;
    calcWeightedMean(P, dP, 72, P_ave, dP_ave);

    // Histrograming
    HHPAK(36010, LumiSum_r[0]);  HHPAK(36110, LumiSum_r[1]); 
    HHPAK(36200, LumiRatio); 
    HHPAK(36210, Asym);  HHPAKE(36210, dAsym);
    HHPAK(36230, P_phi); HHPAKE(36230, dP_phi);
    HHPAK(36240, P);     HHPAKE(36240, dP);
    HHPAK(36250, phiRun5); 


    //-------------------------------------------------------
    // Run Informations
    //-------------------------------------------------------

    runinfo.RunTime = runinfo.StopTime - runinfo.StartTime;
    runinfo.EvntRate = float(Nevtot)/float(runinfo.RunTime);
    runinfo.ReadRate = float(Nread)/float(runinfo.RunTime);

    printf("-----------------------------------------------\n");
    printf(" RunTime                     = %10d\n", runinfo.RunTime);
    printf(" Event Rate                  = %10.1f\n", runinfo.EvntRate);
    printf(" Read Rate                   = %10.1f\n", runinfo.ReadRate);
    printf(" WCM Average                 = %10.1f\n", runinfo.WcmAve);
    printf(" WCM Average w/in rnge       = %10.1f\n", average.average);
    printf(" Specific Luminosity         = %10.2f%10.2f%10.4f\n", hstat.mean, hstat.RMS, hstat.RMSnorm);
    printf(" # of Filled Bunch           = %10d\n", NFilledBunch);
    printf(" bunch w/in WCM range        = %10d\n", average.counter);
    printf(" process rate                = %10.1f [%]\n",(float)average.counter/(float)NFilledBunch*100);
    if (dproc.FEEDBACKMODE) 
      printf(" feedback average tshift     = %10.1f [ns]\n",analysis.TshiftAve);
    printf(" Average Polarization        = %10.4f%8.4f\n",P_ave,dP_ave);
    printf("-----------------------------------------------\n");



    //-------------------------------------------------------
    // RAMP MEASUREMENT 
    //-------------------------------------------------------
    /*if (dproc.RAMPMODE==1) {

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
        }*/

    return(0);

  }//end-of-if(Flag.feedback)

}


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

  float A = float(a);
  float B = float(b);
  float Atot = float(atot);
  float Btot = float(btot);

  float R = Atot/Btot;
  Asym  = (A-R*B) / (A+R*B);
  dAsym = sqrt(4*B*B*A + 4*A*A*B)/(A+B)/(A+B);

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
// Input       : int FeedBackLevel 1) no fit, just max and mean
//             :                   2) gaussian fit
// Return      : average tshift [ns] @ 500keV
//
float
TshiftFinder(int FeedBackLevel){

  char CHOICE[5]="HIST";
  const int np=3;
  float par[np], step[np], pmin[np], pmax[np], sigpar[np];
  char chfun[3]="G";
  char chopt[2]="Q";
  float chi2, hmax;
  float err[NSTRIP];

  for (int st=0; st<72; st++){
    err[st]=1; // initialization
    int hid=16200+st+1;

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
	HHFITHN(16200+st+1, chfun, chopt, np, par, step, pmin, pmax, sigpar, chi2); 
      }else{
	par[1] = par[2] = err[st] = 0; // set weight 0
      }
      err[st] = sigpar[1];
      feedback.mdev[st] =  par[1] - MASS_12C*k2G; 
      feedback.RMS[st]  =  par[2]; 
      break;

    }//end-of-switch(FinderLevel)

  }//end-of-st loop

  HHPAK(16300,feedback.mdev);  HHPAKE(16300,err);
  float adev = WeightedMean(feedback.mdev,err,72)*G2k*runconst.M2T/sqrt(500);

    // Unit Conversion [GeV] -> [keV]
    for (int i=0; i<NSTRIP; i++) {
      feedback.mdev[i] *= (G2k*runconst.M2T);
      feedback.RMS[i] *= G2k;
    }

    return adev;

}
