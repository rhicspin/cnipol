//  Asymmetry Analysis of RHIC pC Polarimeter
//  End-of-Run routine
//
//  Authors   :   Itaru Nakagawa
//                Dmitri Smirnov
//
//  Creation  :   01/21/2006
//

#include "AsymCalculator.h"

#include <math.h>

#include "AsymHbook.h"
#include "AsymRoot.h"
#include "AnaInfo.h"
#include "AnaResult.h"
#include "ChannelCalib.h"
#include "RunInfo.h"
#include "TargetInfo.h"
#include "MseRunInfo.h"

using namespace std;

float RawP[72], dRawP[72]; // Raw Polarization (Not corrected for phi)


// End of data process
void end_process(MseRunInfoX &run)
{ //{{{
   //if (gAnaInfo->HasAlphaBit()) {
   //   //gAsymRoot->PostProcess();
   //}

   gSystem->Info("   ::end_process", "Starting...");

   // Feedback Mode
   if (Flag.feedback) {

      int FeedBackLevel = 2; // 1: no fit just max and mean
                             // 2: gaussian fit
      gAnaResult->TshiftAve = TshiftFinder(Flag.feedback, FeedBackLevel);

      // reset counters
      Nevtot = Nread = 0;
      for (int i=0; i<N_BUNCHES; i++) Ntotal[i] = 0;
   }

   // Calculate Statistics
   CalcStatistics();

   // Skip end-of-run analysis routines for deadlayer and calibration modes.
   //if (!gAnaInfo->DMODE) {

   // Energy Yeild Weighted Average Analyzing Power
   //gAnaResult->A_N[0] = AsymCalculator::WeightAnalyzingPower(10040); // no cut in energy spectra
   gAnaResult->A_N[1] = AsymCalculator::WeightAnalyzingPower(10050); // banana cut in energy spectra

   if (!gAnaResult->A_N[1]) {
      printf("Zero analyzing power\n");
      return;
   }

   //CumulativeAsymmetry();

   // Specific Luminosity (ds: this one is not used in asymmetry calculation)
   SpecificLuminosity(gHstat.mean, gHstat.RMS, gHstat.RMSnorm);

   // Strip-by-Strip Asymmetries
   StripAsymmetry(run);

   //gAsymCalculator.

   // ds: these routines are skipped by default
   if (Flag.EXE_ANOMALY_CHECK) {

      // Check for 12C Invariant Mass and energy dependences
      TshiftFinder(Flag.feedback, 2);

      // Check for slope of Energy Spectrum
      DetectorAnomaly();

      // Check for bunch anomaly
      BunchAnomalyDetector();
   }

   // Complete Histograms
   CompleteHistogram();

   // Draw polarization vs target position
   DrawPlotvsTar();
   //}

   // Run Information
   PrintWarning();
   PrintRunResults();

   gSystem->Info("   ::end_process", "End");
} //}}}


// Method name : CompleteHistograms()
// Description : Complete dressing histograms
void CompleteHistogram()
{ //{{{
   // Draw reg./alt. event selection borders in Invariant Mass plots
   float MASS_12C_k2G = MASS_12C*k2G;

   for (int i=0; i<N_SILICON_CHANNELS; i++) {

      float max = mass_nocut[i]->GetMaximum();

      DrawVertLine(mass_nocut[i], MASS_12C_k2G,                                              max*1.05, 14, 2);
      DrawVertLine(mass_nocut[i], MASS_12C_k2G + feedback.RMS[i] * k2G * gAnaInfo->MassSigma,    max*0.3,   4, 2);
      DrawVertLine(mass_nocut[i], MASS_12C_k2G - feedback.RMS[i] * k2G * gAnaInfo->MassSigma,    max*0.3,   4, 2);
      DrawVertLine(mass_nocut[i], MASS_12C_k2G + feedback.RMS[i] * k2G * gAnaInfo->MassSigma,    max*0.3,   4, 2);
      DrawVertLine(mass_nocut[i], MASS_12C_k2G - feedback.RMS[i] * k2G * gAnaInfo->MassSigma,    max*0.3,   4, 2);
      DrawVertLine(mass_nocut[i], MASS_12C_k2G + feedback.RMS[i] * k2G * gAnaInfo->MassSigmaAlt, max*0.3,   4, 1);
      DrawVertLine(mass_nocut[i], MASS_12C_k2G - feedback.RMS[i] * k2G * gAnaInfo->MassSigmaAlt, max*0.3,   4, 1);
   }
} //}}}


