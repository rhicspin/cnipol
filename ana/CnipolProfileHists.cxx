/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include <algorithm>

#include "CnipolProfileHists.h"

#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TGraphErrors.h"

#include "utils/utils.h"

#include "AsymGlobals.h"
#include "AnaResult.h"
#include "TargetInfo.h"


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

   sprintf(hName, "hIntensProfile");
   o[hName] = new TH1D(hName, hName, 1, 0, 1); // The number of steps will be taken from data
   ((TH1*) o[hName])->GetXaxis()->SetTitle("time, s");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Events");
   //((TH1*) o[hName])->SetBit(TH1::kCanRebin);
   ((TH1*) o[hName])->Sumw2();

   sprintf(hName, "hIntensProfile2");
   //o[hName] = new TH1D(hName, hName, 400, 0, 400);
   o[hName] = new TH1D(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->SetTitle(";time, s;Events");
   ((TH1*) o[hName])->SetLineColor(kRed);
   ((TH1*) o[hName])->Sumw2();

   sprintf(hName, "hIntensProfileFwd");
   o[hName] = new TH1D(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Target Steps");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Events");

   sprintf(hName, "hIntensProfileBck");
   o[hName] = new TH1D(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Target Steps");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Events");

   sprintf(hName, "hIntensProfileFold");
   o[hName] = new TH1D(hName, hName, 1, 0, 1); // The number of steps will be taken from data
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Target Steps");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Events");

   sprintf(hName, "hPolarProfile");
   o[hName] = new TH1D(hName, hName, 1, 0, 1); // The number of delimeters is not known beforehand
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Target Steps");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Polarization");
   ((TH1*) o[hName])->SetBit(TH1::kCanRebin);
   //((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 1.05);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(-1.05, 1.05);

   sprintf(hName, "hPolarProfileFwd");
   o[hName] = new TH1D(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Target Steps");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Polarization");
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 1.05);
   ((TH1*) o[hName])->SetMarkerStyle(kFullDotLarge);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(kRed);

   sprintf(hName, "hPolarProfileBck");
   o[hName] = new TH1D(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Target Steps");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Polarization");
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 1.05);
   ((TH1*) o[hName])->SetMarkerStyle(kFullDotLarge);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(kRed);

   sprintf(hName, "hPolarProfileFold");
   o[hName] = new TH1D(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Target Steps");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Polarization");
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 1.05);

   sprintf(hName, "hPolarProfileFinal");
   //o[hName] = new TH1D(hName, hName, 1, -2.5, 2.5);
   o[hName] = new TH1D(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Distance, mm");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Polarization");
   //((TH1*) o[hName])->SetAxisRange(0, 1, "Y");
   //((TH1*) o[hName])->GetYaxis()->SetLimits(0, 1);
   ((TH1*) o[hName])->SetMarkerStyle(kFullDotLarge);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(kRed);

   sprintf(hName, "hPolarVsIntensProfile");
   o[hName] = new TH1F(hName, hName, 10, 0, 1.1);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Relative Intensity I/I_{max}");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Polarization");
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 1.05);

   sprintf(hName, "hPolarVsIntensProfileBin");
   o[hName] = new TH1F(hName, hName, 22, 0, 1.1);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Relative Intensity I/I_{max}");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Polarization");
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 1.05);
   ((TH1*) o[hName])->SetOption("p");
   ((TH1*) o[hName])->SetMarkerStyle(kFullDotLarge);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(kBlue);
   ((TH1*) o[hName])->Sumw2();

   sprintf(hName, "hIntensUniProfile");
   o[hName] = new TH1F(hName, hName, 1, -3, 3);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("X");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Intensity");

   sprintf(hName, "hIntensUniProfileBin");
   o[hName] = new TH1F(hName, hName, 20, -3, 3);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("X");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Intensity");
   ((TH1*) o[hName])->SetOption("p");
   ((TH1*) o[hName])->SetMarkerStyle(kFullDotLarge);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(kBlue);
   ((TH1*) o[hName])->Sumw2();

   //sprintf(hName, "grIntensUniProfile");
   //o[hName] = new TGraphErrors();

   sprintf(hName, "hPolarUniProfile");
   o[hName] = new TH1F(hName, hName, 1, -5, 5);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("X");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Polarization");
   //((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 1.05);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(-1.05, 1.05);

   sprintf(hName, "hPolarUniProfileBin");
   o[hName] = new TH1F(hName, hName, 20, -3, 3);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("X");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Polarization");
   //((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 1.05);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(-1.05, 1.05);
   ((TH1*) o[hName])->SetOption("p");
   ((TH1*) o[hName])->SetMarkerStyle(kFullDotLarge);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(kBlue);
   ((TH1*) o[hName])->Sumw2();

   //sprintf(hName, "grPolarUniProfile");
   //o[hName] = new TGraphErrors();

} //}}}


