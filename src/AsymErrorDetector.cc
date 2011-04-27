//  Asymmetry Analysis of RHIC pC Polarimeter
//  Error/Anomaly Finding Routine
//  file name :   AsymErrorDetector.cc
//
//  Authors   :   Itaru Nakagawa
//                Dmitri Smirnov
//
//  Creation  :   08/01/2006
//

#include "AsymErrorDetector.h"

#include "TText.h"

#include "AnaInfo.h"
#include "AnaResult.h"
#include "RunInfo.h"

// Description : Fit the slope of energy spectrum given in (-t) with exponential.
void DetectorAnomaly()
{
   Kinema->cd();
 
   TF1 * expf = new TF1("expf","expo");
   expf -> SetParameters(1,-60);
   expf -> SetLineColor(2);
 
   // limit fitting range between energy cuts. Be careful to change this
   // not to include small entry bins at the both edge of the histogram
   // into the fit.
   //ds: extern StructHist Eslope;
   float dbin  = (Eslope.xmax - Eslope.xmin)/float(Eslope.nxbin);
   float min_t = 2*gAnaInfo.enel*MASS_12C*k2G*k2G + dbin;
   float max_t = 2*gAnaInfo.eneu*MASS_12C*k2G*k2G - dbin;
   energy_spectrum_all -> Fit("expf"," "," ",min_t,max_t);
   gAnaResults.energy_slope[0] = expf -> GetParameter(1);
   gAnaResults.energy_slope[1] = expf -> GetParError(1);
 
   // print fitting results on histogram
   char text[36];
   sprintf(text,"slope=%6.1f +/- %6.2f", gAnaResults.energy_slope[0], gAnaResults.energy_slope[1]);
   TText * t = new TText(0.01, energy_spectrum_all->GetMaximum()/4, text);
   energy_spectrum_all -> GetListOfFunctions()->Add(t);
}


// Description : Check the Mass vs. 12C Kinetic Enegy Correlation. Apply linear fit on
//             : Mass .vs Energy scatter plot and record resulting slops for all strips
void InvariantMassCorrelation(int st)
{
   Kinema->cd();
 
   char htitle[100],histname[100];
 
   // Function for Fitting
   TF1 * g1 = new TF1("g1","gaus",5,16);
   TF1 * f1 = new TF1("f1","pol1",200,1000);
   f1->SetLineColor(2);
 
   // Mass vs. Energy correlation
   sprintf(histname,"mass_vs_e_ecut_st%d",st+1);
   mass_vs_e_ecut[st]->Write();
   TH2F * hslice = (TH2F*) gDirectory->Get(histname);
   hslice->SetName("hslice");
 
   // slice histogram for fit
   hslice->FitSlicesY(g1);
 
   TH1D *hslice_1 = (TH1D*)gDirectory->Get("hslice_1");
   sprintf(histname,"mass_vs_energy_corr_st%d",st+1);
   hslice_1 -> SetName(histname);
   sprintf(htitle,"%.3f:Invariant Mass vs. Energy Correlation Fit (Str%d)",gRunInfo.RUNID,st+1);
   hslice_1 -> SetTitle(htitle);
   hslice_1 -> GetXaxis()->SetTitle("12C Kinetic Energy [keV]");
   hslice_1 -> GetYaxis()->SetTitle("Invariant Mass [GeV]");
   hslice_1 -> Fit("f1","Q");
   TLine * l = new TLine(200, 1000, MASS_12C*k2G, MASS_12C*k2G);
   hslice_1 -> GetListOfFunctions() -> Add(l);
 
   //Get Fitting results
   for (int j=0;j<2; j++) {
     strpchk.ecorr.p[j][st]    = f1->GetParameter(j);
     strpchk.ecorr.perr[j][st] = strpchk.ecorr.p[j][st] != 0 ? f1->GetParError(j) : 0;
   }
 
   // Delete unnecessary histograms generated by FitSliceY();
   TH1D *hslice_0 = (TH1D*)gDirectory->Get("hslice_0");
   TH1D *hslice_2 = (TH1D*)gDirectory->Get("hslice_2");
   TH1D *hslice_chi2 = (TH1D*)gDirectory->Get("hslice_chi2");
   hslice_0->Delete(); hslice_2->Delete(); hslice_chi2->Delete();
 
   // Make graph of p1 paramter as a function of strip number when strip number is the last one
   if (st == NSTRIP-1) {
      ErrDet->cd();
 
      float strip[NSTRIP],ex[NSTRIP];
      for (int k=0;k<NSTRIP;k++) {strip[k]=k+1;ex[k]=0;}
 
      float min,max;
      float margin=0.2;
      GetMinMaxOption(strpchk.p1.allowance*1.5, NBUNCH, strpchk.ecorr.p[1], margin, min, max);
      sprintf(htitle,"Run%.3f : P[1] distribution for Mass vs. Energy Correlation", gRunInfo.RUNID);
      mass_e_correlation_strip = new TH2F("mass_e_correlation_strip", htitle, NSTRIP+1, 0, NSTRIP+1, 50, min, max);
      TGraphErrors * mass_e_gr = AsymmetryGraph(1, NSTRIP, strip, strpchk.ecorr.p[1], ex, strpchk.ecorr.perr[1]);
      mass_e_gr -> SetName("mass_e_gr");
      mass_e_correlation_strip -> GetListOfFunctions() -> Add(mass_e_gr,"p");
      mass_e_correlation_strip -> GetXaxis()->SetTitle("Strip Number");
      mass_e_correlation_strip -> GetYaxis()->SetTitle("slope [GeV/keV]");
      DrawHorizLine(mass_e_correlation_strip, 0, NSTRIP+1,  strpchk.p1.allowance, 2, 2, 2);
      DrawHorizLine(mass_e_correlation_strip, 0, NSTRIP+1, -strpchk.p1.allowance, 2, 2, 2);
   }
}