// Method name : CumulativeAsymmetry(){
// Description : Calculate bunch cumulative asymmetries
void CumulativeAsymmetry()
{ //{{{
   asymStruct x90[120];  // x90[119] is total
   asymStruct x45[120];
   asymStruct y45[120];
   asymStruct cr45[120];
   asymStruct tx90[120][6];
   asymStruct tx45[120][6];
   asymStruct ty45[120][6];
   float RL90[120], RL90E[120];
   float RL45[120], RL45E[120];
   float BT45[120], BT45E[120];
   float NL, NR;
   float tmpasym, tmpasyme;
   float RU[120], RD[120], LU[120], LD[120];
   int   gbid[120];    // if 1:good and used 0: be discarded
   float fspinpat[120];
   long  Nsi[6] = {0,0,0,0,0,0};

   // Right-Left asymmetry
   for (int bid=0; bid<120; bid++) {

       fspinpat[bid] = (float) gSpinPattern[bid];

       // R-L X90
       if (Ncounts[2-1][bid] + Ncounts[5-1][bid] != 0)
       {
           NR = Ncounts[2-1][bid];
           NL = Ncounts[5-1][bid];
           RL90[bid]  = (float) (NR - NL)/(NR + NL);
           RL90E[bid] = (float) 2*NL*NR*sqrt( (1./NR) + (1./NL) ) / (NL+NR) / (NL+NR);

       } else {
           RL90[bid]  = 0.;
           RL90E[bid] = 0.;
       }

       // R-L X45
       if (Ncounts[1-1][bid] + Ncounts[3-1][bid] +
           Ncounts[4-1][bid] + Ncounts[6-1][bid] != 0)
       {
           NR = Ncounts[1-1][bid] + Ncounts[3-1][bid];
           NL = Ncounts[4-1][bid] + Ncounts[6-1][bid];
           RL45[bid] = (float) (NR-NL)/(NR+NL);
           RL45E[bid] = (float) 2*NL*NR*sqrt((1./NR)+(1./NL))/(NL+NR)/(NL+NR);

       } else {
           RL45[bid]  = 0.;
           RL45E[bid] = 0.;
       }

       // B-T Y45
       if (Ncounts[3-1][bid] + Ncounts[4-1][bid]+
           Ncounts[1-1][bid] + Ncounts[6-1][bid]!=0)
       {
           NR = Ncounts[3-1][bid] + Ncounts[4-1][bid];
           NL = Ncounts[1-1][bid] + Ncounts[6-1][bid];

           BT45[bid]  = (float) (NR-NL)/(NR+NL);
           BT45E[bid] = (float) 2*NL*NR*sqrt((1./NR)+(1./NL))/(NL+NR)/(NL+NR);

       } else {
           BT45[bid] = 0.;
           BT45E[bid] = 0.;
       }
   }

   // *** GOOD/BAD BUNCH CRITERIA 1
   float gtmin, gtmax, btmin, btmax;
   gtmin = 0.0;
   gtmax = 1.0;
   btmin = 0.0;
   btmax = 1.00;

   for (int bid=0; bid<120; bid++) {

      gbid[bid] = 1;

      // good/total event rate
      if (Ntotal[bid] != 0) {
         if ( ((float) Ngood[bid]/Ntotal[bid]) < gtmin) {
            fprintf(stdout,"BID: %d discarded (GOOD/TOTAL) %f \n",
                    bid, (float) Ngood[bid]/Ntotal[bid]);
            gbid[bid] = 0;
         }

         if ( ((float) Ngood[bid]/Ntotal[bid]) > gtmax){
            fprintf(stdout,"BID: %d discarded (GOOD/TOTAL) %f \n",
                    bid, (float) Ngood[bid]/Ntotal[bid]);
            gbid[bid] = 0;
         }
      }

      // background / carbon event rate
      if (Ngood[bid]!=0) {

         if (((float)Nback[bid]/Ngood[bid]) < btmin){
            fprintf(stdout,"BID: %d discarded (BG/GOOD) %f \n",
                    bid,(float)Nback[bid]/Ngood[bid]);
            gbid[bid] = 0;
         }

         if (((float)Nback[bid]/Ngood[bid]) > btmax) {
            fprintf(stdout,"BID: %d discarded (BG/GOOD) %f \n",
                    bid,(float)Nback[bid]/Ngood[bid]);
            gbid[bid] = 0;
         }
      }
   }

   // Counts for each detector
   for (int bid=0; bid<N_BUNCHES; bid++) {
      Nsi[0] += Ncounts[0][bid];
      Nsi[1] += Ncounts[1][bid];
      Nsi[2] += Ncounts[2][bid];
      Nsi[3] += Ncounts[3][bid];
      Nsi[4] += Ncounts[4][bid];
      Nsi[5] += Ncounts[5][bid];
   }

   //HHPAK(31010, (float*)Ncounts[0]);
   //HHPAK(31020, (float*)Ncounts[1]);
   //HHPAK(31030, (float*)Ncounts[2]);
   //HHPAK(31040, (float*)Ncounts[3]);
   //HHPAK(31050, (float*)Ncounts[4]);
   //HHPAK(31060, (float*)Ncounts[5]);

   float x90phys[2][120], x90acpt[2][120], x90lumi[2][120];
   float x45phys[2][120], x45acpt[2][120], x45lumi[2][120];
   float y45phys[2][120], y45acpt[2][120], y45lumi[2][120];
   float c45phys[2][120], c45acpt[2][120], c45lumi[2][120];

   // X90 (2-5) (C:1-4)
   for (int bid=0; bid<120; bid++) {

      RU[bid] = ( bid == 0 ? 0 : RU[bid-1] ) + Ncounts[2-1][bid] * ((gSpinPattern[bid] ==  1)?1:0);
      RD[bid] = ( bid == 0 ? 0 : RD[bid-1] ) + Ncounts[2-1][bid] * ((gSpinPattern[bid] == -1)?1:0);
      LU[bid] = ( bid == 0 ? 0 : LU[bid-1] ) + Ncounts[5-1][bid] * ((gSpinPattern[bid] ==  1)?1:0);
      LD[bid] = ( bid == 0 ? 0 : LD[bid-1] ) + Ncounts[5-1][bid] * ((gSpinPattern[bid] == -1)?1:0);

      sqass(RU[bid], LD[bid], RD[bid], LU[bid], &tmpasym, &tmpasyme);

      x90[bid].phys   = tmpasym;
      x90[bid].physE  = tmpasyme;
      x90phys[0][bid] = tmpasym;
      x90phys[1][bid] = tmpasyme;

      sqass(RU[bid], RD[bid], LD[bid], LU[bid], &tmpasym, &tmpasyme);

      x90[bid].acpt   = tmpasym;
      x90[bid].acptE  = tmpasyme;
      x90acpt[0][bid] = tmpasym;
      x90acpt[1][bid] = tmpasyme;

      sqass(RU[bid], LU[bid], RD[bid], LD[bid], &tmpasym, &tmpasyme);

      x90[bid].lumi   = tmpasym;
      x90[bid].lumiE  = tmpasyme;
      x90lumi[0][bid] = tmpasym;
      x90lumi[1][bid] = tmpasyme;
      //        printf("%d : %d %f %f %f %f \n",bid,gSpinPattern[bid],
      //       RU[bid],RD[bid],LU[bid],LD[bid]);
   }

   fprintf(stdout, "si2 up :%10.0f down :%10.0f\n", RU[119], RD[119]);
   fprintf(stdout, "si5 up :%10.0f down :%10.0f\n", LU[119], LD[119]);

   // X45 (13-46) (C:02-35)
   for (int bid=0; bid<120; bid++) {

       RU[bid] = ((bid==0)?0:RU[bid-1]) + (Ncounts[1-1][bid] + Ncounts[3-1][bid]) * ((gSpinPattern[bid]==1)?1:0);
       RD[bid] = ((bid==0)?0:RD[bid-1]) + (Ncounts[1-1][bid] + Ncounts[3-1][bid]) * ((gSpinPattern[bid]==-1)?1:0);
       LU[bid] = ((bid==0)?0:LU[bid-1]) + (Ncounts[4-1][bid] + Ncounts[6-1][bid]) * ((gSpinPattern[bid]==1)?1:0);
       LD[bid] = ((bid==0)?0:LD[bid-1]) + (Ncounts[4-1][bid] + Ncounts[6-1][bid]) * ((gSpinPattern[bid]==-1)?1:0);

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

   fprintf(stdout, "si1,3 up :%10.0f down :%10.0f\n", RU[119], RD[119]);
   fprintf(stdout, "si4,6 up :%10.0f down :%10.0f\n", LU[119], LD[119]);

   // Y45 (34-16) (C:23-05)
   for (int bid=0; bid<120; bid++) {

      RU[bid] = ((bid==0)?0:RU[bid-1]) + (Ncounts[3-1][bid]+Ncounts[4-1][bid])*((gSpinPattern[bid]==1)?1:0);
      RD[bid] = ((bid==0)?0:RD[bid-1]) + (Ncounts[3-1][bid]+Ncounts[4-1][bid])*((gSpinPattern[bid]==-1)?1:0);
      LU[bid] = ((bid==0)?0:LU[bid-1]) + (Ncounts[1-1][bid]+Ncounts[6-1][bid])*((gSpinPattern[bid]==1)?1:0);
      LD[bid] = ((bid==0)?0:LD[bid-1]) + (Ncounts[1-1][bid]+Ncounts[6-1][bid])*((gSpinPattern[bid]==-1)?1:0);

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
   for (int bid=0; bid<120; bid++) {

      RU[bid] = ((bid==0)?0:RU[bid-1])
          + (Ncounts[1-1][bid]+Ncounts[4-1][bid])*((gSpinPattern[bid]==1)?1:0);
      RD[bid] = ((bid==0)?0:RD[bid-1])
          + (Ncounts[1-1][bid]+Ncounts[4-1][bid])*((gSpinPattern[bid]==-1)?1:0);
      LU[bid] = ((bid==0)?0:LU[bid-1])
          + (Ncounts[3-1][bid]+Ncounts[6-1][bid])*((gSpinPattern[bid]==1)?1:0);
      LD[bid] = ((bid==0)?0:LD[bid-1])
          + (Ncounts[3-1][bid]+Ncounts[6-1][bid])*((gSpinPattern[bid]==-1)?1:0);

      sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);

      cr45[bid].phys  = tmpasym;
      cr45[bid].physE = tmpasyme;
      c45phys[0][bid] = tmpasym;
      c45phys[1][bid] = tmpasyme;

      sqass(RU[bid],RD[bid],LD[bid],LU[bid],&tmpasym,&tmpasyme);

      cr45[bid].acpt  = tmpasym;
      cr45[bid].acptE = tmpasyme;
      c45acpt[0][bid] = tmpasym;
      c45acpt[1][bid] = tmpasyme;

      sqass(RU[bid],LU[bid],RD[bid],LD[bid],&tmpasym,&tmpasyme);

      cr45[bid].lumi  = tmpasym;
      cr45[bid].lumiE = tmpasyme;
      c45lumi[0][bid] = tmpasym;
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
      for (int bid=0; bid<120; bid++) {
         RU[bid] = ((bid==0)?0:RU[bid-1]) + NDcounts[2-1][bid][i]*((gSpinPattern[bid]==1)?1:0);
         RD[bid] = ((bid==0)?0:RD[bid-1]) + NDcounts[2-1][bid][i]*((gSpinPattern[bid]==-1)?1:0);
         LU[bid] = ((bid==0)?0:LU[bid-1]) + NDcounts[5-1][bid][i]*((gSpinPattern[bid]==1)?1:0);
         LD[bid] = ((bid==0)?0:LD[bid-1]) + NDcounts[5-1][bid][i]*((gSpinPattern[bid]==-1)?1:0);

         sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);

         x90[bid].phys = tmpasym;
         x90[bid].physE = tmpasyme;
         x90phys[0][bid] = tmpasym;
         x90phys[1][bid] = tmpasyme;
      }

      HHPAK(37000+i, x90phys[0]); HHPAKE(37000+i, x90phys[1]);

      // X45 (13-46) (C:02-35)
      for (int bid=0; bid<120; bid++) {
          RU[bid] = ((bid==0)?0:RU[bid-1]) + (NDcounts[1-1][bid][i]+NDcounts[3-1][bid][i])*((gSpinPattern[bid]==1)?1:0);
          RD[bid] = ((bid==0)?0:RD[bid-1]) + (NDcounts[1-1][bid][i]+NDcounts[3-1][bid][i])*((gSpinPattern[bid]==-1)?1:0);
          LU[bid] = ((bid==0)?0:LU[bid-1]) + (NDcounts[4-1][bid][i]+NDcounts[6-1][bid][i])*((gSpinPattern[bid]==1)?1:0);
          LD[bid] = ((bid==0)?0:LD[bid-1]) + (NDcounts[4-1][bid][i]+NDcounts[6-1][bid][i])*((gSpinPattern[bid]==-1)?1:0);

          sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);
          x45[bid].phys = tmpasym; x45[bid].physE = tmpasyme;
          x45phys[0][bid] = tmpasym;
          x45phys[1][bid] = tmpasyme;
      }

      HHPAK(37500+i, x45phys[0]); HHPAKE(37500+i, x45phys[1]);
   }

   // **** for different t range ****
   float txasym90[6][120], txasym90E[6][120];
   float txasym45[6][120], txasym45E[6][120];
   float tyasym45[6][120], tyasym45E[6][120];
   float tcasym45[6][120], tcasym45E[6][120];

   for (int tr=0; tr<NTBIN; tr++) {

      float SUM=0;

      // X90 (2 vs 5)
      for (int bid=0; bid<N_BUNCHES; bid++) {

          RU[bid] = ((bid==0)?0:RU[bid-1]) + NTcounts[2-1][bid][tr] * ( (gSpinPattern[bid] == 1)  ? 1:0) * gbid[bid];
          RD[bid] = ((bid==0)?0:RD[bid-1]) + NTcounts[2-1][bid][tr] * ( (gSpinPattern[bid] == -1) ? 1:0) * gbid[bid];
          LU[bid] = ((bid==0)?0:LU[bid-1]) + NTcounts[5-1][bid][tr] * ( (gSpinPattern[bid] == 1)  ? 1:0) * gbid[bid];
          LD[bid] = ((bid==0)?0:LD[bid-1]) + NTcounts[5-1][bid][tr] * ( (gSpinPattern[bid] == -1) ? 1:0) * gbid[bid];

          sqass(RU[bid], LD[bid], RD[bid], LU[bid], &tmpasym, &tmpasyme);

          tx90[bid][tr].phys  = tmpasym;
          tx90[bid][tr].physE = tmpasyme;
          txasym90[tr][bid]   = tmpasym;
          txasym90E[tr][bid]  = tmpasyme;

          SUM += NTcounts[2-1][bid][tr];
      }

      // X45 (1+3 vs 4+6)
      for (int bid=0; bid<120; bid++) {
          RU[bid] = ((bid==0)?0:RU[bid-1]) + (NTcounts[1-1][bid][tr]+NTcounts[3-1][bid][tr]) *((gSpinPattern[bid]==1)?1:0)*gbid[bid];
          RD[bid] = ((bid==0)?0:RD[bid-1]) + (NTcounts[1-1][bid][tr]+NTcounts[3-1][bid][tr]) *((gSpinPattern[bid]==-1)?1:0)*gbid[bid];
          LU[bid] = ((bid==0)?0:LU[bid-1]) + (NTcounts[4-1][bid][tr]+NTcounts[6-1][bid][tr]) *((gSpinPattern[bid]==1)?1:0)*gbid[bid];
          LD[bid] = ((bid==0)?0:LD[bid-1]) + (NTcounts[4-1][bid][tr]+NTcounts[6-1][bid][tr]) *((gSpinPattern[bid]==-1)?1:0)*gbid[bid];

          sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);

          tx45[bid][tr].phys = tmpasym; tx45[bid][tr].physE = tmpasyme;
          txasym45[tr][bid] = tmpasym; txasym45E[tr][bid] = tmpasyme;
      }

      // Y45 (3+4 vs 1+6)
      for (int bid=0; bid<120; bid++) {
          RU[bid] = ((bid==0)?0:RU[bid-1]) + (NTcounts[3-1][bid][tr]+NTcounts[4-1][bid][tr]) *((gSpinPattern[bid]==1)?1:0)*gbid[bid];
          RD[bid] = ((bid==0)?0:RD[bid-1]) + (NTcounts[3-1][bid][tr]+NTcounts[4-1][bid][tr]) *((gSpinPattern[bid]==-1)?1:0)*gbid[bid];
          LU[bid] = ((bid==0)?0:LU[bid-1]) + (NTcounts[1-1][bid][tr]+NTcounts[6-1][bid][tr]) *((gSpinPattern[bid]==1)?1:0)*gbid[bid];
          LD[bid] = ((bid==0)?0:LD[bid-1]) + (NTcounts[1-1][bid][tr]+NTcounts[6-1][bid][tr]) *((gSpinPattern[bid]==-1)?1:0)*gbid[bid];
          sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);
          ty45[bid][tr].phys = tmpasym; ty45[bid][tr].physE = tmpasyme;
          tyasym45[tr][bid] = tmpasym; tyasym45E[tr][bid] = tmpasyme;
      }

      // CROSS 45 (1+4 vs 3+6)
      for (int bid=0; bid<120; bid++) {
          RU[bid] = ((bid==0)?0:RU[bid-1]) + (NTcounts[1-1][bid][tr]+NTcounts[4-1][bid][tr]) *((gSpinPattern[bid]==1)?1:0)*gbid[bid];
          RD[bid] = ((bid==0)?0:RD[bid-1]) + (NTcounts[1-1][bid][tr]+NTcounts[4-1][bid][tr]) *((gSpinPattern[bid]==-1)?1:0)*gbid[bid];
          LU[bid] = ((bid==0)?0:LU[bid-1]) + (NTcounts[3-1][bid][tr]+NTcounts[6-1][bid][tr]) *((gSpinPattern[bid]==1)?1:0)*gbid[bid];
          LD[bid] = ((bid==0)?0:LD[bid-1]) + (NTcounts[3-1][bid][tr]+NTcounts[6-1][bid][tr]) *((gSpinPattern[bid]==-1)?1:0)*gbid[bid];
          sqass(RU[bid], LD[bid], RD[bid], LU[bid], &tmpasym, &tmpasyme);
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
   HHPAK(31100, RL90);   HHPAKE(31100, RL90E);
   HHPAK(31110, RL45);   HHPAKE(31110, RL45E);
   HHPAK(31120, BT45);   HHPAKE(31120, BT45E);

   HHPAK(33000, (float*) Ngood);
   HHPAK(33010, (float*) Nback);
   HHPAK(33020, (float*) Ntotal);

   // Spin Sorted Strip Distribution
   HHPAK(36000, (float*) cntr.reg.NStrip[0]);
   HHPAK(36100, (float*) cntr.reg.NStrip[1]);
} //}}}


