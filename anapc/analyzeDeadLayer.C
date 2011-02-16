
#include "analyzeDeadLayer.h"

using namespace std;


void analyzeDeadLayer(string runName, std::string pattern, Long64_t nEvents)
{
   analyzeDeadLayer_initialize(runName);
   //analyzeDeadLayer_book_histograms();
   //analyzeDeadLayer_fill_histograms(nEvents);
   //analyzeDeadLayer_fit_histograms();
   //analyzeDeadLayer_fill_histograms_derivative();
   analyzeDeadLayer_finalize(pattern);
}


void analyzeDeadLayer_initialize(string runName)
{
   gStyle->SetPalette(1);
   gStyle->SetOptFit(1111);
   gStyle->SetOptStat("emroui");
   gStyle->SetStatX(0.99);
   gStyle->SetStatY(0.99);
   gStyle->SetStatW(0.15);
   gStyle->SetStatH(0.15);

   gStyle->SetPadRightMargin(0.30);

   gOutFile = new TFile("./out.root", "recreate");
   //gH = new CnipolCalibHists(gOutFile);
   //gH->fDir = gOutFile;

   gChain = new TChain("AnaEventTree");

   const char* outDir = getenv("CNIPOL_RESULTS_DIR");

   gRunName = runName;
   gOutDir.reserve(255);
   sprintf(&gOutDir[0], "%s/%s", outDir, gRunName.c_str());

   string fileName("", 255);
   sprintf(&fileName[0], "%s/%s_tree_00.root", gOutDir.c_str(), gRunName.c_str());

   gChain->AddFile(fileName.c_str());
   gChain->SetBranchAddress("AnaEvent", &gAnaEvent);

   sprintf(&fileName[0], "%s/%s.root", gOutDir.c_str(), gRunName.c_str());

   gInFile = new TFile(fileName.c_str(), "READ");

   if (!gInFile) exit(-1);

   gH = new DrawObjContainer(gInFile);

   gH->Add(new CnipolRunHists(gInFile));

   gH->d["std"] = new CnipolHists();

   gH->ReadFromDir(gInFile);
   //gH->ReadFromDir(dir);

   ec = (EventConfig*) gInFile->FindObjectAny("EventConfig");
   //ec->Print();
   //ec->fCalibrator->fChannelCalibs.clear();
}


/*
void analyzeDeadLayer_fill_histograms(Long64_t nEvents)
{ //{{{
   gRandom = new TRandom();

   Int_t maxEvents = gChain->GetEntries();

   if (nEvents < 0 || nEvents > maxEvents) nEvents = maxEvents;

   Long64_t nbytes = 0;

   for (Long64_t i=0; i<nEvents; i++) {

      nbytes += gChain->GetEntry(i);

      //printf("num. of events: %lld\n", nEvents);

      //int nChannels = gAnaEvent->fChannels.size();
      //printf("num. of channels: %d\n", nChannels);

      //gAnaEvent->Print();

      ChannelDataMap::iterator mi;
      ChannelDataMap::iterator mb = gAnaEvent->fChannels.begin();
      ChannelDataMap::iterator me = gAnaEvent->fChannels.end();

      string  sSi("  ");

      for (mi=mb; mi!=me; mi++) {

         //if (mi->first+1 != 54) continue;

         //float energy  = gAnaEvent->GetEnergy(ec, *mi);
         //float energyQ = gAnaEvent->GetEnergyIntegral(ec, *mi);
         //float time    = gAnaEvent->GetTime(ec, *mi);

         float amplitude  = mi->second.fAmpltd;
         float integral   = mi->second.fIntgrl;
         float tdc        = mi->second.fTdc;
         //float stime      = gAnaEvent->fEventId.fRevolutionId + gAnaEvent->fEventId.fBunchId;

         ((TH1F*) gH->o["hAmpltd"])->Fill(amplitude);
         ((TH1F*) gH->o["hIntgrl"])->Fill(integral);
         ((TH1F*) gH->o["hTdc"])->Fill(tdc);
         ((TH2F*) gH->o["hTvsA"])->Fill(amplitude, tdc);
         ((TH2F*) gH->o["hTvsI"])->Fill(integral, tdc);
         ((TH2F*) gH->o["hIvsA"])->Fill(amplitude, integral);

         // This is dangerous but we know what we are doing
         sprintf(&sSi[0], "%02d", mi->first+1);

         ((TH1F*) gH->d["channel"+sSi].o["hAmpltd_st"+sSi])->Fill(amplitude);
         ((TH1F*) gH->d["channel"+sSi].o["hIntgrl_st"+sSi])->Fill(integral);
         ((TH1F*) gH->d["channel"+sSi].o["hTdc_st"+sSi])->Fill(tdc);
         ((TH2F*) gH->d["channel"+sSi].o["hTvsA_st"+sSi])->Fill(amplitude, tdc);
         ((TH2F*) gH->d["channel"+sSi].o["hTvsI_st"+sSi])->Fill(integral, tdc);
         ((TH2F*) gH->d["channel"+sSi].o["hIvsA_st"+sSi])->Fill(amplitude, integral);

         if (tdc >=15 && tdc <= 50 && amplitude >= 130 && amplitude <= 210) {
            ((TH1F*) gH->o["hAmpltd_cut1"])->Fill(amplitude);
            ((TH1F*) gH->d["channel"+sSi].o["hAmpltd_cut1_st"+sSi])->Fill(amplitude);
         }

         if (tdc >=12 && tdc <= 16 && integral >= 100) {
            ((TH1F*) gH->o["hIntgrl_cut1"])->Fill(integral);
            ((TH1F*) gH->d["channel"+sSi].o["hIntgrl_cut1_st"+sSi])->Fill(integral);
         }

         //hEventSeq->Fill(stime);
         //printf("e: %f\n", energy);
         //hAmpltd->Fill(energy);
         //hIntgrl->Fill(energyQ);
         //hTdc->Fill(time);

         //hTvsE->Fill(energy, time);
         //hTvsQ->Fill(energyQ, time);

         //hEvsQ->Fill(energy, energyQ);
      }
   }
}
// }}}
*/


