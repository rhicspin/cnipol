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

   // directory structure
   //FeedBack  = new TDirectoryFile("FeedBack", "FeedBack", "", fOutRootFile);   //fOutRootFile->mkdir("FeedBack");
   //Kinema    = new TDirectoryFile("Kinema", "Kinema", "", fOutRootFile);   //fOutRootFile->mkdir("Kinema");
   ////Bunch     = new TDirectoryFile("Bunch", "Bunch", "", fOutRootFile);   //fOutRootFile->mkdir("Bunch");
   //ErrDet    = new TDirectoryFile("ErrDet", "ErrDet", "", fOutRootFile);   //fOutRootFile->mkdir("ErrDet");
   //Asymmetry = new TDirectoryFile("Asymmetry", "Asymmetry", "", fOutRootFile);   //fOutRootFile->mkdir("Asymmetry");

   //BookHists();

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

   if (gAsymAnaInfo->HasCalibBit() && !gAsymAnaInfo->HasAlphaBit()) {
      dir = new TDirectoryFile("calib", "calib", "", fOutRootFile);
      fHists->d["calib"] = new CnipolCalibHists(dir);
   }

   if (gAsymAnaInfo->HasNormalBit()) {
      dir = new TDirectoryFile("std", "std", "", fOutRootFile);
      oc  = new CnipolHists(dir);
      fHists->d["std"] = oc;
      fHistCuts[kCUT_CARBON].insert(oc);

      //dir = new TDirectoryFile("std_eb", "std_eb", "", fOutRootFile);
      //oc  = new CnipolHists(dir);
      //fHists->d["std_eb"] = oc;
      //fHistCuts[kCUT_CARBON_EB].insert(oc);
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

      //dir = new TDirectoryFile("asym_eb", "asym_eb", "", fOutRootFile);
      //oc = new CnipolAsymHists(dir);
      //fHists->d["asym_eb"] = oc;
      //fHistCuts[kCUT_CARBON_EB].insert(oc);
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

   //fOutTreeFile->ls();
}


/**
 * (Deprecated)
 * Updates the default fEventConfig to the one taken from the DL_CALIB_RUN_NAME
 * file. If ALPHA_CALIB_RUN_NAME is another file then also updates alpha calib
 * constants from that file.
 */
