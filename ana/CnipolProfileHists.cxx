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
   ((TH1*) o[hName])->SetMarkerStyle(kFullDotLarge);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(kRed);

   sprintf(hName, "hPolProfileBck");
   o[hName] = new TH1D(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Target Steps");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Polarization");
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 1.05);
   ((TH1*) o[hName])->SetMarkerStyle(kFullDotLarge);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(kRed);

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
   ((TH1*) o[hName])->SetMarkerStyle(kFullDotLarge);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(kRed);

   sprintf(hName, "hPolVsIntensProfile");
   o[hName] = new TH1F(hName, hName, 10, -1, 1);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("(I - I_{max})/I_{max}");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Polarization");

   sprintf(hName, "hIntUniProfile");
   o[hName] = new TH1F(hName, hName, 1, -3, 3);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("X");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Intensity");

   sprintf(hName, "hIntUniProfileBin");
   o[hName] = new TH1F(hName, hName, 20, -3, 3);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("X");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Intensity");
   ((TH1*) o[hName])->SetOption("p");
   ((TH1*) o[hName])->SetMarkerStyle(kFullDotLarge);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(kRed);
   ((TH1*) o[hName])->Sumw2();

   //sprintf(hName, "grIntUniProfile");
   //o[hName] = new TGraphErrors();

   sprintf(hName, "hPolUniProfile");
   o[hName] = new TH1F(hName, hName, 1, -3, 3);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("X");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Polarization");
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 1.05);

   sprintf(hName, "hPolUniProfileBin");
   o[hName] = new TH1F(hName, hName, 20, -3, 3);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("X");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Polarization");
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 1.05);
   ((TH1*) o[hName])->SetOption("p");
   ((TH1*) o[hName])->SetMarkerStyle(kFullDotLarge);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(kRed);
   ((TH1*) o[hName])->Sumw2();

   //sprintf(hName, "grPolUniProfile");
   //o[hName] = new TGraphErrors();

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

   while (hPtr != hData+n) { *hdPtr++ = *hPtr++; *hdErrPtr++ = 1./TMath::Sqrt(*hPtr); }

   for (UInt_t i=0; i<n+3; i++) {
      printf("i: %d, %ld, %f\n", i, *(hData+i), *(hd+i));
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

   double chi2Ndf  = 0;
   double sigma    = 0;
   double mean1    = 0;
   double mean1Err = 0;
   double mean2    = 0;
   double mean2Err = 0;

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
   grPolProfileFinal->SetMarkerStyle(kFullDotLarge);
   grPolProfileFinal->SetMarkerSize(1);
   grPolProfileFinal->SetMarkerColor(kRed);

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

   TF1 *my_gaus1 = new TF1("my_gaus1", "[0]*TMath::Gaus(x, [1], [2], 0)", minDistance, maxDistance);

   my_gaus1->SetParameters(0.5, 0, 0.5);
   my_gaus1->SetParLimits(0, 0, 1);
   my_gaus1->SetParLimits(1, -2, 2);
   //my_gaus1->FixParameter(1, 0);
   my_gaus1->SetParLimits(2, 0.1, 10);

   grPolProfileFinal->Fit("my_gaus1", "M E R");

   hPolProfileFinal->GetListOfFunctions()->Add(grPolProfileFinal, "p");
   hPolProfileFinal->GetXaxis()->SetLimits(minDistance*1.1, maxDistance*1.1);
   hPolProfileFinal->GetYaxis()->SetLimits(0, maxPol*1.1);
   //hPolProfileFinal->SetAxisRange(minDistance*1.1, maxDistance*1.1, "X");
   //hPolProfileFinal->SetAxisRange(0, maxPol*1.1, "Y");

   //delete grPolProfileFinal;

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

   TF1 *my_gaus = new TF1("my_gaus", "[0]*TMath::Gaus(x, [1], [2], 0)", -0.8, 0.8);

   my_gaus->SetParameters(0.5, 0, 0.3);
   my_gaus->SetParLimits(0, 0, 1);
   //my_gaus->SetParLimits(1, -0.1, 0);
   my_gaus->FixParameter(1, 0);
   my_gaus->SetParLimits(2, 0.1, 100);

   grPolVsIntensProfile->Fit("my_gaus", "M E R");

   TH1* hPolVsIntensProfile = (TH1*) o["hPolVsIntensProfile"];
   hPolVsIntensProfile->GetListOfFunctions()->Add(grPolVsIntensProfile, "p");

   //TPaveStats *stats = (TPaveStats*) hPolVsIntensProfile->FindObject("stats");

   //if (stats) {
   //   stats->SetX1NDC(0.84);
   //   stats->SetX2NDC(0.99);
   //   stats->SetY1NDC(0.10);
   //   stats->SetY2NDC(0.50);
   //} else {
   //   printf("could not find stats\n");
   //}

   delete my_gaus;

   // Crazy new method

   TGraphErrors *grPolUniProfile = new TGraphErrors();
   //TGraphErrors *grPolUniProfile = (TGraphErrors*) o["grPolUniProfile"];
   grPolUniProfile->SetName("grPolUniProfile");
   grPolUniProfile->SetMarkerStyle(kFullDotLarge);
   grPolUniProfile->SetMarkerSize(1);
   grPolUniProfile->SetMarkerColor(kRed);

   TGraphErrors *grIntUniProfile = new TGraphErrors();
   //TGraphErrors *grIntUniProfile = (TGraphErrors*) o["grIntUniProfile"];
   grIntUniProfile->SetName("grIntUniProfile");
   grIntUniProfile->SetMarkerStyle(kFullDotLarge);
   grIntUniProfile->SetMarkerSize(1);
   grIntUniProfile->SetMarkerColor(kRed);

   for (int i=1; i<=hProfile->GetNbinsX(); i++) {
      float intens    = hProfile->GetBinContent(i);
      float intensErr = hProfile->GetBinError(i);
      float polar     = hPolProfile->GetBinContent(i);
      float polarErr  = hPolProfile->GetBinError(i);

      float xuni_p =    TMath::Sqrt(-2*TMath::Log(intens));
      float xuni_m = -1*TMath::Sqrt(-2*TMath::Log(intens));

      printf("%5d %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f\n", i, intens, intensErr, polar, polarErr, xuni_p, xuni_m);

      grIntUniProfile->SetPoint( (i-1)*2 + 0, xuni_p, intens);
      grIntUniProfile->SetPoint( (i-1)*2 + 1, xuni_m, intens);
      grIntUniProfile->SetPointError( (i-1)*2 + 0, 0, intensErr);
      grIntUniProfile->SetPointError( (i-1)*2 + 1, 0, intensErr);

      grPolUniProfile->SetPoint( (i-1)*2 + 0, xuni_p, polar);
      grPolUniProfile->SetPoint( (i-1)*2 + 1, xuni_m, polar);
      grPolUniProfile->SetPointError( (i-1)*2 + 0, 0, polarErr);
      grPolUniProfile->SetPointError( (i-1)*2 + 1, 0, polarErr);
   }

   TF1 *my_gaus2 = new TF1("my_gaus2", "[0]*TMath::Gaus(x, [1], [2], 0)", -3, 3);

   my_gaus2->SetParameters(0.5, 0, 0.3);
   my_gaus2->SetParLimits(0, 0, 1);
   //my_gaus2->SetParLimits(1, -0.1, 0);
   my_gaus2->FixParameter(1, 0);
   my_gaus2->SetParLimits(2, 0.1, 1000);

   grPolUniProfile->Fit("my_gaus2", "M E R");

   TH1* hPolUniProfile = (TH1*) o["hPolUniProfile"];
   hPolUniProfile->GetListOfFunctions()->Add(grPolUniProfile, "p");

   TF1 *my_gaus3 = new TF1("my_gaus3", "TMath::Gaus(x, 0, [0], 0)", -3, 3);

   my_gaus3->SetParameters(0.5, 0);
   my_gaus3->SetParLimits(0, 0.1, 10);

   grIntUniProfile->Fit("my_gaus3", "M E R");

   TH1* hIntUniProfile = (TH1*) o["hIntUniProfile"];
   hIntUniProfile->GetListOfFunctions()->Add(grIntUniProfile, "p");


   // Fill hist from graph
   TH1* hIntUniProfileBin = (TH1*) o["hIntUniProfileBin"];
   TH1* hPolUniProfileBin = (TH1*) o["hPolUniProfileBin"];

   Double_t x, xe, y, ye;

   for (int i=0; i<grIntUniProfile->GetN(); i++) {

      grIntUniProfile->GetPoint(i, x, y);

      if (fabs(x) > 3) continue;

      //xe = grIntUniProfile->GetErrorX(i);
      ye = grIntUniProfile->GetErrorY(i);

      Int_t    ib  = hIntUniProfileBin->FindBin(x);
      Double_t ibc = hIntUniProfileBin->GetBinContent(ib);
      Double_t ibe = hIntUniProfileBin->GetBinError(ib);

      Double_t w1 = 1., w2 = 1.;
      if (ye  > 0) w1 = 1./(ye*ye);
      if (ibe > 0) w2 = 1./(ibe*ibe);

      Double_t ibc_new = (w1*y + w2*ibc)/(w1 + w2);
      Double_t ibe_new = 1./TMath::Sqrt(w1 + w2);

      printf("i: %8d %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f\n", ib, ibc, ibe, x, y, ye, w1, w2, ibc_new, ibe_new);

      hIntUniProfileBin->SetBinContent(ib, ibc_new);
      hIntUniProfileBin->SetBinError(ib, ibe_new);

      // Polarization part
      grPolUniProfile->GetPoint(i, x, y);

      //xe = grPolUniProfile->GetErrorX(i);
      ye = grPolUniProfile->GetErrorY(i);

      ib  = hPolUniProfileBin->FindBin(x);
      ibc = hPolUniProfileBin->GetBinContent(ib);
      ibe = hPolUniProfileBin->GetBinError(ib);

      w1 = 1., w2 = 1.;
      if (ye  > 0) w1 = 1./(ye*ye);
      if (ibe > 0) w2 = 1./(ibe*ibe);

      ibc_new = (w1*y + w2*ibc)/(w1 + w2);
      ibe_new = 1./TMath::Sqrt(w1 + w2);

      printf("p: %8d %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f\n", ib, ibc, ibe, x, y, ye, w1, w2, ibc_new, ibe_new);

      hPolUniProfileBin->SetBinContent(ib, ibc_new);
      hPolUniProfileBin->SetBinError(ib, ibe_new);
   }

   TF1 *my_gaus4 = new TF1("my_gaus4", "[0]*TMath::Gaus(x, 0, [1], 0)", -3, 3);

   my_gaus4->SetParameter(1, 0.5);
   my_gaus4->SetParLimits(0, 0.5, 1.5);
   my_gaus4->SetParLimits(1, 0.1, 10);

   hPolUniProfileBin->Fit("my_gaus4", "M E R");

   TF1 *my_gaus5 = new TF1("my_gaus5", "[0]*TMath::Gaus(x, 0, [1], 0)", -3, 3);

   my_gaus5->SetParameters(0.5, 1);
   my_gaus5->SetParLimits(0, 0.0, 1.1);
   my_gaus5->SetParLimits(1, 0.1, 100);

   hIntUniProfileBin->Fit("my_gaus5", "M E R");

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
