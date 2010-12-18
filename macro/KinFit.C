/**
 *
 * Nov 2, 2010 - Dmitri Smirnov
 *    -
 *
 * Nov 12, 2010 - Dmitri Smirnov
 *    - Put all definitions in header where they should be
 *    - Fixed lots of compilator errors and warnings (how can one be so
 *      sloppy?)
 *    - This macro can be compiled now
 *
 */

#include "KinFit.h"

using namespace std;

void ExeKinFit(string runName,  Float_t bene=0, Int_t RHICBeam=1, Float_t E2T=0,
   Float_t EMIN=400, Float_t EMAX=900, Int_t HID=15000, Char_t *cfile="",
   Char_t *online_cfile="", Char_t *outputdir="./", Int_t mode=1)
{
   // open root file 
   const char* resDir = getenv("CNIPOL_RESULTS_DIR");

   string outDir("");
   outDir.reserve(255);
   sprintf(&outDir[0], "%s/%s", resDir, runName.c_str());

   string fileName("");
   fileName.reserve(255);
   sprintf(&fileName[0], "%s/%s.root", outDir.c_str(), runName.c_str());

   TFile *f = TFile::Open(fileName.c_str());
 
   if (!f) { 
       cerr << "ERROR:" << fileName << " not found" << endl;
       exit(-1);
   }

   CnipolHists hists;
   hists.ReadFromDir(f);
 
   KinFit *kfit = new KinFit(runName, bene, RHICBeam, E2T, EMIN, EMAX, HID, cfile,
                             online_cfile, outputdir);

   kfit->fHists = &hists;
   kfit->Fit(0);    // arg = 0 ... new fit with two free paramters 
   //kfit->Fit(1);    
   //kfit->Fit(mode);    
 
   kfit->PlotResult();
     
   delete kfit;
}


Double_t KinFunc(Double_t *x, Double_t *par)
{
   Double_t pp[5];
   Double_t x0 = x[0];
   Double_t x2 = x0*x0;

   Double_t par0 = 0;
   if (par[0] >= 0) par0 = TMath::Abs(par[0]);
   Double_t p2 = par0*par0;
   Double_t p3 = par0*par0*par0;

   pp[0] = cp0[0] + cp0[1]*par0 + cp0[2]*p2 + cp0[3]*p3;
   pp[1] = cp1[0] + cp1[1]*par0 + cp1[2]*p2 + cp1[3]*p3;
   pp[2] = cp2[0] + cp2[1]*par0 + cp2[2]*p2 + cp2[3]*p3;
   pp[3] = cp3[0] + cp3[1]*par0 + cp3[2]*p2 + cp3[3]*p3;
   pp[4] = cp4[0] + cp4[1]*par0 + cp4[2]*p2 + cp4[3]*p3;

   Double_t Ekin = pp[0] + pp[1]*x0 + pp[2]*x2 + pp[3]*x2*x0 + pp[4]*x2*x2;
   Double_t tof = (Ekin != 0.0) ?  KinConst_E2T/sqrt(Ekin) + par[1] : 0.0 ;

   return tof;
}


// Extracting the Coefficients
Double_t ExCoef(Double_t x, Int_t Index)
{
   // x : dlayer [ug/cm2]
   // Index : the Index-th order of coefficient

   const Double_t cp0[4] = {-0.5174     , 0.4172      , 0.3610E-02  , -0.1286E-05};
   const Double_t cp1[4] = {1.0000      , 0.8703E-02  , 0.1252E-04  , 0.6948E-07};
   const Double_t cp2[4] = {0.2990E-05  , -0.7937E-05 , -0.2219E-07 , -0.2877E-09};
   const Double_t cp3[4] = {-0.8258E-08 , 0.4031E-08  , 0.9673E-12  , 0.3661E-12};
   const Double_t cp4[4] = {0.3652E-11  , -0.8652E-12 , 0.4059E-14  , -0.1294E-15};

   Double_t pp[5];

   pp[0] = cp0[0] + cp0[1]*x + cp0[2]*pow(x,2) + cp0[3]*pow(x,3);
   pp[1] = cp1[0] + cp1[1]*x + cp1[2]*pow(x,2) + cp1[3]*pow(x,3);
   pp[2] = cp2[0] + cp2[1]*x + cp2[2]*pow(x,2) + cp2[3]*pow(x,3);
   pp[3] = cp3[0] + cp3[1]*x + cp3[2]*pow(x,2) + cp3[3]*pow(x,3);
   pp[4] = cp4[0] + cp4[1]*x + cp4[2]*pow(x,2) + cp4[3]*pow(x,3);

   return pp[Index];
}


//
// Class name  :
// Method name : BaseName(Char_t *filename)
//
// Description : strip off directories from filename
// Input       : Char_t *filename
// Return      : string *filename without directory path
//
string BaseName(Char_t *filename)
{
    Int_t   ptr = 0;
    Char_t *pch = strchr(filename,'/');

    while (pch!=NULL) {
       ptr=pch-filename+1;
       pch=strchr(pch+1,'/');
    }

    string str(filename);

    return  str.substr(ptr,strlen(filename));
}