/** */
void CnipolProfileHists::Fill(ChannelEvent *ch, string sid)
{ //{{{
   //UChar_t chId  = ch->GetChannelId();
   
   if (sid == "_cut2") { // fill these if only pass the carbon mass cut
      UInt_t ttime = ch->GetRevolutionId()/RHIC_REVOLUTION_FREQ;
      //printf("ttime: %d, %d\n", ttime, ch->GetRevolutionId());
      //((TH2F*) sd->o["hSpinVsDelim"+sid+"_ch"+sSi])->Fill(ch->GetDelimiterId(), gSpinPattern[bId]);
      //((TH2F*) sd->o["hSpinVsDelim"+sid+"_ch"+sSi])->Fill(ttime, gSpinPattern[bId]);
      ((TH1*) o["hIntensProfile2"])->Fill(ttime);
   }
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
   Double_t  nEntries = 0;
   //UInt_t    i = 0;

   hdPtr    += 2; // offset by 2: 1 underflow bin and 1 lost one
   hdErrPtr += 2; // offset by 2: 1 underflow bin and 1 lost one

   // Fill linear data and error arrays with proper dimensions, hd and hdErr
   while (hPtr != hData+n) {
      //printf("prof check i: %d, %d, %d\n", hPtr, hdPtr, hdErrPtr);
      *hdPtr++ = (Double_t) *hPtr++;
      *hdErrPtr++ = (*(hPtr-1) != 0 ? 1./TMath::Sqrt(*(hPtr-1)) : 0.);
      //cout << "prof check i: " << *(hPtr-1) << ", " << *(hdPtr-1) << ", " << *(hdErrPtr-1) << endl;
      //i++;
      nEntries += *(hPtr-1);
   }

   //for (UInt_t i=0; i<n+3; i++) {
   //   printf("prof check 2 i: %d, %ld, %f, %f\n", i, *(hData+i), *(hd+i), *(hdErr+i));
   //}

   ((TH1*) o["hIntensProfile"])->SetBins(n+1, 0, n+1);
   ((TH1*) o["hIntensProfile"])->SetContent(hd);
   ((TH1*) o["hIntensProfile"])->SetError(hdErr);
   //((TH1*) o["hIntensProfile"])->Sumw2();
   ((TH1*) o["hIntensProfile"])->SetEntries(nEntries);

   delete [] hd;
   delete [] hdErr;
} //}}}


/** */
void CnipolProfileHists::PreFill(string sid)
{ //{{{
   if (sid == "_cut2") {
      ((TH1*) o["hIntensProfile2"])->SetBins(gNDelimeters, 0, gNDelimeters);
   }
} //}}}


/** */
void CnipolProfileHists::PostFill()
{ //{{{
   //((TH1*) o["hIntensProfile"])->SetBins(nTgtIndex, 0, nTgtIndex);
   //((TH1*) o["hPolarProfile"])->SetBins(nTgtIndex, 0, nTgtIndex);
   ((TH1*) o["hPolarProfile"])->SetBins(gNDelimeters, 0, gNDelimeters);

   //Double_t ymax = ((TH1*) o["hIntensProfile2"])->GetMaximum();
   //((TH1*) o["hIntensProfile2"])->Scale(1./ymax);
} //}}}


