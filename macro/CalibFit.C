#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TMinuit.h>
#include <iostream.h>
#include <fstream>
#include <sstream>
#include <string.h>
#include <math.h>
#include <iomanip>

#define Debug 0

// 241 Am peak 5.486MeV (85%)
// 
//  21db = 0.0891251 
//  5db = 0.562341  
//  16db = 0.158489 
//  14db = 0.19953
//  14.?db = 0.2000  setting of attenuator board

#define AMPEAK 5486.0
#define CCONST 0.2000      // regular calibration 
//#define CCONST 0.4000       // attenuation (x2) runs


gROOT->Reset();

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  MAIN
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


class CalibFit {
public:
    CalibFit(Char_t *);
    ~CalibFit();

    void Fit(Int_t);
    void PlotResult();
    void PlotRatio();

    ofstream fout;
    
    Char_t runid[50];

    Float_t peak[72], peakE[72];   
    Float_t acoef[72], acoefE[72];
    Float_t gwidth[72], gwidthE[72];

};

// Initialization of the class
CalibFit::CalibFit(Char_t *runidinput){
    sprintf(runid,"%s",runidinput);
    memset(peak,0, sizeof(peak));
    memset(peakE,0, sizeof(peakE));
    memset(acoef,0, sizeof(acoef));
    memset(acoefE,0, sizeof(acoefE));
    memset(gwidth,0, sizeof(gwidth));
    memset(gwidthE,0, sizeof(gwidthE));
};
CalibFit::~CalibFit(){};

// -------------------------------------------------------------------
void CalibFit::Fit() 
// -------------------------------------------------------------------
{    

    fout.open("testfit.dat");
    
    gStyle->SetGridColor(1);
    gStyle->SetGridStyle(2);
    gStyle->SetPalette(1,0);
    gStyle->SetOptFit();


    TCanvas *CurC = new TCanvas("CurC","",1);
    TPostScript ps("testfit.ps",112);
    
    CurC -> Divide(4,3);
    ps.NewPage();
    
    for (Int_t Si=0;Si<6;Si++) {
        
        Int_t Padn=0;

        for (Int_t St=Si*12; St<Si*12+12; St++) {
            Char_t hName[100];
            sprintf(hName,"h%d",12000+St+1);  

            TH1D* hadc = (TH1D*) gDirectory->Get(hName);

            Padn++;   // change pad even if the histograms is empty
            CurC->cd(Padn);
            
            // Fit Only non-empty Hists

            if (hadc->GetEntries() > 30) {

                Float_t MaxPosition = hadc->GetMaximumBin( );
                
                hadc -> Fit("gaus","E", "0",MaxPosition-20., MaxPosition+20.);

                TF1 *fitf = (TF1*)hadc->GetFunction("gaus");;

                
                peak[St] = fitf->GetParameter(1);
                peakE[St] = fitf->GetParError(1);
		gwidth[St] = fitf->GetParameter(2);
		gwidthE[St] = fitf->GetParError(2);                
                
                
                // Draw +/- 7sigma region
                hadc -> GetXaxis()-> 
                    SetRangeUser(
                                 peak[St] - 7.* fitf->GetParameter(2),
                                 peak[St] + 7.* fitf->GetParameter(2));
                
                
                fitf -> SetLineColor(2);
                hadc -> SetLineColor(4);
                hadc -> Draw();

                if (peak[St]!=0.) {
                    acoef[St]  = AMPEAK * CCONST/peak[St];
                    acoefE[St] = AMPEAK * CCONST/pow(peak[St],2)*peakE[St];   
                }

                // ==== Force Default Numbers ==============
                //acoef[St] = 5.0;

                
                fout << St <<" ";
                fout << setprecision(4) << acoef[St]<<" ";
                fout << setprecision(4) << acoefE[St]<<" ";
                fout << setprecision(4) << peak[St]<<" ";
                fout << setprecision(4) << peakE[St]<<" ";
                fout << setprecision(3) << fitf->GetChisquare()
                    /fitf->GetNDF() <<" ";
                fout << fitf->GetNDF()<<" ";
                fout << gMinuit->fCstatu.Data() << endl;

            }
        }

        CurC->Update();
        ps.NewPage();
    }

    CurC->Update();
    fout.close();


    PlotRatio();
    ps.Close();

};