// Initialization of the class
KinFit::KinFit(string runidinput, Float_t beneinput, Int_t RHICBeam, Float_t E2T, Float_t EMIN,
               Float_t EMAX, Int_t hid, Char_t *cfile, Char_t *online_cfile, Char_t *outputdir)
{
   gROOT->Reset();
   // default online deadlayer plot on
   flag.PLOT_ONLINE_CONFIG=1;

   sprintf(runid, "%s", runidinput.c_str());
   RUNID        = atof(runid);
   RHIC_Beam    = RHICBeam;
   bene         = beneinput;
   HID          = hid;
   //KinConst_E2T = E2T;
   FitRangeLow  = EMIN;
   FitRangeUpp  = EMAX;

   sprintf(CONFFILE, "%s", cfile);

   sprintf(ONLINE_CONFFILE, "%s", online_cfile);
   if (!strlen(ONLINE_CONFFILE)) flag.PLOT_ONLINE_CONFIG = 0;

   sprintf(OUTPUTDIR, "%s", outputdir);
   if (!strlen(OUTPUTDIR)) sprintf(OUTPUTDIR, ".");

   if (RUNID > 7400) RHIC_Beam += 2;

   printf("\n");
   printf("=========================================================\n");
   printf(" RunID                            : %s \n", runid);
   printf(" Beam Energy [GeV]                : %6.2f\n", bene);
   if (HID==15000) printf(" Fit on constant time cut, HID   : %5d\n", HID);
   if (HID==15100) printf(" Fit on  banana-mass  cut, HID   : %5d\n", HID);
   printf(" Fitting Energy Range <Emin-Emax> :%4.1f-%4.1f\n", FitRangeLow, FitRangeUpp);
   printf(" Configuration File               : %s \n", CONFFILE);
   printf(" Online Configuration File        : %s \n", ONLINE_CONFFILE);
   if (strlen(OUTPUTDIR)!=1)
      printf(" Temporary Output Directory       : %s \n", OUTPUTDIR);
   printf("=========================================================\n");
   printf("\n");

   memset(dl,  0, sizeof(dl) );
   memset(dlE, 0, sizeof(dlE));
   memset(t0,  0, sizeof(t0) );
   memset(t0E, 0, sizeof(t0E));

   // globally used. St runs from 0 -> 71, whereas strip 1 -> 72.
   for (Int_t St=0; St<72; St++) {
       strip[St] = (Float_t)(St+1);
       stripE[St] = 0.;
   }

   // Dl, t0 plotting range initialization
   plotrange.TMIN = -35.;
   plotrange.TMAX =  10.;
   plotrange.DMIN =   0.;
   plotrange.DMAX =  80.;

   if (RUNID > 7400) { //  for Run06
       plotrange.TMIN = -25;
       plotrange.TMAX = 10;
   }

   devpst = 0;
   dlave  = 0;
}


KinFit::~KinFit() {}


// -------------------------------------------------------------------
//  fit the E-T band with non-linear dlayer corrected function
//  plot and extract fit results
//  mode : 0 ... two free parameters
//         1 ... fix dlayer width, t0 free parameter
// -------------------------------------------------------------------
void KinFit::Fit(Int_t mode)
{
   Char_t filename[100];

   if (!mode&1) { // two param
      cout << "TRY TWO FREE PARAMETER FIT" << endl;
   } else if (mode&1) { // one param
      cout << "TRY T0 PARAMETER FIT (fixed Dlayer)" << endl;
   }

   TCanvas *CurC = new TCanvas("CurC1", "CurC1", 1500, 1000);

   sprintf(filename, "%s/testfit.ps", OUTPUTDIR);
   TPostScript ps(filename, 112);

   //if (mode&1) {
   sprintf(filename,"%s/testfit.dat", OUTPUTDIR);
   fout.open(filename);

   gStyle->SetGridColor(1);
   gStyle->SetGridStyle(2);
   gStyle->SetPalette(1);

   CurC->Divide(4, 3);
   ps.NewPage();
   //}

   static Int_t NValidSt = 0;
   T0.Delta = 0;

   // Loop over detectors
   for (Int_t Si=0; Si<6; Si++) {

      Int_t Padn       = 0;
      Float_t dl_accum = 0;
      Int_t valstn     = 0;
      Int_t j          = 0;

      // Loop over strips in detector
      for (Int_t St=Si*12; St<Si*12+12; St++) {

         dlValid[St] = dlEValid[St] = dlSi[j] = dlESi[j] = 0;

         Char_t hName[100];
         //sprintf(hName, "h%d", HID+St+1);
         string sSt("  ");
         sprintf(&sSt[0], "%02d", St+1);

         //TH2D* htemp = (TH2D*) gDirectory->Get(hName);
         TH2F* htemp = (TH2F*) fHists->d["channel"+sSt].o["hTimeVsEnergyA_st"+sSt];

         if (htemp) {
            htemp->Print();
				//return;
         } else return;

         Padn++;   // change pad even if the histograms is empty
         //if (mode&1) CurC->cd(Padn);
         CurC->cd(Padn);

         if (htemp->GetEntries() > 20000) {

            // Main fit procedure
            FitOne(htemp, St, mode);

            // fill arrays only if strip is valid
            //if (!mode&1) { // mode=0, two param
               if (!DisableList(RHIC_Beam, St)) {
                  dl_accum    += dl[St];
                  dlValid[St]  = dl[St];
                  dlEValid[St] = dlE[St];
                  dlSi[j]      = dl[St];
                  dlESi[j]     = dlE[St];
                  valstn++;
               }
            //} else if (mode&1) { // mode=1
               if (!DisableList(RHIC_Beam,St)) {
                  T0.Valid[St]  = t0s[St];
                  T0.ValidE[St] = t0sE[St];
                  if (St>0) T0.Delta += fabs(t0[St] - t0[St-1]);
               }
            //}
         }
         ++j;
      }

      //if (mode&1) { // one param
      CurC->Update();
      if (Si!=5) ps.NewPage();
      //}

      //if (!mode&1) { // two param
      dlsum[Si] = WeightedMean(dlSi, dlESi, 12);
      SiDev[Si] = 0;

      for (Int_t St=Si*12; St<Si*12+12; St++) {
        if (!DisableList(RHIC_Beam,St)){
          StDev[St]  = fabs(dlsum[Si] - dlValid[St]);
          SiDev[Si] += fabs(dlsum[Si] - dl[St]);
        }
      }

      totSiDev += SiDev[Si];
      NValidSt += valstn;
      dlave    += dl_accum;
      //}
   }

   //if (!mode&1) { // two param
   dlave = (Float_t) WeightedMean(dlValid, dlEValid, NSTRIP);

   if (NValidSt) {
     //dlave/=float(NValidSt);
     devpst = WeightedMean(StDev, dlEValid, NSTRIP);
   }

   //} else if (mode&1) { // one param
   T0.ave = (Float_t) WeightedMean(T0.Valid, T0.ValidE, NSTRIP);
   T0.Delta /= (NValidSt-1);
   printf("\n\n");
   printf("-----------------------------------------------------------\n");
   printf(" number of valid strips %3d\n", NValidSt);
   printf(" dlave = %7.2f\n",dlave);
   printf(" Chi2/Det=");
   for (Int_t Si=0;Si<6;Si++) printf(" %7.1f",SiDev[Si]);
   printf("\n dLayer/det=");
   for (Int_t Si=0;Si<6;Si++) printf(" %7.1f",dlsum[Si]);
   printf("\n Total Deviation=%7.1f\n",totSiDev);
   printf(" Deviation/strip=%7.1f\n",devpst);
   printf(" t0 average= %7.1f\n", T0.ave);
   printf(" Delta_t0 average= %7.1f\n", T0.Delta);
   printf("-----------------------------------------------------------\n");
   printf("\n\n");
   fout.close();
   ps.Close();
   //}
}