// Description : fit banana with kinematic function. This routine is incomplete.
//             : 1. fix hard corded runconst.E2T consntant 1459.43.
//             : 2. delete hbananan_1 histograms
void BananaFit(int st)
{
   char f[50];
   sprintf(f,"%8.3f/sqrt(x)", gRunConsts[st+1].E2T);
   TF1 * functof = new TF1("functof", f,200,1500);
   functof->SetLineColor(2);
   functof->SetLineWidth(4);
   functof->SetName("kin_func");
   functof->Write();
 
   char hname[100];
   sprintf(hname,"t_vs_e_st%d",st);
   t_vs_e[st]->GetListOfFunctions()->Add(functof);
   t_vs_e[st]->Write();
   TH2F * hbanana = (TH2F*) gDirectory->Get(hname);
   hbanana->SetName("hbanana");
 
   // Get centers of banana
   hbanana->FitSlicesY();
 
   TH1D *hbanana_1 = (TH1D*) gDirectory->Get("hbanana_1");
   sprintf(hname,"banana_center_st%d",st);
   hbanana_1->SetName(hname);
 
   // Delete unnecessary histograms
   TH1D *hbanana_0    = (TH1D*) gDirectory->Get("hbanana_0");
   TH1D *hbanana_2    = (TH1D*) gDirectory->Get("hbanana_2");
   TH1D *hbanana_chi2 = (TH1D*) gDirectory->Get("hbanana_chi2");
   hbanana_0->Delete();  hbanana_2->Delete();  hbanana_chi2->Delete();
}


