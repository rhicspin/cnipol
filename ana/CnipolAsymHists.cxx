/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolAsymHists.h"

#include "TF1.h"
#include "TFitResultPtr.h"
#include "TGraphErrors.h"

#include "AsymCalculator.h"
#include "MeasInfo.h"

#include "CnipolHists.h"

ClassImp(CnipolAsymHists)

using namespace std;

/** Default constructor. */
CnipolAsymHists::CnipolAsymHists() : DrawObjContainer()
{
   BookHists();
}


CnipolAsymHists::CnipolAsymHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
}


/** Default destructor. */
CnipolAsymHists::~CnipolAsymHists()
{
}


/** */
void CnipolAsymHists::BookHists()
{ //{{{
   string  shName;
   TH1    *hist;

   fDir->cd();

   // Spin vs Strip Id
   //shName = "hSpinVsChannel";
   //o[shName] = new TH2I(shName.c_str(), shName.c_str(), N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5, N_SPIN_STATES, -1.5, 1.5);
   //((TH1*) o[shName])->SetOption("colz NOIMG");
   //((TH1*) o[shName])->SetTitle(";Channel Id;Spin State;");

   shName = "hKinEnergyAChAsym";
   o[shName] = new TH1F(shName.c_str(), shName.c_str(), 25, 22.5, 1172.2);
   ((TH1*) o[shName])->SetOption("E1");
   ((TH1*) o[shName])->SetTitle("; Kinematic Energy, keV; Asymmetry;");

   Double_t xbins[8] = {-20, -5, -3, -1, +1, +3, +5, +20};

   shName = "hLongiChAsym";
   o[shName] = new TH1F(shName.c_str(), shName.c_str(), 7, xbins);
   //o[shName] = new TH1F(shName.c_str(), shName.c_str(), 40, -20, 20);
   ((TH1*) o[shName])->SetTitle("; Longi. Time Diff, ns; Asymmetry;");
   ((TH1*) o[shName])->SetOption("E1");

   // Channel Id vs bunch id
   shName = "hChVsBunchId";
   o[shName] = new TH2I(shName.c_str(), shName.c_str(), N_BUNCHES, 0.5, N_BUNCHES+0.5, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   ((TH1*) o[shName])->SetTitle("; Bunch Id; Channel Id;");
   ((TH1*) o[shName])->SetOption("colz NOIMG GRIDX");

   // Channel Id vs delim
   shName = "hChVsDelim";
   o[shName] = new TH2I(shName.c_str(), shName.c_str(), 1, 0, 1, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   ((TH1*) o[shName])->SetTitle("; Time, s; Channel Id;");
   ((TH1*) o[shName])->SetOption("colz NOIMG");

   // Detector Id vs bunch id
   shName = "hDetVsBunchId";
   o[shName] = new TH2I(shName.c_str(), shName.c_str(), N_BUNCHES, 0.5, N_BUNCHES+0.5, N_DETECTORS, 0.5, N_DETECTORS+0.5);
   ((TH1*) o[shName])->SetTitle("; Bunch Id; Detector Id;");
   ((TH1*) o[shName])->SetOption("colz NOIMG GRIDX");

   // Detector Id vs delim
   shName = "hDetVsDelim";
   o[shName] = new TH2I(shName.c_str(), shName.c_str(), 1, 0, 1, N_DETECTORS, 0.5, N_DETECTORS+0.5);
   ((TH1*) o[shName])->SetTitle("; Time, s; Detector Id;");
   ((TH1*) o[shName])->SetOption("colz NOIMG GRIDY");

   shName = "hAsymVsBunchId_X90";
   o[shName] = new TH2F(shName.c_str(), shName.c_str(), 1, 0.5, N_BUNCHES+0.5, 1, -0.05, 0.05);
   ((TH1*) o[shName])->SetTitle("; Bunch Id; X90 Asymmetry;");
   ((TH1*) o[shName])->SetOption("DUMMY GRIDX");

   shName = "hAsymVsBunchId_X45";
   o[shName] = new TH2F(shName.c_str(), shName.c_str(), 1, 0.5, N_BUNCHES+0.5, 1, -0.05, 0.05);
   ((TH1*) o[shName])->SetTitle("; Bunch Id; X45 Asymmetry;");
   ((TH1*) o[shName])->SetOption("DUMMY GRIDX");

   shName = "hAsymVsBunchId_Y45";
   o[shName] = new TH2F(shName.c_str(), shName.c_str(), 1, 0.5, N_BUNCHES+0.5, 1, -0.05, 0.05);
   ((TH1*) o[shName])->SetTitle("; Bunch Id; Y45 Asymmetry;");
   ((TH1*) o[shName])->SetOption("DUMMY GRIDX");

   //
   shName = "hChannelAsym";
   o[shName] = new TH1D(shName.c_str(), shName.c_str(), N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);//, 100, -0.03, 0.03);
   ((TH1*) o[shName])->SetTitle("; Channel Id; Asymmetry;");
   ((TH1*) o[shName])->SetOption("hist E1 NOIMG GRIDX");

   //
   shName = "hAsymVsDelim4Ch";
   o[shName] = new TH1D(shName.c_str(), shName.c_str(), 1, 0, 1);
   ((TH1*) o[shName])->SetTitle("; Time, s; Asymmetry;");
   ((TH1*) o[shName])->SetOption("E1 NOIMG");

   shName = "hAsymVsDelim4Det";
   hist = new TH1D(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle("; Time, s; Asymmetry;");
   hist->SetOption("E1 NOIMG");
   o[shName] = hist;

   //
   TGraphErrors *grAsymVsPhi = new TGraphErrors();
   grAsymVsPhi->SetName("grAsymVsPhi");
   grAsymVsPhi->SetMarkerStyle(kFullCircle);
   grAsymVsPhi->SetMarkerSize(1);
   grAsymVsPhi->SetMarkerColor(kMagenta+2);

   shName = "hAsymVsPhi";
   hist = new TH2C(shName.c_str(), shName.c_str(), 1, 0, 2*M_PI, 1, -0.025, 0.025);
   hist->SetTitle("; #phi; Asymmetry;");
   hist->SetOption("DUMMY GRIDX");
   hist->GetListOfFunctions()->Add(grAsymVsPhi, "p");
   o[shName] = hist;

   //
   TGraphErrors *grPolarVsPhi = new TGraphErrors();
   grPolarVsPhi->SetName("grPolarVsPhi");
   grPolarVsPhi->SetMarkerStyle(kFullCircle);
   grPolarVsPhi->SetMarkerSize(1);
   grPolarVsPhi->SetMarkerColor(kMagenta+2);

   shName = "hPolarVsPhi";
   o[shName] = new TH2C(shName.c_str(), shName.c_str(), 1, 0, 2*M_PI, 1, -110, 110);
   ((TH1*) o[shName])->SetTitle("; #phi; Polarization, %;");
   ((TH1*) o[shName])->SetOption("DUMMY GRIDX");
   ((TH1*) o[shName])->GetListOfFunctions()->Add(grPolarVsPhi, "p");

   //DrawObjContainer        *oc;
   //DrawObjContainerMapIter  isubdir;

   SpinStateSetIter iSS = gRunConfig.fSpinStates.begin();

   for (; iSS!=gRunConfig.fSpinStates.end(); ++iSS)
   {
      string sSS = gRunConfig.AsString(*iSS);

      Color_t color = RunConfig::AsColor(*iSS);

      // Create graphs for different spin states
      // X90
      shName = "grAsymVsBunchId_X90_" + sSS;
      TGraphErrors *grAsymVsBunchId_X90 = new TGraphErrors();
      grAsymVsBunchId_X90->SetName(shName.c_str());
      grAsymVsBunchId_X90->SetMarkerStyle(kFullCircle);
      grAsymVsBunchId_X90->SetMarkerSize(1);
      grAsymVsBunchId_X90->SetMarkerColor(color);

      // Add graphs to histos
      ((TH1*) o["hAsymVsBunchId_X90"])->GetListOfFunctions()->Add(grAsymVsBunchId_X90, "p");

      // X45
      shName = "grAsymVsBunchId_X45_" + sSS;
      TGraphErrors *grAsymVsBunchId_X45 = new TGraphErrors();
      grAsymVsBunchId_X45->SetName(shName.c_str());
      grAsymVsBunchId_X45->SetMarkerStyle(kFullCircle);
      grAsymVsBunchId_X45->SetMarkerSize(1);
      grAsymVsBunchId_X45->SetMarkerColor(color);

      // Add graphs to histo
      ((TH1*) o["hAsymVsBunchId_X45"])->GetListOfFunctions()->Add(grAsymVsBunchId_X45, "p");

      // Y45
      shName = "grAsymVsBunchId_Y45_" + sSS;
      TGraphErrors *grAsymVsBunchId_Y45 = new TGraphErrors();
      grAsymVsBunchId_Y45->SetName(shName.c_str());
      grAsymVsBunchId_Y45->SetMarkerStyle(kFullCircle);
      grAsymVsBunchId_Y45->SetMarkerSize(1);
      grAsymVsBunchId_Y45->SetMarkerColor(color);

      // Add graphs to histo
      ((TH1*) o["hAsymVsBunchId_Y45"])->GetListOfFunctions()->Add(grAsymVsBunchId_Y45, "p");

      // Counts per channel
      shName = "hChannelCounts_" + sSS;
      o[shName] = new TH1I(shName.c_str(), shName.c_str(), N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      ((TH1*) o[shName])->SetTitle("; Channel Id; Events;");
      ((TH1*) o[shName])->SetOption("hist NOIMG");

      // Channel Id vs bunch id
      shName = "hChVsBunchId_" + sSS;
      o[shName] = new TH2I(shName.c_str(), shName.c_str(), N_BUNCHES, 0.5, N_BUNCHES+0.5, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      ((TH1*) o[shName])->SetTitle("; Bunch Id; Channel Id;");
      ((TH1*) o[shName])->SetOption("colz");

      // Channel Id vs energy
      shName = "hChVsKinEnergyA_" + sSS;
      o[shName] = new TH2I(shName.c_str(), shName.c_str(), 25, 22.5, 1172.2, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      ((TH1*) o[shName])->SetTitle("; Kinematic Energy, keV; Channel Id;");
      ((TH1*) o[shName])->SetOption("colz NOIMG");

      // Channel Id vs energy
      shName = "hChVsLongiTimeDiff_" + sSS;
      o[shName] = new TH2I(shName.c_str(), shName.c_str(), 7, xbins, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      ((TH1*) o[shName])->SetTitle("; Time Diff, ns; Channel Id;");
      ((TH1*) o[shName])->SetOption("colz NOIMG");

      // Channel Id vs energy
      shName = "hChVsDelim_" + sSS;
      o[shName] = new TH2I(shName.c_str(), shName.c_str(), 1, 0, 1, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      ((TH1*) o[shName])->SetTitle("; Time, s; Channel Id;");
      ((TH1*) o[shName])->SetOption("colz");

      // Detector Id vs bunch id
      shName = "hDetVsBunchId_" + sSS;
      o[shName] = new TH2I(shName.c_str(), shName.c_str(), N_BUNCHES, 0.5, N_BUNCHES+0.5, N_DETECTORS, 0.5, N_DETECTORS+0.5);
      ((TH1*) o[shName])->SetTitle("; Bunch Id; Detector Id;");
      ((TH1*) o[shName])->SetOption("colz NOIMG");

      // Detector Id vs energy
      shName = "hDetVsKinEnergyA_" + sSS;
      o[shName] = new TH2I(shName.c_str(), shName.c_str(), 25, 22.5, 1172.2, N_DETECTORS, 0.5, N_DETECTORS+0.5);
      ((TH1*) o[shName])->SetTitle("; Kinematic Energy, keV; Detector Id;");
      ((TH1*) o[shName])->SetOption("colz NOIMG");

      // Detector Id vs delim
      shName = "hDetVsDelim_" + sSS;
      o[shName] = new TH2I(shName.c_str(), shName.c_str(), 1, 0, 1, N_DETECTORS, 0.5, N_DETECTORS+0.5);
      ((TH1*) o[shName])->SetTitle("; Time, s; Detector Id;");
      ((TH1*) o[shName])->SetOption("colz NOIMG");
   }
} //}}}


/** */
void CnipolAsymHists::PreFill()
{ //{{{
   ((TH1*) o["hChVsDelim"])->SetBins(gNDelimeters*10, 0, gNDelimeters, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   ((TH1*) o["hDetVsDelim"])->SetBins(gNDelimeters*10, 0, gNDelimeters, N_DETECTORS, 0.5, N_DETECTORS+0.5);
   ((TH1*) o["hAsymVsDelim4Ch"])->SetBins(gNDelimeters*10, 0, gNDelimeters);
   ((TH1*) o["hAsymVsDelim4Det"])->SetBins(gNDelimeters*10, 0, gNDelimeters);

   SpinStateSetIter iSS=gRunConfig.fSpinStates.begin();

   for (; iSS!=gRunConfig.fSpinStates.end(); ++iSS)
   {
      string sSS = gRunConfig.AsString(*iSS);

      ((TH1*) o["hChVsDelim_"  + sSS])->SetBins(gNDelimeters*10, 0, gNDelimeters, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      ((TH1*) o["hDetVsDelim_" + sSS])->SetBins(gNDelimeters*10, 0, gNDelimeters, N_DETECTORS, 0.5, N_DETECTORS+0.5);
   }
} //}}}


/** */
void CnipolAsymHists::Fill(ChannelEvent *ch)
{ //{{{
   UChar_t chId  = ch->GetChannelId();
   //UChar_t detId = ch->GetDetectorId();
   UChar_t bId   = ch->GetBunchId() + 1;

   //((TH1*) o["hDetVsBunchId"]) -> Fill(bId, detId);

   Float_t  kinEnergy = ch->GetKinEnergyAEDepend();
   //Float_t timeDiff  = ch->GetTdcAdcTimeDiff();
   Double_t ttime     = ((Double_t) ch->GetRevolutionId())/RHIC_REVOLUTION_FREQ;

   string sSS = gRunConfig.AsString( gMeasInfo->GetBunchSpin(bId) );

   ((TH1*) o.find("hChVsBunchId_"    + sSS)->second) -> Fill(bId, chId);
   ((TH1*) o.find("hChVsKinEnergyA_" + sSS)->second) -> Fill(kinEnergy, chId);
   ((TH1*) o.find("hChVsDelim_"      + sSS)->second) -> Fill(ttime, chId);
   //((TH1*) o["hChVsLongiTimeDiff_" + sSS]) -> Fill(timeDiff, chId);

} //}}}


/** */
void CnipolAsymHists::FillDerived()
{ //{{{
   Info("FillDerived()", "Called");

   // First fill integral and derivative histograms
   TH2* hChVsDelim    = (TH2*) o["hChVsDelim"];
   TH2* hChVsBunchId  = (TH2*) o["hChVsBunchId"];
   TH2* hDetVsBunchId = (TH2*) o["hDetVsBunchId"];
   TH2* hDetVsDelim   = (TH2*) o["hDetVsDelim"];

   SpinStateSetIter iSS = gRunConfig.fSpinStates.begin();

   for ( ; iSS!=gRunConfig.fSpinStates.end(); ++iSS)
   {
      string sSS = gRunConfig.AsString(*iSS);

      TH1* hChannelCounts_   = (TH1*) o["hChannelCounts_"   + sSS];
      TH2* hChVsBunchId_     = (TH2*) o["hChVsBunchId_"     + sSS];
      TH2* hChVsKinEnergyA_  = (TH2*) o["hChVsKinEnergyA_"  + sSS];
      TH2* hChVsDelim_       = (TH2*) o["hChVsDelim_"       + sSS];
      TH2* hDetVsBunchId_    = (TH2*) o["hDetVsBunchId_"    + sSS];
      TH2* hDetVsKinEnergyA_ = (TH2*) o["hDetVsKinEnergyA_" + sSS];
      TH2* hDetVsDelim_      = (TH2*) o["hDetVsDelim_"      + sSS];

      for (int iCh=1; iCh<=N_SILICON_CHANNELS; iCh++)
      {
         UInt_t counts = hChVsBunchId_->Integral(1, N_BUNCHES, iCh, iCh);

         hChannelCounts_->SetBinContent(iCh, counts);

         // Fill detector histograms from the channel ones
         // Skip disabled channels
         if (gMeasInfo->IsDisabledChannel(iCh)) continue;

         UShort_t iDet = RunConfig::GetDetectorId(iCh);

         for (int iBunch=1; iBunch<=hDetVsBunchId_->GetNbinsX(); iBunch++)
         {
            Double_t bc     = hChVsBunchId_->GetBinContent(iBunch, iCh);
            Double_t bc_det = hDetVsBunchId_->GetBinContent(iBunch, iDet);
            hDetVsBunchId_->SetBinContent(iBunch, iDet, bc_det + bc);
         }

         for (int iDelim=1; iDelim<=hDetVsDelim_->GetNbinsX(); iDelim++)
         {
            Double_t bc     = hChVsDelim_->GetBinContent(iDelim, iCh);
            Double_t bc_det = hDetVsDelim_->GetBinContent(iDelim, iDet);
            hDetVsDelim_->SetBinContent(iDelim, iDet, bc_det + bc);
         }

         for (int iKinE=1; iKinE<=hDetVsKinEnergyA_->GetNbinsX(); iKinE++)
         {
            Double_t bc     = hChVsKinEnergyA_->GetBinContent(iKinE, iCh);
            Double_t bc_det = hDetVsKinEnergyA_->GetBinContent(iKinE, iDet);
            hDetVsKinEnergyA_->SetBinContent(iKinE, iDet, bc_det + bc);
         }
      }

      hChVsDelim->Add(hChVsDelim_);
      hChVsBunchId->Add(hChVsBunchId_);
      hDetVsBunchId->Add(hDetVsBunchId_);
      hDetVsDelim->Add(hDetVsDelim_);
   }
} //}}}


/** */
void CnipolAsymHists::FillDerived(DrawObjContainer &oc)
{ //{{{
   Info("FillDerived(DrawObjContainer &oc)", "Called");

   CnipolHists *hists_std = (CnipolHists*) oc.d.find("std")->second;

   if (!hists_std) {
      Error("FillDerived(DrawObjContainer &oc)", "Histogram container \"std\" required");
      return;
   }

   SpinStateSetIter iSS = gRunConfig.fSpinStates.begin();

   for (; iSS!=gRunConfig.fSpinStates.end(); ++iSS) {

      string sSS = gRunConfig.AsString(*iSS);

      TH2* hChVsLongiTimeDiff_ = (TH2*) o["hChVsLongiTimeDiff_" + sSS];

      ChannelSetIter iCh = gMeasInfo->fSiliconChannels.begin();

      for (; iCh!=gMeasInfo->fSiliconChannels.end(); ++iCh) {

         string sChId("  ");
         sprintf(&sChId[0], "%02d", *iCh);

         DrawObjContainer *oc_ch = hists_std->d.find("channel" + sChId)->second;

         TH2* hLongiTimeDiffVsEnergyA_ch_ss = (TH2*) oc_ch->o["hLongiTimeDiffVsEnergyA_ch" + sChId + "_" + sSS];
         
         Int_t nBinsX = hLongiTimeDiffVsEnergyA_ch_ss->GetNbinsX();
         Int_t nBinsY = hLongiTimeDiffVsEnergyA_ch_ss->GetNbinsY();

         // Loop over time diff bins
         for (Int_t iby=1; iby<=nBinsY; iby++) {

            Double_t td = hLongiTimeDiffVsEnergyA_ch_ss->GetYaxis()->GetBinCenter(iby);
            Double_t nEvents_ch_ss = hLongiTimeDiffVsEnergyA_ch_ss->Integral(1, nBinsX, iby, iby);

            Int_t tdBin = hChVsLongiTimeDiff_->GetXaxis()->FindBin(td);
            
            Int_t gBin = hChVsLongiTimeDiff_->GetBin(tdBin, *iCh);
            //Int_t gBin = hChVsLongiTimeDiff_->GetBin(iby, *iCh);
            hChVsLongiTimeDiff_->AddBinContent(gBin, nEvents_ch_ss);
            hChVsLongiTimeDiff_->SetEntries(hChVsLongiTimeDiff_->GetEntries() + nEvents_ch_ss);
         }
      }
   }

   // Some histograms in this container depend on the 'std' container
   // Strip-by-Strip Asymmetries
   gAsymCalculator.CalcStripAsymmetry(this);
   gAsymCalculator.CalcStripAsymmetryByProfile(this);
   gAsymCalculator.CalcKinEnergyAChAsym(this);
   gAsymCalculator.CalcLongiChAsym(this);

   //printf("XXX %f\n", run.polarization);

   //gAsymCalculator.CalcBunchAsym(*hDetVsBunchId);
   gAsymCalculator.CalcBunchAsym(this);
   gAsymCalculator.CalcBunchAsymSqrtFormula(this);

   //gAsymCalculator.CalcDelimAsym(this);
   gAsymCalculator.CalcDelimAsymSqrtFormula(this);
} //}}}


/** */
void CnipolAsymHists::PostFill()
{ //{{{
   // Assume all required histograms are filled
   // Fit (or do whatever) them at this step

   TH2* hAsymVsBunchId_X90 = (TH2*) o["hAsymVsBunchId_X90"];
   TH2* hAsymVsBunchId_X45 = (TH2*) o["hAsymVsBunchId_X45"];
   TH2* hAsymVsBunchId_Y45 = (TH2*) o["hAsymVsBunchId_Y45"];

   SpinStateSetIter iSS = gRunConfig.fSpinStates.begin();
   
   for ( ; iSS!=gRunConfig.fSpinStates.end(); ++iSS)
   {
      string  sSS   = gRunConfig.AsString(*iSS);
      Color_t color = RunConfig::AsColor(*iSS);

      string grName;
      TGraphErrors *gr;
      TFitResultPtr fitres;

      //
      grName = "grAsymVsBunchId_X90_" + sSS;

      gr = (TGraphErrors*) hAsymVsBunchId_X90->GetListOfFunctions()->FindObject(grName.c_str());
      TF1 *funcConst= new TF1("funcConst", "[0]");
      funcConst->SetParNames("const");
      funcConst->SetLineColor(color);
      fitres = gr->Fit("funcConst", "S");
      gAnaMeasResult->fFitResAsymBunchX90[*iSS] = fitres;
      delete funcConst;

      //
      grName = "grAsymVsBunchId_X45_" + sSS;

      gr = (TGraphErrors*) hAsymVsBunchId_X45->GetListOfFunctions()->FindObject(grName.c_str());
      funcConst= new TF1("funcConst", "[0]");
      funcConst->SetParNames("const");
      funcConst->SetLineColor(color);
      fitres = gr->Fit("funcConst", "S");
      gAnaMeasResult->fFitResAsymBunchX45[*iSS] = fitres;
      delete funcConst;

      //
      grName = "grAsymVsBunchId_Y45_" + sSS;

      gr = (TGraphErrors*) hAsymVsBunchId_Y45->GetListOfFunctions()->FindObject(grName.c_str());
      funcConst= new TF1("funcConst", "[0]");
      funcConst->SetParNames("const");
      funcConst->SetLineColor(color);
      fitres = gr->Fit("funcConst", "S");
      gAnaMeasResult->fFitResAsymBunchY45[*iSS] = fitres;
      delete funcConst;
   }
} //}}}
