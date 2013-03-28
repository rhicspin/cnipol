#include <cstdlib>
#include <string>
#include <map>
#include <vector>

#include "malpha.h"

#include "TEnv.h"
#include "TROOT.h"

#include "MAlphaAnaInfo.h"
#include "AsymHeader.h"
#include "MeasInfo.h"

#include "utils/utils.h"

using namespace std;

typedef double Time;
typedef int    DetectorId;
typedef pair<
   map<Time, double>,
   map< Time, vector<double> >
> ResultMean;


/** */
void FillFromHist(TH1F *h, double startTime, ResultMean &result, ResultMean &result_err)
{
   TFitResultPtr fitres = h->Fit("pol0", "S"); // S: return fitres

   result.first[startTime] = fitres->Value(0);
   result_err.first[startTime] = fitres->FitResult::Error(0);

   result.second[startTime].resize(N_DETECTORS);
   result_err.second[startTime].resize(N_DETECTORS);

   for(int det = 0; det < N_DETECTORS; det++) {
      int xmin = 0.5 + det * NSTRIP_PER_DETECTOR;
      int xmax = xmin + NSTRIP_PER_DETECTOR;

      TFitResultPtr fitres = h->Fit("pol0", "S", "", xmin, xmax); // S: return fitres

      result.second[startTime][det] = fitres->Value(0);
      result_err.second[startTime][det] = fitres->FitResult::Error(0);
   }
}


/** */
void PlotMean(const char *name, ResultMean &result, ResultMean &result_err, double min_startTime, double max_startTime)
{
   double min_value = FLT_MAX, max_value = -FLT_MAX;

   TH1F  *h = new TH1F(name, name, 100 * result.first.size(), -86400, max_startTime - min_startTime + 86400);
   for (map< Time, double >::iterator it = result.first.begin(); it != result.first.end(); it++) {
      double startTime = it->first;
      double value = it->second;

      h->SetBinContent(
         h->FindBin(startTime - min_startTime),
         value
      );
      h->SetBinError(
         h->FindBin(startTime - min_startTime),
         result_err.first[startTime]
      );
   }
   h->GetXaxis()->SetTimeDisplay(1);
   h->GetXaxis()->SetTimeFormat("%d.%m.%Y");
   h->GetXaxis()->SetTimeOffset(min_startTime);

   string hostNameStr = string(name) + "_Per_Detector";
   const char *hostName = hostNameStr.c_str();
   TH1F  *host = new TH1F(hostName, hostName, 100 * result.first.size(), -86400, max_startTime - min_startTime + 86400);

   for(int det = 0; det < N_DETECTORS; det++) {
      TGraphErrors *g = new TGraphErrors(result.second.size());
      g->SetLineColor(det + 2);
      int i = 0;
      TString sDet(hostNameStr);
      sDet += det;
      g->SetName(sDet);

      for (map< Time, vector<double> >::iterator it = result.second.begin(); it != result.second.end(); it++) {
         double startTime = it->first;
         vector<double> &values = it->second;

         if (min_value > values[det]) {
            min_value = values[det];
         }
         if (max_value < values[det]) {
            max_value = values[det];
         }

         g->SetPoint(i, startTime - min_startTime, values[det]);
         g->SetPointError(i, 0, result_err.second[startTime][det]);
         i++;
      }

      host->GetListOfFunctions()->Add(g, "pl");
   }

   host->SetOption("DUMMY GRIDX GRIDY");
   host->GetXaxis()->SetTimeDisplay(1);
   host->GetXaxis()->SetTimeFormat("%d.%m.%Y");
   host->GetXaxis()->SetTimeOffset(min_startTime);

   double vpadding = (max_value - min_value) * 0.4;
   h->GetYaxis()->SetRangeUser(min_value - vpadding, max_value + vpadding);
   host->GetYaxis()->SetRangeUser(min_value - vpadding, max_value + vpadding);
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
   double max_startTime = -1;
   double min_startTime = -1;

   // Fill chain with all input files from filelist
   TObject *o;
   TIter   *next = new TIter(utils::getFileList(filelist));

   // Loop over the runs and record the time of the last flattop measurement in the fill
   while (next && (o = (*next)()) ) {
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

      int         alphaSources    = gMM->fAnaInfo->fAlphaSourceCount;
      Short_t     polId           = gMM->fMeasInfo->fPolId;
      Double_t    startTime       = gMM->fMeasInfo->fStartTime;
      EBeamEnergy beamEnergy      = gMM->fMeasInfo->GetBeamEnergy();

      if (alphaSources != 2) {
         Info("malpha", "Not enough alpha sources in %s. Skipping", fileName.c_str());
         continue;
      }

      if (gMM->fMeasInfo->RUNID == 70213) {
         Info("malpha", "File %s is blacklisted. Skipping", fileName.c_str());
         continue;
      }

      if ((max_startTime == -1) || (max_startTime < startTime)) {
         max_startTime = startTime;
      }
      if ((min_startTime == -1) || (min_startTime > startTime)) {
         min_startTime = startTime;
      }

      TH1F  *hAmGdFit0Coef = (TH1F*) f->FindObjectAny("hAmGdFit0Coef");
      TH1F  *hAmGdAmpCoef_over_AmAmpCoef = (TH1F*) f->FindObjectAny("hAmGdAmpCoef_over_AmAmpCoef");

      FillFromHist(hAmGdFit0Coef, startTime, result_fit0mean[polId], result_fit0mean_err[polId]);
      FillFromHist(hAmGdAmpCoef_over_AmAmpCoef, startTime, result_am_amgd_mean[polId], result_am_amgd_mean_err[polId]);

      f->Close();
      delete f;
   }

   TFile f1(mAlphaAnaInfo.fOutputFileName.c_str(), "RECREATE");

   PolarimeterIdSetConstIter iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId) {
      Short_t polId = *iPolId;
      string  polIdName = RunConfig::AsString(*iPolId);

      TDirectory *fDir = f1.mkdir(polIdName.c_str());
      fDir->cd();

      PlotMean("hAmGdFit0Coef", result_fit0mean[polId], result_fit0mean_err[polId], min_startTime, max_startTime);
      PlotMean("hAmGdAmpCoef_over_AmAmpCoef", result_am_amgd_mean[polId], result_am_amgd_mean_err[polId], min_startTime, max_startTime);
   }

   f1.Write();

   return EXIT_SUCCESS;
}
