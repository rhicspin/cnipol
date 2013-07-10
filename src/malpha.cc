#include <cstdlib>
#include <string>
#include <map>
#include <vector>

#include "malpha.h"

#include "TEnv.h"
#include "TROOT.h"
#include "TLegend.h"

#include "MAlphaAnaInfo.h"
#include "AsymHeader.h"
#include "MeasInfo.h"

#include "utils/utils.h"

using namespace std;

typedef double Time;
typedef string RunName;
typedef int    DetectorId;
struct ResultMean
{
   map<Time, double>	first;
   map< Time, vector<double> >	second;
   string	YTitle;
};


/** */
void FillFromHist(TH1F *h, double startTime, ResultMean &result, ResultMean &result_err)
{
   // there are few alpha runs which don't have any data in their channels, skip them
   if ((!h) || (!h->GetEntries())) return;

   TFitResultPtr fitres = h->Fit("pol0", "S"); // S: return fitres

   result.first[startTime] = fitres->Value(0);
   result_err.first[startTime] = fitres->FitResult::Error(0);

   result.second[startTime].resize(N_DETECTORS);
   result_err.second[startTime].resize(N_DETECTORS);

   for(int det = 0; det < N_DETECTORS; det++)
	{
      int xmin = 0.5 + det * NSTRIP_PER_DETECTOR;
      int xmax = xmin + NSTRIP_PER_DETECTOR;

      TFitResultPtr fitres = h->Fit("pol0", "S", "", xmin, xmax); // S: return fitres

      if (fitres.Get())
      {
         result.second[startTime][det] = fitres->Value(0);
         result_err.second[startTime][det] = fitres->FitResult::Error(0);
      } else {
         result.second[startTime][det] = NAN;
         result_err.second[startTime][det] = NAN;
      }
   }

   result.YTitle = h->GetYaxis()->GetTitle();
}


/** */
void PlotMean(const char *name, ResultMean &result, ResultMean &result_err, map<Time, RunName> &runNameD, double min_startTime, double max_startTime)
{
   double min_value = FLT_MAX, max_value = -FLT_MAX;

   TH1F  *h;

   if (max_startTime) {
       h = new TH1F(name, name, 100 * result.first.size(), -86400, max_startTime - min_startTime + 86400);
   } else {
       h = new TH1F(name, name, result.first.size(), 0.0, 1.0);
   }

   int bin = 1;
   for (map< Time, double >::iterator it = result.first.begin(); it != result.first.end(); it++) {
      double startTime = it->first;
      const RunName &runName = runNameD[startTime];
      double value = it->second;

      if (max_startTime) {
         bin = h->FindBin(startTime - min_startTime);
      }

      h->SetBinContent(bin, value);
      h->SetBinError(bin, result_err.first[startTime]);

      if (!max_startTime) {
         h->GetXaxis()->SetBinLabel(bin, runName.c_str());
         bin++;
      }
   }

   if (max_startTime) {
      h->GetXaxis()->SetTimeDisplay(1);
      h->GetXaxis()->SetTimeFormat("%d.%m.%Y");
      h->GetXaxis()->SetTimeOffset(min_startTime);
   }
   h->SetYTitle(result.YTitle.c_str());


   for(int det = 0; det < N_DETECTORS; det++) {
      for (map< Time, vector<double> >::iterator it = result.second.begin(); it != result.second.end(); it++) {
         double value = it->second[det];
         if (min_value > value) {
            min_value = value;
         }
         if (max_value < value) {
            max_value = value;
         }
     }
   }

   if (!max_startTime)
   {
      TH1F *hdet = 0;
      for(int det = 0; det < N_DETECTORS; det++) {
         TString hname(name);
         hname += (det + 1);
         hdet = new TH1F(hname, hname, 100, min_value, max_value);
         hdet->SetXTitle(h->GetYaxis()->GetTitle());
         for (map< Time, vector<double> >::iterator it = result.second.begin(); it != result.second.end(); it++) {
            double value = it->second[det];
            hdet->Fill(value);
         }
      }
   }

   string hostName = "det_" + string(name);
   const char *hostNameStr = hostName.c_str();
   TCanvas c(hostNameStr);
   TLegend leg(0.7,0.1,0.9,0.4);
   leg.SetHeader("The Legend Title");
   TH1F  *host;
   if (max_startTime) {
       host = new TH1F("host", hostNameStr, 100 * result.first.size(), -86400, max_startTime - min_startTime + 86400);
   } else {
       host = new TH1F("host", hostNameStr, result.first.size(), 0.0, result.first.size());
   }

   for(int det = 0; det < N_DETECTORS; det++) {
      TGraphErrors *g = new TGraphErrors(result.second.size());
      g->SetLineColor(det + 2);
      int i = 0;
      double xval = -0.5;
      TString sDet("Detector");
      sDet += (det + 1);
      g->SetName(sDet);

      for (map< Time, vector<double> >::iterator it = result.second.begin(); it != result.second.end(); it++) {
         double startTime = it->first;
         const RunName &runName = runNameD[startTime];
         double value = it->second[det];

         if (max_startTime) {
            xval = startTime - min_startTime;
         } else {
            xval++;
            host->GetXaxis()->SetBinLabel(i + 1, runName.c_str());
         }

         if (isnan(value))
         {
            g->RemovePoint(0);
            continue;
         }

         g->SetPoint(i, xval, value);
         g->SetPointError(i, 0, result_err.second[startTime][det]);
         i++;
      }

      host->GetListOfFunctions()->Add(g, "pl");
      leg.AddEntry(g, sDet, "pl");
   }

   host->SetOption("DUMMY GRIDX GRIDY");
   host->GetXaxis()->SetTimeDisplay(1);
   host->GetXaxis()->SetTimeFormat("%d.%m.%Y");
   host->GetXaxis()->SetTimeOffset(min_startTime);
   host->SetYTitle(h->GetYaxis()->GetTitle());

   double vpadding = (max_value - min_value) * 0.4;
   h->GetYaxis()->SetRangeUser(min_value - vpadding, max_value + vpadding);
   host->GetYaxis()->SetRangeUser(min_value - vpadding, max_value + vpadding);

   host->Draw();
   leg.Draw();
   c.Write();
   delete host;
}