// Description : find suspicious strips through folllowing three checks
//             : (chi-2 check of Gaussian Fit on Inviariant Mass is not included now)
//             : strip_err_code (1111)
//             : Bit[0]: RMS width of Carbin Invariant Mass Deviation from Average
//             : Bit[1]: Maximum deviation of invariant mass peak from 12Mass
//             : Bit[2]: Number of events in Banana Cuts Deviation from Average
//             : Bit[3]: Invariant Mass vs. Energy Correlation
void StripAnomalyDetector()
{
   // Errror allowance
   strpchk.p1.allowance    = errdet.MASS_ENERGY_CORR_ALLOWANCE;
   strpchk.dev.allowance   = errdet.MASS_POSITION_ALLOWANCE;
   strpchk.width.allowance = errdet.MASS_WIDTH_DEV_ALLOWANCE;
   strpchk.chi2.allowance  = errdet.MASS_CHI2_ALLOWANCE;
   strpchk.evnt.allowance  = errdet.GOOD_CARBON_EVENTS_ALLOWANCE;
 
   // Get weighted average for width of 12C invariant mass distributions
   strpchk.width.average[0] = WeightedMean(feedback.RMS,feedback.err,NSTRIP);
   DrawHorizLine(mass_sigma_vs_strip, 0, NSTRIP+1, strpchk.width.average[0], 1, 1, 2);
 
   // Calculate average carbon events in banana cuts
   strpchk.evnt.average[0] = good_carbon_events_strip->GetEntries()/float(gRunInfo.NActiveStrip);
   DrawHorizLine(good_carbon_events_strip, 0.5, NSTRIP+0.5, strpchk.evnt.average[0], 1, 1, 2);
   printf(" Average # of Events in Banana Cut : %6.2f\n", strpchk.evnt.average[0]);
 
   // initiarization before following worst (max) search
   strpchk.dev.max  = 0;
   strpchk.chi2.max = 0;
   strpchk.evnt.max = 0;
 
   // registration of the worst strips
   float evntdev[NSTRIP];
   float sigma=0;
   int counter=0;
 
   for (int i=0; i<NSTRIP; i++) {
      evntdev[i] = 0;
      printf("Anomary Check for strip=%d ...\r", i);
 
      if (gRunInfo.ActiveStrip[i]) {
         // t vs. Energy (this routine is incomplete)
         //BananaFit(i);
 
         // MASS vs. Energy correlation
         InvariantMassCorrelation(i);
 
         if (!i) strpchk.p1.max = fabs(double(strpchk.ecorr.p[1][i]));  // initialize max w/ strip 0
 
         if (fabs(double(strpchk.ecorr.p[1][i])) > fabs(double(strpchk.p1.max)) ) {
           strpchk.p1.max = strpchk.ecorr.p[1][i];
           strpchk.p1.st  = i;
         }
 
         // Maximum devistion of peak from 12C_MASS
         if ((fabs(feedback.mdev[i]) > strpchk.dev.max)&&(feedback.mdev[i]!=ASYM_DEFAULT)) {
           strpchk.dev.max  = feedback.mdev[i];
           strpchk.dev.st   = i;
         }
 
         // Gaussian Mass fit Largest chi2
         if (feedback.chi2[i] > strpchk.chi2.max) {
           strpchk.chi2.max  = fabs(double(feedback.chi2[i]));
           strpchk.chi2.st   = i;
         }
 
         // Good carbon events within banana
         evntdev[i]=fabs(double(good_carbon_events_strip->GetBinContent(i+1)-strpchk.evnt.average[0]))/strpchk.evnt.average[0] ;
 
         if (evntdev[i]>strpchk.evnt.max) {
           strpchk.evnt.max  = evntdev[i];
           strpchk.evnt.st   = i;
         }
 
         // Calculate one sigma of RMS distribution
         if (feedback.err[i]){
           sigma += (feedback.RMS[i]-strpchk.width.average[0])*(feedback.RMS[i]-strpchk.width.average[0])
             /feedback.err[i]/feedback.err[i];
           counter++;
         }
      }
   }
 
   sigma=sqrt(sigma)/counter;
 
   // Draw lines to objects
   float widthlimit=strpchk.width.allowance+strpchk.width.average[0];
   DrawHorizLine(mass_sigma_vs_strip, 0, NSTRIP+1, widthlimit, 2, 2, 2);
 
   float chi2limit={strpchk.chi2.allowance};
   DrawHorizLine(mass_chi2_vs_strip, 0, NSTRIP+1, chi2limit, 2, 2, 2);
 
   // draw average line for 12C mass width distribution
   DrawHorizLine(mass_pos_dev_vs_strip, 0, NSTRIP+1,    strpchk.dev.allowance, 2, 2, 2);
   DrawHorizLine(mass_pos_dev_vs_strip, 0, NSTRIP+1, -1*strpchk.dev.allowance, 2, 2, 2);
 
   // draw allowance for good_carbon_events_strip
   float evelim=(1+strpchk.evnt.allowance)*strpchk.evnt.average[0];
   DrawHorizLine(good_carbon_events_strip, 0.5, NSTRIP+0.5, evelim, 2, 2, 2);
   evelim=(1-strpchk.evnt.allowance)*strpchk.evnt.average[0];
   DrawHorizLine(good_carbon_events_strip, 0.5, NSTRIP+0.5, evelim, 2, 2, 2);
 
   // register and count suspicious strips
   gAnaResults.anomaly.nstrip = 0;
   gAnaResults.anomaly.strip_err_code = 0;
 
   char text[36];
 
   for (int i=0;i<NSTRIP; i++) {
 
      sprintf(text, "%2d", i+1);
      int strip_err_code = 0;
 
      if (gRunInfo.ActiveStrip[i]){ // process only if the strip is active
 
         // deviation from average width of 12C mass distribution
         if (fabs(double(feedback.RMS[i]))-strpchk.width.average[0]>strpchk.width.allowance) {
            strip_err_code += 1;
            printf(" WARNING: strip # %d Mass width %8.4f exeeds allowance limit %8.4f\n",
                   i+1, feedback.RMS[i], strpchk.width.allowance);
            DrawText(mass_sigma_vs_strip, float(i+1), feedback.RMS[i], 2, text);
         }
 
         // Invariant mass peak position deviation from 12C mass
         if (fabs(double(feedback.mdev[i])) > fabs(double(strpchk.dev.allowance))) {
            strip_err_code += 2;
            printf(" WARNING: strip # %d Mass position deviation %8.4f exeeds allowance limit %8.4f\n",
                   i+1, feedback.mdev[i], strpchk.dev.allowance);
            DrawText(mass_pos_dev_vs_strip, float(i+1), feedback.mdev[i], 2, text);
         }
 
         // number of carbon events per strip checker
         if (evntdev[i] > strpchk.evnt.allowance) {
            strip_err_code += 4;
            printf(" WARNING: strip # %d number of events in banana cut  %6.2f exeeds allowance limit %6.2f\n",
                   i+1, evntdev[i], strpchk.evnt.allowance);
            // comment in h2 histogram
            DrawText(good_carbon_events_strip, float(i+1), good_carbon_events_strip->GetBinContent(i+1), 2, text);
         }
 
         // mass vs. 12C kinetic energy correlation
         if (fabs(double(strpchk.ecorr.p[1][i])) > strpchk.p1.allowance) {
            strip_err_code += 8;
            printf(" WARNING: strip # %d Mass-Energy Correlation %8.4f exeeds allowance limit %8.4f\n",
                   i+1, strpchk.ecorr.p[1][i],strpchk.p1.allowance);
            DrawText(mass_e_correlation_strip, float(i+1), strpchk.ecorr.p[1][i], 2, text);
         }
 
         /* Currently chi2 is poor. Follwing routine is disabled until chi2 is inproved
         // chi2 of Gaussian fit on Inv. Mass peak
         if (feedback.chi2[i] > strpchk.chi2.allowance) {
         strip_err_code += 4;
         printf(" WARNING: strip # %d chi2 of Gaussian fit on mass %8.4f exeeds allowance limit %8.4f\n",
         i+1, feedback.chi2[i], strpchk.chi2.allowance);
         }*/
 
         if (strip_err_code) {
            gAnaResults.anomaly.st[gAnaResults.anomaly.nstrip]=i;
            ++gAnaResults.anomaly.nstrip;
         }
 
         // register global strip error code
         gAnaResults.anomaly.strip_err_code = gAnaResults.anomaly.strip_err_code | strip_err_code ;
      }
   }
 
   // register unrecognized anomaly strips
   UnrecognizedAnomaly(gAnaResults.anomaly.st, gAnaResults.anomaly.nstrip,
      gRunInfo.fDisabledChannels, gRunInfo.NDisableStrip,
      gAnaResults.unrecog.anomaly.st, gAnaResults.unrecog.anomaly.nstrip);
}


