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
void FillFromHist(TH1F *h, double runId, map< double, vector<double> > &result)
{
   Int_t xfirst  =  h->GetXaxis()->GetFirst();
   Int_t xlast   =  h->GetXaxis()->GetLast();

   assert(h->GetNbinsX() == N_SILICON_CHANNELS);
   if (result.count(runId) == 0) {
      result[runId].resize(N_SILICON_CHANNELS);
      for (int i = xfirst; i <= xlast; i++) {
         result[runId][i - xfirst] = h->GetBinContent(i);
      }
   }
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
   map< double, vector<double> > result_gd;
   double max_runId = -1;
   double min_runId = -1;

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

      Double_t    runId           = gMM->fMeasInfo->RUNID;
      UInt_t      fillId          = (UInt_t) runId;
      EBeamEnergy beamEnergy      = gMM->fMeasInfo->GetBeamEnergy();

      if ((gMM->fMeasInfo->fPolBeam != 1) || (gMM->fMeasInfo->fPolStream != 1)) {
         continue;
      }

      if ((max_runId == -1) || (max_runId < runId)) {
         max_runId = runId;
      }
      if ((min_runId == -1) || (min_runId > runId)) {
         min_runId = runId;
      }

      TH1F  *hAmAmpCoef = (TH1F*) f->FindObjectAny("hAmAmpCoef");
      TH1F  *hGdAmpCoef = (TH1F*) f->FindObjectAny("hGdAmpCoef");

      FillFromHist(hAmAmpCoef, runId, result_am);
      FillFromHist(hGdAmpCoef, runId, result_gd);

      f->Close();
      delete f;
   }

   TFile f1(mAlphaAnaInfo.fOutputFileName.c_str(), "NEW");

   for (int channel_id = 0; channel_id < N_SILICON_CHANNELS; channel_id++) {
      TString obj_name("AmGain_over_GdGain_");
      obj_name += channel_id;
      const char *obj_name_cstr = obj_name.Data();

      TH1F  h(obj_name_cstr, obj_name_cstr, 10 * result_am.size(), min_runId * 0.9, max_runId * 1.1);
      for (map< double, vector<double> >::iterator it = result_am.begin(); it != result_am.end(); it++) {
         double runId = it->first;

         h.SetBinContent(h.FindBin(runId), result_am[runId][channel_id] / result_gd[runId][channel_id]);
      }
      h.Draw();

      h.Write(obj_name_cstr);
   }

   return EXIT_SUCCESS;
}