void AsymRoot::UpdateRunConfig()
{
   AsymAnaInfo *anaInfo = fEventConfig->GetAnaInfo();

   // if not calib
   if ( !anaInfo->HasCalibBit() ) {

      string fname = anaInfo->GetDlCalibFile();
      Info("AsymRoot::UpdateRunConfig", "Reading MeasConfig object from file %s", fname.c_str());
      TFile *f = TFile::Open(fname.c_str());
      fEventConfig = (EventConfig*) f->FindObjectAny("measConfig");
      //delete f;

      if (!fEventConfig) {
         Error("AsymRoot::UpdateRunConfig", "No MeasConfig object found in file %s", fname.c_str());
         return;
      }

      // Update the pointer to MeasConfig object in the event
      delete fChannelEvent->fEventConfig;
      fChannelEvent->fEventConfig = fEventConfig;

   // else if not alpha mode what am I trying to do here?
   // else if ( !(anaInfo->fModes & (AnaInfo::MODE_ALPHA^AnaInfo::MODE_CALIB)) )
   } else if ( !anaInfo->HasAlphaBit() && anaInfo->HasNormalBit()) {
   // else if ( anaInfo->HasNormalBit())

      // Now, if alpha calib file is different update alpha constants from that
      // MeasConfig
      string fnameAlpha = anaInfo->GetAlphaCalibFile();

      // XXX not implemented. Need to fix it ASAP!
      //if (fnameAlpha != fname) {
         Info("AsymRoot::UpdateRunConfig", "Reading MeasConfig object from alpha calib file %s", fnameAlpha.c_str());

         TFile *f = TFile::Open(fnameAlpha.c_str());
         EventConfig *alphaRunConfig = (EventConfig*) f->FindObjectAny("EventConfig");
         //delete f;

         if (!alphaRunConfig) {
            Error("AsymRoot::UpdateRunConfig", "No MeasConfig object found in alpha calib file %s", fnameAlpha.c_str());
            return;
         }

         fEventConfig = alphaRunConfig;
         //fEventConfig->Print();

         // XXX not implemented. Need to fix it!
         // ....
      //}

      // Update the pointer to MeasConfig object in the event
      delete fChannelEvent->fEventConfig;
      fChannelEvent->fEventConfig = fEventConfig;
   }

   // Update the calibrator based on the running mode, i.e. alpha or
   // normal data
   UpdateCalibrator();
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
   //Info("FillPassOne", "Called");
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
   //fHists->PostFillPassOne(fHists);

   // The order is important!
   if ( fHists->d.find("alpha") != fHists->d.end() )
      fHists->d["alpha"]->PostFillPassOne(fHists);

   //DrawObjContainer *pulserHists = 0;
   //
   //if (gAsymAnaInfo->HasPulserBit()) {
   //   pulserHists = fHists->d["pulser"];
   //}
   //
   //if (gAsymAnaInfo->HasPmtBit()) {
   //   ((CnipolPmtHists*) fHists->d["pmt"])->PostFillPassOne();
   //}
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

   // Special processing for some of the histogram containers
   //if (gAsymAnaInfo->HasProfileBit()) {
   //   ((CnipolProfileHists*) fHists->d["profile"])->Process();
   //   fEventConfig->GetMeasInfo()->fMeasType = ((CnipolProfileHists*) fHists->d["profile"])->MeasurementType();
   //}

   // Add info to database entry
   run.profile_ratio       = gAnaMeasResult->fProfilePolarR.first;
   run.profile_ratio_error = gAnaMeasResult->fProfilePolarR.second;

   // Old way... should be deleted
   //run.polarization        = gAnaMeasResult->sinphi[0].P[0];
   //run.polarization_error  = gAnaMeasResult->sinphi[0].P[1],
   //run.phase               = gAnaMeasResult->sinphi[0].dPhi[0];
   //run.phase_error         = gAnaMeasResult->sinphi[0].dPhi[1];

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
      //fChannelEvents[fChannelEvent->fEventId] = *fChannelEvent;

      fChannelEvents.insert(*fChannelEvent);

      //int sizeb = fChannelEvents.size()*sizeof(ChannelEvent);
      int sizen = fChannelEvents.size();

      // a factor of 2 comes from some root overhead
      //if (sizeb > 100000000*2) {
      if (sizen >= 12000000) { // corresponds to 300Mb if all 3 trees are saved

         //printf("sizeb: %d\n", sizeb);
         printf("sizen: %d\n", sizen);

         //PrintEventMap();

         //fEventConfig->PrintAsPhp();
         //fEventConfig->Write("measConfig");
         SaveChannelTrees();
         SaveEventTree();

         fTreeFileId++;
         WriteTreeFile();
         fOutTreeFile->Close();
         fOutTreeFile->Delete();

         //fOutTreeFile  = 0;
         fRawEventTree = 0;
         fAnaEventTree = 0;

         fChannelEvents.clear();
         //fChannelEvent
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
   //fEventConfig->PrintAsPhp();
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
      //delete fAnaEventTree;
   }
}