//
// Class name  :
// Method name : BunchAsymmetryGaussianFit()
//
// Description : find suspicious bunch thru Gaussian fit on bunch asymmetry histograms
//             : the bunches deviates more than sigma from fitted Gaussian width will be
//             : registered as problematic bunch ID
// Input       : TH1F * h1, TH2F * h2, float A[], float dA[], int err_code
// Return      : sigma of Gaussian fit
//
float BunchAsymmetryGaussianFit(TH1F * h1, TH2F * h2, float A[], float dA[], int err_code)
{
  // define Gaussian function
  TF1 * g = new TF1("g","gaus");
  g -> SetLineColor(2);

  // Perform Gaussian Fit
  h1->Fit("g","Q");
  //float hight = g -> GetParameter(0);
  float mean  = g -> GetParameter(1);
  float sigma = g -> GetParameter(2);

  // get sigma from Gaussian fit and calculate allowance limit
  //  bnchchk.asym[0].allowance = mean - errdet.BUNCH_ASYM_SIGMA_ALLOWANCE*sigma;
  //  bnchchk.asym[1].allowance = mean + errdet.BUNCH_ASYM_SIGMA_ALLOWANCE*sigma;

  // draw lines to mean asymmetery vs. bunch histograms
  DrawHorizLine(h2, -0.5, NBUNCH-0.5,  mean, 4, 4, 2);
  DrawHorizLine(h2, -0.5, NBUNCH-0.5, -mean, 2, 2, 2);

  // axis titles
  h1->GetYaxis()->SetTitle("Counts devided by statistical error");
  h1->GetXaxis()->SetTitle("Raw Asymmetry");

  // local anamaly bunch array and counter
  struct StructBUNCH {
    float A[NBUNCH];
    int bunch[NBUNCH];
    int nbunch;
    float dev;
    int active_bunch[2];
    float chi2[2]; // [0]:plus [1]:minus bunches
  } local;

  // Anomaly bunch finding
  char text[20];
  local.nbunch = local.active_bunch[0] = local.active_bunch[1] = 0;
  local.chi2[0] = local.chi2[1] = 0;
  for (int bid=0;bid<NBUNCH;bid++) {
    local.bunch[bid] = -999; // default not to appear in plots

    if (gFillPattern[bid] && (A[bid] != -ASYM_DEFAULT) ) {

      if (gSpinPattern[bid] == 1) {
        local.active_bunch[0]++;
        local.dev =  fabs(double(A[bid] - mean));
        local.chi2[0] += local.dev*local.dev/dA[bid]/dA[bid];
      }
      if (gSpinPattern[bid] == -1) {
        local.active_bunch[1]++;
        local.dev =  fabs(double(A[bid] - (-1)*mean));
        local.chi2[1] += local.dev*local.dev/dA[bid]/dA[bid];
      }

      if (local.dev/dA[bid] > errdet.BUNCH_ASYM_SIGMA_ALLOWANCE) {

         local.bunch[local.nbunch] = bid;
         local.A[local.nbunch] = A[bid];
         local.nbunch++;
         printf(" WARNING: bunch # %d asym sigma %6.1f exeeds %6.1f limit from average\n",
                bid, local.dev/dA[bid], errdet.BUNCH_ASYM_SIGMA_ALLOWANCE);

         // comment in h2 histogram
         sprintf(text,"%6.1f sigma (%d)", local.dev/dA[bid],bid);
         TText * t = new TText(bid+2, A[bid], text);
         h2 -> GetListOfFunctions()->Add(t);
      }
    }
  }

  // print chi2 in plot
  sprintf(text,"chi2(+)=%6.1f", local.chi2[0]/float(local.active_bunch[0]));
  TText * t1 = new TText(3, h2->GetYaxis()->GetXmax()*0.90, text);
  t1->SetTextColor(13);
  h2 -> GetListOfFunctions()->Add(t1);
  sprintf(text,"chi2(-)=%6.1f", local.chi2[1]/float(local.active_bunch[1]));
  TText * t2 = new TText(90, h2->GetYaxis()->GetXmin()*0.90, text);
  t2->SetTextColor(13);
  h2 -> GetListOfFunctions()->Add(t2);

  if (local.nbunch){
    // error_code registration
    gAnaResults.anomaly.bunch_err_code += err_code;

    // global registration
    RegisterAnomaly(local.bunch, local.nbunch, gAnaResults.anomaly.bunch, gAnaResults.anomaly.nbunch,
                    gAnaResults.anomaly.bunch, gAnaResults.anomaly.nbunch);

    // Superpose h2 histogram
    float bindex[local.nbunch];
    for (int i=0;i<local.nbunch;i++) bindex[i]=local.bunch[i];
    TGraph * gr = new TGraph(local.nbunch, bindex, local.A);
    gr -> SetMarkerStyle(24);
    gr -> SetMarkerSize(MSIZE);
    gr -> SetMarkerColor(3);

    // append suspicious bunch in h2 hitogram
    h2 -> GetListOfFunctions() -> Add(gr,"P");
  }

  return sigma;
}


