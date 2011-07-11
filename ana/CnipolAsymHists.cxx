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

   fDir->cd();

   sprintf(hName, "hAsymVsKinEnergyA%s", cutid.c_str());
   o[hName] = new TH2F(hName, hName, 25, 22.5, 1172.2, 20, 0, 0.05);
   ((TH1*) o[hName])->SetOption("hist E1 NOIMG");
   ((TH1*) o[hName])->SetTitle(";Kinematic Energy, keV;Asymmetry;");

   //DrawObjContainer        *oc;
   //DrawObjContainerMapIter  isubdir;

   IterSpinState iSS=gRunConfig.fSpinStates.begin();

   for (; iSS!=gRunConfig.fSpinStates.end(); ++iSS) {

      //string sSS = RunConst::AsString();
      string sSS = gRunConfig.AsString(*iSS);
      //sprintf(&sSS[0], "%+2d", *iSS);

      //string sChId("  ");
      //sprintf(&sChId[0], "%02d", iss);

      //string dName = "channel" + sChId;

      //isubdir = d.find(dName);

      //if ( isubdir == d.end()) { // if dir not found
      //   oc = new DrawObjContainer();
      //   oc->fDir = new TDirectoryFile(dName.c_str(), dName.c_str(), "", fDir);
      //} else {
      //   oc = isubdir->second;
      //}

      // Channel Id vs energy
      sprintf(hName, "hChVsKinEnergyA_%s", sSS.c_str());
      o[hName] = new TH2I(hName, hName, 25, 22.5, 1172.2, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      ((TH1*) o[hName])->SetOption("colz NOIMG");
      ((TH1*) o[hName])->SetTitle(";Kinematic Energy, keV;Channel Id;");

      // Detector Id vs energy
      sprintf(hName, "hDetVsKinEnergyA_%s", sSS.c_str());
      o[hName] = new TH2I(hName, hName, 25, 22.5, 1172.2, N_DETECTORS, 0.5, N_DETECTORS+0.5);
      ((TH1*) o[hName])->SetOption("colz NOIMG");
      ((TH1*) o[hName])->SetTitle(";Kinematic Energy, keV;Detector Id;");

      // If this is a new directory then we need to add it to the list
      //if ( isubdir == d.end()) {
      //   d[dName] = oc;
      //}
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
   UChar_t bId   = ch->GetBunchId();

   Float_t kinEnergy = ch->GetKinEnergyAEDepend();

   string sSS = gRunConfig.AsString( (ESpinState) gSpinPattern[bId] );

   ((TH1*) o["hChVsKinEnergyA_"  + sSS]) -> Fill(kinEnergy, chId);
   ((TH1*) o["hDetVsKinEnergyA_" + sSS]) -> Fill(kinEnergy, detId);
} //}}}
