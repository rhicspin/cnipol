#include "AsymRoot.h"

#include "TClassTable.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TLine.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TStreamerInfo.h"

#include "AlphaCalibrator.h"
#include "CnipolAsymHists.h"
#include "CnipolAlphaHists.h"
#include "CnipolCalibHists.h"
#include "CnipolHists.h"
#include "CnipolKinematHists.h"
#include "CnipolKinemaCleanHists.h"
#include "CnipolPmtHists.h"
#include "CnipolPulserHists.h"
#include "CnipolPreprocHists.h"
#include "CnipolProfileHists.h"
#include "CnipolRawHists.h"
#include "CnipolRawExtendedHists.h"
#include "CnipolRunHists.h"
#include "CnipolScalerHists.h"
#include "CnipolSpinStudyHists.h"
#include "CnipolTargetHists.h"
#include "DeadLayerCalibrator.h"
#include "DeadLayerCalibratorEDepend.h"

#include "AsymAnaInfo.h"
#include "MeasInfo.h"


// ROOT Histograms
TDirectory *FeedBack;
TDirectory *Kinema;
TDirectory *ErrDet;
TDirectory *Asymmetry;

// FeedBack Dir
TH2F  *mdev_feedback;
TH1F  *mass_feedback_all;
TH1F  *mass_feedback[TOT_WFD_CH];  // invariant mass for feedback

// Kinema Direcotry
TH2F  *t_vs_e[TOT_WFD_CH];
TH2F  *t_vs_e_yescut[TOT_WFD_CH];
TH2F  *mass_vs_e_ecut[TOT_WFD_CH];  // Mass vs. 12C Kinetic Energy
TF1   *banana_cut_l[NSTRIP][2];     // banana cut low
TF1   *banana_cut_h[NSTRIP][2];     // banana cut high
TLine *energy_cut_l[NSTRIP];        // energy cut low
TLine *energy_cut_h[NSTRIP];        // energy cut high
TH1F  *energy_spectrum[N_DETECTORS];  // energy spectrum per detector
TH1F  *energy_spectrum_all;         // energy spectrum for all detector sum
TH1F  *mass_nocut_all;              // invariant mass without banana cut
TH1F  *mass_nocut[TOT_WFD_CH];      // invariant mass without banana cut
TH1F  *mass_yescut_all;
TH1F  *mass_yescut[TOT_WFD_CH];     // invariant mass with banana cut

// ErrDet Direcotry
TH2F  *mass_chi2_vs_strip;          // Chi2 of Gaussian Fit on Mass peak
TH2F  *mass_sigma_vs_strip;         // Mass sigma width vs. strip
TH2F  *mass_e_correlation_strip;    // Mass-energy correlation vs. strip
TH2F  *mass_pos_dev_vs_strip;       // Mass position deviation vs. strip
TH1I  *good_carbon_events_strip;    // number of good carbon events per strip
TH2F  *spelumi_vs_bunch;            // Counting rate vs. bunch
TH1F  *bunch_spelumi;               // Counting rate per bunch hisogram
TH1F  *asym_bunch_x45;              // Bunch asymmetry histogram for x45
TH1F  *asym_bunch_x90;              // Bunch asymmetry histogram for x90
TH1F  *asym_bunch_y45;              // Bunch asymmetry histogram for y45

// Asymmetry Directory
TH2F  *asym_vs_bunch_x45;      // Asymmetry vs. bunch (x45)
TH2F  *asym_vs_bunch_x90;      // Asymmetry vs. bunch (x90)
TH2F  *asym_vs_bunch_y45;      // Asymmetry vs. bunch (y45)
TH2F  *asym_sinphi_fit;        // strip asymmetry and sin(phi) fit
TH2F  *scan_asym_sinphi_fit;   // scan asymmetry and sin(phi) fit


using namespace std;


/** */
AsymRoot::AsymRoot() :
   fOutRootFile(), fOutTreeFile(), fTreeFileId(0),
   fRawEventTree(0), fAnaEventTree(0),
   fChannelEventTrees(), fAnaEvent(new AnaEvent()),
   fChannelEvent(new ChannelEvent()), fChannelData(new ChannelData()),
   fChannelEvents(), fEventConfig(new EventConfig()), fHists(0)
{
   gStyle->SetPalette(1);
   gStyle->SetOptFit(1111);
   gStyle->SetOptStat("emroui");
   gStyle->SetStatX(0.99);
   gStyle->SetStatY(0.90);
   gStyle->SetStatW(0.18);
   gStyle->SetStatH(0.15);
   gStyle->SetPadRightMargin(0.30);

   gStyle->SetStatBorderSize(1); // removes shadow

   fChannelEvent->fEventConfig = fEventConfig;
}


