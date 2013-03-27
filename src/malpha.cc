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
void FillFromHist(TH1F *h, double startTime, map< double, vector<double> > &result, map< double, vector<double> > &result_err)
{
   Int_t xfirst  =  h->GetXaxis()->GetFirst();
   Int_t xlast   =  h->GetXaxis()->GetLast();

   assert(h->GetNbinsX() == N_SILICON_CHANNELS);
   if (result.count(startTime) == 0) {
      result[startTime].resize(N_SILICON_CHANNELS);
      result_err[startTime].resize(N_SILICON_CHANNELS);
      for (int i = xfirst; i <= xlast; i++) {
         result[startTime][i - xfirst] = h->GetBinContent(i);
         result_err[startTime][i - xfirst] = h->GetBinError(i);
      }
   }
}


/** */
void PlotMean(const char *name, map< double, double > &result, map< double, double > &result_err, double min_startTime, double max_startTime)
{
   TH1F  h(name, name, 10 * result.size(), -86400, max_startTime - min_startTime + 86400);
   for (map< double, double >::iterator it = result.begin(); it != result.end(); it++) {
      double startTime = it->first;

      h.SetBinContent(
         h.FindBin(startTime - min_startTime),
         result[startTime]
      );
      h.SetBinError(
         h.FindBin(startTime - min_startTime),
         result_err[startTime]
      );
   }
   h.GetXaxis()->SetTimeDisplay(1);
   h.GetXaxis()->SetTimeFormat("%d.%m.%Y");
   h.GetXaxis()->SetTimeOffset(min_startTime);

   h.Write(name);
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

   // Create a default canvas here to get rid of weird root messages while
   // reading objects from root files
   TCanvas canvas("canvas", "canvas", 1400, 600);

   map< double, vector<double> > result_am;
   map< double, vector<double> > result_amgd;
   map< double, vector<double> > result_fit0;
   map< double, vector<double> > result_am_err;
   map< double, vector<double> > result_amgd_err;
   map< double, vector<double> > result_fit0_err;
   map< double, double > result_am_amgd_mean;
   map< double, double > result_am_amgd_mean_err;
   map< double, double > result_fit0mean;
   map< double, double > result_fit0mean_err;
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
      Double_t    startTime       = gMM->fMeasInfo->fStartTime;
      EBeamEnergy beamEnergy      = gMM->fMeasInfo->GetBeamEnergy();

      if (alphaSources != 2)
      {
          Info("malpha", "Not enough alpha sources in %s. Skipping", fileName.c_str());
      }

      if ((gMM->fMeasInfo->fPolBeam != 1) || (gMM->fMeasInfo->fPolStream != 1)) {
         continue;
      }

      Info("malpha", "Using file: %s", fileName.c_str());

      if ((max_startTime == -1) || (max_startTime < startTime)) {
         max_startTime = startTime;
      }
      if ((min_startTime == -1) || (min_startTime > startTime)) {
         min_startTime = startTime;
      }

      TH1F  *hAmAmpCoef = (TH1F*) f->FindObjectAny("hAmAmpCoef");
      TH1F  *hAmGdAmpCoef = (TH1F*) f->FindObjectAny("hAmGdAmpCoef");
      TH1F  *hAmGdAmpCoef_over_AmAmpCoef = (TH1F*) f->FindObjectAny("hAmGdAmpCoef_over_AmAmpCoef");
      TH1F  *hAmGdFit0Coef = (TH1F*) f->FindObjectAny("hAmGdFit0Coef");

      FillFromHist(hAmAmpCoef, startTime, result_am, result_am_err);
      FillFromHist(hAmGdAmpCoef, startTime, result_amgd, result_amgd_err);
      FillFromHist(hAmGdFit0Coef, startTime, result_fit0, result_fit0_err);

      {
         TFitResultPtr fitres = hAmGdFit0Coef->Fit("pol0", "S"); // S: return fitres
         result_fit0mean[startTime] = fitres->Value(0);
         result_fit0mean_err[startTime] = fitres->FitResult::Error(0);
      }

      {
         TFitResultPtr fitres = hAmGdAmpCoef_over_AmAmpCoef->Fit("pol0", "S"); // S: return fitres
         result_am_amgd_mean[startTime] = fitres->Value(0);
         result_am_amgd_mean_err[startTime] = fitres->FitResult::Error(0);
      }

      f->Close();
      delete f;
   }

   TFile f1(mAlphaAnaInfo.fOutputFileName.c_str(), "RECREATE");

   for (int channel_id = 0; channel_id < N_SILICON_CHANNELS; channel_id++) {
      TString obj_name("AmGain_over_AmGdGain_");
      obj_name += channel_id;
      const char *obj_name_cstr = obj_name.Data();

      TH1F  h(obj_name_cstr, obj_name_cstr, 10 * result_am.size(), -86400, max_startTime - min_startTime + 86400);
      for (map< double, vector<double> >::iterator it = result_am.begin(); it != result_am.end(); it++) {
         double startTime = it->first;
         double value = result_am[startTime][channel_id] / result_amgd[startTime][channel_id];

         h.SetBinContent(
            h.FindBin(startTime - min_startTime),
            value
         );
         h.SetBinError(
            h.FindBin(startTime - min_startTime),
            value *
            (result_am_err[startTime][channel_id] / result_am[startTime][channel_id]
            + result_amgd_err[startTime][channel_id] / result_amgd[startTime][channel_id])
         );
      }
      h.GetXaxis()->SetTimeDisplay(1);
      h.GetXaxis()->SetTimeFormat("%d.%m.%Y");
      h.GetXaxis()->SetTimeOffset(min_startTime);
      h.Draw();

      h.Write(obj_name_cstr);
   }

   PlotMean("hAmGdFit0Coef", result_fit0mean, result_fit0mean_err, min_startTime, max_startTime);
   PlotMean("hAmGdAmpCoef_over_AmAmpCoef", result_am_amgd_mean, result_am_amgd_mean_err, min_startTime, max_startTime);

   return EXIT_SUCCESS;
}