/** */
int main(int argc, char *argv[])
{
   setbuf(stdout, NULL);

   // Create a default one
   gMeasInfo = new MeasInfo();

   // Do not attempt to recover files
   gEnv->SetValue("TFile.Recover", 0);

   MAlphaAnaInfo mAlphaAnaInfo;
   mAlphaAnaInfo.ProcessOptions(argc, argv);
   mAlphaAnaInfo.VerifyOptions();

   gROOT->Macro("~/rootmacros/styles/style_malpha.C");

   gRunConfig.fBeamEnergies.clear();
   gRunConfig.fBeamEnergies.insert(kINJECTION);
   gRunConfig.fBeamEnergies.insert(kBEAM_ENERGY_255);

   string filelist = mAlphaAnaInfo.GetMListFullPath();

   Info("malpha", "Starting first pass...");

   map< Short_t, ResultMean > result_am_amgd_mean;
   map< Short_t, ResultMean > result_am_amgd_mean_err;
   map< Short_t, ResultMean > result_fit0mean;
   map< Short_t, ResultMean > result_fit0mean_err;
   map< Short_t, ResultMean > rDeadLayerEnergy;
   map< Short_t, ResultMean > rDeadLayerEnergyErr;
   double max_startTime = -1;
   double min_startTime = -1;
   map< Short_t, map<Time, string> > runNameD;

   // Fill chain with all input files from filelist
   TObject *o;
   TIter   *next = new TIter(utils::getFileList(filelist));

   // Loop over the runs and record the time of the last flattop measurement in the fill
   while (next && (o = (*next)()) )
	{
      string measId   = string(((TObjString*) o)->GetName());
      string fileName = mAlphaAnaInfo.GetResultsDir() + "/" + measId + "/" + measId + mAlphaAnaInfo.GetSuffix() + ".root";

      TFile *f = new TFile(fileName.c_str(), "READ");

      if (!f) {
         Error("malpha", "file not found. Skipping...");
         delete f;
         continue;
      }

      if (f->IsZombie()) {
         Error("malpha", "file is zombie %s. Skipping...", fileName.c_str());
         f->Close();
         delete f;
         continue;
      }

      Info("malpha", "Found file: %s", fileName.c_str());

      EventConfig *gMM = (EventConfig*) f->FindObjectAny("measConfig");

      if (!gMM) {
         Error("malpha", "MM not found. Skipping...");
         f->Close();
         delete f;
         continue;
      }

      int      alphaSources = gMM->fMeasInfo->GetAlphaSourceCount();
      string   runName      = gMM->fMeasInfo->GetRunName();
      Short_t  polId        = gMM->fMeasInfo->fPolId;
      Double_t startTime    = gMM->fMeasInfo->fStartTime;

      if (alphaSources != 2) {
         Info("malpha", "Not enough alpha sources in %s. Skipping", fileName.c_str());
         continue;
      }

      if (gMM->fMeasInfo->RUNID == 70213) {
         Info("malpha", "File %s is blacklisted. Skipping", fileName.c_str());
         continue;
      }

      if (runNameD[polId].count(startTime)) {
         Error("malpha", "Duplicate time = %f, %s, %s",
            startTime, runName.c_str(), runNameD[polId][startTime].c_str());
         exit(EXIT_FAILURE);
      }
      runNameD[polId][startTime] = runName;

      if ((max_startTime == -1) || (max_startTime < startTime)) {
         max_startTime = startTime;
      }
      if ((min_startTime == -1) || (min_startTime > startTime)) {
         min_startTime = startTime;
      }

      TH1F  *hAmGdFit0Coef = (TH1F*) f->FindObjectAny("hAmGdFit0Coef");
      TH1F  *hAmGdAmpCoef_over_AmAmpCoef = (TH1F*) f->FindObjectAny("hAmGdAmpCoef_over_AmAmpCoef");
      TH1F  *hDeadLayerEnergy = (TH1F*) f->FindObjectAny("hDeadLayerEnergy");

      FillFromHist(hAmGdFit0Coef, startTime, result_fit0mean[polId], result_fit0mean_err[polId]);
      FillFromHist(hAmGdAmpCoef_over_AmAmpCoef, startTime, result_am_amgd_mean[polId], result_am_amgd_mean_err[polId]);
      FillFromHist(hDeadLayerEnergy, startTime, rDeadLayerEnergy[polId], rDeadLayerEnergyErr[polId]);

      f->Close();
      delete f;
   }

   TFile f1(mAlphaAnaInfo.fOutputFileName.c_str(), "RECREATE");

   PolarimeterIdSetConstIter iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
	{
      Short_t polId = *iPolId;
      string  polIdName = RunConfig::AsString(*iPolId);

      TDirectory *fDir = f1.mkdir(polIdName.c_str());
      fDir->cd();

      PlotMean("hAmGdFit0Coef_by_day", result_fit0mean[polId], result_fit0mean_err[polId], runNameD[polId], min_startTime, max_startTime);
      PlotMean("hAmGdGain_over_AmGain_by_day", result_am_amgd_mean[polId], result_am_amgd_mean_err[polId], runNameD[polId], min_startTime, max_startTime);
      PlotMean("hDeadLayerEnergy_by_day", rDeadLayerEnergy[polId], rDeadLayerEnergyErr[polId], runNameD[polId], min_startTime, max_startTime);
      PlotMean("hAmGdFit0Coef_by_run", result_fit0mean[polId], result_fit0mean_err[polId], runNameD[polId], 0, 0);
      PlotMean("hAmGdGain_over_AmGain_by_run", result_am_amgd_mean[polId], result_am_amgd_mean_err[polId], runNameD[polId], 0, 0);
      PlotMean("hDeadLayerEnergy_by_run", rDeadLayerEnergy[polId], rDeadLayerEnergyErr[polId], runNameD[polId], 0, 0);
   }

   f1.Write();

   return EXIT_SUCCESS;
}
