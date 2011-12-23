/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2F.h"
#include "TFitResult.h"
#include "TStyle.h"

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

   string  shName;   
   TH1    *hist;

   //TStyle  styleMarker;
   TAttMarker  styleMarker;

   styleMarker.SetMarkerStyle(kFullCircle);
   styleMarker.SetMarkerSize(1);


   // histogram stacks used for setting histogram limits
   IterBeamEnergy iBE = gRunConfig.fBeamEnergies.begin();

   for ( ; iBE != gRunConfig.fBeamEnergies.end(); ++iBE)
   {
      string sBeamE = RunConfig::AsString(*iBE);

      // Mainly needed to calculate common limits
      shName = "hsPolarVsFill_" + sBeamE;
      fHStacks[shName] = new THStack(shName.c_str(), shName.c_str());

      shName = "hsDLVsFill_U_" + sBeamE;
      fHStacks[shName] = new THStack(shName.c_str(), shName.c_str());

      shName = "hsDLVsFill_D_" + sBeamE;
      fHStacks[shName] = new THStack(shName.c_str(), shName.c_str());

      shName = "hsT0VsFill_U_" + sBeamE;
      fHStacks[shName] = new THStack(shName.c_str(), shName.c_str());

      shName = "hsT0VsFill_D_" + sBeamE;
      fHStacks[shName] = new THStack(shName.c_str(), shName.c_str());
   }


   IterPolarimeterId iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      string  sPolId   = RunConfig::AsString(*iPolId);
      Color_t color    = RunConfig::AsColor(*iPolId);
      string  sDirName = sPolId;

      DrawObjContainer        *oc;
      DrawObjContainerMapIter  isubdir = d.find(sDirName);

      if (isubdir == d.end()) { // if dir not found
         oc = new DrawObjContainer();
         oc->fDir = new TDirectoryFile(sDirName.c_str(), sDirName.c_str(), "", fDir);
      } else {
         oc = isubdir->second;
      }

      IterBeamEnergy iBE = gRunConfig.fBeamEnergies.begin();

      for ( ; iBE != gRunConfig.fBeamEnergies.end(); ++iBE)
      {
         BookHistsByPolarimeter(*oc, *iPolId, *iBE);
      }

      styleMarker.SetMarkerColor(color);

      shName = "hT0VsChannelDiff_" + sPolId;
      hist = new TH1F(shName.c_str(), shName.c_str(), N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      hist->SetOption("gridx");
      hist->SetTitle(";Channel;t_{0} Diff, ns;");
      styleMarker.Copy(*hist); oc->o[shName] = hist;

      shName = "hDLVsChannelDiff_" + sPolId;
      hist = new TH1F(shName.c_str(), shName.c_str(), N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      hist->SetOption("gridx");
      hist->SetTitle(";Channel;Dead Layer Diff, #mug/cm^{2};");
      styleMarker.Copy(*hist); oc->o[shName] = hist;

      // t0 vs DL diff
      TGraphErrors* grT0VsDLDiff = new TGraphErrors();
      grT0VsDLDiff->SetName("grT0VsDLDiff");
      styleMarker.Copy(*grT0VsDLDiff);

      shName = "hT0VsDLDiff_" + sPolId;
      hist = new TH2F(shName.c_str(), shName.c_str(), 1000, 0, 1, 1000, 0, 1);
      hist->SetTitle(";Dead Layer Diff, #mug/cm^{2};t_{0} Diff, ns;");
      hist->GetListOfFunctions()->Add(grT0VsDLDiff, "p");
      styleMarker.Copy(*hist); oc->o[shName] = hist;

      shName = "hPolarFirstMeasRatioVsFill_" + sPolId;
      hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
      hist->GetYaxis()->SetRangeUser(0, 100);
      hist->SetTitle(";Fill;Polarization Ratio (Inj/First);");
      hist->SetOption("E1");
      styleMarker.Copy(*hist); oc->o[shName] = hist;

      // If this is a new directory then we need to add it to the list
      if ( isubdir == d.end()) {
         d[sDirName] = oc;
      }
   }


   RingIdSetIter iRingId = gRunConfig.fRings.begin();

   for ( ; iRingId != gRunConfig.fRings.end(); ++iRingId)
   {
      string sRingId  = RunConfig::AsString(*iRingId);
      string sDirName = sRingId;

      DrawObjContainer        *oc;
      DrawObjContainerMapIter  isubdir = d.find(sDirName);

      if (isubdir == d.end()) { // if dir not found
         oc = new DrawObjContainer();
         oc->fDir = new TDirectoryFile(sDirName.c_str(), sDirName.c_str(), "", fDir);
      } else {
         oc = isubdir->second;
      }

      IterBeamEnergy iBE = gRunConfig.fBeamEnergies.begin();

      for ( ; iBE != gRunConfig.fBeamEnergies.end(); ++iBE)
      {
         string sBeamE = RunConfig::AsString(*iBE);

         BookHistsByRing(*oc, *iRingId, *iBE);
      }

      // If this is a new directory then we need to add it to the list
      if ( isubdir == d.end()) {
         d[sDirName] = oc;
      }
   }
} //}}}