/** Default destructor. */
AsymRoot::~AsymRoot()
{
   if (!fOutRootFile)  { delete fOutRootFile;  fOutRootFile  = 0; }
   if (!fOutTreeFile)  { delete fOutTreeFile;  fOutTreeFile  = 0; }
   if (!fRawEventTree) { delete fRawEventTree; fRawEventTree = 0; }
   if (!fAnaEventTree) { delete fAnaEventTree; fAnaEventTree = 0; }

   delete fAnaEvent;     fAnaEvent     = 0;
   delete fChannelEvent; fChannelEvent = 0;
   delete fChannelData;  fChannelData  = 0;
   delete fEventConfig;  fEventConfig  = 0;

   if (!fHists) { delete fHists; fHists = 0; }
}


/**
 * Opens a ROOT file and creates a directory structure of histograms in that file.
 */
void AsymRoot::CreateRootFile(string filename)
{
   printf("Creating ROOT file: %s\n", filename.c_str());

   fOutRootFile = new TFile(filename.c_str(), "RECREATE", "AsymRoot Histogram file");

   if (!fOutRootFile) {
      Fatal("CreateRootFile", "Cannot open ROOT file %s", filename.c_str());
   }

   gSystem->Chmod(filename.c_str(), 0775);

   gROOT->SetMacroPath("./:~/rootmacros/:");
   gROOT->Macro("styles/style_asym.C");
   gROOT->ForceStyle(kTRUE);

   TDirectory       *dir;
   DrawObjContainer *oc;

   // Create default empty hist container
   fHists = new DrawObjContainer(fOutRootFile);

   if (gAsymAnaInfo->HasAlphaBit()) {
      dir = new TDirectoryFile("alpha", "alpha", "", fOutRootFile);
      oc  = new CnipolAlphaHists(dir);
      fHists->d["alpha"] = oc;
      fHistCuts[kCUT_PASSONE].insert(oc);
   }

   dir = new TDirectoryFile("calib", "calib", "", fOutRootFile);
   fHists->d["calib"] = new CnipolCalibHists(dir);

   if (gAsymAnaInfo->HasNormalBit()) {
      dir = new TDirectoryFile("std", "std", "", fOutRootFile);
      oc  = new CnipolHists(dir);
      fHists->d["std"] = oc;
      fHistCuts[kCUT_CARBON].insert(oc);
   }

   // If requested create scaler histograms and add them to the container
   if (gAsymAnaInfo->HasScalerBit()) {
      dir = new TDirectoryFile("scalers", "scalers", "", fOutRootFile);
      fHists->d["scalers"] = new CnipolScalerHists(dir);
   }

   if (gAsymAnaInfo->HasRawExtendedBit()) {
      dir = new TDirectoryFile("raw", "raw", "", fOutRootFile);
      oc  = new CnipolRawExtendedHists(dir);
      fHists->d["raw"] = oc;
      fHistCuts[kCUT_PASSONE].insert(oc);

      dir = new TDirectoryFile("raw_eb", "raw_eb", "", fOutRootFile);
      oc  = new CnipolRawExtendedHists(dir);
      fHists->d["raw_eb"] = oc;
      fHistCuts[kCUT_PASSONE_RAW_EB].insert(oc);

      dir = new TDirectoryFile("raw_neb", "raw_neb", "", fOutRootFile);
      oc  = new CnipolRawExtendedHists(dir);
      fHists->d["raw_neb"] = oc;
   }
   else if (gAsymAnaInfo->HasRawBit()) {
      dir = new TDirectoryFile("raw", "raw", "", fOutRootFile);
      oc  = new CnipolRawHists(dir);
      fHists->d["raw"] = oc;
      fHistCuts[kCUT_PASSONE].insert(oc);

      dir = new TDirectoryFile("raw_eb", "raw_eb", "", fOutRootFile);
      oc  = new CnipolRawHists(dir);
      fHists->d["raw_eb"] = oc;
      fHistCuts[kCUT_PASSONE_RAW_EB].insert(oc);

      dir = new TDirectoryFile("raw_neb", "raw_neb", "", fOutRootFile);
      oc  = new CnipolRawHists(dir);
      fHists->d["raw_neb"] = oc;
   }

   if (gAsymAnaInfo->HasTargetBit()) {
      dir = new TDirectoryFile("targets", "targets", "", fOutRootFile);
      fHists->d["targets"] = new CnipolTargetHists(dir);
   }

   if (gAsymAnaInfo->HasProfileBit()) {
      dir = new TDirectoryFile("profile", "profile", "", fOutRootFile);
      oc  = new CnipolProfileHists(dir);
      fHists->d["profile"] = oc;
      fHistCuts[kCUT_CARBON].insert(oc);
   }

   if (gAsymAnaInfo->HasAsymBit()) {
      dir = new TDirectoryFile("asym", "asym", "", fOutRootFile);
      oc  = new CnipolAsymHists(dir);
      fHists->d["asym"] = oc;
      fHistCuts[kCUT_CARBON].insert(oc);
   }

   if (gAsymAnaInfo->HasKinematBit()) {
      // Pre mass cut
      dir = new TDirectoryFile("kinema_premass", "kinema_premass", "", fOutRootFile);
      oc  = new CnipolKinematHists(dir);
      fHists->d["kinema_premass"] = oc;
      fHistCuts[kCUT_NOISE].insert(oc);

      // data after mass cut
      dir = new TDirectoryFile("kinema", "kinema", "", fOutRootFile);
      oc  = new CnipolKinemaCleanHists(dir);
      fHists->d["kinema"] = oc;
      fHistCuts[kCUT_CARBON].insert(oc);
   }

   if (gAsymAnaInfo->HasPmtBit()) {
      dir = new TDirectoryFile("pmt", "pmt", "", fOutRootFile);
      oc  = new CnipolPmtHists(dir);
      fHists->d["pmt"] = oc;
      fHistCuts[kCUT_PASSONE_PMT].insert(oc);
   }

   if (gAsymAnaInfo->HasPulserBit()) {
      dir = new TDirectoryFile("pulser", "pulser", "", fOutRootFile);
      oc  = new CnipolPulserHists(dir);
      fHists->d["pulser"] = oc;
      //fHistCuts[kCUT_PASSONE_PULSER].insert(oc);
      //fHistCuts[kCUT_CARBON_EB].insert(oc);
   }

   if (gAsymAnaInfo->HasStudiesBit()) {
      dir = new TDirectoryFile("studies", "studies", "", fOutRootFile);
      oc  = new CnipolSpinStudyHists(dir);
      fHists->d["studies"] = oc;
      fHistCuts[kCUT_CARBON].insert(oc);
   }

   // Should be reconsidered once preproc is used to fill raw hists for alpha runs
   if (!gAsymAnaInfo->HasAlphaBit()) {
      dir = new TDirectoryFile("run", "run", "", fOutRootFile);
      fHists->d["run"] = new CnipolRunHists(dir);

      dir = new TDirectoryFile("preproc", "preproc", "", fOutRootFile);
      oc  = new CnipolPreprocHists(dir);
      fHists->d["preproc"] = oc;
      //fHistCuts[kCUT_PASSONE_CALIB].insert(oc);
   }
}


