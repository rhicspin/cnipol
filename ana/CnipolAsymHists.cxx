/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolAsymHists.h"

#include "TF1.h"
#include "TFitResultPtr.h"
#include "TGraphErrors.h"

#include "AsymCalculator.h"
#include "RunInfo.h"

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
   char hName[256];
   string shName;

   fDir->cd();

   // Spin vs Strip Id
   sprintf(hName, "hSpinVsChannel%s", cutid.c_str());
   o[hName] = new TH2I(hName, hName, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5, N_SPIN_STATES, -1.5, 1.5);
   ((TH1*) o[hName])->SetOption("colz NOIMG");
   ((TH1*) o[hName])->SetTitle(";Channel Id;Spin State;");

   sprintf(hName, "hKinEnergyAChAsym%s", cutid.c_str());
   o[hName] = new TH1F(hName, hName, 25, 22.5, 1172.2);
   ((TH1*) o[hName])->SetOption("E1");
   ((TH1*) o[hName])->SetTitle(";Kinematic Energy, keV;Asymmetry;");

   Float_t xbins[4] = {-20, -2, +2, +20};
   sprintf(hName, "hLongiChAsym%s", cutid.c_str());
   o[hName] = new TH1F(hName, hName, 3, xbins);
   ((TH1*) o[hName])->SetOption("E1");
   ((TH1*) o[hName])->SetTitle(";Longi. Time Diff, ns;Asymmetry;");

   // Detector Id vs bunch id
   shName = "hDetVsBunchId";
   o[shName] = new TH2I(shName.c_str(), shName.c_str(), N_BUNCHES, 0.5, N_BUNCHES+0.5, N_DETECTORS, 0.5, N_DETECTORS+0.5);
   ((TH1*) o[shName])->SetOption("colz NOIMG");
   ((TH1*) o[shName])->SetTitle(";Bunch Id;Detector Id;");

   shName = "hAsymVsBunchId_X90" + cutid;
   o[shName] = new TH2F(shName.c_str(), shName.c_str(), N_BUNCHES, 0.5, N_BUNCHES+0.5, 100, -0.05, 0.05);
   //((TH1*) o[shName])->SetOption("NOIMG");
   ((TH1*) o[shName])->SetTitle(";Bunch Id;X90 Asymmetry;");

   shName = "hAsymVsBunchId_X45" + cutid;
   o[shName] = new TH2F(shName.c_str(), shName.c_str(), N_BUNCHES, 0.5, N_BUNCHES+0.5, 100, -0.05, 0.05);
   //((TH1*) o[shName])->SetOption("NOIMG");
   ((TH1*) o[shName])->SetTitle(";Bunch Id;X45 Asymmetry;");

   shName = "hAsymVsBunchId_Y45" + cutid;
   o[shName] = new TH2F(shName.c_str(), shName.c_str(), N_BUNCHES, 0.5, N_BUNCHES+0.5, 100, -0.05, 0.05);
   //((TH1*) o[shName])->SetOption("NOIMG");
   ((TH1*) o[shName])->SetTitle(";Bunch Id;Y45 Asymmetry;");

   //
   shName = "hChannelAsym" + cutid;
   o[shName] = new TH1D(shName.c_str(), shName.c_str(), N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);//, 100, -0.03, 0.03);
   ((TH1*) o[shName])->SetOption("hist E1 NOIMG");
   ((TH1*) o[shName])->SetTitle(";Channel Id;Asymmetry;");

   //
   shName = "hDelimChAsym" + cutid;
   o[shName] = new TH1D(shName.c_str(), shName.c_str(), 1, 0, 1);
   ((TH1*) o[shName])->SetOption("hist NOIMG");
   ((TH1*) o[shName])->SetTitle(";Target Steps, s;Asymmetry;");

   //
   TGraphErrors *grAsymVsPhi = new TGraphErrors();
   grAsymVsPhi->SetName("grAsymVsPhi");
   grAsymVsPhi->SetMarkerStyle(kFullCircle);
   grAsymVsPhi->SetMarkerSize(1);
   grAsymVsPhi->SetMarkerColor(kMagenta+2);

   shName = "hAsymVsPhi" + cutid;
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

   shName = "hPolarVsPhi" + cutid;
   o[shName] = new TH2F(shName.c_str(), shName.c_str(), 100, 0, 2*M_PI, 100, -1.5, 1.5);
   //((TH1*) o[shName])->SetOption("NOIMG");
   ((TH1*) o[shName])->SetTitle(";#phi;Asymmetry;");
   ((TH1*) o[shName])->GetListOfFunctions()->Add(grPolarVsPhi, "p");

   //DrawObjContainer        *oc;
   //DrawObjContainerMapIter  isubdir;

   IterSpinState iSS=gRunConfig.fSpinStates.begin();

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
      ((TH1*) o[shName])->SetOption("colz NOIMG");
      ((TH1*) o[shName])->SetTitle(";Bunch Id;Channel Id;");

      // Channel Id vs energy
      shName = "hChVsKinEnergyA_" + sSS;
      o[shName] = new TH2I(shName.c_str(), shName.c_str(), 25, 22.5, 1172.2, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      ((TH1*) o[shName])->SetOption("colz NOIMG");
      ((TH1*) o[shName])->SetTitle(";Kinematic Energy, keV;Channel Id;");

      // Channel Id vs energy
      Double_t xbins[4] = {-20, -2, +2, +20};
      shName = "hChVsLongiTimeDiff_" + sSS;
      o[shName] = new TH2I(shName.c_str(), shName.c_str(), 3, xbins, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      ((TH1*) o[shName])->SetOption("colz NOIMG");
      ((TH1*) o[shName])->SetTitle(";Time Diff, ns;Channel Id;");

      // Channel Id vs energy
      shName = "hChVsDelim_" + sSS;
      o[shName] = new TH2I(shName.c_str(), shName.c_str(), 1, 0, 1, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      ((TH1*) o[shName])->SetOption("colz NOIMG");
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
   }
} //}}}


/** */
void CnipolAsymHists::PreFill(string cutid)
{ //{{{
   //if (cutid != "") return;

   ((TH1*) o["hDelimChAsym"])->SetBins(gNDelimeters, 0, gNDelimeters);

   IterSpinState iSS=gRunConfig.fSpinStates.begin();

   for (; iSS!=gRunConfig.fSpinStates.end(); ++iSS) {

      string sSS = gRunConfig.AsString(*iSS);

      ((TH1*) o["hChVsDelim_" + sSS])->SetBins(gNDelimeters, 0, gNDelimeters, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   }

} //}}}


/** */
void CnipolAsymHists::Fill(ChannelEvent *ch, string cutid)
{ //{{{
   UChar_t chId  = ch->GetChannelId();
   //UChar_t detId = ch->GetDetectorId();
   UChar_t bId   = ch->GetBunchId() + 1;

   //((TH1*) o["hDetVsBunchId"]) -> Fill(bId, detId);

   Float_t kinEnergy = ch->GetKinEnergyAEDepend();
   Float_t timeDiff  = ch->GetTdcAdcTimeDiff();
   UInt_t  ttime     = ch->GetRevolutionId()/RHIC_REVOLUTION_FREQ;

   string sSS = gRunConfig.AsString( gRunInfo->GetBunchSpin(bId) );

   ((TH1*) o["hChVsBunchId_"       + sSS]) -> Fill(bId, chId);
   ((TH1*) o["hChVsKinEnergyA_"    + sSS]) -> Fill(kinEnergy, chId);
   ((TH1*) o["hChVsLongiTimeDiff_" + sSS]) -> Fill(timeDiff, chId);
   ((TH1*) o["hChVsDelim_"         + sSS]) -> Fill(ttime, chId);

   //((TH1*) o["hDetVsKinEnergyA_" + sSS]) -> Fill(kinEnergy, detId);
   //((TH1*) o["hDetVsBunchId_"    + sSS]) -> Fill(bId, detId);
} //}}}


/** */
void CnipolAsymHists::FillDerived()
{ //{{{
   // First fill integral and derivative histograms
   TH2* hDetVsBunchId = (TH2*) o["hDetVsBunchId"];

   IterSpinState iSS = gRunConfig.fSpinStates.begin();

   for ( ; iSS!=gRunConfig.fSpinStates.end(); ++iSS)
   {
      string sSS = gRunConfig.AsString(*iSS);

      TH1* hChannelCounts   = (TH1*) o["hChannelCounts_"   + sSS];
      TH2* hChVsBunchId     = (TH2*) o["hChVsBunchId_"     + sSS];
      TH2* hChVsKinEnergyA  = (TH2*) o["hChVsKinEnergyA_"  + sSS];
      TH2* hChVsDelim       = (TH2*) o["hChVsDelim_"       + sSS];
      TH2* hDetVsBunchId_ss = (TH2*) o["hDetVsBunchId_"    + sSS];
      TH2* hDetVsKinEnergyA = (TH2*) o["hDetVsKinEnergyA_" + sSS];

      for (int iCh=1; iCh<=N_SILICON_CHANNELS; iCh++) {

         UInt_t counts = hChVsBunchId->Integral(1, N_BUNCHES, iCh, iCh);

         hChannelCounts->SetBinContent(iCh, counts);
      
         UShort_t iDet = RunConfig::GetDetectorId(iCh);

         for (int iBunch=1; iBunch<=hDetVsBunchId_ss->GetNbinsX(); iBunch++)
         {
            Double_t bc     = hChVsBunchId->GetBinContent(iBunch, iCh);
            //Int_t bin    = hDetVsBunchId_ss->GetBin(iBunch, iDet);
            //hDetVsBunchId_ss->AddBinContent(bin, bc);
            Double_t bc_det = hDetVsBunchId_ss->GetBinContent(iBunch, iDet);
            hDetVsBunchId_ss->SetBinContent(iBunch, iDet, bc_det + bc);
         }

         for (int iKinE=1; iKinE<=hDetVsKinEnergyA->GetNbinsX(); iKinE++)
         {
            Double_t bc  = hChVsKinEnergyA->GetBinContent(iKinE, iCh);
            //Int_t bin = hDetVsKinEnergyA->GetBin(iKinE, iDet);
            //hDetVsKinEnergyA->AddBinContent(bin, bc);
            Double_t bc_det = hDetVsKinEnergyA->GetBinContent(iKinE, iDet);
            hDetVsKinEnergyA->SetBinContent(iKinE, iDet, bc_det + bc);
         }
      }

      hDetVsBunchId->Add(hDetVsBunchId_ss);
   }
} //}}}


/** */
void CnipolAsymHists::PostFill()
{ //{{{

   // Strip-by-Strip Asymmetries
   gAsymCalculator.CalcStripAsymmetry(this);
   gAsymCalculator.CalcStripAsymmetryByProfile(this);
   gAsymCalculator.CalcKinEnergyAChAsym(this);
   gAsymCalculator.CalcLongiChAsym(this);

   //printf("XXX %f\n", run.polarization);

   //gAsymCalculator.CalcBunchAsym(*hDetVsBunchId);
   gAsymCalculator.CalcBunchAsym(this);
   gAsymCalculator.CalcBunchAsymSqrtFormula(this);

   // Assume all required histograms are filled
   // Fit (or do whatever) them at this step

   TH2* hAsymVsBunchId_X90 = (TH2*) o["hAsymVsBunchId_X90"];
   TH2* hAsymVsBunchId_X45 = (TH2*) o["hAsymVsBunchId_X45"];
   TH2* hAsymVsBunchId_Y45 = (TH2*) o["hAsymVsBunchId_Y45"];

   IterSpinState iSS = gRunConfig.fSpinStates.begin();
   
   for ( ; iSS!=gRunConfig.fSpinStates.end(); ++iSS)
   {
      string sSS = gRunConfig.AsString(*iSS);
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
      gAnaResult->fFitResAsymBunchX90[*iSS] = fitres;

      //
      grName = "grAsymVsBunchId_X45_" + sSS;

      gr = (TGraphErrors*) hAsymVsBunchId_X45->GetListOfFunctions()->FindObject(grName.c_str());
      funcConst= new TF1("funcConst", "[0]");
      funcConst->SetParNames("const");
      funcConst->SetLineColor(color);
      fitres = gr->Fit("funcConst", "S");
      gAnaResult->fFitResAsymBunchX45[*iSS] = fitres;

      //
      grName = "grAsymVsBunchId_Y45_" + sSS;

      gr = (TGraphErrors*) hAsymVsBunchId_Y45->GetListOfFunctions()->FindObject(grName.c_str());
      funcConst= new TF1("funcConst", "[0]");
      funcConst->SetParNames("const");
      funcConst->SetLineColor(color);
      fitres = gr->Fit("funcConst", "S");
      gAnaResult->fFitResAsymBunchY45[*iSS] = fitres;
   }
} //}}}