// Description : Calculate Statistics
void CalcStatistics()
{ //{{{
   // Integrate good carbon events in banana
   cntr.good_event = 0;

   int X_index = gRunInfo->Run >= 6 ? nTgtIndex : gNDelimeters;

   for (int i=0; i<X_index; i++) cntr.good_event += cntr.good[i];

   // Run time duration
   gRunInfo->RunTime = gRunInfo->Run == 5 ? gNDelimeters : cntr.revolution/RHIC_REVOLUTION_FREQ;

   // Calculate rates
   if (gRunInfo->RunTime) {
      gRunInfo->GoodEventRate = float(cntr.good_event) / gRunInfo->RunTime / 1e6;
      gRunInfo->EvntRate      = float(Nevtot) / gRunInfo->RunTime / 1e6;
      gRunInfo->ReadRate      = float(Nread) / gRunInfo->RunTime / 1e6;
   }

   // Misc
   if (gRunInfo->fWallCurMonSum) gAnaResult->wcm_norm_event_rate = gRunInfo->GoodEventRate/gRunInfo->fWallCurMonSum*100;
   if (gAnaInfo->reference_rate) gAnaResult->UniversalRate       = gAnaResult->wcm_norm_event_rate/gAnaInfo->reference_rate;
   if (gRunInfo->Run == 5)       gAnaResult->profile_error       = gAnaResult->UniversalRate < 1 ? ProfileError(gAnaResult->UniversalRate) : 0;
} //}}}


// Description : print warnings
void PrintWarning()
{ //{{{
   printf("-----------------------------------------------------------------------------------------\n");
   printf("------------------------------  Error Detector Results ----------------------------------\n");
   printf("-----------------------------------------------------------------------------------------\n");
   printf("===> Bunch distribution \n");
   printf(" # of Bunches requirement for check  : %6d\n",errdet.NBUNCH_REQUIREMENT);
   printf(" Good Bunch SpeLumi Sigma Allowance  : %6.1f\n",errdet.BUNCH_RATE_SIGMA_ALLOWANCE);
   printf(" Good Bunch Asymmetry Sigma Allowance: %6.1f\n",errdet.BUNCH_ASYM_SIGMA_ALLOWANCE);
   printf(" SpeLumi sigma/mean                  : %8.4f\n",bnchchk.rate.sigma_over_mean);
   printf(" Asymmmetry sigma x90, x45, y45      : %8.4f %8.4f %8.4f\n",
          bnchchk.asym[0].sigma, bnchchk.asym[1].sigma, bnchchk.asym[2].sigma);
   if (bnchchk.rate.max_dev) printf(" Max SpeLumi deviation from average  : %6.1f\n",bnchchk.rate.max_dev);
   printf(" Number of Problemeatic Bunches      : %6d \n",  gAnaResult->anomaly.nbunch);
   printf(" Problemeatic Bunches Rate [%%]      : %6.1f\n", gAnaResult->anomaly.bad_bunch_rate);
   printf(" Bunch error code                    :   "); binary_zero(gAnaResult->anomaly.bunch_err_code,4); printf("\n");
   printf(" Problemeatic Bunch ID's             : ");
   for (int i=0; i<gAnaResult->anomaly.nbunch; i++) printf("%d ", gAnaResult->anomaly.bunch[i]) ;
   printf("\n");
   printf(" Unrecognized Problematic Bunches    : ");
   for (int i=0; i<gAnaResult->unrecog.anomaly.nbunch; i++) printf("%d ", gAnaResult->unrecog.anomaly.bunch[i]) ;
   printf("\n");
   printf("===> Invariant Mass / strip \n");
   printf(" Maximum Mass Deviation [GeV]        : %6.2f (%d)\n", strpchk.dev.max,  strpchk.dev.st+1);
   printf(" Maximum Mass fit chi-2              : %6.2f (%d)\n", strpchk.chi2.max, strpchk.chi2.st+1);
   printf(" Maximum #Events Deviation from Ave  : %6.2f (%d)\n", strpchk.evnt.max, strpchk.evnt.st+1);
   printf(" Maximum Mass-Energy Correlation     : %8.4f (%d)\n", strpchk.p1.max,   strpchk.p1.st+1);
   printf(" Weighted Mean InvMass Sigma         : %6.2f \n",     strpchk.width.average[0]);
   printf(" Good strip Mass-Energy Correlation  : %8.4f \n",     strpchk.p1.allowance);
   printf(" Good strip Mass Sigma Allowance[GeV]: %6.2f \n",     strpchk.width.allowance);
   printf(" Good strip Mass Pos. Allowance[GeV] : %6.2f \n",     strpchk.dev.allowance);
   printf(" Good strip Mass Fit chi2 Allowance  : %6.2f \n",     strpchk.chi2.allowance);
   printf(" Good strip #Events Allowance        : %6.2f \n",     strpchk.evnt.allowance);
   printf(" Strip error code                    : "); binary_zero(gAnaResult->anomaly.strip_err_code,4);printf("\n");
   printf(" Number of Problematic Strips        : %6d \n", gAnaResult->anomaly.nstrip);
   printf(" Problematic Strips                  : ");
   for (int i=0; i<gAnaResult->anomaly.nstrip; i++) printf("%d ", gAnaResult->anomaly.st[i]+1);
   printf("\n");
   printf(" Number of Unrecog. Problematic Strip: %6d \n", gAnaResult->unrecog.anomaly.nstrip);
   printf(" Unrecognized Problematic Strips     : ");
   for (int i=0; i<gAnaResult->unrecog.anomaly.nstrip; i++) printf("%d ", gAnaResult->unrecog.anomaly.st[i]+1) ;
   printf("\n");
   printf("===> Detector \n");
   printf(" Slope of Energy Spectrum (sum 6 det): %6.1f %6.2f\n", gAnaResult->energy_slope[0], gAnaResult->energy_slope[1]);
   printf("-----------------------------------------------------------------------------------------\n");
   printf("\n\n");
} //}}}


// Method name : PrintRunResults()
//
// Description : print analysis results and run infomation
void PrintRunResults()
{ //{{{
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
   printf(" RunTime                 [s]    = %10.1f\n", gRunInfo->RunTime);
   printf(" Total events in banana         = %10ld\n",  cntr.good_event);
   printf(" Good Carbon Max Rate  [MHz]    = %10.4f\n", gAnaResult->max_rate);
   printf(" Good Carbon Rate      [MHz]    = %10.4f\n", gRunInfo->GoodEventRate);
   printf(" Good Carbon Rate/WCM_sum       = %10.5f\n", gAnaResult->wcm_norm_event_rate);
   printf(" Universal Rate                 = %10.5f\n", gAnaResult->UniversalRate);
   printf(" Event Rate            [MHz]    = %10.4f\n", gRunInfo->EvntRate);
   printf(" Read Rate             [MHz]    = %10.4f\n", gRunInfo->ReadRate);
   printf(" Target                         = %c%c\n",   gRunInfo->fTargetOrient, gRunInfo->fTargetId);

   if (gRunInfo->Run >=6 ) {
      printf(" Maximum Revolution #           = %10d\n",   gRunInfo->MaxRevolution);
      printf(" Reconstructed Duration  [s]    = %10.1f\n", gRunInfo->MaxRevolution/RHIC_REVOLUTION_FREQ);
      printf(" Target Motion Counter          = %10ld\n",  cntr.tgtMotion);
   }

   printf(" WCM Sum     [10^11 protons]    = %10.1f\n", gRunInfo->fWallCurMonSum/100);
   printf(" WCM Average [10^9  protons]    = %10.1f\n", gRunInfo->fWallCurMonAve);
   printf(" WCM Average w/in range         = %10.1f\n", average.average);
   printf(" Specific Luminosity            = %10.2f%10.2f%10.4f\n", gHstat.mean, gHstat.RMS, gHstat.RMSnorm);
   printf(" # of Filled Bunch              = %10d\n", gRunInfo->NFilledBunch);
   printf(" # of Active Bunch              = %10d\n", gRunInfo->NActiveBunch);
   printf(" bunch w/in WCM range           = %10d\n", average.counter);
   printf(" process rate                   = %10.1f [%%]\n", (float)average.counter/(float)NFilledBunch*100);
   printf(" Analyzing Power Average        = %10.4f \n",     gAnaResult->A_N[1]);
   if (gAnaInfo->FEEDBACKMODE)
   printf(" feedback average tshift        = %10.1f [ns]\n", gAnaResult->TshiftAve);
   printf(" Average Polarization (phi=0)   = %10.4f%9.4f\n", gAnaResult->P[0],                  gAnaResult->P[1]);
   printf(" Polarization (sinphi)          = %10.4f%9.4f\n", gAnaResult->sinphi[0].P[0],        gAnaResult->sinphi[0].P[1]);
   printf(" Phase (sinphi)  [deg.]         = %10.4f%9.4f\n", gAnaResult->sinphi[0].dPhi[0]*R2D, gAnaResult->sinphi[0].dPhi[1]*R2D);
   printf(" chi2/d.o.f (sinphi fit)        = %10.4f\n",      gAnaResult->sinphi[0].chi2);
   printf(" Polarization (bunch ave)       = %10.4f%9.4f\n", gBunchAsym.ave.Ax[0]/gAnaResult->A_N[1], gBunchAsym.ave.Ax[1]/gAnaResult->A_N[1]);
   printf(" Phase (bunch ave)              = %10.4f%9.4f\n", gBunchAsym.ave.phase[0]*R2D, gBunchAsym.ave.phase[1]*R2D);
   if (gRunInfo->Run == 5)
   printf(" profile error (absolute)[%%]   = %10.4f\n",      gAnaResult->profile_error * fabs(gAnaResult->P[0]));
   printf("--- Alternative %3.1f sigma result & ratio to %3.1f sigma ---\n", gAnaInfo->MassSigmaAlt, gAnaInfo->MassSigma);
   printf(" Polarization (sinphi) alt      = %10.4f%9.4f\n", gAnaResult->sinphi[1].P[0],        gAnaResult->sinphi[1].P[1]);
   printf(" Ratio (alt/reg)                = %10.2f%9.2f\n", gAnaResult->P_sigma_ratio[0],      gAnaResult->P_sigma_ratio[1]);
   printf(" Ratio ((alt-reg)/reg)          = %10.3f%9.3f\n", gAnaResult->P_sigma_ratio_norm[0], gAnaResult->P_sigma_ratio_norm[1]);
   printf(" Polarization (PHENIX)          = %10.4f%9.4f\n", gAnaResult->sinphi[2].P[0],        gAnaResult->sinphi[2].P[1]);
   printf(" Polarization (STAR)            = %10.4f%9.4f\n", gAnaResult->sinphi[3].P[0],        gAnaResult->sinphi[3].P[1]);
   printf("-----------------------------------------------------------------------------------------\n");

   for(Int_t i=0; i<nTgtIndex+1; i++)
   printf(" Polarization tgt pos %6.1f     = %10.4f %9.4f\n", tgt.all.x[(int) tgt.Time[i]], gAnaResult->sinphi[100+i].P[0], gAnaResult->sinphi[100+i].P[1]);

   printf("-----------------------------------------------------------------------------------------\n");
} //}}}


