//  Asymmetry Analysis of RHIC pC Polarimeter
//  End-of-Run routine
//  file name :   AsymCalc.cc
//
//  Author    :   Itaru Nakagawa
//  Creation  :   01/21/2006
//

#include "AsymCalc.h"

using namespace std;

const int ASYM_DEFAULT = -999;

float RawP[72], dRawP[72]; // Raw Polarization (Not corrected for phi)


// =========================
// End of data process
// =========================
void end_process()
{
   StructHistStat hstat;

   if (dproc.CMODE) {
      //gAsymRoot.PostProcess();
   }

   // Feedback Mode
   if (Flag.feedback) {

      int FeedBackLevel = 2; // 1: no fit just max and mean
                             // 2: gaussian fit
      gAnaResults.TshiftAve = TshiftFinder(Flag.feedback, FeedBackLevel);

      // reset counters
      Nevtot = Nread = 0;
      for (int i=0; i<120; i++) Ntotal[i] = 0;
   }

   // Calculate Statistics
   CalcStatistics();

   // Skip end-of-run analysis routines for deadlayer and calibration modes.
   //if (!dproc.DMODE) {

      // Energy Yeild Weighted Average Analyzing Power
      //gAnaResults.A_N[0] = WeightAnalyzingPower(10040); // no cut in energy spectra
      gAnaResults.A_N[1] = WeightAnalyzingPower(10050); // banana cut in energy spectra

      if (!gAnaResults.A_N[1]) {
         printf("Zero analyzing power\n");
         return;
      }

      // CumulativeAsymmetry();

      // Specific Luminosity (ds: this one is not used in asymmetry calculation)
      SpecificLuminosity(hstat.mean, hstat.RMS, hstat.RMSnorm);

      // Bunch Asymmetries
      //CalcBunchAsymmetry();

      // Strip-by-Strip Asymmetries
      StripAsymmetry();

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

      // target associated histograms
      TgtHist();

      // Draw polarization vs target position
      DrawPlotvsTar();
   //}

   // Run Informations
   PrintWarning();
   PrintRunResults(hstat);
}


// Method name : CompleteHistograms()
// Description : Complete dressing histograms
void CompleteHistogram()
{
   // Draw reg./alt. event selection borders in Invariant Mass plots
   float MASS_12C_k2G = MASS_12C*k2G;
 
   for (int i=0; i<NSTRIP; i++) {
      float max = mass_nocut[i]->GetMaximum();
 
      DrawLine(mass_nocut[i], MASS_12C_k2G, max*1.05, 14, 2);
      DrawLine(mass_nocut[i], MASS_12C_k2G + feedback.RMS[i] * k2G * dproc.MassSigma,    max*0.3, 4, 2);
      DrawLine(mass_nocut[i], MASS_12C_k2G - feedback.RMS[i] * k2G * dproc.MassSigma,    max*0.3, 4, 2);
      DrawLine(mass_nocut[i], MASS_12C_k2G + feedback.RMS[i] * k2G * dproc.MassSigma,    max*0.3, 4, 2);
      DrawLine(mass_nocut[i], MASS_12C_k2G - feedback.RMS[i] * k2G * dproc.MassSigma,    max*0.3, 4, 2);
      DrawLine(mass_nocut[i], MASS_12C_k2G + feedback.RMS[i] * k2G * dproc.MassSigmaAlt, max*0.3, 4, 1);
      DrawLine(mass_nocut[i], MASS_12C_k2G - feedback.RMS[i] * k2G * dproc.MassSigmaAlt, max*0.3, 4, 1);
   }
}


// Description : Make target histograms
void TgtHist()
{
   char  htitle[100];
   float xmin   = fabs(double(ASYM_DEFAULT)), xmax;
   float margin = 0.02;
   float dx[MAXDELIM], y[MAXDELIM], dy[MAXDELIM];
   int   X_index = runinfo.Run >= 6 ? nTgtIndex : ndelim;
 
   //GetMinMax(nTgtIndex, tgt.X, margin, xmin, xmax);
   GetMinMax(X_index, tgt.X, margin, xmin, xmax);

   printf("xmin, xmax: %f, %f, %d, %d\n", xmin, xmax, X_index, ndelim);
 
   Run->cd();

   TH1* h = (TH1*) gAsymRoot.fHists->d["run"]->o["hTargetSteps"];
 
   // Make rate _vs deliminter plots
   for (int i=0; i<X_index; i++) {

     UInt_t count = h->GetBinContent(i+1);

     dx[i] = 0;
     // y[i] = tgt.Interval[i] ? float(cntr.good[i]) / tgt.Interval[i] * MHz : 0 ;
     //dy[i] = tgt.Interval[i] ? float( sqrt( double(cntr.good[i]) ) ) / tgt.Interval[i] * MHz : 0;

      y[i] = tgt.Interval[i] ? float(count) / tgt.Interval[i] * MHz : 0 ;
     dy[i] = tgt.Interval[i] ? float(sqrt(count)) / tgt.Interval[i] * MHz : 0;
   }
 
   gAnaResults.max_rate = GetMax(X_index, y);

   float ymin = fabs(double(ASYM_DEFAULT)), ymax;

   GetMinMax(X_index, y, margin, ymin, ymax);

   sprintf(htitle,"%.3f : Rate vs Taret Postion", runinfo.RUNID);
 
   //rate_vs_delim = new TH2F("rate_vs_delim", htitle, 100, xmin, xmax, 100, ymin, ymax);
   rate_vs_delim->SetName("rate_vs_delim");
   rate_vs_delim->SetTitle("rate_vs_delim");
   rate_vs_delim->SetBins(100, xmin, xmax, 100, ymin, ymax);
   rate_vs_delim->GetXaxis()->SetTitle("Target Position");
   rate_vs_delim->GetYaxis()->SetTitle("Carbon in Banana Rate[MHz]");
 
   //TGraphErrors *rate_delim = new TGraphErrors(nTgtIndex, tgt.X, y, dx, dy);
   TGraphErrors *rate_delim = new TGraphErrors(X_index, tgt.X, y, dx, dy);
   rate_delim->SetMarkerStyle(20);
   rate_delim->SetMarkerColor(4);
   rate_vs_delim->GetListOfFunctions()->Add(rate_delim, "P");
 
   // Target Position vs Time
   sprintf(htitle,"%.3f : Taret Postion vs. Time", runinfo.RUNID);
 
   TH2F *tgtx_vs_time = new TH2F("tgtx_vs_time", htitle, 10, xmin, xmax, 10, 0.5, runinfo.RunTime*1.2);
 
   delete gAsymRoot.fHists->d["run"]->d["Run"]->o["tgtx_vs_time"];
   gAsymRoot.fHists->d["run"]->d["Run"]->o["tgtx_vs_time"] = tgtx_vs_time;
 
   tgtx_vs_time->GetYaxis()->SetTitle("Duration from Measurement Start [s]");
   tgtx_vs_time->GetXaxis()->SetTitle("Target Position");
 
   //TGraph *tgtx_time = new TGraph(nTgtIndex, tgt.X, tgt.Time);
   TGraph *tgtx_time = new TGraph(X_index, tgt.X, tgt.Time);
   tgtx_time->SetMarkerStyle(20);
   tgtx_time->SetMarkerColor(4);
   tgtx_vs_time->GetListOfFunctions()->Add(tgtx_time, "P");
}


// square root formula
// A-RightUp  B-LeftDown  C-RightDown  D-LeftUp
// elastic Carbons are scattered off more in Right for Up
void sqass(float A, float B, float C, float D, float *asym, float *easym)
{
   float den;
   den = sqrt(A*B) + sqrt(C*D);
   if ( (A*B == 0.) && (C*D == 0.) ) {
      *asym  = 0.;
      *easym = 0.;
   } else {
      *asym  = (sqrt(A*B) - sqrt(C*D))/den;
      *easym = sqrt(A*B*(C+D) + C*D*(A+B))/den/den;
   }
}


