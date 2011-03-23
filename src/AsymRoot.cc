//  Asymmetry Analysis of RHIC pC Polarimeter
//  file name :   AsymRoot.cc
//
//  Authors   :   I. Nakagawa
//                Dmitri Smirnov
//
//  Creation  :   7/11/2006
//

/**
 *
 * Oct 18, 2010 - Dmitri Smirnov
 *    - Added SaveChannelTrees() and SaveEventTree(), PrintEventMap()
 *
 */


#include "AsymRoot.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TLine.h"
#include "TStyle.h"

#include "AlphaCalibrator.h"
#include "CnipolCalibHists.h"
#include "CnipolHists.h"
#include "CnipolRawHists.h"
#include "CnipolRunHists.h"
#include "CnipolScalerHists.h"
#include "CnipolTargetHists.h"
#include "CnipolProfileHists.h"
#include "DeadLayerCalibrator.h"
#include "DeadLayerCalibratorEDepend.h"

#include "AnaInfo.h"
#include "RunInfo.h"

using namespace std;

// ROOT Histograms
TDirectory *FeedBack;
TDirectory *Kinema;
TDirectory *Bunch;
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
TH1F  *energy_spectrum[NDETECTOR];  // energy spectrum per detector
TH1F  *energy_spectrum_all;         // energy spectrum for all detector sum
TH1F  *mass_nocut_all;              // invariant mass without banana cut
TH1F  *mass_nocut[TOT_WFD_CH];      // invariant mass without banana cut
TH1F  *mass_yescut_all;
TH1F  *mass_yescut[TOT_WFD_CH];     // invariant mass with banana cut

// Bunch Distribution
TH1F  *bunch_dist;                  // counts per bunch
TH1F  *wall_current_monitor;        // wall current monitor
TH1F  *specific_luminosity;         // specific luminosity

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


/**
 *
 */
AsymRoot::AsymRoot() : rootfile(), fOutTreeFile(), fTreeFileId(0),
   fRawEventTree(0), fAnaEventTree(0),
   fChannelEventTrees(), fAnaEvent(new AnaEvent()),
   fChannelEvent(new ChannelEvent()), fChannelData(new ChannelData()),
   fChannelEvents(), fEventConfig(), fHists(0)
{
   gStyle->SetPalette(1);
   gStyle->SetOptFit(1111);
   gStyle->SetOptStat("emroui");
   gStyle->SetStatX(0.99);
   gStyle->SetStatY(0.99);
   gStyle->SetStatW(0.15);
   gStyle->SetStatH(0.15);

   gStyle->SetPadRightMargin(0.30);
}


/** Default destructor. */
AsymRoot::~AsymRoot()
{
   //delete fRawEventTree;
}


