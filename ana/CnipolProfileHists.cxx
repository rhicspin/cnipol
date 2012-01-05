/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include <algorithm>

#include "CnipolProfileHists.h"

#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TGraphErrors.h"
#include "TStyle.h"

#include "utils/utils.h"

#include "AsymGlobals.h"
#include "AnaMeasResult.h"
#include "TargetInfo.h"
#include "CnipolAsymHists.h"


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

   char        hName[256];
   string      shName;
   TH1        *hist;
   TAttMarker  styleMarker;

   styleMarker.SetMarkerStyle(kFullCircle);
   styleMarker.SetMarkerSize(1);
   styleMarker.SetMarkerColor(kGreen+2);

   // this one is filled from the scaler data
   sprintf(hName, "hIntensProfileScaler");
   o[hName] = new TH1D(hName, hName, 1, 0, 1); // The number of steps will be taken from data
   ((TH1*) o[hName])->SetTitle(";time, s;Events;");
   ((TH1*) o[hName])->SetOption("NOIMG");
   //((TH1*) o[hName])->SetBit(TH1::kCanRebin);
   ((TH1*) o[hName])->Sumw2();

   // this one is filled from the event data (it is a more correct way of doing
   // this than using hIntensProfileScaler)
   sprintf(hName, "hIntensProfile");
   o[hName] = new TH1D(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->SetTitle(";time, s;Events");
   //((TH1*) o[hName])->SetLineColor(kRed);
   ((TH1*) o[hName])->Sumw2();

   shName = "hIntensProfileFineBin";
   hist = new TH1D(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle(";time, s;Events");
   o[shName] = hist;

   sprintf(hName, "hIntensProfileFwd");
   o[hName] = new TH1D(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->SetTitle(";Target Steps, s;Events;");
   ((TH1*) o[hName])->SetOption("NOIMG");

   sprintf(hName, "hIntensProfileBck");
   o[hName] = new TH1D(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->SetTitle(";Target Steps, s;Events;");
   ((TH1*) o[hName])->SetOption("NOIMG");

   sprintf(hName, "hIntensProfileFold");
   o[hName] = new TH1D(hName, hName, 1, 0, 1); // The number of steps will be taken from data
   ((TH1*) o[hName])->SetTitle(";Target Steps, s;Events;");
   ((TH1*) o[hName])->SetOption("NOIMG");

   sprintf(hName, "hPolarProfile");
   o[hName] = new TH1D(hName, hName, 1, 0, 1); // The number of delimeters is not known beforehand
   ((TH1*) o[hName])->SetTitle(";Target Steps, s;Polarization;");
   ((TH1*) o[hName])->SetBit(TH1::kCanRebin);
   //((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 1.05);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(-1.05, 1.05);

   sprintf(hName, "hPolarProfileFwd");
   o[hName] = new TH1D(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->SetTitle(";Target Steps, s;Polarization;");
   ((TH1*) o[hName])->SetOption("NOIMG");
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 1.05);
   ((TH1*) o[hName])->SetMarkerStyle(kFullDotLarge);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(kRed);

   sprintf(hName, "hPolarProfileBck");
   o[hName] = new TH1D(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->SetTitle(";Target Steps, s;Polarization;");
   ((TH1*) o[hName])->SetOption("NOIMG");
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 1.05);
   ((TH1*) o[hName])->SetMarkerStyle(kFullDotLarge);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(kRed);

   sprintf(hName, "hPolarProfileFold");
   o[hName] = new TH1D(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->SetTitle(";Target Steps, s;Polarization;");
   ((TH1*) o[hName])->SetOption("NOIMG");
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 1.05);

   sprintf(hName, "hPolarProfileFinal");
   //o[hName] = new TH1D(hName, hName, 1, -2.5, 2.5);
   o[hName] = new TH1D(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->SetTitle(";Distance, mm;Polarization;");
   ((TH1*) o[hName])->SetOption("NOIMG");
   //((TH1*) o[hName])->SetAxisRange(0, 1, "Y");
   //((TH1*) o[hName])->GetYaxis()->SetLimits(0, 1);
   ((TH1*) o[hName])->SetMarkerStyle(kFullDotLarge);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(kRed);

   // asymmetry vs intensity
   TGraphErrors *grAsymVsIntensProfile = new TGraphErrors();
   grAsymVsIntensProfile->SetName("grAsymVsIntensProfile");
   styleMarker.Copy(*grAsymVsIntensProfile);

   //TGraphErrors *grAsymVsIntensProfileFineBin = new TGraphErrors();
   //grAsymVsIntensProfileFineBin->SetName("grAsymVsIntensProfileFineBin");
   //styleMarker.SetMarkerColor(kRed);
   //styleMarker.Copy(*grAsymVsIntensProfileFineBin);

   shName = "hAsymVsIntensProfile";
   hist = new TH2I(shName.c_str(), shName.c_str(), 100, 0, 1.1, 100, -0.1, 0.1);
   hist->SetTitle(";Relative Intensity I/I_{max};Asymmetry;");
   hist->GetListOfFunctions()->Add(grAsymVsIntensProfile, "p");
   //hist->GetListOfFunctions()->Add(grAsymVsIntensProfileFineBin, "p");
   o[shName] = hist;

   // this is a copy from the "asym" container
   shName = "hAsymVsDelim4Det";
   hist = new TH1D(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetOption("E1 NOIMG");
   hist->SetTitle(";Target Steps, s;Asymmetry;");
   o[shName] = hist;

   // polar vs intensity
   //TGraphErrors *grPolarVsIntensProfileFineBin = new TGraphErrors();
   //grPolarVsIntensProfileFineBin->SetName("grPolarVsIntensProfileFineBin");
   //grPolarVsIntensProfileFineBin->SetMarkerStyle(kFullDotLarge);
   //grPolarVsIntensProfileFineBin->SetMarkerSize(1);
   //grPolarVsIntensProfileFineBin->SetMarkerColor(kMagenta);

   shName = "hPolarVsIntensProfile";
   hist = new TH1F(shName.c_str(), shName.c_str(), 10, 0, 1.1);
   hist->SetTitle(";Relative Intensity I/I_{max};Polarization;");
   hist->GetYaxis()->SetRangeUser(0, 1.05);
   o[shName] = hist;
   //hist->GetListOfFunctions()->Add(grPolarVsIntensProfileFineBin, "p");

   sprintf(hName, "hPolarVsIntensProfileBin");
   o[hName] = new TH1F(hName, hName, 22, 0, 1.1);
   ((TH1*) o[hName])->SetTitle(";Relative Intensity I/I_{max};Polarization;");
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 1.05);
   ((TH1*) o[hName])->SetOption("p");
   ((TH1*) o[hName])->SetMarkerStyle(kFullDotLarge);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(kBlue);
   ((TH1*) o[hName])->Sumw2();

   // Intensity profile
   TGraphErrors *grIntensUniProfileFineBin = new TGraphErrors();
   grIntensUniProfileFineBin->SetName("grIntensUniProfileFineBin");
   styleMarker.Copy(*grIntensUniProfileFineBin);

   shName = "hIntensUniProfile";
   hist = new TH1F(shName.c_str(), shName.c_str(), 100, -5, 5);
   hist->SetTitle(";Sigma Units;Intensity;");
   hist->GetListOfFunctions()->Add(grIntensUniProfileFineBin, "p");
   o[shName] = hist;

   sprintf(hName, "hIntensUniProfileBin");
   o[hName] = new TH1F(hName, hName, 20, -5, 5);
   ((TH1*) o[hName])->SetTitle(";Sigma Units;Intensity;");
   ((TH1*) o[hName])->SetOption("p");
   ((TH1*) o[hName])->SetMarkerStyle(kFullDotLarge);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(kBlue);
   ((TH1*) o[hName])->Sumw2();

   //sprintf(hName, "grIntensUniProfile");
   //o[hName] = new TGraphErrors();

   // Polarization profile
   //TGraphErrors *grPolarUniProfileFineBin = new TGraphErrors();
   //grPolarUniProfileFineBin->SetName("grPolarUniProfileFineBin");
   //grPolarUniProfileFineBin->SetMarkerStyle(kFullDotLarge);
   //grPolarUniProfileFineBin->SetMarkerSize(1);
   //grPolarUniProfileFineBin->SetMarkerColor(kMagenta);

   shName = "hPolarUniProfile";
   hist = new TH1F(shName.c_str(), shName.c_str(), 100, -5, 5);
   hist->SetTitle(";Sigma Units;Polarization;");
   hist->GetYaxis()->SetRangeUser(-1.05, 1.05);
   //hist->GetListOfFunctions()->Add(grPolarUniProfileFineBin, "p");
   o[shName] = hist;

   sprintf(hName, "hPolarUniProfileBin");
   o[hName] = new TH1F(hName, hName, 20, -5, 5);
   ((TH1*) o[hName])->SetTitle(";Sigma Units;Polarization;");
   //((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 1.05);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(-1.05, 1.05);
   ((TH1*) o[hName])->SetOption("p");
   ((TH1*) o[hName])->SetMarkerStyle(kFullDotLarge);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(kBlue);
   ((TH1*) o[hName])->Sumw2();

   // Asymmetry profile
   TGraphErrors *grAsymUniProfileFineBin = new TGraphErrors();
   grAsymUniProfileFineBin->SetName("grAsymUniProfileFineBin");
   styleMarker.Copy(*grAsymUniProfileFineBin);

   shName = "hAsymUniProfile";
   hist = new TH1F(shName.c_str(), shName.c_str(), 100, -5, 5);
   hist->SetTitle(";Sigma Units;Asymmetry;");
   hist->SetOption("E1 P NOIMG");
   hist->GetYaxis()->SetRangeUser(-1.05, 1.05);
   hist->GetListOfFunctions()->Add(grAsymUniProfileFineBin, "p");
   o[shName] = hist;

   shName = "hAsymUniProfileBin";
   hist = new TH1F(shName.c_str(), shName.c_str(), 50, -5, 5);
   hist->SetTitle(";Sigma Units;Asymmetry;");
   hist->SetOption("E1 P NOIMG");
   hist->GetYaxis()->SetRangeUser(-1.05, 1.05);
   o[shName] = hist;
} //}}}


/** */
void CnipolProfileHists::PreFill(string sid)
{ //{{{
   ((TH1*) o["hIntensProfile"])->SetBins(gNDelimeters, 0, gNDelimeters);
   ((TH1*) o["hIntensProfileFineBin"])->SetBins(gNDelimeters*10, 0, gNDelimeters);
   ((TH1*) o["hAsymVsDelim4Det"])->SetBins(gNDelimeters*10, 0, gNDelimeters);

   //((TH1*) o["hIntensProfileScaler"])->SetBins(nTgtIndex, 0, nTgtIndex);
   //((TH1*) o["hPolarProfile"])->SetBins(nTgtIndex, 0, nTgtIndex);
   ((TH1*) o["hPolarProfile"])->SetBins(gNDelimeters, 0, gNDelimeters);

   //Double_t ymax = ((TH1*) o["hIntensProfile"])->GetMaximum();
   //((TH1*) o["hIntensProfile"])->Scale(1./ymax);
} //}}}


/** */
void CnipolProfileHists::Fill(ChannelEvent *ch, string sid)
{ //{{{
   Double_t ttime = ((Double_t) ch->GetRevolutionId())/RHIC_REVOLUTION_FREQ;

   //printf("ttime: %d, %d\n", ttime, ch->GetRevolutionId());
   //((TH2F*) sd->o["hSpinVsDelim"+sid+"_ch"+sSi])->Fill(ch->GetDelimiterId(), gSpinPattern[bId]);
   //((TH2F*) sd->o["hSpinVsDelim"+sid+"_ch"+sSi])->Fill(ttime, gSpinPattern[bId]);

   ((TH1*) o["hIntensProfile"])->Fill(ttime);
   ((TH1*) o["hIntensProfileFineBin"])->Fill(ttime);
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

   ((TH1*) o["hIntensProfileScaler"])->SetBins(n+1, 0, n+1);
   ((TH1*) o["hIntensProfileScaler"])->SetContent(hd);
   ((TH1*) o["hIntensProfileScaler"])->SetError(hdErr);
   //((TH1*) o["hIntensProfileScaler"])->Sumw2();
   ((TH1*) o["hIntensProfileScaler"])->SetEntries(nEntries);

   delete [] hd;
   delete [] hdErr;
} //}}}


/** */
void CnipolProfileHists::FillDerived(DrawObjContainer &oc)
{ //{{{
   CnipolAsymHists *hists_asym = (CnipolAsymHists*) oc.d.find("asym")->second;

   if (!hists_asym) {
      Error("FillDerived(DrawObjContainer &oc)", "Histogram container \"asym\" required");
      return;
   }

   TH1 *hAsymVsDelim4Det_asym = (TH1*) hists_asym->o["hAsymVsDelim4Det"];
   TH1 *hAsymVsDelim4Det      = (TH1*) o["hAsymVsDelim4Det"];
   TH1 *hIntensProfile        = (TH1*) o["hIntensProfileFineBin"];
   TH1 *hAsymVsIntensProfile  = (TH1*) o["hAsymVsIntensProfile"];

   TGraphErrors *gr = (TGraphErrors*) hAsymVsIntensProfile->GetListOfFunctions()->FindObject("grAsymVsIntensProfile");

   Double_t intensMax = hIntensProfile->GetMaximum();

   for (int ib=1; ib<=hAsymVsDelim4Det_asym->GetNbinsX(); ib++)
   {
      Double_t asym      = hAsymVsDelim4Det_asym->GetBinContent(ib);
      Double_t asymErr   = hAsymVsDelim4Det_asym->GetBinError(ib);
      Double_t intens    = hIntensProfile->GetBinContent(ib)/intensMax;
      Double_t intensErr = hIntensProfile->GetBinError(ib)/intensMax;

      hAsymVsDelim4Det->SetBinContent(ib, asym);
      hAsymVsDelim4Det->SetBinError(ib, asymErr);

      Int_t nPoint = gr->GetN();
      gr->SetPoint( nPoint, intens, asym);
      gr->SetPointError( nPoint, intensErr, asymErr);
   }

} //}}}


/** */
void CnipolProfileHists::PostFill()
{ //{{{
   //TH1* hIntensProfile = (TH1*) o["hIntensProfileScaler"];
   TH1* hIntensProfile = (TH1*) o["hIntensProfile"];

   if (!hIntensProfile->Integral()) {
      Error("PostFill", "Intensity profile histogram (hIntensProfile) is empty. Skipping...");
      return;
   }

   Double_t ymax = hIntensProfile->GetMaximum();

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

   // Now split the hIntensProfile hist into two: forward and backward motions

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

   if (!hPolarProfile->Integral()) {
      Error("PostFill", "Polarization profile histogram (hPolarProfile) is empty. Skipping...");
      return;
   }

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

   grPolProfileFinal->Fit("gaus", "M E");

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

   for (int i=1; i<=hIntensProfile->GetNbinsX(); i++)
   {
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

   TF1 *mfPow = new TF1("mfPow", "[0]*TMath::Power(x, [1])", 0.1, 1);

   mfPow->SetParNames("P_{max}", "r");
   mfPow->SetParameter(0, 0.5);
   mfPow->SetParLimits(0, 0, 1);
   mfPow->SetParameter(1, 0.1);
   mfPow->SetParLimits(1, -2, 2);

   fitres = grPolarVsIntensProfile->Fit(mfPow, "M E R S");

   if (fitres.Get()) {
      gAnaMeasResult->fFitResProfilePvsI = fitres;

      // the following should retire
      //gAnaMeasResult->fProfilePolarMax = ValErrPair(mfPow->GetParameter(0), mfPow->GetParError(0));
      //gAnaMeasResult->fProfilePolarR   = ValErrPair(mfPow->GetParameter(1), mfPow->GetParError(1));

      gAnaMeasResult->fProfilePolarMax = ValErrPair(fitres->Value(0), fitres->FitResult::Error(0));
      gAnaMeasResult->fProfilePolarR   = ValErrPair(fitres->Value(1), fitres->FitResult::Error(1));

   } else {
      Error("PostFill", "Something is wrong with profile fit");
   }

   TH1* hPolarVsIntensProfile = (TH1*) o["hPolarVsIntensProfile"];
   hPolarVsIntensProfile->GetListOfFunctions()->Add(grPolarVsIntensProfile, "p");

   //char sratio[50];
   //sprintf(sratio, "% 8.3f, % 6.3f", gAnaMeasResult->fProfilePolarR.first, gAnaMeasResult->fProfilePolarMax.second);
   //gRunDb.fFields["PROFILE_RATIO"] = sratio;

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

   for (int i=1; i<=hIntensProfile->GetNbinsX(); i++)
   {
      float intens    = hIntensProfile->GetBinContent(i);

      if (intens <= 0) continue;

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

   grIntensUniProfile->Fit("gaus", "M E");

   TH1* hIntensUniProfile = (TH1*) o["hIntensUniProfile"];
   hIntensUniProfile->GetListOfFunctions()->Add(grIntensUniProfile, "p");

   grPolarUniProfile->Fit("gaus", "M E", "", -3, 3);

   TH1* hPolarUniProfile = (TH1*) o["hPolarUniProfile"];
   hPolarUniProfile->GetListOfFunctions()->Add(grPolarUniProfile, "p");


   // Fine binning profiles
   TH1 *hIntensProfileFineBin = (TH1*) o["hIntensProfileFineBin"];
   TH1 *hAsymVsDelim4Det      = (TH1*) o["hAsymVsDelim4Det"];
   TH1 *hAsymUniProfile       = (TH1*) o["hAsymUniProfile"];

   TGraphErrors *grIntensUniProfileFineBin = (TGraphErrors*) hIntensUniProfile->GetListOfFunctions()->FindObject("grIntensUniProfileFineBin");
   TGraphErrors *grAsymUniProfileFineBin   = (TGraphErrors*) hAsymUniProfile->GetListOfFunctions()->FindObject("grAsymUniProfileFineBin");

   Double_t intensMax = hIntensProfileFineBin->GetMaximum();

   for (Int_t ib=1; ib<=hIntensProfileFineBin->GetNbinsX(); ib++)
   {
      float intens    = hIntensProfileFineBin->GetBinContent(ib)/intensMax;

      if (intens <= 0) continue;

      float intensErr = hIntensProfileFineBin->GetBinError(ib)/intensMax;
      float asym      = hAsymVsDelim4Det->GetBinContent(ib);
      float asymErr   = hAsymVsDelim4Det->GetBinError(ib);

      float xuni_p =    TMath::Sqrt(-2*TMath::Log(intens));
      float xuni_m = -1*TMath::Sqrt(-2*TMath::Log(intens));

      grIntensUniProfileFineBin->SetPoint( (ib-1)*2 + 0, xuni_p, intens);
      grIntensUniProfileFineBin->SetPoint( (ib-1)*2 + 1, xuni_m, intens);
      grIntensUniProfileFineBin->SetPointError( (ib-1)*2 + 0, 0, intensErr);
      grIntensUniProfileFineBin->SetPointError( (ib-1)*2 + 1, 0, intensErr);

      grAsymUniProfileFineBin->SetPoint( (ib-1)*2 + 0, xuni_p, asym);
      grAsymUniProfileFineBin->SetPoint( (ib-1)*2 + 1, xuni_m, asym);
      grAsymUniProfileFineBin->SetPointError( (ib-1)*2 + 0, 0, asymErr);
      grAsymUniProfileFineBin->SetPointError( (ib-1)*2 + 1, 0, asymErr);
   }

   Info("PostFill", "fitting test");

   TF1 *myGaus = new TF1("myGaus", "[0]*TMath::Gaus(x, [1], [2])", -3, 3);
   myGaus->SetParNames("Constant", "Mean", "Sigma");
   myGaus->SetParameters(0.5, 0, 1);
   myGaus->SetParLimits(0, 0, 100);
   myGaus->FixParameter(1, 0); // fix mean at 0 by definition
   myGaus->SetParLimits(2, 0, 100);

   grAsymUniProfileFineBin->Fit(myGaus, "M E R");

   // reset parameters and fit another graph
   myGaus->SetParameters(0.5, 0, 1);
   myGaus->SetParLimits(0, 0, 100);
   myGaus->FixParameter(1, 0); // fix mean at 0 by definition
   myGaus->SetParLimits(2, 0, 100);
   //grIntensUniProfileFineBin->Fit("gaus", "M E", "", -3, 3);
   grIntensUniProfileFineBin->Fit(myGaus, "M E R");

   delete myGaus;

   TH1 *hAsymUniProfileBin = (TH1*) o["hAsymUniProfileBin"];
   utils::BinGraph(grAsymUniProfileFineBin, hAsymUniProfileBin);

   // Fill hist from graph
   TH1* hIntensUniProfileBin     = (TH1*) o["hIntensUniProfileBin"];
   TH1* hPolarUniProfileBin      = (TH1*) o["hPolarUniProfileBin"];
   TH1* hPolarVsIntensProfileBin = (TH1*) o["hPolarVsIntensProfileBin"];

   utils::BinGraph(grIntensUniProfile,     hIntensUniProfileBin);
   utils::BinGraph(grPolarUniProfile,      hPolarUniProfileBin);
   utils::BinGraph(grPolarVsIntensProfile, hPolarVsIntensProfileBin);

   hIntensUniProfileBin->Fit("gaus", "M E");
   hPolarUniProfileBin->Fit("gaus", "M E", "", -3, 3);

   hPolarVsIntensProfileBin->Fit("mfPow", "M E R");

   TH1 *hAsymVsIntensProfile = (TH1*) o["hAsymVsIntensProfile"];
   TGraphErrors *gr = (TGraphErrors*) hAsymVsIntensProfile->GetListOfFunctions()->FindObject("grAsymVsIntensProfile");
   gr->Fit("mfPow", "M E R");

   delete mfPow;

   // Set measurement type
   gMeasInfo->fMeasType = GuessMeasurementType();
} //}}}


/** */
EMeasType CnipolProfileHists::GuessMeasurementType()
{ //{{{
   TH1* hIntensProfile = (TH1*) o["hIntensProfile"];

   if (!hIntensProfile) {
      Error("GuessMeasurementType", "Histogram (hIntensProfile) not defined");
      return kMEASTYPE_UNKNOWN;
   }

   Double_t ymax = hIntensProfile->GetMaximum();

   if (ymax > 0) hIntensProfile->Scale(1./ymax);
   else
      Error("GuessMeasurementType", "Empty histogram (hIntensProfile) ?");

   TH1F *hIntensProj = new TH1F("hIntensProj", "hIntensProj", 20, 0, 1);

   utils::ConvertToProfile(hIntensProfile, hIntensProj, kFALSE);

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