// Method name : CumulativeAsymmetry(){
// Description : Caluclate bunch cumulative asymmetries
void CumulativeAsymmetry()
{
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
   float RU[120],RD[120],LU[120],LD[120];
   int   gbid[120];    // if 1:good and used 0: be discarded
   float gtmin,gtmax,btmin,btmax;
   float fspinpat[120];
   long  Nsi[6]={0,0,0,0,0,0};

   // Right-Left asymmetry
   for (int bid=0; bid<120; bid++){

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

   for (int bid=0; bid<120; bid++) {
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
   for (int bid=0; bid<NBUNCH; bid++){
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
   for (int bid=0; bid<120; bid++) {
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
   for (int bid=0; bid<120; bid++) {
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
   for (int bid=0; bid<120; bid++) {
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
   for (int bid=0; bid<120; bid++) {
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
      for (int bid=0; bid<120; bid++) {
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
      for (int bid=0; bid<120; bid++) {
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
   }

   // **** for different t range ****
   float txasym90[6][120], txasym90E[6][120];
   float txasym45[6][120], txasym45E[6][120];
   float tyasym45[6][120], tyasym45E[6][120];
   float tcasym45[6][120], tcasym45E[6][120];

   for (int tr=0;tr<NTBIN;tr++){
     float SUM=0;
       // X90 (2-5)
       for (int bid=0; bid<NBUNCH; bid++){
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
       for (int bid=0; bid<120; bid++) {
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
       for (int bid=0; bid<120; bid++) {
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
       for (int bid=0; bid<120; bid++) {
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
   HHPAK(36000, (float*)cntr.reg.NStrip[0]);
   HHPAK(36100, (float*)cntr.reg.NStrip[1]);
}


//
// Class name  :
// Method name : binary_zero(int n)
//
// Description : print integer in binary with zero filled from the most significant bit
//             : to zero bit, f.i. 0101 for n=5, mb=4
// Input       : int n, int mb(the most significant bit)
// Return      : writes out n in binary
//
void binary_zero(int n, int mb)
{
   int X = int(pow(double(2), double(mb-1)));
 
   for (int i=0; i<mb; i++) {
     int j = n << i & X ? 1 : 0;
     cout << j ;
   }
}


//
// Class name  :
// Method name : CalcStatistics(){
//
// Description : Calculate Statistics
// Input       :
// Return      :
//
void CalcStatistics()
{
   // Integrate good carbon events in banana
   cntr.good_event = 0;
 
   int X_index = runinfo.Run >= 6 ? nTgtIndex : ndelim;
 
   for (int i=0; i<X_index; i++) cntr.good_event += cntr.good[i];
 
   // Run time duration
   runinfo.RunTime  = runinfo.Run == 5 ? ndelim : cntr.revolution/RHIC_REVOLUTION_FREQ;
 
   // Calculate rates
   if (runinfo.RunTime) {
      runinfo.GoodEventRate = float(cntr.good_event) / runinfo.RunTime / 1e6;
      runinfo.EvntRate      = float(Nevtot) / runinfo.RunTime / 1e6;
      runinfo.ReadRate      = float(Nread) / runinfo.RunTime / 1e6;
   }
 
   // Misc
   if (runinfo.WcmSum)        gAnaResults.wcm_norm_event_rate = runinfo.GoodEventRate/runinfo.WcmSum*100;
   if (dproc.reference_rate)  gAnaResults.UniversalRate       = gAnaResults.wcm_norm_event_rate/dproc.reference_rate;
   if (runinfo.Run==5)        gAnaResults.profile_error       = gAnaResults.UniversalRate < 1 ? ProfileError(gAnaResults.UniversalRate) : 0;
}


//
// Class name  :
// Method name : PrintWarning()
//
// Description : print warnings
// Input       :
// Return      :
//
void PrintWarning()
{
   extern StructStripCheck strpchk;
   extern StructBunchCheck bnchchk;

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
   printf(" Number of Problemeatic Bunches      : %6d \n",  gAnaResults.anomaly.nbunch);
   printf(" Problemeatic Bunches Rate [%%]      : %6.1f\n", gAnaResults.anomaly.bad_bunch_rate);
   printf(" Bunch error code                    :   "); binary_zero(gAnaResults.anomaly.bunch_err_code,4); printf("\n");
   printf(" Problemeatic Bunch ID's             : ");
   for (int i=0; i<gAnaResults.anomaly.nbunch; i++) printf("%d ", gAnaResults.anomaly.bunch[i]) ;
   printf("\n");
   printf(" Unrecognized Problematic Bunches    : ");
   for (int i=0; i<gAnaResults.unrecog.anomaly.nbunch; i++) printf("%d ", gAnaResults.unrecog.anomaly.bunch[i]) ;
   printf("\n");
   printf("===> Invariant Mass / strip \n");
   printf(" Maximum Mass Deviation [GeV]        : %6.2f   (%d)\n", strpchk.dev.max,  strpchk.dev.st+1);
   printf(" Maximum Mass fit chi-2              : %6.2f   (%d)\n", strpchk.chi2.max, strpchk.chi2.st+1);
   printf(" Maximum #Events Deviation from Ave  : %6.2f   (%d)\n", strpchk.evnt.max, strpchk.evnt.st+1);
   printf(" Maximum Mass-Energy Correlation     : %8.4f (%d)\n", strpchk.p1.max, strpchk.p1.st+1);
   printf(" Weighted Mean InvMass Sigma         : %6.2f \n", strpchk.width.average[0]);
   printf(" Good strip Mass-Energy Correlation  : %8.4f \n", strpchk.p1.allowance);
   printf(" Good strip Mass Sigma Allowance[GeV]: %6.2f \n", strpchk.width.allowance);
   printf(" Good strip Mass Pos. Allowance[GeV] : %6.2f \n", strpchk.dev.allowance);
   printf(" Good strip Mass Fit chi2 Allowance  : %6.2f \n", strpchk.chi2.allowance);
   printf(" Good strip #Events Allowance        : %6.2f \n", strpchk.evnt.allowance);
   printf(" Strip error code                    :   "); binary_zero(gAnaResults.anomaly.strip_err_code,4);printf("\n");
   printf(" Number of Problematic Strips        : %6d \n", gAnaResults.anomaly.nstrip);
   printf(" Problematic Strips                  : ");
   for (int i=0; i<gAnaResults.anomaly.nstrip; i++) printf("%d ", gAnaResults.anomaly.st[i]+1);
   printf("\n");
   printf(" Number of Unrecog. Problematic Strip: %6d \n", gAnaResults.unrecog.anomaly.nstrip);
   printf(" Unrecognized Problematic Strips     : ");
   for (int i=0; i<gAnaResults.unrecog.anomaly.nstrip; i++) printf("%d ", gAnaResults.unrecog.anomaly.st[i]+1) ;
   printf("\n");
   printf("===> Detector \n");
   printf(" Slope of Energy Spectrum (sum 6 det): %6.1f %6.2f\n", gAnaResults.energy_slope[0], gAnaResults.energy_slope[1]);
   printf("-----------------------------------------------------------------------------------------\n");
   printf("\n\n");
}


// Method name : PrintRunResults()
//
// Description : print analysis results and run infomation
// Input       : StructHistStat hstat
void PrintRunResults(StructHistStat hstat)
{
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
   printf(" RunTime                 [s]    = %10.1f\n", runinfo.RunTime);
   printf(" Total events in banana         = %10ld\n",  cntr.good_event);
   printf(" Good Carbon Max Rate  [MHz]    = %10.4f\n", gAnaResults.max_rate);
   printf(" Good Carbon Rate      [MHz]    = %10.4f\n", runinfo.GoodEventRate);
   printf(" Good Carbon Rate/WCM_sum       = %10.5f\n", gAnaResults.wcm_norm_event_rate);
   printf(" Universal Rate                 = %10.5f\n", gAnaResults.UniversalRate);
   printf(" Event Rate            [MHz]    = %10.4f\n", runinfo.EvntRate);
   printf(" Read Rate             [MHz]    = %10.4f\n", runinfo.ReadRate);
   printf(" Target                         = %c%c\n",   runinfo.target, runinfo.targetID);
   printf(" Target Operation               = %s\n",     runinfo.TgtOperation);
   if (runinfo.Run >=6 ) {
      printf(" Maximum Revolution #           = %10d\n", runinfo.MaxRevolution);
      printf(" Reconstructed Duration  [s]    = %10.1f\n",runinfo.MaxRevolution/RHIC_REVOLUTION_FREQ);
      printf(" Target Motion Counter          = %10ld\n",cntr.tgtMotion);
   }
   printf(" WCM Sum     [10^11 protons]    = %10.1f\n", runinfo.WcmSum/100);
   printf(" WCM Average [10^9  protons]    = %10.1f\n", runinfo.WcmAve);
   printf(" WCM Average w/in range         = %10.1f\n", average.average);
   printf(" Specific Luminosity            = %10.2f%10.2f%10.4f\n",hstat.mean, hstat.RMS, hstat.RMSnorm);
   printf(" # of Filled Bunch              = %10d\n", runinfo.NFilledBunch);
   printf(" # of Active Bunch              = %10d\n", runinfo.NActiveBunch);
   printf(" bunch w/in WCM range           = %10d\n", average.counter);
   printf(" process rate                   = %10.1f [%%]\n", (float)average.counter/(float)NFilledBunch*100);
   printf(" Analyzing Power Average        = %10.4f \n",     gAnaResults.A_N[1]);
   if (dproc.FEEDBACKMODE)
   printf(" feedback average tshift        = %10.1f [ns]\n", gAnaResults.TshiftAve);
   printf(" Average Polarization (phi=0)   = %10.4f%9.4f\n", gAnaResults.P[0],                  gAnaResults.P[1]);
   printf(" Polarization (sinphi)          = %10.4f%9.4f\n", gAnaResults.sinphi[0].P[0],        gAnaResults.sinphi[0].P[1]);
   printf(" Phase (sinphi)  [deg.]         = %10.4f%9.4f\n", gAnaResults.sinphi[0].dPhi[0]*R2D, gAnaResults.sinphi[0].dPhi[1]*R2D);
   printf(" chi2/d.o.f (sinphi fit)        = %10.4f\n",      gAnaResults.sinphi[0].chi2);
   printf(" Polarization (bunch ave)       = %10.4f%9.4f\n", basym.ave.Ax[0]/gAnaResults.A_N[1], basym.ave.Ax[1]/gAnaResults.A_N[1]);
   printf(" Phase (bunch ave)              = %10.4f%9.4f\n", basym.ave.phase[0]*R2D, basym.ave.phase[1]*R2D);
   if (runinfo.Run==5)
   printf(" profile error (absolute)[%%]   = %10.4f\n",      gAnaResults.profile_error * fabs(gAnaResults.P[0]));
   printf("--- Alternative %3.1f sigma result & ratio to %3.1f sigma ---\n", dproc.MassSigmaAlt, dproc.MassSigma);
   printf(" Polarization (sinphi) alt      = %10.4f%9.4f\n", gAnaResults.sinphi[1].P[0],        gAnaResults.sinphi[1].P[1]);
   printf(" Ratio (alt/reg)                = %10.2f%9.2f\n", gAnaResults.P_sigma_ratio[0],      gAnaResults.P_sigma_ratio[1]);
   printf(" Ratio ((alt-reg)/reg)          = %10.3f%9.3f\n", gAnaResults.P_sigma_ratio_norm[0], gAnaResults.P_sigma_ratio_norm[1]);
   printf(" Polarization (PHENIX)          = %10.4f%9.4f\n", gAnaResults.sinphi[2].P[0],        gAnaResults.sinphi[2].P[1]);
   printf(" Polarization (STAR)            = %10.4f%9.4f\n", gAnaResults.sinphi[3].P[0],        gAnaResults.sinphi[3].P[1]);
   printf("-----------------------------------------------------------------------------------------\n");

   for(Int_t i=0; i<nTgtIndex+1; i++)
   printf(" Polarization tgt pos %6.1f     = %10.4f %9.4f\n", tgt.all.x[(int) tgt.Time[i]], gAnaResults.sinphi[100+i].P[0], gAnaResults.sinphi[100+i].P[1]);

   printf("-----------------------------------------------------------------------------------------\n");
}


/** */
void DrawPlotvsTar()
{
   Double_t polvstar[nTgtIndex+1],epolvstar[nTgtIndex+1],posvstar[nTgtIndex+1];

   for(Int_t i=0;i<nTgtIndex+1;i++){
      polvstar[i]  = gAnaResults.sinphi[100+i].P[0];
      epolvstar[i] = gAnaResults.sinphi[100+i].P[1];
      posvstar[i]  = tgt.all.x[(int)tgt.Time[i]];
   }
 
   Asymmetry->cd();
   TH2F *h_vstar = new TH2F("Pol_vs_tarpos","Polarization vs target position",100,posvstar[0]-1,posvstar[nTgtIndex]+1,100,0,1);

   TGraphErrors *gpolvstar = new TGraphErrors(nTgtIndex+1,posvstar,polvstar,0,epolvstar);
   h_vstar->GetXaxis()->SetTitle("Target position");
   h_vstar->GetYaxis()->SetTitle("Polarization");
   gpolvstar->SetMarkerStyle(20);
   gpolvstar->SetMarkerColor(kGreen+100);

   DrawLine(h_vstar, posvstar[0]-1, posvstar[nTgtIndex]+1, gAnaResults.sinphi[0].P[0], kRed, 1,2);
   DrawLine(h_vstar, posvstar[0]-1, posvstar[nTgtIndex]+1, gAnaResults.sinphi[0].P[0] - gAnaResults.sinphi[0].P[1], kRed, 1,1);
   DrawLine(h_vstar, posvstar[0]-1, posvstar[nTgtIndex]+1, gAnaResults.sinphi[0].P[0] + gAnaResults.sinphi[0].P[1], kRed, 1,1);

   h_vstar->GetListOfFunctions()->Add(gpolvstar, "P");
}


// Description : Caluclate Energy Yeild weighted Analyzing power
//             : Histogram 34000 is filled only once, which is controled by CallFlag
// Input       : int HID
// Return      : A_N
float WeightAnalyzingPower(int HID)
{
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

   if (runinfo.BeamEnergy > 200) {
      // XXX scale flattop values 250 GeV by 15% 1.176 = 1./ (1-0.15)
      //for (int i=0; i<25; i++) anth[i] = anth100[i] * 1.176; v1.2.0 and earlier

      // A new correction introduced in v1.3.1 scales pC polarization down by
      // approx 18% (0.823 +/- 0.012) at 250 GeV 
      for (int i=0; i<25; i++) anth[i] = anth100[i] * 1.215;
   } else if (runinfo.BeamEnergy > 50) {
      for (int i=0; i<25; i++) anth[i] = anth100[i];
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
   TH1F* hEnergy = (TH1F*) gAsymRoot.fHists->d["std"]->o["hKinEnergyA_o_cut2"];
   //TH1F* hEnergy = (TH1F*) gAsymRoot.fHists->o["hKinEnergyA_oo"];

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
}


//
// Class name  :
// Method name : ExclusionList(int i, int j)
//
// Description : returns true if strip #j is in exclusion candidate
// Input       : int i, int j
// Return      : true/faulse
//
int ExclusionList(int k, int j, int polBeam)
{
  int test = 0;
  int i = (k > 35) ? k-36 : k;

  if (j == i || j == 35-i || j == 36+i || j == 71-i ) test = 1;

  return test;
}


// Description : calculate Asymmetry
// Input       : int a, int b, int atot, int btot
// Return      : float Asym, float dAsym
void calcAsymmetry(int a, int b, int atot, int btot, float &Asym, float &dAsym)
{
   float R = 0;
   float A = float(a);
   float B = float(b);
   float Atot = float(atot);
   float Btot = float(btot);
 
   if (Btot) R = Atot/Btot;

   if ( A+R*B && A+B ) {
      Asym  = (A-R*B)/(A+R*B);
      dAsym = sqrt(4*B*B*A + 4*A*A*B)/(A+B)/(A+B);
   } else {
      Asym = dAsym = 0;
   }
}


//
// Class name  :
// Method name : WeightedMean(float A[N], float dA[N], int NDAT)
//
// Description : calculate weighted mean
// Input       : float A[N], float dA[N], int NDAT
// Return      : weighted mean
//
float WeightedMean(float A[], float dA[], int NDAT)
{
   float sum1, sum2, dA2, WM;
   sum1 = sum2 = dA2 = 0;
 
   for (int i=0; i<NDAT; i++) {
      if (dA[i]) {  // skip dA=0 data
         dA2 = dA[i]*dA[i];
         sum1 += A[i]/dA2 ;
         sum2 += 1/dA2 ;
      }
   }
 
   WM = dA2 == 0 ? 0 : sum1/sum2 ;
 
   return WM ;
}


//
// Class name  :
// Method name : WeightedMeanError(float dA[N], int NDAT)
//
// Description : calculate weighted mean error. A[i] is skipped if dA[i]=0.
// Input       : float dA[N], int NDAT
// Return      : weighted mean error
//
float WeightedMeanError(float A[], float dA[], float Ave, int NDAT)
{
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
}


//
// Class name  :
// Method name : calcWeightedMean(float A[N], float dA[N], int NDAT, float &Ave, float &dAve)
//
// Description : call weighted mean and error
// Input       : float A[N], float dA[N], float Ave, int NDAT
// Return      : Ave, dAve
//
void calcWeightedMean(float A[], float dA[], int NDAT, float &Ave, float &dAve)
{
   Ave  = WeightedMean(A, dA, NDAT);
   dAve = WeightedMeanError(A, dA, Ave, NDAT);
}


// Description : calculates profile error for given universal rate x
//             : For run5, zero for blue beam
float ProfileError(float x)
{
  return runinfo.fPolBeam == 1 ? exp(-0.152*log(1/x)) : 0 ;
}


// Description : Handle Specific Luminosity
// Input       : Histograms 10033, 11033, 10034, 11034
// Return      : float &mean, float &RMS, float &RMS_norm
void SpecificLuminosity(float &mean, float &RMS, float &RMS_norm)
{
   //-------------------------------------------------------
   // Specific Luminosity
   //-------------------------------------------------------
   int bid;
   float SpeLumi_norm[NBUNCH], dSpeLumi_norm[NBUNCH];
 
   // initialization
   SpeLumi.max = SpeLumi.Cnts[0];
   SpeLumi.min = 9999999;
 
   for (bid=0; bid<NBUNCH; bid++) {
      SpeLumi.Cnts[bid] = wcmdist[bid] != 0 ? Ngood[bid]/wcmdist[bid] : 0 ;
      SpeLumi.dCnts[bid] = sqrt(double(Ngood[bid]));
      specific_luminosity->Fill(bid,SpeLumi.Cnts[bid]);
      if (fillpat[bid]) {
         if (SpeLumi.max<SpeLumi.Cnts[bid])SpeLumi.max=SpeLumi.Cnts[bid];
         if (SpeLumi.min>SpeLumi.Cnts[bid])SpeLumi.min=SpeLumi.Cnts[bid];
      }
   }

   HHPAK(10033, SpeLumi.Cnts);    HHPAKE(11033, SpeLumi.dCnts);
   SpeLumi.ave = WeightedMean(SpeLumi.Cnts,SpeLumi.dCnts,NBUNCH);
 
   if (SpeLumi.ave){
     for (bid=0; bid<NBUNCH; bid++) {
       SpeLumi_norm[bid] = SpeLumi.Cnts[bid]/SpeLumi.ave;
       dSpeLumi_norm[bid] = SpeLumi.dCnts[bid]/SpeLumi.ave;
     }
   }

   HHPAK(10034, SpeLumi_norm);    HHPAKE(11034, dSpeLumi_norm);
 
   // Book and fill histograms
   char hcomment[256];
   sprintf(hcomment,"Specific Luminosity");
   HHBOOK1(10035,hcomment,100,SpeLumi.ave-SpeLumi.ave/2,SpeLumi.ave+SpeLumi.ave/2.);
   for (bid=0;bid<120;bid++) HHF1(10035,SpeLumi.Cnts[bid],1);
 
   // Get variables
   char CHOICE[5]="HIST";
   mean = HHSTATI(10035, 1, CHOICE, 0) ;
   RMS  = HHSTATI(10035, 2, CHOICE, 0) ;
   RMS_norm = (!mean) ? 0 : RMS/mean ;
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
float TshiftFinder(int Mode, int FeedBackLevel)
{
   int   np = 3;
   float par[np], sigpar[np];
   float chi2;
   float mdev,adev;
   float ex[NSTRIP];
   char  htitle[50];
   float min,max;
   float margin = 0.2;
 
   TGraphErrors *tg;
   TF1 *f1 = new TF1("f1", "gaus");
 
   for (int st=0; st<NSTRIP; st++) {
 
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
         par[2] = dproc.OneSigma * k2G; // sigma [GeV]
 
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
 
     TGraphErrors *gre = new TGraphErrors(NSTRIP, feedback.strip, feedback.mdev, ex, feedback.err);
     gre->SetMarkerSize(MSIZE);
     gre->SetMarkerStyle(20);
     gre->SetMarkerColor(4);
 
     GetMinMaxOption(errdet.MASS_POSITION_ALLOWANCE*1.2, NSTRIP, feedback.mdev, margin, min, max);
 
     mdev_feedback = new TH2F("mdev_feedback", "Mass Deviation in FeedBack Mode", NSTRIP, -0.5, NSTRIP+0.5, 50, min, max);
 
     mdev_feedback->GetListOfFunctions()->Add(gre,"P");
     mdev_feedback->GetXaxis()->SetTitle("Strip ID");
     mdev_feedback->GetYaxis()->SetTitle("Mass Deviation [GeV]");
 
   //} else {
 
     ErrDet->cd();
 
     // Mass Position Deviation from M_12
     GetMinMaxOption(errdet.MASS_POSITION_ALLOWANCE*1.2, NSTRIP, feedback.mdev, margin, min, max);
     sprintf(htitle,"Run%.3f:Invariant mass position deviation vs. strip", runinfo.RUNID);
     mass_pos_dev_vs_strip =  new TH2F("mass_pos_dev_vs_strip",htitle,NSTRIP+1,0,NSTRIP+1,50, min, max);
     tg =  AsymmetryGraph(1, NSTRIP, feedback.strip, feedback.mdev, ex, ex);
     tg->SetName("tg");
     mass_pos_dev_vs_strip -> GetListOfFunctions()-> Add(tg,"p");
     mass_pos_dev_vs_strip -> GetYaxis() -> SetTitle("Peak - M_12C [GeV]");
     mass_pos_dev_vs_strip -> GetXaxis() -> SetTitle("Strip Number");
 
     // RMS width mapping of 12C mass peak
     GetMinMax(NSTRIP, feedback.RMS, margin, min, max);
     sprintf(htitle,"Run%.3f:Gaussian fit on Invariant mass sigma vs. strip", runinfo.RUNID);
     mass_sigma_vs_strip =  new TH2F("mass_sigma_vs_strip",htitle,NSTRIP+1,0,NSTRIP+1,50, min, max);
     tg =  AsymmetryGraph(1, NSTRIP, feedback.strip, feedback.RMS, ex, feedback.err);
     tg->SetName("tg");
     mass_sigma_vs_strip -> GetListOfFunctions()-> Add(tg,"p");
     mass_sigma_vs_strip -> GetYaxis() -> SetTitle("RMS Width of 12C Mass Peak[GeV]");
     mass_sigma_vs_strip -> GetXaxis() -> SetTitle("Strip Number");
 
     // Chi2 mapping of Gaussian fit on 12C mass peak
     sprintf(htitle,"Run%.3f:Gaussian fit on Invariant mass chi2 vs. strip",runinfo.RUNID);
     GetMinMax(NSTRIP, feedback.chi2, margin, min, max);
     mass_chi2_vs_strip =  new TH2F("mass_chi2_vs_strip", htitle, NSTRIP+1, 0, NSTRIP+1, 50, min, max);
     tg  =  AsymmetryGraph(1, NSTRIP, feedback.strip, feedback.chi2, ex, ex);
     tg->SetName("tg");
     mass_chi2_vs_strip -> GetListOfFunctions()-> Add(tg, "p");
     mass_chi2_vs_strip -> GetYaxis() -> SetTitle("Chi2 of Gaussian Fit on 12C Mass Peak");
     mass_chi2_vs_strip -> GetXaxis() -> SetTitle("Strip Number");
 
     // Call strip anomaly detector routine
     StripAnomalyDetector();
   //}
 
   mdev = WeightedMean(feedback.mdev, feedback.err, NSTRIP);
   printf("Average Mass Deviation  = %10.2f [GeV/c]\n", mdev);
 
   adev = mdev * G2k * gRunConsts[0].M2T/sqrt(400.);
 
   printf("Tshift Average @ 400keV = %10.2f [ns]\n", adev);
 
   // Unit Conversion [GeV] -> [keV]
   for (int i=0; i<NSTRIP; i++) {
     feedback.tedev[i] = feedback.mdev[i]*G2k* gRunConsts[i+1].M2T;
     feedback.RMS[i] *= G2k;
   }
 
   return adev;
}


//
// Class name  :
// Method name : FillAsymmetryHistgram(char Mode[], int sign, int N, float y[])
//
// Description : Fill out bunch by bunch Asymmetry Histograms
//             : asym_bunch_x90, asym_bunch_x45, asym_bunch_y45
//             : These histograms are then applied Gaussian fit to check anomaly bunches
// Input       : char Mode[], int sign, int N, float A[], float dA[], float bunch[]
// Return      :
//
void FillAsymmetryHistgram(char Mode[], int sign, int N, float A[], float dA[], float bunch[])
{
  float a[N];

  for (int i=0; i<N; i++) { // loop for bunch number

    // flip the asymmetry sign for spin=-1 to be consistent with spin=+1
    a[i] = A[i]*sign;

    if ((fillpat[i])&&(dA[i])){

      // process only active bunches
      if (bunch[i] != -1){
        if (Mode=="x90") asym_bunch_x90->Fill(a[i],1/dA[i]); //weighted by error
        if (Mode=="x45") asym_bunch_x45->Fill(a[i],1/dA[i]);
        if (Mode=="y45") asym_bunch_y45->Fill(a[i],1/dA[i]);
      }
    }
  }

  return;
}


// Description : Define net TGraphErrors object asymgraph for vectors x,y,ex,ey
//             : specifies marker color based on mode
//             : positive spin : blue
//             : negative spin : red
// Input       : int Mode, int N, float x[], float y[], float ex[], float ey[]
// Return      : TGraphErrors * asymgraph
//
TGraphErrors* AsymmetryGraph(int Mode, int N, float x[], float y[], float ex[], float ey[])
{
  int Color = Mode == 1 ? 4 : 2;

  TGraphErrors *asymgraph = new TGraphErrors(N, x, y, ex, ey);
  asymgraph->SetMarkerStyle(20);
  asymgraph->SetMarkerSize(MSIZE);
  asymgraph->SetMarkerColor(Color);

  return asymgraph;
}


// Method name : calcDivisionError
//
// Description : Calculates error propagation of x/y for (x,dx) and (y,dy)
//             :
// Input       : float x, float y, float dx, float dy
// Return      : error propagation of x/y
float calcDivisionError(float x, float y, float dx, float dy)
{
   if (x*y) {
      return x/y * sqrt ( dx*dx/x/x + dy*dy/y/y );
   } else {
      return 0;
   }
}


// Description : call BunchAsymmetry to calculate asymmetries bunch by bunch and
//             : Fill out asym_vs_bunch_x90, x45, y45 histograms
void CalcBunchAsymmetry()
{
   // Calculate bunch asymmetries [0] and corresponding errors [1] for x45,
   // x90, y45
   BunchAsymmetry(0, basym.Ax90[0], basym.Ax90[1]);
   BunchAsymmetry(1, basym.Ax45[0], basym.Ax45[1]);
   BunchAsymmetry(2, basym.Ay45[0], basym.Ay45[1]);

   // Define TH2F histograms first
   Asymmetry->cd();

   char  htitle[100];
   float min, max;
   float margin=0.2;
   float prefix=0.028;

   sprintf(htitle, "Run%.3f: Raw Asymmetry X90", runinfo.RUNID);
   GetMinMaxOption(prefix, NBUNCH, basym.Ax90[0], margin, min, max);

   //asym_vs_bunch_x90 = new TH2F("asym_vs_bunch_x90", htitle, NBUNCH, -0.5, NBUNCH-0.5, 100, min, max);
   asym_vs_bunch_x90->SetName("asym_vs_bunch_x90");
   asym_vs_bunch_x90->SetTitle(htitle);
   asym_vs_bunch_x90->SetBins(NBUNCH, -0.5, NBUNCH-0.5, 100, min, max);

   DrawLine(asym_vs_bunch_x90, -0.5, NBUNCH-0.5, 0, 1, 1, 1);

   sprintf(htitle,"Run%.3f : Raw Asymmetry X45", runinfo.RUNID);
   GetMinMaxOption(prefix, NBUNCH, basym.Ax45[0], margin, min, max);

   //asym_vs_bunch_x45 = new TH2F("asym_vs_bunch_x45", htitle, NBUNCH, -0.5, NBUNCH-0.5, 100, min, max);
   asym_vs_bunch_x45->SetName("asym_vs_bunch_x45");
   asym_vs_bunch_x45->SetTitle(htitle);
   asym_vs_bunch_x45->SetBins(NBUNCH, -0.5, NBUNCH-0.5, 100, min, max);
   asym_vs_bunch_x45->GetYaxis()->SetTitle("Counts weighted by error");
   asym_vs_bunch_x45->GetXaxis()->SetTitle("Raw Asymmetry");

   DrawLine(asym_vs_bunch_x45, -0.5, NBUNCH-0.5, 0, 1, 1, 1);

   sprintf(htitle,"Run%.3f : Raw Asymmetry Y45", runinfo.RUNID);
   GetMinMaxOption(prefix, NBUNCH, basym.Ay45[0], margin, min, max);

   //asym_vs_bunch_y45 = new TH2F("asym_vs_bunch_y45", htitle, NBUNCH, -0.5, NBUNCH-0.5, 100, min, max);
   asym_vs_bunch_y45->SetName("asym_vs_bunch_y45");
   asym_vs_bunch_y45->SetTitle(htitle);
   asym_vs_bunch_y45->SetBins(NBUNCH, -0.5, NBUNCH-0.5, 100, min, max);
   asym_vs_bunch_y45 -> GetYaxis() -> SetTitle("Counts weighted by error");
   asym_vs_bunch_y45 -> GetXaxis() -> SetTitle("Raw Asymmetry");

   DrawLine(asym_vs_bunch_y45, -0.5, NBUNCH-0.5, 0, 1, 1, 1);

   // fill bunch ID array [1 - NBUNCH], not [0 - NBUNCH-1]
   float bunch[NBUNCH], ex[NBUNCH];

   for (int bid=0; bid<NBUNCH; bid++) { ex[bid] = 0; bunch[bid] = bid; }

   // Superpose Positive/Negative Bunches arrays into TH2F histograms defined above
   TGraphErrors * asymgraph ;

   for (int spin=1; spin>=-1; spin-=2 ) {

      // Selectively disable bunch ID by matching spin pattern
      for (int bid=0; bid<NBUNCH; bid++) { bunch[bid]= spinpat[bid] == spin ? bid : -1 ;}

      // X90
      asymgraph = AsymmetryGraph(spin, NBUNCH, bunch, basym.Ax90[0], ex, basym.Ax90[1]);
      asymgraph -> SetName("asymgraph");
      FillAsymmetryHistgram("x90", spin, NBUNCH, basym.Ax90[0], basym.Ax90[1], bunch);
      asym_vs_bunch_x90->GetListOfFunctions() -> Add(asymgraph,"p");
      asym_vs_bunch_x90->GetXaxis()->SetTitle("Bunch Number");
      asym_vs_bunch_x90->GetYaxis()->SetTitle("Raw Asymmetry ");

      // X45
      asymgraph = AsymmetryGraph(spin, NBUNCH, bunch, basym.Ax45[0], ex, basym.Ax45[1]);
      FillAsymmetryHistgram("x45", spin, NBUNCH, basym.Ax45[0], basym.Ax45[1], bunch);
      asymgraph -> SetName("asymgraph");
      asym_vs_bunch_x45->GetListOfFunctions() -> Add(asymgraph,"p");
      asym_vs_bunch_x45->GetXaxis()->SetTitle("Bunch Number");
      asym_vs_bunch_x45->GetYaxis()->SetTitle("Raw Asymmetry ");

      // Y45
      asymgraph = AsymmetryGraph(spin, NBUNCH, bunch, basym.Ay45[0], ex, basym.Ay45[1]);
      FillAsymmetryHistgram("y45", spin, NBUNCH, basym.Ay45[0], basym.Ay45[1], bunch);
      asymgraph->SetName("asymgraph");
      asym_vs_bunch_y45->GetListOfFunctions()->Add(asymgraph, "p");
      asym_vs_bunch_y45->GetXaxis()->SetTitle("Bunch Number");
      asym_vs_bunch_y45->GetYaxis()->SetTitle("Raw Asymmetry ");
   }

   // bunch asymmetry averages
   calcBunchAsymmetryAverage();
}


// Description : calculate asymmetries bunch by bunch
// Input       : int Mode0[Ax90], 1[Ax45], 2[Ay45]
// Return      : Asym[NBUNCH], dA[NBUNCH]
void BunchAsymmetry(int Mode, float A[], float dA[])
{
   // Allocate adequate detector IDs involved in X90,X45,Y45, respectively
   int Rdet[2], Ldet[2];

   switch (Mode) {
   case 0:  // Ax90
      Rdet[0] = 1; Ldet[0] = 4;
      break;
   case 1:  // Ax45
      Rdet[0] = 0; Rdet[1] = 2; Ldet[0] = 3; Ldet[1] = 5;
      break;
   case 2:  // Ay45
      Rdet[0] = 0; Rdet[1] = 5; Ldet[0] = 2; Ldet[1] = 3;
      break;
   default:
      cerr << "BunchAsymmetry: No muching mode is coded in for " << Mode << endl;
      return;
   }
 
   // Calculate detector luminosities by taking sum over R/L detectors and all
   // bunches
   long int LumiR = 0;
   long int LumiL = 0;

   for (int i=0; i<2; i++) { // run for 2 for X45 and Y45
      for (int bid=0; bid<NBUNCH; bid++) {
         if (fillpat[bid]){
            LumiR += Ncounts[Rdet[i]][bid] ;
            LumiL += Ncounts[Ldet[i]][bid] ;
         }
      }
      if (!Mode) break; // no R/L detector loop for x90
   }
 
   // Main bunch loop to calculate asymmetry bunch by bunch
   for (int bid=0; bid<NBUNCH; bid++) {
 
      int R = 0, L = 0;
       A[bid] = ASYM_DEFAULT;
      dA[bid] = 0;

      if (fillpat[bid]){
         // Take sum of Up/Down for Right and Left detectors
         for (int i=0; i<2; i++) {
            R += Ncounts[Rdet[i]][bid];
            L += Ncounts[Ldet[i]][bid];
            if (!Mode) break; // no detector loop for X90
         }
 
         calcAsymmetry(R, L, LumiR, LumiL, A[bid], dA[bid]);
      }
   }
}


// Description : calculate average left-right and top-bottom asymmetry from bunch by bunch asymmetris.
//             : also calculate phase from LR and Top-Bottom asymmetry
void calcBunchAsymmetryAverage()
{
   // flip the sign of negative spin bunches
   for (int i=0; i<NBUNCH; i++) {
      if (spinpat[i]==-1) {
         basym.Ax90[0][i] *= -1;
         basym.Ax45[0][i] *= -1;
         basym.Ay45[0][i] *= -1;
      }
   }
 
   // Calculate weighgted beam for Ax90, Ax45, Ay45 combinations
   calcWeightedMean(basym.Ax90[0], basym.Ax90[1], NBUNCH, basym.ave.Ax90[0], basym.ave.Ax90[1]);
   calcWeightedMean(basym.Ax45[0], basym.Ax45[1], NBUNCH, basym.ave.Ax45[0], basym.ave.Ax45[1]);
   calcWeightedMean(basym.Ay45[0], basym.Ay45[1], NBUNCH, basym.ave.Ay45[0], basym.ave.Ay45[1]);
 
   // Calculate Left-Right asymmetry using Ax90 and Ax45
   calcLRAsymmetry(basym.ave.Ax90, basym.ave.Ax45, basym.ave.Ax[0], basym.ave.Ax[1]);
 
   // Calculate Top-Bottom asymmetry using Ay45
   basym.ave.Ay[0] = basym.ave.Ay45[0] * M_SQRT2;
   basym.ave.Ay[1] = basym.ave.Ay45[1] * M_SQRT2;
 
   // Calculate phase (spin pointing vector w.r.t. vertical axis) from Ax and Ay
   if (basym.ave.Ay[0]) {
      float r_xy = basym.ave.Ax[0]/basym.ave.Ay[0];
      basym.ave.phase[0] = atan(r_xy) - M_PI_2;
   } else {
      basym.ave.phase[0] = 0;
   }
 
   basym.ave.phase[1] = calcDivisionError(basym.ave.Ax[0], basym.ave.Ay[0], basym.ave.Ax[1], basym.ave.Ay[1]);
   basym.ave.phase[1] = atan(basym.ave.phase[1]) ? fabs( atan(basym.ave.phase[1]) - M_PI_2 ) : 0 ;
 
   // Calculate Polarization
   if (gAnaResults.A_N[0]) {
      gAnaResults.basym[0].P[0] = basym.ave.Ax[0]/gAnaResults.A_N[0];
      gAnaResults.basym[0].P[1] = basym.ave.Ax[1]/gAnaResults.A_N[0];
   } else {
      gAnaResults.basym[0].P[0] = 0;
      gAnaResults.basym[0].P[1] = 0;
   };
 
   cout << "========== bunch asymmetry average ===========" << endl;
   cout << basym.ave.Ax90[0] << " " << basym.ave.Ax90[1] << " "
        << basym.ave.Ax45[0] << " " << basym.ave.Ax45[1] << " "
        << basym.ave.Ay45[0] << " " << basym.ave.Ay45[1] << " " << endl;
   cout << basym.ave.Ax[0]   << " " << basym.ave.Ax[1] << endl;
   cout << "========== bunch asymmetry average ===========" << endl;
}


// Description : Calculate Left-Right asymmetry from X90 and X45 asymmetry combination
//             :
// Input       : float X90[2], float X45[2]
// Return      : LR asymmetry and error : float &A, float &dA
void calcLRAsymmetry(float X90[2], float X45_tmp[2], float &A, float &dA)
{
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
}


//
// Class name  :
// Method name : StripAsymmetry
//
// Description : call calcStripAsymmetry() subroutines for
//             : regular and alternative sigma banana cuts, respectively.
//             : Also call for PHENIX and STAR colliding bunches asymmetries
// Input       :
// Return      :
//
void StripAsymmetry()
{
   //// Calculate Asymmetries for colliding bunches at PHENIX
   //CalcStripAsymmetry(gAnaResults.A_N[1], 2, cntr.phx.NStrip);
 
   //// Calculate Asymmetries for colliding bunches at STAR
   //CalcStripAsymmetry(gAnaResults.A_N[1], 3, cntr.str.NStrip);
 
   //// alternative sigma cut
   //CalcStripAsymmetry(gAnaResults.A_N[1], 1, cntr.alt.NStrip);
 
   // regular sigma cut
   CalcStripAsymmetry(gAnaResults.A_N[1], 0, cntr.reg.NStrip);

   // Add info to database entry
   char sPol[50];

   sprintf(sPol, "% 8.3f, % 6.3f, % 8.3f, % 6.3f",
      gAnaResults.sinphi[0].P[0],    gAnaResults.sinphi[0].P[1],
      gAnaResults.sinphi[0].dPhi[0], gAnaResults.sinphi[0].dPhi[1]);

   gRunDb.fFields["POLARIZATION"] = sPol;
 
   // Some consistency checks for different sigma cuts
   if (gAnaResults.sinphi[0].P[0]) {
 
      float diff[2];

      // calculate differences and ratio
      diff[0] = gAnaResults.sinphi[1].P[0] - gAnaResults.sinphi[0].P[0];
 
      gAnaResults.P_sigma_ratio[0]      = gAnaResults.sinphi[1].P[0] / gAnaResults.sinphi[0].P[0];
      gAnaResults.P_sigma_ratio_norm[0] = diff[0] / gAnaResults.sinphi[0].P[0];
 
      // calculate errors for above, respectively
      diff[1] = QuadErrorSum(gAnaResults.sinphi[1].P[1], gAnaResults.sinphi[0].P[1]);
 
      gAnaResults.P_sigma_ratio[1] =
         QuadErrorDiv(gAnaResults.sinphi[1].P[0], gAnaResults.sinphi[0].P[0], gAnaResults.sinphi[1].P[1], gAnaResults.sinphi[0].P[1]);
 
      gAnaResults.P_sigma_ratio_norm[1] =
         QuadErrorDiv(diff[0], gAnaResults.sinphi[0].P[0], diff[1], gAnaResults.sinphi[0].P[1]);
   }
 
   // Calculate Asymmetries for each target position

   TH1* hpp = 0;
   if (dproc.HasProfileBit()) {
      hpp = (TH1*) gAsymRoot.fHists->d["profile"]->o["hPolProfile"];
   }

   //ds:
   //for(Int_t i=0; i<nTgtIndex+1; i++)
   for(Int_t i=0; i<ndelim; i++) {

      CalcStripAsymmetry(gAnaResults.A_N[1], 100+i, cntr_tgt.reg.NStrip[i]);
      printf("i, p: %d, %f\n", i, gAnaResults.sinphi[100+i].P[0]);

      if (hpp) {
         hpp->SetBinContent(i+1, gAnaResults.sinphi[100+i].P[0]);
         hpp->SetBinError(i+1, gAnaResults.sinphi[100+i].P[1]);
      }
   }
   //for(Int_t i=20; i<21; i++) CalcStripAsymmetry(gAnaResults.A_N[1], 100+i, cntr_tgt.reg.NStrip[i]);
   gAsymRoot.ProcessProfileHists();
}


// Description : calculate asymmetries strip by strip
// Input       : float aveA_N, int Mode, long int nstrip[][NSTRIP]
// Return      :
void CalcStripAsymmetry(float aveA_N, int Mode, long int nstrip[][NSTRIP])
{
   //ds : Overwrite nstrip array

   int ss_code = 0;

   // only for Mode < 100
   if (Mode < 100) {
      TH2I* hSpVsCh = (TH2I*) gAsymRoot.fHists->d["std"]->o["hSpinVsChannel_cut2"];

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
      for (int i=1; i<=NSTRIP; i++) {

         sprintf(dName, "channel%02d", i);

         DrawObjContainer* oc = gAsymRoot.fHists->d["std"]->d.find(dName)->second;     

         sprintf(hName, "hSpinVsDelim_cut2_st%02d", i);

         TH1* hSpVsDelim = (TH1*) oc->o[hName];

         for (int iy=1; iy<=hSpVsDelim->GetNbinsY(); iy++) { // spin

            double bcont  = hSpVsDelim->GetBinContent(iDelim+1, iy);

            ss_code = iy == 3 ? 0 : (iy == 1 ? 1 : 2); // spin down

            nstrip[ss_code][i-1] = bcont;
         }
      }
   }


   // Strip-by-Strip Asymmetries
   int   LumiSum[2][NSTRIP];          // Total Luminosity [0]:Spin Up, [1]:Spin Down
   float LumiSum_r[2][NSTRIP];        // Reduced order Total luminosity for histograming
   float LumiRatio[NSTRIP];           // Luminosity Ratio
   float Asym[NSTRIP], dAsym[NSTRIP]; // Raw Asymmetries strip-by-strip
   float P[NSTRIP],    dP[NSTRIP];    // phi corrected polarization
   float Pt[NSTRIP],   dPt[NSTRIP];   // phi Trancated corrected polarization,
   long int counts[2];                // local counter variables

   for (int i=0; i<NSTRIP; i++) {

      Asym[i] = dAsym[i] = RawP[i] = dRawP[i] = LumiSum_r[0][i] = LumiSum_r[0][i] = LumiRatio[i] = 0;
      LumiSum[0][i] = LumiSum[1][i] = 0;

      // Loop for Total Luminosity
      for (int j=0; j<NSTRIP; j++) {

         // Calculate Luminosity. Own Strip and ones in cross geometry are excluded.
         if (!ExclusionList(i, j, runinfo.fPolBeam)) {
            for (int k=0; k<=1; k++) LumiSum[k][i] += nstrip[k][j];
         }
      }

      counts[0] = nstrip[0][i]; // spin up
      counts[1] = nstrip[1][i]; // spin down

      //printf("XXX: %3d, %8.5f, %10d, %10d, %10d, %10d, %8.5f, %8.5f\n",
      //       i, aveA_N, counts[0], counts[1], LumiSum[0][i], LumiSum[1][i],
      //       Asym[i], dAsym[i]);

      // Luminosity Ratio
      LumiRatio[i] = (float) LumiSum[0][i]/(float)LumiSum[1][i];

      // Calculate Raw Asymmetries for strip-i
      if ( (LumiSum[1][i]) && (counts[0]+counts[1]) ) {
         calcAsymmetry(counts[0], counts[1], LumiSum[0][i], LumiSum[1][i], Asym[i], dAsym[i]);

         //printf("ZZZ: %3d, %8.5f, %10d, %10d, %10d, %10d, %8.5f, %8.5f\n",
         //       i, aveA_N, counts[0], counts[1], LumiSum[0][i], LumiSum[1][i],
         //       Asym[i], dAsym[i]);
      }

      // Reduced Order Luminosity for histograms. Histogram scale is given in float, not double.
      // Cannot accomomdate large entry.
      LumiSum_r[0][i] = LumiSum[0][i]/1e3;
      LumiSum_r[1][i] = LumiSum[1][i]/1e3;

      // Since this is the recoil asymmetries, flip the sign of asymmetry
      Asym[i] *= -1;

      // Raw polarization without phi angle weighted A_N
       RawP[i] = (aveA_N != 0 ? ( Asym[i] / aveA_N) : 0);
      dRawP[i] = (aveA_N != 0 ? (dAsym[i] / aveA_N) : 0);

      // Polarization with sin(phi) correction
       P[i] = RawP[i] / sin(-phi[i]);
      dP[i] = fabs(dRawP[i] / sin(-phi[i]));

      // Polarization with trancated sin(phi) correction
       Pt[i] = RawP[i] / sin(-phit[i]);
      dPt[i] = fabs(dRawP[i] / sin(-phit[i]));

      // ds temp fix: give huge errors to disabled strips
      if (gRunInfo.fDisabledChannels[i]) {
         Asym[i]  =  RawP[i] =  P[i] =  Pt[i] = 0;
         dAsym[i] = dRawP[i] = dP[i] = dPt[i] = 1e6;
      }

      //printf("ZZZ: %3d, %8.5f, %10ld, %10ld, %10d, %10d, %8.5f, %8.5f\n",
      //       i, aveA_N, counts[0], counts[1], LumiSum[0][i], LumiSum[1][i],
      //       Asym[i], dAsym[i]);
   }

   // printing routine
   //if (Flag.VERBOSE)
   if (0) {

      printf("*========== strip by strip =============\n");

      for (int i=0; i<NSTRIP; i++) {
         printf("%4d", i);
         printf("%12.5f", phi[i]);
         printf("%12.5f %12.5f", Asym[i], dAsym[i]);
         printf("%12.5f %12.5f", RawP[i], dRawP[i]);
         printf("%12.5f %12.5f",    P[i],    dP[i]);
         printf("%12.5f %12.5f",   Pt[i],   dPt[i]);
         printf("\n");
      }

      printf("*=======================================\n");
      printf("\n");
   }

   // Caluclate Weighted Average
   calcWeightedMean(P, dP, 72, gAnaResults.P[0], gAnaResults.P[1]);

   //printf("P0, P1: %8.5f %8.5f\n", gAnaResults.P[0], gAnaResults.P[1]);

   //ds:// Histrograming
   //ds:HHPAK(36010, LumiSum_r[0]);  HHPAK(36110, LumiSum_r[1]);
   //ds:HHPAK(36200, LumiRatio);
   //ds:HHPAK(36210, Asym);  HHPAKE(36210, dAsym);
   //ds:HHPAK(36220, RawP);  HHPAKE(36220, dRawP);
   //ds:HHPAK(36240, P);     HHPAKE(36240, dP);
   //ds:HHPAK(36250, phi);

   // Fit phi-distribution
   // Fit everything other than scan data
   if (Mode < 100) {
      AsymFit asymfit;
      asymfit.SinPhiFit(gAnaResults.P[0], RawP, dRawP, phi, gAnaResults.sinphi[Mode].P,
                        gAnaResults.sinphi[Mode].dPhi, gAnaResults.sinphi[Mode].chi2);
   }

   // Fit scan data
   if (Mode >= 100) {
      AsymFit asymfit;
      asymfit.ScanSinPhiFit(gAnaResults.P[0], RawP, dRawP, phi, gAnaResults.sinphi[Mode].P,
                            gAnaResults.sinphi[Mode].dPhi, gAnaResults.sinphi[Mode].chi2);
   }

   //asymfit.SinPhiFit(gAnaResults.P[0], gAnaResults.sinphi.P, gAnaResults.sinphi.dPhi, gAnaResults.sinphi.chi2);
}


// Description : sin(x) fit. Amplitude and phase as parameters
// Input       : Double_t *x, Double_t *par
// Return      : par[0]*sin(x+par[1])
Double_t sin_phi(Double_t *x, Double_t *par)
{
   return par[0] * sin(-x[0] + par[1]);
}


// Description : Master Routine for sin(phi) root-fit
// Input       : Float_t p0 (1-par Polarization for par[0] initialization)
//             : Float_t *RawP, Float_t *dRawP, Float_t *phi (vectors to be fit)
// Return      : Float_t *P, Float_t *dphi, Float_t &chi2dof
void AsymFit::SinPhiFit(Float_t p0, Float_t *RawP, Float_t *dRawP, Float_t *phi,
                        Float_t *P, Float_t *dphi, Float_t &chi2dof)
{
   char htitle[100];
   float dx[NSTRIP];
   for ( int i=0; i<NSTRIP; i++) dx[i] = 0;
 
   // define TH2D sin(phi) histogram
   Asymmetry->cd();

   float min, max, prefix, margin; prefix = margin = 0.3;

   GetMinMaxOption(prefix, NSTRIP, RawP, margin, min, max);

   sprintf(htitle,"Run%.3f: Strip Asymmetry sin(phi) fit", runinfo.RUNID);

   asym_sinphi_fit->SetName("asym_sinphi_fit");
   asym_sinphi_fit->SetTitle(htitle);
   asym_sinphi_fit->SetBins(100, 0, 2*M_PI, 100, min, max);
   asym_sinphi_fit->GetXaxis()->SetTitle("phi [deg.]");
   asym_sinphi_fit->GetYaxis()->SetTitle("Asymmetry / A_N [%]");

   DrawLine(asym_sinphi_fit, 0, 2*M_PI, 0, 1, 1, 1);
 
   // define sin(phi) fit function & initialize parmaeters
   TF1 *func = new TF1("sin_phi", sin_phi, 0, 2*M_PI, 2);
   func->SetParameters(p0, 0);
   func->SetParLimits(0, -1, 1);
   func->SetParLimits(1, -M_PI, M_PI);
   func->SetParNames("P", "dPhi");
   func->SetLineColor(2);
 
   // define TGraphError obect for fitting
   TGraphErrors *tg = AsymmetryGraph(1, NSTRIP, phi, RawP, dx, dRawP);
 
   // Perform sin(phi) fit
   tg->Fit("sin_phi", "R");
   tg->SetName("tg");
 
   // Dump TGraphError obect to TH2D histogram
   asym_sinphi_fit->GetListOfFunctions()->Add(tg, "p");
 
   // Get fitting results
   P[0]    = func->GetParameter(0);
   P[1]    = func->GetParError(0);
   dphi[0] = func->GetParameter(1);
   dphi[1] = func->GetParError(1);
   chi2dof = func->GetChisquare()/func->GetNDF();
 
   // Write out fitting results on plot
   char text[80];
   sprintf(text, "(%3.2f +/- %3.2f)* sin(phi%+5.2f)", P[0], P[1], dphi[0]);
   TText *txt = new TText(0.5, max*0.8, text);
   asym_sinphi_fit->GetListOfFunctions()->Add(txt);

   sprintf(text, "chi/dof = %5.2f", chi2dof);
   txt = new TText(0.5, max*0.1, text);

   asym_sinphi_fit->GetListOfFunctions()->Add(txt);
}


// April, 2008; Introduced by Vipuli to keep dphi fixed at gAnaResults.sinphi[0].dPhi[0]
// when doing the fit to data taken at each scaned position
//
// Description : Master Routine for sin(phi) root-fit
// Input       : Float_t p0 (1-par Polarization for par[0] initialization)
//             : Float_t *RawP, Float_t *dRawP, Float_t *phi (vectors to be fit)
// Return      : Float_t *P, Float_t *dphi, Float_t &chi2dof
//
void AsymFit::ScanSinPhiFit(Float_t p0, Float_t *RawP, Float_t *dRawP, Float_t *phi,
                   Float_t *P, Float_t *dphi, Float_t &chi2dof)
{ //{{{
   float dx[NSTRIP];

   for (int i=0; i<NSTRIP; i++) dx[i] = 0;
 
   // define TH2D sin(phi) histogram
   Asymmetry->cd();

   float min, max, prefix=0.3, margin=0.3;

   GetMinMaxOption(prefix, NSTRIP, RawP, margin, min, max);

   char htitle[100];
   sprintf(htitle, "Run%.3f: Strip Asymmetry sin(phi) fit", runinfo.RUNID);
 
   //scan_asym_sinphi_fit = new TH2F("scan_asym_sinphi_fit",htitle, 100, 0, 2*M_PI, 100, min, max);
   scan_asym_sinphi_fit->SetName("scan_asym_sinphi_fit");
   scan_asym_sinphi_fit->SetTitle(htitle);
   scan_asym_sinphi_fit->SetBins(100, 0, 2*M_PI, 100, min, max);
   scan_asym_sinphi_fit->GetXaxis()->SetTitle("phi [deg.]");
   scan_asym_sinphi_fit->GetYaxis()->SetTitle("Asymmetry / A_N [%]");
 
   DrawLine(scan_asym_sinphi_fit, 0, 2*M_PI, 0, 1, 1, 1);
 
   // define sin(phi) fit function & initialize parmaeters
   TF1 *func = new TF1("sin_phi", sin_phi, 0, 2*M_PI, 2);
   func->SetParameters(p0, 0);
   func->SetParLimits(0, -1, 1);

   // Keeping phi fixed - chnged by Vipuli April 2008
   //printf("************************************ \n");
   //printf("keeping phi fixed at = %12.3e \n", gAnaResults.sinphi[0].dPhi[0]);
   //printf("************************************ \n");

   func->SetParLimits(1, gAnaResults.sinphi[0].dPhi[0], gAnaResults.sinphi[0].dPhi[0]);
   //func->SetParLimits(1,-M_PI, M_PI);
   func->SetParNames("P", "dPhi");
   func->SetLineColor(2);
 
   // define TGraphError obect for fitting
   TGraphErrors *tg = AsymmetryGraph(1, NSTRIP, phi, RawP, dx, dRawP);
 
   // Perform sin(phi) fit
   tg->Fit("sin_phi","R Q");
   tg->SetName("tg");
 
   // Dump TGraphError obect to TH2D histogram
   scan_asym_sinphi_fit->GetListOfFunctions()->Add(tg, "p");
 
   // Get fitting results
   P[0]    = func->GetParameter(0);
   P[1]    = func->GetParError(0);
   dphi[0] = func->GetParameter(1);
   dphi[1] = func->GetParError(1);
   chi2dof = func->GetChisquare()/func->GetNDF();
 
   // Write out fitting results on plot
   char text[80];
 
   sprintf(text, "(%3.2f +/- %3.2f)* sin(phi%+5.2f)", P[0], P[1], dphi[0]);
   TText *txt = new TText(0.5, max*0.8, text);
 
   scan_asym_sinphi_fit->GetListOfFunctions()->Add(txt);
 
   sprintf(text, "chi^{2} = %5.2f", chi2dof);
   txt = new TText(0.5, max*0.1, text);
 
   scan_asym_sinphi_fit->GetListOfFunctions()->Add(txt);
} //}}}


// Return Maximum from array A[N]. Ignores ASYM_DEFAULT as an exception
float GetMax(int N, float A[]){
  float max = A[0] != ASYM_DEFAULT ? A[0] : A[1];
  for (int i=1; i<N; i++) max = (A[i])&&(max<A[i])&&(A[i]!=ASYM_DEFAULT) ? A[i] : max;
  return max;
}


// Return Miminum from array A[N]. Ignores ASYM_DEFAULT as an exception
float GetMin(int N, float A[])
{
  float min = A[0] != ASYM_DEFAULT ? A[0] : A[1];
  for (int i=1; i<N; i++) min = (A[i]) && (min>A[i]) && (A[i] != ASYM_DEFAULT) ? A[i] : min;
  return min;
}


/**
 * Return Minimum and Maximum from array A[N]
 *
 * If margin is not 0 the maximum is increased by max*margin and minimum is
 * decreased by min*margin
 */
void GetMinMax(int N, float A[], float margin, float &min, float &max)
{
   min  = GetMin(N, A);
   max  = GetMax(N, A);
   min -= fabs(min) * margin;
   max += fabs(max) * margin;
}


// Return Minimum and Maximum from array A[N]. Same as GetMinMax() function. But
// GetMinMaxOption takes prefix value which forces min, max to be prefix when the
// absolute min,max are smaller than prefix.
void GetMinMaxOption(float prefix, int N, float A[], float margin, float &min, float &max)
{
   GetMinMax(N, A, margin, min, max);
   if ( fabs(min) < prefix ) min = -prefix;
   if ( fabs(max) < prefix ) max =  prefix;
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
void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
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
Float_t AsymFit::sinx(Float_t x, Double_t *par)
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
void AsymFit::SinPhiFit(Float_t p0, Float_t *P, Float_t *phi, Float_t &chi2dof)
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
Double_t AsymFit::GetFittingErrors(TMinuit *gMinuit, Int_t NUM){

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
void RAMP::CalcRAMP()
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