// Description : find suspicious bunch thru Gaussian fit on bunch asymmetry histograms
//             : the bunches deviates more than sigma from fitted Gaussian width will be
//             : registered as problematic bunch ID.
//             : The last argument of the function BunchAsymmetryGaussianFit() is the
//             : error code to record which asymmetry gives warning.
void BunchAsymmetryAnomaly()
{
   // X90 Asymmetry Check
   printf("BunchAsymmetryAnomaly(): check for x90\n");
   bnchchk.asym[0].sigma =
      BunchAsymmetryGaussianFit(asym_bunch_x90, asym_vs_bunch_x90, gBunchAsym.Ax90[0], gBunchAsym.Ax90[1], 1);

   // X45 Asymmetry Check
   printf("BunchAsymmetryAnomaly(): check for x45\n");
   bnchchk.asym[1].sigma =
      BunchAsymmetryGaussianFit(asym_bunch_x45, asym_vs_bunch_x45, gBunchAsym.Ax45[0], gBunchAsym.Ax45[1], 2);

   // Y45 Asymmetry Check
   printf("BunchAsymmetryAnomaly(): check for y45\n");
   bnchchk.asym[2].sigma =
      BunchAsymmetryGaussianFit(asym_bunch_y45, asym_vs_bunch_y45, gBunchAsym.Ay45[0], gBunchAsym.Ay45[1], 4);
}