// -------------------------------------------------------------------
//     Fit the given Strip
// -------------------------------------------------------------------
void KinFit::FitOne(TH2* h, Int_t St, Int_t mode)
{
   Int_t Si = (Int_t) St/12;

   cout << " ====================================" << endl;
   cout << " === Now starting - " << St+1 << " ( mode: " << mode << " )" << endl;
   cout << " ====================================" << endl;

   Char_t hName[100];
   //sprintf(hName, "h%d", HID+St+1);
   //TH2D* h2d = (TH2D*) gDirectory->Get(hName);
	TH2D *h2d = (TH2D*) h;

   h2d->SetStats(0);
   sprintf(hName, "%s ST-%d", runid, St+1);
   h2d->SetTitle(hName);
   h2d->SetName("h2d");
   h2d->GetXaxis()->SetTitle("Measured Energy (keV)");
   h2d->GetYaxis()->SetTitle("Tof");

   h2d->GetXaxis()->SetRangeUser(100., 1000.);
   h2d->GetYaxis()->SetRangeUser(0., 100.);

   TH2D* h2dcarbon = (TH2D*) h2d->Clone();
   h2dcarbon->SetName("h2dcarbon");

   //Extracting information of the 2D histogram
   //Int_t xnbins = h2dcarbon ->GetXaxis()->GetNbins();
   //Float_t xmin = h2dcarbon ->GetXaxis()->GetXmin();
   //Float_t xmax = h2dcarbon ->GetXaxis()->GetXmax();
   //Int_t ynbins = h2dcarbon ->GetYaxis()->GetNbins();
   //Float_t ymin = h2dcarbon ->GetYaxis()->GetXmin();
   //Float_t ymax = h2dcarbon ->GetYaxis()->GetXmax();

   // ======================================================
   // ====   The fit                                  ==
   // ======================================================

   // Fit slices for the 1st fit (auto fit routine with gaussian)

   TF1* myf = new TF1("myf","gaus",0.0,150.0);

   h2d->FitSlicesY(myf);

   // Get histogram with means
   TH1F *sfit = (TH1F*) gDirectory->Get("h2d_1");

   // Define banana function
   TF1 *kinf = new TF1("kinf", KinFunc, 100.0, 1000.0, 2);

   // Set the default values
   // Dlayer Fix/ Or not

   kinf->SetParLimits(1, -50., 50.);

   if ( !mode & 1) { // two parameter fit

      kinf->SetParameters(65.0, -16.0);
      kinf->SetParLimits(0, 0., 200.);
      kinf->SetParLimits(1, -100, 100.);

   } else if (mode&1) {

      if (!(mode>>1) & 1) { // fixed par[0] ???
         kinf->SetParameters(dlsum[Si], 0.0);
         kinf->SetParLimits(0, dlsum[Si], dlsum[Si]);
      } else { // this is an expert mode. Fix t0 and play with dlayer
         cout << "mode=" << mode << endl;
         kinf->SetParameters(dlsum[Si], t0[St]+5);
         kinf->SetParLimits(1, t0[St]+5, t0[St]+5);
      }
   }

   //if (mode & 1)
   h2d->Draw("colz");

   //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   sfit->Fit(kinf, "E0", "", FitRangeLow, FitRangeUpp);
   //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

   // putting in vectors
   Double_t ChiDOF = 0;

   //if (!mode&1) {

      // two parameter fit
      Chi2[St] = kinf->GetChisquare()/kinf->GetNDF();
      dl[St]   = fabs(kinf->GetParameter(0));
      dlE[St]  = Chi2[St] + kinf->GetParError(0);
      t0[St]   = kinf->GetParameter(1);
      t0E[St]  = Chi2[St] + kinf->GetParError(1);

   //} else if (mode&1) {

      // one parameter fit
      ChiDOF   = kinf->GetChisquare()/kinf->GetNDF();
      t0s[St]  = kinf->GetParameter(1);
      t0sE[St] = ChiDOF *  kinf->GetParError(1);

      //if (mode>>1&1){
         //dl[St]  = fabs(kinf->GetParameter(0));
         //dlE[St] = Chi2[St] + kinf->GetParError(0);
      //}
   //}

   //if (mode&1) {

   // Plot data points (Magenta)
   TH1D* Dpoints = (TH1D*) sfit->Clone();
   Dpoints->SetName("Dpoints");
   Dpoints->SetLineColor(kBlack);
   Dpoints->SetMarkerStyle(20);
   Dpoints->SetMarkerSize(0.5);
   Dpoints->SetMarkerColor(kBlack);
   Dpoints->SetLineWidth(3);
   Dpoints->GetXaxis()->SetRangeUser(200., 1000.);
   Dpoints->Draw("same p");

   // Plot fit function (Green)
   TF1* fitfun = (TF1*) kinf->Clone();
   fitfun->SetName("fitfun");
   fitfun->SetLineColor(kRed);
   fitfun->SetLineWidth(3);
   fitfun->Draw("same");

   // TYPE the result on the Plots
   TText t; Char_t rtext[50];
   sprintf(rtext, "Dl = %6.1f ug/cm2", dl[St]); //kinf->GetParameter(0));
   t.DrawTextNDC(0.5, 0.8, rtext);
   sprintf(rtext, "T0 = %6.2f nsec",   kinf->GetParameter(1));
   t.DrawTextNDC(0.5, 0.72, rtext);
   sprintf(rtext, "Chi2/d.o.f = %6.2f", Chi2[St]);
   if (Chi2[St]>40) t.SetTextColor(2);
   t.DrawTextNDC(0.5, 0.64, rtext);
   t.SetTextColor(1);

   // for Residuals study
   Double_t fitpar[2], x[2];

   for (int l=0; l<2; l++) fitpar[l] = kinf->GetParameter(l);

   for (int k=0; k<Dpoints->GetNbinsX(); k++) {
       x[0] = Dpoints->GetBinCenter(k);
       resd.x[St][k] = x[0];
       resd.y[St][k] = Dpoints->GetBinContent(k) - KinFunc(x, fitpar);
   }

   // Calculate the Linear Function Coefficients
   // Using the reference of 400-900keV fit
   //             const0   const1  const2     slope0  slope1
   // 204-583keV  0.2035   1.1013  0.0075     1.0098  0.0036
   // 300-900keV  -0.480095 1.81638 0.00702198 1.00975 0.00213976
   // 400-900keV  -0.545298 2.05241 0.00689393 1.00961 0.00182463
   // 500-900keV  -0.71873 2.27486 0.00654193 1.00909 0.0015731

   Const[St] = 0.235 + 1.1013 * dlsum[(Int_t)St/12]
       + 0.0075 * dlsum[(Int_t)St/12]*dlsum[(Int_t)St/12];

   Slope[St] = 1.0098 + 0.0036 * dlsum[(Int_t)St/12];

   // Write Out the Result to File
   fout << St << " ";                                          // 0

   if (!(mode>>1)&1) {
     fout << setprecision(4) << dlsum[(Int_t)St/12]  <<" ";    // 1
   }else{
     fout << setprecision(4) << dl[St] << " ";                 // 1
   }

   fout << setprecision(4) << t0s[St]  <<" "; // 2
   fout << setprecision(4) << t0sE[St] <<" "; // 3
   fout << setprecision(4) << dl[St]  <<" ";
   fout << setprecision(4) << dlE[St] <<" ";
   fout << setprecision(4) << t0[St]  <<" ";
   fout << setprecision(4) << t0E[St] <<" ";
   fout << setprecision(3) << kinf->GetChisquare()/kinf->GetNDF() <<" ";
   fout << kinf->GetNDF()  << " ";
   fout << gMinuit->fCstatu.Data() << " ";
   fout << setprecision(4) << Const[St] << " ";  // 11
   fout << setprecision(4) << Slope[St] << " ";  // 12
   fout << setprecision(5) << ExCoef(dlsum[(Int_t)St/12], 0) <<" ";
   fout << setprecision(5) << ExCoef(dlsum[(Int_t)St/12], 1) <<" ";
   fout << setprecision(5) << ExCoef(dlsum[(Int_t)St/12], 2) <<" ";
   fout << setprecision(5) << ExCoef(dlsum[(Int_t)St/12], 3) <<" ";
   fout << setprecision(5) << ExCoef(dlsum[(Int_t)St/12], 4) << endl;
   //}

   //h2d_0    -> Delete();    // just to reduce the error message
   //h2d_1    -> Delete();
   //h2d_2    -> Delete();
   //h2d_chi2 -> Delete();
   kinf->Delete();
   myf->Delete();
}


