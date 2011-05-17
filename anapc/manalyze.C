
#include <time.h>
#include <map>

#include "manalyze.h"

#include "MAsymFillHists.h"
#include "MAsymRunHists.h"
#include "MAsymRateHists.h"

#include "utils/utils.h"

using namespace std;

//MAsymRateHists       *gH;
DrawObjContainer    *gH;

void manalyze()
{
   initialize();
}


void initialize()
{
   gStyle->SetOptTitle(0);
   gStyle->SetOptStat(0);
   gStyle->SetPadRightMargin(0.18);

   gMAsymRoot = new MAsymRoot("masym_out.root");

   gH = new DrawObjContainer(gMAsymRoot);

   gH->d["fills"] = new MAsymFillHists(new TDirectoryFile("fills", "fills", "", gMAsymRoot));
   gH->d["rate"]  = new MAsymRateHists(new TDirectoryFile("rate",  "rate",  "", gMAsymRoot));
   gH->d["runs"]  = new MAsymRunHists (new TDirectoryFile("runs",  "runs",  "", gMAsymRoot));

   struct tm tm;
   time_t firstDay;

   if ( strptime("2011-01-01 00:00:00", "%Y-%m-%d %H:%M:%S", &tm) != NULL )
      firstDay = mktime(&tm);

   //printf("firstDay %d\n", firstDay);

   TString filelistPath("/eic/u/dsmirnov/run/");
   Color_t color = kRed;
   TString filelist = filelistPath + "runs11_rampupdown.dat";

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

   TH1* hProfR = new TH1F("hProfR", "hProfR", 1, 0, 1);

   // 
   TH2* hPolar = new TH2F("hPolar", "hPolar", 1, 20, 100, 1, 20, 80);
   hPolar->GetXaxis()->SetTitle("Days");
   hPolar->GetYaxis()->SetTitle("Polarization, %");

   TGraphErrors *grPolar = new TGraphErrors();
   grPolar->SetName("grPolar");
   grPolar->SetMarkerStyle(kFullCircle);
   grPolar->SetMarkerSize(1);
   grPolar->SetMarkerColor(color);

   TH2* hPolarVsTime = new TH2F("hPolarVsTime", "hPolarVsTime", 1, 20, 100, 1, 20, 80);
   hPolarVsTime->GetXaxis()->SetTitle("Date/Time");
   hPolarVsTime->GetYaxis()->SetTitle("Polarization, %");

   TGraphErrors *grPolarVsTime = new TGraphErrors();
   grPolarVsTime->SetName("grPolarVsTime");
   grPolarVsTime->SetMarkerStyle(kFullCircle);
   grPolarVsTime->SetMarkerSize(1);
   grPolarVsTime->SetMarkerColor(color);

   TH2* hRVsTime = new TH2F("hRVsTime", "hRVsTime", 1, 20, 100, 1, -0.1, 1);
   hRVsTime->GetXaxis()->SetTitle("Days");
   hRVsTime->GetYaxis()->SetTitle("r");

   TGraphErrors *grRVsTime = new TGraphErrors();
   grRVsTime->SetName("grRVsTime");
   grRVsTime->SetMarkerStyle(kFullCircle);
   grRVsTime->SetMarkerSize(1);
   grRVsTime->SetMarkerColor(color);

   UInt_t i = 0;

   // Fill chain with all input files from filelist
   TObject *o;
   TIter   *next = new TIter(utils::getFileList(filelist));

   // Loop over the runs and record the time of the last flattop measurement in the fill
   while (next && (o = (*next)()) )
   {
      TString fileName = "/data1/run11/root/" + string(((TObjString*) o)->GetName()) + "/" + string(((TObjString*) o)->GetName()) + ".root";

      TFile *f = new TFile(fileName, "READ");

      if (!f) {
         gSystem->Error("", "file not found\n");
         continue;
      }

      gSystem->Info("", "file found: %s", fileName.Data());

      gRC = (EventConfig*) f->FindObjectAny("EventConfig");
      delete f;

      if (!gRC) {
         gSystem->Error("", "RC not found\n");
         continue;
      }

      UInt_t beamEnergy = (UInt_t) (gRC->fRunInfo->fBeamEnergy + 0.5);
      UInt_t fillId = (UInt_t) gRC->fRunInfo->RUNID;

      if (flattopTimes.find(fillId) == flattopTimes.end()) 
         flattopTimes[fillId] = 0;

      if ( beamEnergy == 250 && gRC->fRunInfo->StartTime > flattopTimes[fillId]) {
         flattopTimes[fillId] = gRC->fRunInfo->StartTime;
      }
   }

   map<UInt_t, UInt_t>::iterator ift;

   for (ift=flattopTimes.begin(); ift!=flattopTimes.end(); ++ift) {
      printf("%d -> %d\n", ift->first, ift->second);
   }


   //return;
   next->Begin();
   //next->Reset();
   //*next = next->Begin();

   while (next && (o = (*next)()) )
   {
      TString fileName = "/data1/run11/root/" + string(((TObjString*) o)->GetName()) + "/" + string(((TObjString*) o)->GetName()) + ".root";

      TFile *f = new TFile(fileName, "READ");

      if (!f) {
         gSystem->Error("", "file not found\n");
         continue; //exit(-1);
      }

      gSystem->Info("", "file found: %s", fileName.Data());

      gRC = (EventConfig*) f->FindObjectAny("EventConfig");
      delete f;

      if (!gRC) {
         gSystem->Error("", "RC not found\n");
         continue;
      }

      //gRC->Print();

      //Double_t energy = gRC->fRunInfo->fBeamEnergy;
      //printf("%f, %d\n", energy, beamEnergy);

      //gH = new DrawObjContainer(f);
      //gH->d["profile"] = new CnipolProfileHists();
      //gH->ReadFromDir(f);

      //TH1* h = (TH1*) gH->d["profile"]->o[histName];
      //h->Print();
      //h->SetBit(TH1::kIsAverage);

      Double_t day = (gRC->fRunInfo->StartTime - firstDay)/60./60./24.;
      char strTime[80];
      strftime(strTime, 80, "%X", localtime(&gRC->fRunInfo->StartTime));

      //grPolar->SetPoint(i, gRC->fRunInfo->RUNID, gRC->fAnaResult->sinphi[0].P[0]);

      Double_t runId            = gRC->fRunInfo->RUNID;
      UInt_t   fillId           = (UInt_t) runId;
      UInt_t   beamEnergy       = (UInt_t) (gRC->fRunInfo->fBeamEnergy + 0.5);
      Float_t  ana_power        = gRC->fAnaResult->A_N[1];
      Float_t  asymmetry        = gRC->fAnaResult->sinphi[0].P[0] * gRC->fAnaResult->A_N[1];
      Float_t  asymmetry_err    = gRC->fAnaResult->sinphi[0].P[1] * gRC->fAnaResult->A_N[1];
      Float_t  polarization     = gRC->fAnaResult->sinphi[0].P[0] * 100.;
      Float_t  polarization_err = gRC->fAnaResult->sinphi[0].P[1] * 100.;
      Float_t  profileRatio     = gRC->fAnaResult->fIntensPolarR;
      Float_t  profileRatioErr  = gRC->fAnaResult->fIntensPolarRErr;
      Float_t  tzero            = gRC->fCalibrator->fChannelCalibs[7].fT0Coef;
      Float_t  tzeroErr         = gRC->fCalibrator->fChannelCalibs[7].fT0CoefErr;

      // Substitute the beam energy for special ramp fills.
      // XXX Comment this for normal summary reports
      //if ( beamEnergy == 100 && gRC->fRunInfo->StartTime > flattopTimes[fillId]) {
      //   gRC->fRunInfo->fBeamEnergy = 400;
      //   beamEnergy = 400;
      //}

      //printf("tzero: %f %f %f %d %f \n", tzero, tzeroErr, runId, gRC->fRunInfo->StartTime, asymmetry);
      printf("%8.3f, %s, %3d, %f, %f, %f, %f, %f\n", runId, strTime,
         beamEnergy, asymmetry, asymmetry_err, ana_power, polarization,
         polarization_err);

      if (polarization <= 1 || polarization_err > 5 || polarization > 99.99) {
         continue;
      }

      gH->Fill(*gRC);

      //grPolar->SetPoint(i, day, tzero);
      //grPolar->SetPointError(i, 0, tzeroErr);

      grPolar->SetPoint(i, day, polarization);
      grPolar->SetPointError(i, 0, polarization_err);

      grPolarVsTime->SetPoint(i, gRC->fRunInfo->StartTime, polarization);
      grPolarVsTime->SetPointError(i, 0, polarization_err);

      grRVsTime->SetPoint(i, gRC->fRunInfo->StartTime, profileRatio);
      grRVsTime->SetPointError(i, 0, profileRatioErr);
      
      //if (i == 0) havrg = h;
      //else havrg->Add(h);
      //havrg->Add(h);

      i++;
   }

   Double_t xmin, ymin, xmax, ymax;

   string imageName;
   TCanvas *c = new TCanvas("cName", "cName", 1200, 600);

   hPolar->GetListOfFunctions()->Add(grPolar, "p");
   hPolar->Draw();
   //imageName = "polar_" + filelist + "_" + sEnergyId + ".png";
   //c->SaveAs(imageName.c_str());

   hPolarVsTime->GetListOfFunctions()->Add(grPolarVsTime, "p");
   grPolarVsTime->ComputeRange(xmin, ymin, xmax, ymax);
   printf("xmin, xmax: %f, %f\n", xmin, xmax);
   //gStyle->SetTimeOffset();
   //hPolarVsTime->GetXaxis()->SetTimeOffset(6*3600, "local");
   hPolarVsTime->GetXaxis()->SetTimeOffset(0, "gmt");
   hPolarVsTime->GetXaxis()->SetLimits(xmin, xmax);
   hPolarVsTime->GetXaxis()->SetTimeDisplay(1);
   hPolarVsTime->GetXaxis()->SetTimeFormat("%b %d");
   hPolarVsTime->Draw();
   c->Update();
   //imageName = "hPolarVsTime_" + filelist + "_" + sEnergyId + ".png";
   //c->SaveAs(imageName.c_str());

   grRVsTime->ComputeRange(xmin, ymin, xmax, ymax);
   printf("xmin, xmax: %f, %f\n", xmin, xmax);
   TF1 *f2 = new TF1("f2", "[0]", xmin, xmax);
   grRVsTime->Fit("f2", "R");

   hRVsTime->GetListOfFunctions()->Add(grRVsTime, "p");
   hRVsTime->GetXaxis()->SetTimeOffset(0, "gmt");
   hRVsTime->GetXaxis()->SetLimits(xmin, xmax);
   hRVsTime->GetXaxis()->SetTimeDisplay(1);
   hRVsTime->GetXaxis()->SetTimeFormat("%b %d");
   hRVsTime->Draw();
   //imageName = "hRVsTime_" + filelist + "_" + sEnergyId + ".png";
   //c->SaveAs(imageName.c_str());

   gH->PostFill();

   gH->SetSignature(gRC->GetSignature());

   TCanvas canvas("cName2", "cName2", 1400, 600);
   gH->SaveAllAs(canvas);

   gH->Write();
   //gH->Delete();

   gMAsymRoot->Close();
}