// Description : finds suspicious bunch thru following two checks of bunch by bunch
//             : Asymmetry anomaly check
//             : counting rate anomaly check
void BunchAnomalyDetector()
{
   // Initialize anomaly bunch counter and error_code
   gAnaResults.anomaly.nbunch= gRunInfo.NFilledBunch > errdet.NBUNCH_REQUIREMENT ? 0 : -1 ;
   gAnaResults.anomaly.bunch_err_code = 0;
   bnchchk.rate.max_dev = 0;

   if (gAnaResults.anomaly.nbunch != -1) {
      // Find anomaly bunches from unusual deviation from average asymmetry
      BunchAsymmetryAnomaly();

      // Find Hot bunches from counting rates per bunch
      HotBunchFinder(8);

      // check unrecognized anomaly
      UnrecognizedAnomaly(gAnaResults.anomaly.bunch, gAnaResults.anomaly.nbunch, gRunInfo.DisableBunch,gRunInfo.NDisableBunch,
                        gAnaResults.unrecog.anomaly.bunch, gAnaResults.unrecog.anomaly.nbunch);

      gAnaResults.anomaly.bad_bunch_rate = gRunInfo.NFilledBunch ? gAnaResults.anomaly.nbunch/float(gRunInfo.NFilledBunch)*100 : -1 ;
   }
}