/** */
void AsymRoot::CreateTrees()
{
   if (fTreeFileId > 99) {
      Fatal("CreateTrees", "fTreeFileId is too big");
   }

   string filename = gAsymAnaInfo->GetRootTreeFileName(fTreeFileId);

   fOutTreeFile = new TFile(filename.c_str(), "RECREATE", "AsymRoot Histogram file");

   // Create trees with raw data
   if (gAsymAnaInfo->fSaveTrees.test(0) ) {

      fRawEventTree = new TTree("RawEventTree", "Raw Event Tree");
      fRawEventTree->Branch("ChannelEvent", "ChannelEvent", &fChannelEvent);
   }

   // Create trees with channel events
   if (gAsymAnaInfo->fSaveTrees.test(1) ) {

      char tmpCharStr[19];

      for (int i=0; i!=NSTRIP; i++) {
         sprintf(tmpCharStr, "ChannelEventTree%02d", i);
         TTree *chEventTree = new TTree(tmpCharStr, "Channel Event Tree");
         chEventTree->Branch("ChannelData", "ChannelData", &fChannelData);
         fChannelEventTrees.push_back(chEventTree);
         //printf("size: %d\n", chEventTrees.size());
      }
   }

   // Create tree with time ordered events
   if (gAsymAnaInfo->fSaveTrees.test(2) ) {

      fAnaEventTree = new TTree("AnaEventTree", "Ana Event Tree");
      fAnaEventTree->Branch("AnaEvent", "AnaEvent", &fAnaEvent);
   }

}


