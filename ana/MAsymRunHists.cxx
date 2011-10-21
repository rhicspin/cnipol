/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2F.h"
#include "TFitResult.h"

#include "utils/utils.h"

#include "CnipolAsymHists.h"
#include "MAsymRunHists.h"

//#include "RunInfo.h"
#include "MseRunInfo.h"
#include "ChannelCalib.h"


ClassImp(MAsymRunHists)

using namespace std;

/** Default constructor. */
MAsymRunHists::MAsymRunHists() : DrawObjContainer(),
   fMinFill(UINT_MAX), fMaxFill(0),
   fMinTime(UINT_MAX), fMaxTime(0),
	fHTargetVsRun(), fVTargetVsRun(),
   fHStacks()
{
   BookHists();
}


MAsymRunHists::MAsymRunHists(TDirectory *dir) : DrawObjContainer(dir),
   fMinFill(UINT_MAX), fMaxFill(0),
   fMinTime(UINT_MAX), fMaxTime(0),
	fHTargetVsRun(), fVTargetVsRun(),
   fHStacks()
{
   BookHists();
}


/** Default destructor. */
MAsymRunHists::~MAsymRunHists()
{
   //delete grHTargetVsMeas;
   //delete grVTargetVsMeas;
}


/** */
void MAsymRunHists::BookHists(string sid)
{ //{{{
   fDir->cd();

   char   hName[256];
   string shName;   
   string sName;   

   IterPolarimeterId iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId!=gRunConfig.fPolarimeters.end(); ++iPolId) {

      string strPolId = RunConfig::AsString(*iPolId);
      Color_t color   = RunConfig::AsColor(*iPolId);
      string dName    = strPolId;

      DrawObjContainer        *oc;
      DrawObjContainerMapIter  isubdir = d.find(dName);

      if ( isubdir == d.end()) { // if dir not found
         oc = new DrawObjContainer();
         oc->fDir = new TDirectoryFile(dName.c_str(), dName.c_str(), "", fDir);
      } else {
         oc = isubdir->second;
      }

      IterBeamEnergy iBE = gRunConfig.fBeamEnergies.begin();

      for ( ; iBE!=gRunConfig.fBeamEnergies.end(); ++iBE)
      {
         BookHistsPolarimeter(*oc, *iPolId, *iBE);
      }

      sprintf(hName, "hT0VsChannelDiff_%s", strPolId.c_str());
      oc->o[hName] = new TH1F(hName, hName, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      ((TH1*) oc->o[hName])->SetOption("gridx");
      ((TH1*) oc->o[hName])->SetTitle(";Channel;t_{0} Diff, ns;");

      sprintf(hName, "hDLVsChannelDiff_%s", strPolId.c_str());
      oc->o[hName] = new TH1F(hName, hName, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      ((TH1*) oc->o[hName])->SetOption("gridx");
      ((TH1*) oc->o[hName])->SetTitle(";Channel;Dead Layer Diff, #mug/cm^{2};");

      // t0 vs DL diff
      TGraphErrors* grT0VsDLDiff = new TGraphErrors();
      grT0VsDLDiff->SetName("grT0VsDLDiff");
      grT0VsDLDiff->SetMarkerStyle(kFullCircle);
      grT0VsDLDiff->SetMarkerSize(1);
      grT0VsDLDiff->SetMarkerColor(color);

      sprintf(hName, "hT0VsDLDiff_%s", strPolId.c_str());
      oc->o[hName] = new TH2F(hName, hName, 1000, 0, 1, 1000, 0, 1);
      ((TH1*) oc->o[hName])->SetTitle(";Dead Layer Diff, #mug/cm^{2};t_{0} Diff, ns;");
      ((TH1*) oc->o[hName])->GetListOfFunctions()->Add(grT0VsDLDiff, "p");

      shName = "hPolarFirstMeasRatioVsFill_" + strPolId;
      oc->o[shName] = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
      ((TH1*) oc->o[shName])->GetYaxis()->SetRangeUser(0, 100);
      ((TH1*) oc->o[shName])->SetTitle(";Fill;Polarization Ratio (Inj/First);");
      ((TH1*) oc->o[shName])->SetMarkerStyle(kFullCircle);
      ((TH1*) oc->o[shName])->SetMarkerSize(1);
      ((TH1*) oc->o[shName])->SetMarkerColor(color);
      ((TH1*) oc->o[shName])->SetOption("E1");

      // If this is a new directory then we need to add it to the list
      if ( isubdir == d.end()) {
         d[dName] = oc;
      }
   }

   IterBeamEnergy iBE=gRunConfig.fBeamEnergies.begin();

   for ( ; iBE!=gRunConfig.fBeamEnergies.end(); ++iBE)
   {
      string  strBeamE = RunConfig::AsString(*iBE);
      Color_t color    = RunConfig::AsColor(kB1U);

      sprintf(hName, "hBlueHJetVsFill_%s", strBeamE.c_str());
      o[hName] = new TH1F(hName, hName, 1, 0, 1);
      ((TH1*) o[hName])->SetTitle(";Fill;H-jet Polarization, %;");
      ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
      ((TH1*) o[hName])->SetMarkerSize(1);
      ((TH1*) o[hName])->SetMarkerColor(color);
      ((TH1*) o[hName])->SetOption("E1");

      sprintf(hName, "hBluePolarRatioVsFill_%s", strBeamE.c_str());
      o[hName] = new TH1F(hName, hName, 1, 0, 1);
      ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 100);
      ((TH1*) o[hName])->SetTitle(";Fill;Polarization Ratio;");
      ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
      ((TH1*) o[hName])->SetMarkerSize(1);
      ((TH1*) o[hName])->SetMarkerColor(color);
      ((TH1*) o[hName])->SetOption("E1");

      sprintf(hName, "hBluePolarVsFill_%s", strBeamE.c_str());
      o[hName] = new TH1F(hName, hName, 1, 0, 1);
      ((TH1*) o[hName])->SetTitle(";Fill;Beam Polarization, %;");
      ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
      ((TH1*) o[hName])->SetMarkerSize(1);
      ((TH1*) o[hName])->SetMarkerColor(color);
      ((TH1*) o[hName])->SetOption("E1");

      sprintf(hName, "hBluePolarDiffVsFill_%s", strBeamE.c_str());
      o[hName] = new TH1F(hName, hName, 1, 0, 1);
      ((TH1*) o[hName])->SetTitle(";Fill;Polarization Diff. (U vs D), %;");
      ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
      ((TH1*) o[hName])->SetMarkerSize(1);
      ((TH1*) o[hName])->SetMarkerColor(color);
      ((TH1*) o[hName])->SetOption("E1");

      sprintf(hName, "hBluePolarRelDiffVsFill_%s", strBeamE.c_str());
      o[hName] = new TH1F(hName, hName, 1, 0, 1);
      ((TH1*) o[hName])->SetTitle(";Fill;Polarization Rel. Diff., %;");
      ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
      ((TH1*) o[hName])->SetMarkerSize(1);
      ((TH1*) o[hName])->SetMarkerColor(color);
      ((TH1*) o[hName])->SetOption("E1");

      sprintf(hName, "hBluePolarRelDiff_%s", strBeamE.c_str());
      o[hName] = new TH1F(hName, hName, 200, 0, 100);
      //((TH1*) o[hName])->SetTitle(";#Delta_{U vs. D};;");
      ((TH1*) o[hName])->SetTitle(";Polarization Rel. Diff., %;;");
      ((TH1*) o[hName])->SetLineWidth(2);
      ((TH1*) o[hName])->SetLineColor(color);
      ((TH1*) o[hName])->SetOption("hist");

      sprintf(hName, "hBluePolarRelDiffCumul_%s", strBeamE.c_str());
      o[hName] = new TH1F(hName, hName, 200, 0, 100);
      //((TH1*) o[hName])->SetTitle(";Cumulative #Delta_{U vs. D};;");
      ((TH1*) o[hName])->SetTitle(";Cumulative Pol. Rel. Diff., %;;");
      ((TH1*) o[hName])->SetLineWidth(2);
      ((TH1*) o[hName])->SetLineColor(color);
      ((TH1*) o[hName])->SetOption("hist");

      color = RunConfig::AsColor(kY2U);

      sprintf(hName, "hYellowHJetVsFill_%s", strBeamE.c_str());
      o[hName] = new TH1F(hName, hName, 1, 0, 1);
      ((TH1*) o[hName])->SetTitle(";Fill;H-jet Polarization, %;");
      ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
      ((TH1*) o[hName])->SetMarkerSize(1);
      ((TH1*) o[hName])->SetMarkerColor(color);
      ((TH1*) o[hName])->SetOption("E1");

      sprintf(hName, "hYellowPolarRatioVsFill_%s", strBeamE.c_str());
      o[hName] = new TH1F(hName, hName, 1, 0, 1);
      ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 100);
      ((TH1*) o[hName])->SetTitle(";Fill;Polarization Ratio;");
      ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
      ((TH1*) o[hName])->SetMarkerSize(1);
      ((TH1*) o[hName])->SetMarkerColor(color);
      ((TH1*) o[hName])->SetOption("E1");

      sprintf(hName, "hYellowPolarVsFill_%s", strBeamE.c_str());
      o[hName] = new TH1F(hName, hName, 1, 0, 1);
      ((TH1*) o[hName])->SetTitle(";Fill;Beam Polarization, %;");
      ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
      ((TH1*) o[hName])->SetMarkerSize(1);
      ((TH1*) o[hName])->SetMarkerColor(color);
      ((TH1*) o[hName])->SetOption("E1");

      sprintf(hName, "hYellowPolarDiffVsFill_%s", strBeamE.c_str());
      o[hName] = new TH1F(hName, hName, 1, 0, 1);
      ((TH1*) o[hName])->SetTitle(";Fill;Polarization Diff. (U vs D), %;");
      ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
      ((TH1*) o[hName])->SetMarkerSize(1);
      ((TH1*) o[hName])->SetMarkerColor(color);
      ((TH1*) o[hName])->SetOption("E1");

      sprintf(hName, "hYellowPolarRelDiffVsFill_%s", strBeamE.c_str());
      o[hName] = new TH1F(hName, hName, 1, 0, 1);
      ((TH1*) o[hName])->SetTitle(";Fill;Polarization Rel. Diff., %;");
      ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
      ((TH1*) o[hName])->SetMarkerSize(1);
      ((TH1*) o[hName])->SetMarkerColor(color);
      ((TH1*) o[hName])->SetOption("E1");

      sprintf(hName, "hYellowPolarRelDiff_%s", strBeamE.c_str());
      o[hName] = new TH1F(hName, hName, 200, 0, 100);
      //((TH1*) o[hName])->SetTitle(";#Delta_{U vs. D};;");
      ((TH1*) o[hName])->SetTitle(";Polarization Rel. Diff., %;;");
      ((TH1*) o[hName])->SetLineWidth(2);
      ((TH1*) o[hName])->SetLineColor(color);
      ((TH1*) o[hName])->SetOption("hist");

      sprintf(hName, "hYellowPolarRelDiffCumul_%s", strBeamE.c_str());
      o[hName] = new TH1F(hName, hName, 200, 0, 100);
      //((TH1*) o[hName])->SetTitle(";Cumulative #Delta_{U vs. D};;");
      ((TH1*) o[hName])->SetTitle(";Cumulative Pol. Rel. Diff., %;;");
      ((TH1*) o[hName])->SetLineWidth(2);
      ((TH1*) o[hName])->SetLineColor(color);
      ((TH1*) o[hName])->SetOption("hist");

      // Mainly needed to calculate common limits
      sName = "hsPolarVsFill_" + strBeamE;
      fHStacks[sName] = new THStack(sName.c_str(), sName.c_str());

      sName = "hsDLVsFill_U_" + strBeamE;
      fHStacks[sName] = new THStack(sName.c_str(), sName.c_str());

      sName = "hsDLVsFill_D_" + strBeamE;
      fHStacks[sName] = new THStack(sName.c_str(), sName.c_str());

      sName = "hsT0VsFill_U_" + strBeamE;
      fHStacks[sName] = new THStack(sName.c_str(), sName.c_str());

      sName = "hsT0VsFill_D_" + strBeamE;
      fHStacks[sName] = new THStack(sName.c_str(), sName.c_str());
   }
} //}}}