// -------------------------------------------------------------------
//     Plot Residuals of deadlayer fit
// -------------------------------------------------------------------
void KinFit::Residual()
{
   gStyle->SetGridColor(1);
   gStyle->SetGridStyle(2);

   TCanvas *CurC = new TCanvas("CurC2", "CurC2", 1500, 1000);

   Char_t filename[100];
   sprintf(filename, "%s/residual.ps", OUTPUTDIR);
   TPostScript ps(filename, 112);

   CurC->Divide(4,3);
   ps.NewPage();

   for (Int_t Si=0; Si<6; Si++) {

      Int_t Padn = 0;

      for (Int_t St=Si*12; St<Si*12+12; St++) {
          Padn++;   // change pad even if the histograms is empty
          CurC->cd(Padn);
          PlotResidual(St);
      }

      CurC->Update();

      if (Si!=5) ps.NewPage();
   }

   ps.Close();
}


// -------------------------------------------------------------------
//     Plot Residuals of deadlayer fit
// -------------------------------------------------------------------
void KinFit::PlotResidual(Int_t St)
{
    Float_t x[1000], y[1000], dx[1000], dy[1000];

    for (Int_t k=0; k<1000; k++) {
        x[k]  = resd.x[St][k];
        y[k]  = resd.y[St][k];
        dx[k] = dy[k] = 0;
    }

    Char_t title[100];
    sprintf(title, "%s ST-%d", runid, St+1);
    TH2D* frame = new TH2D(title, title, 10, 195, 1185, 10, -1.5, 1.5);
    frame->SetStats(0);
    frame->GetXaxis()->SetTitle("deposited energy [keV]");
    frame->GetYaxis()->SetTitle("residuals [ns]");
    frame->Draw();

    TGraphErrors* tgae = new TGraphErrors(1000, x, y, dx, dy);
    tgae->SetMarkerStyle(20);
    tgae->SetMarkerSize(0.5);
    tgae->SetLineWidth(2);
    tgae->SetMarkerColor(kRed);
    tgae->Draw("P");

    TLine *l1 = new TLine(FitRangeLow, -1.5, FitRangeLow, 1.5);
    l1->Draw();
    TLine *l2 = new TLine(FitRangeUpp, -1.5, FitRangeUpp, 1.5);
    l2->Draw();
}