/**
 * Sets current event with data from raw file.
 */
void AsymRoot::SetChannelEvent(ATStruct &at, long delim, unsigned chId)
{
   fChannelEvent->fEventId.fRevolutionId = delim*512 + at.rev*2 + at.rev0;
   fChannelEvent->fEventId.fBunchId      = at.b;
   fChannelEvent->fEventId.fChannelId    = chId;
   fChannelEvent->fEventId.fDelimiterId  = delim;
   fChannelEvent->fChannel.fAmpltd       = at.a;
   fChannelEvent->fChannel.fIntgrl       = at.s;
   fChannelEvent->fChannel.fTdc          = at.t;
   fChannelEvent->fChannel.fTdcAMax      = at.tmax;
}


/** */
void AsymRoot::AddSpinFlipperMarker()
{
   UInt_t revId = fChannelEvent->GetRevolutionId();
   fEventConfig->GetMeasInfo()->AddSpinFlipperMarker(revId);
}


/** */
void AsymRoot::FillPassOne(ECut cut)
{
   set<DrawObjContainer*> hists = fHistCuts[cut];

   set<DrawObjContainer*>::iterator hi;
   set<DrawObjContainer*>::iterator hb = hists.begin();
   set<DrawObjContainer*>::iterator he = hists.end();

   for (hi=hb; hi!=he; hi++) {
      (*hi)->FillPassOne(fChannelEvent);
   }
}


/** */
void AsymRoot::FillDerivedPassOne()
{
   Info("FillDerivedPassOne", "Called");
   fHists->FillDerivedPassOne();

   // The order is important!
   if ( fHists->d.find("raw_neb") != fHists->d.end() )
      fHists->d["raw_neb"]->FillDerivedPassOne(*fHists);

   if ( fHists->d.find("preproc") != fHists->d.end() )
      fHists->d["preproc"]->FillDerivedPassOne(*fHists);
}


/** */
void AsymRoot::PostFillPassOne()
{
   Info("PostFillPassOne", "Called");

   // The order is important!
   if ( fHists->d.find("alpha") != fHists->d.end() )
      fHists->d["alpha"]->PostFillPassOne(fHists);
}


/** */
void AsymRoot::PreFill()
{
   fHists->PreFill();
}


/** */
void AsymRoot::Fill(ECut cut)
{
   set<DrawObjContainer*> hists = fHistCuts[cut];

   set<DrawObjContainer*>::iterator hi;
   set<DrawObjContainer*>::iterator hb = hists.begin();
   set<DrawObjContainer*>::iterator he = hists.end();

   for (hi=hb; hi!=he; hi++) {
      (*hi)->Fill(fChannelEvent);
   }
}


/** */
void AsymRoot::FillDerived()
{
   Info("FillDerived", "Called");

   fHists->FillDerived();

   // Process dependencies
   // asym depends on std
   if (gAsymAnaInfo->HasAsymBit() && gAsymAnaInfo->HasNormalBit() ) {
      fHists->d["asym"]->FillDerived( *fHists );
   }

   // profile depends on asym
   if (gAsymAnaInfo->HasProfileBit() && gAsymAnaInfo->HasNormalBit() ) {
      fHists->d["profile"]->FillDerived( *fHists );
   }
}


/** */
void AsymRoot::PostFill(MseMeasInfoX &run)
{
   Info("PostFill", "Called");

   // One should be carefull here as the order of post processing is important.
   // Some histograms may depend on other histograms in independent containers
   // For example, 'profile' depends on 'asym'
   fHists->PostFill();

   // Add info to database entry
   run.profile_ratio       = gAnaMeasResult->fProfilePolarR.first;
   run.profile_ratio_error = gAnaMeasResult->fProfilePolarR.second;

   run.polarization        = gAnaMeasResult->GetPCPolar().first;
   run.polarization_error  = gAnaMeasResult->GetPCPolar().second;
   run.phase               = gAnaMeasResult->GetPCPolarPhase().first;
   run.phase_error         = gAnaMeasResult->GetPCPolarPhase().second;
}


