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
void CnipolProfileHists::BookHists()
{
   fDir->cd();

   string      shName;
   TH1        *hist;
   TAttMarker  styleMarker;

   styleMarker.SetMarkerStyle(kFullCircle);
   styleMarker.SetMarkerSize(1);
   styleMarker.SetMarkerColor(kGreen+2);

   // this one is filled from the scaler data
   shName = "hIntensProfileScaler";
   hist = new TH1D(shName.c_str(), shName.c_str(), 1, 0, 1); // The number of steps will be taken from data
   hist->SetTitle("; Time, s; Events");
   hist->SetOption("NOIMG");
   //hist->SetBit(TH1::kCanRebin);
   hist->Sumw2();
   o[shName] = hist;

   // this one is filled from the event data (it is a more correct way of doing
   // this than using hIntensProfileScaler)
   shName = "hIntensProfile";
   hist = new TH1D(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle("; Time, s; Events");
   hist->SetOption("E1");
   hist->Sumw2();
   o[shName] = hist;
   fhIntensProfile = hist;

   shName = "hIntensProfileFine";
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle("; Time, s; Events");
   hist->SetOption("E1");
   hist->Sumw2();
   o[shName] = hist;
   fhIntensProfileFine = hist;

   shName = "hIntensProfileFwd";
   hist = new TH1D(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle("; Target Steps, s; Events;");
   hist->SetOption("E1 NOIMG");
   o[shName] = hist;

   shName = "hIntensProfileBck";
   hist = new TH1D(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle("; Target Steps, s; Events;");
   hist->SetOption("E1 NOIMG");
   o[shName] = hist;

   shName = "hIntensProfileFold";
   hist = new TH1D(shName.c_str(), shName.c_str(), 1, 0, 1); // The number of steps will be taken from data
   hist->SetTitle("; Target Steps, s; Events;");
   hist->SetOption("E1 NOIMG");
   o[shName] = hist;

   shName = "hPolarProfile";
   hist = new TH1D(shName.c_str(), shName.c_str(), 1, 0, 1); // The number of delimeters is not known beforehand
   hist->SetTitle("; Time, s; Polarization;");
   hist->SetOption("E1");
   hist->SetBit(TH1::kCanRebin);
   hist->GetYaxis()->SetRangeUser(-1.05, 1.05);
   o[shName] = hist;
   fhPolarProfile = hist;

   shName = "hPolarProfileFwd";
   hist = new TH1D(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle("; Time, s; Polarization;");
   hist->SetOption("NOIMG");
   hist->GetYaxis()->SetRangeUser(0, 1.05);
   hist->SetMarkerStyle(kFullDotLarge);
   hist->SetMarkerSize(1);
   hist->SetMarkerColor(kRed);
   o[shName] = hist;

   shName = "hPolarProfileBck";
   hist = new TH1D(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle("; Time, s; Polarization;");
   hist->SetOption("NOIMG");
   hist->GetYaxis()->SetRangeUser(0, 1.05);
   hist->SetMarkerStyle(kFullDotLarge);
   hist->SetMarkerSize(1);
   hist->SetMarkerColor(kRed);
   o[shName] = hist;

   shName = "hPolarProfileFold";
   hist = new TH1D(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle("; Time, s; Polarization;");
   hist->SetOption("NOIMG");
   hist->GetYaxis()->SetRangeUser(0, 1.05);
   o[shName] = hist;

   shName = "hPolarProfileFinal";
   hist = new TH1D(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle("; Distance, mm; Polarization;");
   hist->SetOption("NOIMG");
   //hist->SetAxisRange(0, 1, "Y");
   //hist->GetYaxis()->SetLimits(0, 1);
   hist->SetMarkerStyle(kFullDotLarge);
   hist->SetMarkerSize(1);
   hist->SetMarkerColor(kRed);
   o[shName] = hist;

   // asymmetry vs intensity
   TGraphErrors *grAsymVsIntensProfile = new TGraphErrors();
   grAsymVsIntensProfile->SetName("grAsymVsIntensProfile");
   styleMarker.Copy(*grAsymVsIntensProfile);

   //TGraphErrors *grAsymVsIntensProfileFineBin = new TGraphErrors();
   //grAsymVsIntensProfileFineBin->SetName("grAsymVsIntensProfileFineBin");
   //styleMarker.SetMarkerColor(kRed);
   //styleMarker.Copy(*grAsymVsIntensProfileFineBin);

   shName = "hAsymVsIntensProfile";
   hist = new TH2I(shName.c_str(), shName.c_str(), 1, 0, 1.1, 1, -0.05, 0.05);
   hist->SetTitle("; Relative Intensity I/I_{max}; Asymmetry;");
   hist->SetOption("DUMMY");
   hist->GetListOfFunctions()->Add(grAsymVsIntensProfile, "p");
   //hist->GetListOfFunctions()->Add(grAsymVsIntensProfileFineBin, "p");
   o[shName] = hist;
   fhAsymVsIntensProfile = hist;

   // this is a copy from the "asym" container
   shName = "hAsymVsDelim4Det";
   hist = new TH1D(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetOption("E1");
   hist->SetTitle("; Time, s; Asymmetry;");
   o[shName] = hist;

   // polar vs intensity
   //TGraphErrors *grPolarVsIntensProfileFineBin = new TGraphErrors();
   //grPolarVsIntensProfileFineBin->SetName("grPolarVsIntensProfileFineBin");
   //grPolarVsIntensProfileFineBin->SetMarkerStyle(kFullDotLarge);
   //grPolarVsIntensProfileFineBin->SetMarkerSize(1);
   //grPolarVsIntensProfileFineBin->SetMarkerColor(kMagenta);

   shName = "hPolarVsIntensProfile";
   hist = new TH1F(shName.c_str(), shName.c_str(), 10, 0, 1.1);
   hist->SetTitle("; Relative Intensity I/I_{max}; Polarization;");
   hist->SetOption("DUMMY");
   hist->GetYaxis()->SetRangeUser(0, 1.05);
   o[shName] = hist;
   //hist->GetListOfFunctions()->Add(grPolarVsIntensProfileFineBin, "p");

   shName = "hPolarVsIntensProfileBin";
   hist = new TH1F(shName.c_str(), shName.c_str(), 22, 0, 1.1);
   hist->SetTitle("; Relative Intensity I/I_{max}; Polarization;");
   //hist->SetOption("");
   hist->GetYaxis()->SetRangeUser(0, 1.05);
   hist->SetMarkerStyle(kFullDotLarge);
   hist->SetMarkerSize(1);
   hist->SetMarkerColor(kBlue);
   hist->Sumw2();
   o[shName] = hist;

   shName = "hPolarVsIntensProfileLargeBins";
   hist = new TH1F(shName.c_str(), shName.c_str(), 11, 0, 1.1);
   hist->SetTitle("; Relative Intensity I/I_{max}; Polarization;");
   //hist->SetOption("");
   hist->GetYaxis()->SetRangeUser(0, 1.05);
   hist->SetMarkerStyle(kFullDotLarge);
   hist->SetMarkerSize(1);
   hist->SetMarkerColor(kBlue);
   hist->Sumw2();
   o[shName] = hist;


   // Intensity profile
   TGraphErrors *grIntensUniProfileFineBin = new TGraphErrors();
   grIntensUniProfileFineBin->SetName("grIntensUniProfileFineBin");
   styleMarker.Copy(*grIntensUniProfileFineBin);

   shName = "hIntensUniProfile";
   hist = new TH1F(shName.c_str(), shName.c_str(), 100, -5, 5);
   hist->SetTitle("; Sigma Units; Intensity;");
   hist->GetListOfFunctions()->Add(grIntensUniProfileFineBin, "p");
   o[shName] = hist;

   shName = "hIntensUniProfileBin";
   hist = new TH1F(shName.c_str(), shName.c_str(), 20, -5, 5);
   hist->SetTitle("; Sigma Units; Intensity;");
   hist->SetOption("p");
   hist->SetMarkerStyle(kFullDotLarge);
   hist->SetMarkerSize(1);
   hist->SetMarkerColor(kBlue);
   hist->Sumw2();
   o[shName] = hist;

   //sprintf(shName, "grIntensUniProfile");
   //o[shName] = new TGraphErrors();

   // Polarization profile
   //TGraphErrors *grPolarUniProfileFineBin = new TGraphErrors();
   //grPolarUniProfileFineBin->SetName("grPolarUniProfileFineBin");
   //grPolarUniProfileFineBin->SetMarkerStyle(kFullDotLarge);
   //grPolarUniProfileFineBin->SetMarkerSize(1);
   //grPolarUniProfileFineBin->SetMarkerColor(kMagenta);

   shName = "hPolarUniProfile";
   hist = new TH1C(shName.c_str(), shName.c_str(), 1, -5, 5);
   hist->SetTitle("; Sigma Units; Polarization;");
   hist->SetOption("DUMMY");
   hist->GetYaxis()->SetRangeUser(0, 1.05);
   //hist->GetListOfFunctions()->Add(grPolarUniProfileFineBin, "p");
   o[shName] = hist;

   shName = "hPolarUniProfileBin";
   hist = new TH1F(shName.c_str(), shName.c_str(), 20, -5, 5);
   hist->SetTitle("; Sigma Units; Polarization;");
   //hist->GetYaxis()->SetRangeUser(0, 1.05);
   hist->GetYaxis()->SetRangeUser(0, 1.05);
   //hist->SetOption("NOIMG");
   hist->SetMarkerStyle(kFullDotLarge);
   hist->SetMarkerSize(1);
   hist->SetMarkerColor(kBlue);
   hist->Sumw2();
   o[shName] = hist;

   // Asymmetry profile
   TGraphErrors *grAsymUniProfileFineBin = new TGraphErrors();
   grAsymUniProfileFineBin->SetName("grAsymUniProfileFineBin");
   styleMarker.Copy(*grAsymUniProfileFineBin);

   shName = "hAsymUniProfile";
   hist = new TH1F(shName.c_str(), shName.c_str(), 100, -5, 5);
   hist->SetTitle("; Sigma Units; Asymmetry;");
   hist->SetOption("DUMMY");
   //hist->GetYaxis()->SetRangeUser(-1.05, 1.05);
   hist->GetYaxis()->SetRangeUser(-0.01, 0.01);
   hist->GetListOfFunctions()->Add(grAsymUniProfileFineBin, "p");
   o[shName] = hist;

   shName = "hAsymUniProfileBin";
   hist = new TH1F(shName.c_str(), shName.c_str(), 50, -5, 5);
   hist->SetTitle("; Sigma Units; Asymmetry;");
   hist->SetOption("E1 P");
   //hist->GetYaxis()->SetRangeUser(-1.05, 1.05);
   hist->GetYaxis()->SetRangeUser(-0.01, 0.01);
   o[shName] = hist;


   DrawObjContainer        *oc;
   DrawObjContainerMapIter  isubdir;

   for (int iChId=1; iChId<=N_SILICON_CHANNELS; iChId++)
   {
      string sChId("  ");
      sprintf(&sChId[0], "%02d", iChId);

      string dName = "channel" + sChId;

      isubdir = d.find(dName);

      if ( isubdir == d.end()) { // if dir not found
         oc = new DrawObjContainer();
         oc->fDir = new TDirectoryFile(dName.c_str(), dName.c_str(), "", fDir);
      } else {
         oc = isubdir->second;
      }

      oc->fDir->cd();

      shName = "hIntensProfile_ch" + sChId;
      hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
      hist->SetTitle("; Time, s; Events");
      hist->Sumw2();
      hist->SetOption("E1");
      oc->o[shName] = hist;
      fhIntensProfile_ch[iChId-1] = hist;

      shName = "hIntensProfileFine_ch" + sChId;
      hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
      hist->SetTitle("; Time, s; Events");
      hist->Sumw2();
      hist->SetOption("E1");
      oc->o[shName] = hist;
      fhIntensProfileFine_ch[iChId-1] = hist;

      // If this is a new directory then we need to add it to the list
      if ( isubdir == d.end()) {
         d[dName] = oc;
      }
   }
}


/** */
void CnipolProfileHists::PreFill()
{
   fhIntensProfile->SetBins(10*gNDelimeters, 0, gNDelimeters);
   fhIntensProfileFine->SetBins(100*gNDelimeters, 0, gNDelimeters);
   ((TH1*) o["hAsymVsDelim4Det"])->SetBins(10*gNDelimeters, 0, gNDelimeters);

   //((TH1*) o["hIntensProfileScaler"])->SetBins(nTgtIndex, 0, nTgtIndex);
   //fhPolarProfile->SetBins(nTgtIndex, 0, nTgtIndex);
   fhPolarProfile->SetBins(10*gNDelimeters, 0, gNDelimeters); // this is equivalent to hAsymVsDelim4Ch in asym

   for (int iChId=1; iChId<=N_SILICON_CHANNELS; iChId++)
   {
      fhIntensProfile_ch[iChId-1]->SetBins(10*gNDelimeters, 0, gNDelimeters);
      fhIntensProfileFine_ch[iChId-1]->SetBins(100*gNDelimeters, 0, gNDelimeters);
   }
}


/** */
void CnipolProfileHists::Fill(ChannelEvent *ch)
{
   Double_t time = ((Double_t) ch->GetRevolutionId())/RHIC_REVOLUTION_FREQ;
   UChar_t  chId = ch->GetChannelId();

   //printf("time: %d, %d\n", time, ch->GetRevolutionId());
   //((TH2F*) sd->o["hSpinVsDelim_ch"+sSi])->Fill(ch->GetDelimiterId(), gSpinPattern[bId]);
   //((TH2F*) sd->o["hSpinVsDelim_ch"+sSi])->Fill(time, gSpinPattern[bId]);

   fhIntensProfile_ch[chId-1]->Fill(time);
   fhIntensProfileFine_ch[chId-1]->Fill(time);
}


/** */
void CnipolProfileHists::Fill(UInt_t n, int32_t* hData)
{
   Double_t *hd    = new Double_t[n+3](); // 2+1 seems one bin is lost compared to PC_TARGET histograms
   Double_t *hdErr = new Double_t[n+3](); // 2+1 seems one bin is lost compared to PC_TARGET histograms

   //copy(hData, hData+1, hd);
   int32_t  *hPtr     = hData;
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
}


/** */
void CnipolProfileHists::FillDerived(DrawObjContainer &oc)
{
   Info("FillDerived(DrawObjContainer &oc)", "Called");

   // Combine individual channel histograms
   for (int iChId=1; iChId<=N_SILICON_CHANNELS; iChId++)
   {
      fhIntensProfile->Add(fhIntensProfile_ch[iChId-1]);
      fhIntensProfileFine->Add(fhIntensProfileFine_ch[iChId-1]);
   }

   // Fill/transfer asym and polar vs time/delim histograms
   CnipolAsymHists *hists_asym = (CnipolAsymHists*) oc.d.find("asym")->second;

   if (!hists_asym) {
      Error("FillDerived(DrawObjContainer &oc)", "Histogram container \"asym\" not found. Skipping...");
      return;
   }

   TH1 *hAsymVsDelim4Det_asym = (TH1*) hists_asym->o["hAsymVsDelim4Det"];
   //TH1 *hAsymVsDelim4Ch_asym  = (TH1*) hists_asym->o["hAsymVsDelim4Ch"];
   TH1 *hAsymVsDelim4Det      = (TH1*) o["hAsymVsDelim4Det"];

   TGraphErrors *grAsymVsIntensProfile = (TGraphErrors*) fhAsymVsIntensProfile->GetListOfFunctions()->FindObject("grAsymVsIntensProfile");

   Double_t maxCarbonRate = fhIntensProfile->GetMaximum();

   for (int ib=1; ib<=hAsymVsDelim4Det_asym->GetNbinsX(); ib++)
   {
      Double_t asym      = hAsymVsDelim4Det_asym->GetBinContent(ib);
      Double_t asymErr   = hAsymVsDelim4Det_asym->GetBinError(ib);
      Double_t intens    = fhIntensProfile->GetBinContent(ib)/maxCarbonRate;
      Double_t intensErr = fhIntensProfile->GetBinError(ib)/maxCarbonRate;

      hAsymVsDelim4Det->SetBinContent(ib, asym);
      hAsymVsDelim4Det->SetBinError(ib, asymErr);

      //asym    = hAsymVsDelim4Ch_asym->GetBinContent(ib);
      //asymErr = hAsymVsDelim4Ch_asym->GetBinError(ib);

      utils::AppendToGraph(grAsymVsIntensProfile, intens, asym, intensErr, asymErr);

      fhPolarProfile->SetBinContent(ib, asym/gAnaMeasResult->A_N[1]);
      fhPolarProfile->SetBinError(ib,   asymErr/gAnaMeasResult->A_N[1]);
   }
}


/** */
void CnipolProfileHists::PostFill()
{
   Info("PostFill", "Called");

   //TH1* hIntensProfile = (TH1*) o["hIntensProfileScaler"];
   //TH1* hIntensProfile = (TH1*) o["hIntensProfile"];

   if (!fhIntensProfile->Integral()) {
      Error("PostFill", "Intensity profile histogram (hIntensProfile) is empty. Skipping...");
      return;
   }

   Double_t ymaxFine = fhIntensProfileFine->GetMaximum();
   fhIntensProfileFine->Scale(1./ymaxFine);

   Double_t ymax = fhIntensProfile->GetMaximum();

   fhIntensProfile->Scale(1./ymax);
   Double_t xmin = fhIntensProfile->GetXaxis()->GetXmin();
   Double_t xmax = fhIntensProfile->GetXaxis()->GetXmax();
   ymax = fhIntensProfile->GetMaximum();

   //TF1 profileFitFunc("ProfileFitFunc", CnipolProfileHists::ProfileFitFunc, xmin, xmax, 4);
   TF1 profileFitFunc("ProfileFitFunc", this, &CnipolProfileHists::ProfileFitFunc, xmin, xmax, 4, "CnipolProfileHists", "ProfileFitFunc");

   profileFitFunc.SetLineColor(kRed-3);
   profileFitFunc.SetParNames("#sigma", "#mu_{1}", "#mu_{2}", "N_{scale}");
   profileFitFunc.SetParameters(5, (xmax-xmin)*0.33, (xmax-xmin)*0.66, 1e6);
   profileFitFunc.SetParLimits(0, 0.5, 10);
   profileFitFunc.SetParLimits(1, (xmax-xmin)*0.1, (xmax-xmin)*0.4);
   profileFitFunc.SetParLimits(2, (xmax-xmin)*0.6, xmax*0.9);
   profileFitFunc.SetParLimits(3, ymax*0.5, ymax*1.5);

   TFitResultPtr fitres = fhIntensProfile->Fit(&profileFitFunc, "M S", "");


   TF1 profileTailFitFunc("ProfileTailFitFunc", this, &CnipolProfileHists::ProfileTailFitFunc, xmin, xmax, 4, "CnipolProfileHists", "ProfileTailFitFunc");

   profileTailFitFunc.SetLineColor(kGreen-3);
   profileTailFitFunc.SetParNames("#sigma", "#mu_{1}", "#mu_{2}", "N_{scale}");
   profileTailFitFunc.SetParameters(5, (xmax-xmin)*0.33, (xmax-xmin)*0.66, 1e6);
   profileTailFitFunc.SetParLimits(0, 0.5, 10);
   profileTailFitFunc.SetParLimits(1, (xmax-xmin)*0.1, (xmax-xmin)*0.4);
   profileTailFitFunc.SetParLimits(2, (xmax-xmin)*0.6, xmax*0.9);
   profileTailFitFunc.SetParLimits(3, ymax*0.5, ymax*3);

   fitres = fhIntensProfile->Fit(&profileTailFitFunc, "M S R +", "");

   // Now split the fhIntensProfile hist into two: forward and backward motions
   double sigma    = 0;
   double mean1    = 0;
   double mean2    = 0;

   if (fitres.Get()) {
      sigma    = fitres->Value(0);
      mean1    = fitres->Value(1);
      mean2    = fitres->Value(2);
   }

   TH1* hIntensProfileFwd  = (TH1*) o["hIntensProfileFwd"];
   TH1* hIntensProfileBck  = (TH1*) o["hIntensProfileBck"];
   TH1* hIntensProfileFold = (TH1*) o["hIntensProfileFold"];
   TH1* hPolarProfileFwd   = (TH1*) o["hPolarProfileFwd"];
   TH1* hPolarProfileBck   = (TH1*) o["hPolarProfileBck"];
   TH1* hPolarProfileFold  = (TH1*) o["hPolarProfileFold"];
   TH1* hPolarProfileFinal = (TH1*) o["hPolarProfileFinal"];

   if (!fhPolarProfile->Integral()) {
      Error("PostFill", "Polarization profile histogram (hPolarProfile) is empty. Skipping...");
      return;
   }

   Int_t bc  = fhIntensProfile->FindBin( mean1 + (mean2 - mean1)*0.5);

   Int_t bmean1 = fhIntensProfile->FindBin(mean1);
   Int_t bmean2 = fhIntensProfile->FindBin(mean2);
   Int_t bbegin = fhIntensProfile->FindBin(mean1-3*sigma);
   Int_t bend   = fhIntensProfile->FindBin(mean1+3*sigma);

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

   for (int i=bbegin, j=1; i<=bend; i++, j++)
   {
      hIntensProfileFwd->SetBinContent(j, fhIntensProfile->GetBinContent(i));
      hIntensProfileBck->SetBinContent(j, fhIntensProfile->GetBinContent(i + bmean2 - bmean1));
      //hIntensProfileFold->SetBinContent(j, fhIntensProfile->GetBinContent(i) + fhIntensProfile->GetBinContent(i + bmean2 - bmean1));

      hPolarProfileFwd->SetBinContent(j, fhPolarProfile->GetBinContent(i));
      hPolarProfileFwd->SetBinError  (j, fhPolarProfile->GetBinError(i));
      hPolarProfileBck->SetBinContent(j, fhPolarProfile->GetBinContent(i + bmean2 - bmean1));
      hPolarProfileBck->SetBinError  (j, fhPolarProfile->GetBinError(i + bmean2 - bmean1));
      //hPolarProfileFold->SetBinContent(j, fhPolarProfile->GetBinContent(i) + fhPolarProfile->GetBinContent(i + bmean2 - bmean1));
   }

   // Add forward and backward motions
   hPolarProfileFold->Add(hPolarProfileFwd, hPolarProfileBck);
   hPolarProfileFold->GetYaxis()->SetRangeUser(0, 1.05);

   //hPolarProfileFinal->Add(hPolarProfileFwd, hPolarProfileBck);

   Float_t maxDistance = FLT_MIN, minDistance = FLT_MAX;
   Float_t maxPol = FLT_MIN;

   TGraphErrors *grPolProfileFinal = new TGraphErrors();
   grPolProfileFinal->SetName("grPolProfileFinal");
   grPolProfileFinal->SetMarkerStyle(kFullDotLarge);
   grPolProfileFinal->SetMarkerSize(1);
   grPolProfileFinal->SetMarkerColor(kRed);

   //printf("\n%5s %5s %5s\n", "dist", "pol", "err");

   for (int i=bbegin, j=0; i<=bend; i++, j++)
   {
      Double_t pol     = hPolarProfileFold->GetBinContent(j+1);
      Double_t polErr  = hPolarProfileFold->GetBinError(j+1);
      Float_t distance = (tgt.all.x[i] - tgt.all.x[bmean1]) / 730.;

      if (distance > maxDistance) maxDistance = distance;
      if (distance < minDistance) minDistance = distance;
      if (pol > maxPol) maxPol = pol;

      //printf("%5.2f %5.2f %5.2f\n", distance, pol, polErr);

      grPolProfileFinal->SetPoint(j, distance, pol);
      grPolProfileFinal->SetPointError(j, 0, polErr);
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

   for (int i=1; i<=fhIntensProfile->GetNbinsX(); i++)
   {
      float intens    = fhIntensProfile->GetBinContent(i);
      float intensErr = fhIntensProfile->GetBinError(i);
      float polar     = fhPolarProfile->GetBinContent(i);
      float polarErr  = fhPolarProfile->GetBinError(i);

      //printf("%5d %5d %5.2f %5.2f %5.2f\n", i, bmean, intens, polar, polarErr);

      grPolarVsIntensProfile->SetPoint(i, intens, polar);
      grPolarVsIntensProfile->SetPointError(i, intensErr, polarErr);
   }

   TF1 mfPow("mfPow", "[0]*TMath::Power(x, [1])", 0.1, 1);

   mfPow.SetParNames("P_{max}", "R");
   mfPow.SetParameters(0.5, 0.1);
   mfPow.SetParLimits(0, 0, 1);
   mfPow.SetParLimits(1, -2, 20);

   fitres = grPolarVsIntensProfile->Fit(&mfPow, "M E R S");

   if (fitres.Get()) {
      gAnaMeasResult->fFitResProfilePvsI = fitres;

      // the following should retire
      gAnaMeasResult->fProfilePolarMax = ValErrPair(fitres->Value(0), fitres->FitResult::Error(0));
      gAnaMeasResult->fProfilePolarR   = ValErrPair(fitres->Value(1), fitres->FitResult::Error(1));

   } else {
      Error("PostFill", "Something is wrong with profile fit");
   }

   mfPow.SetRange(0.1, 0.75);
   mfPow.SetLineColor(kOrange-1);
   fitres = grPolarVsIntensProfile->Fit(&mfPow, "M E R S +");

   if (fitres.Get()) gAnaMeasResult->fFitResProfilePvsIRange1 = fitres;
   else Error("PostFill", "Something is wrong with profile fit");

   mfPow.SetRange(0.80, 1);
   mfPow.SetLineColor(kOrange-3);
   fitres = grPolarVsIntensProfile->Fit(&mfPow, "M E R S +");

   if (fitres.Get()) gAnaMeasResult->fFitResProfilePvsIRange2 = fitres;
   else Error("PostFill", "Something is wrong with profile fit");


   TH1* hPolarVsIntensProfile = (TH1*) o["hPolarVsIntensProfile"];
   hPolarVsIntensProfile->GetListOfFunctions()->Add(grPolarVsIntensProfile, "p");

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

   for (int i=1; i<=fhIntensProfile->GetNbinsX(); i++)
   {
      float intens    = fhIntensProfile->GetBinContent(i);

      if (intens <= 0) continue;

      float intensErr = fhIntensProfile->GetBinError(i);
      float polar     = fhPolarProfile->GetBinContent(i);
      float polarErr  = fhPolarProfile->GetBinError(i);


      float xuni_p =    TMath::Sqrt(-2*TMath::Log(intens));
      float xuni_m = -1*TMath::Sqrt(-2*TMath::Log(intens));

      printf("%5d %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f\n", i, intens, intensErr, polar, polarErr, xuni_p, xuni_m);

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


   TH1 *hAsymVsDelim4Det = (TH1*) o["hAsymVsDelim4Det"];
   TH1 *hAsymUniProfile  = (TH1*) o["hAsymUniProfile"];

   TGraphErrors *grIntensUniProfileFineBin = (TGraphErrors*) hIntensUniProfile->GetListOfFunctions()->FindObject("grIntensUniProfileFineBin");
   TGraphErrors *grAsymUniProfileFineBin   = (TGraphErrors*) hAsymUniProfile->GetListOfFunctions()->FindObject("grAsymUniProfileFineBin");

   Double_t maxCarbonRate = fhIntensProfile->GetMaximum();

   for (Int_t ib=1; ib<=fhIntensProfile->GetNbinsX(); ib++)
   {
      float intens    = fhIntensProfile->GetBinContent(ib)/maxCarbonRate;

      if (intens <= 0) continue;

      float intensErr = fhIntensProfile->GetBinError(ib)/maxCarbonRate;
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

   TF1 myGaus("myGaus", "[0]*TMath::Gaus(x, [1], [2])", -3, 3);
   myGaus.SetParNames("Constant", "Mean", "Sigma");
   myGaus.SetParameters(0.5, 0, 1);
   myGaus.SetParLimits(0, 0, 100);
   myGaus.FixParameter(1, 0); // fix mean at 0 by definition
   myGaus.SetParLimits(2, 0, 100);

   grAsymUniProfileFineBin->Fit(&myGaus, "M E R");

   // reset parameters and fit another graph
   myGaus.SetParameters(0.5, 0, 1);
   myGaus.SetParLimits(0, 0, 100);
   myGaus.FixParameter(1, 0); // fix mean at 0 by definition
   myGaus.SetParLimits(2, 0, 100);
   grIntensUniProfileFineBin->Fit(&myGaus, "M E R");


   TH1 *hAsymUniProfileBin = (TH1*) o["hAsymUniProfileBin"];
   utils::BinGraph(grAsymUniProfileFineBin, hAsymUniProfileBin);

   // Fill hist from graph
   TH1* hIntensUniProfileBin     = (TH1*) o["hIntensUniProfileBin"];
   TH1* hPolarUniProfileBin      = (TH1*) o["hPolarUniProfileBin"];
   TH1* hPolarVsIntensProfileBin = (TH1*) o["hPolarVsIntensProfileBin"];
   TH1* hPolarVsIntensProfileLargeBins =(TH1*) o["hPolarVsIntensProfileLargeBins"];

   utils::BinGraph(grIntensUniProfile,     hIntensUniProfileBin);
   utils::BinGraph(grPolarUniProfile,      hPolarUniProfileBin);
   utils::BinGraph(grPolarVsIntensProfile, hPolarVsIntensProfileBin);
   utils::BinGraph(grPolarVsIntensProfile, hPolarVsIntensProfileLargeBins);

   hIntensUniProfileBin->Fit("gaus", "M E");
   hPolarUniProfileBin->Fit("gaus", "M E", "", -3, 3);

   mfPow.SetRange(0.10, 1);
   mfPow.SetLineColor(kBlue);

   hPolarVsIntensProfileBin->Fit(&mfPow, "M E R");
   hPolarVsIntensProfileLargeBins->Fit(&mfPow, "M E R");
   TGraphErrors *grAsymVsIntensProfile = (TGraphErrors*) fhAsymVsIntensProfile->GetListOfFunctions()->FindObject("grAsymVsIntensProfile");
   fitres = grAsymVsIntensProfile->Fit(&mfPow, "M E R S");

   //if (fitres.Get()) {
   //   gAnaMeasResult->fFitResProfileAsymVsI = fitres;
   //} else {
   //   Error("PostFill", "Something is wrong with asym profile fit");
   //}


   // Set measurement type
   if (gMeasInfo->GetMeasType() == kMEASTYPE_UNKNOWN)
      gMeasInfo->SetMeasType( GuessMeasurementType() );
}


/** */
EMeasType CnipolProfileHists::GuessMeasurementType()
{
   if (!fhIntensProfile) {
      Error("GuessMeasurementType", "Histogram (hIntensProfile) not defined");
      return kMEASTYPE_UNKNOWN;
   }

   Double_t ymax = fhIntensProfile->GetMaximum();

   if (ymax > 0) fhIntensProfile->Scale(1./ymax);
   else
      Error("GuessMeasurementType", "Empty histogram (hIntensProfile) ?");

   TH1F *hIntensProj = new TH1F("hIntensProj", "hIntensProj", 20, 0, 1);

   utils::ConvertToProfile(fhIntensProfile, hIntensProj, kFALSE);

   Int_t nSteps = fhIntensProfile->GetNbinsX();

   // start from the second bin (0.05 - 0.10 intensity) since we don't care
   // about small intensity steps
   for (Int_t i=2; i<=hIntensProj->GetNbinsX(); i++) {
      //if (nTotal)
      //printf("proj: %f, %d\n", hIntensProj->GetBinContent(i), nSteps);
      if (hIntensProj->GetBinContent(i) >= 0.5*nSteps)
         return kMEASTYPE_FIXED;
   }

   return kMEASTYPE_SWEEP;
}


/** */
Double_t CnipolProfileHists::ProfileFitFunc(Double_t *x, Double_t *par)
{
   Double_t time = x[0];
   Double_t sigma = par[0];
   Double_t mean1 = par[1];
   Double_t mean2 = par[2];
   Double_t norm  = par[3];

   Double_t g1 = norm*TMath::Gaus(time, mean1, sigma);
   Double_t g2 = norm*TMath::Gaus(time, mean2, sigma);

   return g1 + g2;
}


/** */
Double_t CnipolProfileHists::ProfileTailFitFunc(Double_t *x, Double_t *par)
{
   Double_t time = x[0];

   Int_t    iBin    = fhIntensProfile->FindBin(time);
   Double_t crbRate = fhIntensProfile->GetBinContent(iBin);

   if (crbRate > 0.50) {
      TF1::RejectPoint();
      return 0;
   }

   return ProfileFitFunc(x, par);
}