/** */
void DrawPlotvsTar()
{ //{{{
   Double_t polvstar[nTgtIndex+1],epolvstar[nTgtIndex+1],posvstar[nTgtIndex+1];

   for(Int_t i=0;i<nTgtIndex+1;i++){
      polvstar[i]  = gAnaResult->sinphi[100+i].P[0];
      epolvstar[i] = gAnaResult->sinphi[100+i].P[1];
      posvstar[i]  = tgt.all.x[(int)tgt.Time[i]];
   }

   Asymmetry->cd();
   TH2F *h_vstar = new TH2F("Pol_vs_tarpos","Polarization vs target position",100,posvstar[0]-1,posvstar[nTgtIndex]+1,100,0,1);

   TGraphErrors *gpolvstar = new TGraphErrors(nTgtIndex+1,posvstar,polvstar,0,epolvstar);
   h_vstar->GetXaxis()->SetTitle("Target position");
   h_vstar->GetYaxis()->SetTitle("Polarization");
   gpolvstar->SetMarkerStyle(20);
   gpolvstar->SetMarkerColor(kGreen+100);

   DrawHorizLine(h_vstar, posvstar[0]-1, posvstar[nTgtIndex]+1, gAnaResult->sinphi[0].P[0],                              kRed, 1, 2);
   DrawHorizLine(h_vstar, posvstar[0]-1, posvstar[nTgtIndex]+1, gAnaResult->sinphi[0].P[0] - gAnaResult->sinphi[0].P[1], kRed, 1, 1);
   DrawHorizLine(h_vstar, posvstar[0]-1, posvstar[nTgtIndex]+1, gAnaResult->sinphi[0].P[0] + gAnaResult->sinphi[0].P[1], kRed, 1, 1);

   h_vstar->GetListOfFunctions()->Add(gpolvstar, "P");
} //}}}


// Description : Caluclate Energy Yeild weighted Analyzing power
//             : Histogram 34000 is filled only once, which is controled by CallFlag
// Input       : int HID
// Return      : A_N
float AsymCalculator::WeightAnalyzingPower(int HID)
{ //{{{
   //static int CallFlag = 0;

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

   if (gRunInfo->GetBeamEnergy() > 200)
   {
      // Scale up the flattop (250 GeV) polarization values by 15%: 1.176 = 1./ (1-0.15)
      //for (int i=0; i<25; i++) anth[i] = anth100[i] * 1.176; v1.2.0 and earlier
      //
      // A new correction introduced in v1.3.1 scales pC polarization down by
      // approx 18% (0.823 +/- 0.012) at 250 GeV
      //for (int i=0; i<25; i++) anth[i] = anth100[i] * 1.215;
      //
      // A new correction to the H-jet introduced in v1.3.14 scales pC polarization up by
      // approx 13% at 250 GeV
      //
      // Yet another correction in for v1.6.1 that is implemented in v1.6.2

      Float_t a_n_scale_v1_3_14 = 1;
      Float_t a_n_scale_v1_6_1  = 1;

      switch (gRunInfo->fPolId) {
      case kB1U:
         a_n_scale_v1_3_14 = 0.8371;
         a_n_scale_v1_6_1  = 1./0.9519; // overall is: 0.8794
         break;
      case kY1D:
         a_n_scale_v1_3_14 = 0.8773;
         a_n_scale_v1_6_1  = 1./0.9640; // overall is: 0.9101
         break;
      case kB2D:
         a_n_scale_v1_3_14 = 0.7870;
         a_n_scale_v1_6_1  = 1./0.9852; // overall is: 0.7988
         break;
      case kY2U:
         a_n_scale_v1_3_14 = 0.8481;
         a_n_scale_v1_6_1  = 1./0.9615; // overall is: 0.8821
         break;
      }

      for (int i=0; i<25; i++) anth[i] = anth100[i] * 1.215 * a_n_scale_v1_3_14 * a_n_scale_v1_6_1;

   } else if (gRunInfo->GetBeamEnergy() > 50) {

      for (int i=0; i<25; i++) anth[i] = anth100[i];

   } else if (gRunInfo->GetBeamEnergy() > 20) {
      // A new correction to the H-jet polarization is introduced in v1.5.0 scales pC polarization up

      //Float_t a_n_scale_v1_5_0 = 1;

      //switch (gRunInfo->fPolId) {
      //case kB1U:
      //   //a_n_scale_v1_5_0 = 0.8525;
      //   a_n_scale_v1_5_0 = 1;
      //   break;
      //case kY1D:
      //   //a_n_scale_v1_5_0 = 0.9231;
      //   a_n_scale_v1_5_0 = 1;
      //   break;
      //case kB2D:
      //   //a_n_scale_v1_5_0 = 0.9016;
      //   a_n_scale_v1_5_0 = 1;
      //   break;
      //case kY2U:
      //   //a_n_scale_v1_5_0 = 0.8000;
      //   a_n_scale_v1_5_0 = 1;
      //   break;
      //}

      //for (int i=0; i<25; i++) anth[i] = anth[i] * a_n_scale_v1_5_0;
   }

   float Emin = 22.5;
   float Emax = 1172.2;
   float DeltaE = (Emax - Emin)/25.;

   //int nbin   = HENEBIN;
   //int fstbin = 1;
   //int lstbin = HENEBIN;

   float X[HENEBIN];
   float Y[HENEBIN];
   //float EX[HENEBIN];
   //float EY[HENEBIN];

   //ds hhrebin_(&HID, X, Y, EX, EY, &nbin, &fstbin, &lstbin);

   int j      = 0;
   float sum  = 0.;
   float suma = 0.;

   for (int i=0; i<HENEBIN; i++){
      j = (int) ((X[i] - Emin)/DeltaE);
      sum  += Y[i];
      //suma += Y[i] * anth[j];
      suma += Y[i] * anth[i];
      //printf("%d: Y %f AN %f X %f\n",i,Y[i],anth[j],X[i]);
   }

   float aveA_N = sum != 0 ? suma/sum : 0;

   // ds fix
   TH1F* hEnergy = (TH1F*) gAsymRoot->fHists->d["std"]->o["hKinEnergyA_o_cut2"];
   //TH1F* hEnergy = (TH1F*) gAsymRoot->fHists->o["hKinEnergyA_oo"];

   if (!hEnergy) return 0;

   j = 0;
   sum = suma = 0;

   for (int i=1; i<=hEnergy->GetNbinsX(); i++) {
      double bcont  = hEnergy->GetBinContent(i);
      //double bcentr = hEnergy->GetBinCenter(i);
      //double bwidth = hEnergy->GetBinWidth(i);

      //j = (int) ((bcentr - Emin)/bwidth);

      sum  += bcont;
      suma += bcont * anth[i-1];
      //suma += bcont * anth[j];
   }

   float aveA_N_2 = sum != 0 ? suma/sum : 0;

   printf("Average analyzing power: %10.8f %10.8f\n", aveA_N, aveA_N_2);

   // Fill Analyzing power histogram only once.
   //ds if (!CallFlag) HHF1(34000, 0.5, aveA_N);

   //ds CallFlag=1;

   return aveA_N_2;
} //}}}


// Description : returns true if strip #j is in exclusion candidate
// Input       : int i, int j
// Return      : true/false
Bool_t AsymCalculator::ExcludeStrip(int k, int j)
{ //{{{
   int i = (k >= 36) ? k-36 : k;

   if (j == i || j == 35-i || j == 36+i || j == 71-i )
      return kTRUE;

   return kFALSE;
} //}}}


// Description : calculate Asymmetry
// Input       : int a, int b, int atot, int btot
// Return      : float Asym, float dAsym
//
// Asym = (A - R * B) / (A + R * B), where R = atot/btot
//
void AsymCalculator::CalcAsymmetry(int a, int b, int atot, int btot, float &Asym, float &dAsym)
{ //{{{
   float R    = 0;
   float A    = float(a);
   float B    = float(b);
   float Atot = float(atot);
   float Btot = float(btot);

   if (Btot) R = Atot/Btot;

   if ( A+R*B && A+B ) {
      Asym  = (A-R*B)/(A+R*B);
      dAsym = sqrt(4*B*B*A + 4*A*A*B)/(A+B)/(A+B);
   } else {
      Asym = dAsym = 0;
   }
} //}}}


// Description : calculates profile error for given universal rate x
//             : For run5, zero for blue beam
float ProfileError(float x)
{
   return gRunInfo->fPolBeam == 1 ? exp(-0.152*log(1/x)) : 0 ;
}


// Specific Luminosity
// Description : Handle Specific Luminosity
// Input       : Histograms 10033, 11033, 10034, 11034
// Return      : float &mean, float &RMS, float &RMS_norm
void SpecificLuminosity(float &mean, float &RMS, float &RMS_norm)
{ //{{{
   float SpeLumi_norm[N_BUNCHES], dSpeLumi_norm[N_BUNCHES];

   // initialization
   SpeLumi.max = SpeLumi.Cnts[0];
   SpeLumi.min = 9999999;

   for (int bid=0; bid<N_BUNCHES; bid++) {

      if (gRunInfo->fWallCurMon.find(bid+1) != gRunInfo->fWallCurMon.end() )

      SpeLumi.Cnts[bid] = gRunInfo->fWallCurMon[bid+1] != 0 ? Ngood[bid]/gRunInfo->fWallCurMon[bid+1] : 0;

      SpeLumi.dCnts[bid] = sqrt(double(Ngood[bid]));

      ((TH1*) gAsymRoot->fHists->d["run"]->o["specific_luminosity"])->Fill(bid+1, SpeLumi.Cnts[bid]);

      if (gFillPattern[bid]) {
         if (SpeLumi.max < SpeLumi.Cnts[bid]) SpeLumi.max = SpeLumi.Cnts[bid];
         if (SpeLumi.min > SpeLumi.Cnts[bid]) SpeLumi.min = SpeLumi.Cnts[bid];
      }
   }

   HHPAK(10033, SpeLumi.Cnts);
   HHPAKE(11033, SpeLumi.dCnts);

   SpeLumi.ave = WeightedMean(SpeLumi.Cnts,SpeLumi.dCnts,N_BUNCHES);

   if (SpeLumi.ave) {
      for (int bid=0; bid<N_BUNCHES; bid++) {
         SpeLumi_norm[bid]  = SpeLumi.Cnts[bid] / SpeLumi.ave;
         dSpeLumi_norm[bid] = SpeLumi.dCnts[bid] / SpeLumi.ave;
      }
   }

   HHPAK(10034, SpeLumi_norm);    HHPAKE(11034, dSpeLumi_norm);

   // Book and fill histograms
   char hcomment[256];
   sprintf(hcomment, "Specific Luminosity");
   HHBOOK1(10035, hcomment, 100, SpeLumi.ave-SpeLumi.ave/2, SpeLumi.ave + SpeLumi.ave/2.);

   for (int bid=0; bid < 120; bid++) HHF1(10035, SpeLumi.Cnts[bid], 1);

   // Get variables
   char CHOICE[5]="HIST";
   mean = HHSTATI(10035, 1, CHOICE, 0) ;
   RMS  = HHSTATI(10035, 2, CHOICE, 0) ;
   RMS_norm = (!mean) ? 0 : RMS/mean ;
} //}}}