/** */
void AsymRoot::FillScallerHists(int32_t *hData, UShort_t chId)
{
   if ( !gAsymAnaInfo->HasScalerBit() ) return;

   ((CnipolScalerHists*) fHists->d["scalers"])->Fill(hData, chId);
}


/** */
void AsymRoot::FillTargetHists(Int_t n, Double_t *hData)
{
   if (!gAsymAnaInfo->HasTargetBit()) return;

   ((CnipolTargetHists*) fHists->d["targets"])->Fill(n, hData);
}


/** */
void AsymRoot::FillProfileHists(UInt_t n, int32_t *hData)
{
   if (!gAsymAnaInfo->HasProfileBit()) return;

   ((CnipolProfileHists*) fHists->d["profile"])->Fill(n, hData);
}


/** */
void AsymRoot::FillRunHists()
{
   if (gAsymAnaInfo->HasAlphaBit()) return;

   ((CnipolRunHists*) fHists->d["run"])->Fill(*gMeasInfo);
}


/**
 *
 */
void AsymRoot::AddChannelEvent()
{
   if (gAsymAnaInfo->fSaveTrees.test(0))
      fRawEventTree->Fill();

   if (gAsymAnaInfo->fSaveTrees.test(1) || gAsymAnaInfo->fSaveTrees.test(2))
   {
      fChannelEvents.insert(*fChannelEvent);

      int sizen = fChannelEvents.size();

      if (sizen >= 12000000) { // corresponds to 300Mb if all 3 trees are saved

         printf("sizen: %d\n", sizen);

         SaveChannelTrees();
         SaveEventTree();

         fTreeFileId++;
         WriteTreeFile();
         fOutTreeFile->Close();
         fOutTreeFile->Delete();

         fRawEventTree = 0;
         fAnaEventTree = 0;

         fChannelEvents.clear();
         CreateTrees();
      }
   }
}


/**
 *
 */
void AsymRoot::WriteTreeFile()
{
   fOutTreeFile->cd();

   // Write run configuration object
   fEventConfig->Write("measConfig");

   if (fRawEventTree) {
      fRawEventTree->Write();
      fRawEventTree->Delete();
   }

   if (fChannelEventTrees.size()>0) {
      for (int i=0; i!=NSTRIP; i++) {
         if (fChannelEventTrees[i]->GetEntries() > 0)
            fChannelEventTrees[i]->Write();
         fChannelEventTrees[i]->Delete();
      }
      fChannelEventTrees.clear();
   }

   if (fAnaEventTree) {
      fAnaEventTree->Write();
      fAnaEventTree->Delete();
   }
}


/** */
void AsymRoot::PrintEventMap()
{
   ChannelEventSet::const_iterator mi;
   ChannelEventSet::const_iterator mb = fChannelEvents.begin();
   ChannelEventSet::const_iterator me = fChannelEvents.end();

   for (mi=mb; mi!=me; mi++) {
      mi->Print();
   }
}


/** */
void AsymRoot::PrintChannelEvent()
{
   fChannelEvent->Print();
}