// -------------------------------------------------------------------
//     Plot the result of the Fit 
// -------------------------------------------------------------------
void CalibFit::PlotResult()
{

    TCanvas *CurC = new TCanvas("CurC","",1);
    TPostScript ps("testsummary.ps",112);
    
    Float_t strip[72], stripE[72];
    for (Int_t St=0; St<72; St++) {
        strip[St] = (Float_t)St;
        stripE[St] = 0.;
    }

    // -------------
    // Plot - 1 (Ecoef distribution)
    // -------------

    CurC -> SetGridy();
    Char_t title[100];
    sprintf(title, "(%s) Am calibration Result", runid);
    TH2D* frame = new TH2D("frame", title, 10, -0.5, 71.5, 10, 0., 15.);
    frame -> SetStats(0);
    frame -> GetXaxis()->SetTitle("Strip Number");
    frame -> GetYaxis()->SetTitle("Ecoefs (keV/ADC)");
    frame -> Draw();

    // draw the separaters btw detectors
    for (Int_t isep=0; isep<6 ; isep++) {
        TLine *l = new TLine(12.*isep -0.5, 0., 12.*isep -0.5, 15.);
        l -> Draw();
    }


    TGraphErrors* tgae = new TGraphErrors(72, strip, acoef, stripE, acoefE);
    tgae -> SetMarkerStyle(20);
    tgae -> SetMarkerSize(0.5);
    tgae -> SetLineWidth(1.0);
    tgae -> SetLineColor(4);
    tgae -> SetMarkerColor(4);
    
    tgae -> Draw("P");

    CurC -> Update();
    ps.Close();

}