// Description : Find Time shift from 12C mass peak fit. Units are all in [GeV]
//             : ouotputs are converted to [keV]
// Input       : int Mode
//             : int FeedBackLevel 1) no fit, just max and mean
//             :                   2) gaussian fit
// Return      : average tshift [ns] @ 500keV
//
float TshiftFinder(int Mode, int FeedBackLevel)
{ //{{{
   int   np = 3;
   float par[np], sigpar[np];
   float chi2;
   float mdev,adev;
   float ex[N_SILICON_CHANNELS];
   char  htitle[50];
   float min,max;
   float margin = 0.2;

   TGraphErrors *tg;
   TF1 *f1 = new TF1("f1", "gaus");

   for (int st=0; st<N_SILICON_CHANNELS; st++) {

      feedback.strip[st] = st + 1;

      chi2 = sigpar[1] = 0; ex[st]=0; feedback.err[st]=1; // initialization

      switch (FeedBackLevel) {

      case 1:     // Level 1 Histogram Maximum and Mean
         cerr << "TshiftFinder:: Case=1 is no longer avarilable" << endl;
         exit(-1);
         break;

      case 2:   // Level 2 Gaussian Fit
         // parameter initialization
         par[0] = Mode ? mass_feedback[st]->GetMaximum() : mass_yescut[st]->GetMaximum(); // amplitude
         par[1] = MASS_12C * k2G;       // mean [GeV]
         par[2] = gAnaInfo->OneSigma * k2G; // sigma [GeV]

         if (par[0]) { // Gaussian Fit unless histogram isn't empty
            Mode ? mass_feedback[st]->Fit(f1, "Q") : mass_yescut[st]->Fit(f1, "Q");

            par[1]    = f1->GetParameter(1);
            par[2]    = f1->GetParameter(2);
            sigpar[1] = f1->GetParError(2);
            chi2      = f1->GetChisquare();
         } else {
            par[1] = MASS_12C*k2G + ASYM_DEFAULT;
            par[2] = ASYM_DEFAULT;
            feedback.err[st] = 0; // set weight 0
         }

         feedback.err[st]  = Mode ? sigpar[1] : sigpar[1] * chi2 * chi2;
         feedback.mdev[st] = par[1] - MASS_12C*k2G;
         feedback.RMS[st]  = par[2];
         feedback.chi2[st] = chi2;

         break;
      }
   }

   // ds
   //if (Mode) {

     FeedBack->cd();

     TGraphErrors *gre = new TGraphErrors(N_SILICON_CHANNELS, feedback.strip, feedback.mdev, ex, feedback.err);
     gre->SetMarkerSize(MSIZE);
     gre->SetMarkerStyle(20);
     gre->SetMarkerColor(4);

     GetMinMaxOption(errdet.MASS_POSITION_ALLOWANCE*1.2, N_SILICON_CHANNELS, feedback.mdev, margin, min, max);

     mdev_feedback = new TH2F("mdev_feedback", "Mass Deviation in FeedBack Mode", N_SILICON_CHANNELS, -0.5, N_SILICON_CHANNELS+0.5, 50, min, max);

     mdev_feedback->GetListOfFunctions()->Add(gre,"P");
     mdev_feedback->GetXaxis()->SetTitle("Strip ID");
     mdev_feedback->GetYaxis()->SetTitle("Mass Deviation [GeV]");

   //} else {

     ErrDet->cd();

     // Mass Position Deviation from M_12
     GetMinMaxOption(errdet.MASS_POSITION_ALLOWANCE*1.2, N_SILICON_CHANNELS, feedback.mdev, margin, min, max);
     sprintf(htitle,"Run%.3f:Invariant mass position deviation vs. strip", gRunInfo->RUNID);
     mass_pos_dev_vs_strip =  new TH2F("mass_pos_dev_vs_strip",htitle,N_SILICON_CHANNELS+1,0,N_SILICON_CHANNELS+1,50, min, max);
     tg =  AsymmetryGraph(1, N_SILICON_CHANNELS, feedback.strip, feedback.mdev, ex, ex);
     tg->SetName("tg");
     mass_pos_dev_vs_strip -> GetListOfFunctions()-> Add(tg,"p");
     mass_pos_dev_vs_strip -> GetYaxis() -> SetTitle("Peak - M_12C [GeV]");
     mass_pos_dev_vs_strip -> GetXaxis() -> SetTitle("Strip Number");

     // RMS width mapping of 12C mass peak
     GetMinMax(N_SILICON_CHANNELS, feedback.RMS, margin, min, max);
     sprintf(htitle,"Run%.3f:Gaussian fit on Invariant mass sigma vs. strip", gRunInfo->RUNID);
     mass_sigma_vs_strip =  new TH2F("mass_sigma_vs_strip",htitle,N_SILICON_CHANNELS+1,0,N_SILICON_CHANNELS+1,50, min, max);
     tg =  AsymmetryGraph(1, N_SILICON_CHANNELS, feedback.strip, feedback.RMS, ex, feedback.err);
     tg->SetName("tg");
     mass_sigma_vs_strip -> GetListOfFunctions()-> Add(tg,"p");
     mass_sigma_vs_strip -> GetYaxis() -> SetTitle("RMS Width of 12C Mass Peak[GeV]");
     mass_sigma_vs_strip -> GetXaxis() -> SetTitle("Strip Number");

     // Chi2 mapping of Gaussian fit on 12C mass peak
     sprintf(htitle,"Run%.3f:Gaussian fit on Invariant mass chi2 vs. strip",gRunInfo->RUNID);
     GetMinMax(N_SILICON_CHANNELS, feedback.chi2, margin, min, max);
     mass_chi2_vs_strip =  new TH2F("mass_chi2_vs_strip", htitle, N_SILICON_CHANNELS+1, 0, N_SILICON_CHANNELS+1, 50, min, max);
     tg  =  AsymmetryGraph(1, N_SILICON_CHANNELS, feedback.strip, feedback.chi2, ex, ex);
     tg->SetName("tg");
     mass_chi2_vs_strip -> GetListOfFunctions()-> Add(tg, "p");
     mass_chi2_vs_strip -> SetTitle(";Strip Number;Chi2 of Gaussian Fit on 12C Mass Peak;");

     // Call strip anomaly detector routine
     StripAnomalyDetector();
   //}

   mdev = WeightedMean(feedback.mdev, feedback.err, N_SILICON_CHANNELS);
   printf("Average Mass Deviation  = %10.2f [GeV/c]\n", mdev);

   adev = mdev * G2k * gRunConsts[0].M2T/sqrt(400.);

   printf("Tshift Average @ 400keV = %10.2f [ns]\n", adev);

   // Unit Conversion [GeV] -> [keV]
   for (int i=0; i<N_SILICON_CHANNELS; i++) {
     feedback.tedev[i] = feedback.mdev[i]*G2k* gRunConsts[i+1].M2T;
     feedback.RMS[i] *= G2k;
   }

   return adev;
} //}}}


// Description : Fill out bunch by bunch Asymmetry Histograms
//             : asym_bunch_x90, asym_bunch_x45, asym_bunch_y45
//             : These histograms are then applied Gaussian fit to check anomaly bunches
// Input       : string mode, int sign, int N, float A[], float dA[], float bunch[]
void FillAsymmetryHistgram(string mode, int sign, int N, float A[], float dA[], float bunch[])
{ //{{{
   float a[N];

   for (int i=0; i<N; i++) { // loop for bunch number
      // flip the asymmetry sign for spin=-1 to be consistent with spin=+1
      a[i] = A[i]*sign;

      if ( gFillPattern[i] && dA[i] ) {
         // process only active bunches
         if (bunch[i] != -1) {
            if (mode == "x90") asym_bunch_x90->Fill(a[i], 1/dA[i]); //weighted by error
            if (mode == "x45") asym_bunch_x45->Fill(a[i], 1/dA[i]);
            if (mode == "y45") asym_bunch_y45->Fill(a[i], 1/dA[i]);
         }
      }
   }
} //}}}


// Description : call BunchAsymmetry to calculate asymmetries bunch by bunch and
//             : Fill out asym_vs_bunch_x90, x45, y45 histograms
void AsymCalculator::CalcBunchAsymmetry()
{ //{{{
   // Calculate bunch asymmetries [0] and corresponding errors [1] for x45,
   // x90, y45
   BunchAsymmetry(0, gBunchAsym.Ax90[0], gBunchAsym.Ax90[1]); // x90
   BunchAsymmetry(1, gBunchAsym.Ax45[0], gBunchAsym.Ax45[1]); // x45
   BunchAsymmetry(2, gBunchAsym.Ay45[0], gBunchAsym.Ay45[1]); // y45

   // XXX ds
   return;

   // Define TH2F histograms first
   Asymmetry->cd();

   char  htitle[100];
   float min, max;
   float margin=0.2;
   float prefix=0.028;

   sprintf(htitle, "Run%.3f: Raw Asymmetry X90", gRunInfo->RUNID);
   GetMinMaxOption(prefix, N_BUNCHES, gBunchAsym.Ax90[0], margin, min, max);

   //asym_vs_bunch_x90 = new TH2F("asym_vs_bunch_x90", htitle, N_BUNCHES, -0.5, N_BUNCHES-0.5, 100, min, max);
   asym_vs_bunch_x90->SetName("asym_vs_bunch_x90");
   asym_vs_bunch_x90->SetTitle(htitle);
   asym_vs_bunch_x90->SetBins(N_BUNCHES, -0.5, N_BUNCHES-0.5, 100, min, max);
   DrawHorizLine(asym_vs_bunch_x90, -0.5, N_BUNCHES-0.5, 0, 1, 1, 1);

   sprintf(htitle,"Run%.3f : Raw Asymmetry Y45", gRunInfo->RUNID);
   GetMinMaxOption(prefix, N_BUNCHES, gBunchAsym.Ay45[0], margin, min, max);

   //asym_vs_bunch_y45 = new TH2F("asym_vs_bunch_y45", htitle, N_BUNCHES, -0.5, N_BUNCHES-0.5, 100, min, max);
   asym_vs_bunch_y45->SetName("asym_vs_bunch_y45");
   asym_vs_bunch_y45->SetTitle(htitle);
   asym_vs_bunch_y45->SetBins(N_BUNCHES, -0.5, N_BUNCHES-0.5, 100, min, max);
   asym_vs_bunch_y45->GetYaxis()->SetTitle("Counts weighted by error");
   asym_vs_bunch_y45->GetXaxis()->SetTitle("Raw Asymmetry");

   DrawHorizLine(asym_vs_bunch_y45, -0.5, N_BUNCHES-0.5, 0, 1, 1, 1);

   // fill bunch ID array [1 - N_BUNCHES], not [0 - N_BUNCHES-1]
   float bunch[N_BUNCHES], ex[N_BUNCHES];

   for (int bid=0; bid<N_BUNCHES; bid++) { ex[bid] = 0; bunch[bid] = bid; }

   // Superpose Positive/Negative Bunches arrays into TH2F histograms defined above
   TGraphErrors *asymgraph;

   for (int spin=1; spin>=-1; spin-=2 ) {

      // Selectively disable bunch ID by matching spin pattern
      for (int bid=0; bid<N_BUNCHES; bid++) {
         bunch[bid] = (gSpinPattern[bid] == spin ? bid : -1);
      }

      // X90
      asymgraph = AsymmetryGraph(spin, N_BUNCHES, bunch, gBunchAsym.Ax90[0], ex, gBunchAsym.Ax90[1]);
      asymgraph->SetName("asymgraph");
      FillAsymmetryHistgram("x90", spin, N_BUNCHES, gBunchAsym.Ax90[0], gBunchAsym.Ax90[1], bunch);
      asym_vs_bunch_x90->GetListOfFunctions() -> Add(asymgraph,"p");
      asym_vs_bunch_x90->GetXaxis()->SetTitle("Bunch Number");
      asym_vs_bunch_x90->GetYaxis()->SetTitle("Raw Asymmetry ");

      // X45
      asymgraph = AsymmetryGraph(spin, N_BUNCHES, bunch, gBunchAsym.Ax45[0], ex, gBunchAsym.Ax45[1]);
      FillAsymmetryHistgram("x45", spin, N_BUNCHES, gBunchAsym.Ax45[0], gBunchAsym.Ax45[1], bunch);
      asymgraph -> SetName("asymgraph");
      asym_vs_bunch_x45->GetListOfFunctions() -> Add(asymgraph,"p");
      asym_vs_bunch_x45->GetXaxis()->SetTitle("Bunch Number");
      asym_vs_bunch_x45->GetYaxis()->SetTitle("Raw Asymmetry ");

      // Y45
      asymgraph = AsymmetryGraph(spin, N_BUNCHES, bunch, gBunchAsym.Ay45[0], ex, gBunchAsym.Ay45[1]);
      FillAsymmetryHistgram("y45", spin, N_BUNCHES, gBunchAsym.Ay45[0], gBunchAsym.Ay45[1], bunch);
      asymgraph->SetName("asymgraph");
      asym_vs_bunch_y45->GetListOfFunctions()->Add(asymgraph, "p");
      asym_vs_bunch_y45->GetXaxis()->SetTitle("Bunch Number");
      asym_vs_bunch_y45->GetYaxis()->SetTitle("Raw Asymmetry ");
   }

   // bunch asymmetry averages
   calcBunchAsymmetryAverage();
} //}}}