/** */
void AsymRoot::UpdateCalibrator()
{
   AsymAnaInfo *anaInfo = fEventConfig->GetAnaInfo();

   if (anaInfo->HasAlphaBit())
   {
      Info("AsymRoot::UpdateCalibrator", "Setting AlphaCalibrator");

      // Existing calibrator will be replaced so, delete the existing one first
      delete fEventConfig->fCalibrator;
      // and assign a new calibrator
      fEventConfig->fCalibrator = new AlphaCalibrator();
   }
   else if (anaInfo->HasNormalBit())
   {
      Info("AsymRoot::UpdateCalibrator", "Setting DeadLayerCalibrator");

      // Existing calibrator will be replaced so, delete it first
      delete fEventConfig->fCalibrator;
      // and finally, assign the new calibrator
      fEventConfig->fCalibrator =  new DeadLayerCalibratorEDepend();

      // Now overwrite alpha calibration constants from an alpha calib file
      string fnameAlpha = anaInfo->GetAlphaCalibFile();
      cout << "alpha filename: " << fnameAlpha << endl;
      string frunNameTest  = anaInfo->GetRunName();
      cout << "run file name: " << frunNameTest << endl;
      Info("AsymRoot::UpdateCalibrator", "Reading MeasConfig object from alpha calib file %s", fnameAlpha.c_str());

      TFile *f = TFile::Open(fnameAlpha.c_str());

      if (!f) {
         Fatal("UpdateCalibrator", "File not found: %s", fnameAlpha.c_str());
      }

      TList *streamerList = f->GetStreamerInfoList();

      if (!streamerList) {
         Fatal("UpdateCalibrator", "No MeasConfig object found in alpha calib file %s", fnameAlpha.c_str());
      }

      TStreamerInfo *streamerInfo = (TStreamerInfo*) streamerList->FindObject("EventConfig"); // this is actually class name
      Int_t mcVer = 0;

      if (!streamerInfo) {
         Error("UpdateCalibrator", "No TStreamerInfo for MeasConfig found in alpha calib file %s", fnameAlpha.c_str());
      } else {
         mcVer = streamerInfo->GetClassVersion();
      }

      delete streamerList;

      Version_t    mcLoadedVer = TClassTable::GetID("EventConfig");
      EventConfig *eventConfig = (EventConfig*) f->FindObjectAny("measConfig");

      if (!eventConfig || mcVer != mcLoadedVer) {
         Fatal("UpdateCalibrator", "No MeasConfig object of known version %d found in alpha calib file %s", mcLoadedVer, fnameAlpha.c_str());
      }

      fEventConfig->fCalibrator->CopyAlphaCoefs(*eventConfig->fCalibrator);

      if (gMeasInfo->IsRunYear(2013)) {
         fEventConfig->fCalibrator->ApplyRun13BiasCurrentCorrection(gMeasInfo, true);
      } else {
         // For all other years we just use plain alpha gains
         fEventConfig->fCalibrator->UsePlainAlphaGain();
      }

      delete eventConfig;
      delete f;

   } else {
      Fatal("UpdateCalibrator", "Cannot select calibrator for this kind of run");
   }
}


void AsymRoot::UpdateFromChannelEvent()
{
   UInt_t revId = fChannelEvent->GetRevolutionId();
   fEventConfig->GetMeasInfo()->UpdateRevolutions( revId );
}


/** */
const Calibrator* AsymRoot::GetCalibrator() const { return fEventConfig->GetCalibrator(); }


/** */
void AsymRoot::Calibrate()
{
   fEventConfig->fCalibrator->Calibrate(fHists);

   AsymAnaInfo *anaInfo = fEventConfig->GetAnaInfo();
   if (gMeasInfo->IsRunYear(2013) && anaInfo->HasAlphaBit()) {
      fEventConfig->fCalibrator->ApplyRun13BiasCurrentCorrection(gMeasInfo, false);
   }
}


/** */
void AsymRoot::SaveChannelTrees()
{
   if (!gAsymAnaInfo->fSaveTrees.test(1)) return;

   if (fChannelEvents.size() <= 0) {
      printf("No channels to save in ChannelTree\n");
      return;
   }

   ChannelEventSet::iterator mi;
   ChannelEventSet::iterator mb = fChannelEvents.begin();
   ChannelEventSet::iterator me = fChannelEvents.end();

   for (mi=mb; mi!=me; mi++) {
      *fChannelData = (mi->fChannel);
      fChannelEventTrees[mi->fEventId.fChannelId]->Fill();
   }
}


/** */
void AsymRoot::SaveEventTree()
{
   if (!gAsymAnaInfo->fSaveTrees.test(2)) return;

   if (fChannelEvents.size() <= 0) {
      printf("No channels to save in EventTree\n");
      return;
   }

   ChannelEventSet::iterator mi;
   ChannelEventSet::iterator mb = fChannelEvents.begin();
   ChannelEventSet::iterator me = fChannelEvents.end();
   ChannelEventSet::iterator nextmi;

   for (mi=mb; mi!=me; mi++) {

      fAnaEvent->fEventId = mi->fEventId;
      fAnaEvent->fChannels[mi->fEventId.fChannelId] = mi->fChannel;

      // Pointer to the next element, can be end of map
      nextmi = mi; nextmi++;

      if (fAnaEvent->fEventId < nextmi->fEventId || nextmi == me) {

         fAnaEventTree->Fill();
         fAnaEvent->fChannels.clear();
      }
   }
}


/**
 * (Deprecated) Book AsymRoot histograms.
 */
