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

gROOT->Reset();


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  MAIN
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class KinFit {
public:
    KinFit(Char_t *);
    ~KinFit();

    void Fit(Int_t);
    void FitOne(Int_t);
    
    void PlotResult();

    ofstream fout;
    
    Char_t runid[10];
    
    Float_t A0[72], A0E[72];
    Float_t A1[72], A1E[72];
};

// Initialization of the class
KinFit::KinFit(Char_t *runidinput){
    sprintf(runid,"%s",runidinput);

    memset(A0,0, sizeof(A0));
    memset(A0E,0, sizeof(A0E));

    memset(A1,0, sizeof(A1));
    memset(A1E,0, sizeof(A1E));
};
KinFit::~KinFit(){};

// -------------------------------------------------------------------
void KinFit::Fit() 
// -------------------------------------------------------------------
{    

    gStyle->SetGridColor(1);
    gStyle->SetGridStyle(2);
    //        gStyle->SetPalette(1,0);
    
    TCanvas *CurC = new TCanvas("CurC","",1);
    TPostScript ps("testfit.ps",112);
    
    CurC -> Divide(4,3);
    ps.NewPage();
    fout.open("testfit.dat");
    
    for (Int_t Si=0;Si<6;Si++) {
        
        Int_t Padn=0;
        
        for (Int_t St=Si*12; St<Si*12+12; St++) {
            Char_t hName[100];
            sprintf(hName,"h%d",12200+St+1);  
            
            TH2D* hAI = (TH2D*) gDirectory->Get(hName);
            
            Padn++;   // change pad even if the histograms is empty
            CurC->cd(Padn);
            
            if (hAI->GetEntries() > 100) {
                
                FitOne(St);
                
            }
        }
        
        CurC->Update();
        ps.NewPage();

    }

    fout.close();
    ps.Close();
};


// -------------------------------------------------------------------
//     Fit the given Strip
// -------------------------------------------------------------------
void KinFit::FitOne(Int_t St)
{

    Int_t Si = (Int_t)St/12;

    cout << " ===================================="<<endl;
    cout << " === Now starting - "<<St<<endl;
    cout << " ===================================="<<endl;

    Char_t hName[100];
    sprintf(hName,"h%d",12200+St+1);  
    
    TH2D* h2d = (TH2D*) gDirectory->Get(hName);
    h2d -> SetStats(0);
    sprintf(hName,"%s ST-%d",runid, St);
    h2d -> SetTitle(hName);
    h2d -> SetName("h2d");
    h2d->GetXaxis()->SetTitle("Pulse height");
    h2d->GetYaxis()->SetTitle("Integral");
    
    h2d->GetXaxis()->SetRangeUser(50., 200.);
    h2d->GetYaxis()->SetRangeUser(0., 3500.);
    
    // ======================================================
    // ====   The fit                                  ==
    // ======================================================

    // Fit slices for the 1st fit (auto fit routine with gaussian)
    
    TF1* myf = new TF1("myf","gaus",500.0,3000.0);

    h2d->FitSlicesY(myf);
    
    TH1F* sfit = (TH1F*) gDirectory->Get("h2d_1");

    h2d  -> Draw("color");

    // %%%%%%%%%%%%%%
    Float_t FitRangeLow = 70.;
    Float_t FitRangeUpp = 140.;

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    sfit -> Fit("pol1","E0","", FitRangeLow, FitRangeUpp);
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // putting in vectors

    TF1* fitf = (TF1*) sfit -> GetFunction("pol1");

    A0[St] = fitf->GetParameter(0);
    A1[St] = fitf->GetParameter(1);
    A0E[St] = fitf->GetParError(0);
    A1E[St] = fitf->GetParError(1);
    
    // Plot data points (Magenta)
    TH1D * Dpoints = (TH1D*) sfit->Clone();
    Dpoints -> SetLineColor(6);
    Dpoints -> SetLineWidth(1.2);
    Dpoints -> GetXaxis()-> SetRangeUser(FitRangeLow, FitRangeUpp);
    Dpoints -> Draw("same");


    // Plot fit function (Green)
    
    TF1* fitfc = (TF1*) fitf->Clone();
    fitfc -> SetName("fitf");
    fitfc -> SetLineColor(3);
    fitfc -> SetLineWidth(2.0);
    fitfc -> Draw("same");

    // TYPE the result on the Plots
    
    TText t; Char_t rtext[50];
    sprintf(rtext, "A0 = %6.1f +/- %6.1", A0[St], A0E[St]);
    t.DrawTextNDC(0.5, 0.8, rtext);
    sprintf(rtext, "A1 = %6.1f +/- %6.1", A1[St], A1E[St]);
    t.DrawTextNDC(0.5, 0.72, rtext);
    
    // Write Out the Result on File
    
    // ==== Force Default Numbers ==============
    //    A0[St] = -250.;
    //    A1[St] = 13.;
    
    fout << St<< " ";
    fout << setprecision(4) << A0[St]  <<" ";
    fout << setprecision(4) << A1[St] <<" ";
    fout << setprecision(4) << A0E[St]  <<" ";
    fout << setprecision(4) << A1E[St] <<" ";
    fout << setprecision(3) << fitfc->GetChisquare()/fitfc->GetNDF() <<" ";
    fout << fitfc->GetNDF()<<" ";
    fout << gMinuit->fCstatu.Data()<< endl;

    h2d_0 -> Delete();
    h2d_1 -> Delete();
    h2d_2 -> Delete();
    h2d_chi2 -> Delete();
    
    
}


