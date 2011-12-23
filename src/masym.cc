
#include <time.h>
#include <map>

#include "masym.h"

#include "TEnv.h"

#include "CnipolAsymHists.h"
#include "MAsymFillHists.h"
#include "MAsymRunHists.h"
#include "MAsymRateHists.h"
#include "MAsymPmtHists.h"

#include "AnaGlobResult.h"

#include "AsymDbSql.h"
#include "AsymGlobals.h"
#include "AnaInfo.h"
#include "MeasInfo.h"

#include "utils/utils.h"

using namespace std;

DrawObjContainer    *gHIn;
DrawObjContainer    *gH;
AnaInfo             *gAnaInfo;
set<string>          gGoodRuns;
AnaGlobResult        gAnaGlobResult;


int main(int argc, char *argv[])
{
   // Create a default one
   gMeasInfo = new RunInfo();

   // do not attept to recover files
   gEnv->SetValue("TFile.Recover", 0);

   initialize();

   return 1;
}


void initialize()
{
   gStyle->SetOptTitle(0);
   //gStyle->SetOptStat("emroui");
   gStyle->SetOptStat("e");
   gStyle->SetOptFit(1111);
   gStyle->SetPadRightMargin(0.18);

   TString filelistPath("/eic/u/dsmirnov/run/");

   //TString filelistName = "run09_10XXX_11XXX";
   //TString filelistName = "run09_tmp_longi_check_10373";
   //TString filelistName = "run09_tmp_longi_check_10490";
   //TString filelistName = "run09_tmp_longi_check_10532";
   //TString filelistName = "runs11_rampupdown";
   //TString filelistName = "runs_all";
   //TString filelistName = "run11_153XX_tmp";
   //TString filelistName = "run11_15393";
   //TString filelistName = "run11_15397";
   //TString filelistName = "run11_15399";
   //TString filelistName = "run11_15XXX_1XX_2XX_3XX_4XX";
   //TString filelistName = "run11_153XX";
   //TString filelistName = "run11_153XX_Y2U";
   //TString filelistName = "run11_pol_decay";
   //TString filelistName = "run11_1547X_4_5";
   //TString filelistName = "run11_154XX_00_23_before_rotators";
   TString filelistName = "run11_tmp_goodruns";
   //TString filelistName = "run11_tmp_goodruns_small";
   //TString filelistName = "run11_15473_74_75_injection";
   //TString filelistName = "run11_15XXX_Y1D_B2D_V_hama";

	TString filelist    = filelistPath + filelistName + ".txt";
   TString outFileName = "masym_" + filelistName + "_out.root";
   TString fileSuffix  = "";
   //TString fileSuffix  = "_hama";

   //std::find(gRunConfig.fBeamEnergies.begin(), gRunConfig.fBeamEnergies.end(), kBEAM_ENERGY_100);
   gRunConfig.fBeamEnergies.erase(kBEAM_ENERGY_100);

   gAnaInfo   = new AnaInfo();
   gMAsymRoot = new MAsymRoot(outFileName.Data());

   gH = new DrawObjContainer(gMAsymRoot);

   gH->d["fills"] = new MAsymFillHists(new TDirectoryFile("fills", "fills", "", gMAsymRoot));
   gH->d["rate"]  = new MAsymRateHists(new TDirectoryFile("rate",  "rate",  "", gMAsymRoot));
   gH->d["runs"]  = new MAsymRunHists (new TDirectoryFile("runs",  "runs",  "", gMAsymRoot));
   gH->d["pmt"]   = new MAsymPmtHists (new TDirectoryFile("pmt",   "pmt",   "", gMAsymRoot));

   string  histName = "hPolarVsIntensProfileBin";

   //UInt_t minTime = UINT_MAX;
   //UInt_t maxTime = 0;

   // Fill chain with all input files from filelist
   TObject *o;
   TIter   *next = new TIter(utils::getFileList(filelist));

   // Loop over the runs and record the time of the last flattop measurement in the fill
   while (next && (o = (*next)()) )
   {
      string  fName    = string(((TObjString*) o)->GetName());
      TString fileName = gAnaInfo->GetResultsDir() + "/" + fName + "/" + fName + fileSuffix + ".root";

      TFile *f = new TFile(fileName, "READ");

      if (!f) {
         gSystem->Error("masym", "file not found. Skipping...");
         continue;
      }

      if (f->IsZombie()) {
         gSystem->Error("masym", "file is zombie %s. Skipping...", fileName.Data());
         delete f;
         continue;
      }

      gSystem->Info("", "file found: %s", fileName.Data());

      gRC = (EventConfig*) f->FindObjectAny("EventConfig");

      if (!gRC) {
         gSystem->Error("masym", "RC not found. Skipping...");
         delete f;
         continue;
      }

      // Check that asym hist container exists in this file
      gHIn = new DrawObjContainer(f);
      gHIn->d["asym"] = new CnipolAsymHists();
      gHIn->ReadFromDir();

      if (!gHIn) {
         gSystem->Error("masym", "Hists asym not found\n");
         delete f;
         continue;
      }

      char strTime[80];
      strftime(strTime, 80, "%X", localtime(&gRC->fMeasInfo->fStartTime));

      Double_t runId            = gRC->fMeasInfo->RUNID;
      UInt_t   fillId           = (UInt_t) runId;
      UInt_t   beamEnergy       = (UInt_t) (gRC->fMeasInfo->GetBeamEnergy() + 0.5);
      string   asymVersion      = gRC->fMeasInfo->fAsymVersion;
      Float_t  ana_power        = gRC->fAnaMeasResult->A_N[1];
      Float_t  asymmetry        = gRC->fAnaMeasResult->sinphi[0].P[0] * gRC->fAnaMeasResult->A_N[1];
      Float_t  asymmetry_err    = gRC->fAnaMeasResult->sinphi[0].P[1] * gRC->fAnaMeasResult->A_N[1];
      Float_t  polarization     = gRC->fAnaMeasResult->sinphi[0].P[0] * 100.;
      Float_t  polarization_err = gRC->fAnaMeasResult->sinphi[0].P[1] * 100.;
      Double_t profileRatio     = gRC->fAnaMeasResult->fProfilePolarR.first;
      Double_t profileRatioErr  = gRC->fAnaMeasResult->fProfilePolarR.second;
      //Float_t  profileRatio     = gRC->fAnaMeasResult->fIntensPolarR;
      //Float_t  profileRatioErr  = gRC->fAnaMeasResult->fIntensPolarRErr;
      //Float_t  tzero            = gRC->fCalibrator->fChannelCalibs[7].fT0Coef;
      //Float_t  tzeroErr         = gRC->fCalibrator->fChannelCalibs[7].fT0CoefErr;

      // Substitute the beam energy for special ramp fills.
      // XXX Comment this for normal summary reports
      //if ( beamEnergy == 100 && gRC->fMeasInfo->fStartTime > flattopTimes[fillId]) {
      //   gRC->fMeasInfo->fBeamEnergy = 400;
      //   beamEnergy = 400;
      //}

      //printf("tzero: %f %f %f %d %f \n", tzero, tzeroErr, runId, gRC->fMeasInfo->fStartTime, asymmetry);
      //printf("%8.3f, %s, %3d, %f, %f, %f, %f, %f, %s\n", runId, strTime,
      //   beamEnergy, asymmetry, asymmetry_err, ana_power, polarization,
      //   polarization_err, asymVersion.c_str());

      if (asymVersion != "v1.7.0") {
	      Warning("masym", "Wrong version %s", asymVersion.c_str());
         continue;
      }

      if (polarization < 5 || polarization > 99 || polarization_err > 30 ||
          gRunConfig.fBeamEnergies.find((EBeamEnergy) beamEnergy) == gRunConfig.fBeamEnergies.end() ||
          gRC->fMeasInfo->fMeasType != kMEASTYPE_SWEEP ||
          profileRatio > 1.0 || TMath::Abs(profileRatio) < 0.0001 || profileRatioErr < 0.001
         )
      {
	      Warning("masym", "Didn't pass basic QA check");
         continue;
      }

      if (flattopTimes.find(fillId) == flattopTimes.end()) 
         flattopTimes[fillId] = 0;

      if ( beamEnergy == 250 && gRC->fMeasInfo->fStartTime > flattopTimes[fillId]) {
         flattopTimes[fillId] = gRC->fMeasInfo->fStartTime;
      }

      //if (gRC->fMeasInfo->fStartTime < minTime ) minTime = gRC->fMeasInfo->fStartTime;
      //if (gRC->fMeasInfo->fStartTime > maxTime ) maxTime = gRC->fMeasInfo->fStartTime;

      if (gH->d.find("runs") != gH->d.end()) {
         ((MAsymRunHists*) gH->d["runs"])->SetMinMaxFill(fillId);
         ((MAsymRunHists*) gH->d["runs"])->SetMinMaxTime(gRC->fMeasInfo->fStartTime);
	   }

      // To calculate normalization factors for p-Carbon we need to do it in the first pass
      if ( beamEnergy == 250 ) {
         gAnaGlobResult.AddRunResult(*gRC);
      }

      delete f;
     
      gGoodRuns.insert(fName);
   }

   map<UInt_t, UInt_t>::iterator ift;

   for (ift=flattopTimes.begin(); ift!=flattopTimes.end(); ++ift) {
      printf("%d -> %d\n", ift->first, ift->second);
   }

   // Adjust min/max fill for histogram limits
   if (gH->d.find("runs") != gH->d.end()) {
      ((MAsymRunHists*) gH->d["runs"])->AdjustMinMaxFill();
      gAnaGlobResult.AdjustMinMaxFill();
   }


   // Process run/fill results, i.e. calculate fill average, ...
   gAnaGlobResult.Process();
   //gAnaGlobResult.Print();
   //return;

   // Now process only good runs
   set<string>::iterator iRunName = gGoodRuns.begin();

   for ( ; iRunName!=gGoodRuns.end(); ++iRunName) {

      TString fileName = gAnaInfo->GetResultsDir() + "/" + (*iRunName) + "/" + (*iRunName) + fileSuffix + ".root";

      TFile *f = new TFile(fileName, "READ");

      //gSystem->Info("", "Processing file: %s", fileName.Data());
      printf("%s\n", (*iRunName).c_str());

      gRC = (EventConfig*) f->FindObjectAny("EventConfig");

      // Get asym hist container. It must exist
      gHIn = new DrawObjContainer(f);
      gHIn->d["asym"] = new CnipolAsymHists();
      gHIn->ReadFromDir();

      // Overwrite the default gMeasInfo with the saved one
      gMeasInfo = gRC->fMeasInfo;

      gH->Fill(*gRC);
      gH->Fill(*gRC, *gHIn);

      delete f;
   }

   gH->PostFill(gAnaGlobResult);
   gH->PostFill();
   gH->UpdateLimits();
   //gH->SetSignature(gRC->GetSignature());
   gH->SetSignature("");

   TCanvas canvas("cName2", "cName2", 1400, 600);
   gH->SaveAllAs(canvas, "^.*$", filelistName.Data());
   //gH->SaveAllAs(canvas, "^.*hPolar.*VsFill.*$", filelistName.Data());
   //gH->SaveAllAs(canvas, "^.*SystVsFill.*$", filelistName.Data());
   //gH->SaveAllAs(canvas, "^.*ChAsym.*$", filelistName.Data());
   //gH->SaveAllAs(canvas, "^.*First.*$", filelistName.Data());

   gH->Write();
   //gH->Delete();

   gMAsymRoot->Close();

   gAnaGlobResult.Print();

   //if (kTRUE) {
   if (kFALSE) {
      gAsymDb2 = new AsymDbSql();
      gAnaGlobResult.UpdateInsertDb();
   }
}
