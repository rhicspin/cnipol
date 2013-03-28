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


/** */
TH1F* PlotMean(const char *name, map< double, double > &result, map< double, double > &result_err, double min_startTime, double max_startTime)
{
   double min_value = FLT_MAX, max_value = -FLT_MAX;

   TH1F  *h = new TH1F(name, name, 100 * result.size(), -86400, max_startTime - min_startTime + 86400);
   for (map< double, double >::iterator it = result.begin(); it != result.end(); it++) {
      double startTime = it->first;

      if (min_value > result[startTime]) {
         min_value = result[startTime];
      }
      if (max_value < result[startTime]) {
         max_value = result[startTime];
      }

      h->SetBinContent(
         h->FindBin(startTime - min_startTime),
         result[startTime]
      );
      h->SetBinError(
         h->FindBin(startTime - min_startTime),
         result_err[startTime]
      );
   }
   h->GetXaxis()->SetTimeDisplay(1);
   h->GetXaxis()->SetTimeFormat("%d.%m.%Y");
   h->GetXaxis()->SetTimeOffset(min_startTime);

   double vpadding = (max_value - min_value) * 0.4;
   h->GetYaxis()->SetRangeUser(min_value - vpadding, max_value + vpadding);

   return h;
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

   map< Short_t, map< double, double > > result_am_amgd_mean;
   map< Short_t, map< double, double > > result_am_amgd_mean_err;
   map< Short_t, map< double, double > > result_fit0mean;
   map< Short_t, map< double, double > > result_fit0mean_err;
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

      TH1F  *hAmGdAmpCoef_over_AmAmpCoef = (TH1F*) f->FindObjectAny("hAmGdAmpCoef_over_AmAmpCoef");
      TH1F  *hAmGdFit0Coef = (TH1F*) f->FindObjectAny("hAmGdFit0Coef");

      {
         TFitResultPtr fitres = hAmGdFit0Coef->Fit("pol0", "S"); // S: return fitres
         result_fit0mean[polId][startTime] = fitres->Value(0);
         result_fit0mean_err[polId][startTime] = fitres->FitResult::Error(0);
      }

      {
         TFitResultPtr fitres = hAmGdAmpCoef_over_AmAmpCoef->Fit("pol0", "S"); // S: return fitres
         result_am_amgd_mean[polId][startTime] = fitres->Value(0);
         result_am_amgd_mean_err[polId][startTime] = fitres->FitResult::Error(0);
      }

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