// -------------------------------------------------------------------
//     Plot the result of the Fit 
// -------------------------------------------------------------------
void KinFit::PlotResult()
{

    TCanvas *CurC = new TCanvas("CurC","",1);
    TPostScript ps("testsummary.ps",112);
    
    CurC -> Divide(1,2);

    Float_t strip[72], stripE[72];
    for (Int_t St=0; St<72; St++) {
        strip[St] = (Float_t)St;
        stripE[St] = 0.;
    }

    // -------------
    // Plot - 1 (Dead layer distribution)
    // -------------

    CurC -> cd(1);
    
    Char_t title[40];
    sprintf(title, "(%s) A0 distribution", runid);
    TH2D* frame = new TH2D("frame", title, 10, -0.5, 71.5, 10, -400., 200.);
    frame -> SetStats(0);
    frame -> GetXaxis()->SetTitle("Strip Number");
    frame -> GetYaxis()->SetTitle("A0 (Integral)");
    frame -> Draw();

    // draw the separaters btw detectors
    for (Int_t isep=0; isep<6 ; isep++) {
        TLine *l = new TLine(12.*isep -0.5, -400., 12.*isep -0.5, 200.);
        l -> Draw();
    }

    TGraphErrors* tgA0 = new TGraphErrors(72, strip, A0, stripE, A0E);
    tgA0 -> SetMarkerStyle(20);
    tgA0 -> SetMarkerSize(0.5);
    tgA0 -> SetLineWidth(1.0);
    tgA0 -> SetLineColor(4);
    tgA0 -> SetMarkerColor(4);
    
    tgA0 -> Draw("P");
    
        
    // -------------
    // Plot - 2 (T0 distribution)
    // -------------
    
    CurC -> cd(2);
    
    Char_t title[40];
    sprintf(title, "(%s) A1 Distribution", runid);
    TH2D* frame = new TH2D("frame", title, 10, -0.5, 71.5, 10, 0., 30.);
    frame -> SetStats(0);
    frame -> GetXaxis()->SetTitle("Strip Number");
    frame -> GetYaxis()->SetTitle("T0 values (nsec)");
    frame -> Draw();

    // draw the separaters btw detectors
    for (Int_t isep=0; isep<6 ; isep++) {
        TLine *l = new TLine(12.*isep -0.5, 0., 12.*isep -0.5, 30.);
        l -> Draw();
    }
    
    // result of two paramter fit (blue)
    TGraphErrors* tgA1 = new TGraphErrors(72, strip, A1, stripE, A1E);
    tgA1 -> SetMarkerStyle(20);
    tgA1 -> SetMarkerSize(0.5);
    tgA1 -> SetLineWidth(1.0);
    tgA1-> SetLineColor(4);
    tgA1 -> SetMarkerColor(4);
    
    tgA1 -> Draw("P");
    
    ps.Close();


}