// Description : calculate asymmetries bunch by bunch
// Input       : int Mode0[Ax90], 1[Ax45], 2[Ay45]
// Return      : Asym[N_BUNCHES], dA[N_BUNCHES]
void AsymCalculator::BunchAsymmetry(int Mode, float *A, float *dA)
{ //{{{
   // Allocate adequate detector IDs involved in X90,X45,Y45, respectively
   int Rdet[2], Ldet[2];
   set<UShort_t> RDetectors;
   set<UShort_t> LDetectors;

   switch (Mode) {
   case 0:  // Ax90
      Rdet[0] = 1; RDetectors.insert(2);
      Ldet[0] = 4; LDetectors.insert(5);
      break;
   case 1:  // Ax45
      Rdet[0] = 0; Rdet[1] = 2; RDetectors.insert(1); RDetectors.insert(2);
      Ldet[0] = 3; Ldet[1] = 5; LDetectors.insert(4); LDetectors.insert(6);
      break;
   case 2:  // Ay45
      Rdet[0] = 0; Rdet[1] = 5; RDetectors.insert(1); RDetectors.insert(6);
      Ldet[0] = 2; Ldet[1] = 3; LDetectors.insert(3); LDetectors.insert(4);
      break;
   default:
      cerr << "BunchAsymmetry: No matching mode is coded in for " << Mode << endl;
      return;
   }

   // Calculate detector luminosities by taking sum over R/L detectors and all
   // bunches
   int LumiR = 0;
   int LumiL = 0;

   // If more than two detectors combine their counts
   //for (int i=0; i<=1; i++) { // run for X45 and Y45
   //   for (int bid=0; bid<N_BUNCHES; bid++) {
   //      if (gFillPattern[bid]){
   //         LumiR += Ncounts[Rdet[i]][bid];
   //         LumiL += Ncounts[Ldet[i]][bid];
   //      }
   //   }
   //   if (!Mode) break; // no R/L detector loop for x90
   //}

   TH2* hDetVsBunchId = (TH2*) gAsymRoot->fHists->d["asym"]->o["hDetVsBunchId"];

   set<UShort_t>::iterator iRDet = RDetectors.begin();
   set<UShort_t>::iterator iLDet = LDetectors.begin();

   for ( ; iRDet!=RDetectors.end(); ++iRDet)
      LumiR += hDetVsBunchId->Integral(1, N_BUNCHES, *iRDet, *iRDet);

   for ( ; iLDet!=RDetectors.end(); ++iLDet)
      LumiL += hDetVsBunchId->Integral(1, N_BUNCHES, *iLDet, *iLDet);

   // Main bunch loop to calculate asymmetry bunch by bunch
   for (int bid=0; bid<N_BUNCHES; bid++) {

      int R = 0, L = 0;
       A[bid] = ASYM_DEFAULT;
      dA[bid] = 0;

      if (gFillPattern[bid]) {

         // Take sum of Up/Down for Right and Left detectors
         for (int i=0; i<=1; i++) {
            R += Ncounts[Rdet[i]][bid];
            L += Ncounts[Ldet[i]][bid];
            if (!Mode) break; // no detector loop for X90
         }

         //set<UShort_t>::iterator iRDet = RDetectors.begin();

         //for ( ; iRDet!=RDetectors.end(); ++iRDet)

         AsymCalculator::CalcAsymmetry(R, L, LumiR, LumiL, A[bid], dA[bid]);
      }
   }
} //}}}


// Description : calculate average left-right and top-bottom asymmetry from bunch by bunch asymmetris.
//             : also calculate phase from LR and Top-Bottom asymmetry
void calcBunchAsymmetryAverage()
{ //{{{
   // flip the sign of negative spin bunches
   for (int i=0; i<N_BUNCHES; i++) {
      if (gSpinPattern[i] == -1) {
         gBunchAsym.Ax90[0][i] *= -1;
         gBunchAsym.Ax45[0][i] *= -1;
         gBunchAsym.Ay45[0][i] *= -1;
      }
   }

   // Calculate weighgted beam for Ax90, Ax45, Ay45 combinations
   CalcWeightedMean(gBunchAsym.Ax90[0], gBunchAsym.Ax90[1], N_BUNCHES, gBunchAsym.ave.Ax90[0], gBunchAsym.ave.Ax90[1]);
   CalcWeightedMean(gBunchAsym.Ax45[0], gBunchAsym.Ax45[1], N_BUNCHES, gBunchAsym.ave.Ax45[0], gBunchAsym.ave.Ax45[1]);
   CalcWeightedMean(gBunchAsym.Ay45[0], gBunchAsym.Ay45[1], N_BUNCHES, gBunchAsym.ave.Ay45[0], gBunchAsym.ave.Ay45[1]);

   // Calculate Left-Right asymmetry using Ax90 and Ax45
   calcLRAsymmetry(gBunchAsym.ave.Ax90, gBunchAsym.ave.Ax45, gBunchAsym.ave.Ax[0], gBunchAsym.ave.Ax[1]);

   // Calculate Top-Bottom asymmetry using Ay45
   gBunchAsym.ave.Ay[0] = gBunchAsym.ave.Ay45[0] * M_SQRT2;
   gBunchAsym.ave.Ay[1] = gBunchAsym.ave.Ay45[1] * M_SQRT2;

   // Calculate phase (spin pointing vector w.r.t. vertical axis) from Ax and Ay
   if (gBunchAsym.ave.Ay[0]) {
      float r_xy = gBunchAsym.ave.Ax[0]/gBunchAsym.ave.Ay[0];
      gBunchAsym.ave.phase[0] = atan(r_xy) - M_PI_2;
   } else {
      gBunchAsym.ave.phase[0] = 0;
   }

   gBunchAsym.ave.phase[1] = CalcDivisionError(gBunchAsym.ave.Ax[0], gBunchAsym.ave.Ay[0], gBunchAsym.ave.Ax[1], gBunchAsym.ave.Ay[1]);
   gBunchAsym.ave.phase[1] = atan(gBunchAsym.ave.phase[1]) ? fabs( atan(gBunchAsym.ave.phase[1]) - M_PI_2 ) : 0 ;

   // Calculate Polarization
   if (gAnaResult->A_N[1]) {
      gAnaResult->basym[0].P[0] = gBunchAsym.ave.Ax[0]/gAnaResult->A_N[1];
      gAnaResult->basym[0].P[1] = gBunchAsym.ave.Ax[1]/gAnaResult->A_N[1];
   } else {
      gAnaResult->basym[0].P[0] = 0;
      gAnaResult->basym[0].P[1] = 0;
   };

   cout << "========== bunch asymmetry average ===========" << endl;
   cout << gBunchAsym.ave.Ax90[0] << " " << gBunchAsym.ave.Ax90[1] << " "
        << gBunchAsym.ave.Ax45[0] << " " << gBunchAsym.ave.Ax45[1] << " "
        << gBunchAsym.ave.Ay45[0] << " " << gBunchAsym.ave.Ay45[1] << " " << endl;
   cout << gBunchAsym.ave.Ax[0]   << " " << gBunchAsym.ave.Ax[1] << endl << endl;
} //}}}


// Description : Calculate Left-Right asymmetry from X90 and X45 asymmetry combination
//             :
// Input       : float X90[2], float X45[2]
// Return      : LR asymmetry and error : float &A, float &dA
void calcLRAsymmetry(float X90[2], float X45_tmp[2], float &A, float &dA)
{ //{{{
   float X45[2] = {0, 0};

   // give sqrt(2) weight to X45 asymmetry
   X45[0] = X45_tmp[0] * M_SQRT2;
   X45[1] = X45_tmp[1] * M_SQRT2;

   // if horizontal target or X90 is zero, then return X45*sqrt(2) as LR asymmetry
   if (tgt.VHtarget) {
       A = X45[0];
      dA = X45[1];
      return;
   }

   // define error squares
   float dX45 = X45[1]*X45[1];
   float dX90 = X90[1]*X90[1];

   // calculate weighted mean of X90 and X45 asymetries
   if (dX90 + dX45 != 0 ) {
       A = (X90[0] * dX45 + X45[0] * dX90) / (dX90 + dX45);
      dA = (X90[1] * X45[1]) / sqrt(dX90 + dX45);
   } else {
      printf("WARNING: Denominator is zero in calcLRAsymmetry(). Assign zero LR asymmetry\n");
      A = dA = 0;
   }
} //}}}


