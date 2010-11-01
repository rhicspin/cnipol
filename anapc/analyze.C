
#include "analyze.h"


void analyze(Long64_t nEvents)
{
   //analyze_initialize();
   //analyze_book_histograms();
   analyze_fill_histograms(nEvents);
   //analyze_fill_histograms_derivative();
   //analyze_finalize();
}


void analyze_fill_histograms(Long64_t nEvents)
{
   gStyle->SetPalette(1);
   gStyle->SetOptStat("emroui");
   gStyle->SetStatX(0.65);
   gStyle->SetStatY(0.98);
   gStyle->SetStatW(0.17);
   gStyle->SetStatH(0.08);


   char hName[256];
   sprintf(hName, "hTvsE");
   hTvsE = new TH2F(hName, hName, 100, 0, 1500, 100, 0, 80);

   sprintf(hName, "hTvsQ");
   hTvsQ = new TH2F(hName, hName, 100, 0, 1500, 100, 0, 80);

   gRandom = new TRandom();

   TChain *chain = new TChain("AnaEventTree");

   //TFile *_file0 = TFile::Open("~/cnipol_install/offline/11068.109.root");
   //TFile *_file0 = TFile::Open("~/cnipol_install/offline/11068.109_tree_00.root");
   TFile *_file0 = TFile::Open("~/cnipol_install/offline/10765.006_tree_00.root");

   //chain->AddFile("~/cnipol_install/offline/11068.109_tree_00.root");
   chain->AddFile("~/cnipol_install/offline/10765.006_tree_00.root");
   chain->SetBranchAddress("AnaEvent", &gAnaEvent);

   EventConfig *ec = (EventConfig*) _file0->FindObjectAny("EventConfig");
   //ec->Print();

   Int_t maxEvents = chain->GetEntries();
   if (nEvents < 0 || nEvents > maxEvents) nEvents = maxEvents;
   Long64_t nbytes       = 0;

   for (Long64_t i=0; i<nEvents; i++) {
      nbytes += chain->GetEntry(i);
      
      //printf("num. of events: %lld\n", nEvents);

      int nChannels = gAnaEvent->fChannels.size();
      //printf("num. of channels: %d\n", nChannels);

      //gAnaEvent->Print();

      ChannelDataMap::iterator mi;
      ChannelDataMap::iterator mb = gAnaEvent->fChannels.begin();
      ChannelDataMap::iterator me = gAnaEvent->fChannels.end();

      //for (int i=0; i!=nChannels; i++) {
      for (mi=mb; mi!=me; mi++) {

         if (mi->first != 0) continue;
         
         float energy  = gAnaEvent->GetEnergy(ec, *mi);
         float energyQ = gAnaEvent->GetEnergyIntegral(ec, *mi);
         float time    = gAnaEvent->GetTime(ec, *mi);

         //printf("e: %f\n", energy);
         hTvsE->Fill(energy, time);
         hTvsQ->Fill(energyQ, time);
      }
   }

   //if (ec) ec->Print();
   TCanvas *c1 = new TCanvas();
   c1->SetLogz();
   hTvsE->Draw("colz");
   hTvsE->GetXaxis()->SetTitle("E, keV");
   hTvsE->GetYaxis()->SetTitle("T, ns");
   c1->SaveAs("hTvsE.png");

   TCanvas *c2 = new TCanvas();
   c2->SetLogz();
   hTvsQ->Draw("colz");
   hTvsQ->GetXaxis()->SetTitle("E, keV");
   hTvsQ->GetYaxis()->SetTitle("T, ns");
   c2->SaveAs("hTvsQ.png");
}
