
#include <time.h>

#include "manalyze.h"

#include "utils/utils.h"

#include "EventConfig.h"

EventConfig *gRC;

using namespace std;

void manalyze()
{
   initialize();
   //analyze_book_histograms();
   //analyze_fill_histograms(nEvents);
   //analyze_fill_histograms_derivative();
   //analyze_finalize();
}


void initialize()
{
   struct tm tm;
   time_t firstDay;

   if ( strptime("2011-02-01 00:00:00", "%Y-%m-%d %H:%M:%S", &tm) != NULL )
      firstDay = mktime(&tm);

   printf("firstDay %d\n", firstDay);

   //TString filelist = "list_B1U.dat";
   //TString filelist = "list_B2D.dat";
   //TString filelist = "list_Y2U.dat";
   TString filelist = "list_Y1D.dat";

   TString fileName;
   //string  histName = "hPolarUniProfileBin";
   string  histName = "hPolarVsIntensProfileBin";

   //TH1* havrg = new TH1F("havrg", "havrg", 20, -3, 3);
   TH1* havrg = new TH1F("havrg", "havrg", 22, 0, 1.1);

   havrg->GetYaxis()->SetRangeUser(0, 1.05);
   havrg->SetOption("p");
   havrg->SetMarkerStyle(kFullDotLarge);
   havrg->SetMarkerSize(1);
   havrg->SetMarkerColor(kRed);
   havrg->Sumw2();
   havrg->SetBit(TH1::kIsAverage);

   TH1* hPolar = new TH1F("hPolar", "hPolar", 1, 0, 1);
   TH1* hProfR = new TH1F("hProfR", "hProfR", 1, 0, 1);

   TGraphErrors *grPolar = new TGraphErrors();
   grPolar->SetName("grPolar");
   grPolar->SetMarkerStyle(kFullDotLarge);
   grPolar->SetMarkerSize(1);
   grPolar->SetMarkerColor(kRed);

   UInt_t i = 0;

   // Fill chain with all input files from filelist
   TObject *o;
   TIter   *next = new TIter(utils::getFileList(filelist));

   while (next && (o = (*next)()) ) {
      //chain->AddFile(((TObjString*) o)->GetName());
      cout << (((TObjString*) o)->GetName()) << endl;
      fileName = "";
      //fileName += "/usr/local/polarim/root/" + string(((TObjString*) o)->GetName()) + "/" + string(((TObjString*) o)->GetName()) + ".root";
      fileName += "/data1/run11/root/" + string(((TObjString*) o)->GetName()) + "/" + string(((TObjString*) o)->GetName()) + ".root";

      TFile *f = new TFile(fileName, "READ");
      if (!f) continue; //exit(-1);

      gRC = (EventConfig*) f->FindObjectAny("EventConfig");
      if (!gRC) continue;
      //gRC->Print();

      float beamEnergy = gRC->fRunInfo->BeamEnergy;

      if (beamEnergy < 200) continue;

      //if (beamEnergy < 20 || beamEnergy > 30) continue;

      //gH = new DrawObjContainer(f);
      //gH->d["profile"] = new CnipolProfileHists();
      //gH->ReadFromDir(f);

      //TH1* h = (TH1*) gH->d["profile"]->o[histName];
      //h->Print();
      //h->SetBit(TH1::kIsAverage);

      Double_t day = (gRC->fRunInfo->StartTime - firstDay)/60./60./24.;
      //grPolar->SetPoint(i, gRC->fRunInfo->RUNID, gRC->fAnaResult->sinphi[0].P[0]);
      grPolar->SetPoint(i, day, gRC->fAnaResult->sinphi[0].P[0]);
      grPolar->SetPointError(i, 0, gRC->fAnaResult->sinphi[0].P[1]);

      //if (i == 0) havrg = h;
      //else havrg->Add(h);
      //havrg->Add(h);

      i++;
   }

   TF1 *mfPow = new TF1("mfPow", "[0]*TMath::Power(x, [1])", 0.1, 1);

   mfPow->SetParNames("P_{max}", "r");
   mfPow->SetParameter(0, 0.5);
   mfPow->SetParLimits(0, 0, 1);
   mfPow->SetParameter(1, 0.1);
   mfPow->SetParLimits(1, -2, 2);

   havrg->Fit("mfPow", "M E R");

   havrg->Print();
   havrg->GetYaxis()->SetRangeUser(0, 1.05);
   //havrg->Draw();
   //gPad->Update();
   //gPad->SaveAs("15221.2.v4.png");


   Double_t xmin, ymin, xmax, ymax;
   grPolar->ComputeRange(xmin, ymin, xmax, ymax);
   hPolar->GetListOfFunctions()->Add(grPolar, "p");
   hPolar->GetXaxis()->SetLimits(xmin, xmax);
   hPolar->GetYaxis()->SetLimits(ymin, ymax);
   hPolar->Draw();

   gPad->SaveAs("polar_Y1D_250.png");
}