/** */
void AsymRoot::PrintEventMap()
{
   ChannelEventSet::const_iterator mi;
   ChannelEventSet::const_iterator mb = fChannelEvents.begin();
   ChannelEventSet::const_iterator me = fChannelEvents.end();

   for (mi=mb; mi!=me; mi++) {
      //mi->first.Print();
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

   if ( anaInfo->HasAlphaBit() && !anaInfo->HasCalibBit() ) {

      Error("AsymRoot::UpdateCalibrator", "Alpha runs must be (self) calibrated to produce reasonable results");
      return;
   }
   else if ( anaInfo->HasAlphaBit() && anaInfo->HasCalibBit() ) {

      Info("AsymRoot::UpdateCalibrator", "Setting AlphaCalibrator");

      // Existing calibrator will be replaced so, delete the existing one first
      delete fEventConfig->fCalibrator;
      // and assign a new calibrator
      fEventConfig->fCalibrator = new AlphaCalibrator();
   }
   else if ( anaInfo->HasNormalBit() ) { // regular data runs

      if ( anaInfo->HasCalibBit() ) { // create new calibrator

         Info("AsymRoot::UpdateCalibrator", "Setting DeadLayerCalibrator");

         // Existing calibrator will be replaced so, delete it first
         delete fEventConfig->fCalibrator;
         // and finally, assign the new calibrator
         fEventConfig->fCalibrator =  new DeadLayerCalibratorEDepend();
         //fEventConfig->fCalibrator = new DeadLayerCalibrator();

      } else { // use calibration consts from a DL file

         string fname = anaInfo->GetDlCalibFile();

         if (fname.empty()) {
            Fatal("UpdateCalibrator", "Dead layer calibration file not specified. Use --calib option");
         }

         Info("AsymRoot::UpdateCalibrator", "Reading MeasConfig object from file %s", fname.c_str());
         TFile *f = TFile::Open(fname.c_str());

         if (!f) {
            Fatal("UpdateCalibrator", "File not found: %s", fname.c_str());
         }

         EventConfig* eventConfig = (EventConfig*) f->FindObjectAny("measConfig");

         if (!eventConfig) {
            Error("AsymRoot::UpdateCalibrator", "No MeasConfig object found in file %s", fname.c_str());
            return;
         }

         // Update the pointer to MeasConfig object in the event
         //delete fChannelEvent->fEventConfig;
         //fChannelEvent->fEventConfig = fEventConfig;

         // Copy all calibration consts from the file to the current one
         fEventConfig->fCalibrator->fChannelCalibs = eventConfig->fCalibrator->fChannelCalibs;

         delete eventConfig;
         delete f;
      }

      // Now overwrite alpha calibration constants from an alpha calib file
      string fnameAlpha = anaInfo->GetAlphaCalibFile();
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
      //if (streamerInfo) delete streamerInfo;

      Version_t    mcLoadedVer = TClassTable::GetID("EventConfig");
      EventConfig *eventConfig = (EventConfig*) f->FindObjectAny("measConfig");

      if (!eventConfig || mcVer != mcLoadedVer) {
         Fatal("UpdateCalibrator", "No MeasConfig object of known version %d found in alpha calib file %s", mcLoadedVer, fnameAlpha.c_str());
      }

      fEventConfig->fCalibrator->CopyAlphaCoefs(*eventConfig->fCalibrator);

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
      //mi->fChannel.Print();
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

      //mi->Print();

      fAnaEvent->fEventId = mi->fEventId;
      fAnaEvent->fChannels[mi->fEventId.fChannelId] = mi->fChannel;

      // Pointer to the next element, can be end of map
      nextmi = mi; nextmi++;

      //printf("check event ids\n");
      //fAnaEvent->fEventId.Print();
      //printf("\t");
      //nextmi->fEventId.Print();
      //printf("\n\n");

      if (fAnaEvent->fEventId < nextmi->fEventId || nextmi == me) {

         fAnaEventTree->Fill();
         //printf("Filled ana event\n");
         //if (fAnaEvent->fChannels.size() >= 2) {
         //   printf("XXX size: %d\n", fAnaEvent->fChannels.size());
         //}

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
      //energy_spectrum[i] = (TH1F*) fHists->d["Kinema"].o[hname];
   }

   sprintf(htitle,"%.3f : Energy Spectrum (All Detectors)", gMeasInfo->RUNID);
   energy_spectrum_all = new TH1F("energy_spectrum_all", htitle, Eslope.nxbin, Eslope.xmin, Eslope.xmax);
   energy_spectrum_all -> GetXaxis() -> SetTitle("Momentum Transfer [-GeV/c]^2");
   //energy_spectrum_all = (TH1F*) fHists->d["Kinema"].o["energy_spectrum_all"];

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

   //Asymmetry->cd();
   //asym_vs_bunch_x45    = new TH2F();
   //asym_vs_bunch_x90    = new TH2F();
   //asym_vs_bunch_y45    = new TH2F();
   //asym_sinphi_fit      = new TH2F();
   //scan_asym_sinphi_fit = new TH2F();
}


// Description : Book ROOT Functions and Histograms using Feedback infomations
//             : This routine shuould be called after Feedback operation
void AsymRoot::BookHists2(StructFeedBack &feedback)
{
   fOutRootFile->cd();
   Kinema->cd();

   char  formula[100], fname[100];
   float low, high, sigma;
   int   Color=2;
   int   Width=2;

   for (int i=0; i<NSTRIP; i++) {

      for (int j=0; j<2; j++) {

         sigma = j ? gRunConsts[i+1].M2T*feedback.RMS[i]*gAsymAnaInfo->MassSigmaAlt :
                     gRunConsts[i+1].M2T*feedback.RMS[i]*gAsymAnaInfo->MassSigma;

         int Style = j + 1 ;

         // lower limit
         sprintf(formula, "%f/sqrt(x)+(%f)/sqrt(x)", gRunConsts[i+1].E2T, sigma);
         sprintf(fname, "banana_cut_l_st%d_mode%d", i, j);
         banana_cut_l[i][j] = new TF1(fname, formula, gAsymAnaInfo->enel, gAsymAnaInfo->eneu);
         banana_cut_l[i][j] -> SetLineColor(Color);
         banana_cut_l[i][j] -> SetLineWidth(Width);
         banana_cut_l[i][j] -> SetLineStyle(Style);

         // upper limit
         sprintf(formula,"%f/sqrt(x)-(%f)/sqrt(x)", gRunConsts[i+1].E2T, sigma);
         sprintf(fname, "banana_cut_h_st%d", i);
         banana_cut_h[i][j] = new TF1(fname, formula, gAsymAnaInfo->enel, gAsymAnaInfo->eneu);
         banana_cut_h[i][j] -> SetLineColor(Color);
         banana_cut_h[i][j] -> SetLineWidth(Width);
         banana_cut_h[i][j] -> SetLineStyle(Style);
      }

      // energy cut low
      low  = gRunConsts[i+1].E2T / sqrt(double(gAsymAnaInfo->enel)) -
                 gRunConsts[i+1].M2T * feedback.RMS[i] * gAsymAnaInfo->MassSigma / sqrt(double(gAsymAnaInfo->enel));
      high = gRunConsts[i+1].E2T / sqrt(double(gAsymAnaInfo->enel)) +
                 gRunConsts[i+1].M2T * feedback.RMS[i] * gAsymAnaInfo->MassSigma / sqrt(double(gAsymAnaInfo->enel));

      energy_cut_l[i] = new TLine(gAsymAnaInfo->enel, low, gAsymAnaInfo->enel, high);
      energy_cut_l[i] ->SetLineColor(Color);
      energy_cut_l[i] ->SetLineWidth(Width);

      // energy cut high
      low  = gRunConsts[i+1].E2T / sqrt(double(gAsymAnaInfo->eneu)) -
                 gRunConsts[i+1].M2T * feedback.RMS[i] * gAsymAnaInfo->MassSigma / sqrt(double(gAsymAnaInfo->eneu));
      high = gRunConsts[i+1].E2T / sqrt(double(gAsymAnaInfo->eneu)) +
                 gRunConsts[i+1].M2T * feedback.RMS[i] * gAsymAnaInfo->MassSigma / sqrt(double(gAsymAnaInfo->eneu));

      energy_cut_h[i] = new TLine(gAsymAnaInfo->eneu, low, gAsymAnaInfo->eneu, high);
      energy_cut_h[i] ->SetLineColor(Color);
      energy_cut_h[i] ->SetLineWidth(Width);
   }
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
   //fOutRootFile->cd();
   //Kinema->cd();

   //for (int i=0; i<NSTRIP; i++) {
   //   if (t_vs_e[i]) {

   //      for (int j=0; j<2; j++){
   //         if (banana_cut_l[i][j]) t_vs_e[i]->GetListOfFunctions()->Add(banana_cut_l[i][j]);
   //         if (banana_cut_h[i][j]) t_vs_e[i]->GetListOfFunctions()->Add(banana_cut_h[i][j]);
   //      }

   //      if (energy_cut_l[i]) t_vs_e[i]->GetListOfFunctions()->Add(energy_cut_l[i]);
   //      if (energy_cut_h[i]) t_vs_e[i]->GetListOfFunctions()->Add(energy_cut_h[i]);
   //   }
   //}

   //fOutRootFile->cd();

   fHists->Write(); // this is NOT equivalent to fOutRootFile->Write();
   //fHists->Delete();

   //fOutRootFile->Write();

   fOutRootFile->cd();
   //fEventConfig->PrintAsPhp();
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