// Description : call calcStripAsymmetry() subroutines for
//             : regular and alternative sigma banana cuts, respectively.
//             : Also call for PHENIX and STAR colliding bunches asymmetries
void StripAsymmetry(MseRunInfoX &run)
{ //{{{
   //// Calculate Asymmetries for colliding bunches at PHENIX
   //CalcStripAsymmetry(gAnaResult->A_N[1], 2);

   //// Calculate Asymmetries for colliding bunches at STAR
   //CalcStripAsymmetry(gAnaResult->A_N[1], 3);

   //// alternative sigma cut
   //CalcStripAsymmetry(gAnaResult->A_N[1], 1);

   // regular sigma cut
   CalcStripAsymmetry(gAnaResult->A_N[1], 0);

   // Add info to database entry
   char sPol[50];

   sprintf(sPol, "% 8.3f, % 6.3f, % 8.3f, % 6.3f",
      gAnaResult->sinphi[0].P[0],    gAnaResult->sinphi[0].P[1],
      gAnaResult->sinphi[0].dPhi[0], gAnaResult->sinphi[0].dPhi[1]);

   gRunDb.fFields["POLARIZATION"] = sPol;
   run.polarization       = gAnaResult->sinphi[0].P[0];
   run.polarization_error = gAnaResult->sinphi[0].P[1],
   run.phase              = gAnaResult->sinphi[0].dPhi[0];
   run.phase_error        = gAnaResult->sinphi[0].dPhi[1];

   //printf("XXX %f\n", run.polarization);

   // Some consistency checks for different sigma cuts
   if (gAnaResult->sinphi[0].P[0]) {

      float diff[2];

      // calculate differences and ratio
      diff[0] = gAnaResult->sinphi[1].P[0] - gAnaResult->sinphi[0].P[0];

      gAnaResult->P_sigma_ratio[0]      = gAnaResult->sinphi[1].P[0] / gAnaResult->sinphi[0].P[0];
      gAnaResult->P_sigma_ratio_norm[0] = diff[0] / gAnaResult->sinphi[0].P[0];

      // calculate errors for above, respectively
      diff[1] = QuadErrorSum(gAnaResult->sinphi[1].P[1], gAnaResult->sinphi[0].P[1]);

      gAnaResult->P_sigma_ratio[1] =
         QuadErrorDiv(gAnaResult->sinphi[1].P[0], gAnaResult->sinphi[0].P[0], gAnaResult->sinphi[1].P[1], gAnaResult->sinphi[0].P[1]);

      gAnaResult->P_sigma_ratio_norm[1] =
         QuadErrorDiv(diff[0], gAnaResult->sinphi[0].P[0], diff[1], gAnaResult->sinphi[0].P[1]);
   }

   // Calculate asymmetries for each target position
   TH1 *hpp = 0;

   if (gAnaInfo->HasProfileBit()) {
      hpp = (TH1*) gAsymRoot->fHists->d["profile"]->o["hPolarProfile"];
   }

   for(Int_t i=0; i<gNDelimeters; i++) {

      CalcStripAsymmetry(gAnaResult->A_N[1], 100+i);
      //printf("i, p: %d, %f\n", i, gAnaResult->sinphi[100+i].P[0]);

      if (hpp) {
         hpp->SetBinContent(i+1, gAnaResult->sinphi[100+i].P[0]);
         hpp->SetBinError(i+1, gAnaResult->sinphi[100+i].P[1]);
      }
   }

   if (gAnaInfo->HasProfileBit()) gAsymRoot->ProcessProfileHists();

   run.profile_ratio       = gAnaResult->fIntensPolarR;
   run.profile_ratio_error = gAnaResult->fIntensPolarRErr;
} //}}}


/**
 * Calculates asymmetries strip by strip. This method updates the global object
 * gAnaResult with a calculated polarization value for the give strip.
 */
void CalcStripAsymmetry(float aveA_N, int Mode)
{ //{{{
   //ds : Overwrite nstrip array
   long int nstrip[N_SPIN_STATES][N_SILICON_CHANNELS];

   int ss_code = 0;

   // only for Mode < 100
   if (Mode < 100) {
      TH2I* hSpVsCh = (TH2I*) gAsymRoot->fHists->d["std"]->o["hSpinVsChannel_cut2"];

      for (int ix=1; ix<=hSpVsCh->GetNbinsX(); ix++) { // channels
         for (int iy=1; iy<=hSpVsCh->GetNbinsY(); iy++) { // spin

            double bcont  = hSpVsCh->GetBinContent(ix, iy);
            //double bXcntr = hSpVsCh->GetBinCenter(ix, iy);

            ss_code = iy == 3 ? 0 : (iy == 1 ? 1 : 2); // spin down

            nstrip[ss_code][ix-1] = bcont;
         }
      }
   } else if (Mode >= 100) {

      UInt_t iDelim = Mode - 100;
      char dName[256], hName[256];

      // loop over strips and update nstrip array content
      for (int i=1; i<=N_SILICON_CHANNELS; i++) {

         sprintf(dName, "channel%02d", i);

         DrawObjContainer* oc = gAsymRoot->fHists->d["std"]->d.find(dName)->second;

         sprintf(hName, "hSpinVsDelim_cut2_ch%02d", i);

         TH1* hSpVsDelim = (TH1*) oc->o[hName];

         for (int iy=1; iy<=hSpVsDelim->GetNbinsY(); iy++) { // spin

            double bcont  = hSpVsDelim->GetBinContent(iDelim+1, iy);

            ss_code = iy == 3 ? 0 : (iy == 1 ? 1 : 2); // spin down

            nstrip[ss_code][i-1] = bcont;
         }
      }
   }

   // Strip-by-Strip Asymmetries
   int   LumiSum[2][N_SILICON_CHANNELS];          // Total Luminosity [0]:Spin Up, [1]:Spin Down
   float LumiSum_r[2][N_SILICON_CHANNELS];        // Reduced order Total luminosity for histograming
   float LumiRatio[N_SILICON_CHANNELS];           // Luminosity Ratio
   float Asym[N_SILICON_CHANNELS], dAsym[N_SILICON_CHANNELS]; // Raw Asymmetries strip-by-strip
   float AsymPhiCorr[N_SILICON_CHANNELS], dAsymPhiCorr[N_SILICON_CHANNELS]; // Phi corrected Asymmetries strip-by-strip
   float P[N_SILICON_CHANNELS],    dP[N_SILICON_CHANNELS];    // Strip phi corrected polarization
   float Pt[N_SILICON_CHANNELS],   dPt[N_SILICON_CHANNELS];   // Strip phi truncated corrected polarization,
   long  counts[2];                   // local counter variables

   for (int i=0; i<N_SILICON_CHANNELS; i++) {

      Asym[i]         = dAsym[i]        = 0;
      AsymPhiCorr[i]  = dAsymPhiCorr[i] = 0;
      RawP[i]         = dRawP[i]        = 0;
      LumiSum_r[0][i] = LumiSum_r[0][i] = LumiRatio[i] = 0;
      LumiSum[0][i]   = LumiSum[1][i]   = 0;

      // Loop for Total Luminosity
      for (int j=0; j<N_SILICON_CHANNELS; j++) {

         // Calculate luminosity. This strip and ones in cross geometry are excluded.
         if (!AsymCalculator::ExcludeStrip(i, j)) {
            for (int k=0; k<=1; k++) LumiSum[k][i] += nstrip[k][j];
         }
      }

      counts[0] = nstrip[0][i]; // spin up
      counts[1] = nstrip[1][i]; // spin down

      //printf("XXX: %3d, %8.5f, %10ld, %10ld, %10d, %10d, %8.5e, %8.5e\n",
      //       i, aveA_N, counts[0], counts[1], LumiSum[0][i], LumiSum[1][i],
      //       Asym[i], dAsym[i]);

      // Luminosity Ratio
      LumiRatio[i] = float(LumiSum[0][i]) / float(LumiSum[1][i]);

      // Calculate raw asymmetries for the i-th strip
      if ( LumiSum[1][i] && counts[0] + counts[1] ) {

         // Calculate Asym and dAsym
         AsymCalculator::CalcAsymmetry(counts[0], counts[1], LumiSum[0][i], LumiSum[1][i], Asym[i], dAsym[i]);

         //printf("YYY: %3d, %8.5f, %10ld, %10ld, %10d, %10d, %8.5e, %8.5e\n",
         //       i, aveA_N, counts[0], counts[1], LumiSum[0][i], LumiSum[1][i],
         //       Asym[i], dAsym[i]);
      } else {
          Asym[i] = 0;
         dAsym[i] = 1e6;
      }

      // Reduced Order Luminosity for histograms. Histogram scale is given in float, not double.
      // Cannot accomomdate large entry.
      LumiSum_r[0][i] = LumiSum[0][i]/1e3;
      LumiSum_r[1][i] = LumiSum[1][i]/1e3;

      // Since this is the recoil asymmetries, flip the sign of asymmetry
      Asym[i] *= -1;

      // Asymmetry with sin(phi) correction
       AsymPhiCorr[i] = Asym[i] / sin(-gPhi[i]);
      dAsymPhiCorr[i] = fabs( dAsym[i] / sin(-gPhi[i]) );

      // Raw polarization without phi angle weighted A_N
       RawP[i] = (aveA_N != 0 ? ( Asym[i] / aveA_N) : 0);
      dRawP[i] = (aveA_N != 0 ? (dAsym[i] / aveA_N) : 0);

      // Polarization with sin(phi) correction
       P[i] = RawP[i] / sin(-gPhi[i]);
      dP[i] = fabs(dRawP[i] / sin(-gPhi[i]));

      // Polarization with trancated sin(phi) correction
       Pt[i] = RawP[i] / sin(-phit[i]);
      dPt[i] = fabs(dRawP[i] / sin(-phit[i]));

      // ds temp fix: give huge errors to disabled strips
      //if ( gRunInfo->IsDisabledChannel(i+1) )    // || gRunInfo->IsHamaChannel(i+1) ) 
      if (gAsymRoot->fEventConfig->fCalibrator->GetFitStatus(i+1) != kDLFIT_OK) {

         Asym[i]  =  RawP[i] =  P[i] =  Pt[i] = 0;
         AsymPhiCorr[i] = 0;
         dAsym[i] = dRawP[i] = dP[i] = dPt[i] = 1e6;
         dAsymPhiCorr[i] = 1e6;
      }

      //printf("ZZZ: %3d, %8.5f, %10ld, %10ld, %10d, %10d, %8.5e, %8.5e\n",
      //       i, aveA_N, counts[0], counts[1], LumiSum[0][i], LumiSum[1][i],
      //       Asym[i], dAsym[i]);
   }

   // printing routine
   //if (Flag.VERBOSE)
   if (0) {
      printf("*========== strip by strip =============\n");

      for (int i=0; i<N_SILICON_CHANNELS; i++) {
         printf("%4d", i);
         printf("%12.5f", gPhi[i]);
         printf("%12.5f %12.5e", Asym[i], dAsym[i]);
         printf("%12.5f %12.5e", RawP[i], dRawP[i]);
         printf("%12.5f %12.5e",    P[i],    dP[i]);
         printf("%12.5f %12.5e",   Pt[i],   dPt[i]);
         printf("\n");
      }

      printf("*=======================================\n");
      printf("\n");
   }

   // Caluclate Weighted Average
   CalcWeightedMean(P, dP, N_SILICON_CHANNELS, gAnaResult->P[0], gAnaResult->P[1]);

   CalcWeightedMean(AsymPhiCorr, dAsymPhiCorr, N_SILICON_CHANNELS, gAnaResult->fAvrgPMAsym, gAnaResult->fAvrgPMAsymErr);

   //printf("P0, P1: %8.5f %8.5f\n", gAnaResult->P[0], gAnaResult->P[1]);

   //ds:// Histrograming
   //ds:HHPAK(36010, LumiSum_r[0]);  HHPAK(36110, LumiSum_r[1]);
   //ds:HHPAK(36200, LumiRatio);
   //ds:HHPAK(36210, Asym);  HHPAKE(36210, dAsym);
   //ds:HHPAK(36220, RawP);  HHPAKE(36220, dRawP);
   //ds:HHPAK(36240, P);     HHPAKE(36240, dP);
   //ds:HHPAK(36250, gPhi);

   // Fit polarization vs phi distributions
   if (Mode < 100) { // Fit everything other than scan data

      gAsymCalculator.SinPhiFit(gAnaResult->P[0], RawP, dRawP,
         gAnaResult->sinphi[Mode].P, gAnaResult->sinphi[Mode].dPhi, gAnaResult->sinphi[Mode].chi2);

   } else if (Mode >= 100) { // Fit scan data
      gAsymCalculator.ScanSinPhiFit(gAnaResult->P[0], RawP, dRawP,
         gAnaResult->sinphi[Mode].P, gAnaResult->sinphi[Mode].dPhi, gAnaResult->sinphi[Mode].chi2);
   }

   //gAsymCalculator.SinPhiFit(gAnaResult->P[0], gAnaResult->sinphi.P, gAnaResult->sinphi.dPhi, gAnaResult->sinphi.chi2);
} //}}}