// Description : Open Root File and define directory structure of histograms
void AsymRoot::RootFile(string filename)
{ //{{{
   printf("Creating ROOT file: %s\n", filename.c_str());

   rootfile = new TFile(filename.c_str(), "RECREATE", "AsymRoot Histogram file");

   if (!rootfile) {
      Fatal("RootFile", "Cannot open ROOT file %s", filename.c_str());
      exit(-1);
   }

   gSystem->Chmod(filename.c_str(), 0775);

   // directory structure
   FeedBack  = new TDirectoryFile("FeedBack", "FeedBack", "", rootfile);   //rootfile->mkdir("FeedBack");
   Kinema    = new TDirectoryFile("Kinema", "Kinema", "", rootfile);   //rootfile->mkdir("Kinema");
   Bunch     = new TDirectoryFile("Bunch", "Bunch", "", rootfile);   //rootfile->mkdir("Bunch");
   ErrDet    = new TDirectoryFile("ErrDet", "ErrDet", "", rootfile);   //rootfile->mkdir("ErrDet");
   Asymmetry = new TDirectoryFile("Asymmetry", "Asymmetry", "", rootfile);   //rootfile->mkdir("Asymmetry");

   BookHists();

   // Create default empty hist container
   fHists = new DrawObjContainer(rootfile);

   if (gAnaInfo.HasAlphaBit()) {
      //DrawObjContainer *hists = new CnipolCalibHists(rootfile);
      //fHists->Add(hists);
      //delete hists;
      TDirectory *dir = new TDirectoryFile("alpha", "alpha", "", rootfile);
      fHists->d["alpha"] = new CnipolCalibHists(dir);
   }

   if (gAnaInfo.HasNormalBit()) {
      //DrawObjContainer *hists = new CnipolHists(rootfile);
      //fHists->Add(hists);
      //delete hists;
      TDirectory *dir = new TDirectoryFile("std", "std", "", rootfile);
      fHists->d["std"] = new CnipolHists(dir);
   }

   // If requested create scaler histograms and add them to the container
   if (gAnaInfo.HasScalerBit()) {
      //DrawObjContainer *hists = new CnipolScalerHists(rootfile);
      //fHists->Add(hists);
      //delete hists;
      TDirectory *dir = new TDirectoryFile("scalers", "scalers", "", rootfile);
      fHists->d["scalers"] = new CnipolScalerHists(dir);
   }

   if (gAnaInfo.HasRawBit()) {
      TDirectory *dir = new TDirectoryFile("raw", "raw", "", rootfile);
      fHists->d["raw"] = new CnipolRawHists(dir);
   }

   if (gAnaInfo.HasTargetBit()) {
      TDirectory *dir = new TDirectoryFile("targets", "targets", "", rootfile);
      fHists->d["targets"] = new CnipolTargetHists(dir);
   }

   if (gAnaInfo.HasProfileBit()) {
      TDirectory *dir = new TDirectoryFile("profile", "profile", "", rootfile);
      fHists->d["profile"] = new CnipolProfileHists(dir);
   }

   // OLD common global histogram inherited from previous life
   //DrawObjContainer *hists = new CnipolRunHists(rootfile);
   //fHists->Add(hists);
   //delete hists;

   //if (!gAnaInfo.HasAlphaBit()) {
      TDirectory *dir = new TDirectoryFile("run", "run", "", rootfile);
      fHists->d["run"] = new CnipolRunHists(dir);
   //}

   // a temporary fix...
   //Kinema    = fHists->d["Kinema"].fDir;
} //}}}


