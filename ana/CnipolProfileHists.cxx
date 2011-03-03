/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include <algorithm>

#include "CnipolProfileHists.h"

#include "AsymGlobals.h"


ClassImp(CnipolProfileHists)

using namespace std;

/** Default constructor. */
CnipolProfileHists::CnipolProfileHists() : DrawObjContainer()
{
   BookHists();
}


CnipolProfileHists::CnipolProfileHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
}


/** Default destructor. */
CnipolProfileHists::~CnipolProfileHists()
{
}


/** */
void CnipolProfileHists::BookHists(string sid)
{ //{{{
   fDir->cd();

   if (!sid.empty()) sid = "_" + sid;

   char hName[256];

   sprintf(hName, "hProfile");
   o[hName] = new TH1D(hName, hName, 1, 0, 1); // The number of steps will be taken from data
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Target Steps");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Events");
   //((TH1*) o[hName])->SetBit(TH1::kCanRebin);
   ((TH1*) o[hName])->Sumw2();

   sprintf(hName, "hProfileFwd");
   o[hName] = new TH1D(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Target Steps");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Events");

   sprintf(hName, "hProfileBck");
   o[hName] = new TH1D(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Target Steps");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Events");

   sprintf(hName, "hProfileFold");
   o[hName] = new TH1D(hName, hName, 1, 0, 1); // The number of steps will be taken from data
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Target Steps");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Events");

   sprintf(hName, "hPolProfile");
   o[hName] = new TH1D(hName, hName, 1, 0, 1); // The number of delimeters is not known beforehand
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Target Steps");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Polarization");
   ((TH1*) o[hName])->SetBit(TH1::kCanRebin);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 1.05);

   sprintf(hName, "hPolProfileFwd");
   o[hName] = new TH1D(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Target Steps");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Polarization");
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 1.05);

   sprintf(hName, "hPolProfileBck");
   o[hName] = new TH1D(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Target Steps");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Polarization");
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 1.05);

   sprintf(hName, "hPolProfileFold");
   o[hName] = new TH1D(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Target Steps");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Polarization");
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 1.05);

   sprintf(hName, "hPolProfileFinal");
   //o[hName] = new TH1D(hName, hName, 1, -2.5, 2.5);
   o[hName] = new TH1D(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Distance, mm");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Polarization");
   //((TH1*) o[hName])->SetAxisRange(0, 1, "Y");
   //((TH1*) o[hName])->GetYaxis()->SetLimits(0, 1);

   sprintf(hName, "hPolVsIntensProfile");
   o[hName] = new TH1F(hName, hName, 1, -1, 1);
   //((TH1*) o[hName])->GetXaxis()->SetTitle("#frac{I - I_{max}}{I_{max}}");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Polarization");

} //}}}


/** */
void CnipolProfileHists::Fill(UInt_t n, Long_t* hData)
{ //{{{

   Double_t *hd    = new Double_t[n+3](); // 2+1 seems one bin is lost compared to PC_TARGET histograms
   Double_t *hdErr = new Double_t[n+3](); // 2+1 seems one bin is lost compared to PC_TARGET histograms

   //copy(hData, hData+1, hd);
   Long_t   *hPtr     = hData;
   Double_t *hdPtr    = hd;
   Double_t *hdErrPtr = hdErr;

   hdPtr    += 2; // offset by 2: 1 underflow bin and 1 lost one
   hdErrPtr += 2; // offset by 2: 1 underflow bin and 1 lost one

   while (hPtr != hData+n) { *hdPtr++ = *hPtr++; *hdErrPtr++ = sqrt(*hPtr); }

   for (int i=0; i<n+3; i++) {
      printf("i: %d, %d, %f\n", i, *(hData+i), *(hd+i));
   }

   ((TH1*) o["hProfile"])->SetBins(n+1, 0, n+1);
   ((TH1*) o["hProfile"])->SetContent(hd);
   ((TH1*) o["hProfile"])->SetError(hdErr);
   //((TH1*) o["hProfile"])->Sumw2();

   delete [] hd;
} //}}}


/** */
void CnipolProfileHists::PostFill()
{
   //((TH1*) o["hProfile"])->SetBins(nTgtIndex, 0, nTgtIndex);
   //((TH1*) o["hPolProfile"])->SetBins(nTgtIndex, 0, nTgtIndex);
   ((TH1*) o["hPolProfile"])->SetBins(ndelim, 0, ndelim);
}