/** */
void MAsymRunHists::BookHistsPolarimeter(DrawObjContainer &oc, EPolarimeterId polId, EBeamEnergy beamE)
{ //{{{
   char    hName[256];
   string  shName;
   string  strPolId = RunConfig::AsString(polId);
   string  strBeamE = RunConfig::AsString(beamE);
   Color_t color    = RunConfig::AsColor(polId);

   // Rate
   sprintf(hName, "grMaxRateVsMeas");
   TGraphErrors *grMaxRateVsMeas = new TGraphErrors();
   grMaxRateVsMeas->SetName(hName);
   grMaxRateVsMeas->SetMarkerStyle(kFullCircle);
   grMaxRateVsMeas->SetMarkerSize(1);
   grMaxRateVsMeas->SetMarkerColor(color);

   sprintf(hName, "hMaxRateVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   oc.o[hName] = new TH2F(hName, hName, 1, 0, 1, 1, 0, 50);
   ((TH1*) oc.o[hName])->SetTitle(";Measurement;Max Rate;");
   ((TH1*) oc.o[hName])->GetListOfFunctions()->Add(grMaxRateVsMeas, "p");

   // Targets
   sprintf(hName, "grHTargetVsMeas");
   TGraphErrors *grHTargetVsMeas = new TGraphErrors();
   grHTargetVsMeas->SetName(hName);
   //grHTargetVsMeas->SetMarkerStyle(kPlus);
   grHTargetVsMeas->SetMarkerStyle(kFullTriangleUp);
   grHTargetVsMeas->SetMarkerSize(2);
   grHTargetVsMeas->SetMarkerColor(color-3);

   sprintf(hName, "grVTargetVsMeas");
   TGraphErrors *grVTargetVsMeas = new TGraphErrors();
   grVTargetVsMeas->SetName(hName);
   //grVTargetVsMeas->SetMarkerStyle(kMultiply);
   grVTargetVsMeas->SetMarkerStyle(kFullTriangleDown);
   grVTargetVsMeas->SetMarkerSize(2);
   grVTargetVsMeas->SetMarkerColor(color+2);

   sprintf(hName, "hTargetVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   oc.o[hName] = new TH2F(hName, hName, 1, 0, 1, 1, 0, 7);
   ((TH1*) oc.o[hName])->SetTitle(";Measurement;Target Id;");
   ((TH1*) oc.o[hName])->GetListOfFunctions()->Add(grHTargetVsMeas, "p");
   ((TH1*) oc.o[hName])->GetListOfFunctions()->Add(grVTargetVsMeas, "p");

   // Polarization
   TGraphErrors *grPolarVsMeas = new TGraphErrors();
   grPolarVsMeas->SetName("grPolarVsMeas");
   grPolarVsMeas->SetMarkerStyle(kFullCircle);
   grPolarVsMeas->SetMarkerSize(1);
   grPolarVsMeas->SetMarkerColor(color);

   sprintf(hName, "hPolarVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   oc.o[hName] = new TH2F(hName, hName, 1, 0, 1, 1, 0, 100);
   ((TH1*) oc.o[hName])->SetTitle(";Measurement;Polarization, %;");
   ((TH1*) oc.o[hName])->GetListOfFunctions()->Add(grPolarVsMeas, "p");

   shName = "hPolarVsFill_" + strPolId + "_" + strBeamE;
   oc.o[shName] = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   ((TH1*) oc.o[shName])->GetYaxis()->SetRangeUser(0, 100);
   ((TH1*) oc.o[shName])->SetTitle(";Fill;Polarization, %;");
   ((TH1*) oc.o[shName])->SetMarkerStyle(kFullCircle);
   ((TH1*) oc.o[shName])->SetMarkerSize(1);
   ((TH1*) oc.o[shName])->SetMarkerColor(color);
   ((TH1*) oc.o[shName])->SetOption("E1");
   ((TH1*) oc.o[shName])->GetListOfFunctions()->Add(grPolarVsMeas, "p");

   shName = "hPolarFirstMeasVsFill_" + strPolId + "_" + strBeamE;
   oc.o[shName] = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   ((TH1*) oc.o[shName])->GetYaxis()->SetRangeUser(0, 100);
   ((TH1*) oc.o[shName])->SetTitle(";Fill;Polarization, %;");
   ((TH1*) oc.o[shName])->SetMarkerStyle(kFullCircle);
   ((TH1*) oc.o[shName])->SetMarkerSize(1);
   ((TH1*) oc.o[shName])->SetMarkerColor(color);
   ((TH1*) oc.o[shName])->SetOption("E1");
   ((TH1*) oc.o[shName])->GetListOfFunctions()->Add(grPolarVsMeas, "p");

   shName = "hPolar2VsFill_" + strPolId + "_" + strBeamE;
   oc.o[shName] = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   ((TH1*) oc.o[shName])->GetYaxis()->SetRangeUser(0, 100);
   ((TH1*) oc.o[shName])->SetTitle(";Fill;Polarization, %;");
   ((TH1*) oc.o[shName])->SetMarkerStyle(kFullCircle);
   ((TH1*) oc.o[shName])->SetMarkerSize(1);
   ((TH1*) oc.o[shName])->SetMarkerColor(color);
   ((TH1*) oc.o[shName])->SetOption("E1");

   // polarization from pol profile
   shName = "hProfPolarVsFill_" + strPolId + "_" + strBeamE;
   oc.o[shName] = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   ((TH1*) oc.o[shName])->GetYaxis()->SetRangeUser(0, 100);
   ((TH1*) oc.o[shName])->SetTitle(";Fill;Polarization, %;");
   ((TH1*) oc.o[shName])->SetMarkerStyle(kFullCircle);
   ((TH1*) oc.o[shName])->SetMarkerSize(1);
   ((TH1*) oc.o[shName])->SetMarkerColor(color);
   ((TH1*) oc.o[shName])->SetOption("E1");

   // polarization from pol profile
   shName = "hProfPolarDiffVsFill_" + strPolId + "_" + strBeamE;
   oc.o[shName] = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   //((TH1*) oc.o[shName])->GetYaxis()->SetRangeUser(0, 100);
   ((TH1*) oc.o[shName])->SetTitle(";Fill;Profile Polarization Diff., %;");
   ((TH1*) oc.o[shName])->SetMarkerStyle(kFullCircle);
   ((TH1*) oc.o[shName])->SetMarkerSize(1);
   ((TH1*) oc.o[shName])->SetMarkerColor(color);
   ((TH1*) oc.o[shName])->SetOption("E1");

   // polarization from pol profile
   shName = "hProfPolarRelDiffVsFill_" + strPolId + "_" + strBeamE;
   oc.o[shName] = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   ((TH1*) oc.o[shName])->SetTitle(";Fill;Profile Polarization Rel. Diff., %;");
   ((TH1*) oc.o[shName])->SetMarkerStyle(kFullCircle);
   ((TH1*) oc.o[shName])->SetMarkerSize(1);
   ((TH1*) oc.o[shName])->SetMarkerColor(color);
   ((TH1*) oc.o[shName])->SetOption("E1");

   // polarization from pol profile
   shName = "hProfPolarRelDiff_" + strPolId + "_" + strBeamE;
   oc.o[shName] = new TH1F(shName.c_str(), shName.c_str(), 200, 0, 100);
   ((TH1*) oc.o[shName])->SetTitle(";Profile Polarization Rel. Diff., %;;");
   ((TH1*) oc.o[shName])->SetLineWidth(2);
   ((TH1*) oc.o[shName])->SetLineColor(color);
   ((TH1*) oc.o[shName])->SetOption("hist");

   // polarization from pol profile
   shName = "hProfPolarRelDiffCumul_" + strPolId + "_" + strBeamE;
   oc.o[shName] = new TH1F(shName.c_str(), shName.c_str(), 200, 0, 100);
   ((TH1*) oc.o[shName])->SetTitle(";Cumul. Profile Polarization Rel. Diff., %;;");
   ((TH1*) oc.o[shName])->SetLineWidth(2);
   ((TH1*) oc.o[shName])->SetLineColor(color);
   ((TH1*) oc.o[shName])->SetOption("hist");

   // Ratio Hjet to pC
   shName = "hNormJCVsFill_" + strPolId + "_" + strBeamE;
   oc.o[shName] = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   ((TH1*) oc.o[shName])->SetTitle(";Fill;H-jet/p-Carbon;");
   ((TH1*) oc.o[shName])->SetMarkerStyle(kFullCircle);
   ((TH1*) oc.o[shName])->SetMarkerSize(1);
   ((TH1*) oc.o[shName])->SetMarkerColor(color);
   ((TH1*) oc.o[shName])->SetOption("E1");

   // Polarization vs time
   TGraphErrors *grPolarVsTime = new TGraphErrors();
   grPolarVsTime->SetName("grPolarVsTime");
   grPolarVsTime->SetMarkerStyle(kFullCircle);
   grPolarVsTime->SetMarkerSize(1);
   grPolarVsTime->SetMarkerColor(color);

   sprintf(hName, "hPolarVsTime_%s_%s", strPolId.c_str(), strBeamE.c_str());
   oc.o[hName] = new TH2F(hName, hName, 1, 0, 1, 1, 0, 100);
   ((TH1*) oc.o[hName])->SetTitle(";Date & Time;Polarization, %;");
   ((TH1*) oc.o[hName])->GetListOfFunctions()->Add(grPolarVsTime, "p");
   ((TH1*) oc.o[hName])->GetXaxis()->SetNdivisions(10, kFALSE);
   ((TH1*) oc.o[hName])->GetXaxis()->SetTimeOffset(0, "gmt");
   ((TH1*) oc.o[hName])->GetXaxis()->SetTimeDisplay(1);
   //((TH1*) oc.o[hName])->GetXaxis()->SetTimeFormat("%b %d, %H:%M:%S");
   //((TH1*) oc.o[hName])->GetXaxis()->SetTimeFormat("%b %d, %H:%M");
   ((TH1*) oc.o[hName])->GetXaxis()->SetTimeFormat("%m/%d");
   //((TH1*) oc.o[hName])->GetXaxis()->SetTimeFormat("%H:%M");

   // Profile r
   TGraphErrors *grRVsMeas = new TGraphErrors();
   grRVsMeas->SetName("grRVsMeas");
   grRVsMeas->SetMarkerStyle(kFullCircle);
   grRVsMeas->SetMarkerSize(1);
   grRVsMeas->SetMarkerColor(color);

   TGraphErrors *grRVsMeasH = new TGraphErrors();
   grRVsMeasH->SetName("grRVsMeasH");
   grRVsMeasH->SetMarkerStyle(kFullTriangleUp);
   grRVsMeasH->SetMarkerSize(2);
   grRVsMeasH->SetMarkerColor(color-3);

   TGraphErrors *grRVsMeasV = new TGraphErrors();
   grRVsMeasV->SetName("grRVsMeasV");
   grRVsMeasV->SetMarkerStyle(kFullTriangleDown);
   grRVsMeasV->SetMarkerSize(2);
   grRVsMeasV->SetMarkerColor(color+2);

   sprintf(hName, "hRVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   oc.o[hName] = new TH2F(hName, hName, 1, 0, 1, 1, -0.3, 1);
   ((TH1*) oc.o[hName])->SetTitle(";Measurement;r;");
   ((TH1*) oc.o[hName])->GetListOfFunctions()->Add(grRVsMeasH, "p");
   ((TH1*) oc.o[hName])->GetListOfFunctions()->Add(grRVsMeasV, "p");

   sprintf(hName, "hRVsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
   oc.o[hName] = new TH1F(hName, hName, 1, 0, 1);
   ((TH1*) oc.o[hName])->GetYaxis()->SetRangeUser(-0.3, 1);
   ((TH1*) oc.o[hName])->SetTitle(";Fill;r;");
   ((TH1*) oc.o[hName])->SetMarkerStyle(kFullCircle);
   ((TH1*) oc.o[hName])->SetMarkerSize(1);
   ((TH1*) oc.o[hName])->SetMarkerColor(color);
   ((TH1*) oc.o[hName])->SetOption("E1");
   ((TH1*) oc.o[hName])->GetListOfFunctions()->Add(grRVsMeas, "p");

   sprintf(hName, "hRVsFill_H_%s_%s", strPolId.c_str(), strBeamE.c_str());
   oc.o[hName] = new TH1F(hName, hName, 1, 0, 1);
   ((TH1*) oc.o[hName])->GetYaxis()->SetRangeUser(-0.3, 1);
   ((TH1*) oc.o[hName])->SetTitle(";Fill;r;");
   ((TH1*) oc.o[hName])->SetMarkerStyle(kFullTriangleUp);
   ((TH1*) oc.o[hName])->SetMarkerSize(2);
   ((TH1*) oc.o[hName])->SetMarkerColor(color-3);
   ((TH1*) oc.o[hName])->SetOption("E1");
   ((TH1*) oc.o[hName])->GetListOfFunctions()->Add(grRVsMeasH, "p");

   sprintf(hName, "hRVsFill_V_%s_%s", strPolId.c_str(), strBeamE.c_str());
   oc.o[hName] = new TH1F(hName, hName, 1, 0, 1);
   ((TH1*) oc.o[hName])->GetYaxis()->SetRangeUser(-0.3, 1);
   ((TH1*) oc.o[hName])->SetTitle(";Fill;r;");
   ((TH1*) oc.o[hName])->SetMarkerStyle(kFullTriangleDown);
   ((TH1*) oc.o[hName])->SetMarkerSize(2);
   ((TH1*) oc.o[hName])->SetMarkerColor(color+2);
   ((TH1*) oc.o[hName])->SetOption("E1");
   ((TH1*) oc.o[hName])->GetListOfFunctions()->Add(grRVsMeasV, "p");

   // t0
   TGraphErrors *grT0VsMeas = new TGraphErrors();
   grT0VsMeas->SetName("grT0VsMeas");
   grT0VsMeas->SetMarkerStyle(kFullCircle);
   grT0VsMeas->SetMarkerSize(1);
   grT0VsMeas->SetMarkerColor(color);

   Int_t t0Lo = -30;
   Int_t t0Hi =  30;

   Int_t dlLo = 10;
   Int_t dlHi = 80;

   //if (beamE == 250) {
   //   if (polId == kB1U || polId == kY2U) { t0Lo = -10; t0Hi = 5; }
   //   if (polId == kB2D || polId == kY1D) { t0Lo = -20; t0Hi = 5; }

   //   //if (polId == kB2D) { dlLo = 40; dlHi = 60; }

   //} else if (beamE == 24) {
   //   if (polId == kB1U || polId == kY2U) { t0Lo = -20; t0Hi = 0; }
   //   if (polId == kB2D || polId == kY1D) { t0Lo = -30; t0Hi = 0; }

   //   //if (polId == kY1D) { dlLo = 60; dlHi = 80; }
   //}

   sprintf(hName, "hT0VsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   oc.o[hName] = new TH2F(hName, hName, 1, 0, 1, 1, t0Lo, t0Hi);
   ((TH1*) oc.o[hName])->SetTitle(";Measurement;t_{0}, ns;");
   ((TH1*) oc.o[hName])->GetListOfFunctions()->Add(grT0VsMeas, "p");

   sprintf(hName, "hT0VsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
   oc.o[hName] = new TH1F(hName, hName, 1, 0, 1);
   //((TH1*) oc.o[hName])->GetYaxis()->SetRangeUser(t0Lo, t0Hi);
   ((TH1*) oc.o[hName])->SetTitle(";Fill;t_{0}, ns;");
   ((TH1*) oc.o[hName])->SetMarkerStyle(kFullCircle);
   ((TH1*) oc.o[hName])->SetMarkerSize(1);
   ((TH1*) oc.o[hName])->SetMarkerColor(color);
   ((TH1*) oc.o[hName])->SetOption("E1");
   ((TH1*) oc.o[hName])->GetListOfFunctions()->Add(grT0VsMeas, "p");

   TGraphErrors *grT0VsTime = new TGraphErrors();
   grT0VsTime->SetName("grT0VsTime");
   grT0VsTime->SetMarkerStyle(kFullCircle);
   grT0VsTime->SetMarkerSize(1);
   grT0VsTime->SetMarkerColor(color);

   sprintf(hName, "hT0VsTime_%s_%s", strPolId.c_str(), strBeamE.c_str());
   oc.o[hName] = new TH2F(hName, hName, 1, 0, 1, 100, 0, 100);
   ((TH1*) oc.o[hName])->SetTitle(";Date & Time;t_{0}, ns;");
   ((TH1*) oc.o[hName])->GetListOfFunctions()->Add(grT0VsTime, "p");
   ((TH1*) oc.o[hName])->GetXaxis()->SetNdivisions(10, kFALSE);
   ((TH1*) oc.o[hName])->GetXaxis()->SetTimeOffset(0, "gmt");
   ((TH1*) oc.o[hName])->GetXaxis()->SetTimeDisplay(1);
   ((TH1*) oc.o[hName])->GetXaxis()->SetTimeFormat("%m/%d");

   // DL
   TGraphErrors *grDLVsMeas = new TGraphErrors();
   grDLVsMeas->SetName("grDLVsMeas");
   grDLVsMeas->SetMarkerStyle(kFullCircle);
   grDLVsMeas->SetMarkerSize(1);
   grDLVsMeas->SetMarkerColor(color);

   sprintf(hName, "hDLVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   oc.o[hName] = new TH2F(hName, hName, 1, 0, 1, dlHi-dlLo, dlLo, dlHi);
   ((TH1*) oc.o[hName])->SetTitle(";Measurement;Dead Layer, #mug/cm^{2};");
   ((TH1*) oc.o[hName])->GetListOfFunctions()->Add(grDLVsMeas, "p");

   sprintf(hName, "hDLVsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
   oc.o[hName] = new TH1F(hName, hName, 1, 0, 1);
   ((TH1*) oc.o[hName])->SetTitle(";Fill;Dead Layer, #mug/cm^{2};");
   ((TH1*) oc.o[hName])->SetMarkerStyle(kFullCircle);
   ((TH1*) oc.o[hName])->SetMarkerSize(1);
   ((TH1*) oc.o[hName])->SetMarkerColor(color);
   ((TH1*) oc.o[hName])->SetOption("E1");
   ((TH1*) oc.o[hName])->GetListOfFunctions()->Add(grDLVsMeas, "p");

   TGraphErrors *grDLVsTime = new TGraphErrors();
   grDLVsTime->SetName("grDLVsTime");
   grDLVsTime->SetMarkerStyle(kFullCircle);
   grDLVsTime->SetMarkerSize(1);
   grDLVsTime->SetMarkerColor(color);

   sprintf(hName, "hDLVsTime_%s_%s", strPolId.c_str(), strBeamE.c_str());
   oc.o[hName] = new TH2F(hName, hName, 1, 0, 1, 100, 0, 100);
   ((TH1*) oc.o[hName])->SetTitle(";Date & Time;Dead Layer, #mug/cm^{2};");
   ((TH1*) oc.o[hName])->GetListOfFunctions()->Add(grDLVsTime, "p");
   ((TH1*) oc.o[hName])->GetXaxis()->SetNdivisions(10, kFALSE);
   ((TH1*) oc.o[hName])->GetXaxis()->SetTimeOffset(0, "gmt");
   ((TH1*) oc.o[hName])->GetXaxis()->SetTimeDisplay(1);
   ((TH1*) oc.o[hName])->GetXaxis()->SetTimeFormat("%m/%d");

   // Banana fit params
   TGraphErrors *grBananaChi2Ndf = new TGraphErrors();
   grBananaChi2Ndf->SetName("grBananaChi2Ndf");
   grBananaChi2Ndf->SetMarkerStyle(kFullCircle);
   grBananaChi2Ndf->SetMarkerSize(1);
   grBananaChi2Ndf->SetMarkerColor(color);

   sprintf(hName, "hBananaChi2NdfVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   oc.o[hName] = new TH2F(hName, hName, 1, 0, 1, 1, 0, 50);
   ((TH1*) oc.o[hName])->SetTitle(";Measurement;#chi^{2}/ndf;");
   ((TH1*) oc.o[hName])->GetListOfFunctions()->Add(grBananaChi2Ndf, "p");

   // per channel histograms

   ChannelSetIter iCh = gRunInfo->fSiliconChannels.begin();

   for (; iCh!=gRunInfo->fSiliconChannels.end(); ++iCh) {

      string sChId("  ");
      sprintf(&sChId[0], "%02d", *iCh);

      string dName = "channel" + sChId;

      DrawObjContainer        *oc_ch;
      DrawObjContainerMapIter  isubdir = oc.d.find(dName);

      if ( isubdir == oc.d.end()) { // if dir not found
         oc_ch = new DrawObjContainer();
         oc_ch->fDir = new TDirectoryFile(dName.c_str(), dName.c_str(), "", oc.fDir);
      } else {
         oc_ch = isubdir->second;
      }

      // t0
      grT0VsMeas = new TGraphErrors();
      grT0VsMeas->SetName("grT0VsMeas");
      grT0VsMeas->SetMarkerStyle(kFullCircle);
      grT0VsMeas->SetMarkerSize(1);
      grT0VsMeas->SetMarkerColor(color);

      shName = "hT0VsMeas_" + strPolId + "_" + strBeamE + "_" + sChId;
      oc_ch->o[shName] = new TH2F(shName.c_str(), shName.c_str(), 1, 0, 1, t0Hi-t0Lo, t0Lo, t0Hi);
      ((TH1*) oc_ch->o[shName])->SetOption("NOIMG");
      ((TH1*) oc_ch->o[shName])->SetTitle(";Measurement;t_{0}, ns;");
      ((TH1*) oc_ch->o[shName])->GetListOfFunctions()->Add(grT0VsMeas, "p");

      shName = "hT0_" + strPolId + "_" + strBeamE + "_" + sChId;
      oc_ch->o[shName] = new TH1F(shName.c_str(), shName.c_str(), t0Hi-t0Lo, t0Lo, t0Hi);
      ((TH1*) oc_ch->o[shName])->SetOption("hist NOIMG");
      ((TH1*) oc_ch->o[shName])->SetTitle(";t_{0}, ns;Events;");

      // DL
      grDLVsMeas = new TGraphErrors();
      grDLVsMeas->SetName("grDLVsMeas");
      grDLVsMeas->SetMarkerStyle(kFullCircle);
      grDLVsMeas->SetMarkerSize(1);
      grDLVsMeas->SetMarkerColor(color);

      shName = "hDLVsMeas_" + strPolId + "_" + strBeamE + "_" + sChId;
      oc_ch->o[shName] = new TH2F(shName.c_str(), shName.c_str(), 1, 0, 1, dlHi-dlLo, dlLo, dlHi);
      ((TH1*) oc_ch->o[shName])->SetOption("NOIMG");
      ((TH1*) oc_ch->o[shName])->SetTitle(";Measurement;Dead Layer, #mug/cm^{2};");
      ((TH1*) oc_ch->o[shName])->GetListOfFunctions()->Add(grDLVsMeas, "p");

      shName = "hDL_" + strPolId + "_" + strBeamE + "_" + sChId;
      oc_ch->o[shName] = new TH1F(shName.c_str(), shName.c_str(), dlHi-dlLo, dlLo, dlHi);
      ((TH1*) oc_ch->o[shName])->SetOption("hist NOIMG");
      ((TH1*) oc_ch->o[shName])->SetTitle(";Dead Layer, #mug/cm^{2};Events;");

      // t0 vs DL
      TGraphErrors* grT0VsDL = new TGraphErrors();
      grT0VsDL->SetName("grT0VsDL");
      grT0VsDL->SetMarkerStyle(kFullCircle);
      grT0VsDL->SetMarkerSize(1);
      grT0VsDL->SetMarkerColor(color);

      shName = "hT0VsDL_" + strPolId + "_" + strBeamE + "_" + sChId;
      oc_ch->o[shName] = new TH2F(shName.c_str(), shName.c_str(), dlHi-dlLo, dlLo, dlHi, t0Hi-t0Lo, t0Lo, t0Hi);
      ((TH1*) oc_ch->o[shName])->SetOption("NOIMG");
      ((TH1*) oc_ch->o[shName])->SetTitle(";Dead Layer, #mug/cm^{2};t_{0}, ns;");
      ((TH1*) oc_ch->o[shName])->GetListOfFunctions()->Add(grT0VsDL, "p");

      // If this is a new directory then we need to add it to the list
      if ( isubdir == oc.d.end()) {
         oc.d[dName] = oc_ch;
      }
   }

   sprintf(hName, "hT0VsChannel_%s_%s", strPolId.c_str(), strBeamE.c_str());
   oc.o[hName] = new TH1F(hName, hName, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   ((TH1*) oc.o[hName])->SetOption("gridx");
   ((TH1*) oc.o[hName])->SetTitle(";Channel;Mean t_{0}, ns;");

   sprintf(hName, "hDLVsChannel_%s_%s", strPolId.c_str(), strBeamE.c_str());
   oc.o[hName] = new TH1F(hName, hName, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   ((TH1*) oc.o[hName])->SetOption("gridx");
   ((TH1*) oc.o[hName])->SetTitle(";Channel;Mean Dead Layer, #mug/cm^{2};");

   // t0 vs DL
   TGraphErrors* grT0VsDLMean = new TGraphErrors();
   grT0VsDLMean->SetName("grT0VsDLMean");
   grT0VsDLMean->SetMarkerStyle(kFullCircle);
   grT0VsDLMean->SetMarkerSize(1);
   grT0VsDLMean->SetMarkerColor(color);

   sprintf(hName, "hT0VsDLMean_%s_%s", strPolId.c_str(), strBeamE.c_str());
   oc.o[hName] = new TH2F(hName, hName, dlHi-dlLo, dlLo, dlHi, t0Hi-t0Lo, t0Lo, t0Hi);
   ((TH1*) oc.o[hName])->SetTitle(";Dead Layer, #mug/cm^{2};Mean t_{0}, ns;");
   ((TH1*) oc.o[hName])->GetListOfFunctions()->Add(grT0VsDLMean, "p");

   // Combined asymmetry histogram

   shName = "hKinEnergyAChAsym_" + strPolId + "_" + strBeamE;
   oc.o[shName] = new TH1F(shName.c_str(), shName.c_str(), 25, 22.5, 1172.2);
   ((TH1*) oc.o[shName])->SetOption("E1");
   ((TH1*) oc.o[shName])->SetMarkerStyle(kFullCircle);
   ((TH1*) oc.o[shName])->SetMarkerSize(1);
   ((TH1*) oc.o[shName])->SetMarkerColor(color);
   ((TH1*) oc.o[shName])->SetBit(TH1::kIsAverage);
   ((TH1*) oc.o[shName])->SetTitle(";Kinematic Energy, keV;Asymmetry;");

   Double_t xbins[8] = {-20, -5, -3, -1, +1, +3, +5, +20};

   shName = "hLongiChAsym_" + strPolId + "_" + strBeamE;
   oc.o[shName] = new TH1F(shName.c_str(), shName.c_str(), 7, xbins);
   ((TH1*) oc.o[shName])->SetOption("E1");
   ((TH1*) oc.o[shName])->SetMarkerStyle(kFullCircle);
   ((TH1*) oc.o[shName])->SetMarkerSize(1);
   ((TH1*) oc.o[shName])->SetMarkerColor(color);
   ((TH1*) oc.o[shName])->SetBit(TH1::kIsAverage);
   ((TH1*) oc.o[shName])->SetTitle(";Longi. Time Diff, ns;Asymmetry;");

} //}}}


/** */
void MAsymRunHists::Fill(EventConfig &rc)
{ //{{{
   Double_t runId            = rc.fRunInfo->RUNID;
   //UInt_t   fillId           = (UInt_t) runId;
   UInt_t   beamEnergy       = (UInt_t) (rc.fRunInfo->GetBeamEnergy() + 0.5);
   Short_t  polId            = rc.fRunInfo->fPolId;
   time_t   runStartTime     = rc.fRunInfo->StartTime;
   Short_t  targetId         = rc.fMseRunInfoX->target_id;
   Char_t   targetOrient     = rc.fMseRunInfoX->target_orient[0];
   //Float_t  ana_power        = rc.fAnaResult->A_N[1];
   //Float_t  asymmetry        = rc.fAnaResult->sinphi[0].P[0] * rc.fAnaResult->A_N[1];
   //Float_t  asymmetryErr     = rc.fAnaResult->sinphi[0].P[1] * rc.fAnaResult->A_N[1];
   Float_t  polarization     = rc.fAnaResult->sinphi[0].P[0] * 100.;
   Float_t  polarizationErr  = rc.fAnaResult->sinphi[0].P[1] * 100.;
   Float_t  profileRatio     = rc.fAnaResult->fProfilePolarR.first;
   Float_t  profileRatioErr  = rc.fAnaResult->fProfilePolarR.second;
   Float_t  max_rate         = rc.fAnaResult->max_rate;
   Float_t  t0               = rc.fCalibrator->fChannelCalibs[0].fT0Coef;
   Float_t  t0Err            = rc.fCalibrator->fChannelCalibs[0].fT0CoefErr;
   Float_t  dl               = rc.fCalibrator->fChannelCalibs[0].fDLWidth;
   Float_t  dlErr            = rc.fCalibrator->fChannelCalibs[0].fDLWidthErr;

   ChannelCalib *chCalib     = rc.fCalibrator->GetAverage();

   t0    = chCalib->fT0Coef;
   t0Err = chCalib->fT0CoefErr;
   dl    = chCalib->fDLWidth;
   dlErr = chCalib->fDLWidthErr;

   // Some QA checks... should be removed in the future...
   if (isnan(t0) || isinf(t0)) {
      printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
      t0 = 0;
   }

   //t0Err         = chCalib->fT0CoefErr;

   if (gRunConfig.fBeamEnergies.find((EBeamEnergy) beamEnergy) == gRunConfig.fBeamEnergies.end())
      return;

   char hName[256];

   string strPolId = RunConfig::AsString((EPolarimeterId) polId);
   string strBeamE = RunConfig::AsString((EBeamEnergy) beamEnergy);

   // Find corresponding sub directory
   DrawObjContainer *oc_pol = d.find(strPolId)->second;

   TGraphErrors *graphErrs = 0;
   TGraph       *graph = 0;

   // Rate
   sprintf(hName, "hMaxRateVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graph = (TGraph*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grMaxRateVsMeas");
   graph->SetPoint(graph->GetN(), runId, max_rate);

   // Targets
   sprintf(hName, "hTargetVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());

   if (targetOrient == 'H') {
      graph = (TGraph*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grHTargetVsMeas");
   }

   if (targetOrient == 'V') {
      graph = (TGraph*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grVTargetVsMeas");
   }

   graph->SetPoint(graph->GetN(), runId, targetId);

   UInt_t nPoints = 0;

   // Polarization
   sprintf(hName, "hPolarVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grPolarVsMeas");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runId, polarization);
   graphErrs->SetPointError(nPoints, 0, polarizationErr);

   // Polarization vs time
   sprintf(hName, "hPolarVsTime_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grPolarVsTime");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runStartTime, polarization);
   graphErrs->SetPointError(nPoints, 0, polarizationErr);

   // Profiles r
   sprintf(hName, "hRVsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grRVsMeas");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runId, profileRatio);
   graphErrs->SetPointError(nPoints, 0, profileRatioErr);

   sprintf(hName, "hRVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());

   if (targetOrient == 'H') {
      graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grRVsMeasH");
   }

   if (targetOrient == 'V') {
      graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grRVsMeasV");
   }

   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runId, profileRatio);
   graphErrs->SetPointError(nPoints, 0, profileRatioErr);

   // t0
   sprintf(hName, "hT0VsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grT0VsMeas");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runId, t0);
   graphErrs->SetPointError(nPoints, 0, t0Err);

   // t0 vs time
   sprintf(hName, "hT0VsTime_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grT0VsTime");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runStartTime, t0);
   graphErrs->SetPointError(nPoints, 0, t0Err);

   // Dead layer
   sprintf(hName, "hDLVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grDLVsMeas");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runId, dl);
   graphErrs->SetPointError(nPoints, 0, dlErr);

   // DL vs time
   sprintf(hName, "hDLVsTime_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grDLVsTime");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runStartTime, dl);
   graphErrs->SetPointError(nPoints, 0, dlErr);


   // Banana fit params
   //Float_t bananaChi2Ndf = rc.fCalibrator->fChannelCalibs[0].fBananaChi2Ndf;
   Float_t bananaChi2Ndf = chCalib->fBananaChi2Ndf;

   sprintf(hName, "hBananaChi2NdfVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grBananaChi2Ndf");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runId, bananaChi2Ndf);

   // per channel hists
   //ChannelSetIter iCh = gRunInfo->fSiliconChannels.begin();

   for (UShort_t iCh=1; iCh<=N_SILICON_CHANNELS; iCh++) {
   //for (; iCh!=gRunInfo->fSiliconChannels.end(); ++iCh)

      string sChId("  ");
      sprintf(&sChId[0], "%02d", iCh);

      // Find corresponding sub directory
      DrawObjContainer *oc_ch = oc_pol->d.find("channel" + sChId)->second;

      // T0
      t0    = rc.fCalibrator->fChannelCalibs[iCh].fT0Coef;
      t0Err = rc.fCalibrator->fChannelCalibs[iCh].fT0CoefErr;

      // XXX also should be removed when real QA is available
      if (isnan(t0) || isinf(t0) || isnan(t0Err) || isinf(t0Err)) continue;

      string shName = "hT0VsMeas_" + strPolId + "_" + strBeamE + "_" + sChId;
      graphErrs = (TGraphErrors*) ((TH1*) oc_ch->o[shName])->GetListOfFunctions()->FindObject("grT0VsMeas");
      nPoints = graphErrs->GetN();
      graphErrs->SetPoint(nPoints, runId, t0);
      graphErrs->SetPointError(nPoints, 0, t0Err);

      shName = "hT0_" + strPolId + "_" + strBeamE + "_" + sChId;
      ((TH1*) oc_ch->o[shName]) -> Fill(t0);

      // DL
      dl    = rc.fCalibrator->fChannelCalibs[iCh].fDLWidth;
      dlErr = rc.fCalibrator->fChannelCalibs[iCh].fDLWidthErr;

      sprintf(hName, "hDLVsMeas_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), iCh);
      graphErrs = (TGraphErrors*) ((TH1*) oc_ch->o[hName])->GetListOfFunctions()->FindObject("grDLVsMeas");
      nPoints = graphErrs->GetN();
      graphErrs->SetPoint(nPoints, runId, dl);
      graphErrs->SetPointError(nPoints, 0, dlErr);

      sprintf(hName, "hDL_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), iCh);
      ((TH1*) oc_ch->o[hName])->Fill(dl);

      // t0 vs DL
      sprintf(hName, "hT0VsDL_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), iCh);
      graphErrs = (TGraphErrors*) ((TH1*) oc_ch->o[hName])->GetListOfFunctions()->FindObject("grT0VsDL");
      nPoints = graphErrs->GetN();
      graphErrs->SetPoint(nPoints, dl, t0);
      graphErrs->SetPointError(nPoints, dlErr, t0Err);
   }
} //}}}


/** */
void MAsymRunHists::Fill(EventConfig &rc, DrawObjContainer &oc)
{ //{{{
   //if (!oc) {
   //   Error("Fill(DrawObjContainer *oc)", "Argument required");
   //   return;
   //}

   Short_t  polId            = rc.fRunInfo->fPolId;
   UInt_t   beamEnergy       = (UInt_t) (rc.fRunInfo->GetBeamEnergy() + 0.5);

   string strPolId = RunConfig::AsString((EPolarimeterId) polId);
   string strBeamE = RunConfig::AsString((EBeamEnergy) beamEnergy);

   CnipolAsymHists *ah = (CnipolAsymHists*) oc.d.find("asym")->second;

   if (!ah) {
      Error("Fill(DrawObjContainer &oc)", "Asym container required");
      return;
   }

   // Find corresponding sub directory
   DrawObjContainer *oc_pol = d.find(strPolId)->second;

   TH1 *hAsym_meas;
   TH1 *hAsym;

   hAsym_meas = (TH1*) ah->o.find("hKinEnergyAChAsym")->second;

   hAsym = (TH1*) oc_pol->o["hKinEnergyAChAsym_" + strPolId + "_" + strBeamE];

   hAsym_meas->SetBit(TH1::kIsAverage);
   hAsym->Add(hAsym_meas);

   hAsym_meas = (TH1*) ah->o.find("hLongiChAsym")->second;

   hAsym = (TH1*) oc_pol->o["hLongiChAsym_" + strPolId + "_" + strBeamE];
   hAsym_meas->SetBit(TH1::kIsAverage);
   hAsym->Add(hAsym_meas);

   //printf("Found asym hist %s %d\n", hAsym->GetName(), hAsym->GetIntegral());
   //printf("Found asym hist %f\n", hAsym->Integral());
   //printf("Found asym hist \n");

} //}}}


/** */
void MAsymRunHists::Print(const Option_t* opt) const
{ //{{{
   opt = ""; //printf("MAsymRunHists:\n");
   DrawObjContainer::Print();
} //}}}


/** */
void MAsymRunHists::PostFill()
{ //{{{
   char   hName[256];
   string shName;

   for (UInt_t i=0; i!=N_POLARIMETERS; i++) {

      string strPolId = RunConfig::AsString((EPolarimeterId) i);

      // Find corresponding sub directory
      DrawObjContainer *oc_pol = d.find(strPolId)->second;

      // Adjust axis range
      Double_t xmin, ymin, xmax, ymax, xdelta, ydelta;
      Double_t xmean, ymean;
      Double_t xminDL, yminDL, xmaxDL, ymaxDL, xdeltaDL, ydeltaDL;

      IterBeamEnergy iBE=gRunConfig.fBeamEnergies.begin();

      for (; iBE!=gRunConfig.fBeamEnergies.end(); ++iBE) {

         string strBeamE = RunConfig::AsString(*iBE);

         sprintf(hName, "hT0VsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
         TGraphErrors *graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grT0VsMeas");
         graphErrs->ComputeRange(xmin, ymin, xmax, ymax);
         ydelta = (ymax - ymin)*0.1;
         ((TH1*) oc_pol->o[hName])->GetYaxis()->SetLimits(ymin-ydelta, ymax+ydelta);

         //sprintf(hName, "hT0VsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
         //graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grT0VsMeas");
         //((TH1*) oc_pol->o[hName])->GetYaxis()->SetLimits(ymin-ydelta, ymax+ydelta);
         //((TH1*) oc_pol->o[hName])->GetYaxis()->SetRangeUser(ymin-ydelta, ymax+ydelta);

         sprintf(hName, "hT0VsTime_%s_%s", strPolId.c_str(), strBeamE.c_str());
         graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grT0VsTime");
         graphErrs->ComputeRange(xmin, ymin, xmax, ymax);
         ymean = graphErrs->GetMean(2);
         //xdelta = fabs(xmax - xmin)*0.1;
         //ydelta = fabs(ymax - ymin)*0.1;
         ((TH1*) oc_pol->o[hName])->GetXaxis()->SetLimits(xmin, xmax);
         //((TH1*) oc_pol->o[hName])->GetYaxis()->SetLimits(ymin - ydelta, ymax + ydelta);
         ((TH1*) oc_pol->o[hName])->GetYaxis()->SetLimits(ymean - 5, ymean + 5);

         sprintf(hName, "hDLVsTime_%s_%s", strPolId.c_str(), strBeamE.c_str());
         graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grDLVsTime");
         graphErrs->ComputeRange(xmin, ymin, xmax, ymax);
         ymean = graphErrs->GetMean(2);
         //xdelta = fabs(xmax - xmin)*0.1;
         ydelta = fabs(ymax - ymin)*0.1;
         //((TH1*) oc_pol->o[hName])->GetXaxis()->SetLimits(xmin, xmax);
         //((TH1*) oc_pol->o[hName])->GetYaxis()->SetLimits(ymin - ydelta, ymax + ydelta);
         ((TH1*) oc_pol->o[hName])->GetYaxis()->SetLimits(ymean - 20, ymean + 20);

         TGraphErrors *graph;

         // Polarization
         shName = "hPolarVsFill_" + strPolId + "_" + strBeamE;
         ((TH1*) oc_pol->o[shName])->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
         graph = (TGraphErrors*) ((TH1*) oc_pol->o[shName])->GetListOfFunctions()->FindObject("grPolarVsMeas");

         //utils::RemoveOutliers(graph, 2, 3);
         utils::BinGraph(graph, (TH1*) oc_pol->o[shName]);

         ((TH1*) oc_pol->o[shName])->GetListOfFunctions()->Remove(graph);
         ((TH1*) oc_pol->o[shName])->Fit("pol0");

         fHStacks["hsPolarVsFill_" + strBeamE]->Add( (TH1*) oc_pol->o[shName] );


         // Polarization: first measurement in fill
         shName = "hPolarFirstMeasVsFill_" + strPolId + "_" + strBeamE;
         ((TH1*) oc_pol->o[shName])->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
         graph = (TGraphErrors*) ((TH1*) oc_pol->o[shName])->GetListOfFunctions()->FindObject("grPolarVsMeas");

         utils::BinGraphByFirstOnly(graph, (TH1*) oc_pol->o[shName]);

         ((TH1*) oc_pol->o[shName])->GetListOfFunctions()->Remove(graph);

         // Polarization vs time
         //sprintf(hName, "hPolarVsTime_%s_%s", strPolId.c_str(), strBeamE.c_str());
         //graph = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grPolarVsTime");

         //TF1 *funcPolarVsTime = new TF1("funcPolarVsTime", "[0] + [1]*x");
         ////funcPolarVsTime->SetParameter(0, 0);
         //funcPolarVsTime->SetParNames("offset", "slope");
         //graph->Fit("funcPolarVsTime");
         //delete funcPolarVsTime;

         // Profiles r
         shName = "hRVsFill_" + strPolId + "_" + strBeamE;
         graph = (TGraphErrors*) ((TH1*) oc_pol->o[shName])->GetListOfFunctions()->FindObject("grRVsMeas");
         ((TH1*) oc_pol->o[shName])->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

         //utils::RemoveOutliers(graph, 2, 3);
         utils::BinGraph(graph, (TH1*) oc_pol->o[shName]);
         ((TH1*) oc_pol->o[shName])->GetListOfFunctions()->Remove(graph);
         ((TH1*) oc_pol->o[shName])->Fit("pol0");


         // H target = vert profile
         sprintf(hName, "hRVsFill_H_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) oc_pol->o[hName])->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
         graph = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grRVsMeasH");

         utils::RemoveOutliers(graph, 2, 3);
         utils::BinGraph(graph, (TH1*) oc_pol->o[hName]);

         ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->Remove(graph);

         TF1 *funcRVsFill = new TF1("funcRVsFill", "[0]");
         funcRVsFill->SetParameter(0, 0.02);
         ((TH1*) oc_pol->o[hName])->Fit("funcRVsFill");
         //graph->Fit("funcRVsFill");
         delete funcRVsFill;

         // V target = horiz profile
         sprintf(hName, "hRVsFill_V_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) oc_pol->o[hName])->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
         graph = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grRVsMeasV");

         utils::RemoveOutliers(graph, 2, 3);
         utils::BinGraph(graph, (TH1*) oc_pol->o[hName]);

         ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->Remove(graph);

         funcRVsFill = new TF1("funcRVsFill", "[0]");
         funcRVsFill->SetParameter(0, 0.02);
         ((TH1*) oc_pol->o[hName])->Fit("funcRVsFill");
         //graph->Fit("funcRVsFill");
         delete funcRVsFill;

         // t0
         sprintf(hName, "hT0VsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) oc_pol->o[hName])->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
         graph = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grT0VsMeas");

         //utils::RemoveOutliers(graph, 2, 3);
         utils::BinGraph(graph, (TH1*) oc_pol->o[hName]);

         ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->Remove(graph);

         //TF1 *funcT0VsFill = new TF1("funcT0VsFill", "[0] + [1]*x");
         //funcT0VsFill->SetParameters(0, 0);
         //funcT0VsFill->SetParNames("offset", "slope");
         //((TH1*) oc_pol->o[hName])->Fit("funcT0VsFill");
         //delete funcT0VsFill;

         if (strPolId == "B1U" || strPolId == "Y2U" )
            fHStacks["hsT0VsFill_U_" + strBeamE]->Add( (TH1*) oc_pol->o[hName] );
         else
            fHStacks["hsT0VsFill_D_" + strBeamE]->Add( (TH1*) oc_pol->o[hName] );


         // DL
         sprintf(hName, "hDLVsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) oc_pol->o[hName])->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
         graph = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grDLVsMeas");

         utils::RemoveOutliers(graph, 2, 3);
         utils::BinGraph(graph, (TH1*) oc_pol->o[hName]);

         ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->Remove(graph);

         TF1 *funcDLVsFill = new TF1("funcDLVsFill", "[0]");
         funcDLVsFill->SetParNames("const");
         ((TH1*) oc_pol->o[hName])->Fit("funcDLVsFill");
         delete funcDLVsFill;

         if (strPolId == "B1U" || strPolId == "Y2U" )
            fHStacks["hsDLVsFill_U_" + strBeamE]->Add( (TH1*) oc_pol->o[hName] );
         else
            fHStacks["hsDLVsFill_D_" + strBeamE]->Add( (TH1*) oc_pol->o[hName] );

         sprintf(hName, "hT0VsChannel_%s_%s", strPolId.c_str(), strBeamE.c_str());
         TH1* hT0VsChannel = (TH1*) oc_pol->o[hName];

         sprintf(hName, "hDLVsChannel_%s_%s", strPolId.c_str(), strBeamE.c_str());
         TH1* hDLVsChannel = (TH1*) oc_pol->o[hName];

         sprintf(hName, "hT0VsDLMean_%s_%s", strPolId.c_str(), strBeamE.c_str());
         TGraphErrors* grT0VsDLMean = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grT0VsDLMean");


         //ChannelSetIter iCh = gRunInfo->fSiliconChannels.begin();
      
         //for (; iCh!=gRunInfo->fSiliconChannels.end(); ++iCh)
         for (UShort_t iCh=1; iCh<=N_SILICON_CHANNELS; iCh++) {
      
            string sChId("  ");
            sprintf(&sChId[0], "%02d", iCh);
      
            // Find corresponding sub directory
            DrawObjContainer *oc_ch = oc_pol->d.find("channel" + sChId)->second;

            sprintf(hName, "hT0VsMeas_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), iCh);
            graphErrs = (TGraphErrors*) ((TH1*) oc_ch->o[hName])->GetListOfFunctions()->FindObject("grT0VsMeas");
            graphErrs->ComputeRange(xmin, ymin, xmax, ymax);
            ydelta = (ymax - ymin)*0.1;
            ((TH1*) oc_ch->o[hName])->GetYaxis()->SetLimits(ymin-ydelta, ymax+ydelta);

            sprintf(hName, "hDLVsMeas_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), iCh);
            graphErrs = (TGraphErrors*) ((TH1*) oc_ch->o[hName])->GetListOfFunctions()->FindObject("grDLVsMeas");
            graphErrs->ComputeRange(xminDL, yminDL, xmaxDL, ymaxDL);
            ydeltaDL = (ymaxDL - yminDL)*0.1;

            // T0
            sprintf(hName, "hT0_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), iCh);
            TH1* hT0 = (TH1*) oc_ch->o[hName];

            //utils::RemoveOutliers(hT0);

            //if (hT0->Integral()) {
            //   TF1* myGaus = new TF1("myGaus", "gaus(0)");
            //   //myGaus->SetParameters(hT0->Integral());
            //   //myGaus->SetParameter(0, hT0->GetMaximum());
            //   myGaus->SetNpx(1000);
            //   myGaus->FixParameter(0, hT0->GetMaximum());
            //   myGaus->SetParameter(1, hT0->GetMean());
            //   myGaus->SetParLimits(1, hT0->GetMean()-5, hT0->GetMean()+5);
            //   myGaus->SetParameter(2, 1);
            //   myGaus->SetParLimits(2, 0, 10);
            //   TFitResultPtr fitres = hT0->Fit(myGaus, "M I W S");

            hT0VsChannel->SetBinContent(iCh, hT0->GetMean());
            hT0VsChannel->SetBinError(iCh,   hT0->GetRMS());
            //hT0VsChannel->SetBinError(*iCh, fitres->Parameter(2));

            //   //delete fitres;
            //   delete myGaus;
            //}

            // Dead layer
            sprintf(hName, "hDL_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), iCh);
            TH1* hDL = (TH1*) oc_ch->o[hName];
            //utils::RemoveOutliers(hDL);

            hDLVsChannel->SetBinContent(iCh, hDL->GetMean());
            hDLVsChannel->SetBinError(iCh, hDL->GetRMS());

            // t0 vs DL
            sprintf(hName, "hT0VsDL_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), iCh);
            //graphErrs = (TGraphErrors*) ((TH1*) oc_ch->o[hName])->GetListOfFunctions()->FindObject("grT0VsDL");
            ((TH1*) oc_ch->o[hName])->GetXaxis()->SetLimits(yminDL-ydeltaDL, ymaxDL+ydeltaDL);
            ((TH1*) oc_ch->o[hName])->GetYaxis()->SetLimits(ymin-ydelta, ymax+ydelta);

            UInt_t nPoints = grT0VsDLMean->GetN();
            grT0VsDLMean->SetPoint(nPoints, hDL->GetMean(), hT0->GetMean());
            //grT0VsDLMean->SetPointError(nPoints, hDL->GetRMS(), hT0->GetRMS());
         }

         sprintf(hName, "hT0VsDLMean_%s_%s", strPolId.c_str(), strBeamE.c_str());
         grT0VsDLMean = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grT0VsDLMean");
         grT0VsDLMean->ComputeRange(xmin, ymin, xmax, ymax);
         ydelta = fabs(ymax - ymin)*0.1;
         ((TH1*) oc_pol->o[hName])->GetYaxis()->SetLimits(ymin-ydelta, ymax+ydelta);
      }

      TH1* hT0VsChannelDiff  = (TH1*) oc_pol->o["hT0VsChannelDiff_"+strPolId];
      TH1* hT0VsChannelInj   = (TH1*) oc_pol->o["hT0VsChannel_"+strPolId+"_024"];
      TH1* hT0VsChannelFlt   = (TH1*) oc_pol->o["hT0VsChannel_"+strPolId+"_250"];

      hT0VsChannelDiff->Add(hT0VsChannelInj, hT0VsChannelFlt, 1, -1);

      TH1* hDLVsChannelDiff  = (TH1*) oc_pol->o["hDLVsChannelDiff_"+strPolId];
      TH1* hDLVsChannelInj   = (TH1*) oc_pol->o["hDLVsChannel_"+strPolId+"_024"];
      TH1* hDLVsChannelFlt   = (TH1*) oc_pol->o["hDLVsChannel_"+strPolId+"_250"];

      hDLVsChannelDiff->Add(hDLVsChannelInj, hDLVsChannelFlt, 1, -1);

      TH1* hT0VsDLDiff  = (TH1*) oc_pol->o["hT0VsDLDiff_"+strPolId];
      TGraphErrors* grT0VsDLDiff = (TGraphErrors*) hT0VsDLDiff->GetListOfFunctions()->FindObject("grT0VsDLDiff");

      for (UShort_t iCh=1; iCh<=N_SILICON_CHANNELS; iCh++)
      {
         UInt_t nPoints = grT0VsDLDiff->GetN();
         grT0VsDLDiff->SetPoint(nPoints, hDLVsChannelDiff->GetBinContent(iCh), hT0VsChannelDiff->GetBinContent(iCh));
      }

      grT0VsDLDiff->ComputeRange(xmin, ymin, xmax, ymax);
      xdelta = fabs(xmax - xmin)*0.1;
      ydelta = fabs(ymax - ymin)*0.1;
      hT0VsDLDiff->GetXaxis()->SetLimits(xmin-xdelta, xmax+xdelta);
      hT0VsDLDiff->GetYaxis()->SetLimits(ymin-ydelta, ymax+ydelta);

      // Ratio of inj to first meas
      TH1F* hPolarInj = (TH1F*) oc_pol->o["hPolarVsFill_" + strPolId + "_024"];
      TH1F* hPolarFst = (TH1F*) oc_pol->o["hPolarFirstMeasVsFill_" + strPolId + "_250"];
      shName = "hPolarFirstMeasRatioVsFill_" + strPolId;
      ((TH1*) oc_pol->o[shName])->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
      ((TH1*) oc_pol->o[shName])->Divide(hPolarInj, hPolarFst);
   }

   //IterBeamEnergy iBE=gRunConfig.fBeamEnergies.begin();

   //for (; iBE!=gRunConfig.fBeamEnergies.end(); ++iBE)
   //{
   //   string strBeamE = RunConfig::AsString(*iBE);

   //   // Blue
   //   TH1F* hPolarU = (TH1F*) d["B1U"]->o["hPolarVsFill_B1U_" + strBeamE];
   //   TH1F* hPolarD = (TH1F*) d["B2D"]->o["hPolarVsFill_B2D_" + strBeamE];

   //   shName = "hBluePolarRatioVsFill_" + strBeamE;
   //   ((TH1*) o[shName])->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
   //   ((TH1*) o[shName])->Divide(hPolarU, hPolarD);
   //   ((TH1*) o[shName])->Fit("pol0");

   //   // Yellow
   //   hPolarU = (TH1F*) d["Y2U"]->o["hPolarVsFill_Y2U_" + strBeamE];
   //   hPolarD = (TH1F*) d["Y1D"]->o["hPolarVsFill_Y1D_" + strBeamE];

   //   shName = "hYellowPolarRatioVsFill_" + strBeamE;
   //   ((TH1*) o[shName])->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
   //   ((TH1*) o[shName])->Divide(hPolarU, hPolarD);
   //   ((TH1*) o[shName])->Fit("pol0");
   //}
} //}}}


/** */
void MAsymRunHists::PostFill(AnaGlobResult &agr)
{ //{{{
   //IterBeamEnergy iBE=gRunConfig.fBeamEnergies.begin();

   //for (; iBE!=gRunConfig.fBeamEnergies.end(); ++iBE) {

   //string strBeamE = RunConfig::AsString(*iBE);
   string strBeamE = RunConfig::AsString((EBeamEnergy) 250);


   TH1F* hBlueHJetVsFill_ = (TH1F*) o["hBlueHJetVsFill_" + strBeamE];
   hBlueHJetVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

   TH1F* hBluePolarVsFill_ = (TH1F*) o["hBluePolarVsFill_" + strBeamE];
   hBluePolarVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

   TH1F* hBluePolarDiffVsFill_ = (TH1F*) o["hBluePolarDiffVsFill_" + strBeamE];
   hBluePolarDiffVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

   TH1F* hBluePolarRelDiffVsFill_ = (TH1F*) o["hBluePolarRelDiffVsFill_" + strBeamE];
   hBluePolarRelDiffVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);


   TH1F* hYellowHJetVsFill_ = (TH1F*) o["hYellowHJetVsFill_" + strBeamE];
   hYellowHJetVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

   TH1F* hYellowPolarVsFill_ = (TH1F*) o["hYellowPolarVsFill_" + strBeamE];
   hYellowPolarVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

   TH1F* hYellowPolarDiffVsFill_ = (TH1F*) o["hYellowPolarDiffVsFill_" + strBeamE];
   hYellowPolarDiffVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

   TH1F* hYellowPolarRelDiffVsFill_ = (TH1F*) o["hYellowPolarRelDiffVsFill_" + strBeamE];
   hYellowPolarRelDiffVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);


   IterPolarimeterId iPolId=gRunConfig.fPolarimeters.begin();

   for (; iPolId!=gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      string strPolId = RunConfig::AsString(*iPolId);

      // Find corresponding sub directory
      DrawObjContainer *oc_pol = d.find(strPolId)->second;

      TH1F* hPolar2VsFill_ = (TH1F*) oc_pol->o["hPolar2VsFill_" + strPolId + "_" + strBeamE];
      hPolar2VsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

      TH1F* hProfPolarVsFill_ = (TH1F*) oc_pol->o["hProfPolarVsFill_" + strPolId + "_" + strBeamE];
      hProfPolarVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

      TH1F* hProfPolarDiffVsFill_ = (TH1F*) oc_pol->o["hProfPolarDiffVsFill_" + strPolId + "_" + strBeamE];
      hProfPolarDiffVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

      TH1F* hProfPolarRelDiffVsFill_ = (TH1F*) oc_pol->o["hProfPolarRelDiffVsFill_" + strPolId + "_" + strBeamE];
      hProfPolarRelDiffVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

      AnaFillResultMapIter iFill = agr.fAnaFillResults.begin();

      for ( ; iFill != agr.fAnaFillResults.end(); ++iFill)
      {
         UInt_t         fillId   =  iFill->first;
         AnaFillResult *fillRslt = &iFill->second;

         Int_t ib = hPolar2VsFill_->FindBin(fillId);

         // Save beam histograms (thus check once per beam, upstream polarimeters)
         if (*iPolId == kB1U) {

            ValErrPair polarHJ = fillRslt->GetHjetPolar(kBLUE_BEAM);

            if (polarHJ.second > 0) {
               hBlueHJetVsFill_->SetBinContent(ib, polarHJ.first*100);
               hBlueHJetVsFill_->SetBinError(  ib, polarHJ.second*100);
            }
      
            ValErrPair avrgPolarBlue = fillRslt->GetBeamPolar(kBLUE_BEAM);

            if (avrgPolarBlue.second > 0) {
               hBluePolarVsFill_->SetBinContent(ib, avrgPolarBlue.first*100);
               hBluePolarVsFill_->SetBinError(  ib, avrgPolarBlue.second*100);
      
               Double_t diff  = fillRslt->GetSystUvsD(kBLUE_BEAM).first;
      
               if (diff >= 0) { // some reasonable number
                  hBluePolarDiffVsFill_->SetBinContent(ib, diff*100);
                  hBluePolarDiffVsFill_->SetBinError(  ib, 1e-6);
                  hBluePolarRelDiffVsFill_->SetBinContent(ib, diff/avrgPolarBlue.first*100);
                  hBluePolarRelDiffVsFill_->SetBinError(  ib, 1e-6);
               }
            }
         }

         if (*iPolId == kY2U) {

            ValErrPair polarHJ = fillRslt->GetHjetPolar(kYELLOW_BEAM);

            if (polarHJ.second > 0) {
               hYellowHJetVsFill_->SetBinContent(ib, polarHJ.first*100);
               hYellowHJetVsFill_->SetBinError(  ib, polarHJ.second*100);
            }
      
            ValErrPair avrgPolarYellow = fillRslt->GetBeamPolar(kYELLOW_BEAM);

            if (avrgPolarYellow.second > 0) {
               hYellowPolarVsFill_->SetBinContent(ib, avrgPolarYellow.first*100);
               hYellowPolarVsFill_->SetBinError(  ib, avrgPolarYellow.second*100);
      
               Double_t diff  = fillRslt->GetSystUvsD(kYELLOW_BEAM).first;
      
               if (diff >= 0) { // some reasonable number
                  hYellowPolarDiffVsFill_->SetBinContent(ib, diff*100);
                  hYellowPolarDiffVsFill_->SetBinError(  ib, 1e-6);
                  hYellowPolarRelDiffVsFill_->SetBinContent(ib, diff/avrgPolarYellow.first*100);
                  hYellowPolarRelDiffVsFill_->SetBinError(  ib, 1e-6);
               }
            }
         }

         // Consider P and P_prof by polarimeter
         if (fillRslt->fPolars.find(*iPolId) == fillRslt->fPolars.end()) continue;
         //if (fillRslt->fProfPolars.find(*iPolId) == fillRslt->fProfPolars.end()) continue;

         ValErrPair polar     = fillRslt->fPolars[*iPolId];
         ValErrPair polarNorm = fillRslt->GetPolar(*iPolId, &agr.fNormJetCarbon2);

         if (polarNorm.second >= 0) {
            //hPolar2VsFill_->SetBinContent(ib, polar.first*100);
            //hPolar2VsFill_->SetBinError(  ib, polar.second*100);
            hPolar2VsFill_->SetBinContent(ib, polarNorm.first*100);
            hPolar2VsFill_->SetBinError(  ib, polarNorm.second*100);
         }

         ValErrPair profPolar = fillRslt->fProfPolars[*iPolId];

         hProfPolarVsFill_->SetBinContent(ib, profPolar.first*100);
         hProfPolarVsFill_->SetBinError(  ib, profPolar.second*100);

         Double_t diff = fillRslt->GetSystProfPolar(*iPolId).first;

         if (diff >= 0) { // some reasonable number
            hProfPolarDiffVsFill_->SetBinContent(ib, diff*100);
            hProfPolarDiffVsFill_->SetBinError(  ib, 1e-6);

            hProfPolarRelDiffVsFill_->SetBinContent(ib, diff/polar.first*100);
            hProfPolarRelDiffVsFill_->SetBinError(  ib, 1e-6);
         }
      }
   
      TH1F* hProfPolarRelDiff_      = (TH1F*) oc_pol->o["hProfPolarRelDiff_" + strPolId + "_" + strBeamE];
      TH1F* hProfPolarRelDiffCumul_ = (TH1F*) oc_pol->o["hProfPolarRelDiffCumul_" + strPolId + "_" + strBeamE];

      utils::ConvertToProfile(hProfPolarRelDiffVsFill_, hProfPolarRelDiff_, kFALSE);
      utils::ConvertToProfile(hProfPolarRelDiffVsFill_, hProfPolarRelDiffCumul_, kFALSE);
      utils::ConvertToCumulative(hProfPolarRelDiffCumul_);

      hPolar2VsFill_->Fit("pol0");
      hProfPolarVsFill_->Fit("pol0");
   }

   iPolId=gRunConfig.fPolarimeters.begin();

   for (; iPolId!=gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      string strPolId          = RunConfig::AsString(*iPolId);
      DrawObjContainer *oc_pol = d.find(strPolId)->second;

      // Ratio of Hjet over pC
      TH1F* hPolar2VsFill_ = (TH1F*) oc_pol->o["hPolar2VsFill_" + strPolId + "_" + strBeamE];
      TH1F* hNormJCVsFill_ = (TH1F*) oc_pol->o["hNormJCVsFill_" + strPolId + "_" + strBeamE];
      TH1F* hHJetVsFill_   = RunConfig::GetBeamId(*iPolId) == kBLUE_BEAM ? hBlueHJetVsFill_ : hYellowHJetVsFill_;

      //if (RunConfig::GetBeamId(*iPolId) == kBLUE_BEAM )
      //   hNormJCVsFill_->Divide(hBlueHJetVsFill_, hPolar2VsFill_);
      //else
      //   hNormJCVsFill_->Divide(hYellowHJetVsFill_, hPolar2VsFill_);

      hNormJCVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
      hNormJCVsFill_->Divide(hHJetVsFill_, hPolar2VsFill_);
      hNormJCVsFill_->Fit("pol0");
   }
   
   // Blue beam
   TH1F* hBluePolarRelDiff_      = (TH1F*) o["hBluePolarRelDiff_" + strBeamE];
   TH1F* hBluePolarRelDiffCumul_ = (TH1F*) o["hBluePolarRelDiffCumul_" + strBeamE];

   utils::ConvertToProfile(hBluePolarRelDiffVsFill_, hBluePolarRelDiff_, kFALSE);
   utils::ConvertToProfile(hBluePolarRelDiffVsFill_, hBluePolarRelDiffCumul_, kFALSE);
   utils::ConvertToCumulative(hBluePolarRelDiffCumul_);
   
   // Yellow beam
   TH1F* hYellowPolarRelDiff_      = (TH1F*) o["hYellowPolarRelDiff_" + strBeamE];
   TH1F* hYellowPolarRelDiffCumul_ = (TH1F*) o["hYellowPolarRelDiffCumul_" + strBeamE];

   utils::ConvertToProfile(hYellowPolarRelDiffVsFill_, hYellowPolarRelDiff_, kFALSE);
   utils::ConvertToProfile(hYellowPolarRelDiffVsFill_, hYellowPolarRelDiffCumul_, kFALSE);
   utils::ConvertToCumulative(hYellowPolarRelDiffCumul_);

   // Blue beam
   hBlueHJetVsFill_->Fit("pol0");

   TH1F* hPolarU     = (TH1F*) d["B1U"]->o["hPolar2VsFill_B1U_" + strBeamE];
   TH1F* hPolarD     = (TH1F*) d["B2D"]->o["hPolar2VsFill_B2D_" + strBeamE];
   TH1F* hPolarRatio = (TH1F*) o["hBluePolarRatioVsFill_" + strBeamE];

   hPolarRatio->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
   hPolarRatio->Divide(hPolarU, hPolarD);
   hPolarRatio->Fit("pol0");

   // Yellow beam
   hYellowHJetVsFill_->Fit("pol0");

   hPolarU     = (TH1F*) d["Y2U"]->o["hPolar2VsFill_Y2U_" + strBeamE];
   hPolarD     = (TH1F*) d["Y1D"]->o["hPolar2VsFill_Y1D_" + strBeamE];
   hPolarRatio = (TH1F*) o["hYellowPolarRatioVsFill_" + strBeamE];

   hPolarRatio->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
   hPolarRatio->Divide(hPolarU, hPolarD);
   hPolarRatio->Fit("pol0");
} //}}}


/** */
void MAsymRunHists::UpdateLimits()
{ //{{{
   char    hName[256];
   string  shName;

   IterPolarimeterId iPolId=gRunConfig.fPolarimeters.begin();

   for (; iPolId!=gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      string strPolId = RunConfig::AsString(*iPolId);

      // Find corresponding sub directory
      DrawObjContainer *oc_pol = d.find(strPolId)->second;

      Double_t min, max, marg;

      IterBeamEnergy iBE=gRunConfig.fBeamEnergies.begin();

      for (; iBE!=gRunConfig.fBeamEnergies.end(); ++iBE)
      {
         string strBeamE = RunConfig::AsString(*iBE);

         shName = "hMaxRateVsMeas_" + strPolId + "_" + strBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

         shName = "hTargetVsMeas_" + strPolId + "_" + strBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

         shName = "hPolarVsFill_" + strPolId + "_" + strBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         min  = utils::GetNonEmptyMinimum(fHStacks["hsPolarVsFill_" + strBeamE], "nostack");
         max  = utils::GetNonEmptyMaximum(fHStacks["hsPolarVsFill_" + strBeamE], "nostack");
         marg = (max - min) * 0.1;
         ((TH1*) oc_pol->o[shName])->GetYaxis()->SetRangeUser(min-marg, max+marg);

         shName = "hPolarFirstMeasVsFill_" + strPolId + "_" + strBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         ((TH1*) oc_pol->o[shName])->GetYaxis()->SetRangeUser(min-marg, max+marg);

         shName = "hPolar2VsFill_" + strPolId + "_" + strBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         ((TH1*) oc_pol->o[shName])->GetYaxis()->SetRangeUser(min-marg, max+marg);

         shName = "hProfPolarVsFill_" + strPolId + "_" + strBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         ((TH1*) oc_pol->o[shName])->GetYaxis()->SetRangeUser(min-marg, max+marg);

         shName = "hPolarVsMeas_" + strPolId + "_" + strBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         ((TH1*) oc_pol->o[shName])->GetYaxis()->SetLimits(min-marg, max+marg);

         shName = "hPolarVsTime_" + strPolId + "_" + strBeamE;
			//utils::UpdateLimitsFromGraphs((TH1*) oc_pol->o[shName], 1);
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinTime, fMaxTime);
         ((TH1*) oc_pol->o[shName])->GetYaxis()->SetLimits(min-marg, max+marg);


         shName = "hRVsMeas_" + strPolId + "_" + strBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

         shName = "hRVsFill_H_" + strPolId + "_" + strBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

         shName = "hRVsFill_V_" + strPolId + "_" + strBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);


         shName = "hT0VsFill_" + strPolId + "_" + strBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

         if (*iPolId == kB1U || *iPolId == kY2U) {
            min  = utils::GetNonEmptyMinimum(fHStacks["hsT0VsFill_U_" + strBeamE], "nostack");
            max  = utils::GetNonEmptyMaximum(fHStacks["hsT0VsFill_U_" + strBeamE], "nostack");
			} else {
            min  = utils::GetNonEmptyMinimum(fHStacks["hsT0VsFill_D_" + strBeamE], "nostack");
            max  = utils::GetNonEmptyMaximum(fHStacks["hsT0VsFill_D_" + strBeamE], "nostack");
		   }

         marg = (max - min) * 0.1;
         ((TH1*) oc_pol->o[shName])->GetYaxis()->SetRangeUser(min-marg, max+marg);

         sprintf(hName, "hT0VsTime_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) oc_pol->o[hName])->GetXaxis()->SetLimits(fMinTime, fMaxTime);
         ((TH1*) oc_pol->o[hName])->GetYaxis()->SetLimits(min-marg, max+marg);

         sprintf(hName, "hT0VsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) oc_pol->o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         ((TH1*) oc_pol->o[hName])->GetYaxis()->SetLimits(min-marg, max+marg);


         sprintf(hName, "hDLVsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) oc_pol->o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

         if (*iPolId == kB1U || *iPolId == kY2U) {
            min  = utils::GetNonEmptyMinimum(fHStacks["hsDLVsFill_U_" + strBeamE], "nostack");
            max  = utils::GetNonEmptyMaximum(fHStacks["hsDLVsFill_U_" + strBeamE], "nostack");
			} else {
            min  = utils::GetNonEmptyMinimum(fHStacks["hsDLVsFill_D_" + strBeamE], "nostack");
            max  = utils::GetNonEmptyMaximum(fHStacks["hsDLVsFill_D_" + strBeamE], "nostack");
			}

         marg = (max - min) * 0.1;
         ((TH1*) oc_pol->o[hName])->GetYaxis()->SetRangeUser(min-marg, max+marg);

         sprintf(hName, "hDLVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) oc_pol->o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         ((TH1*) oc_pol->o[hName])->GetYaxis()->SetLimits(min-marg, max+marg);

         sprintf(hName, "hDLVsTime_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) oc_pol->o[hName])->GetXaxis()->SetLimits(fMinTime, fMaxTime);
         ((TH1*) oc_pol->o[hName])->GetYaxis()->SetLimits(min-marg, max+marg);


         sprintf(hName, "hBananaChi2NdfVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) oc_pol->o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);


         ChannelSetIter iCh = gRunInfo->fSiliconChannels.begin();
      
         for (; iCh!=gRunInfo->fSiliconChannels.end(); ++iCh) {
      
            string sChId("  ");
            sprintf(&sChId[0], "%02d", *iCh);
      
            // Find corresponding sub directory
            DrawObjContainer *oc_ch = oc_pol->d.find("channel" + sChId)->second;

            sprintf(hName, "hT0VsMeas_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), *iCh);
            ((TH1*) oc_ch->o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

            sprintf(hName, "hDLVsMeas_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), *iCh);
            ((TH1*) oc_ch->o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
	      }

         // Asymmetry histograms
         ((TH1*) oc_pol->o["hKinEnergyAChAsym_" + strPolId + "_" + strBeamE])->GetYaxis()->SetRangeUser(1e-5, 0.015);
         //((TH1*) oc_pol->o["hLongiChAsym_"      + strPolId + "_" + strBeamE])->GetYaxis()->SetRangeUser(1e-5, 0.015);
      }
   }

   IterBeamEnergy iBE=gRunConfig.fBeamEnergies.begin();

   for (; iBE!=gRunConfig.fBeamEnergies.end(); ++iBE)
   {
      string strBeamE = RunConfig::AsString(*iBE);

      sprintf(hName, "hBluePolarRatioVsFill_%s", strBeamE.c_str());
      ((TH1*) o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

      sprintf(hName, "hYellowPolarRatioVsFill_%s", strBeamE.c_str());
      ((TH1*) o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
   }

} //}}}


/** */
void MAsymRunHists::SetMinMaxFill(UInt_t fillId)
{ //{{{
   if (fillId < fMinFill ) fMinFill = fillId;
   if (fillId > fMaxFill ) fMaxFill = fillId;
} //}}}


/** */
void MAsymRunHists::AdjustMinMaxFill()
{ //{{{
   fMinFill -= 0.5;
   fMaxFill += 0.5;
} //}}}


/** */
void MAsymRunHists::SetMinMaxTime(UInt_t time)
{ //{{{
   if (time < fMinTime ) fMinTime = time;
   if (time > fMaxTime ) fMaxTime = time;
} //}}}
