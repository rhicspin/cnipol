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
void CnipolAsymHists::BookHists(string cutid)
{ //{{{
   char    hName[256];
   string  shName;
   TH1    *hist;

   fDir->cd();

   // Spin vs Strip Id
   //sprintf(hName, "hSpinVsChannel%s", cutid.c_str());
   //o[hName] = new TH2I(hName, hName, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5, N_SPIN_STATES, -1.5, 1.5);
   //((TH1*) o[hName])->SetOption("colz NOIMG");
   //((TH1*) o[hName])->SetTitle(";Channel Id;Spin State;");

   sprintf(hName, "hKinEnergyAChAsym%s", cutid.c_str());
   o[hName] = new TH1F(hName, hName, 25, 22.5, 1172.2);
   ((TH1*) o[hName])->SetOption("E1");
   ((TH1*) o[hName])->SetTitle(";Kinematic Energy, keV;Asymmetry;");

   Double_t xbins[8] = {-20, -5, -3, -1, +1, +3, +5, +20};

   sprintf(hName, "hLongiChAsym%s", cutid.c_str());
   o[hName] = new TH1F(hName, hName, 7, xbins);
   //o[hName] = new TH1F(hName, hName, 40, -20, 20);
   ((TH1*) o[hName])->SetOption("E1");
   ((TH1*) o[hName])->SetTitle(";Longi. Time Diff, ns;Asymmetry;");

   // Channel Id vs bunch id
   shName = "hChVsBunchId";
   o[shName] = new TH2I(shName.c_str(), shName.c_str(), N_BUNCHES, 0.5, N_BUNCHES+0.5, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   ((TH1*) o[shName])->SetOption("colz NOIMG");
   ((TH1*) o[shName])->SetTitle(";Bunch Id;Channel Id;");

   // Channel Id vs delim
   shName = "hChVsDelim";
   o[shName] = new TH2I(shName.c_str(), shName.c_str(), 1, 0, 1, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   ((TH1*) o[shName])->SetOption("colz NOIMG");
   ((TH1*) o[shName])->SetTitle(";Target Steps, s;Channel Id;");

   // Detector Id vs bunch id
   shName = "hDetVsBunchId";
   o[shName] = new TH2I(shName.c_str(), shName.c_str(), N_BUNCHES, 0.5, N_BUNCHES+0.5, N_DETECTORS, 0.5, N_DETECTORS+0.5);
   ((TH1*) o[shName])->SetOption("colz NOIMG");
   ((TH1*) o[shName])->SetTitle(";Bunch Id;Detector Id;");

   // Detector Id vs delim
   shName = "hDetVsDelim";
   o[shName] = new TH2I(shName.c_str(), shName.c_str(), 1, 0, 1, N_DETECTORS, 0.5, N_DETECTORS+0.5);
   ((TH1*) o[shName])->SetOption("colz NOIMG");
   ((TH1*) o[shName])->SetTitle(";Target Steps, s;Detector Id;");

   shName = "hAsymVsBunchId_X90";
   o[shName] = new TH2F(shName.c_str(), shName.c_str(), N_BUNCHES, 0.5, N_BUNCHES+0.5, 100, -0.05, 0.05);
   //((TH1*) o[shName])->SetOption("NOIMG");
   ((TH1*) o[shName])->SetTitle(";Bunch Id;X90 Asymmetry;");

   shName = "hAsymVsBunchId_X45";
   o[shName] = new TH2F(shName.c_str(), shName.c_str(), N_BUNCHES, 0.5, N_BUNCHES+0.5, 100, -0.05, 0.05);
   //((TH1*) o[shName])->SetOption("NOIMG");
   ((TH1*) o[shName])->SetTitle(";Bunch Id;X45 Asymmetry;");

   shName = "hAsymVsBunchId_Y45";
   o[shName] = new TH2F(shName.c_str(), shName.c_str(), N_BUNCHES, 0.5, N_BUNCHES+0.5, 100, -0.05, 0.05);
   //((TH1*) o[shName])->SetOption("NOIMG");
   ((TH1*) o[shName])->SetTitle(";Bunch Id;Y45 Asymmetry;");

   //
   shName = "hChannelAsym";
   o[shName] = new TH1D(shName.c_str(), shName.c_str(), N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);//, 100, -0.03, 0.03);
   ((TH1*) o[shName])->SetOption("hist E1 NOIMG");
   ((TH1*) o[shName])->SetTitle(";Channel Id;Asymmetry;");

   //
   shName = "hAsymVsDelim4Ch";
   o[shName] = new TH1D(shName.c_str(), shName.c_str(), 1, 0, 1);
   ((TH1*) o[shName])->SetOption("hist NOIMG");
   ((TH1*) o[shName])->SetTitle(";Target Steps, s;Asymmetry;");

   shName = "hAsymVsDelim4Det";
   hist = new TH1D(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetOption("E1 NOIMG");
   hist->SetTitle(";Target Steps, s;Asymmetry;");
   o[shName] = hist;

   //
   TGraphErrors *grAsymVsPhi = new TGraphErrors();
   grAsymVsPhi->SetName("grAsymVsPhi");
   grAsymVsPhi->SetMarkerStyle(kFullCircle);
   grAsymVsPhi->SetMarkerSize(1);
   grAsymVsPhi->SetMarkerColor(kMagenta+2);

   shName = "hAsymVsPhi";
   o[shName] = new TH2F(shName.c_str(), shName.c_str(), 100, 0, 2*M_PI, 100, -0.03, 0.03);
   //((TH1*) o[shName])->SetOption("NOIMG");
   ((TH1*) o[shName])->SetTitle(";#phi;Asymmetry;");
   ((TH1*) o[shName])->GetListOfFunctions()->Add(grAsymVsPhi, "p");

   //
   TGraphErrors *grPolarVsPhi = new TGraphErrors();
   grPolarVsPhi->SetName("grPolarVsPhi");
   grPolarVsPhi->SetMarkerStyle(kFullCircle);
   grPolarVsPhi->SetMarkerSize(1);
   grPolarVsPhi->SetMarkerColor(kMagenta+2);

   shName = "hPolarVsPhi";
   o[shName] = new TH2F(shName.c_str(), shName.c_str(), 100, 0, 2*M_PI, 100, -1.5, 1.5);
   //((TH1*) o[shName])->SetOption("NOIMG");
   ((TH1*) o[shName])->SetTitle(";#phi;Asymmetry;");
   ((TH1*) o[shName])->GetListOfFunctions()->Add(grPolarVsPhi, "p");

   //DrawObjContainer        *oc;
   //DrawObjContainerMapIter  isubdir;

   IterSpinState iSS = gRunConfig.fSpinStates.begin();

   for (; iSS!=gRunConfig.fSpinStates.end(); ++iSS) {

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
      ((TH1*) o[shName])->SetOption("hist NOIMG");
      ((TH1*) o[shName])->SetTitle(";Channel Id;;");

      // Channel Id vs bunch id
      shName = "hChVsBunchId_" + sSS;
      o[shName] = new TH2I(shName.c_str(), shName.c_str(), N_BUNCHES, 0.5, N_BUNCHES+0.5, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      ((TH1*) o[shName])->SetOption("colz");
      ((TH1*) o[shName])->SetTitle(";Bunch Id;Channel Id;");

      // Channel Id vs energy
      shName = "hChVsKinEnergyA_" + sSS;
      o[shName] = new TH2I(shName.c_str(), shName.c_str(), 25, 22.5, 1172.2, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      ((TH1*) o[shName])->SetOption("colz NOIMG");
      ((TH1*) o[shName])->SetTitle(";Kinematic Energy, keV;Channel Id;");

      // Channel Id vs energy
      shName = "hChVsLongiTimeDiff_" + sSS;
      o[shName] = new TH2I(shName.c_str(), shName.c_str(), 7, xbins, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      ((TH1*) o[shName])->SetOption("colz NOIMG");
      ((TH1*) o[shName])->SetTitle(";Time Diff, ns;Channel Id;");

      // Channel Id vs energy
      shName = "hChVsDelim_" + sSS;
      o[shName] = new TH2I(shName.c_str(), shName.c_str(), 1, 0, 1, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      ((TH1*) o[shName])->SetOption("colz");
      ((TH1*) o[shName])->SetTitle(";Target Steps, s;Channel Id;");

      // Detector Id vs bunch id
      shName = "hDetVsBunchId_" + sSS;
      o[shName] = new TH2I(shName.c_str(), shName.c_str(), N_BUNCHES, 0.5, N_BUNCHES+0.5, N_DETECTORS, 0.5, N_DETECTORS+0.5);
      ((TH1*) o[shName])->SetOption("colz NOIMG");
      ((TH1*) o[shName])->SetTitle(";Bunch Id;Detector Id;");

      // Detector Id vs energy
      shName = "hDetVsKinEnergyA_" + sSS;
      o[shName] = new TH2I(shName.c_str(), shName.c_str(), 25, 22.5, 1172.2, N_DETECTORS, 0.5, N_DETECTORS+0.5);
      ((TH1*) o[shName])->SetOption("colz NOIMG");
      ((TH1*) o[shName])->SetTitle(";Kinematic Energy, keV;Detector Id;");

      // Detector Id vs delim
      shName = "hDetVsDelim_" + sSS;
      o[shName] = new TH2I(shName.c_str(), shName.c_str(), 1, 0, 1, N_DETECTORS, 0.5, N_DETECTORS+0.5);
      ((TH1*) o[shName])->SetOption("colz NOIMG");
      ((TH1*) o[shName])->SetTitle(";Target Steps, s;Detector Id;");
   }
} //}}}


/** */
void CnipolAsymHists::PreFill(string cutid)
{ //{{{
   ((TH1*) o["hChVsDelim"])->SetBins(gNDelimeters*10, 0, gNDelimeters, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   ((TH1*) o["hDetVsDelim"])->SetBins(gNDelimeters*10, 0, gNDelimeters, N_DETECTORS, 0.5, N_DETECTORS+0.5);
   ((TH1*) o["hAsymVsDelim4Ch"])->SetBins(gNDelimeters*10, 0, gNDelimeters);
   ((TH1*) o["hAsymVsDelim4Det"])->SetBins(gNDelimeters*10, 0, gNDelimeters);

   IterSpinState iSS=gRunConfig.fSpinStates.begin();

   for (; iSS!=gRunConfig.fSpinStates.end(); ++iSS)
   {
      string sSS = gRunConfig.AsString(*iSS);

      ((TH1*) o["hChVsDelim_"  + sSS])->SetBins(gNDelimeters*10, 0, gNDelimeters, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      ((TH1*) o["hDetVsDelim_" + sSS])->SetBins(gNDelimeters*10, 0, gNDelimeters, N_DETECTORS, 0.5, N_DETECTORS+0.5);
   }

} //}}}


/** */
void CnipolAsymHists::Fill(ChannelEvent *ch, string cutid)
{ //{{{
   UChar_t chId  = ch->GetChannelId();
   //UChar_t detId = ch->GetDetectorId();
   UChar_t bId   = ch->GetBunchId() + 1;

   //((TH1*) o["hDetVsBunchId"]) -> Fill(bId, detId);

   Float_t  kinEnergy = ch->GetKinEnergyAEDepend();
   //Float_t timeDiff  = ch->GetTdcAdcTimeDiff();
   Double_t ttime     = ((Double_t) ch->GetRevolutionId())/RHIC_REVOLUTION_FREQ;

   string sSS = gRunConfig.AsString( gMeasInfo->GetBunchSpin(bId) );

   ((TH1*) o.find("hChVsBunchId_"       + sSS)->second) -> Fill(bId, chId);
   ((TH1*) o.find("hChVsKinEnergyA_"    + sSS)->second) -> Fill(kinEnergy, chId);
   ((TH1*) o.find("hChVsDelim_"         + sSS)->second) -> Fill(ttime, chId);
   //((TH1*) o["hChVsLongiTimeDiff_" + sSS]) -> Fill(timeDiff, chId);

   //((TH1*) o["hDetVsKinEnergyA_" + sSS]) -> Fill(kinEnergy, detId);
   //((TH1*) o["hDetVsBunchId_"    + sSS]) -> Fill(bId, detId);
} //}}}


/** */
void CnipolAsymHists::FillDerived()
{ //{{{
   // First fill integral and derivative histograms
   TH2* hChVsDelim    = (TH2*) o["hChVsDelim"];
   TH2* hChVsBunchId  = (TH2*) o["hChVsBunchId"];
   TH2* hDetVsBunchId = (TH2*) o["hDetVsBunchId"];
   TH2* hDetVsDelim   = (TH2*) o["hDetVsDelim"];

   IterSpinState iSS = gRunConfig.fSpinStates.begin();

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

      for (int iCh=1; iCh<=N_SILICON_CHANNELS; iCh++) {

         UInt_t counts = hChVsBunchId_->Integral(1, N_BUNCHES, iCh, iCh);

         hChannelCounts_->SetBinContent(iCh, counts);
      
         // Fill detector count histograms from channel count ones
         UShort_t iDet = RunConfig::GetDetectorId(iCh);

         for (int iBunch=1; iBunch<=hDetVsBunchId_->GetNbinsX(); iBunch++)
         {
            Double_t bc     = hChVsBunchId_->GetBinContent(iBunch, iCh);
            //Int_t bin    = hDetVsBunchId_->GetBin(iBunch, iDet);
            //hDetVsBunchId_->AddBinContent(bin, bc);
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
            Double_t bc  = hChVsKinEnergyA_->GetBinContent(iKinE, iCh);
            //Int_t bin = hDetVsKinEnergyA_->GetBin(iKinE, iDet);
            //hDetVsKinEnergyA_->AddBinContent(bin, bc);
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
   CnipolHists *hists_std = (CnipolHists*) oc.d.find("std")->second;

   if (!hists_std) {
      Error("FillDerived(DrawObjContainer &oc)", "Histogram container \"std\" required");
      return;
   }

   IterSpinState iSS = gRunConfig.fSpinStates.begin();

   for (; iSS!=gRunConfig.fSpinStates.end(); ++iSS) {

      string sSS = gRunConfig.AsString(*iSS);

      TH2* hChVsLongiTimeDiff_ss = (TH2*) o["hChVsLongiTimeDiff_" + sSS];

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

            Int_t tdBin = hChVsLongiTimeDiff_ss->GetXaxis()->FindBin(td);
            
            Int_t gBin = hChVsLongiTimeDiff_ss->GetBin(tdBin, *iCh);
            //Int_t gBin = hChVsLongiTimeDiff_ss->GetBin(iby, *iCh);
            hChVsLongiTimeDiff_ss->AddBinContent(gBin, nEvents_ch_ss);
            hChVsLongiTimeDiff_ss->SetEntries(hChVsLongiTimeDiff_ss->GetEntries() + nEvents_ch_ss);
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

   IterSpinState iSS = gRunConfig.fSpinStates.begin();
   
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