/** */
void CnipolProfileHists::Process()
{ //{{{
   TH1* hProfile = (TH1*) o["hProfile"];

   Double_t ymax = hProfile->GetMaximum();

   //hProfile->Scale(1./hProfile->Integral());
   hProfile->Scale(1./ymax);
   Double_t xmin = hProfile->GetXaxis()->GetXmin();
   Double_t xmax = hProfile->GetXaxis()->GetXmax();
   //Double_t ymin = hProfile->GetYaxis()->GetXmin();
   //Double_t ymax = hProfile->GetYaxis()->GetXmax();
   ymax = hProfile->GetMaximum();

   TF1 *fitFunc = new TF1("ProfileFitFunc", CnipolProfileHists::ProfileFitFunc, xmin, xmax, 4);

   fitFunc->SetParameters(5, (xmax-xmin)*0.33, (xmax-xmin)*0.66, 1e6);
   fitFunc->SetParLimits(0, 0.5, 10);
   fitFunc->SetParLimits(1, (xmax-xmin)*0.1, (xmax-xmin)*0.4);
   fitFunc->SetParLimits(2, (xmax-xmin)*0.6, xmax*0.9);
   fitFunc->SetParLimits(3, ymax*0.5, ymax*1.5);

   TFitResultPtr fitres = hProfile->Fit(fitFunc, "I M LL S R", "");

   // Now split the hProfile hist into two: forward and backward motionsward motions

   double chi2Ndf;
   double sigma; 
   double mean1;
   double mean1Err;
   double mean2;
   double mean2Err;

   if (fitres.Get()) {
      chi2Ndf  = fitres->Ndf() > 0 ? fitres->Chi2()/fitres->Ndf() : -1;
      sigma    = fitres->Value(0);
      mean1    = fitres->Value(1);
      mean1Err = fitres->FitResult::Error(1);
      mean2    = fitres->Value(2);
      mean2Err = fitres->FitResult::Error(2);
   }
   
   TH1* hProfileFwd      = (TH1*) o["hProfileFwd"];
   TH1* hProfileBck      = (TH1*) o["hProfileBck"];
   TH1* hProfileFold     = (TH1*) o["hProfileFold"];
   TH1* hPolProfileFwd   = (TH1*) o["hPolProfileFwd"];
   TH1* hPolProfileBck   = (TH1*) o["hPolProfileBck"];
   TH1* hPolProfileFold  = (TH1*) o["hPolProfileFold"];
   TH1* hPolProfileFinal = (TH1*) o["hPolProfileFinal"];

   TH1* hPolProfile = (TH1*) o["hPolProfile"];

   Int_t bc  = hProfile->FindBin( mean1 + (mean2 - mean1)*0.5);

   Int_t bmean1 = hProfile->FindBin(mean1);
   Int_t bmean2 = hProfile->FindBin(mean2);
   Int_t bbegin = hProfile->FindBin(mean1-3*sigma);
   Int_t bend   = hProfile->FindBin(mean1+3*sigma);

   bbegin = bbegin >= 1 ? bbegin : 1;
   bend   = bend <= bc  ? bend : bc;

   printf("bc, bmean1, bbegin, bend 1: %d, %d, %d, %d\n", bc, bmean1, bbegin, bend);

   hProfileFwd     ->SetBins(bend-bbegin+1, 0, bend-bbegin+1);
   hProfileBck     ->SetBins(bend-bbegin+1, 0, bend-bbegin+1);
   hProfileFold    ->SetBins(bend-bbegin+1, 0, bend-bbegin+1);
   hPolProfileFwd  ->SetBins(bend-bbegin+1, 0, bend-bbegin+1);
   hPolProfileBck  ->SetBins(bend-bbegin+1, 0, bend-bbegin+1);
   hPolProfileFold ->SetBins(bend-bbegin+1, 0, bend-bbegin+1);
   //hPolProfileFinal->SetBins(bend-bbegin+1, 0, bend-bbegin+1);

   hPolProfileFwd  ->SetBit(TH1::kIsAverage);
   hPolProfileBck  ->SetBit(TH1::kIsAverage);
   hPolProfileFold ->SetBit(TH1::kIsAverage);
   //hPolProfileFinal->SetBit(TH1::kIsAverage);

   hPolProfileFwd  ->Sumw2();
   hPolProfileBck  ->Sumw2();
   hPolProfileFold ->Sumw2();
   //hPolProfileFinal->Sumw2();

   for (int i=bbegin, j=1; i<=bend; i++, j++) {
      hProfileFwd ->SetBinContent(j, hProfile->GetBinContent(i));
      hProfileBck ->SetBinContent(j, hProfile->GetBinContent(i + bmean2 - bmean1));
      //hProfileFold->SetBinContent(j, hProfile->GetBinContent(i) + hProfile->GetBinContent(i + bmean2 - bmean1));

      hPolProfileFwd ->SetBinContent(j, hPolProfile->GetBinContent(i));
      hPolProfileFwd ->SetBinError  (j, hPolProfile->GetBinError(i));
      hPolProfileBck ->SetBinContent(j, hPolProfile->GetBinContent(i + bmean2 - bmean1));
      hPolProfileBck ->SetBinError  (j, hPolProfile->GetBinError(i + bmean2 - bmean1));
      //hPolProfileFold->SetBinContent(j, hPolProfile->GetBinContent(i) + hPolProfile->GetBinContent(i + bmean2 - bmean1));
   }

   // Add forward and backward motions
   hPolProfileFold->Add(hPolProfileFwd, hPolProfileBck);
   hPolProfileFold->GetYaxis()->SetRangeUser(0, 1.05);

   //hPolProfileFinal->Add(hPolProfileFwd, hPolProfileBck);

   //char label[5];
   Float_t maxDistance = FLT_MIN, minDistance = FLT_MAX;
   Float_t maxPol = FLT_MIN;

   TGraphErrors *grPolProfileFinal = new TGraphErrors();
   grPolProfileFinal->SetName("grPolProfileFinal");

   printf("\n%5s %5s %5s\n", "dist", "pol", "err");

   for (int i=bbegin, j=0; i<=bend; i++, j++) {

      Double_t pol = hPolProfileFold->GetBinContent(j+1);
      Double_t polErr = hPolProfileFold->GetBinError(j+1);
      Float_t distance = (tgt.all.x[i] - tgt.all.x[bmean1]) / 730.;

      if (distance > maxDistance) maxDistance = distance;
      if (distance < minDistance) minDistance = distance;
      if (pol > maxPol) maxPol = pol;

      printf("%5.2f %5.2f %5.2f\n", distance, pol, polErr);

      grPolProfileFinal->SetPoint(j, distance, pol);
      grPolProfileFinal->SetPointError(j, 0, polErr);

      //sprintf(label, "%+5.2f", distance);
      //hPolProfileFinal->GetXaxis()->SetBinLabel(j, label);
   }

   printf("minmax: %5.2f %5.2f %5.2f\n", maxPol, minDistance, maxDistance);

   grPolProfileFinal->Fit("gaus", "M E");
   hPolProfileFinal->GetListOfFunctions()->Add(grPolProfileFinal, "p");
   hPolProfileFinal->GetXaxis()->SetLimits(minDistance*1.1, maxDistance*1.1);
   hPolProfileFinal->GetYaxis()->SetLimits(0, maxPol*1.1);
   //hPolProfileFinal->SetAxisRange(minDistance*1.1, maxDistance*1.1, "X");
   //hPolProfileFinal->SetAxisRange(0, maxPol*1.1, "Y");


   // Create graph polarization vs intensity
   TGraphErrors *grPolVsIntensProfile = new TGraphErrors();
   grPolVsIntensProfile->SetName("grPolVsIntensProfile");
   grPolVsIntensProfile->SetMarkerStyle(kFullDotLarge);
   grPolVsIntensProfile->SetMarkerSize(1);
   grPolVsIntensProfile->SetMarkerColor(kRed);

   for (int i=1; i<=hProfile->GetNbinsX(); i++) {
      float intens    = hProfile->GetBinContent(i);
      float intensErr = hProfile->GetBinError(i);
      float polar     = hPolProfile->GetBinContent(i);
      float polarErr  = hPolProfile->GetBinError(i);

      Int_t bmean = (i < bc ? bmean1 : bmean2);

      intens = (i <= bmean ? intens - 1 : 1 - intens);

      printf("%5d %5d %5.2f %5.2f %5.2f\n", i, bmean, intens, polar, polarErr);

      grPolVsIntensProfile->SetPoint(i, intens, polar);
      grPolVsIntensProfile->SetPointError(i, intensErr, polarErr);
   }

   TH1* hPolVsIntensProfile = (TH1*) o["hPolVsIntensProfile"];
   hPolVsIntensProfile->GetListOfFunctions()->Add(grPolVsIntensProfile, "p");

} //}}}


/** */
Double_t CnipolProfileHists::ProfileFitFunc(Double_t *x, Double_t *par)
{ //{{{
   Double_t x0 = x[0];
   Double_t sigma = par[0];
   Double_t mean1 = par[1];
   Double_t mean2 = par[2];
   Double_t norm  = par[3];

   Double_t g1 = norm*TMath::Gaus(x0, mean1, sigma);
   Double_t g2 = norm*TMath::Gaus(x0, mean2, sigma);

   return g1 + g2;
} //}}}