// -------------------------------------------------------------------
//     Plot the peak/width ratio result of the Fit 
// -------------------------------------------------------------------
void CalibFit::PlotRatio()
{


    Float_t strip[72], stripE[72];
    Float_t ratio[72], ratioE[72];
    Char_t title[40];

    memset(ratio,0, sizeof(ratio));
    memset(ratioE,0, sizeof(ratioE));
    memset(stripE,0, sizeof(stripE));

    for (Int_t St=0; St<72; St++) {
        strip[St] = (Float_t)St;
    }

    TH1F* pdis = new TH1F("pdis","Peak Distribution",256,0,255);
    TH1F* wdis = new TH1F("wdis","Width Distribution",100,0,4);
    TH1F* rdis = new TH1F("rdis","Ratio Distribution",100,0,0.02);

//    pdis->SetStats(0);
    pdis->GetXaxis()->SetTitle("Peak Position");
    pdis->GetXaxis()->CenterTitle();

//    wdis->SetStats(0);
    wdis->GetXaxis()->SetTitle("Width");
    wdis->GetXaxis()->CenterTitle();

//    rdis->SetStats(0);
    rdis->GetXaxis()->SetTitle("Width/Peak");
    rdis->GetXaxis()->CenterTitle();

    CurC2 = new TCanvas();
    CurC2->SetFillColor(10);

    padL = new TPad("padL","Left Pad",  0.0,0.0, 0.3,1.0);
    padR = new TPad("padR","Right Pad", 0.3,0.0, 1.0,1.0);

    padL->SetFillColor(10);
    padL->Divide(1,3);

    padR->SetFillColor(10);
    padR->Divide(1,3);

    padL->Draw();
    padR->Draw();

// Peak positions (1)...
    Float_t pmin = 0.;
    Float_t pmax = 0.;
    for (int i=0; i<72; i++) {
     if (peak[i]-peakE[i] < pmin) pmin = peak[i]-peakE[i];
     if (peak[i]+peakE[i] > pmax) pmax = peak[i]+peakE[i];
     if (peak[i] > 0) pdis->Fill(peak[i]);
    }
    padL->cd(1);
    pdis->Draw();

    if (pmin < 0.) pmin = 1.1*pmin;
    else pmin = 0.9*pmin;

    padR->cd(1);
    sprintf(title,"Fitted Peak Positions Run %s", runid);
    TH2D* fr1 = new TH2D("fr1",title, 10, -0.5, 71.5, 10, pmin, 1.1*pmax);
    fr1->SetStats(0);
    fr1->GetXaxis()->SetTitle("Strip Number");
    fr1->GetXaxis()->CenterTitle();
    fr1->GetYaxis()->SetTitle("Peak Position");
    fr1->GetYaxis()->CenterTitle();

    fr1->Draw();

    TGraphErrors* gr1 = new TGraphErrors(72, strip, peak, stripE, peakE);
    gr1->SetMarkerStyle(20);
    gr1->SetMarkerSize(0.5);
    gr1->SetLineWidth(1.0);
    gr1->SetLineColor(4);
    gr1->SetMarkerColor(4);
    gr1->Draw("P");

// Fitted widths (2)...
    Float_t wmin = 0.;
    Float_t wmax = 0.;
    for (int i=0; i<72; i++) {
     if (gwidth[i]-gwidthE[i] < wmin) wmin = gwidth[i]-gwidthE[i];
     if (gwidth[i]+gwidthE[i] > wmax) wmax = gwidth[i]+gwidthE[i];
     if (gwidth[i] > 0) wdis->Fill(gwidth[i]);
    }
    padL->cd(2);
    wdis->Draw();

    if (wmin < 0.) wmin = 1.1*wmin;
    else wmin = 0.9*wmin;

    padR->cd(2);
    sprintf(title,"Fitted Widths Run %s", runid);
    TH2D* fr2 = new TH2D("fr2",title, 10, -0.5, 71.5, 10, wmin, 1.1*wmax);
    fr2->SetStats(0);
    fr2->GetXaxis()->SetTitle("Strip Number");
    fr2->GetXaxis()->CenterTitle();
    fr2->GetYaxis()->SetTitle("Width");
    fr2->GetYaxis()->CenterTitle();

    fr2->Draw();

    TGraphErrors* gr2 = new TGraphErrors(72, strip, gwidth, stripE, gwidthE);
    gr2->SetMarkerStyle(20);
    gr2->SetMarkerSize(0.5);
    gr2->SetLineWidth(1.0);
    gr2->SetLineColor(4);
    gr2->SetMarkerColor(4);
    gr2->Draw("P");

// Ratios of the fits (3)...
    Float_t rmin = 0.;
    Float_t rmax = 0.;

    for (int i=0; i<72; i++) {
     if (peak[i] > 0.) {
      ratio[i] = gwidth[i]/peak[i];
      ratioE[i] = ratio[i]*sqrt(pow((gwidthE[i]/gwidth[i]),2)+pow((peakE[i]/peak[i]),2));

      if (ratio[i]-ratioE[i] < rmin) rmin = ratio[i]-ratioE[i];
      if (ratio[i]+ratioE[i] > rmax) rmax = ratio[i]+ratioE[i];
      if (ratio[i] > 0) rdis->Fill(ratio[i]);
     }
    }
    padL->cd(3);
    rdis->Draw();

    if (rmin < 0.) rmin = 1.1*rmin;
    else rmin = 0.9*rmin;

    padR->cd(3);
    sprintf(title,"Width/Peak Run %s",runid);
    TH2D* fr3 = new TH2D("fr3",title, 10, -0.5, 71.5, 10, rmin, 1.1*rmax);
    fr3->SetStats(0);
    fr3->GetXaxis()->SetTitle("Strip Number");
    fr3->GetXaxis()->CenterTitle();
    fr3->GetYaxis()->SetTitle("Width/Peak");
    fr3->GetYaxis()->CenterTitle();

    fr3->Draw();

    TGraphErrors* gr3 = new TGraphErrors(72, strip, ratio, stripE, ratioE);
    gr3->SetMarkerStyle(20);
    gr3->SetMarkerSize(0.5);
    gr3->SetLineWidth(1.0);
    gr3->SetLineColor(4);
    gr3->SetMarkerColor(4);
    gr3->Draw("P");

    CurC2->Update();

    return;
}