// -------------------------------------------------------------------
//     Plot the result of the Fit
// -------------------------------------------------------------------
void KinFit::PlotResult()
{
    // plot Residuals
    Residual();

    TCanvas *CurC = new TCanvas("CurC3", "CurC3", 1500, 1000);

    Char_t filename[100];
    sprintf(filename, "%s/testsummary.ps", OUTPUTDIR);
    TPostScript ps(filename,112);
    ps.NewPage();

    TLegend * aLegend = new TLegend(0.7,0.1,0.9,0.3);
    CurC -> Divide(1,2);

    // -------------
    // Plot - 1 (Dead layer distribution)
    // -------------
    CurC -> cd(1);
    PlotDlayer(1, aLegend);

    // -------------
    // Plot - 2 (T0 distribution)
    // -------------
    CurC -> cd(2);
    PlotT0(1);

    ps.NewPage();

    aLegend->Delete();

    //-----------------------------------------------------------------
    //    Compare with dl and t0 in Configulation File
    //-----------------------------------------------------------------
    aLegend = new TLegend(0.7, 0.1, 0.9, 0.3);

    if (ReferenceConfig()) {
       printf(" --> Comparison to reference is skipped.\n");
    } else {
       ps.NewPage();
       ReferenceDlayer(aLegend);
    }

    ps.Close();
}


//
// Class name  :
// Method name : ReferenceDlayer()
//
// Description : Compare fitting results with reference deadlayer
// Input       :
// Return      :
//
Int_t KinFit::ReferenceDlayer(TLegend *aLegend)
{
    // Get deadlayer data defined in current config.
    GetDlayerFromFile();

    //========================
    //    Plot 1 - dl
    //========================
    TCanvas *CurC = new TCanvas("CurC4", "CurC4", 1500, 1000);

    CurC->cd(1);
    PlotDlayer(2, aLegend);

    // if online config file is defined, superpose on plot
    if (flag.PLOT_ONLINE_CONFIG) {
        GetDlayer(ONLINE_CONFFILE);
        SuperposeDlayerPlot(ONLINE_CONFFILE, aLegend, 7);

        //Restore current configulation file before t0 plot
        GetDlayer(CONFFILE);
    }

    //========================
    //    Plot 2 - t0
    //========================
    CurC -> cd(2);
    PlotT0(2);

    // if online config file is defined, superpose on plot
    if (flag.PLOT_ONLINE_CONFIG) {
        GetDlayer(ONLINE_CONFFILE);
        SuperposeT0Plot(7);
    }

    return 0;
}


//
// Class name  :
// Method name : GetDlayer()
//
// Description : Get DLAYERFILE from CONFIG_FILE and read data from DLAYEFILE
// Input       :
// Return      :
//
Int_t KinFit::GetDlayer(Char_t* CONFIG_FILE)
{
    GetData(CONFIG_FILE);
    GetDlayerFromFile();

    return 1;
}