/** */
void AsymRoot::CreateTrees()
{
   if (fTreeFileId > 99) {
      Fatal("CreateTrees", "fTreeFileId is too big");
      exit(-1);
   }

   string filename = gAnaInfo.GetRootTreeFileName(fTreeFileId);

   fOutTreeFile = new TFile(filename.c_str(), "RECREATE", "AsymRoot Histogram file");

   // Create trees with raw data
   if (gAnaInfo.SAVETREES.test(0) ) {

      fRawEventTree = new TTree("RawEventTree", "Raw Event Tree");
      fRawEventTree->Branch("ChannelEvent", "ChannelEvent", &fChannelEvent);
   }

   // Create trees with channel events
   if (gAnaInfo.SAVETREES.test(1) ) {

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
   if (gAnaInfo.SAVETREES.test(2) ) {

      fAnaEventTree = new TTree("AnaEventTree", "Ana Event Tree");
      fAnaEventTree->Branch("AnaEvent", "AnaEvent", &fAnaEvent);
   }

   //fOutTreeFile->ls();
}


/** */
Bool_t AsymRoot::UseCalibFile(std::string cfname)
{ //{{{
   if (cfname == "" && gAnaInfo.CMODE) {

      UpdateCalibrator();
      return true; // check if config is already set

   } else if (cfname == "" && fEventConfig) {
      return true; // check if config is already set

   } else if (cfname != "") {

      TFile *f = TFile::Open(cfname.c_str());

      fEventConfig = (EventConfig*) f->FindObjectAny("EventConfig");

      if (fEventConfig) {

         //fEventConfig->fRunDB->alpha_calib_run_name = fEventConfig->fRunInfo->runName;
         gRunDb.alpha_calib_run_name = fEventConfig->fRunInfo->runName;
         //fEventConfig->fDatproc->CMODE = 0;

         delete fChannelEvent->fEventConfig;

         fChannelEvent->fEventConfig = fEventConfig;

         // Update the calibrator based on the running mode, i.e. alpha or
         // normal data
         UpdateCalibrator();

         return true;

      } else return false;

   } else return false;
} //}}}


/**
 * Updates the deafult fEventConfig to the one taken from the DL_CALIB_RUN_NAME
 * file. If ALPHA_CALIB_RUN_NAME is another file then also updates alpha calib
 * constants from that file.
 */
void AsymRoot::UpdateRunConfig()
{ //{{{

   // if not calib
   //if ( !(gAnaInfo.fModes & AnaInfo::MODE_CALIB) ) 
   if ( !gAnaInfo.HasCalibBit() ) {

      string fname = gAnaInfo.GetDlCalibFile();
      Info("UpdateRunConfig", "Reading RunConfig object from file %s", fname.c_str());
      TFile *f = TFile::Open(fname.c_str());
      fEventConfig = (EventConfig*) f->FindObjectAny("EventConfig");
      //delete f;

      if (!fEventConfig) {
         Error("UpdateRunConfig", "No RunConfig object found in file %s", fname.c_str());
         return;
      }

   // else if not alpha mode what am I trying to do here? 
   //} else if ( !(gAnaInfo.fModes & (AnaInfo::MODE_ALPHA^AnaInfo::MODE_CALIB)) )
   } else if ( !gAnaInfo.HasAlphaBit() && gAnaInfo.HasNormalBit()) {
   //} else if ( gAnaInfo.HasNormalBit())

      // Now, if alpha calib file is different update alpha constants from that
      // RunConfig
      string fnameAlpha = gAnaInfo.GetAlphaCalibFile();
      
      // XXX not implemented. Need to fix it ASAP!
      //if (fnameAlpha != fname) {
         Info("UpdateRunConfig", "Reading RunConfig object from alpha calib file %s", fnameAlpha.c_str());

         TFile *f = TFile::Open(fnameAlpha.c_str());
         EventConfig *alphaRunConfig = (EventConfig*) f->FindObjectAny("EventConfig");
         //delete f;

         if (!alphaRunConfig) {
            Error("UpdateRunConfig", "No RunConfig object found in alpha calib file %s", fnameAlpha.c_str());
            return;
         }

         fEventConfig = alphaRunConfig;
         //fEventConfig->Print();

         // XXX not implemented. Need to fix it!
         // ....
      //}
   }

   // Update the pointer to RunConfig object in the event
   delete fChannelEvent->fEventConfig;
   fChannelEvent->fEventConfig = fEventConfig;

   // Update the calibrator based on the running mode, i.e. alpha or
   // normal data
   UpdateCalibrator();

} //}}}


/**
 * Sets current event with data from raw file.
 */
void AsymRoot::SetChannelEvent(processEvent &event)
{
   fChannelEvent->fEventId.fRevolutionId = event.delim*512 + event.rev*2 + event.rev0;
   fChannelEvent->fEventId.fBunchId      = event.bid;
   fChannelEvent->fEventId.fChannelId    = event.stN;
   fChannelEvent->fEventId.fDelimiterId  = event.delim;
   fChannelEvent->fChannel.fAmpltd       = event.amp;
   fChannelEvent->fChannel.fIntgrl       = event.intg;
   fChannelEvent->fChannel.fTdc          = event.tdc;
   fChannelEvent->fChannel.fTdcAMax      = event.tdcmax;
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
void AsymRoot::PreProcess()
{
   fHists->PreFill();
   fHists->PreFill("_cut1");
   fHists->PreFill("_cut2");
}


/** */
void AsymRoot::PostProcess()
{
   gAsymCalculator.CalcBunchAsymmetry();

   fHists->PostFill();
}


/** */
void AsymRoot::FillPreProcess()
{
   fHists->d["std"]->FillPreProcess(fChannelEvent);
}


/** */
void AsymRoot::FillScallerHists(Long_t *hData, UShort_t chId)
{
   ((CnipolScalerHists*) fHists->d["scalers"])->Fill(hData, chId);
}


/** */
void AsymRoot::FillTargetHists(Int_t n, Double_t *hData)
{
   ((CnipolTargetHists*) fHists->d["targets"])->Fill(n, hData);
}


/** */
void AsymRoot::FillProfileHists(UInt_t n, Long_t *hData)
{
   ((CnipolProfileHists*) fHists->d["profile"])->Fill(n, hData);
}


/** */
void AsymRoot::ProcessProfileHists()
{
   ((CnipolProfileHists*) fHists->d["profile"])->Process();
}


/**
 *
 */
void AsymRoot::AddChannelEvent()
{
   if (gAnaInfo.SAVETREES.test(0))
      fRawEventTree->Fill();

   if (gAnaInfo.SAVETREES.test(1) || gAnaInfo.SAVETREES.test(2)) {
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
         //fEventConfig->Write("EventConfig");
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
   fEventConfig->Write("EventConfig");

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


/**
 *
 */
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
void AsymRoot::UpdateCalibrator()
{ //{{{
   Calibrator *calibrator;

   if (gAnaInfo.CMODE) {
      Info("UpdateCalibrator", "Setting AlphaCalibrator");
      calibrator = new AlphaCalibrator();
   } else {
      Info("UpdateCalibrator", "Setting DeadLayerCalibrator");

      //calibrator = new DeadLayerCalibrator();
      calibrator = new DeadLayerCalibratorEDepend();

      //calibrator->fChannelCalibs = fEventConfig->fCalibrator->fChannelCalibs;
      //calibrator->Calibrate(fHists);
      //fEventConfig->fCalibrator->fChannelCalibs = calibrator->fChannelCalibs;
      //delete calibrator;
   }

   //if (!calibrator) exit(0);

   // Copy existing constants to the new calibrator
   calibrator->fChannelCalibs = fEventConfig->fCalibrator->fChannelCalibs;

   // Existing calibrator will be replaced so, delete it first
   delete fEventConfig->fCalibrator;

   // and finally, assign the new calibrator
   fEventConfig->fCalibrator = calibrator;

   //((DeadLayerCalibrator*) fEventConfig->fCalibrator)->Calibrate(fHists);
   //calibrator->Calibrate(fHists);
   //fEventConfig->fCalibrator = calibrator;
} //}}}


/** */
void AsymRoot::Calibrate()
{
   fEventConfig->fCalibrator->Calibrate(fHists);
}


/** */
void AsymRoot::CalibrateFast()
{
   fEventConfig->fCalibrator->CalibrateFast(fHists);
}


/** */
void AsymRoot::SaveChannelTrees()
{
   if (!gAnaInfo.SAVETREES.test(1)) return;

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


/**
 *
 */
void AsymRoot::SaveEventTree()
{
   if (!gAnaInfo.SAVETREES.test(2)) return;

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


// Description : Book AsymRoot Histograms
void AsymRoot::BookHists()
{
   Char_t hname[100], htitle[100];
 
   rootfile->cd();

   // FeedBack Directory
   FeedBack->cd();
 
   sprintf(hname, "mass_feedback_all");
   sprintf(htitle, "%.3f : Invariant Mass (feedback) for all strips", gRunInfo.RUNID);
   mass_feedback_all = new TH1F(hname, htitle, 100, 0, 20);
   mass_feedback_all -> GetXaxis() -> SetTitle("Mass [GeV/c^2]");
   mass_feedback_all -> SetLineColor(2);
 
   for (int i=0; i<TOT_WFD_CH; i++) {
 
      sprintf(hname, "mass_feedback_st%d", i+1);
      sprintf(htitle, "%.3f : Invariant Mass (feedback) for Strip-%d ",gRunInfo.RUNID, i+1);
      mass_feedback[i] = new TH1F(hname, htitle, 100, 0, 20);
      mass_feedback[i] -> GetXaxis() -> SetTitle("Mass [GeV/c^2]");
      mass_feedback[i] -> SetLineColor(2);
   }
 
   Kinema->cd();

   // 1-dim Energy Spectrum
   Eslope.nxbin=100; Eslope.xmin=0; Eslope.xmax=0.03;
 
   for (int i=0; i<NDETECTOR; i++) {
      sprintf(hname, "energy_spectrum_det%d", i+1);
      sprintf(htitle,"%.3f : Energy Spectrum Detector %d ", gRunInfo.RUNID, i+1);
      energy_spectrum[i] = new TH1F(hname,htitle, Eslope.nxbin, Eslope.xmin, Eslope.xmax);
      energy_spectrum[i]->GetXaxis()->SetTitle("Momentum Transfer [-GeV/c]^2");
      //energy_spectrum[i] = (TH1F*) fHists->d["Kinema"].o[hname];
   }
 
   sprintf(htitle,"%.3f : Energy Spectrum (All Detectors)", gRunInfo.RUNID);
   energy_spectrum_all = new TH1F("energy_spectrum_all", htitle, Eslope.nxbin, Eslope.xmin, Eslope.xmax);
   energy_spectrum_all -> GetXaxis() -> SetTitle("Momentum Transfer [-GeV/c]^2");
   //energy_spectrum_all = (TH1F*) fHists->d["Kinema"].o["energy_spectrum_all"];
 
   sprintf(hname,"mass_nocut_all");
   sprintf(htitle,"%.3f : Invariant Mass (nocut) for all strips", gRunInfo.RUNID);
   mass_nocut_all = new TH1F(hname, htitle, 100, 0, 20);
   mass_nocut_all->GetXaxis()->SetTitle("Mass [GeV/c^2]");
 
   sprintf(hname,"mass_yescut_all");
   sprintf(htitle,"%.3f : Invariant Mass (w/cut) for all strips", gRunInfo.RUNID);
   mass_yescut_all = new TH1F(hname, htitle, 100, 0, 20);
   mass_yescut_all->GetXaxis()->SetTitle("Mass [GeV/c^2]");
   mass_yescut_all->SetLineColor(2);
 
   // Need to book for TOT_WFD_CH instead of NSTRIP to avoid seg. fault by filling histograms by
   // target events strip [73 - 76].
   for (int i=0; i<TOT_WFD_CH; i++) {
 
      sprintf(hname,"t_vs_e_st%d",i+1);
      sprintf(htitle,"%.3f : t vs. Kin.Energy Strip-%d ", gRunInfo.RUNID, i+1);
      t_vs_e[i] = new TH2F(hname,htitle, 50, 200, 1500, 100, 20, 90);
      t_vs_e[i] -> GetXaxis() -> SetTitle("Kinetic Energy [keV]");
      t_vs_e[i] -> GetYaxis() -> SetTitle("Time of Flight [ns]");
 
      sprintf(hname,"t_vs_e_yescut_st%d",i+1);
      sprintf(htitle,"%.3f : t vs. Kin.Energy (with cut) Strip-%d ", gRunInfo.RUNID, i+1);
      t_vs_e_yescut[i] = new TH2F(hname,htitle, 50, 200, 1500, 100, 20, 90);
      t_vs_e_yescut[i] -> GetXaxis() -> SetTitle("Kinetic Energy [keV]");
      t_vs_e_yescut[i] -> GetYaxis() -> SetTitle("Time of Flight [ns]");
 
      sprintf(hname,"mass_vs_e_ecut_st%d",i+1);
      sprintf(htitle,"%.3f : Mass vs. Kin.Energy (Energy Cut) Strip-%d ", gRunInfo.RUNID, i+1);
      mass_vs_e_ecut[i] = new TH2F(hname,htitle, 50, 200, 1000, 200, 6, 18);
      mass_vs_e_ecut[i] -> GetXaxis() -> SetTitle("Kinetic Energy [keV]");
      mass_vs_e_ecut[i] -> GetYaxis() -> SetTitle("Invariant Mass [GeV]");
 
      sprintf(hname, "mass_nocut_st%d",i+1);
      sprintf(htitle,"%.3f : Invariant Mass (nocut) for Strip-%d ",gRunInfo.RUNID, i+1);
      mass_nocut[i] = new TH1F(hname, htitle, 100, 0, 20);
      mass_nocut[i] -> GetXaxis() -> SetTitle("Mass [GeV/c^2]");
 
      sprintf(hname, "mass_yescut_st%d",i+1);
      sprintf(htitle,"%.3f : Invariant Mass (w/cut) for Strip-%d ",gRunInfo.RUNID, i+1);
      mass_yescut[i] = new TH1F(hname, htitle, 100, 0, 20);
      mass_yescut[i] -> GetXaxis() -> SetTitle("Mass [GeV/c^2]");
      mass_yescut[i] -> SetLineColor(2);
   }
 
   // Bunch Directory
   Bunch->cd();
   sprintf(htitle,"%.3f : Counts per Bunch ", gRunInfo.RUNID);
   bunch_dist = new TH1F("bunch_dist", htitle, NBUNCH, -0.5, NBUNCH-0.5);
   bunch_dist -> GetXaxis() -> SetTitle("Bunch ID");
   bunch_dist -> GetYaxis() -> SetTitle("Counts");
   bunch_dist -> SetFillColor(13);
 
   sprintf(htitle,"%.3f : Wall Current Monitor", gRunInfo.RUNID);
   wall_current_monitor = new TH1F("wall_current_monitor", htitle, NBUNCH, -0.5, NBUNCH-0.5);
   wall_current_monitor -> GetXaxis() -> SetTitle("Bunch ID");
   wall_current_monitor -> GetYaxis() -> SetTitle("x10^9 protons");
   wall_current_monitor -> SetFillColor(13);
 
   sprintf(htitle,"%.3f : Specific Luminosity", gRunInfo.RUNID);
   specific_luminosity = new TH1F("specific_luminosity", htitle, NBUNCH, -0.5, NBUNCH-0.5);
   specific_luminosity -> GetXaxis() -> SetTitle("Bunch ID");
   specific_luminosity -> GetYaxis() -> SetTitle("x10^9 protons");
   specific_luminosity -> SetFillColor(13);
 
   // Error detectors
   ErrDet->cd();
   sprintf(htitle,"%.3f : Bunch Asymmetry X90", gRunInfo.RUNID);
   asym_bunch_x90 = new TH1F("asym_bunch_x90", htitle, 100, -0.1, 0.1);

   sprintf(htitle,"%.3f : Bunch Asymmetry X45", gRunInfo.RUNID);
   asym_bunch_x45 = new TH1F("asym_bunch_x45", htitle, 100, -0.1, 0.1);

   sprintf(htitle,"%.3f : Bunch Asymmetry Y45", gRunInfo.RUNID);
   asym_bunch_y45 = new TH1F("asym_bunch_y45", htitle, 100, -0.1, 0.1);

   sprintf(htitle,"%.3f : # of Events in Banana Cut per strip", gRunInfo.RUNID);
   good_carbon_events_strip = new TH1I("good_carbon_events_strip", htitle, NSTRIP, 0.5, NSTRIP+0.5);
   good_carbon_events_strip->SetFillColor(17);

   Asymmetry->cd();
   asym_vs_bunch_x45    = new TH2F();
   asym_vs_bunch_x90    = new TH2F();
   asym_vs_bunch_y45    = new TH2F();
   asym_sinphi_fit      = new TH2F();
   scan_asym_sinphi_fit = new TH2F();
}


// Description : Book ROOT Functions and Histograms using Feedback infomations
//             : This routine shuould be called after Feedback operation
int AsymRoot::BookHists2(StructFeedBack &feedback)
{
   rootfile->cd();
   Kinema->cd();
 
   char  formula[100], fname[100];
   float low, high, sigma;
   int   Color=2;
   int   Width=2;
 
   for (int i=0; i<NSTRIP; i++) {
 
      for (int j=0; j<2; j++) {
 
         sigma = j ? gRunConsts[i+1].M2T*feedback.RMS[i]*gAnaInfo.MassSigmaAlt :
                     gRunConsts[i+1].M2T*feedback.RMS[i]*gAnaInfo.MassSigma;
 
         int Style = j + 1 ;
 
         // lower limit
         sprintf(formula, "%f/sqrt(x)+(%f)/sqrt(x)", gRunConsts[i+1].E2T, sigma);
         sprintf(fname, "banana_cut_l_st%d_mode%d", i, j);
         banana_cut_l[i][j] = new TF1(fname, formula, gAnaInfo.enel, gAnaInfo.eneu);
         banana_cut_l[i][j] -> SetLineColor(Color);
         banana_cut_l[i][j] -> SetLineWidth(Width);
         banana_cut_l[i][j] -> SetLineStyle(Style);
 
         // upper limit
         sprintf(formula,"%f/sqrt(x)-(%f)/sqrt(x)", gRunConsts[i+1].E2T, sigma);
         sprintf(fname, "banana_cut_h_st%d", i);
         banana_cut_h[i][j] = new TF1(fname, formula, gAnaInfo.enel, gAnaInfo.eneu);
         banana_cut_h[i][j] -> SetLineColor(Color);
         banana_cut_h[i][j] -> SetLineWidth(Width);
         banana_cut_h[i][j] -> SetLineStyle(Style);
      }
 
      // energy cut low
      low  = gRunConsts[i+1].E2T / sqrt(double(gAnaInfo.enel)) -
                 gRunConsts[i+1].M2T * feedback.RMS[i] * gAnaInfo.MassSigma / sqrt(double(gAnaInfo.enel));
      high = gRunConsts[i+1].E2T / sqrt(double(gAnaInfo.enel)) +
                 gRunConsts[i+1].M2T * feedback.RMS[i] * gAnaInfo.MassSigma / sqrt(double(gAnaInfo.enel));
 
      energy_cut_l[i] = new TLine(gAnaInfo.enel, low, gAnaInfo.enel, high);
      energy_cut_l[i] ->SetLineColor(Color);
      energy_cut_l[i] ->SetLineWidth(Width);
 
      // energy cut high
      low  = gRunConsts[i+1].E2T / sqrt(double(gAnaInfo.eneu)) -
                 gRunConsts[i+1].M2T * feedback.RMS[i] * gAnaInfo.MassSigma / sqrt(double(gAnaInfo.eneu));
      high = gRunConsts[i+1].E2T / sqrt(double(gAnaInfo.eneu)) +
                 gRunConsts[i+1].M2T * feedback.RMS[i] * gAnaInfo.MassSigma / sqrt(double(gAnaInfo.eneu));
 
      energy_cut_h[i] = new TLine(gAnaInfo.eneu, low, gAnaInfo.eneu, high);
      energy_cut_h[i] ->SetLineColor(Color);
      energy_cut_h[i] ->SetLineWidth(Width);
   }
 
   return 0;
}


//
// Class name  : AsymRoot
// Method name : DeleteHistogram
//
// Description : Delete Unnecessary Histograms
//             :
// Input       :
// Return      :
//
void AsymRoot::DeleteHistogram()
{ //{{{
  // Delete histograms declared for WFD channel 72 - 75 to avoid crash. These channcles
  // are for target channels and thus thes histograms wouldn't make any sense.
  for (int i=NSTRIP; i<TOT_WFD_CH; i++ ) {
     t_vs_e[i]->Delete();
     t_vs_e_yescut[i]->Delete();
     mass_vs_e_ecut[i]->Delete();  // Mass vs. 12C Kinetic Energy
     mass_nocut[i]->Delete();
     mass_yescut[i]->Delete();
  }
} //}}}


// Description : Write out objects in memory and dump in rootfile before closing it
void AsymRoot::Finalize()
{
  rootfile->cd();
  Kinema->cd();

  for (int i=0; i<NSTRIP; i++) {
     if (t_vs_e[i]) {

        for (int j=0; j<2; j++){
           if (banana_cut_l[i][j]) t_vs_e[i]->GetListOfFunctions()->Add(banana_cut_l[i][j]);
           if (banana_cut_h[i][j]) t_vs_e[i]->GetListOfFunctions()->Add(banana_cut_h[i][j]);
        }

        if (energy_cut_l[i]) t_vs_e[i]->GetListOfFunctions()->Add(energy_cut_l[i]);
        if (energy_cut_h[i]) t_vs_e[i]->GetListOfFunctions()->Add(energy_cut_h[i]);
     }
  }

  rootfile->cd();

  //if (!gAnaInfo.CMODE) {
  fHists->Write();
  fHists->Delete();
  //}

  //rootfile->Write();

  rootfile->cd();
  //fEventConfig->PrintAsPhp();
  fEventConfig->Write("EventConfig");

  // close rootfile
  rootfile->Close();

  if (gAnaInfo.SAVETREES.any()) {
     SaveChannelTrees();
     SaveEventTree();
     WriteTreeFile();
     fOutTreeFile->Close();
     fOutTreeFile->Delete();
  }

  delete fEventConfig;
}


/** */
void AsymRoot::SaveAs(string pattern, string dir)
{
   if (!gAnaInfo.HasAlphaBit()) {
      gStyle->SetMarkerStyle(kFullDotLarge);
      gStyle->SetMarkerSize(1);
      gStyle->SetMarkerColor(kRed);
   }

   TCanvas c("cName", "cName", 1200, 600);

   fHists->SaveAllAs(c, pattern, dir.c_str());
}