void AsymRoot::BookHists()
{
   Char_t hname[100], htitle[100];

   fOutRootFile->cd();

   // FeedBack Directory
   FeedBack->cd();

   sprintf(hname, "mass_feedback_all");
   sprintf(htitle, "%.3f : Invariant Mass (feedback) for all strips", gMeasInfo->RUNID);
   mass_feedback_all = new TH1F(hname, htitle, 100, 0, 20);
   mass_feedback_all -> GetXaxis() -> SetTitle("Mass [GeV/c^2]");
   mass_feedback_all -> SetLineColor(2);

   for (int i=0; i<TOT_WFD_CH; i++) {

      sprintf(hname, "mass_feedback_st%d", i+1);
      sprintf(htitle, "%.3f : Invariant Mass (feedback) for Strip-%d ", gMeasInfo->RUNID, i+1);
      mass_feedback[i] = new TH1F(hname, htitle, 100, 0, 20);
      mass_feedback[i] -> GetXaxis() -> SetTitle("Mass [GeV/c^2]");
      mass_feedback[i] -> SetLineColor(2);
   }

   Kinema->cd();

   // 1-dim Energy Spectrum
   Eslope.nxbin=100; Eslope.xmin=0; Eslope.xmax=0.03;

   for (int i=0; i<N_DETECTORS; i++) {
      sprintf(hname, "energy_spectrum_det%d", i+1);
      sprintf(htitle,"%.3f : Energy Spectrum Detector %d ", gMeasInfo->RUNID, i+1);
      energy_spectrum[i] = new TH1F(hname,htitle, Eslope.nxbin, Eslope.xmin, Eslope.xmax);
      energy_spectrum[i]->GetXaxis()->SetTitle("Momentum Transfer [-GeV/c]^2");
   }

   sprintf(htitle,"%.3f : Energy Spectrum (All Detectors)", gMeasInfo->RUNID);
   energy_spectrum_all = new TH1F("energy_spectrum_all", htitle, Eslope.nxbin, Eslope.xmin, Eslope.xmax);
   energy_spectrum_all -> GetXaxis() -> SetTitle("Momentum Transfer [-GeV/c]^2");

   sprintf(hname,"mass_nocut_all");
   sprintf(htitle,"%.3f : Invariant Mass (nocut) for all strips", gMeasInfo->RUNID);
   mass_nocut_all = new TH1F(hname, htitle, 100, 0, 20);
   mass_nocut_all->GetXaxis()->SetTitle("Mass [GeV/c^2]");

   sprintf(hname,"mass_yescut_all");
   sprintf(htitle,"%.3f : Invariant Mass (w/cut) for all strips", gMeasInfo->RUNID);
   mass_yescut_all = new TH1F(hname, htitle, 100, 0, 20);
   mass_yescut_all->GetXaxis()->SetTitle("Mass [GeV/c^2]");
   mass_yescut_all->SetLineColor(2);

   // Need to book for TOT_WFD_CH instead of NSTRIP to avoid seg. fault by filling histograms by
   // target events strip [73 - 76].
   for (int i=0; i<TOT_WFD_CH; i++) {

      sprintf(hname,"t_vs_e_yescut_st%d",i+1);
      sprintf(htitle,"%.3f : t vs. Kin.Energy (with cut) Strip-%d ", gMeasInfo->RUNID, i+1);
      t_vs_e_yescut[i] = new TH2F(hname,htitle, 50, 200, 1500, 100, 20, 90);
      t_vs_e_yescut[i] -> GetXaxis() -> SetTitle("Kinetic Energy [keV]");
      t_vs_e_yescut[i] -> GetYaxis() -> SetTitle("Time of Flight [ns]");

      sprintf(hname,"mass_vs_e_ecut_st%d",i+1);
      sprintf(htitle,"%.3f : Mass vs. Kin.Energy (Energy Cut) Strip-%d ", gMeasInfo->RUNID, i+1);
      mass_vs_e_ecut[i] = new TH2F(hname,htitle, 50, 200, 1000, 200, 6, 18);
      mass_vs_e_ecut[i] -> GetXaxis() -> SetTitle("Kinetic Energy [keV]");
      mass_vs_e_ecut[i] -> GetYaxis() -> SetTitle("Invariant Mass [GeV]");

      sprintf(hname, "mass_nocut_st%d",i+1);
      sprintf(htitle,"%.3f : Invariant Mass (nocut) for Strip-%d ", gMeasInfo->RUNID, i+1);
      mass_nocut[i] = new TH1F(hname, htitle, 100, 0, 20);
      mass_nocut[i] -> GetXaxis() -> SetTitle("Mass [GeV/c^2]");

      sprintf(hname, "mass_yescut_st%d",i+1);
      sprintf(htitle,"%.3f : Invariant Mass (w/cut) for Strip-%d ", gMeasInfo->RUNID, i+1);
      mass_yescut[i] = new TH1F(hname, htitle, 100, 0, 20);
      mass_yescut[i] -> GetXaxis() -> SetTitle("Mass [GeV/c^2]");
      mass_yescut[i] -> SetLineColor(2);
   }

   // Error detectors
   ErrDet->cd();
   sprintf(htitle,"%.3f : Bunch Asymmetry X90", gMeasInfo->RUNID);
   asym_bunch_x90 = new TH1F("asym_bunch_x90", htitle, 100, -0.1, 0.1);

   sprintf(htitle,"%.3f : Bunch Asymmetry X45", gMeasInfo->RUNID);
   asym_bunch_x45 = new TH1F("asym_bunch_x45", htitle, 100, -0.1, 0.1);

   sprintf(htitle,"%.3f : Bunch Asymmetry Y45", gMeasInfo->RUNID);
   asym_bunch_y45 = new TH1F("asym_bunch_y45", htitle, 100, -0.1, 0.1);

   sprintf(htitle,"%.3f : # of Events in Banana Cut per strip", gMeasInfo->RUNID);
   good_carbon_events_strip = new TH1I("good_carbon_events_strip", htitle, NSTRIP, 0.5, NSTRIP+0.5);
   good_carbon_events_strip->SetFillColor(17);
}


