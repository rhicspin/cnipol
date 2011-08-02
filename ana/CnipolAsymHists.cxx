/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolAsymHists.h"

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

   sprintf(hName, "hAsymVsKinEnergyA%s", cutid.c_str());
   o[hName] = new TH2F(hName, hName, 25, 22.5, 1172.2, 20, 0, 0.05);
   ((TH1*) o[hName])->SetOption("hist E1 NOIMG");
   ((TH1*) o[hName])->SetTitle(";Kinematic Energy, keV;Asymmetry;");

   // Detector Id vs bunch id
   shName = "hDetVsBunchId";
   o[shName] = new TH2I(shName.c_str(), shName.c_str(), N_BUNCHES, 0.5, N_BUNCHES+0.5, N_DETECTORS, 0.5, N_DETECTORS+0.5);
   ((TH1*) o[shName])->SetOption("colz NOIMG");
   ((TH1*) o[shName])->SetTitle(";Bunch Id;Detector Id;");

   shName = "hAsymVsBunch_X90" + cutid;
   o[shName] = new TH2F(shName.c_str(), shName.c_str(), N_BUNCHES, 0.5, N_BUNCHES+0.5, 100, -0.1, 0.1);
   //((TH1*) o[shName])->SetOption("hist E1 NOIMG");
   ((TH1*) o[shName])->SetTitle(";Bunch Id;X90 Asymmetry;");

   //DrawObjContainer        *oc;
   //DrawObjContainerMapIter  isubdir;

   IterSpinState iSS=gRunConfig.fSpinStates.begin();

   for (; iSS!=gRunConfig.fSpinStates.end(); ++iSS) {

      string sSS = gRunConfig.AsString(*iSS);

      Color_t color = RunConfig::AsColor(*iSS);

      // Create graphs for different spin states
      shName = "grAsymVsBunch_X90" + sSS;
      TGraphErrors *grAsymVsBunch_X90 = new TGraphErrors();
      grAsymVsBunch_X90->SetName(shName.c_str());
      grAsymVsBunch_X90->SetMarkerStyle(kFullCircle);
      grAsymVsBunch_X90->SetMarkerSize(1);
      grAsymVsBunch_X90->SetMarkerColor(color);

      // Add graphs to histos
      ((TH1*) o["hAsymVsBunch_X90"])->GetListOfFunctions()->Add(grAsymVsBunch_X90);

      // Channel Id vs energy
      shName = "hChVsKinEnergyA_" + sSS;
      o[shName] = new TH2I(shName.c_str(), shName.c_str(), 25, 22.5, 1172.2, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      ((TH1*) o[shName])->SetOption("colz NOIMG");
      ((TH1*) o[shName])->SetTitle(";Kinematic Energy, keV;Channel Id;");

      // Detector Id vs energy
      shName = "hDetVsKinEnergyA_" + sSS;
      o[shName] = new TH2I(shName.c_str(), shName.c_str(), 25, 22.5, 1172.2, N_DETECTORS, 0.5, N_DETECTORS+0.5);
      ((TH1*) o[shName])->SetOption("colz NOIMG");
      ((TH1*) o[shName])->SetTitle(";Kinematic Energy, keV;Detector Id;");

      // Detector Id vs bunch id
      shName = "hDetVsBunchId_" + sSS;
      o[shName] = new TH2I(shName.c_str(), shName.c_str(), N_BUNCHES, 0.5, N_BUNCHES+0.5, N_DETECTORS, 0.5, N_DETECTORS+0.5);
      ((TH1*) o[shName])->SetOption("colz NOIMG");
      ((TH1*) o[shName])->SetTitle(";Bunch Id;Detector Id;");
   }

   //for (int iDet=1; iDet<=N_DETECTORS; iDet++) {

   //   string sChId("  ");
   //   sprintf(&sChId[0], "%02d", iDet);

   //   string dName = "channel" + sChId;

   //   isubdir = d.find(dName);

   //   if ( isubdir == d.end()) { // if dir not found
   //      oc = new DrawObjContainer();
   //      oc->fDir = new TDirectoryFile(dName.c_str(), dName.c_str(), "", fDir);
   //   } else {
   //      oc = isubdir->second;
   //   }

   //   //sprintf(hName, "hTvsA_ch%02d", iDet);
   //   //oc->o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
   //   //((TH1*) oc->o[hName])->SetOption("colz LOGZ");
   //   //((TH1*) oc->o[hName])->SetTitle(";Amplitude, ADC;TDC;");

   //   // If this is a new directory then we need to add it to the list
   //   if ( isubdir == d.end()) {
   //      d[dName] = oc;
   //   }
   //}
} //}}}


/** */
void CnipolAsymHists::Fill(ChannelEvent *ch, string cutid)
{ //{{{
   if (cutid != "_cut2") return;

   UChar_t chId  = ch->GetChannelId();
   UChar_t detId = ch->GetDetectorId();
   UChar_t bId   = ch->GetBunchId() + 1;

   ((TH1*) o["hDetVsBunchId"]) -> Fill(bId, detId);

   Float_t kinEnergy = ch->GetKinEnergyAEDepend();

   string sSS = gRunConfig.AsString( (ESpinState) gSpinPattern[bId] );

   ((TH1*) o["hChVsKinEnergyA_"  + sSS]) -> Fill(kinEnergy, chId);
   ((TH1*) o["hDetVsKinEnergyA_" + sSS]) -> Fill(kinEnergy, detId);
   ((TH1*) o["hDetVsBunchId_"    + sSS]) -> Fill(bId, detId);
} //}}}
