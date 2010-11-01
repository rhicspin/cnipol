

#include "testCalib.h"



void testCalib(Long64_t nEvents)
{
   char hName[256];
   sprintf(hName, "hTvsE");
   hTvsE = new TH2F(hName, hName, 100, 0, 1500, 100, 0, 80);

   sprintf(hName, "hTvsQ");
   hTvsQ = new TH2F(hName, hName, 100, 0, 1500, 100, 0, 80);

   gRandom = new TRandom();

   TChain *chain = new TChain("AnaEventTree");

   //TFile *_file0 = TFile::Open("~/cnipol_install/offline/11068.109.root");
   TFile *_file0 = TFile::Open("~/cnipol_install/offline/11068.109_tree_00.root");

   chain->AddFile("~/cnipol_install/offline/11068.109_tree_00.root");
   chain->SetBranchAddress("AnaEvent", &gAnaEvent);

   EventConfig *ec = (EventConfig*) _file0->FindObjectAny("EventConfig");

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

         float adc = mi->second.fAmpltd;// * (gRandom->Rndm() - 0.5);
         float energy = ec->ConvertToEnergy(adc, mi->first);
         float time = mi->second.fTdc + (gRandom->Rndm() - 0.5);
         float energyQ = ec->ConvertToEnergy(mi->second.fIntgrl, mi->first);

         //printf("e: %f\n", energy);
         hTvsE->Fill(energy, time);
         hTvsQ->Fill(energyQ, time);
      }
   }

   //if (ec) ec->Print();
   TCanvas *c1 = new TCanvas();
   hTvsE->Draw();

   TCanvas *c2 = new TCanvas();
   hTvsQ->Draw();
}