//
// Class name  :
// Method name : GetDlayerFromFile()
//
// Description : Read in Deadlayers from DLAYEFILE
// Input       :
// Return      :
//
Int_t KinFit::GetDlayerFromFile()
{
    ifstream infile(DLAYERFILE);

    if ( infile.fail() ) {
       cout << "unable to find file:" << DLAYERFILE << "\n" << flush;
       return -1;
    }

    Char_t dumc[10];
    Int_t i=0;
    Int_t ch;
    Float_t dum1,dum2,dum4,dum5,dum6,dum7,dum8,dum9,dum10;
    Float_t St[NSTRIP],dlsum_d[NSTRIP];
    while  ( ( ch = infile.peek()) != EOF) {

        infile >> St[i] >> dlsum_d[(Int_t)St[i]/12] >> t0s_d[i] >> t0sE_d[i] >> dl_d[i] >> dlE_d[i]
               >> t0_d[i] >> t0E_d[i] >> dum1 >> dum2 >> dumc >> dum4 >> dum5 >> dum6 >> dum7
               >> dum8 >> dum9 >> dum10;
        if ((St[i]>=NSTRIP-1) || (i==NSTRIP-1)) break;
        ++i;
    }

    return 0;
}


//
// Class name  :
// Method name : PlotDlayer
//
// Description : plot deadlayer fitting result
// Input       : Int_t Mode
// Return      :
//
Int_t KinFit::PlotDlayer(Int_t Mode, TLegend *aLegend)
{
   // Fill Disabled Strip Arrays
   Float_t disable_strip[NDisableStrip];
   Float_t disable_dl[NDisableStrip];
   Float_t disable_dlE[NDisableStrip];

   for (Int_t i=0; i<NDisableStrip; i++){
      disable_dl[i]    = disable_dlE[i] = 0; // initiarization
      disable_strip[i] = DisableStr[RHIC_Beam][i] + 1;

      if (DisableStr[RHIC_Beam][i] != -1) {
         disable_dl[i]  = dl[int(DisableStr[RHIC_Beam][i])];
         disable_dlE[i] = dlE[int(DisableStr[RHIC_Beam][i])];
      }
   }

   Char_t title[40];
   sprintf(title, "%s Dead Layer Distribution", runid);
   TH2D* framed;

   if(Mode==2) {
      framed = new TH2D("dead layer 2", title, 10, 0.5, 72.5, 10, plotrange.DMIN, 100.);
   } else {
      framed = new TH2D("dead layer 1", title, 10, 0.5, 72.5, 10, plotrange.DMIN, 100.);
   }

   framed->SetStats(0);
   framed->GetXaxis()->SetTitle("Strip Number");
   framed->GetYaxis()->SetTitle("Dead Layer (ug/cm**2)");
   framed->Draw();

   // draw the separators btw detectors
   for (Int_t isep=0; isep<6 ; isep++) {
      TLine *l = new TLine(12.*isep +0.5, 0., 12.*isep +0.5, 100.);
      l->Draw();
   }

   TGraphErrors* tgdl = new TGraphErrors(72, strip, dl, stripE, dlE);
   tgdl->SetMarkerStyle(20);
   tgdl->SetMarkerSize(1.0);
   tgdl->SetLineWidth(1);
   tgdl->SetLineColor(4);
   tgdl->SetMarkerColor(4);
   tgdl->Draw("P");
   aLegend->AddEntry(tgdl,runid,"P");

   // Superposition Disabled Strips
   TGraphErrors* tgdlD = new TGraphErrors(NDisableStrip, disable_strip, disable_dl, stripE, disable_dlE);
   tgdlD->SetMarkerStyle(20);
   tgdlD->SetMarkerSize(1.0);
   tgdlD->SetLineWidth(1);
   tgdlD->SetLineColor(7);
   tgdlD->SetMarkerColor(7);
   tgdlD->Draw("P");

   if (Mode==1) {
      // draw total average value
      TLine *tave = new TLine(0.5, dlave, 72.5, dlave);
      tave->SetLineColor(kRed);
      tave->SetLineWidth(2);
      tave->Draw();

      // draw the average values
      for (Int_t isep=0; isep<6; isep++) {
         TLine *ave = new TLine(12.*isep+0.5, dlsum[isep], 12.*(isep+1)+0.5, dlsum[isep]);
         ave->Print();
         ave->SetLineColor(kMagenta);
         ave->SetLineWidth(2);
         ave->Draw();
      }

      TText t; Char_t rtext[50];
      sprintf(rtext, "Average Dead Layer = %6.1f ug/cm2", dlave);
      t.DrawTextNDC(0.5, 0.80, rtext);
      sprintf(rtext, "Average Deviation = %6.1f ug/cm2", devpst);
      t.DrawTextNDC(0.5, 0.75, rtext);

   } else if (Mode==2) {

       // plot deadlayer from current configulation file.
       SuperposeDlayerPlot(CONFFILE, aLegend, 2);
   }

   return 0;
}


//
// Class name  :
// Method name : SuperposeDlayerPlot(Int_t Mode)
//
// Description : Add deadlayer points on current panel
// Input       : Int_t Mode
// Return      :
//
Int_t KinFit::SuperposeDlayerPlot(Char_t* CONFIG_FILE, TLegend *aLegend, Int_t pmci)
{
   TGraphErrors * tgd2 = new TGraphErrors(72, strip, dl_d, stripE, dlE_d);
   tgd2 -> SetMarkerStyle(20);
   tgd2 -> SetMarkerSize(1.0);
   tgd2 -> SetLineWidth(1);
   tgd2 -> SetLineColor(pmci);
   tgd2 -> SetMarkerColor(pmci);
   tgd2 -> Draw("P");

   string s = BaseName(CONFIG_FILE);
   aLegend->AddEntry(tgd2,s.c_str(),"P");
   aLegend-> Draw("same");

   return 0;
}