// Description : find hot bunch from specific luminosity distribution
// Input       : err_code
void HotBunchFinder(int err_code)
{
   float err[NBUNCH], bindex[NBUNCH];
   float max, min;
   int init_flag=1;
   int EXCLUDE_BUNCH=20; // bunch 20 supposed be excluded from specific luminosity anomaly finding
 
   for (int bnch=0; bnch<NBUNCH; bnch++) {
 
      // inistiarization
      bindex[bnch] = bnch;
      err[bnch]    = 1;
 
      // calculate min and max range of the histogram
      if ((SpeLumi.Cnts[bnch])&&(init_flag) ) {min=SpeLumi.Cnts[bnch]; init_flag=0;}
      if ((SpeLumi.Cnts[bnch] < min)&&(!SpeLumi.Cnts[bnch])) min = SpeLumi.Cnts[bnch];
      if (max < SpeLumi.Cnts[bnch]) max = SpeLumi.Cnts[bnch];
   }
 
   // define rate distribution and fill the histogram
   ErrDet->cd();
   char hname[100];
   sprintf(hname,"%.3f : Specific Luminosiry / bunch", gRunInfo.RUNID);
   bunch_spelumi = new TH1F("bunch_spelumi",hname, 100, min*0.9, max*1.1);
   bunch_spelumi -> GetXaxis()->SetTitle("Good 12C Events/bunch / WCM");
   bunch_spelumi -> GetYaxis()->SetTitle("# Bunches weighted by 1/sqrt(12C Events)");
 
   for (int bnch=0;bnch<NBUNCH;bnch++) {
     if ((SpeLumi.Cnts[bnch])&&(bnch!=EXCLUDE_BUNCH))
       bunch_spelumi->Fill(SpeLumi.Cnts[bnch], 1/SpeLumi.dCnts[bnch]);
   }
 
   // define rate vs. bunch plot
   TGraph *gr = new TGraph(NBUNCH, bindex, SpeLumi.Cnts);
   gr->SetMarkerSize(MSIZE);
   gr->SetMarkerStyle(20);
   gr->SetMarkerColor(4);

   spelumi_vs_bunch = new TH2F("spelumi_vs_bunch", hname, NBUNCH, -0.5, NBUNCH+0.5, 50, min, max*1.3);
   spelumi_vs_bunch -> GetListOfFunctions() -> Add(gr,"P");
   spelumi_vs_bunch -> GetXaxis()->SetTitle("Bunch Number");
   spelumi_vs_bunch -> GetYaxis()->SetTitle("12C Yields/WCM");
 
   // define gaussian function
   // mean (parameter-1) is constrained to be between min and max of entries
   // sigma (parameter-1) is constrained to be between 0 and max-min of entries
   TF1 * g1 = new TF1("g1","gaus");
   g1->SetParameters(bunch_spelumi->GetMaximum(), SpeLumi.ave, (SpeLumi.max-SpeLumi.min)/3);
   g1->SetParLimits(1, SpeLumi.min, SpeLumi.max);
   g1->SetParLimits(2, 0, SpeLumi.max-SpeLumi.min);
   g1->SetLineColor(2);
 
   // apply gaussian fit on specific luminosity distribution
   bunch_spelumi->Fit(g1);
 
   // get mean from gaussian fit
   float ave = g1->GetParameter(1);
   DrawHorizLine(spelumi_vs_bunch, -0.5, NBUNCH+0.5, ave, 1, 1, 1);
 
   // get sigma from Gaussian fit and calculate allowance limit
   float sigma=g1->GetParameter(2);
   bnchchk.rate.allowance = ave + errdet.BUNCH_RATE_SIGMA_ALLOWANCE*sigma;
   bnchchk.rate.sigma_over_mean = ave ? sigma/ave : -1 ;
 
   // draw lines to 1D and 2D histograms
   DrawHorizLine(spelumi_vs_bunch, -0.5, NBUNCH+0.5, bnchchk.rate.allowance, 2, 2, 2);
   DrawVertLine(bunch_spelumi, bnchchk.rate.allowance, g1->GetParameter(0), 2, 2);
 
   // anomaly bunch registration
   int  flag = 0;
   char text[16];

   for (int bnch=0;bnch<NBUNCH;bnch++) {
      if (SpeLumi.Cnts[bnch] > bnchchk.rate.allowance) {
         gAnaResults.anomaly.bunch[gAnaResults.anomaly.nbunch] = bnch;
         gAnaResults.anomaly.nbunch++; flag++;
         float dev = (SpeLumi.Cnts[bnch] - ave)/sigma;
         bnchchk.rate.max_dev = bnchchk.rate.max_dev < dev ? dev : bnchchk.rate.max_dev ;
         printf("WARNING: bunch # %d yeild exeeds %6.1f sigma from average. HOT!\n", bnch, dev);
 
         // comment in h2 histogram
         sprintf(text,"Bunch %d",bnch);
         TText * t = new TText(bnch+2, SpeLumi.Cnts[bnch], text);
         spelumi_vs_bunch -> GetListOfFunctions()->Add(t);
      }
   }
 
   // assign error_code
   if (flag)
      gAnaResults.anomaly.bunch_err_code += err_code;
}


