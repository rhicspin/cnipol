#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TMinuit.h>
#include <iostream.h>
#include <fstream>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <iomanip>

#define Debug 0
gROOT->Reset();

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//   FUNCTION to convert from (Edep, Dwidth) to Ekin
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Double_t 
KinFunc(Double_t *x, Double_t *par){
    
    const Double_t cp0[4] = {-0.5174,0.4172,0.3610E-02,-0.1286E-05}; 
    const Double_t cp1[4] = {1.0000,0.8703E-02,0.1252E-04,0.6948E-07};
    const Double_t cp2[4] = {0.2990E-05,-0.7937E-05,-0.2219E-07,-0.2877E-09};
    const Double_t cp3[4] = {-0.8258E-08,0.4031E-08,0.9673E-12,0.3661E-12};
    const Double_t cp4[4] = {0.3652E-11,-0.8652E-12,0.4059E-14,-0.1294E-15};
    Double_t pp[5];
    
    Double_t par0;
    Double_t x0 = x[0];
    if (par[0]>=0.) {
        par0 = fabs(par[0]);
    } else {
        par0 = 0.;
    }

    pp[0] = cp0[0] + cp0[1]*par0 + cp0[2]*pow(par0,2) + cp0[3]*pow(par0,3);
    pp[1] = cp1[0] + cp1[1]*par0 + cp1[2]*pow(par0,2) + cp1[3]*pow(par0,3);
    pp[2] = cp2[0] + cp2[1]*par0 + cp2[2]*pow(par0,2) + cp2[3]*pow(par0,3);
    pp[3] = cp3[0] + cp3[1]*par0 + cp3[2]*pow(par0,2) + cp3[3]*pow(par0,3);
    pp[4] = cp4[0] + cp4[1]*par0 + cp4[2]*pow(par0,2) + cp4[3]*pow(par0,3);
    
    Double_t Ekin = pp[0] + pp[1]*x0 + pp[2]*pow(x0,2) 
        + pp[3]*pow(x0,3) + pp[4]*pow(x0,4);
    
    if (Ekin != 0.0) {
        Double_t tof = 1419.98/sqrt(Ekin) + par[1];
    } else {
        Double_t tof =0.0;
    }

    return tof;

};


// -------------------------------------------------------------------
//     Fit the given Strip
// -------------------------------------------------------------------
//void KinFit::FitOne(Int_t St, Int_t mode)
void BenchMark()
{


  TFile *f = TFile::Open("benchmark.root");
  if (!f) { 
      cerr << "ERROR:" << filename << " not found" << endl;
      exit(-1);
  }

  Char_t hName[100];
  for (Int_t i=1;i<71;i++){

      sprintf(hName,"h%d",15000+i);  
      TH2D* h2d = (TH2D*) gDirectory->Get(hName);
      h2d -> SetStats(0);
      h2d -> SetName("h2d");
    

      //    TCanvas *CurC = new TCanvas("CurC","",1);

      // ======================================================
      // ====   The fit                                  ==
      // ======================================================
      TF1* myf = new TF1("myf","gaus",0.0,150.0);
      h2d->FitSlicesY(myf);
      TH1F* sfit = (TH1F*) gDirectory->Get("h2d_1");

      TF1 *kinf = new TF1("kinf",KinFunc, 200.0, 1000.0, 2);

      kinf->SetParameters(65.0, -16.0);
      kinf->SetParLimits(0, 0., 200.);
      kinf->SetParLimits(1, -50., 50.);

      //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
      sfit -> Fit(kinf,"E0","", 400, 900);
      //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    /*
        // Plot data points (Magenta)

        TH1D* Dpoints = (TH1D*) sfit->Clone();
        Dpoints -> SetName("Dpoints");
        Dpoints -> SetLineColor(6);
        Dpoints -> SetLineWidth(1.2);
        Dpoints -> GetXaxis()->SetRangeUser(300., 1000.);
        Dpoints -> Draw("");

        // Plot fit function (Green)

        TF1* fitfun = (TF1*) kinf->Clone();
        fitfun -> SetName("fitfun");
        fitfun -> SetLineColor(3);
        fitfun -> SetLineWidth(2.0);
        fitfun -> Draw("same");
    */

  }

	return;
}


