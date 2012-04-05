
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
#include "MAsymAnaInfo.h"
#include "MeasInfo.h"

#include "utils/utils.h"

using namespace std;

DrawObjContainer    *gHIn;
DrawObjContainer    *gH;
AnaInfo             *gAnaInfo;
MAsymAnaInfo         gMAsymAnaInfo;
set<string>          gGoodRuns;
set<EventConfig>     gGoodMeass;
AnaGlobResult        gAnaGlobResult;


int main(int argc, char *argv[])
{
   // Create a default one
   gMeasInfo = new MeasInfo();

   // do not attept to recover files
   gEnv->SetValue("TFile.Recover", 0);
   
   gMAsymAnaInfo.ProcessOptions(argc, argv);
   gMAsymAnaInfo.VerifyOptions();

   initialize();

   return 1;
}


void initialize()
{
   gStyle->SetOptTitle(0);
   //gStyle->SetOptStat("emroui");
   //gStyle->SetOptStat("e");
   gStyle->SetOptStat(1111);
   gStyle->SetOptFit(1111);
   gStyle->SetPadRightMargin(0.25);

   //TString filelistPath("/eic/u/dsmirnov/run/");

   TString filelistName = gMAsymAnaInfo.GetMListFileName();

	//TString filelist    = filelistPath + filelistName + ".txt";
	TString filelist    = gMAsymAnaInfo.GetMListFullPath();
   TString outFileName = "masym_" + filelistName + ".root";
   TString fileSuffix  = "";
   //TString fileSuffix  = "_hama";

   //std::find(gRunConfig.fBeamEnergies.begin(), gRunConfig.fBeamEnergies.end(), kBEAM_ENERGY_100);
   //gRunConfig.fBeamEnergies.erase(kBEAM_ENERGY_100);
   //gRunConfig.fBeamEnergies.erase(kINJECTION);
   gRunConfig.fBeamEnergies.erase(kBEAM_ENERGY_250);

   gAnaInfo   = new AnaInfo();
   gMAsymRoot = new MAsymRoot(outFileName.Data());

   gH = new DrawObjContainer(gMAsymRoot);

   gH->d["fills"] = new MAsymFillHists(new TDirectoryFile("fills", "fills", "", gMAsymRoot));
   gH->d["rate"]  = new MAsymRateHists(new TDirectoryFile("rate",  "rate",  "", gMAsymRoot));
   gH->d["runs"]  = new MAsymRunHists (new TDirectoryFile("runs",  "runs",  "", gMAsymRoot));
   gH->d["pmt"]   = new MAsymPmtHists (new TDirectoryFile("pmt",   "pmt",   "", gMAsymRoot));

   //UInt_t minTime = UINT_MAX;
   //UInt_t maxTime = 0;

   Info("masym", "Starting first pass...");

   // Create a default canvas here to get rid of weird root messages while
   // reading objects from root files
   TCanvas canvas("cName2", "cName2", 1400, 600);

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
         Error("masym", "file not found. Skipping...");
         continue;
      }

      if (f->IsZombie()) {
         Error("masym", "file is zombie %s. Skipping...", fileName.Data());
         delete f;
         continue;
      }

      Info("masym", "Found file: %s", fileName.Data());

      gMM = (EventConfig*) f->FindObjectAny("EventConfig");

      if (!gMM) {
         Error("masym", "RC not found. Skipping...");
         delete f;
         continue;
      }

      // Check that asym hist container exists in this file
      //gHIn = new DrawObjContainer(f);
      //gHIn->d["asym"] = new CnipolAsymHists();
      //gHIn->ReadFromDir();

      //if (!gHIn) {
      //   Error("masym", "Hists asym not found\n");
      //   delete f;
      //   continue;
      //}

      char strTime[80];
      strftime(strTime, 80, "%X", localtime(&gMM->fMeasInfo->fStartTime));

      Double_t runId            = gMM->fMeasInfo->RUNID;
      UInt_t   fillId           = (UInt_t) runId;
      UInt_t   beamEnergy       = (UInt_t) (gMM->fMeasInfo->GetBeamEnergy() + 0.5);
      //string   asymVersion      = gMM->fMeasInfo->fAsymVersion;
      //Float_t  ana_power        = gMM->fAnaMeasResult->A_N[1];
      //Float_t  asymmetry        = gMM->fAnaMeasResult->sinphi[0].P[0] * gMM->fAnaMeasResult->A_N[1];
      //Float_t  asymmetry_err    = gMM->fAnaMeasResult->sinphi[0].P[1] * gMM->fAnaMeasResult->A_N[1];

      //Float_t  polarization    = gMM->fAnaMeasResult->sinphi[0].P[0] * 100.;
      //Float_t  polarizationErr = gMM->fAnaMeasResult->sinphi[0].P[1] * 100.;
      Float_t  polarization    =  0;
      Float_t  polarizationErr = -1;

      TFitResultPtr fitResPolarPhi = gMM->fAnaMeasResult->fFitResPolarPhi;

      if (fitResPolarPhi.Get()) {
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

      //printf("tzero: %f %f %f %d %f \n", tzero, tzeroErr, runId, gMM->fMeasInfo->fStartTime, asymmetry);
      //printf("%8.3f, %s, %3d, %f, %f, %f, %f, %f, %s\n", runId, strTime,
      //   beamEnergy, asymmetry, asymmetry_err, ana_power, polarization,
      //   polarizationErr, asymVersion.c_str());
      //if (asymVersion != "v1.8.5") {
	   //   Warning("masym", "Wrong version %s", asymVersion.c_str());
      //   continue;
      //}

      if (polarization < 5 || polarization > 99 || polarizationErr > 30 ||
          gRunConfig.fBeamEnergies.find((EBeamEnergy) beamEnergy) == gRunConfig.fBeamEnergies.end() ||
          gMM->fMeasInfo->fMeasType != kMEASTYPE_SWEEP ||
          (TMath::Abs(profileRatio) > 0.600 && profileRatioErr < 0.05) ||
          (TMath::Abs(profileRatio) < 0.001 && profileRatioErr < 0.01)
         )
      {
	      Warning("masym", "Measurement %9.3f did not pass basic QA check", runId);
         continue;
      }

      if (flattopTimes.find(fillId) == flattopTimes.end()) 
         flattopTimes[fillId] = 0;

      if ( beamEnergy == kBEAM_ENERGY_100 && gMM->fMeasInfo->fStartTime > flattopTimes[fillId]) {
         flattopTimes[fillId] = gMM->fMeasInfo->fStartTime;
      }

      //if (gMM->fMeasInfo->fStartTime < minTime ) minTime = gMM->fMeasInfo->fStartTime;
      //if (gMM->fMeasInfo->fStartTime > maxTime ) maxTime = gMM->fMeasInfo->fStartTime;

      if (gH->d.find("runs") != gH->d.end()) {
         ((MAsymRunHists*) gH->d["runs"])->SetMinMaxFill(fillId);
         ((MAsymRunHists*) gH->d["runs"])->SetMinMaxTime(gMM->fMeasInfo->fStartTime);
	   }

      // To calculate normalization factors for p-Carbon we need to save all
      // p-Carbon measurements in the first pass
      //if ( beamEnergy == kBEAM_ENERGY_255 )
      if ( beamEnergy == kBEAM_ENERGY_100 )
      {
         gAnaGlobResult.AddMeasResult(*gMM);
      }

      delete f;
     
      //gGoodRuns.insert(fName);
      gGoodMeass.insert(*gMM);
   }

   // Print out flatop start and end times
   Info("masym", "Flattop times:");

   map<UInt_t, UInt_t>::iterator ift;

   for (ift=flattopTimes.begin(); ift!=flattopTimes.end(); ++ift) {
      printf("%d -> %d\n", ift->first, ift->second);
   }

   // Adjust min/max fill for histogram limits
   if (gH->d.find("runs") != gH->d.end()) {
      ((MAsymRunHists*) gH->d["runs"])->AdjustMinMaxFill();
      //((MAsymRunHists*) gH->d["runs"])->AdjustMinMaxFill();
      gAnaGlobResult.AdjustMinMaxFill();
   }


   // Process run/fill results, i.e. calculate fill average, ...
   Info("masym", "Analyzing measurements...");

   gAnaGlobResult.Process();
   //gAnaGlobResult.Print();

   Info("masym", "Starting second pass...");

   // Now process only good runs
   set<EventConfig>::const_iterator iMeas = gGoodMeass.begin();

   for ( ; iMeas!=gGoodMeass.end(); ++iMeas)
   {
      // Overwrite the default gMeasInfo with the saved one
      gMeasInfo = iMeas->fMeasInfo;

      Info("masym", "Processing measurement: %.3f", iMeas->fMeasInfo->RUNID);

      // Get asym hist container. It must exist
      //gHIn = new DrawObjContainer(f);
      //gHIn->d["asym"] = new CnipolAsymHists();
      //gHIn->ReadFromDir();

      gH->Fill(*iMeas);
      //gH->Fill(*iMeas, *gHIn);

      //delete f;
   }

   gH->PostFill(gAnaGlobResult);
   gH->PostFill();
   gH->UpdateLimits();
   gH->SetSignature((--iMeas)->GetSignature()); // get signature of the last measurement
   //gH->SetSignature("");

   //if (gAnaInfo->HasGraphBit())
   //   gAsymRoot->SaveAs("^.*$", gAnaInfo->GetImageDir());

   gH->SaveAllAs(canvas, "^.*$", filelistName.Data());
   //gH->SaveAllAs(canvas, "^.*SpinAngle.*$", filelistName.Data());
   //gH->SaveAllAs(canvas, "^.*hPolarVs.*$", filelistName.Data());
   //gH->SaveAllAs(canvas, "^.*VsFillTime.*$", filelistName.Data());
   //gH->SaveAllAs(canvas, "^.*RVsFill.*$", filelistName.Data());
   //gH->SaveAllAs(canvas, "^.*VsMeas.*$", filelistName.Data());
   //gH->SaveAllAs(canvas, "^.*SystVsFill.*$", filelistName.Data());
   //gH->SaveAllAs(canvas, "^.*ChAsym.*$", filelistName.Data());
   //gH->SaveAllAs(canvas, "^.*First.*$", filelistName.Data());

   gH->Write();
   //gH->Delete();

   gMAsymRoot->Close();

   //gAnaGlobResult.Print("all");
   gAnaGlobResult.Print();

   //if (kTRUE)
   if (kFALSE)
   {
      gAsymDb = new AsymDbSql();
      gAnaGlobResult.UpdateInsertDb();
   }
}