// Description : converts float array x[] into integer and call RegisterAnomaly(int,...)
//             :
// Input       : float x[], int nx, int y[], int ny,
// Return      : result of (x[]&y[]) -> array z[], and nz
//
void RegisterAnomaly(float x[], int nx, int y[], int ny, int z[], int &nz)
{
   int X[nx];
   for (int i=0;i<nx;i++) X[i]=int(x[i]);
   RegisterAnomaly(X, nx, y, ny, z, nz);
}

// Description : Check whether anomalies are recognized or not.
//             : Take AND of array x[] and y[], exclude double counting
// Input       : int x[], int nx, int y[], int ny,
// Return      : result of (x[]&y[]) -> array z[], and nz
//
void RegisterAnomaly(int x[], int nx, int y[], int ny, int z[], int &nz)
{
   // if ny=0, then copy x[] -> z[]. the main loop doesn't work for ny=0
   if (!ny) {
      for (int i=0;i<nx;i++) z[i]=x[i];
      nz=nx;
      return;
   }
 
   // main loop
   nz=0; int J=0;
   for(int i=0; i<nx; i++) {
 
     for (int j=J; j<ny ; j++){
       if (y[j]<x[i]) {
         z[nz]=y[j]; nz++; J++;
       } else if (y[j]==x[i]) {
         z[nz]=y[j]; nz++; J++;
         break;
       } else {
         z[nz]=x[i]; nz++;
         break;
       }
     } // end-of-for(j)
 
   }//end-of-for(i)
 
   // above loop doesn't precess the largest number in x[] or y[] array
   if (x[nx-1]>y[ny-1]) {z[nz]=x[nx-1]; nz++;}
   if (y[ny-1]>x[nx-1]) {z[nz]=y[ny-1]; nz++;}
}


// Description : Check whether anomalies are recognized or not.
//             : Take AND of array x[] and y[], returns false of the tests
// Input       : int *x, int nx, int *y, int ny,
// Return      : unrecongnized (strip/bunch) ID in array z, and nz
void UnrecognizedAnomaly(int x[], int nx, int y[], int ny, int z[], int &nz)
{
   int match[nx];
 
   for (int i=0; i<nx; i++) match[i] = x[i];
 
   // Check for mathing between two arrays x[nx], y[ny]
   for (int i=0; i<ny; i++) {
      for (int j=0; j<nx; j++) {
         if (y[i] == match[j]) { match[j]=-1; break; }
      }
   }
 
   nz = 0;
 
   for (int i=0; i<nx; i++) {
      if (match[i]!=-1) { z[nz] = match[i]; nz++;}
   }
}


/*
// Description : check for bad bunches
void checkForBadBunches()
{
   // counter initiariztion
   gAnaResults.anomaly.nbunch=0;
   bnchchk.rate.allowance=errdet.BUNCH_RATE_SIGMA_ALLOWANCE;
 
   printf("checking for bad bunches\n");
 
   double avg;
   double sigma;
   for(int i=0;i<N_DETECTORS;i++)
   {
      avg=0.;
      for(int j=0;j<120;j++)
      {
         avg+=Ncounts[i][j];
      }
      avg=avg/120.;
 
      sigma=0.;
      for(int j=0;j<120;j++)
      {
         sigma+=((Ncounts[i][j]-avg)*(Ncounts[i][j]-avg));
      }
      sigma=sigma/120.;
      sigma=sqrt(sigma);
 
      for(int j=0; j<120; j++) {
         if((Ncounts[i][j]-avg)> bnchchk.rate.allowance*sigma)
         {
            gAnaResults.anomaly.bunch[gAnaResults.anomaly.nbunch]=j+1;
            gAnaResults.anomaly.nbunch++;
            printf("WARNING: bunch # %d has very many counts in detector # %d\n", j+1, i+1);
         }
      }
   }
 
   UnrecognizedAnomaly(gAnaResults.anomaly.bunch,gAnaResults.anomaly.nbunch,gRunInfo.DisableBunch,gRunInfo.NDisableBunch,
                       gAnaResults.unrecog.anomaly.bunch, gAnaResults.unrecog.anomaly.nbunch);
}
*/
