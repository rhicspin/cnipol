
#include <time.h>
#include <map>

#include "manalyze.h"

#include "MAsymFillHists.h"
#include "MAsymRunHists.h"
#include "MAsymRateHists.h"
#include "MAsymPmtHists.h"

#include "AsymGlobals.h"
#include "AnaInfo.h"

#include "utils/utils.h"

using namespace std;

DrawObjContainer    *gH;
AnaInfo             *gAnaInfo;


void manalyze()
{
   initialize();
}


void initialize()
{
   gStyle->SetOptTitle(0);
   gStyle->SetOptStat(0);
   gStyle->SetPadRightMargin(0.18);

   TString filelistPath("/eic/u/dsmirnov/run/");

   //TString filelistName = "runs11_rampupdown.dat";
   //TString filelistName = "runs_all.dat";
   //TString filelistName = "run11_153XX_tmp.dat";
   //TString filelistName = "runs_tmp2.dat";
   //TString filelistName = "run11_15393.dat";
   //TString filelistName = "run11_15397.dat";
   //TString filelistName = "run11_15399.dat";
   //TString filelistName = "run09_all_tmp.dat";
   //TString filelistName = "run09_all_.dat";
   //TString filelistName = "run11_15XXX_2XX_3XX_4XX.dat";
   //TString filelistName = "run11_15XXX_1XX_2XX_3XX_4XX.dat";
   //TString filelistName = "run11_153XX.dat";
   //TString filelistName = "run11_153XX_Y2U.dat";
   //TString filelistName = "run11_pol_decay.dat";
   //TString filelistName = "run11_1547X_4_5.dat";
   //TString filelistName = "run11_154XX_00_23_before_rotators.dat";
   //TString filelistName = "run11_tmp.dat";
   TString filelistName = "run11_15473_74_75_injection";

	TString filelist = filelistPath + filelistName + ".dat";

   TString outFileName = "masym_" + filelistName + "_out.root";

   //std::find(gRunConfig.fBeamEnergies.begin(), gRunConfig.fBeamEnergies.end(), kBEAM_ENERGY_100);
   gRunConfig.fBeamEnergies.erase(kBEAM_ENERGY_100);

   gAnaInfo   = new AnaInfo();
   gMAsymRoot = new MAsymRoot(outFileName.Data());

   gH = new DrawObjContainer(gMAsymRoot);

   gH->d["fills"] = new MAsymFillHists(new TDirectoryFile("fills", "fills", "", gMAsymRoot));
   gH->d["rate"]  = new MAsymRateHists(new TDirectoryFile("rate",  "rate",  "", gMAsymRoot));
   gH->d["runs"]  = new MAsymRunHists (new TDirectoryFile("runs",  "runs",  "", gMAsymRoot));
   gH->d["pmt"]   = new MAsymPmtHists (new TDirectoryFile("pmt",  "pmt",  "", gMAsymRoot));

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
      TString fileName = gAnaInfo->GetResultsDir() + "/" + fName + "/" + fName + ".root";

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

      //if (gRC->fRunInfo->StartTime < minTime ) minTime = gRC->fRunInfo->StartTime;
      //if (gRC->fRunInfo->StartTime > maxTime ) maxTime = gRC->fRunInfo->StartTime;

      if (gH->d.find("runs") != gH->d.end()) {
         ((MAsymRunHists*) gH->d["runs"])->SetMinMaxFill(fillId);
         ((MAsymRunHists*) gH->d["runs"])->SetMinMaxTime(gRC->fRunInfo->StartTime);
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
      string  fName    = string(((TObjString*) o)->GetName());
      TString fileName = gAnaInfo->GetResultsDir() + "/" + fName + "/" + fName + ".root";

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

      char strTime[80];
      strftime(strTime, 80, "%X", localtime(&gRC->fRunInfo->StartTime));

      Double_t runId            = gRC->fRunInfo->RUNID;
      UInt_t   fillId           = (UInt_t) runId;
      UInt_t   beamEnergy       = (UInt_t) (gRC->fRunInfo->fBeamEnergy + 0.5);
      string   asymVersion      = gRC->fRunInfo->fAsymVersion;
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
      printf("%8.3f, %s, %3d, %f, %f, %f, %f, %f, %s\n", runId, strTime,
         beamEnergy, asymmetry, asymmetry_err, ana_power, polarization,
         polarization_err, asymVersion.c_str());

      //if (asymVersion != "v1.5.1")
      //   continue;

      if (polarization <= 1 || polarization > 99 || polarization_err > 30) {
	      Warning("Fill", "Didn't pass basic QA check");
         continue;
      }

      gRunInfo = gRC->fRunInfo;
      gH->Fill(*gRC);
   }

   Double_t xmin, ymin, xmax, ymax;

   string imageName;

   gH->PostFill();
   gH->UpdateLimits();
   gH->SetSignature(gRC->GetSignature());

   TCanvas canvas("cName2", "cName2", 1400, 600);
   //gH->SaveAllAs(canvas, "^.*$", filelistName.Data());
   gH->SaveAllAs(canvas, "^.*hPolarVs.*$", filelistName.Data());

   gH->Write();
   //gH->Delete();

   gMAsymRoot->Close();
}
