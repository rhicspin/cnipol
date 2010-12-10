

#include "createRunInfo.h"


using namespace std;

void createRunInfo(string runId)
{
   char * envDataDir = getenv("DATADIR");
   printf("DATADIR=%s\n", envDataDir);

   gROOT->ForceStyle(kFALSE);
   //TFile *_file0 = TFile::Open("~/results/10746.204/10746.204.root");
   //TFile *_file0 = TFile::Open("~/results/10450.116/10450.116.root");
   //TFile *_file0 = TFile::Open("~/results/11068.109/11068.109.root");
   TFile *_file0 = TFile::Open("~/results/10765.006/10765.006.root");

   CnipolHists hists;
   hists.ReadFromDir(_file0);

   Calibrator *calibrator = new DeadLayerCalibrator();

   //calibrator->Calibrate(&hists);

   //TCanvas *c = new TCanvas("cName", "cName", 800, 600);
   TCanvas *c = new TCanvas("cName", "cName", 800, 600);

   //hists.Draw(c);
   //TH1D* h = ((TH1D*) hists.d["channel28"].o["hTimeVsEnergyA_cut1_st28"]);

   //h->DrawCopy();

   //TH1D* h2 = (TH1D*) h->Clone();
   //h2->Reset("ICE");

   //TCanvas *c2 = new TCanvas("cName2", "cName2", 800, 600);
   //h2->DrawCopy();

   // Need to create images folder!!! XXX
   //hists.SaveAllAs(*c, "/home/dsmirnov/results/10746.204/images/");
   //hists.SaveAllAs(c, "/home/dsmirnov/results/10450.116/images/");
   //hists.SaveAllAs(c, "/home/dsmirnov/results/11068.109/images/");
   hists.SaveAllAs(*c, "/home/dsmirnov/results/10765.006/images/");

   EventConfig *ec = (EventConfig*) _file0->FindObjectAny("EventConfig");
   //ec->Print();
   //FILE *f = fopen("/home/dsmirnov/results/10746.204/runconfig.php", "w");
   //FILE *f = fopen("/home/dsmirnov/results/10450.116/runconfig.php", "w");
   //FILE *f = fopen("/home/dsmirnov/results/11068.109/runconfig.php", "w");
   FILE *f = fopen("/home/dsmirnov/results/10765.006/runconfig.php", "w");
   //if (!ec) printf("zero\n");
   ec->PrintAsPhp(f);
   ec->PrintAsPhp(stdout);
}