//
// Class name  :
// Method name : PlotT0(Int_t Mode)
//
// Description : plot T0 fitting results
// Input       : Int_t Mode
// Return      :
//
Int_t KinFit::PlotT0(Int_t Mode)
{
   Float_t TMIN=plotrange.TMIN;
   Float_t TMAX=plotrange.TMAX;

   Char_t title[40];
   sprintf(title, "%s : T0 Distribution", runid);
   TH2D* framet;

   if (Mode==2) {
      framet = new TH2D("T0 2", title, 10, 0.5, 72.5, 10, TMIN, TMAX);
   } else {
      framet = new TH2D("T0 1", title, 10, 0.5, 72.5, 10, TMIN, TMAX);
   }

   framet -> SetStats(0);
   framet -> GetXaxis()->SetTitle("Strip Number");
   framet -> GetYaxis()->SetTitle("T0 values (nsec)");
   framet -> Draw();

   // draw the separaters btw detectors
   for (Int_t isep=0; isep<6 ; isep++) {
      TLine *l = new TLine(12.*isep +0.5, TMIN, 12.*isep +0.5, TMAX);
      l -> Draw();
   }

   // result of two paramter fit (blue)
   TGraphErrors* tgt0 = new TGraphErrors(72, strip, t0, stripE, t0E);
   tgt0->SetMarkerStyle(20);
   tgt0->SetMarkerSize(1.0);
   tgt0->SetLineWidth(1);
   tgt0->SetLineColor(4);
   tgt0->SetMarkerColor(4);

   tgt0->Draw("P");

   if (Mode==1) {
      // result of one parameter fit (red)
      TGraphErrors* tgt0s = new TGraphErrors(72, strip, t0s, stripE, t0sE);
      tgt0s -> SetMarkerStyle(20);
      tgt0s -> SetMarkerSize(0.9);
      tgt0s -> SetLineWidth(1);
      tgt0s-> SetLineColor(2);
      tgt0s -> SetMarkerColor(2);
      tgt0s -> Draw("P");

      TText t; Char_t rtext[50];
      sprintf(rtext, "Average Delta_t0 = %6.2f [ns]", T0.Delta);
      t.DrawTextNDC(0.5, 0.8, rtext);

   } else if (Mode==2) {
       SuperposeT0Plot(2);
   }

   //CurC->Update();

   return 0;
}


//
// Class name  :
// Method name : SuperposeT0Plot(Int_t pmci)
//
// Description : Add T0 plot on current panel
// Input       : Int_t Mode
// Return      : 0
//
Int_t KinFit::SuperposeT0Plot(Int_t pmci)
{
        TGraphErrors* tgt0d = new TGraphErrors(72, strip, t0_d, stripE, t0E_d);
        tgt0d -> SetMarkerStyle(20);
        tgt0d -> SetMarkerSize(1.0);
        tgt0d -> SetLineWidth(1);
        tgt0d -> SetLineColor(pmci);
        tgt0d -> SetMarkerColor(pmci);
        tgt0d -> Draw("P");

        return 0;
}


//
// Class name  :
// Method name : ReferenceConfig()
//
// Description : Compare fitting results with reference config.
// Input       :
// Return      :
//
Int_t KinFit::ReferenceConfig()
{
    if (GetData(CONFFILE)){ return -1; }

    Float_t diffx_Min = -15;
    Float_t diffx_Max =  15;
    Float_t difft_Min = -15;
    Float_t difft_Max =  15;

    TCanvas *CurC = new TCanvas("CurC5", "CurC5", 1500, 1000);
    CurC->Divide(1,2);

    // -------------
    // Plot - 1 (Dead layer distribution)
    // -------------

    CurC->cd(1);

    Char_t title[40];
    sprintf(title, "%s Dead Layer Difference w.r.t. Ref.", runid);
    TH2D* framer = new TH2D(title, title, 10, 0.5, 6.5, 10, diffx_Min, diffx_Max);
    framer -> SetStats(0);
    framer -> GetXaxis()->SetTitle("Detector Number");
    framer -> GetYaxis()->SetTitle("Dl(current) - D0(Ref) (#mu g/cm**2)");
    framer -> Draw();

    // draw the separaters btw detectors
    for (Int_t isep=0; isep<6 ; isep++) {
        TLine *l = new TLine(isep+0.5, diffx_Min, isep+0.5, diffx_Max);
        l -> Draw();
    }
    TLine *L = new TLine(0.5, 0,6.5, 0);
    L -> Draw();

    // draw the average values
    Float_t diff[6];
    TLine *ave = 0;

    for (Int_t isep=0; isep<6 ; isep++) {
       // deadlayer from current fit
       diff[isep] = dlsum_ref[isep] - dlsum[isep];
       ave = new TLine(isep+0.5, diff[isep],
                              (isep+1)+0.5, diff[isep]);
       ave -> SetLineColor(4);
       ave -> SetLineWidth(2);
       ave -> Draw();
    }

    Char_t text[256];
    sprintf(text, "Reference:%s", CONFFILE);
    TLegend *aLegend = new TLegend(0.1,0.15,0.65,0.25);
    aLegend -> AddEntry(ave, text, "L");
    aLegend -> Draw("same");

    // -------------
    // Plot - 2 (t0)
    // -------------
    CurC->cd(2);

    Float_t diff_t0[72], diffE_t0[72];

    sprintf(title, "%s : T0 (1-par fit) diff w.r.t. Ref.", runid);
    TH2D* framet = new TH2D("framet", title, 10, 0.5, 72.5, 10, difft_Min, difft_Max);
    framet -> SetStats(0);
    framet -> GetXaxis()->SetTitle("Strip Number");
    framet -> GetYaxis()->SetTitle("T0(Current) - T0(Reference) (nsec)");
    framet -> Draw();

    // draw the separaters btw detectors
    for (Int_t isep=0; isep<6 ; isep++) {
        TLine *l = new TLine(12.*isep +0.5, difft_Min, 12.*isep +0.5, difft_Max);
        l -> Draw();
    }

    L = new TLine(0.5, 0,72.5, 0);
    L -> Draw();

    for (Int_t st=0; st<72; st++) {
        diff_t0[st] = t0s[st] - t0_ref[st];
        diffE_t0[st] = sqrt(t0sE[st]*t0sE[st]+t0E_ref[st]*t0E_ref[st]);
    }

    // result of one parameter fit (red)
    TGraphErrors* tgt0c = new TGraphErrors(72, strip, diff_t0, stripE, diffE_t0);
    tgt0c -> SetMarkerStyle(20);
    tgt0c -> SetMarkerSize(1.0);
    tgt0c -> SetLineWidth(1);
    tgt0c -> SetLineColor(2);
    tgt0c -> SetMarkerColor(2);

    tgt0c -> Draw("P");
    CurC->Update();

    return 0;
}