void analyzeDeadLayer_fit_histograms()
{ // {{{
   
   //ec->fCalibrator->Calibrate(gH);
   ((DeadLayerCalibratorEDepend*) ec->fCalibrator)->PostCalibrate();

   //ec->Print();
   //ec->PrintAsConfig(stdout);

} // }}}


/*
TFitResultPtr analyzeDeadLayer_fit_histograms(TH1 *h, TF1 *f)
{ // {{{
   TFitResultPtr frp;

   if (h->Integral() < 10) {
      f->SetParameters(0, 1, 1);
      f->SetParError(2, 100);
      return frp;
   }

   int   mbin = h->GetMaximumBin();
   float norm = h->GetBinContent(mbin);
   float mean = h->GetBinCenter(mbin);
   //float intg = h->Integral();
   float expectedSigma = 0.7;

   //h->SetAxisRange(apeak-awidth,apeak+awidth);
   f->SetParameters(norm, mean, expectedSigma);
   f->SetParLimits(0, 0.8*norm, 1.2*norm); // norm
   f->SetParLimits(1, mean-5*expectedSigma, mean+5*expectedSigma); // mean
   f->SetParLimits(2, 0.1, 3*expectedSigma); // sigma

   //h->Fit(f, "", "", apeak-awidth, apeak+awidth);
   frp = h->Fit(f, "BMS"); // B: use limits, M: improve fit, S: return frp

   return frp;
}
// }}}
*/


void analyzeDeadLayer_finalize(string pattern)
{ // {{{
   TCanvas c("cName", "cName", 1200, 600);

   string path("", 255);
   sprintf(&path[0], "%s/images", gOutDir.c_str());

   //gH->SaveAllAs(c, "^.*c_combo.*$", path.c_str());
   //gH->SaveAllAs(c, "^.*c_hTimeVsEnergyA.*$", path.c_str());
   //gH->SaveAllAs(c, "^.*c_asym_sinphi_fit.*$", path.c_str());
   gH->SaveAllAs(c, pattern, path.c_str());
   //gH->Write();
   //gOutFile->Close();

   // Save config info to php file
   string fileName("", 255);
   sprintf(&fileName[0], "%s/runconfig.php", gOutDir.c_str());

   FILE *f = fopen(fileName.c_str(), "w");
   ec->PrintAsPhp(f);
   fclose(f);

   //ec->PrintAsPhp(stdout);

   sprintf(&fileName[0], "%s/config_calib.dat", gOutDir.c_str());

   f = fopen(fileName.c_str(), "w");
   ec->PrintAsConfig(f);
   fclose(f);

   ec->PrintAsConfig(stdout);

   // Delete from and Write updated run config object to root file
   //gInFile->cd();
   //gInFile->Delete("EventConfig;1");
   //ec->Write("EventConfig");
   //gInFile->Close();
} // }}}