/** */
void MAsymRunHists::BookHistsByPolarimeter(DrawObjContainer &oc, EPolarimeterId polId, EBeamEnergy beamE)
{ //{{{
   string   shName;
   string   sPolId = RunConfig::AsString(polId);
   string   sBeamE = RunConfig::AsString(beamE);
   Color_t  color    = RunConfig::AsColor(polId);
   TH1     *hist;

   //TStyle styleMarker;
   TAttMarker styleMarker;
   styleMarker.SetMarkerStyle(kFullCircle);
   styleMarker.SetMarkerSize(1);
   styleMarker.SetMarkerColor(color);

   // Rate
   shName = "grMaxRateVsMeas";
   TGraphErrors *grMaxRateVsMeas = new TGraphErrors();
   grMaxRateVsMeas->SetName(shName.c_str());
   styleMarker.Copy(*grMaxRateVsMeas);

   shName = "hMaxRateVsMeas_" + sPolId + "_" + sBeamE;
   hist = new TH2F(shName.c_str(), shName.c_str(), 1, 0, 1, 1, 0, 50);
   hist->SetTitle(";Measurement;Max Rate;");
   hist->GetListOfFunctions()->Add(grMaxRateVsMeas, "p");
   styleMarker.Copy(*hist);
   oc.o[shName] = hist;

   // Targets
   shName = "grHTargetVsMeas";
   TGraphErrors *grHTargetVsMeas = new TGraphErrors();
   grHTargetVsMeas->SetName(shName.c_str());
   //grHTargetVsMeas->SetMarkerStyle(kPlus);
   grHTargetVsMeas->SetMarkerStyle(kFullTriangleUp);
   grHTargetVsMeas->SetMarkerSize(2);
   grHTargetVsMeas->SetMarkerColor(color-3);

   shName = "grVTargetVsMeas";
   TGraphErrors *grVTargetVsMeas = new TGraphErrors();
   grVTargetVsMeas->SetName(shName.c_str());
   //grVTargetVsMeas->SetMarkerStyle(kMultiply);
   grVTargetVsMeas->SetMarkerStyle(kFullTriangleDown);
   grVTargetVsMeas->SetMarkerSize(2);
   grVTargetVsMeas->SetMarkerColor(color+2);

   shName = "hTargetVsMeas_" + sPolId + "_" + sBeamE;
   oc.o[shName] = new TH2F(shName.c_str(), shName.c_str(), 1, 0, 1, 1, 0, 7);
   ((TH1*) oc.o[shName])->SetTitle(";Measurement;Target Id;");
   ((TH1*) oc.o[shName])->GetListOfFunctions()->Add(grHTargetVsMeas, "p");
   ((TH1*) oc.o[shName])->GetListOfFunctions()->Add(grVTargetVsMeas, "p");

   // Polarization
   TGraphErrors *grPolarVsMeas = new TGraphErrors();
   grPolarVsMeas->SetName("grPolarVsMeas");
   styleMarker.Copy(*grPolarVsMeas);

   shName = "hPolarVsMeas_" + sPolId + "_" + sBeamE;
   oc.o[shName] = new TH2F(shName.c_str(), shName.c_str(), 1, 0, 1, 1, 0, 100);
   ((TH1*) oc.o[shName])->SetTitle(";Measurement;Polarization, %;");
   ((TH1*) oc.o[shName])->GetListOfFunctions()->Add(grPolarVsMeas, "p");

   shName = "hPolarVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->GetYaxis()->SetRangeUser(0, 100);
   hist->SetTitle(";Fill;Polarization, %;");
   hist->SetOption("E1");
   hist->GetListOfFunctions()->Add(grPolarVsMeas, "p");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   // Add its pointer to the stack
   fHStacks["hsPolarVsFill_" + sBeamE]->Add(hist);

   shName = "hPolarFirstMeasVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->GetYaxis()->SetRangeUser(0, 100);
   hist->SetTitle(";Fill;Polarization, %;");
   hist->SetOption("E1");
   hist->GetListOfFunctions()->Add(grPolarVsMeas, "p");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   shName = "hPolarHJVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->GetYaxis()->SetRangeUser(0, 100);
   hist->SetTitle(";Fill;Polarization (H-jet), %;");
   hist->SetOption("E1");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   shName = "hPolarHJPCRatioVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle(";Fill;P(H-jet)/P(p-Carbon);");
   hist->SetOption("E1");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   // polarization from pol profile
   shName = "hPolarHJPCRatioSystVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle(";Fill;Profile Polarization Rel. Diff., %;");
   hist->SetOption("E1");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   // polarization from pol profile
   //shName = "hPolarHJPCRelDiff_" + sPolId + "_" + sBeamE;
   //hist = new TH1F(shName.c_str(), shName.c_str(), 100, 0, 50);
   //hist->SetTitle(";Profile Polarization Rel. Diff., %;;");
   //hist->SetLineWidth(2);
   //hist->SetLineColor(color);
   //hist->SetOption("hist");
   //oc.o[shName] = hist;

   // polarization from pol profile
   //shName = "hPolarHJPCRelDiffCumul_" + sPolId + "_" + sBeamE;
   //hist = new TH1F(shName.c_str(), shName.c_str(), 100, 0, 50);
   //hist->GetYaxis()->SetRangeUser(0, 1);
   //hist->SetTitle(";Cumul. Profile Polarization Rel. Diff., %;;");
   //hist->SetLineWidth(2);
   //hist->SetLineColor(color);
   //hist->SetOption("hist");
   //oc.o[shName] = hist;


   shName = "hPolarPCNormByHJVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->GetYaxis()->SetRangeUser(0, 100);
   hist->SetTitle(";Fill;Polarization, %;");
   hist->SetOption("E1");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   shName = "hPolarPCNormByHJVsFill_HJOnly_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->GetYaxis()->SetRangeUser(0, 100);
   hist->SetTitle(";Fill;Polarization, %;");
   hist->SetOption("E1");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   // polarization from pol profile
   shName = "hProfPolarVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->GetYaxis()->SetRangeUser(0, 100);
   hist->SetTitle(";Fill;Prof. Polarization, %;");
   hist->SetOption("E1");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   // polarization from pol profile
   shName = "hProfPolarRatioVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->GetYaxis()->SetRangeUser(0, 100);
   hist->SetTitle(";Fill;Polar/Prof. Polar;");
   hist->SetOption("E1");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   // polarization from pol profile
   shName = "hProfPolarRatioSystVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle(";Fill;Profile Polarization Rel. Diff., %;");
   hist->SetOption("E1");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   // polarization from pol profile
   shName = "hProfPolarRelDiff_" + sPolId + "_" + sBeamE;
   oc.o[shName] = new TH1F(shName.c_str(), shName.c_str(), 100, 0, 50);
   ((TH1*) oc.o[shName])->SetTitle(";Profile Polarization Rel. Diff., %;;");
   ((TH1*) oc.o[shName])->SetLineWidth(2);
   ((TH1*) oc.o[shName])->SetLineColor(color);
   ((TH1*) oc.o[shName])->SetOption("hist");

   // polarization from pol profile
   shName = "hProfPolarRelDiffCumul_" + sPolId + "_" + sBeamE;
   oc.o[shName] = new TH1F(shName.c_str(), shName.c_str(), 100, 0, 50);
   ((TH1*) oc.o[shName])->GetYaxis()->SetRangeUser(0, 1);
   ((TH1*) oc.o[shName])->SetTitle(";Cumul. Profile Polarization Rel. Diff., %;;");
   ((TH1*) oc.o[shName])->SetLineWidth(2);
   ((TH1*) oc.o[shName])->SetLineColor(color);
   ((TH1*) oc.o[shName])->SetOption("hist");

   // Ratio Hjet to pC
   shName = "hNormJCVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle(";Fill;H-jet/p-Carbon;");
   hist->SetOption("E1");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   // Polarization vs time
   TGraphErrors *grPolarVsTime = new TGraphErrors();
   grPolarVsTime->SetName("grPolarVsTime");
   styleMarker.Copy(*grPolarVsTime);

   shName = "hPolarVsTime_" + sPolId + "_" + sBeamE;
   hist = new TH2F(shName.c_str(), shName.c_str(), 1, 0, 1, 1, 0, 100);
   hist->SetTitle(";Date & Time;Polarization, %;");
   hist->GetListOfFunctions()->Add(grPolarVsTime, "p");
   hist->GetXaxis()->SetNdivisions(10, kFALSE);
   hist->GetXaxis()->SetTimeOffset(0, "gmt");
   hist->GetXaxis()->SetTimeDisplay(1);
   hist->GetXaxis()->SetTimeFormat("%m/%d");
   //hist->GetXaxis()->SetTimeFormat("%b %d, %H:%M:%S");
   //hist->GetXaxis()->SetTimeFormat("%b %d, %H:%M");
   //hist->GetXaxis()->SetTimeFormat("%H:%M");
   oc.o[shName] = hist;

   // Profile r
   TGraphErrors *grRVsMeas = new TGraphErrors();
   grRVsMeas->SetName("grRVsMeas");
   styleMarker.Copy(*grRVsMeas);

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

   shName = "hRVsMeas_" + sPolId + "_" + sBeamE;
   oc.o[shName] = new TH2F(shName.c_str(), shName.c_str(), 1, 0, 1, 1, -0.3, 1);
   ((TH1*) oc.o[shName])->SetTitle(";Measurement;r;");
   ((TH1*) oc.o[shName])->GetListOfFunctions()->Add(grRVsMeasH, "p");
   ((TH1*) oc.o[shName])->GetListOfFunctions()->Add(grRVsMeasV, "p");

   shName = "hRVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->GetYaxis()->SetRangeUser(-0.3, 1);
   hist->SetTitle(";Fill;r;");
   hist->SetOption("E1");
   hist->GetListOfFunctions()->Add(grRVsMeas, "p");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   shName = "hRVsFill_H_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->GetYaxis()->SetRangeUser(-0.3, 1);
   hist->SetTitle(";Fill;r;");
   hist->SetMarkerStyle(kFullTriangleUp);
   hist->SetMarkerSize(2);
   hist->SetMarkerColor(color-3);
   hist->SetOption("E1");
   hist->GetListOfFunctions()->Add(grRVsMeasH, "p");
   oc.o[shName] = hist;

   shName = "hRVsFill_V_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->GetYaxis()->SetRangeUser(-0.3, 1);
   hist->SetTitle(";Fill;r;");
   hist->SetMarkerStyle(kFullTriangleDown);
   hist->SetMarkerSize(2);
   hist->SetMarkerColor(color+2);
   hist->SetOption("E1");
   hist->GetListOfFunctions()->Add(grRVsMeasV, "p");
   oc.o[shName] = hist;

   // t0
   TGraphErrors *grT0VsMeas = new TGraphErrors();
   grT0VsMeas->SetName("grT0VsMeas");
   styleMarker.Copy(*grT0VsMeas);

   Int_t t0Lo = -30;
   Int_t t0Hi =  30;

   Int_t dlLo = 10;
   Int_t dlHi = 80;

   shName = "hT0VsMeas_" + sPolId + "_" + sBeamE;
   hist = new TH2F(shName.c_str(), shName.c_str(), 1, 0, 1, 1, t0Lo, t0Hi);
   hist->SetTitle(";Measurement;t_{0}, ns;");
   hist->GetListOfFunctions()->Add(grT0VsMeas, "p");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   shName = "hT0VsFill_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   //hist->GetYaxis()->SetRangeUser(t0Lo, t0Hi);
   hist->SetTitle(";Fill;t_{0}, ns;");
   hist->SetMarkerStyle(kFullCircle);
   hist->SetMarkerSize(1);
   hist->SetMarkerColor(color);
   hist->SetOption("E1");
   hist->GetListOfFunctions()->Add(grT0VsMeas, "p");
   //styleMarker.Copy(*hist);
   oc.o[shName] = hist;

   // Add its pointer to the stack
   if (sPolId == "B1U" || sPolId == "Y2U" )
      fHStacks["hsT0VsFill_U_" + sBeamE]->Add(hist);
   else
      fHStacks["hsT0VsFill_D_" + sBeamE]->Add(hist);

   TGraphErrors *grT0VsTime = new TGraphErrors();
   grT0VsTime->SetName("grT0VsTime");
   styleMarker.Copy(*grT0VsTime);

   shName = "hT0VsTime_" + sPolId + "_" + sBeamE;
   hist = new TH2F(shName.c_str(), shName.c_str(), 1, 0, 1, 100, 0, 100);
   hist->SetTitle(";Date & Time;t_{0}, ns;");
   hist->GetListOfFunctions()->Add(grT0VsTime, "p");
   hist->GetXaxis()->SetNdivisions(10, kFALSE);
   hist->GetXaxis()->SetTimeOffset(0, "gmt");
   hist->GetXaxis()->SetTimeDisplay(1);
   hist->GetXaxis()->SetTimeFormat("%m/%d");
   oc.o[shName] = hist;

   // DL
   TGraphErrors *grDLVsMeas = new TGraphErrors();
   grDLVsMeas->SetName("grDLVsMeas");
   styleMarker.Copy(*grDLVsMeas);


   shName = "hDLVsMeas_" + sPolId + "_" + sBeamE;
   hist = new TH2F(shName.c_str(), shName.c_str(), 1, 0, 1, dlHi-dlLo, dlLo, dlHi);
   hist->SetTitle(";Measurement;Dead Layer, #mug/cm^{2};");
   hist->GetListOfFunctions()->Add(grDLVsMeas, "p");
   oc.o[shName] = hist;

   shName = "hDLVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle(";Fill;Dead Layer, #mug/cm^{2};");
   hist->SetMarkerStyle(kFullCircle);
   hist->SetMarkerSize(1);
   hist->SetMarkerColor(color);
   hist->SetOption("E1");
   hist->GetListOfFunctions()->Add(grDLVsMeas, "p");
   oc.o[shName] = hist;

   // Add its pointer to the stack
   if (sPolId == "B1U" || sPolId == "Y2U" )
      fHStacks["hsDLVsFill_U_" + sBeamE]->Add(hist);
   else
      fHStacks["hsDLVsFill_D_" + sBeamE]->Add(hist);

   TGraphErrors *grDLVsTime = new TGraphErrors();
   grDLVsTime->SetName("grDLVsTime");
   styleMarker.Copy(*grDLVsTime);

   shName = "hDLVsTime_" + sPolId + "_" + sBeamE;
   hist = new TH2F(shName.c_str(), shName.c_str(), 1, 0, 1, 100, 0, 100);
   hist->SetTitle(";Date & Time;Dead Layer, #mug/cm^{2};");
   hist->GetListOfFunctions()->Add(grDLVsTime, "p");
   hist->GetXaxis()->SetNdivisions(10, kFALSE);
   hist->GetXaxis()->SetTimeOffset(0, "gmt");
   hist->GetXaxis()->SetTimeDisplay(1);
   hist->GetXaxis()->SetTimeFormat("%m/%d");
   oc.o[shName] = hist;

   // Banana fit params
   TGraphErrors *grBananaChi2Ndf = new TGraphErrors();
   grBananaChi2Ndf->SetName("grBananaChi2Ndf");
   styleMarker.Copy(*grBananaChi2Ndf);

   shName = "hBananaChi2NdfVsMeas_" + sPolId + "_" + sBeamE;
   hist = new TH2F(shName.c_str(), shName.c_str(), 1, 0, 1, 1, 0, 50);
   hist->SetTitle(";Measurement;#chi^{2}/ndf;");
   hist->GetListOfFunctions()->Add(grBananaChi2Ndf, "p");
   oc.o[shName] = hist;

   // per channel histograms

   ChannelSetIter iCh = gRunInfo->fSiliconChannels.begin();

   for (; iCh!=gRunInfo->fSiliconChannels.end(); ++iCh)
   {
      string sChId("  ");
      sprintf(&sChId[0], "%02d", *iCh);

      string sDirName = "channel" + sChId;

      DrawObjContainer        *oc_ch;
      DrawObjContainerMapIter  isubdir = oc.d.find(sDirName);

      if ( isubdir == oc.d.end()) { // if dir not found
         oc_ch = new DrawObjContainer();
         oc_ch->fDir = new TDirectoryFile(sDirName.c_str(), sDirName.c_str(), "", oc.fDir);
      } else {
         oc_ch = isubdir->second;
      }

      // t0
      grT0VsMeas = new TGraphErrors();
      grT0VsMeas->SetName("grT0VsMeas");
      styleMarker.Copy(*grT0VsMeas);

      shName = "hT0VsMeas_" + sPolId + "_" + sBeamE + "_" + sChId;
      oc_ch->o[shName] = new TH2F(shName.c_str(), shName.c_str(), 1, 0, 1, t0Hi-t0Lo, t0Lo, t0Hi);
      ((TH1*) oc_ch->o[shName])->SetOption("NOIMG");
      ((TH1*) oc_ch->o[shName])->SetTitle(";Measurement;t_{0}, ns;");
      ((TH1*) oc_ch->o[shName])->GetListOfFunctions()->Add(grT0VsMeas, "p");

      shName = "hT0_" + sPolId + "_" + sBeamE + "_" + sChId;
      oc_ch->o[shName] = new TH1F(shName.c_str(), shName.c_str(), t0Hi-t0Lo, t0Lo, t0Hi);
      ((TH1*) oc_ch->o[shName])->SetOption("hist NOIMG");
      ((TH1*) oc_ch->o[shName])->SetTitle(";t_{0}, ns;Events;");

      // DL
      grDLVsMeas = new TGraphErrors();
      grDLVsMeas->SetName("grDLVsMeas");
      styleMarker.Copy(*grDLVsMeas);

      shName = "hDLVsMeas_" + sPolId + "_" + sBeamE + "_" + sChId;
      oc_ch->o[shName] = new TH2F(shName.c_str(), shName.c_str(), 1, 0, 1, dlHi-dlLo, dlLo, dlHi);
      ((TH1*) oc_ch->o[shName])->SetOption("NOIMG");
      ((TH1*) oc_ch->o[shName])->SetTitle(";Measurement;Dead Layer, #mug/cm^{2};");
      ((TH1*) oc_ch->o[shName])->GetListOfFunctions()->Add(grDLVsMeas, "p");

      shName = "hDL_" + sPolId + "_" + sBeamE + "_" + sChId;
      oc_ch->o[shName] = new TH1F(shName.c_str(), shName.c_str(), dlHi-dlLo, dlLo, dlHi);
      ((TH1*) oc_ch->o[shName])->SetOption("hist NOIMG");
      ((TH1*) oc_ch->o[shName])->SetTitle(";Dead Layer, #mug/cm^{2};Events;");

      // t0 vs DL
      TGraphErrors* grT0VsDL = new TGraphErrors();
      grT0VsDL->SetName("grT0VsDL");
      styleMarker.Copy(*grT0VsDL);

      shName = "hT0VsDL_" + sPolId + "_" + sBeamE + "_" + sChId;
      oc_ch->o[shName] = new TH2F(shName.c_str(), shName.c_str(), dlHi-dlLo, dlLo, dlHi, t0Hi-t0Lo, t0Lo, t0Hi);
      ((TH1*) oc_ch->o[shName])->SetOption("NOIMG");
      ((TH1*) oc_ch->o[shName])->SetTitle(";Dead Layer, #mug/cm^{2};t_{0}, ns;");
      ((TH1*) oc_ch->o[shName])->GetListOfFunctions()->Add(grT0VsDL, "p");

      // If this is a new directory then we need to add it to the list
      if ( isubdir == oc.d.end()) {
         oc.d[sDirName] = oc_ch;
      }
   }

   shName = "hT0VsChannel_" + sPolId + "_" + sBeamE;
   hist= new TH1F(shName.c_str(), shName.c_str(), N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   hist->SetOption("gridx");
   hist->SetTitle(";Channel;Mean t_{0}, ns;");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   shName = "hDLVsChannel_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   hist->SetOption("gridx");
   hist->SetTitle(";Channel;Mean Dead Layer, #mug/cm^{2};");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   // t0 vs DL
   TGraphErrors* grT0VsDLMean = new TGraphErrors();
   grT0VsDLMean->SetName("grT0VsDLMean");
   styleMarker.Copy(*grT0VsDLMean);

   shName = "hT0VsDLMean_" + sPolId + "_" + sBeamE;
   hist = new TH2F(shName.c_str(), shName.c_str(), dlHi-dlLo, dlLo, dlHi, t0Hi-t0Lo, t0Lo, t0Hi);
   hist->SetTitle(";Dead Layer, #mug/cm^{2};Mean t_{0}, ns;");
   hist->GetListOfFunctions()->Add(grT0VsDLMean, "p");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   // Combined asymmetry histogram

   shName = "hKinEnergyAChAsym_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 25, 22.5, 1172.2);
   hist->SetOption("E1");
   hist->SetBit(TH1::kIsAverage);
   hist->SetTitle(";Kinematic Energy, keV;Asymmetry;");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   Double_t xbins[8] = {-20, -5, -3, -1, +1, +3, +5, +20};

   shName = "hLongiChAsym_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 7, xbins);
   hist->SetOption("E1");
   hist->SetBit(TH1::kIsAverage);
   hist->SetTitle(";Longi. Time Diff, ns;Asymmetry;");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

} //}}}


/** */
void MAsymRunHists::BookHistsByRing(DrawObjContainer &oc, ERingId ringId, EBeamEnergy beamE)
{ //{{{
   string   shName;
   string   sRingId = RunConfig::AsString(ringId);
   string   sBeamE  = RunConfig::AsString(beamE);
   Color_t  color   = RunConfig::AsColor(ringId);
   TH1     *hist;

   //TStyle styleMarker;
   TAttMarker styleMarker;
   styleMarker.SetMarkerStyle(kFullCircle);
   styleMarker.SetMarkerSize(1);
   styleMarker.SetMarkerColor(color);

   shName = "hPolarHJVsFill_" + sRingId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle(";Fill;H-jet Polarization, %;");
   hist->SetOption("E1");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   shName = "hPolarVsFill_" + sRingId + "_"  + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle(";Fill;Beam Polarization, %;");
   hist->SetOption("E1");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   shName = "hPolarRatioVsFill_" + sRingId + "_"  + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0.5, 1.5);
   hist->SetTitle(";Fill;Polarization Ratio;");
   hist->SetOption("E1");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   shName = "hPolarRatioVsFill_HJOnly_" + sRingId + "_"  + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0.5, 1.5);
   hist->SetTitle(";Fill;Polarization Ratio;");
   hist->SetOption("E1");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   shName = "hPolarRatioSystVsFill_" + sRingId + "_"  + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0.5, 1.5);
   hist->SetTitle(";Fill;Polarization Rel. Diff., %;");
   hist->SetOption("E1");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   shName = "hPolarRelDiff_" + sRingId + "_"  + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 140, 0, 70);
   //hist->SetTitle(";#Delta_{U vs. D};;");
   hist->SetTitle(";Polarization Rel. Diff., %;;");
   hist->SetLineWidth(2);
   hist->SetLineColor(color);
   hist->SetOption("hist");
   o[shName] = hist;

   shName = "hPolarRelDiffCumul_" + sRingId + "_"  + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 140, 0, 70);
   //hist->SetTitle(";Cumulative #Delta_{U vs. D};;");
   hist->SetTitle(";Cumulative Pol. Rel. Diff., %;;");
   hist->SetLineWidth(2);
   hist->SetLineColor(color);
   hist->SetOption("hist");
   oc.o[shName] = hist;

   // By target orientation
   shName = "hProfPMaxVsFill_H_" + sRingId + "_"  + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle(";Fill;Prof. P_max;");
   hist->SetOption("E1");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   shName = "hProfPolarVsFill_H_" + sRingId + "_"  + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle(";Fill;Prof. P;");
   hist->SetOption("E1");
   styleMarker.Copy(*hist); oc.o[shName] = hist;
} //}}}


