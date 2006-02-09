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

    ofstream fout;
    
    Char_t runid[50];

    Float_t peak[72], peakE[72];   
    Float_t acoef[72], acoefE[72];

};

// Initialization of the class
CalibFit::CalibFit(Char_t *runidinput){
    sprintf(runid,"%s",runidinput);
    memset(peak,0, sizeof(peak));
    memset(peakE,0, sizeof(peakE));
    memset(acoef,0, sizeof(acoef));
    memset(acoefE,0, sizeof(acoefE));
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