/** */
void CnipolProfileHists::Process()
{ //{{{
   //TH1* hIntensProfile = (TH1*) o["hIntensProfile"];
   TH1* hIntensProfile = (TH1*) o["hIntensProfile2"];

   Double_t ymax = hIntensProfile->GetMaximum();

   //hIntensProfile->Scale(1./hIntensProfile->Integral());
   hIntensProfile->Scale(1./ymax);
   Double_t xmin = hIntensProfile->GetXaxis()->GetXmin();
   Double_t xmax = hIntensProfile->GetXaxis()->GetXmax();
   //Double_t ymin = hIntensProfile->GetYaxis()->GetXmin();
   //Double_t ymax = hIntensProfile->GetYaxis()->GetXmax();
   ymax = hIntensProfile->GetMaximum();

   TF1 *fitFunc = new TF1("ProfileFitFunc", CnipolProfileHists::ProfileFitFunc, xmin, xmax, 4);

   fitFunc->SetParNames("#sigma", "#mu_{1}", "#mu_{2}", "N_{scale}");
   fitFunc->SetParameters(5, (xmax-xmin)*0.33, (xmax-xmin)*0.66, 1e6);
   fitFunc->SetParLimits(0, 0.5, 10);
   fitFunc->SetParLimits(1, (xmax-xmin)*0.1, (xmax-xmin)*0.4);
   fitFunc->SetParLimits(2, (xmax-xmin)*0.6, xmax*0.9);
   fitFunc->SetParLimits(3, ymax*0.5, ymax*1.5);

   TFitResultPtr fitres = hIntensProfile->Fit(fitFunc, "I M S R", "");

   // Now split the hIntensProfile hist into two: forward and backward motionsward motions

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

   TH1* hIntensProfileFwd  = (TH1*) o["hIntensProfileFwd"];
   TH1* hIntensProfileBck  = (TH1*) o["hIntensProfileBck"];
   TH1* hIntensProfileFold = (TH1*) o["hIntensProfileFold"];
   TH1* hPolarProfileFwd   = (TH1*) o["hPolarProfileFwd"];
   TH1* hPolarProfileBck   = (TH1*) o["hPolarProfileBck"];
   TH1* hPolarProfileFold  = (TH1*) o["hPolarProfileFold"];
   TH1* hPolarProfileFinal = (TH1*) o["hPolarProfileFinal"];

   TH1* hPolarProfile = (TH1*) o["hPolarProfile"];

   Int_t bc  = hIntensProfile->FindBin( mean1 + (mean2 - mean1)*0.5);

   Int_t bmean1 = hIntensProfile->FindBin(mean1);
   Int_t bmean2 = hIntensProfile->FindBin(mean2);
   Int_t bbegin = hIntensProfile->FindBin(mean1-3*sigma);
   Int_t bend   = hIntensProfile->FindBin(mean1+3*sigma);

   bbegin = bbegin >= 1 ? bbegin : 1;
   bend   = bend <= bc  ? bend : bc;

   //printf("bc, bmean1, bbegin, bend 1: %d, %d, %d, %d\n", bc, bmean1, bbegin, bend);

   hIntensProfileFwd ->SetBins(bend-bbegin+1, 0, bend-bbegin+1);
   hIntensProfileBck ->SetBins(bend-bbegin+1, 0, bend-bbegin+1);
   hIntensProfileFold->SetBins(bend-bbegin+1, 0, bend-bbegin+1);
   hPolarProfileFwd  ->SetBins(bend-bbegin+1, 0, bend-bbegin+1);
   hPolarProfileBck  ->SetBins(bend-bbegin+1, 0, bend-bbegin+1);
   hPolarProfileFold ->SetBins(bend-bbegin+1, 0, bend-bbegin+1);
   //hPolarProfileFinal->SetBins(bend-bbegin+1, 0, bend-bbegin+1);

   hPolarProfileFwd ->SetBit(TH1::kIsAverage);
   hPolarProfileBck ->SetBit(TH1::kIsAverage);
   hPolarProfileFold->SetBit(TH1::kIsAverage);
   //hPolarProfileFinal->SetBit(TH1::kIsAverage);

   hPolarProfileFwd ->Sumw2();
   hPolarProfileBck ->Sumw2();
   hPolarProfileFold->Sumw2();
   //hPolarProfileFinal->Sumw2();

   for (int i=bbegin, j=1; i<=bend; i++, j++) {
      hIntensProfileFwd->SetBinContent(j, hIntensProfile->GetBinContent(i));
      hIntensProfileBck->SetBinContent(j, hIntensProfile->GetBinContent(i + bmean2 - bmean1));
      //hIntensProfileFold->SetBinContent(j, hIntensProfile->GetBinContent(i) + hIntensProfile->GetBinContent(i + bmean2 - bmean1));

      hPolarProfileFwd->SetBinContent(j, hPolarProfile->GetBinContent(i));
      hPolarProfileFwd->SetBinError  (j, hPolarProfile->GetBinError(i));
      hPolarProfileBck->SetBinContent(j, hPolarProfile->GetBinContent(i + bmean2 - bmean1));
      hPolarProfileBck->SetBinError  (j, hPolarProfile->GetBinError(i + bmean2 - bmean1));
      //hPolarProfileFold->SetBinContent(j, hPolarProfile->GetBinContent(i) + hPolarProfile->GetBinContent(i + bmean2 - bmean1));
   }

   // Add forward and backward motions
   hPolarProfileFold->Add(hPolarProfileFwd, hPolarProfileBck);
   hPolarProfileFold->GetYaxis()->SetRangeUser(0, 1.05);

   //hPolarProfileFinal->Add(hPolarProfileFwd, hPolarProfileBck);

   //char label[5];
   Float_t maxDistance = FLT_MIN, minDistance = FLT_MAX;
   Float_t maxPol = FLT_MIN;

   TGraphErrors *grPolProfileFinal = new TGraphErrors();
   grPolProfileFinal->SetName("grPolProfileFinal");
   grPolProfileFinal->SetMarkerStyle(kFullDotLarge);
   grPolProfileFinal->SetMarkerSize(1);
   grPolProfileFinal->SetMarkerColor(kRed);

   //printf("\n%5s %5s %5s\n", "dist", "pol", "err");

   for (int i=bbegin, j=0; i<=bend; i++, j++) {

      Double_t pol = hPolarProfileFold->GetBinContent(j+1);
      Double_t polErr = hPolarProfileFold->GetBinError(j+1);
      Float_t distance = (tgt.all.x[i] - tgt.all.x[bmean1]) / 730.;

      if (distance > maxDistance) maxDistance = distance;
      if (distance < minDistance) minDistance = distance;
      if (pol > maxPol) maxPol = pol;

      //printf("%5.2f %5.2f %5.2f\n", distance, pol, polErr);

      grPolProfileFinal->SetPoint(j, distance, pol);
      grPolProfileFinal->SetPointError(j, 0, polErr);

      //sprintf(label, "%+5.2f", distance);
      //hPolarProfileFinal->GetXaxis()->SetBinLabel(j, label);
   }

   //printf("minmax: %5.2f %5.2f %5.2f\n", maxPol, minDistance, maxDistance);

   TF1 *my_gaus1 = new TF1("my_gaus1", "[0]*TMath::Gaus(x, [1], [2], 0)", minDistance, maxDistance);

   my_gaus1->SetParNames("N_{scale}", "#mu", "#sigma");
   my_gaus1->SetParameters(0.5, 0, 0.5);
   my_gaus1->SetParLimits(0, 0, 1);
   my_gaus1->SetParLimits(1, -2, 2);
   //my_gaus1->FixParameter(1, 0);
   my_gaus1->SetParLimits(2, 0.1, 10);

   grPolProfileFinal->Fit("my_gaus1", "M E R");

   hPolarProfileFinal->GetListOfFunctions()->Add(grPolProfileFinal, "p");
   hPolarProfileFinal->GetXaxis()->SetLimits(minDistance*1.1, maxDistance*1.1);
   hPolarProfileFinal->GetYaxis()->SetLimits(0, maxPol*1.1);
   //hPolarProfileFinal->SetAxisRange(minDistance*1.1, maxDistance*1.1, "X");
   //hPolarProfileFinal->SetAxisRange(0, maxPol*1.1, "Y");

   //delete grPolProfileFinal;

   // Create graph polarization vs intensity
   TGraphErrors *grPolarVsIntensProfile = new TGraphErrors();
   grPolarVsIntensProfile->SetName("grPolarVsIntensProfile");
   grPolarVsIntensProfile->SetMarkerStyle(kFullDotLarge);
   grPolarVsIntensProfile->SetMarkerSize(1);
   grPolarVsIntensProfile->SetMarkerColor(kRed);

   for (int i=1; i<=hIntensProfile->GetNbinsX(); i++) {
      float intens    = hIntensProfile->GetBinContent(i);
      float intensErr = hIntensProfile->GetBinError(i);
      float polar     = hPolarProfile->GetBinContent(i);
      float polarErr  = hPolarProfile->GetBinError(i);

      //Int_t bmean = (i < bc ? bmean1 : bmean2);

      // In case we wish to make it symmetric
      //intens = (i <= bmean ? intens - 1 : 1 - intens);

      //printf("%5d %5d %5.2f %5.2f %5.2f\n", i, bmean, intens, polar, polarErr);

      grPolarVsIntensProfile->SetPoint(i, intens, polar);
      grPolarVsIntensProfile->SetPointError(i, intensErr, polarErr);
   }

   //TF1 *my_gaus = new TF1("my_gaus", "[0]*TMath::Gaus(x, [1], [2], 0)", -0.8, 0.8);
   TF1 *mfPow = new TF1("mfPow", "[0]*TMath::Power(x, [1])", 0.1, 1);

   mfPow->SetParNames("P_{max}", "r");
   mfPow->SetParameter(0, 0.5);
   mfPow->SetParLimits(0, 0, 1);
   mfPow->SetParameter(1, 0.1);
   mfPow->SetParLimits(1, -2, 2);

   grPolarVsIntensProfile->Fit("mfPow", "M E R");

   TH1* hPolarVsIntensProfile = (TH1*) o["hPolarVsIntensProfile"];
   hPolarVsIntensProfile->GetListOfFunctions()->Add(grPolarVsIntensProfile, "p");

   gAnaResult->fIntensPolarR    = mfPow->GetParameter(1);
   gAnaResult->fIntensPolarRErr = mfPow->GetParError(1);

   char sratio[50];

   sprintf(sratio, "% 8.3f, % 6.3f", gAnaResult->fIntensPolarR, gAnaResult->fIntensPolarRErr);

   gRunDb.fFields["PROFILE_RATIO"] = sratio;

   //TPaveStats *stats = (TPaveStats*) hPolarVsIntensProfile->FindObject("stats");

   //if (stats) {
   //   stats->SetX1NDC(0.84);
   //   stats->SetX2NDC(0.99);
   //   stats->SetY1NDC(0.10);
   //   stats->SetY2NDC(0.50);
   //} else {
   //   printf("could not find stats\n");
   //}

   //delete mfPow;

   // Crazy new method

   TGraphErrors *grPolarUniProfile = new TGraphErrors();
   //TGraphErrors *grPolarUniProfile = (TGraphErrors*) o["grPolarUniProfile"];
   grPolarUniProfile->SetName("grPolarUniProfile");
   grPolarUniProfile->SetMarkerStyle(kFullDotLarge);
   grPolarUniProfile->SetMarkerSize(1);
   grPolarUniProfile->SetMarkerColor(kRed);

   TGraphErrors *grIntensUniProfile = new TGraphErrors();
   //TGraphErrors *grIntensUniProfile = (TGraphErrors*) o["grIntensUniProfile"];
   grIntensUniProfile->SetName("grIntensUniProfile");
   grIntensUniProfile->SetMarkerStyle(kFullDotLarge);
   grIntensUniProfile->SetMarkerSize(1);
   grIntensUniProfile->SetMarkerColor(kRed);

   for (int i=1; i<=hIntensProfile->GetNbinsX(); i++) {
      float intens    = hIntensProfile->GetBinContent(i);
      float intensErr = hIntensProfile->GetBinError(i);
      float polar     = hPolarProfile->GetBinContent(i);
      float polarErr  = hPolarProfile->GetBinError(i);

      float xuni_p =    TMath::Sqrt(-2*TMath::Log(intens));
      float xuni_m = -1*TMath::Sqrt(-2*TMath::Log(intens));

      //printf("%5d %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f\n", i, intens, intensErr, polar, polarErr, xuni_p, xuni_m);

      grIntensUniProfile->SetPoint( (i-1)*2 + 0, xuni_p, intens);
      grIntensUniProfile->SetPoint( (i-1)*2 + 1, xuni_m, intens);
      grIntensUniProfile->SetPointError( (i-1)*2 + 0, 0, intensErr);
      grIntensUniProfile->SetPointError( (i-1)*2 + 1, 0, intensErr);

      grPolarUniProfile->SetPoint( (i-1)*2 + 0, xuni_p, polar);
      grPolarUniProfile->SetPoint( (i-1)*2 + 1, xuni_m, polar);
      grPolarUniProfile->SetPointError( (i-1)*2 + 0, 0, polarErr);
      grPolarUniProfile->SetPointError( (i-1)*2 + 1, 0, polarErr);
   }


   TF1 *my_gaus3 = new TF1("my_gaus3", "[0]*TMath::Gaus(x, 0, [1], 0)", -3, 3);

   my_gaus3->SetParNames("N_{scale}", "#sigma");
   my_gaus3->SetParameters(0, 0.5);
   my_gaus3->SetParameters(1, 0.3);
   my_gaus3->SetParLimits(0, 0, 1);
   my_gaus3->SetParLimits(1, 0.1, 2);

   grIntensUniProfile->Fit("my_gaus3", "M E R");

   TH1* hIntensUniProfile = (TH1*) o["hIntensUniProfile"];
   hIntensUniProfile->GetListOfFunctions()->Add(grIntensUniProfile, "p");


   TF1 *my_gaus2 = new TF1("my_gaus2", "[0]*TMath::Gaus(x, 0, [1], 0)", -3, 3);

   my_gaus2->SetParNames("N_{scale}", "#sigma");
   my_gaus2->SetParameter(0, 0.5);
   my_gaus2->SetParameter(1, 0.3);
   my_gaus2->SetParLimits(0, 0, 1);
   my_gaus2->SetParLimits(1, 0.1, 100);

   grPolarUniProfile->Fit("my_gaus2", "M E R");

   TH1* hPolarUniProfile = (TH1*) o["hPolarUniProfile"];
   hPolarUniProfile->GetListOfFunctions()->Add(grPolarUniProfile, "p");


   // Fill hist from graph
   TH1* hIntensUniProfileBin     = (TH1*) o["hIntensUniProfileBin"];
   TH1* hPolarUniProfileBin      = (TH1*) o["hPolarUniProfileBin"];
   TH1* hPolarVsIntensProfileBin = (TH1*) o["hPolarVsIntensProfileBin"];

   //Double_t x, xe, y, ye;
   Double_t x, y, ye;

   //printf("   %8s %8s %8s %8s %8s %8s %8s %8s %8s %8s\n", "ib", "ibc", "ibe", "x", "y", "ye", "w1", "w2", "ibc_new", "ibe_new");

   for (int i=0; i<grIntensUniProfile->GetN(); i++) {

      grIntensUniProfile->GetPoint(i, x, y);

      //if (fabs(x) > 3) continue;

      //xe = grIntensUniProfile->GetErrorX(i);
      ye = grIntensUniProfile->GetErrorY(i);

      Int_t    ib  = hIntensUniProfileBin->FindBin(x);
      Double_t ibc = hIntensUniProfileBin->GetBinContent(ib);
      Double_t ibe = hIntensUniProfileBin->GetBinError(ib);

      Double_t w1 = 1., w2 = 1.;
      if (ye  > 0) w1 = 1./(ye*ye);
      if (ibe > 0) w2 = 1./(ibe*ibe);

      Double_t ibc_new = (w1*y + w2*ibc)/(w1 + w2);
      Double_t ibe_new = 1./TMath::Sqrt(w1 + w2);

      //printf("i: %8d %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f\n", ib, ibc, ibe, x, y, ye, w1, w2, ibc_new, ibe_new);

      hIntensUniProfileBin->SetBinContent(ib, ibc_new);
      hIntensUniProfileBin->SetBinError(ib, ibe_new);


      // Polarization part
      grPolarUniProfile->GetPoint(i, x, y);

      //xe = grPolarUniProfile->GetErrorX(i);
      ye = grPolarUniProfile->GetErrorY(i);

      ib  = hPolarUniProfileBin->FindBin(x);
      ibc = hPolarUniProfileBin->GetBinContent(ib);
      ibe = hPolarUniProfileBin->GetBinError(ib);

      w1 = 1., w2 = 1.;
      if (ye  > 0) w1 = 1./(ye*ye);
      if (ibe > 0) w2 = 1./(ibe*ibe);

      ibc_new = (w1*y + w2*ibc)/(w1 + w2);
      ibe_new = 1./TMath::Sqrt(w1 + w2);

      //printf("p: %8d %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f\n", ib, ibc, ibe, x, y, ye, w1, w2, ibc_new, ibe_new);

      hPolarUniProfileBin->SetBinContent(ib, ibc_new);
      hPolarUniProfileBin->SetBinError(ib, ibe_new);


      // Polarization Vs Intens part
      // consider only even data points
      if (i%2 != 0) continue;

      grPolarVsIntensProfile->GetPoint(i/2, x, y);

      //xe = grPolarVsIntensProfile->GetErrorX(i);
      ye = grPolarVsIntensProfile->GetErrorY(i/2);

      ib  = hPolarVsIntensProfileBin->FindBin(x);
      ibc = hPolarVsIntensProfileBin->GetBinContent(ib);
      ibe = hPolarVsIntensProfileBin->GetBinError(ib);

      w1 = 1., w2 = 1.;
      if (ye  > 0) w1 = 1./(ye*ye);
      if (ibe > 0) w2 = 1./(ibe*ibe);

      ibc_new = (w1*y + w2*ibc)/(w1 + w2);
      ibe_new = 1./TMath::Sqrt(w1 + w2);

      //printf("pvi: %8d %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f\n", ib, ibc, ibe, x, y, ye, w1, w2, ibc_new, ibe_new);

      hPolarVsIntensProfileBin->SetBinContent(ib, ibc_new);
      hPolarVsIntensProfileBin->SetBinError(ib, ibe_new);
   }

   TF1 *my_gaus5 = new TF1("my_gaus5", "[0]*TMath::Gaus(x, 0, [1], 0)", -3, 3);

   my_gaus5->SetParNames("N_{scale}", "#sigma");
   my_gaus5->SetParameters(0, 0.5);
   my_gaus5->SetParameters(1, 1.0);
   my_gaus5->SetParLimits(0, 0.0, 1.1);
   my_gaus5->SetParLimits(1, 0.1, 100);

   hIntensUniProfileBin->Fit("my_gaus5", "M E R");
   delete my_gaus5;

   TF1 *my_gaus4 = new TF1("my_gaus4", "[0]*TMath::Gaus(x, 0, [1], 0)", -3, 3);

   my_gaus4->SetParNames("N_{scale}", "#sigma");
   my_gaus4->SetParameter(0, 0.5);
   my_gaus4->SetParameter(1, 1.0);
   my_gaus4->SetParLimits(0, 0.5, 1.5);
   my_gaus4->SetParLimits(1, 0.1, 100);

   hPolarUniProfileBin->Fit("my_gaus4", "M E R");
   delete my_gaus4;

   hPolarVsIntensProfileBin->Fit("mfPow", "M E R");

   delete mfPow;
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


/** */
EMeasType CnipolProfileHists::MeasurementType()
{ //{{{
   TH1* hIntensProfile = (TH1*) o["hIntensProfile"];

   if (!hIntensProfile) {
      Error("MeasurementType", "Histogram (hIntensProfile) not defined");
      return kMEASTYPE_UNKNOWN;
   }

   Double_t ymax = hIntensProfile->GetMaximum();

   if (ymax > 0) hIntensProfile->Scale(1./ymax);
   else
      Error("MeasurementType", "Empty histogram (hIntensProfile) ?");

   TH1F *hIntensProj = new TH1F("hIntensProj", "hIntensProj", 20, 0, 1);

   utils::ConvertToProfile(hIntensProfile, hIntensProj);

   Int_t nSteps = hIntensProfile->GetNbinsX();

   // start from the second bin (0.05 - 0.10 intensity) since we don't care
   // about small intensity steps
   for (Int_t i=2; i<=hIntensProj->GetNbinsX(); i++) {
      //if (nTotal)
      //printf("proj: %f, %d\n", hIntensProj->GetBinContent(i), nSteps);
      if (hIntensProj->GetBinContent(i) >= 0.5*nSteps)
         return kMEASTYPE_FIXED;
   }

   return kMEASTYPE_SWEEP;
} //}}}
