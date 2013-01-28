
#include <string>

#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"
#include "TF2.h"
#include "TGraphErrors.h"
#include "TKey.h"
#include "TObject.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TROOT.h"

#include "utils/utils.h"


void animProfile()
{
   gROOT->SetMacroPath("./:~/rootmacros/:~/rootmacros/utils/:");
   gROOT->Macro("styles/style_slides.C");
   gROOT->ForceStyle();

   TFile* file = new TFile("~/16650.001.root", "READ");
   gDirectory->ls();
   file->cd("profile");
   gDirectory->ls();

   TKey* key;

   key = gDirectory->FindKey("hIntensProfile");
   TH1D* hIntensProfile = (TH1D*) key->ReadObj();
   hIntensProfile->Print();

   key = gDirectory->FindKey("hPolarProfile");
   TH1D* hPolarProfile = (TH1D*) key->ReadObj();
   hPolarProfile->Print();

   TH1D* hPolarProfileReBinned = (TH1D*) hPolarProfile->Clone("hPolarProfileReBinned");

   hPolarProfileReBinned->Reset();
   hPolarProfileReBinned->Rebin(20);
   hPolarProfileReBinned->SetTitle("; Time; Polarization");
   hPolarProfileReBinned->SetMaximum(1.1*hPolarProfile->GetMaximum());
   hPolarProfileReBinned->SetMinimum(0);
   //hPolarProfileReBinned->Print("all");

   TGraphErrors* grPolarProfile = new TGraphErrors(hPolarProfile);
   //grPolarProfile->Print();

   utils::BinGraph(grPolarProfile, hPolarProfileReBinned);
   //hPolarProfileReBinned->Print("all");

   //return;

   // Get the fit parameters
   TF1* fitFunc = (TF1*) hIntensProfile->GetListOfFunctions()->FindObject("ProfileFitFunc");

   Double_t startT = hIntensProfile->GetXaxis()->GetXmin();
   Double_t stopT  = hIntensProfile->GetXaxis()->GetXmax();
   Double_t sigma  = fitFunc->GetParameter(0);
   Double_t mu1    = fitFunc->GetParameter(1);
   //Double_t mu2    = fitFunc->GetParameter(2);
   //Double_t turn   = mu1+0.5*(mu2-mu1);    // more realistic
   Double_t turn   = 0.5*(stopT - startT); // looks better

   // Find the corresponding bins
   //Int_t binSigma = hIntensProfile->FindBin(sigma);
   //Int_t binMu1   = hIntensProfile->FindBin(mu1);
   //Int_t binMu2   = hIntensProfile->FindBin(mu2);
   //Int_t binTurn  = hIntensProfile->FindBin(turn);

   TF2 *gaus2d = new TF2("gaus2d", "[0]*TMath::Gaus(x,[1],[2])*TMath::Gaus(y,[3],[4])",0,1,0,1);
   gaus2d->SetParameters(1, mu1/turn, sigma/turn, 0.5, sigma/turn);

   // Create frames
   //TH2I* hProfileFrame = new TH2I("hProfileFrame", "hProfileFrame", binTurn, 0, 1, binTurn, 0, 1);
   TH2I* hProfileFrame = new TH2I("hProfileFrame", "hProfileFrame", 50, 0, 1, 50, 0, 1);
   hProfileFrame->FillRandom("gaus2d", 20000);
   hProfileFrame->SetNdivisions(505, "XY");

   TH1D* hIntensProfileFrame = (TH1D*) hIntensProfile->Clone("hIntensProfileFrame");
   hIntensProfileFrame->Reset();
   hIntensProfileFrame->SetTitle("; Time; Intensity");
   hIntensProfileFrame->SetMaximum(1.1*hIntensProfile->GetMaximum());
   //hIntensProfileFrame->SetNdivisions(505, "XY");

   TH1D* hPolarProfileFrame = (TH1D*) hPolarProfileReBinned->Clone("hPolarProfileFrame");
   hPolarProfileFrame->Reset();
   //hPolarProfileFrame->SetTitle("; Time; Intensity");
   //hPolarProfileFrame->SetMaximum(1.1*hIntensProfile->GetMaximum());
   //hPolarProfileFrame->SetNdivisions(505, "XY");


   char tmpCharStr[8];
   string sPath = "/eic/u/dsmirnov/animation";
   string sFrameName;

   TCanvas* canvasProfile = new TCanvas("canvasProfile", "canvasProfile", 600, 600);
   canvasProfile->UseCurrentStyle();

   TCanvas* canvasIntens = new TCanvas("canvasIntens", "canvasIntens", 800, 600);
   canvasIntens->UseCurrentStyle();

   TCanvas* canvasPolar = new TCanvas("canvasPolar", "canvasPolar", 800, 600);
   canvasPolar->UseCurrentStyle();

   for(Int_t iBin=1, iFrame=0; iBin<=hIntensProfile->GetNbinsX(); iBin+=5, iFrame++)
   {
      Double_t cnt   = hIntensProfile->GetBinContent(iBin);
      Double_t err   = hIntensProfile->GetBinError(iBin);
      Double_t bcntr = hIntensProfile->GetBinCenter(iBin);
      //printf("%f +- %f\n", cnt, err);

      hIntensProfileFrame->SetBinContent(iBin, cnt);
      hIntensProfileFrame->SetBinError(iBin, err);
      hIntensProfileFrame->SetEntries(hIntensProfileFrame->GetEntries() + cnt);

      // Intensity
      TLine target(bcntr, 0, bcntr, hIntensProfileFrame->GetYaxis()->GetXmax());
      target.SetLineColor(kBlack);
      target.SetLineWidth(3);

      canvasIntens->cd();
      hIntensProfileFrame->Draw();
      target.Draw();

      sprintf(tmpCharStr, "%04d", iFrame);
      sFrameName = sPath + "/intens_" + tmpCharStr + ".gif";
      //cout << sFrameName << endl;
      canvasIntens->SaveAs(sFrameName.c_str());

      // Polarization
      //TLine target(bcntr, 0, bcntr, hIntensProfileFrame->GetYaxis()->GetXmax());
      //target.SetLineColor(kBlack);
      //target.SetLineWidth(3);
      Int_t    iBinPolar  = hPolarProfileReBinned->FindBin(bcntr);
      Double_t cntPolar   = hPolarProfileReBinned->GetBinContent(iBinPolar);
      Double_t errPolar   = hPolarProfileReBinned->GetBinError(iBinPolar);
      //Double_t bcntrPolar = hPolarProfileReBinned->GetBinCenter(iBinPolar);
      //printf("%f +- %f\n", cnt, err);

      hPolarProfileFrame->SetBinContent(iBinPolar, cntPolar);
      hPolarProfileFrame->SetBinError(iBinPolar, errPolar);
      hPolarProfileFrame->SetEntries(hPolarProfileFrame->GetEntries() + cntPolar);

      canvasPolar->cd();
      hPolarProfileFrame->Draw();
      target.Draw();

      sprintf(tmpCharStr, "%04d", iFrame);
      sFrameName = sPath + "/polar_" + tmpCharStr + ".gif";
      //cout << sFrameName << endl;
      canvasPolar->SaveAs(sFrameName.c_str());

      // Now draw transverse view
      Float_t targetX = bcntr <= turn ? bcntr/turn : 1 - (bcntr-turn)/turn;
      TLine target2(targetX, 0, targetX, 1);
      target2.SetLineColor(kBlack);
      target2.SetLineWidth(5);

      canvasProfile->cd();
      hProfileFrame->Draw("col");
      target2.Draw();

      sFrameName = sPath + "/profile_" + tmpCharStr + ".gif";
      canvasProfile->SaveAs(sFrameName.c_str());
   }
}
