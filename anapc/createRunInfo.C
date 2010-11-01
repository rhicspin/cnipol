

#include "createRunInfo.h"


using namespace std;

void createRunInfo(string runId)
{
   char * envDataDir = getenv("DATADIR");
   printf("DATADIR=%s\n", envDataDir);

   gROOT->ForceStyle(kFALSE);
   //TFile *_file0 = TFile::Open("~/results/11068.109/11068.109.root");
   TFile *_file0 = TFile::Open("~/results/10765.006/10765.006.root");

   CnipolHists hists;
   hists.ReadFromDir(_file0);


   //TCanvas *c = new TCanvas("cName", "cName", 800, 600);
   TCanvas c("cName", "cName", 800, 600);

   //hists.Draw(c);

   // Need to create images folder!!! XXX
   hists.SaveAllAs(c, "~/results/10765.006/images/");

   EventConfig *ec = (EventConfig*) _file0->FindObjectAny("EventConfig");
   FILE *f = fopen("/home/dsmirnov/results/10765.006/runconfig.php", "w");
   //if (!f) printf("zero\n");
   ec->PrintAsPhp(f);
}