/** */
void MAsymRunHists::Fill(EventConfig &rc)
{ //{{{
   Double_t runId            = rc.fRunInfo->RUNID;
   //UInt_t   fillId           = (UInt_t) runId;
   UInt_t   beamEnergy       = (UInt_t) (rc.fRunInfo->GetBeamEnergy() + 0.5);
   Short_t  polId            = rc.fRunInfo->fPolId;
   time_t   runStartTime     = rc.fRunInfo->fStartTime;
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

   string sPolId = RunConfig::AsString((EPolarimeterId) polId);
   string sBeamE = RunConfig::AsString((EBeamEnergy) beamEnergy);

   // Find corresponding sub directory
   DrawObjContainer *oc_pol = d.find(sPolId)->second;

   TGraphErrors *graphErrs = 0;
   TGraph       *graph = 0;

   // Rate
   sprintf(hName, "hMaxRateVsMeas_%s_%s", sPolId.c_str(), sBeamE.c_str());
   graph = (TGraph*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grMaxRateVsMeas");
   graph->SetPoint(graph->GetN(), runId, max_rate);

   // Targets
   sprintf(hName, "hTargetVsMeas_%s_%s", sPolId.c_str(), sBeamE.c_str());

   if (targetOrient == 'H') {
      graph = (TGraph*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grHTargetVsMeas");
   }

   if (targetOrient == 'V') {
      graph = (TGraph*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grVTargetVsMeas");
   }

   graph->SetPoint(graph->GetN(), runId, targetId);

   UInt_t nPoints = 0;

   // Polarization
   sprintf(hName, "hPolarVsMeas_%s_%s", sPolId.c_str(), sBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grPolarVsMeas");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runId, polarization);
   graphErrs->SetPointError(nPoints, 0, polarizationErr);

   // Polarization vs time
   sprintf(hName, "hPolarVsTime_%s_%s", sPolId.c_str(), sBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grPolarVsTime");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runStartTime, polarization);
   graphErrs->SetPointError(nPoints, 0, polarizationErr);

   // Profiles r
   sprintf(hName, "hRVsFill_%s_%s", sPolId.c_str(), sBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grRVsMeas");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runId, profileRatio);
   graphErrs->SetPointError(nPoints, 0, profileRatioErr);

   sprintf(hName, "hRVsMeas_%s_%s", sPolId.c_str(), sBeamE.c_str());

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
   sprintf(hName, "hT0VsMeas_%s_%s", sPolId.c_str(), sBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grT0VsMeas");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runId, t0);
   graphErrs->SetPointError(nPoints, 0, t0Err);

   // t0 vs time
   sprintf(hName, "hT0VsTime_%s_%s", sPolId.c_str(), sBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grT0VsTime");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runStartTime, t0);
   graphErrs->SetPointError(nPoints, 0, t0Err);

   // Dead layer
   sprintf(hName, "hDLVsMeas_%s_%s", sPolId.c_str(), sBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grDLVsMeas");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runId, dl);
   graphErrs->SetPointError(nPoints, 0, dlErr);

   // DL vs time
   sprintf(hName, "hDLVsTime_%s_%s", sPolId.c_str(), sBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grDLVsTime");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runStartTime, dl);
   graphErrs->SetPointError(nPoints, 0, dlErr);


   // Banana fit params
   //Float_t bananaChi2Ndf = rc.fCalibrator->fChannelCalibs[0].fBananaChi2Ndf;
   Float_t bananaChi2Ndf = chCalib->fBananaChi2Ndf;

   sprintf(hName, "hBananaChi2NdfVsMeas_%s_%s", sPolId.c_str(), sBeamE.c_str());
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

      string shName = "hT0VsMeas_" + sPolId + "_" + sBeamE + "_" + sChId;
      graphErrs = (TGraphErrors*) ((TH1*) oc_ch->o[shName])->GetListOfFunctions()->FindObject("grT0VsMeas");
      nPoints = graphErrs->GetN();
      graphErrs->SetPoint(nPoints, runId, t0);
      graphErrs->SetPointError(nPoints, 0, t0Err);

      shName = "hT0_" + sPolId + "_" + sBeamE + "_" + sChId;
      ((TH1*) oc_ch->o[shName]) -> Fill(t0);

      // DL
      dl    = rc.fCalibrator->fChannelCalibs[iCh].fDLWidth;
      dlErr = rc.fCalibrator->fChannelCalibs[iCh].fDLWidthErr;

      sprintf(hName, "hDLVsMeas_%s_%s_%02d", sPolId.c_str(), sBeamE.c_str(), iCh);
      graphErrs = (TGraphErrors*) ((TH1*) oc_ch->o[hName])->GetListOfFunctions()->FindObject("grDLVsMeas");
      nPoints = graphErrs->GetN();
      graphErrs->SetPoint(nPoints, runId, dl);
      graphErrs->SetPointError(nPoints, 0, dlErr);

      sprintf(hName, "hDL_%s_%s_%02d", sPolId.c_str(), sBeamE.c_str(), iCh);
      ((TH1*) oc_ch->o[hName])->Fill(dl);

      // t0 vs DL
      sprintf(hName, "hT0VsDL_%s_%s_%02d", sPolId.c_str(), sBeamE.c_str(), iCh);
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

   string sPolId = RunConfig::AsString((EPolarimeterId) polId);
   string sBeamE = RunConfig::AsString((EBeamEnergy) beamEnergy);

   CnipolAsymHists *ah = (CnipolAsymHists*) oc.d.find("asym")->second;

   if (!ah) {
      Error("Fill(DrawObjContainer &oc)", "Asym container required");
      return;
   }

   // Find corresponding sub directory
   DrawObjContainer *oc_pol = d.find(sPolId)->second;

   TH1 *hAsym_meas;
   TH1 *hAsym;

   hAsym_meas = (TH1*) ah->o.find("hKinEnergyAChAsym")->second;

   hAsym = (TH1*) oc_pol->o["hKinEnergyAChAsym_" + sPolId + "_" + sBeamE];

   hAsym_meas->SetBit(TH1::kIsAverage);
   hAsym->Add(hAsym_meas);

   hAsym_meas = (TH1*) ah->o.find("hLongiChAsym")->second;

   hAsym = (TH1*) oc_pol->o["hLongiChAsym_" + sPolId + "_" + sBeamE];
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
   string  shName;
   TH1    *hist;

   IterPolarimeterId iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      string sPolId = RunConfig::AsString(*iPolId);

      // Find corresponding sub directory
      DrawObjContainer *oc_pol = d.find(sPolId)->second;

      // Adjust axis range
      Double_t xmin,   ymin, xmax, ymax, xdelta, ydelta;
      Double_t xmean,  ymean;
      Double_t xminDL, yminDL, xmaxDL, ymaxDL, xdeltaDL, ydeltaDL;

      IterBeamEnergy iBE = gRunConfig.fBeamEnergies.begin();

      for ( ; iBE != gRunConfig.fBeamEnergies.end(); ++iBE) {

         string sBeamE = RunConfig::AsString(*iBE);

         hist  = (TH1*) oc_pol->o["hT0VsMeas_" + sPolId + "_" + sBeamE];
         TGraphErrors *graphErrs = (TGraphErrors*) hist->GetListOfFunctions()->FindObject("grT0VsMeas");
         graphErrs->ComputeRange(xmin, ymin, xmax, ymax);
         ydelta = (ymax - ymin)*0.1;
         hist->GetYaxis()->SetLimits(ymin-ydelta, ymax+ydelta);

         //sprintf(hName, "hT0VsFill_%s_%s", sPolId.c_str(), sBeamE.c_str());
         //graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grT0VsMeas");
         //((TH1*) oc_pol->o[hName])->GetYaxis()->SetLimits(ymin-ydelta, ymax+ydelta);
         //((TH1*) oc_pol->o[hName])->GetYaxis()->SetRangeUser(ymin-ydelta, ymax+ydelta);

         hist  = (TH1*) oc_pol->o["hT0VsTime_" + sPolId + "_" + sBeamE];
         graphErrs = (TGraphErrors*) hist->GetListOfFunctions()->FindObject("grT0VsTime");
         graphErrs->ComputeRange(xmin, ymin, xmax, ymax);
         ymean = graphErrs->GetMean(2);
         //xdelta = fabs(xmax - xmin)*0.1;
         //ydelta = fabs(ymax - ymin)*0.1;
         hist->GetXaxis()->SetLimits(xmin, xmax);
         //hist->GetYaxis()->SetLimits(ymin - ydelta, ymax + ydelta);
         hist->GetYaxis()->SetLimits(ymean - 5, ymean + 5);

         hist  = (TH1*) oc_pol->o["hDLVsTime_" + sPolId + "_" + sBeamE];
         graphErrs = (TGraphErrors*) hist->GetListOfFunctions()->FindObject("grDLVsTime");
         graphErrs->ComputeRange(xmin, ymin, xmax, ymax);
         ymean = graphErrs->GetMean(2);
         //xdelta = fabs(xmax - xmin)*0.1;
         ydelta = fabs(ymax - ymin)*0.1;
         //hist->GetXaxis()->SetLimits(xmin, xmax);
         //hist->GetYaxis()->SetLimits(ymin - ydelta, ymax + ydelta);
         hist->GetYaxis()->SetLimits(ymean - 20, ymean + 20);

         TGraphErrors *graph;

         // Polarization
         hist  = (TH1*) oc_pol->o["hPolarVsFill_" + sPolId + "_" + sBeamE];
         graph = (TGraphErrors*) hist->GetListOfFunctions()->FindObject("grPolarVsMeas");
         hist->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
         //utils::RemoveOutliers(graph, 2, 3);
         utils::BinGraph(graph, hist);
         hist->GetListOfFunctions()->Remove(graph);
         hist->Fit("pol0");

         // Polarization: first measurement in fill
         hist  = (TH1*) oc_pol->o["hPolarFirstMeasVsFill_" + sPolId + "_" + sBeamE];
         graph = (TGraphErrors*) hist->GetListOfFunctions()->FindObject("grPolarVsMeas");
         hist->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
         utils::BinGraphByFirstOnly(graph, hist);
         hist->GetListOfFunctions()->Remove(graph);

         // Polarization vs time
         //sprintf(hName, "hPolarVsTime_%s_%s", sPolId.c_str(), sBeamE.c_str());
         //graph = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grPolarVsTime");

         //TF1 *funcPolarVsTime = new TF1("funcPolarVsTime", "[0] + [1]*x");
         ////funcPolarVsTime->SetParameter(0, 0);
         //funcPolarVsTime->SetParNames("offset", "slope");
         //graph->Fit("funcPolarVsTime");
         //delete funcPolarVsTime;

         // Profiles r
         hist  = (TH1*) oc_pol->o["hRVsFill_" + sPolId + "_" + sBeamE];
         graph = (TGraphErrors*) hist->GetListOfFunctions()->FindObject("grRVsMeas");
         hist->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
         //utils::RemoveOutliers(graph, 2, 3);
         utils::BinGraph(graph, hist);
         hist->GetListOfFunctions()->Remove(graph);
         hist->Fit("pol0");

         // H target = vert profile
         hist  = (TH1*) oc_pol->o["hRVsFill_H_" + sPolId + "_" + sBeamE];
         graph = (TGraphErrors*) hist->GetListOfFunctions()->FindObject("grRVsMeasH");
         hist->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
         //utils::RemoveOutliers(graph, 2, 3);
         utils::BinGraph(graph, hist);
         hist->GetListOfFunctions()->Remove(graph);
         hist->Fit("pol0");

         // V target = horiz profile
         hist  = (TH1*) oc_pol->o["hRVsFill_V_" + sPolId + "_" + sBeamE];
         graph = (TGraphErrors*) hist->GetListOfFunctions()->FindObject("grRVsMeasV");
         hist->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
         //utils::RemoveOutliers(graph, 2, 3);
         utils::BinGraph(graph, hist);
         hist->GetListOfFunctions()->Remove(graph);
         hist->Fit("pol0");

         // t0
         hist  = (TH1*) oc_pol->o["hT0VsFill_" + sPolId + "_" + sBeamE];
         graph = (TGraphErrors*) hist->GetListOfFunctions()->FindObject("grT0VsMeas");
         hist->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
         //utils::RemoveOutliers(graph, 2, 3);
         utils::BinGraph(graph, hist);
         hist->GetListOfFunctions()->Remove(graph);
         hist->Fit("pol1");

         // DL
         hist  = (TH1*) oc_pol->o["hDLVsFill_" + sPolId + "_" + sBeamE];
         graph = (TGraphErrors*) hist->GetListOfFunctions()->FindObject("grDLVsMeas");
         hist->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
         //utils::RemoveOutliers(graph, 2, 3);
         utils::BinGraph(graph, hist);
         hist->GetListOfFunctions()->Remove(graph);
         hist->Fit("pol0");


         TH1* hT0VsChannel_ = (TH1*) oc_pol->o["hT0VsChannel_" + sPolId + "_" + sBeamE];
         TH1* hDLVsChannel_ = (TH1*) oc_pol->o["hDLVsChannel_" + sPolId + "_" + sBeamE];
         TH1* hT0VsDLMean_  = (TH1*) oc_pol->o["hT0VsDLMean_" + sPolId + "_" + sBeamE];

         TGraphErrors* grT0VsDLMean = (TGraphErrors*) hT0VsDLMean_->GetListOfFunctions()->FindObject("grT0VsDLMean");

         //ChannelSetIter iCh = gRunInfo->fSiliconChannels.begin();
      
         //for (; iCh!=gRunInfo->fSiliconChannels.end(); ++iCh)
         for (UShort_t iCh=1; iCh<=N_SILICON_CHANNELS; iCh++)
         {
            string sChId("  ");
            sprintf(&sChId[0], "%02d", iCh);
      
            // Find corresponding sub directory
            DrawObjContainer *oc_ch = oc_pol->d.find("channel" + sChId)->second;

            hist      = (TH1*) oc_ch->o["hT0VsMeas_" + sPolId + "_" + sBeamE + "_" + sChId];
            graphErrs = (TGraphErrors*) hist->GetListOfFunctions()->FindObject("grT0VsMeas");
            graphErrs->ComputeRange(xmin, ymin, xmax, ymax);
            ydelta    = (ymax - ymin)*0.1;
            hist->GetYaxis()->SetLimits(ymin-ydelta, ymax+ydelta);

            hist      = (TH1*) oc_ch->o["hDLVsMeas_" + sPolId + "_" + sBeamE + "_" + sChId];
            graphErrs = (TGraphErrors*) hist->GetListOfFunctions()->FindObject("grDLVsMeas");
            graphErrs->ComputeRange(xminDL, yminDL, xmaxDL, ymaxDL);
            ydeltaDL  = (ymaxDL - yminDL)*0.1;

            // T0
            TH1* hT0 = (TH1*) oc_ch->o["hT0_" + sPolId + "_" + sBeamE + "_" + sChId];

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

            hT0VsChannel_->SetBinContent(iCh, hT0->GetMean());
            hT0VsChannel_->SetBinError(iCh,   hT0->GetRMS());
            //hT0VsChannel_->SetBinError(*iCh, fitres->Parameter(2));

            //   //delete fitres;
            //   delete myGaus;
            //}

            // Dead layer
            TH1* hDL = (TH1*) oc_ch->o["hDL_" + sPolId + "_" + sBeamE + "_" + sChId];
            //utils::RemoveOutliers(hDL);

            hDLVsChannel_->SetBinContent(iCh, hDL->GetMean());
            hDLVsChannel_->SetBinError(iCh, hDL->GetRMS());

            // t0 vs DL
            TH1* hT0VsDL_ = (TH1*) oc_ch->o["hT0VsDL_" + sPolId + "_" + sBeamE + "_" + sChId];
            //graphErrs = (TGraphErrors*) hT0VsDL_->GetListOfFunctions()->FindObject("grT0VsDL");
            hT0VsDL_->GetYaxis()->SetLimits(yminDL-ydeltaDL, ymaxDL+ydeltaDL);
            hT0VsDL_->GetYaxis()->SetLimits(ymin-ydelta, ymax+ydelta);

            UInt_t nPoints = grT0VsDLMean->GetN();
            grT0VsDLMean->SetPoint(nPoints, hDL->GetMean(), hT0->GetMean());
            //grT0VsDLMean->SetPointError(nPoints, hDL->GetRMS(), hT0->GetRMS());
         }

         grT0VsDLMean = (TGraphErrors*) hT0VsDLMean_->GetListOfFunctions()->FindObject("grT0VsDLMean");
         grT0VsDLMean->ComputeRange(xmin, ymin, xmax, ymax);
         ydelta = fabs(ymax - ymin)*0.1;
         hT0VsDLMean_->GetYaxis()->SetLimits(ymin-ydelta, ymax+ydelta);
      }

      TH1* hT0VsChannelDiff_  = (TH1*) oc_pol->o["hT0VsChannelDiff_"+sPolId];
      TH1* hT0VsChannelInj_   = (TH1*) oc_pol->o["hT0VsChannel_"+sPolId+"_024"];
      TH1* hT0VsChannelFlt_   = (TH1*) oc_pol->o["hT0VsChannel_"+sPolId+"_250"];

      hT0VsChannelDiff_->Add(hT0VsChannelInj_, hT0VsChannelFlt_, 1, -1);

      TH1* hDLVsChannelDiff_ = (TH1*) oc_pol->o["hDLVsChannelDiff_"+sPolId];
      TH1* hDLVsChannelInj   = (TH1*) oc_pol->o["hDLVsChannel_"+sPolId+"_024"];
      TH1* hDLVsChannelFlt   = (TH1*) oc_pol->o["hDLVsChannel_"+sPolId+"_250"];

      hDLVsChannelDiff_->Add(hDLVsChannelInj, hDLVsChannelFlt, 1, -1);

      TH1*           hT0VsDLDiff_ = (TH1*) oc_pol->o["hT0VsDLDiff_"+sPolId];
      TGraphErrors* grT0VsDLDiff  = (TGraphErrors*) hT0VsDLDiff_->GetListOfFunctions()->FindObject("grT0VsDLDiff");

      for (UShort_t iCh=1; iCh<=N_SILICON_CHANNELS; iCh++)
      {
         UInt_t nPoints = grT0VsDLDiff->GetN();
         grT0VsDLDiff->SetPoint(nPoints, hDLVsChannelDiff_->GetBinContent(iCh), hT0VsChannelDiff_->GetBinContent(iCh));
      }

      grT0VsDLDiff->ComputeRange(xmin, ymin, xmax, ymax);
      xdelta = fabs(xmax - xmin)*0.1;
      ydelta = fabs(ymax - ymin)*0.1;
      hT0VsDLDiff_->GetXaxis()->SetLimits(xmin-xdelta, xmax+xdelta);
      hT0VsDLDiff_->GetYaxis()->SetLimits(ymin-ydelta, ymax+ydelta);

      // Ratio of inj to first meas
      TH1F* hPolarFirstMeasRatioVsFill_ = (TH1F*) oc_pol->o["hPolarFirstMeasRatioVsFill_" + sPolId];
      TH1F* hPolarInj                   = (TH1F*) oc_pol->o["hPolarVsFill_" + sPolId + "_024"];
      TH1F* hPolarFst                   = (TH1F*) oc_pol->o["hPolarFirstMeasVsFill_" + sPolId + "_250"];

      hPolarFirstMeasRatioVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
      hPolarFirstMeasRatioVsFill_->Divide(hPolarInj, hPolarFst);
      hPolarFirstMeasRatioVsFill_->Fit("pol0");
   }
} //}}}


/** */
void MAsymRunHists::PostFill(AnaGlobResult &agr)
{ //{{{
   AnaFillResultMapIter iFill = agr.fAnaFillResults.begin();

   for ( ; iFill != agr.fAnaFillResults.end(); ++iFill)
   {
      IterBeamEnergy iBE = gRunConfig.fBeamEnergies.begin();
   
      for ( ; iBE != gRunConfig.fBeamEnergies.end(); ++iBE)
      {
         IterPolarimeterId iPolId = gRunConfig.fPolarimeters.begin();
   
         for (; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
         {
            PostFillByPolarimeter(agr, iFill, *iPolId, *iBE);
         }

         RingIdSetIter iRingId = gRunConfig.fRings.begin();
   
         for ( ; iRingId != gRunConfig.fRings.end(); ++iRingId)
         {
            PostFillByRing(agr, iFill, *iRingId, *iBE);
         }
      }
   }


   IterBeamEnergy iBE = gRunConfig.fBeamEnergies.begin();
   
   for ( ; iBE != gRunConfig.fBeamEnergies.end(); ++iBE)
   {
      string sBeamE = RunConfig::AsString(*iBE);

      IterPolarimeterId iPolId = gRunConfig.fPolarimeters.begin();

      for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
      {
         string sPolId = RunConfig::AsString(*iPolId);
         DrawObjContainer *oc_pol = d.find(sPolId)->second;

         TH1F* hPolarHJVsFill_                = (TH1F*) oc_pol->o["hPolarHJVsFill_" + sPolId + "_" + sBeamE];
         TH1F* hPolarHJPCRatioVsFill_         = (TH1F*) oc_pol->o["hPolarHJPCRatioVsFill_" + sPolId + "_" + sBeamE];
         TH1F* hPolarPCNormByHJVsFill_        = (TH1F*) oc_pol->o["hPolarPCNormByHJVsFill_" + sPolId + "_" + sBeamE];
         TH1F* hPolarPCNormByHJVsFill_HJOnly_ = (TH1F*) oc_pol->o["hPolarPCNormByHJVsFill_HJOnly_" + sPolId + "_" + sBeamE];
         TH1F* hProfPolarVsFill_              = (TH1F*) oc_pol->o["hProfPolarVsFill_" + sPolId + "_" + sBeamE];
         TH1F* hProfPolarRatioVsFill_         = (TH1F*) oc_pol->o["hProfPolarRatioVsFill_" + sPolId + "_" + sBeamE];
         TH1F* hPolarHJPCRatioSystVsFill_     = (TH1F*) oc_pol->o["hPolarHJPCRatioSystVsFill_" + sPolId + "_" + sBeamE];
         TH1F* hProfPolarRatioSystVsFill_     = (TH1F*) oc_pol->o["hProfPolarRatioSystVsFill_" + sPolId + "_" + sBeamE];

         // Ratio of Hjet over pC
         TH1F* hNormJCVsFill_          = (TH1F*) oc_pol->o["hNormJCVsFill_" + sPolId + "_" + sBeamE];
         //TH1F* hPolarHJVsFill_         = RunConfig::GetRingId(*iPolId) == kBLUE_BEAM ? hPolarHJVsFill_ : hPolarHJVsFill_;

         hNormJCVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
         hNormJCVsFill_->Divide(hPolarHJVsFill_, hPolarPCNormByHJVsFill_);
         hNormJCVsFill_->Fit("pol0");
   
         //TH1F* hPolarHJPCRelDiff_      = (TH1F*) oc_pol->o["hPolarHJPCRelDiff_" + sPolId + "_" + sBeamE];
         //TH1F* hPolarHJPCRelDiffCumul_ = (TH1F*) oc_pol->o["hPolarHJPCRelDiffCumul_" + sPolId + "_" + sBeamE];
         //TH1F* hProfPolarRelDiff_      = (TH1F*) oc_pol->o["hProfPolarRelDiff_" + sPolId + "_" + sBeamE];
         //TH1F* hProfPolarRelDiffCumul_ = (TH1F*) oc_pol->o["hProfPolarRelDiffCumul_" + sPolId + "_" + sBeamE];
         //utils::ConvertToProfile(hPolarHJPCRatioSystVsFill_, hPolarHJPCRelDiff_, kFALSE);
         //utils::ConvertToProfile(hPolarHJPCRatioSystVsFill_, hPolarHJPCRelDiffCumul_, kFALSE);
         //utils::ConvertToCumulative(hPolarHJPCRelDiffCumul_);
         //utils::ConvertToProfile(hProfPolarRatioSystVsFill_, hProfPolarRelDiff_, kFALSE);
         //utils::ConvertToProfile(hProfPolarRatioSystVsFill_, hProfPolarRelDiffCumul_, kFALSE);
         //utils::ConvertToCumulative(hProfPolarRelDiffCumul_);

         hPolarHJPCRatioVsFill_->Divide(hPolarHJVsFill_, hPolarPCNormByHJVsFill_);

         hPolarHJPCRatioVsFill_->Fit("pol0");
         hPolarHJVsFill_->Fit("pol0");
         hPolarHJPCRatioSystVsFill_->Fit("pol0");
         hPolarPCNormByHJVsFill_->Fit("pol0");
         hPolarPCNormByHJVsFill_HJOnly_->Fit("pol0");
         hProfPolarVsFill_->Fit("pol0");
         hProfPolarRatioSystVsFill_->Fit("pol0");

         utils::Divide(hProfPolarVsFill_, hPolarPCNormByHJVsFill_, 1, hProfPolarRatioVsFill_);
         hProfPolarRatioVsFill_->Fit("pol0");
         hProfPolarRatioVsFill_->GetYaxis()->UnZoom();
      }


      RingIdSetIter iRingId = gRunConfig.fRings.begin();
   
      for ( ; iRingId != gRunConfig.fRings.end(); ++iRingId)
      {
         string sRingId  = RunConfig::AsString(*iRingId);
         DrawObjContainer *oc_ring = d.find(sRingId)->second;

         TH1F* hPolarHJVsFill_        = (TH1F*) oc_ring->o["hPolarHJVsFill_" + sRingId + "_" + sBeamE];
         hPolarHJVsFill_->Fit("pol0");

         TH1F* hPolarRelDiff_         = (TH1F*) oc_ring->o["hPolarRelDiff_" + sRingId + "_" + sBeamE];
         TH1F* hPolarRelDiffCumul_    = (TH1F*) oc_ring->o["hPolarRelDiffCumul_" + sRingId + "_" + sBeamE];
         TH1F* hPolarRatioSystVsFill_ = (TH1F*) oc_ring->o["hPolarRatioSystVsFill_" + sRingId + "_" + sBeamE];

         utils::ConvertToProfile(hPolarRatioSystVsFill_, hPolarRelDiff_, kFALSE);
         utils::ConvertToProfile(hPolarRatioSystVsFill_, hPolarRelDiffCumul_, kFALSE);
         utils::ConvertToCumulative(hPolarRelDiffCumul_);

         hPolarRatioSystVsFill_->Fit("pol0", "W");
         hPolarRatioSystVsFill_->Fit("pol0", "+");
         //hPolarRatioSystVsFill_->GetYaxis()->UnZoom();

         TH1F* hPolarU, *hPolarU_HJOnly;
         TH1F* hPolarD, *hPolarD_HJOnly;

         if (*iRingId == kBLUE_RING) {
            hPolarU        = (TH1F*) d["B1U"]->o["hPolarPCNormByHJVsFill_B1U_" + sBeamE];
            hPolarD        = (TH1F*) d["B2D"]->o["hPolarPCNormByHJVsFill_B2D_" + sBeamE];
            hPolarU_HJOnly = (TH1F*) d["B1U"]->o["hPolarPCNormByHJVsFill_HJOnly_B1U_" + sBeamE];
            hPolarD_HJOnly = (TH1F*) d["B2D"]->o["hPolarPCNormByHJVsFill_HJOnly_B2D_" + sBeamE];
         } else if (*iRingId == kYELLOW_RING) {
            hPolarU        = (TH1F*) d["Y2U"]->o["hPolarPCNormByHJVsFill_Y2U_" + sBeamE];
            hPolarD        = (TH1F*) d["Y1D"]->o["hPolarPCNormByHJVsFill_Y1D_" + sBeamE];
            hPolarU_HJOnly = (TH1F*) d["Y2U"]->o["hPolarPCNormByHJVsFill_HJOnly_Y2U_" + sBeamE];
            hPolarD_HJOnly = (TH1F*) d["Y1D"]->o["hPolarPCNormByHJVsFill_HJOnly_Y1D_" + sBeamE];
         }

         TH1F* hPolarRatioVsFill_ = (TH1F*) oc_ring->o["hPolarRatioVsFill_" + sRingId + "_"  + sBeamE];

         hPolarRatioVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
         //hPolarRatioVsFill_->Divide(hPolarU, hPolarD);
         utils::Divide(hPolarU, hPolarD, 1, hPolarRatioVsFill_);
         hPolarRatioVsFill_->Fit("pol0", "W");
         hPolarRatioVsFill_->Fit("pol0", "+");
         //TVirtualFitter::Fitter(hPolarRatioVsFill_)->SetFCN(MyFittingFunction)
         //hPolarRatioVsFill_->Fit(systRatioFitFunc, "UR");

         TH1F* hPolarRatioVsFill_HJOnly_ = (TH1F*) oc_ring->o["hPolarRatioVsFill_HJOnly_" + sRingId + "_"  + sBeamE];

         hPolarRatioVsFill_HJOnly_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
         //hPolarRatioVsFill_HJOnly_->Divide(hPolarU, hPolarD);
         utils::Divide(hPolarU, hPolarD, 1, hPolarRatioVsFill_HJOnly_);
         hPolarRatioVsFill_HJOnly_->Fit("pol0", "W");
         hPolarRatioVsFill_HJOnly_->Fit("pol0", "+");
      }
   }

   // 
   //TF1 *systRatioFitFunc = new TF1("systRatioFitFunc", new SystRatioFitFunctor(), fMinFill, fMaxFill, 1, "SystRatioFitFunctor");
   //utils::SystRatioFitFunctor *srff = new utils::SystRatioFitFunctor(*hRatio);
   //TF1 *systRatioFitFunc = new TF1("systRatioFitFunc", srff, -10, 10, 0, "SystRatioFitFunctor");
} //}}}


/** */
void MAsymRunHists::PostFillByPolarimeter(AnaGlobResult &agr, AnaFillResultMapIter iafr, EPolarimeterId polId, EBeamEnergy beamE)
{ //{{{
   UInt_t fillId     = iafr->first;
   AnaFillResult afr = iafr->second;

   if (afr.fPolars.find(polId) == afr.fPolars.end()) return;
   //if (afr.fProfPolars.find(polId) == afr.fProfPolars.end()) continue;

   string sPolId = RunConfig::AsString(polId);
   string sBeamE = RunConfig::AsString(beamE);
   
   // Find corresponding sub directory
   DrawObjContainer *oc_pol = d.find(sPolId)->second;
   
   TH1F* hPolarHJVsFill_ = (TH1F*) oc_pol->o["hPolarHJVsFill_" + sPolId + "_" + sBeamE];
   hPolarHJVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
   
   TH1F* hPolarHJPCRatioVsFill_ = (TH1F*) oc_pol->o["hPolarHJPCRatioVsFill_" + sPolId + "_" + sBeamE];
   hPolarHJPCRatioVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
   
   TH1F* hPolarPCNormByHJVsFill_ = (TH1F*) oc_pol->o["hPolarPCNormByHJVsFill_" + sPolId + "_" + sBeamE];
   hPolarPCNormByHJVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
   
   TH1F* hPolarPCNormByHJVsFill_HJOnly_ = (TH1F*) oc_pol->o["hPolarPCNormByHJVsFill_HJOnly_" + sPolId + "_" + sBeamE];
   hPolarPCNormByHJVsFill_HJOnly_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
   
   TH1F* hProfPolarVsFill_ = (TH1F*) oc_pol->o["hProfPolarVsFill_" + sPolId + "_" + sBeamE];
   hProfPolarVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
   
   TH1F* hProfPolarRatioVsFill_ = (TH1F*) oc_pol->o["hProfPolarRatioVsFill_" + sPolId + "_" + sBeamE];
   hProfPolarRatioVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
   
   TH1F* hPolarHJPCRatioSystVsFill_ = (TH1F*) oc_pol->o["hPolarHJPCRatioSystVsFill_" + sPolId + "_" + sBeamE];
   hPolarHJPCRatioSystVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
   
   TH1F* hProfPolarRatioSystVsFill_ = (TH1F*) oc_pol->o["hProfPolarRatioSystVsFill_" + sPolId + "_" + sBeamE];
   hProfPolarRatioSystVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

   Int_t ib = hPolarPCNormByHJVsFill_->FindBin(fillId);

   // Consider P and P_prof by polarimeter

   ValErrPair polarHJ         = afr.GetPolarHJ(polId);
   ValErrPair polarPC         = afr.GetPolarPC(polId);
   //ValErrPair polarPCNorm   = afr.GetPolarPC(polId, &agr.fNormJetCarbon);  // ratio of average
   //ValErrPair polarProfNorm = afr.GetPolarPC(polId, &agr.fNormProfPolar);
   ValErrPair polarPCNorm     = afr.GetPolarPC(polId, &agr.fNormJetCarbon2); // average of ratio
   ValErrPair polarProfNorm   = afr.GetPolarPC(polId, &agr.fNormProfPolar2);

   if (polarPCNorm.second >= 0) //if (polarPC.second >= 0)
   {
      // Save HJ only if corresponding pC is available
      if (polarHJ.second >= 0) {
         hPolarHJVsFill_->SetBinContent(ib, polarHJ.first*100);
         hPolarHJVsFill_->SetBinError(  ib, polarHJ.second*100);

         hPolarPCNormByHJVsFill_HJOnly_->SetBinContent(ib, polarPCNorm.first*100);
         hPolarPCNormByHJVsFill_HJOnly_->SetBinError(  ib, polarPCNorm.second*100);
      }

      //hPolarPCNormByHJVsFill_->SetBinContent(ib, polarPC.first*100);
      //hPolarPCNormByHJVsFill_->SetBinError(  ib, polarPC.second*100);
      hPolarPCNormByHJVsFill_->SetBinContent(ib, polarPCNorm.first*100);
      hPolarPCNormByHJVsFill_->SetBinError(  ib, polarPCNorm.second*100);
   }

   ValErrPair profPolar = afr.fProfPolars[polId];

   hProfPolarVsFill_->SetBinContent(ib, profPolar.first*100);
   hProfPolarVsFill_->SetBinError(  ib, profPolar.second*100);

   ValErrPair systRatio = afr.GetSystJvsCPolar(polId);

   if (systRatio.second >= 0) { // some reasonable number
      hPolarHJPCRatioSystVsFill_->SetBinContent(ib, systRatio.first);
      hPolarHJPCRatioSystVsFill_->SetBinError(  ib, systRatio.second);
   }

   systRatio = afr.GetSystProfPolar(polId);

   if (systRatio.second >= 0) { // some reasonable number
      hProfPolarRatioSystVsFill_->SetBinContent(ib, systRatio.first);
      hProfPolarRatioSystVsFill_->SetBinError(  ib, systRatio.second);
   }
} //}}}


/** */
void MAsymRunHists::PostFillByRing(AnaGlobResult &agr, AnaFillResultMapIter iafr, ERingId ringId, EBeamEnergy beamE)
{ //{{{
   UInt_t fillId     = iafr->first;
   AnaFillResult afr = iafr->second;

   string sRingId = RunConfig::AsString(ringId);
   string sBeamE  = RunConfig::AsString(beamE);

   DrawObjContainer *oc_ring = d.find(sRingId)->second;
   
   TH1F* hPolarHJVsFill_ = (TH1F*) oc_ring->o["hPolarHJVsFill_" + sRingId + "_" + sBeamE];
   hPolarHJVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
   
   TH1F* hPolarVsFill_ = (TH1F*) oc_ring->o["hPolarVsFill_" + sRingId + "_" + sBeamE];
   hPolarVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
   
   TH1F* hPolarRatioSystVsFill_ = (TH1F*) oc_ring->o["hPolarRatioSystVsFill_" + sRingId + "_" + sBeamE];
   hPolarRatioSystVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
   
   TH1F* hProfPMaxVsFill_H_ = (TH1F*) oc_ring->o["hProfPMaxVsFill_H_" + sRingId + "_" + sBeamE];
   hProfPMaxVsFill_H_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
   
   TH1F* hProfPolarVsFill_H_ = (TH1F*) oc_ring->o["hProfPolarVsFill_H_" + sRingId + "_" + sBeamE];
   hProfPolarVsFill_H_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

   ValErrPair polarHJ = afr.GetPolarHJ(ringId);

   Int_t ib = hPolarHJVsFill_->FindBin(fillId);
   
   // Save beam histograms by ring
   if (polarHJ.second > 0) {
      hPolarHJVsFill_->SetBinContent(ib, polarHJ.first*100);
      hPolarHJVsFill_->SetBinError(  ib, polarHJ.second*100);
   }
   
   ValErrPair avrgPolar = afr.GetPolarBeam(ringId);

   if (avrgPolar.second > 0) {
      hPolarVsFill_->SetBinContent(ib, avrgPolar.first*100);
      hPolarVsFill_->SetBinError(  ib, avrgPolar.second*100);
   
      ValErrPair systRatio = afr.GetSystUvsDPolar(ringId);
   
      if (systRatio.second >= 0) { // expect some reasonable number
         hPolarRatioSystVsFill_->SetBinContent(ib, systRatio.first);
         //hPolarRatioSystVsFill_->SetBinError(  ib, systRatio.second);
         hPolarRatioSystVsFill_->SetBinError(  ib, 1e-10);
      }
   }

   // Get polarization from profile meas
   ValErrPair vePMax = afr.GetPolProfPMax(ringId, kTARGET_H);

   if (vePMax.second >= 0) {
      hProfPMaxVsFill_H_->SetBinContent(ib, vePMax.first);
      hProfPMaxVsFill_H_->SetBinError(  ib, vePMax.second);
   }

   ValErrPair veP = afr.GetPolProfP(ringId, kTARGET_H);

   if (veP.second >= 0) {
      hProfPolarVsFill_H_->SetBinContent(ib, veP.first);
      hProfPolarVsFill_H_->SetBinError(  ib, veP.second);
   }
} //}}}


/** */
void MAsymRunHists::UpdateLimits()
{ //{{{
   char    hName[256];
   string  shName;

   IterPolarimeterId iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      string sPolId = RunConfig::AsString(*iPolId);

      // Find corresponding sub directory
      DrawObjContainer *oc_pol = d.find(sPolId)->second;

      Double_t min, max, marg;

      IterBeamEnergy iBE = gRunConfig.fBeamEnergies.begin();

      for (; iBE != gRunConfig.fBeamEnergies.end(); ++iBE)
      {
         string sBeamE = RunConfig::AsString(*iBE);

         shName = "hMaxRateVsMeas_" + sPolId + "_" + sBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

         shName = "hTargetVsMeas_" + sPolId + "_" + sBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

         shName = "hPolarVsFill_" + sPolId + "_" + sBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         min  = utils::GetNonEmptyMinimum(fHStacks["hsPolarVsFill_" + sBeamE], "nostack");
         max  = utils::GetNonEmptyMaximum(fHStacks["hsPolarVsFill_" + sBeamE], "nostack");
         marg = (max - min) * 0.1;
         ((TH1*) oc_pol->o[shName])->GetYaxis()->SetRangeUser(min-marg, max+marg);

         shName = "hPolarFirstMeasVsFill_" + sPolId + "_" + sBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         ((TH1*) oc_pol->o[shName])->GetYaxis()->SetRangeUser(min-marg, max+marg);

         shName = "hPolarHJVsFill_" + sPolId + "_" + sBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         ((TH1*) oc_pol->o[shName])->GetYaxis()->SetRangeUser(min-marg, max+marg);

         shName = "hPolarPCNormByHJVsFill_" + sPolId + "_" + sBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         ((TH1*) oc_pol->o[shName])->GetYaxis()->SetRangeUser(min-marg, max+marg);

         shName = "hPolarPCNormByHJVsFill_HJOnly_" + sPolId + "_" + sBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         ((TH1*) oc_pol->o[shName])->GetYaxis()->SetRangeUser(min-marg, max+marg);

         shName = "hProfPolarVsFill_" + sPolId + "_" + sBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         ((TH1*) oc_pol->o[shName])->GetYaxis()->SetRangeUser(min-marg, max+marg);

         shName = "hPolarVsMeas_" + sPolId + "_" + sBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         ((TH1*) oc_pol->o[shName])->GetYaxis()->SetLimits(min-marg, max+marg);

         shName = "hPolarVsTime_" + sPolId + "_" + sBeamE;
			//utils::UpdateLimitsFromGraphs((TH1*) oc_pol->o[shName], 1);
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinTime, fMaxTime);
         ((TH1*) oc_pol->o[shName])->GetYaxis()->SetLimits(min-marg, max+marg);


         shName = "hRVsMeas_" + sPolId + "_" + sBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

         shName = "hRVsFill_H_" + sPolId + "_" + sBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

         shName = "hRVsFill_V_" + sPolId + "_" + sBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);


         shName = "hT0VsFill_" + sPolId + "_" + sBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

         if (*iPolId == kB1U || *iPolId == kY2U) {
            min  = utils::GetNonEmptyMinimum(fHStacks["hsT0VsFill_U_" + sBeamE], "nostack");
            max  = utils::GetNonEmptyMaximum(fHStacks["hsT0VsFill_U_" + sBeamE], "nostack");
			} else {
            min  = utils::GetNonEmptyMinimum(fHStacks["hsT0VsFill_D_" + sBeamE], "nostack");
            max  = utils::GetNonEmptyMaximum(fHStacks["hsT0VsFill_D_" + sBeamE], "nostack");
		   }

         marg = (max - min) * 0.1;
         ((TH1*) oc_pol->o[shName])->GetYaxis()->SetRangeUser(min-marg, max+marg);

         sprintf(hName, "hT0VsTime_%s_%s", sPolId.c_str(), sBeamE.c_str());
         ((TH1*) oc_pol->o[hName])->GetXaxis()->SetLimits(fMinTime, fMaxTime);
         ((TH1*) oc_pol->o[hName])->GetYaxis()->SetLimits(min-marg, max+marg);

         sprintf(hName, "hT0VsMeas_%s_%s", sPolId.c_str(), sBeamE.c_str());
         ((TH1*) oc_pol->o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         ((TH1*) oc_pol->o[hName])->GetYaxis()->SetLimits(min-marg, max+marg);


         sprintf(hName, "hDLVsFill_%s_%s", sPolId.c_str(), sBeamE.c_str());
         ((TH1*) oc_pol->o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

         if (*iPolId == kB1U || *iPolId == kY2U) {
            min  = utils::GetNonEmptyMinimum(fHStacks["hsDLVsFill_U_" + sBeamE], "nostack");
            max  = utils::GetNonEmptyMaximum(fHStacks["hsDLVsFill_U_" + sBeamE], "nostack");
			} else {
            min  = utils::GetNonEmptyMinimum(fHStacks["hsDLVsFill_D_" + sBeamE], "nostack");
            max  = utils::GetNonEmptyMaximum(fHStacks["hsDLVsFill_D_" + sBeamE], "nostack");
			}

         marg = (max - min) * 0.1;
         ((TH1*) oc_pol->o[hName])->GetYaxis()->SetRangeUser(min-marg, max+marg);

         sprintf(hName, "hDLVsMeas_%s_%s", sPolId.c_str(), sBeamE.c_str());
         ((TH1*) oc_pol->o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         ((TH1*) oc_pol->o[hName])->GetYaxis()->SetLimits(min-marg, max+marg);

         sprintf(hName, "hDLVsTime_%s_%s", sPolId.c_str(), sBeamE.c_str());
         ((TH1*) oc_pol->o[hName])->GetXaxis()->SetLimits(fMinTime, fMaxTime);
         ((TH1*) oc_pol->o[hName])->GetYaxis()->SetLimits(min-marg, max+marg);


         sprintf(hName, "hBananaChi2NdfVsMeas_%s_%s", sPolId.c_str(), sBeamE.c_str());
         ((TH1*) oc_pol->o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);


         ChannelSetIter iCh = gRunInfo->fSiliconChannels.begin();
      
         for (; iCh!=gRunInfo->fSiliconChannels.end(); ++iCh) {
      
            string sChId("  ");
            sprintf(&sChId[0], "%02d", *iCh);
      
            // Find corresponding sub directory
            DrawObjContainer *oc_ch = oc_pol->d.find("channel" + sChId)->second;

            sprintf(hName, "hT0VsMeas_%s_%s_%02d", sPolId.c_str(), sBeamE.c_str(), *iCh);
            ((TH1*) oc_ch->o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

            sprintf(hName, "hDLVsMeas_%s_%s_%02d", sPolId.c_str(), sBeamE.c_str(), *iCh);
            ((TH1*) oc_ch->o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
	      }

         // Asymmetry histograms
         ((TH1*) oc_pol->o["hKinEnergyAChAsym_" + sPolId + "_" + sBeamE])->GetYaxis()->SetRangeUser(1e-5, 0.015);
         //((TH1*) oc_pol->o["hLongiChAsym_"      + sPolId + "_" + sBeamE])->GetYaxis()->SetRangeUser(1e-5, 0.015);
      }
   }

   //IterBeamEnergy iBE=gRunConfig.fBeamEnergies.begin();

   //for (; iBE!=gRunConfig.fBeamEnergies.end(); ++iBE)
   //{
   //   string sBeamE = RunConfig::AsString(*iBE);

   //   shName = "hPolarRatioVsFill_" + sBeamE;
   //   ((TH1*) o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

   //   shName = "hPolarRatioVsFill_HJOnly_" + sBeamE;
   //   ((TH1*) o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

   //   shName = "hPolarRatioVsFill_" + sBeamE;
   //   ((TH1*) o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
   //}

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