// Description : Master Routine for sin(phi) root-fit
// Input       : Float_t p0 (1-par Polarization for par[0] initialization)
//             : Float_t *RawP, Float_t *dRawP, Float_t *phi (vectors to be fit)
// Return      : Float_t *P, Float_t *dphi, Float_t &chi2dof
void AsymCalculator::SinPhiFit(Float_t p0, Float_t *RawP, Float_t *dRawP,
                        Float_t *P, Float_t *phase, Float_t &chi2dof)
{ //{{{
   char  htitle[100];
   float dx[N_SILICON_CHANNELS];

   for (int i=0; i<N_SILICON_CHANNELS; i++) dx[i] = 0;

   // define TH2D sin(phi) histogram
   Asymmetry->cd();

   float min, max;
   float prefix = 0.3;
   float margin = 0.3;

   GetMinMaxOption(prefix, N_SILICON_CHANNELS, RawP, margin, min, max);

   sprintf(htitle, "Run%.3f: Strip Asymmetry Fit", gRunInfo->RUNID);

   //asym_sinphi_fit = (TH2F*) gAsymRoot->fHists->d["run"]->o["asym_sinphi_fit"];
   asym_sinphi_fit->SetName("asym_sinphi_fit");
   asym_sinphi_fit->SetTitle(htitle);
   asym_sinphi_fit->SetBins(100, 0, 2*M_PI, 100, min, max);
   asym_sinphi_fit->GetXaxis()->SetTitle("#phi");
   asym_sinphi_fit->GetYaxis()->SetTitle("Polarization, %");

   DrawHorizLine(asym_sinphi_fit, 0, 2*M_PI, 0, 1, 1, 1);

   // define sin(phi) fit function & initialize parmaeters
   TF1 *func = new TF1("sin_phi", "[0]*TMath::Sin([1]-x)", 0, 2*M_PI);

   func->SetLineColor(kRed);
   func->SetParNames("P", "#Delta#phi");
   func->SetParameter(0, p0);
   func->SetParameter(1, 0);
   //func->SetParLimits(0, -1, 1);
   func->SetParLimits(0, 0, 1);
   func->SetParLimits(1, -M_PI, M_PI);

   // define TGraphError obect for fitting
   TGraphErrors *tg = AsymmetryGraph(1, N_SILICON_CHANNELS, gPhi, RawP, dx, dRawP);

   // Perform sin(phi) fit
   tg->Fit("sin_phi", "R");
   tg->SetName("tg");

   // Dump TGraphError obect to TH2D histogram
   asym_sinphi_fit->GetListOfFunctions()->Add(tg, "p");

   //delete tg;

   // Get fitting results
   P[0]     = func->GetParameter(0);
   P[1]     = func->GetParError(0);
   phase[0] = func->GetParameter(1);
   phase[1] = func->GetParError(1);
   chi2dof  = func->GetChisquare()/func->GetNDF();

   delete func;

   // Write out fitting results on plot
   char text[80];
   TLatex *txt;

   sprintf(text, "(%3.2f #pm %3.2f) * sin#left(#phi + (%+5.2f #pm %3.2f)#right)", P[0], P[1], phase[0], phase[1]);
   txt = new TLatex(0.3, max*0.85, text);
   asym_sinphi_fit->GetListOfFunctions()->Add(txt);
   //delete txt;

   sprintf(text, "#chi^{2} / ndf = %5.2f", chi2dof);
   txt = new TLatex(0.3, max*0.7, text);
   asym_sinphi_fit->GetListOfFunctions()->Add(txt);
   //delete txt;
} //}}}


// April, 2008; Introduced by Vipuli to keep dphi fixed at gAnaResult->sinphi[0].dPhi[0]
// when doing the fit to data taken at each scaned position
//
// Description : Master Routine for sin(phi) root-fit
// Input       : Float_t p0 (1-par Polarization for par[0] initialization)
//             : Float_t *RawP, Float_t *dRawP, Float_t *phi (vectors to be fit)
// Return      : Float_t *P, Float_t *phase, Float_t &chi2dof
//
void AsymCalculator::ScanSinPhiFit(Float_t p0, Float_t *RawP, Float_t *dRawP,
                            Float_t *P, Float_t *phase, Float_t &chi2dof)
{ //{{{
   float dx[N_SILICON_CHANNELS];

   for (int i=0; i<N_SILICON_CHANNELS; i++) dx[i] = 0;

   float min, max;
   float prefix = 0.3;
   float margin = 0.3;

   GetMinMaxOption(prefix, N_SILICON_CHANNELS, RawP, margin, min, max);

   char htitle[100];
   sprintf(htitle, "Run%.3f: Strip Asymmetry Fit", gRunInfo->RUNID);

   //scan_asym_sinphi_fit = new TH2F("scan_asym_sinphi_fit",htitle, 100, 0, 2*M_PI, 100, min, max);
   scan_asym_sinphi_fit->SetName("scan_asym_sinphi_fit");
   scan_asym_sinphi_fit->SetTitle(htitle);
   scan_asym_sinphi_fit->SetBins(100, 0, 2*M_PI, 100, min, max);
   scan_asym_sinphi_fit->GetXaxis()->SetTitle("#phi");
   scan_asym_sinphi_fit->GetYaxis()->SetTitle("Polarization");

   DrawHorizLine(scan_asym_sinphi_fit, 0, 2*M_PI, 0, 1, 1, 1);

   // define sin(phi) fit function & initialize parmaeters
   TF1 *func = new TF1("sin_phi", "[0]*TMath::Sin([1]-x)", 0, 2*M_PI);

   func->SetLineColor(kRed);
   func->SetParNames("P", "#Delta#phi");
   func->SetParameter(0, p0);
   func->SetParameter(1, 0);
   //func->SetParLimits(0, -1, 1);
   func->SetParLimits(0, 0, 1);
   //func->SetParLimits(1, gAnaResult->sinphi[0].dPhi[0], gAnaResult->sinphi[0].dPhi[0]);
   //func->SetParLimits(1,-M_PI, M_PI);
   // Keeping phi fixed - chnged by Vipuli April 2008
   //printf("************************************ \n");
   //printf("keeping phi fixed at = %12.3e \n", gAnaResult->sinphi[0].dPhi[0]);
   //printf("************************************ \n");
   func->FixParameter(1, gAnaResult->sinphi[0].dPhi[0]);

   // define TGraphError obect for fitting
   TGraphErrors *tg = AsymmetryGraph(1, N_SILICON_CHANNELS, gPhi, RawP, dx, dRawP);

   // Perform sin(phi) fit
   tg->Fit("sin_phi", "R");
   tg->SetName("tg");

   // Dump TGraphError obect to TH2D histogram
   scan_asym_sinphi_fit->GetListOfFunctions()->Add(tg, "p");

   delete tg;

   // Get fitting results
   P[0]     = func->GetParameter(0);
   P[1]     = func->GetParError(0);
   phase[0] = func->GetParameter(1);
   phase[1] = func->GetParError(1);
   chi2dof  = func->GetChisquare()/func->GetNDF();

   delete func;

   // Write out fitting results on plot
   char text[80];
   TLatex *txt;

   sprintf(text, "(%3.2f +/- %3.2f) * sin(phi %+5.2f +/- %3.2f)", P[0], P[1], phase[0], phase[1]);
   txt = new TLatex(0.3, max*0.85, text);
   scan_asym_sinphi_fit->GetListOfFunctions()->Add(txt);
   delete txt;

   sprintf(text, "#chi^{2} / ndf = %5.2f", chi2dof);
   txt = new TLatex(0.3, max*0.7, text);
   scan_asym_sinphi_fit->GetListOfFunctions()->Add(txt);
   delete txt;
} //}}}


/*       following 3 subroutines are unsuccessful MINUIT sin(phi) fit routines.

//
// Class name  :
// Method name : fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
//
// Description : Function returns chi2 for MINUIT
// Input       :
// Return      : chi2
//
void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{ //{{{
   AsymCalculator asymfit;
   //calculate chisquare
   Float_t chisq = 0;
   Float_t delta;
   for (Int_t i=0;i<N_SILICON_CHANNELS; i++) {
     if (dRawP[i]) delta  = (RawP[i]-asymfit.sinx(phi[i],par))/dRawP[i];
     chisq += delta*delta;
   }

   FitChi2 = chisq;
   f = chisq;
} //}}}


// Description : sin(x) fit. Amplitude and phase as parameters
// Input       : Float_t x, Double_t *par
// Return      : par[0]*sin(x+par[1])
//
Float_t AsymCalculator::sinx(Float_t x, Double_t *par)
{ //{{{
   Float_t value=par[0]*sin(-x+par[1]);
   return value;
} //}}}


// Description : Master Routine for MINUIT call.
// Input       : Float_t p0 (1-par Polarization for par[0] initialization)
// Return      : Float_t *P, Float_t *phi, Float_t &chi2dof
//
void AsymCalculator::SinPhiFit(Float_t p0, Float_t *P, Float_t *phi, Float_t &chi2dof)
{ //{{{
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
   for (Int_t i=0;i<N_SILICON_CHANNELS; i++) NDATA += dRawP[i] ? 1 : 0;
   chi2dof = FitChi2/Float_t(NDATA-NPAR);

   return;
} //}}}


// Description : Retrun parameter errors for parameter NUM
// Input       : TMinuit *gMinuit, Int_t NUM
// Return      : error
//
Double_t AsymCalculator::GetFittingErrors(TMinuit *gMinuit, Int_t NUM)
{ //{{{
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
} //}}}
*/


//
// Class name  : RAMP
// Method name : CalcRAMP()
//
// Description : Not being use.
// Input       :
// Return      :
/*
void RAMP::CalcRAMP()
{ //{{{
  if (gAnaInfo->RAMPMODE==1) {

    for (int dlm=0;dlm<RAMPTIME;dlm++){
      // not need for initialization for RUN,RD,LU,LD
      // they are initialized at bid=0
      memset(SIU,0,sizeof(SIU));
      memset(SID,0,sizeof(SID));

      for (bid=0;bid<120;bid++){
        for (si=0;si<6;si++){
          SIU[si] += (NRcounts[si][bid][dlm])
            *((gSpinPattern[bid]==1)?1:0)*gbid[bid];
          SID[si] += (NRcounts[si][bid][dlm])
            *((gSpinPattern[bid]==-1)?1:0)*gbid[bid];
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
} //}}}
*/
