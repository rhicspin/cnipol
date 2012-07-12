
#include <fstream>
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
#include "MAsymAnaInfo.h"
#include "MeasInfo.h"

#include "utils/utils.h"

using namespace std;


/** */
int main(int argc, char *argv[])
{
   setbuf(stdout, NULL);

   // Create a default one
   gMeasInfo = new MeasInfo();

   // Do not attempt to recover files
   gEnv->SetValue("TFile.Recover", 0);
   
   MAsymAnaInfo mAsymAnaInfo;
   mAsymAnaInfo.ProcessOptions(argc, argv);
   mAsymAnaInfo.VerifyOptions();

	gMAsymAnaInfo = &mAsymAnaInfo;

   AnaGlobResult gAnaGlobResult;
   gAnaGlobResult.Configure(mAsymAnaInfo);

   gStyle->SetOptTitle(0);
   //gStyle->SetOptStat("emroui");
   //gStyle->SetOptStat("e");
   gStyle->SetOptStat(1111);
   gStyle->SetOptFit(1111);
   gStyle->SetPadRightMargin(0.25);

   //std::find(gRunConfig.fBeamEnergies.begin(), gRunConfig.fBeamEnergies.end(), kBEAM_ENERGY_100);
   //gRunConfig.fBeamEnergies.erase(kINJECTION);
   //gRunConfig.fBeamEnergies.erase(kBEAM_ENERGY_100);
   //gRunConfig.fBeamEnergies.erase(kBEAM_ENERGY_250);
   //gRunConfig.fBeamEnergies.erase(kBEAM_ENERGY_255);

   //string filelistName = mAsymAnaInfo.GetMListFileName();
	string filelist     = mAsymAnaInfo.GetMListFullPath();

   ofstream filelistGood((filelist + "_good").c_str());
   filelistGood << fixed << setprecision(3);

   MAsymRoot mAsymRoot(mAsymAnaInfo);
	mAsymRoot.SetAnaGlobResult(&gAnaGlobResult);

   //UInt_t minTime = UINT_MAX;
   //UInt_t maxTime = 0;

   Info("masym", "Starting first pass...");

   // Create a default canvas here to get rid of weird root messages while
   // reading objects from root files
   //TCanvas canvas("canvas", "canvas", 1400, 600);

   // Container with measurements passed QA cuts. Used to save time on opening
   // input files in the second pass
   set<EventConfig> gGoodMeass;

   //std::map<UInt_t, UInt_t>  flattopTimes;

   // Fill chain with all input files from filelist
   TObject *o;
   TIter   *next = new TIter(utils::getFileList(filelist));

   // Loop over the runs and record the time of the last flattop measurement in the fill
   while (next && (o = (*next)()) )
   {
      string fName    = string(((TObjString*) o)->GetName());
      string fileName = mAsymAnaInfo.GetResultsDir() + "/" + fName + "/" + fName + mAsymAnaInfo.GetSuffix() + ".root";

      TFile *f = new TFile(fileName.c_str(), "READ");

      if (!f) {
         Error("masym", "file not found. Skipping...");
         delete f;
         continue;
      }

      if (f->IsZombie()) {
         Error("masym", "file is zombie %s. Skipping...", fileName.c_str());
         f->Close();
         delete f;
         continue;
      }

      Info("masym", "Found file: %s", fileName.c_str());

      EventConfig *gMM = (EventConfig*) f->FindObjectAny("EventConfig");

      if (!gMM) {
         Error("masym", "MM not found. Skipping...");
         f->Close();
         delete f;
         continue;
      }

      char strTime[80];
      strftime(strTime, 80, "%X", localtime(&gMM->fMeasInfo->fStartTime));

      Double_t    runId      = gMM->fMeasInfo->RUNID;
      UInt_t      fillId     = (UInt_t) runId;
      EBeamEnergy beamEnergy = gMM->fMeasInfo->GetBeamEnergy();

      //Float_t  polarization    = gMM->fAnaMeasResult->sinphi[0].P[0] * 100.;
      //Float_t  polarizationErr = gMM->fAnaMeasResult->sinphi[0].P[1] * 100.;
      Float_t  polarization    =  0;
      Float_t  polarizationErr = -1;

      TFitResultPtr fitResPolarPhi = gMM->fAnaMeasResult->fFitResPolarPhi;

      if (fitResPolarPhi.Get()) {
         //polarization    = fitResPolarPhi->Value(0);
         //polarizationErr = fitResPolarPhi->FitResult::Error(0);
         polarization    = fitResPolarPhi->Value(0) * 100;
         polarizationErr = fitResPolarPhi->FitResult::Error(0) * 100;
      }

      Float_t profileRatio     = gMM->fAnaMeasResult->fProfilePolarR.first;
      Float_t profileRatioErr  = gMM->fAnaMeasResult->fProfilePolarR.second;
      //Float_t  profileRatio     = gMM->fAnaMeasResult->fIntensPolarR;
      //Float_t  profileRatioErr  = gMM->fAnaMeasResult->fIntensPolarRErr;

      // Substitute the beam energy for special ramp fills.
      // XXX Comment this for normal summary reports
      //if ( beamEnergy == 100 && gMM->fMeasInfo->fStartTime > flattopTimes[fillId]) {
      //   gMM->fMeasInfo->fBeamEnergy = 400;
      //   beamEnergy = 400;
      //}


      // the cut on polarization value should be removed
      //if (polarization > 99 || polarizationErr > 30 ||
      if ( polarization < 10 || polarization > 99 || polarizationErr > 30 ||
          gRunConfig.fBeamEnergies.find(beamEnergy) == gRunConfig.fBeamEnergies.end() ||
          gMM->fMeasInfo->fMeasType != kMEASTYPE_SWEEP ||
          (TMath::Abs(profileRatio) > 5.000) ||                           // exclude very large values
          (TMath::Abs(profileRatio) > 1.000 && profileRatioErr < 0.05) || // exclude large values with small errors
          (TMath::Abs(profileRatio) > 1.000 && profileRatioErr > 0.50) || // exclude large values with large errors
          //(profileRatioErr/TMath::Abs(profileRatio) > 2.000 && TMath::Abs(profileRatio) > 1.0) ||
          (profileRatioErr < 0.01) // exclude too small errors. probably from failed fits?
          //(TMath::Abs(profileRatio) < 0.001 && profileRatioErr < 0.01)
         )
      {
	      Warning("masym", "Measurement %9.3f did not pass basic QA check", runId);
         printf("%8.3f, %s, %3d, %f, %f, %s, %f, %f\n", runId, strTime,
            beamEnergy, polarization, polarizationErr,
            RunConfig::AsString(gMM->fMeasInfo->fMeasType).c_str(), profileRatio,
            profileRatioErr );

         f->Close();
         delete f;
         continue;
      }

      //if (flattopTimes.find(fillId) == flattopTimes.end()) 
      //   flattopTimes[fillId] = 0;
      //if ( beamEnergy == kBEAM_ENERGY_100 && gMM->fMeasInfo->fStartTime > flattopTimes[fillId]) {
      //   flattopTimes[fillId] = gMM->fMeasInfo->fStartTime;
      //}

      //if (gMM->fMeasInfo->fStartTime < minTime ) minTime = gMM->fMeasInfo->fStartTime;
      //if (gMM->fMeasInfo->fStartTime > maxTime ) maxTime = gMM->fMeasInfo->fStartTime;

      //if (gH->d.find("runs") != gH->d.end()) {
      //   ((MAsymRunHists*) gH->d["runs"])->UpdMinMaxFill(fillId);
      //   ((MAsymRunHists*) gH->d["runs"])->UpdMinMaxTime(gMM->fMeasInfo->fStartTime);
	   //}

		mAsymRoot.UpdMinMax(*gMM);

      // Check that asym hist container exists in this file
      DrawObjContainer *gHIn = new DrawObjContainer(f);
      gHIn->d["asym"] = new CnipolAsymHists();
      gHIn->ReadFromDir();

      //DrawObjContainer *gHIn = 0;

      // To calculate normalization factors for p-Carbon we need to save all
      // p-Carbon measurements in the first pass
      gAnaGlobResult.AddMeasResult(*gMM, gHIn);
      //gHIn->Print();

		//gHIn->Delete();
      delete gHIn;

      f->Close();
      delete f;
     
      gGoodMeass.insert(*gMM);
      filelistGood << runId << endl;
   }

   filelistGood.close();

	// Update global run parameters before anything else
	gRunConfig.SetBeamEnergies(gAnaGlobResult.GetBeamEnergies());

   // Create graphic containers
   DrawObjContainer *gH = new DrawObjContainer(&mAsymRoot);

   gH->d["fills"] = new MAsymFillHists(new TDirectoryFile("fills", "fills", "", &mAsymRoot));
   gH->d["rate"]  = new MAsymRateHists(new TDirectoryFile("rate",  "rate",  "", &mAsymRoot));
   gH->d["runs"]  = new MAsymRunHists (new TDirectoryFile("runs",  "runs",  "", &mAsymRoot));
   gH->d["pmt"]   = new MAsymPmtHists (new TDirectoryFile("pmt",   "pmt",   "", &mAsymRoot));

   // Adjust min/max fill for histogram limits
   ((MAsymRunHists*) gH->d["runs"])->SetMinMaxFill(gAnaGlobResult.GetMinFill(), gAnaGlobResult.GetMaxFill());
   ((MAsymRunHists*) gH->d["runs"])->SetMinMaxTime(gAnaGlobResult.GetMinTime(), gAnaGlobResult.GetMaxTime());
   ((MAsymRunHists*) gH->d["runs"])->AdjustMinMaxFill();
   gAnaGlobResult.AdjustMinMaxFill();


   // Process run/fill results, i.e. calculate fill average, ...
   Info("masym", "Analyzing measurements...");

   gAnaGlobResult.AddHJMeasResult();
   gAnaGlobResult.Process(gH);
   //gAnaGlobResult.Print("all");
   //gAnaGlobResult.Print();


   Info("masym", "Starting second pass...");

   // Now process only good runs
   set<EventConfig>::const_iterator iMeas = gGoodMeass.begin();

   for ( ; iMeas!=gGoodMeass.end(); ++iMeas)
   {
      // Overwrite the default gMeasInfo with the saved one
      gMeasInfo = iMeas->fMeasInfo;

      Info("masym", "Processing measurement: %.3f", iMeas->fMeasInfo->RUNID);

      gH->Fill(*iMeas);
      //gH->Fill(*iMeas, *gHIn);
   }

   gH->PostFill(gAnaGlobResult);
   gH->PostFill();
   gH->UpdateLimits();

   mAsymRoot.SetHists(*gH);

   if (mAsymAnaInfo.HasGraphBit())
      mAsymRoot.SaveAs("^.*$", mAsymAnaInfo.GetImageDir());

	if (mAsymAnaInfo.fFlagUpdateDb)
   {
      AsymDbSql *asymDbSql = new AsymDbSql();
      gAnaGlobResult.UpdateInsertDb(asymDbSql);
   }

   gH->Write();

   mAsymRoot.Print();
   mAsymRoot.Close();

   //gAnaGlobResult.Print("all");
   gAnaGlobResult.Print();

   return 1;
}
