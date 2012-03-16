
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

   sprintf(hName, "hEventSeq");
   hEventSeq = new TH1F(hName, hName, 50000, 0, 35e6);

   sprintf(hName, "hAmpltd");
   hAmpltd = new TH1F(hName, hName, 100, 0, 255);

   sprintf(hName, "hTdc");
   hTdc = new TH1F(hName, hName, 70, 0, 70);

   sprintf(hName, "hIntgrl");
   hIntgrl = new TH1F(hName, hName, 100, 0, 255);

   sprintf(hName, "hTvsE");
   //hTvsE = new TH2F(hName, hName, 100, 0, 1500, 100, 0, 80);
   hTvsE = new TH2F(hName, hName, 100, 0, 250, 100, 0, 80);

   sprintf(hName, "hTvsQ");
   //hTvsQ = new TH2F(hName, hName, 100, 0, 1500, 100, 0, 80);
   hTvsQ = new TH2F(hName, hName, 100, 0, 250, 100, 0, 80);

   sprintf(hName, "hEvsQ");
   //hTvsE = new TH2F(hName, hName, 100, 0, 1500, 100, 0, 80);
   hEvsQ = new TH2F(hName, hName, 100, 0, 250, 100, 0, 255);

   gRandom = new TRandom();

   TChain *chain = new TChain("AnaEventTree");

   //TFile *_file0 = TFile::Open("~/cnipol_install/offline/11068.109.root");
   //TFile *_file0 = TFile::Open("~/cnipol_install/offline/11068.109_tree_00.root");
   //TFile *_file0 = TFile::Open("~/cnipol_install/offline/10765.006_tree_00.root");
   TFile *_file0 = TFile::Open("~/results/alpha0429.000/0.000_tree_00.root");

   //chain->AddFile("~/cnipol_install/offline/11068.109_tree_00.root");
   //chain->AddFile("~/cnipol_install/offline/10765.006_tree_00.root");
   chain->AddFile("~/results/alpha0429.000/0.000_tree_00.root");
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

         if (mi->first != 33) continue;
         
         //float energy  = gAnaEvent->GetEnergy(ec, *mi);
         //float energyQ = gAnaEvent->GetEnergyIntegral(ec, *mi);
         //float time    = gAnaEvent->GetTime(ec, *mi);

         float energy  = mi->second.fAmpltd;
         float energyQ = mi->second.fIntgrl;
         float time    = mi->second.fTdc;

         float stime   = gAnaEvent->fEventId.fRevolutionId + gAnaEvent->fEventId.fBunchId;

         hEventSeq->Fill(stime);
         //printf("e: %f\n", energy);
         hAmpltd->Fill(energy);
         hIntgrl->Fill(energyQ);
         hTdc->Fill(time);

         hTvsE->Fill(energy, time);
         hTvsQ->Fill(energyQ, time);

         hEvsQ->Fill(energy, energyQ);
      }
   }

   c1 = new TCanvas();
   hEventSeq->Draw();
   hEventSeq->GetXaxis()->SetTitle("rev+bunch");
   c1->SaveAs("hEventSeq.png");

   return;

   //if (ec) ec->Print();
   c1 = new TCanvas();
   hAmpltd->Draw();
   hAmpltd->GetXaxis()->SetTitle("ADC");
   c1->SaveAs("hAmpltd.png");

   c1 = new TCanvas();
   hIntgrl->Draw();
   hIntgrl->GetXaxis()->SetTitle("ADC");
   c1->SaveAs("hIntgrl.png");

   c1 = new TCanvas();
   hTdc->Draw();
   hTdc->GetXaxis()->SetTitle("TDC");
   c1->SaveAs("hTdc.png");

   c1 = new TCanvas();
   c1->SetLogz();
   hTvsE->Draw("colz");
   //hTvsE->GetXaxis()->SetTitle("E, keV");
   //hTvsE->GetYaxis()->SetTitle("T, ns");
   hTvsE->GetXaxis()->SetTitle("ADC");
   hTvsE->GetYaxis()->SetTitle("TDC");
   c1->SaveAs("hTvsE.png");

   c1 = new TCanvas();
   c1->SetLogz();
   hTvsQ->Draw("colz");
   //hTvsQ->GetXaxis()->SetTitle("E, keV");
   //hTvsQ->GetYaxis()->SetTitle("T, ns");
   hTvsE->GetXaxis()->SetTitle("ADC");
   hTvsE->GetYaxis()->SetTitle("TDC");
   c1->SaveAs("hTvsQ.png");

   c1 = new TCanvas();
   c1->SetLogz();
   hEvsQ->Draw("colz");
   //hTvsQ->GetXaxis()->SetTitle("E, keV");
   //hTvsQ->GetYaxis()->SetTitle("T, ns");
   hEvsQ->GetXaxis()->SetTitle("ADC");
   hEvsQ->GetYaxis()->SetTitle("ADC");
   c1->SaveAs("hEvsQ.png");
}