// Deprecated
// Description : Delete Unnecessary Histograms
void AsymRoot::DeleteHistogram()
{
  // Delete histograms declared for WFD channel 72 - 75 to avoid crash. These channcles
  // are for target channels and thus thes histograms wouldn't make any sense.
  for (int i=NSTRIP; i<TOT_WFD_CH; i++ ) {
     //t_vs_e[i]->Delete();
     t_vs_e_yescut[i]->Delete();
     mass_vs_e_ecut[i]->Delete();  // Mass vs. 12C Kinetic Energy
     mass_nocut[i]->Delete();
     mass_yescut[i]->Delete();
  }
}


// Description : Write out objects in memory and dump in fOutRootFile before closing it
void AsymRoot::Finalize()
{
   fHists->Write(); // this is NOT equivalent to fOutRootFile->Write();

   fOutRootFile->cd();
   fEventConfig->Write("measConfig");

   // close fOutRootFile
   fOutRootFile->Close();

   if (gAsymAnaInfo->fSaveTrees.any()) {
      SaveChannelTrees();
      SaveEventTree();
      WriteTreeFile();
      fOutTreeFile->Close();
      fOutTreeFile->Delete();
   }
}


/** */
void AsymRoot::SaveAs(string pattern, string dir)
{
   if (!gAsymAnaInfo->HasAlphaBit()) {
      gStyle->SetMarkerStyle(kFullDotLarge);
      gStyle->SetMarkerSize(1);
      gStyle->SetMarkerColor(kRed);
   }

   TCanvas canvas("canvas", "canvas", 1200, 600);
   canvas.UseCurrentStyle();

   fHists->SetSignature(fEventConfig->GetSignature());

   fHists->SaveAllAs(DrawObjContainer::FORMAT_PNG, canvas, pattern, dir.c_str());

   if (gAsymAnaInfo->fFlagCreateThumbs) {
      TCanvas canvas("canvas", "canvas", 200, 100);
      fHists->SaveAllAs(DrawObjContainer::FORMAT_PNG, canvas, pattern, dir.c_str(), kTRUE);
   }
}


/** */
const EventConfig* AsymRoot::GetMeasConfig() const { return fEventConfig; }


/** */
void AsymRoot::GetMeasConfigs(MeasInfo *&ri, AsymAnaInfo *&ai, AnaMeasResult *&ar)
{
   if ( !fEventConfig ) {
      Error("GetMeasConfigs", "fEventConfig is not defined");
      ri = 0; ai = 0; ar = 0;
      return;
   }

   ri = fEventConfig->GetMeasInfo();
   ai = fEventConfig->GetAnaInfo();
   ar = fEventConfig->GetAnaMeasResult();
}