//
// Class name  :
// Method name : GetData(Char_t * CONFIG_FILE)
//
// Description : Read deadlayer/t0 data from CONFIG_FILE
// Input       :
// Return      :
//
Int_t KinFit::GetData(Char_t *CONFIG_FILE)
{
  ifstream infile(CONFIG_FILE);

  if ( infile.fail() ) {
    cout << "unable to find file:" << CONFFILE << "\n" << flush;
    return -1;
  }

  Char_t   buffer[300];
  string   dfile;
  Char_t  *SearchString = "* for the dead layer and T0  : ";
  Char_t  *tempchar;
  Int_t    stripn;
  Float_t  t0n, ecn, edeadn, a0n, a1n, ealphn, dwidthn, peden;
  Float_t  c0n, c1n, c2n, c3n, c4n;

  Int_t i=0;
  Int_t ch, st;

  while  ( ( ch = infile.peek()) != EOF) {

     infile.getline(buffer, sizeof(buffer), '\n');

     if (strstr(buffer, SearchString)!=0) {
         string str(buffer);
         dfile = str.substr(strlen(SearchString), strlen(buffer) - strlen(SearchString));
         sprintf(DLAYERFILE,"%s/%s", gSystem->Getenv("SHAREDIR"), dfile.c_str());
     }

     if (strstr(buffer,"Channel")!=0) {

        tempchar = strtok(buffer,"l");
        stripn   = atoi(strtok(NULL, "     = "));
        t0n      = atof(strtok(NULL," "));
        ecn      = atof(strtok(NULL," "));
        edeadn   = atof(strtok(NULL," "));
        a0n      = atof(strtok(NULL," "));
        a1n      = atof(strtok(NULL," "));
        ealphn   = atof(strtok(NULL," "));
        dwidthn  = atof(strtok(NULL," "));
        peden    = atof(strtok(NULL," "));
        c0n      = atof(strtok(NULL," "));
        c1n      = atof(strtok(NULL," "));
        c2n      = atof(strtok(NULL," "));
        c3n      = atof(strtok(NULL," "));
        c4n      = atof(strtok(NULL," "));

        st            = i/12;
        t0_ref[i]     = t0n;
        t0E_ref[i]    = 0;
        dlsum_ref[st] = dwidthn;

        if (i == 71) break;

        i++;
     }
  }

  return 0;
}


//=======================================================================
//          Check Disbled Strip List
//=======================================================================
Int_t KinFit::DisableList(Int_t RHIC_Beam, Int_t St)
{
  int Match=0;

  for (int j=0; j<NDisableList; j++) {
     Match = (St == DisableStr[RHIC_Beam][j] ? 1 : 0);
     if (Match) break;
  }

  return Match;
}


//
// Class name  :
// Method name : WeightedMean(double A[N], double dA[N], int NDAT)
//
// Description : calculate weighted mean
// Input       : double A[N], double dA[N], int NDAT
// Return      : weighted mean
//
Float_t KinFit::WeightedMean(Float_t A[72], Float_t dA[72], Int_t NDAT)
{
   Float_t sum1, sum2, dA2;
   sum1 = sum2 = dA2 = 0;

   for (int i=0; i<NDAT; i++) {
      if (dA[i] && !isnan(A[i]) && !isnan(dA[i])) {  // skip dA=0 data
         dA2   = dA[i]*dA[i];
         sum1 += A[i]/dA2;
         sum2 += 1/dA2;
      }
   }

   return (dA2 == 0) ? 0 : sum1/sum2;
}
