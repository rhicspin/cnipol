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

#include "MseMeasInfo.h"
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
void MAsymRunHists::BookHists()
{
   fDir->cd();

   string  shName;
   TH1    *hist;

   TAttMarker  styleMarker;

   styleMarker.SetMarkerStyle(kFullCircle);
   styleMarker.SetMarkerSize(1);

   // Histogram stacks are used to set common histogram limits
   BeamEnergySetIter iBE = gRunConfig.fBeamEnergies.begin();

   for ( ; iBE != gRunConfig.fBeamEnergies.end(); ++iBE)
   {
      string sBeamE = RunConfig::AsString(*iBE);

      // Mainly needed to calculate common limits
      shName = "hsPolarVsFill_" + sBeamE;
      fHStacks[shName] = new THStack(shName.c_str(), shName.c_str());

      shName = "hsPolarVsMeas_" + sBeamE;
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


   PolarimeterIdSetIter iPolId = gRunConfig.fPolarimeters.begin();

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

      BeamEnergySetIter iBE = gRunConfig.fBeamEnergies.begin();

      for ( ; iBE != gRunConfig.fBeamEnergies.end(); ++iBE)
      {
         BookHistsByPolarimeter(*oc, *iPolId, *iBE);
      }

      styleMarker.SetMarkerColor(color);

      shName = "hT0VsChannelDiff_" + sPolId;
      hist = new TH1F(shName.c_str(), shName.c_str(), N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      hist->SetOption("GRIDX GRIDY");
      hist->SetTitle("; Channel; t_{0} Diff, ns;");
      styleMarker.Copy(*hist);
      oc->o[shName] = hist;

      shName = "hDLVsChannelDiff_" + sPolId;
      hist = new TH1F(shName.c_str(), shName.c_str(), N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      hist->SetOption("GRIDX GRIDY");
      hist->SetTitle("; Channel; Dead Layer Diff, #mug/cm^{2};");
      styleMarker.Copy(*hist);
      oc->o[shName] = hist;

      // t0 vs DL diff
      TGraphErrors* grT0VsDLDiff = new TGraphErrors();
      grT0VsDLDiff->SetName("grT0VsDLDiff");
      styleMarker.Copy(*grT0VsDLDiff);

      shName = "hT0VsDLDiff_" + sPolId;
      hist = new TH2F(shName.c_str(), shName.c_str(), 1000, 0, 1, 1000, 0, 1);
      hist->SetTitle("; Dead Layer Diff, #mug/cm^{2}; t_{0} Diff, ns;");
      hist->GetListOfFunctions()->Add(grT0VsDLDiff, "p");
      styleMarker.Copy(*hist);
      oc->o[shName] = hist;

      shName = "hPolarFirstMeasRatioVsFill_" + sPolId;
      hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
      hist->GetYaxis()->SetRangeUser(0, 100);
      hist->SetTitle("; Fill Id; 1st @ FT/Injection;");
      hist->SetOption("E1");
      styleMarker.Copy(*hist);
      oc->o[shName] = hist;

      // If this is a new directory then we need to add it to the list
      if ( isubdir == d.end())
         d[sDirName] = oc;
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

      BeamEnergySetIter iBE = gRunConfig.fBeamEnergies.begin();
      for ( ; iBE != gRunConfig.fBeamEnergies.end(); ++iBE)
      {
         string sBeamE = RunConfig::AsString(*iBE);

         BookHistsByRing(*oc, *iRingId, *iBE);
      }

      // If this is a new directory then we need to add it to the list
      if ( isubdir == d.end())
         d[sDirName] = oc;
   }


   iBE = gRunConfig.fBeamEnergies.begin();
   for ( ; iBE != gRunConfig.fBeamEnergies.end(); ++iBE)
   {
      string sBeamE = RunConfig::AsString(*iBE);

      shName = "hPolarSlopeVsFill_" + sBeamE;
      hist = utils::ConstructTH1C(shName.c_str(), "; Fill Id; Polar. Decay #frac{dP}{dt}, %/hour;");
      o[shName] = hist;
   }
}


/** */
void MAsymRunHists::BookHistsByPolarimeter(DrawObjContainer &oc, EPolarimeterId polId, EBeamEnergy beamE)
{
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
   hist->SetTitle("; Measurement Id; Max Rate;");
   hist->GetListOfFunctions()->Add(grMaxRateVsMeas, "p");
   styleMarker.Copy(*hist);
   oc.o[shName] = hist;

   // Targets
   shName = "grHTargetVsMeas";
   TGraphErrors *grHTargetVsMeas = new TGraphErrors();
   grHTargetVsMeas->SetName(shName.c_str());
   grHTargetVsMeas->SetMarkerStyle(kFullTriangleUp);
   grHTargetVsMeas->SetMarkerSize(2);
   grHTargetVsMeas->SetMarkerColor(color-3);

   shName = "grVTargetVsMeas";
   TGraphErrors *grVTargetVsMeas = new TGraphErrors();
   grVTargetVsMeas->SetName(shName.c_str());
   grVTargetVsMeas->SetMarkerStyle(kFullTriangleDown);
   grVTargetVsMeas->SetMarkerSize(2);
   grVTargetVsMeas->SetMarkerColor(color+2);

   shName = "hTargetVsMeas_" + sPolId + "_" + sBeamE;
   hist = new TH2C(shName.c_str(), shName.c_str(), 1, 0, 1, 1, 0, 7);
   hist->SetOption("DUMMY");
   hist->SetTitle("; Measurement Id; Target Id;");
   hist->GetListOfFunctions()->Add(grHTargetVsMeas, "p");
   hist->GetListOfFunctions()->Add(grVTargetVsMeas, "p");
   oc.o[shName] = hist;

   // Spin Angle
   TGraphErrors *grSpinAngleVsMeas = new TGraphErrors();
   grSpinAngleVsMeas->SetName("grSpinAngleVsMeas");
   styleMarker.Copy(*grSpinAngleVsMeas);

   shName = "hSpinAngleVsMeas_" + sPolId + "_" + sBeamE;
   hist = new TH2C(shName.c_str(), shName.c_str(), 1, 0, 1, 1, -20, 20);
   hist->SetTitle("; Measurement Id; Spin Angle (Radial Comp.), degrees; ");
   hist->SetOption("DUMMY");
   hist->GetListOfFunctions()->Add(grSpinAngleVsMeas, "p");
   oc.o[shName] = hist;
   // Add this pointer to the stack
   //fHStacks["hsSpinAngleVsMeas_" + sBeamE]->Add(hist);

   // Polarization
   TGraphErrors *grPolarVsMeas = new TGraphErrors();
   grPolarVsMeas->SetName("grPolarVsMeas");
   styleMarker.Copy(*grPolarVsMeas);

   shName = "hPolarVsMeas_" + sPolId + "_" + sBeamE;
   hist = new TH2C(shName.c_str(), shName.c_str(), 1, 0, 1, 1, 0, 100);
   hist->SetTitle("; Measurement Id; Polarization, %;");
   hist->SetOption("DUMMY");
   hist->GetListOfFunctions()->Add(grPolarVsMeas, "p");
   oc.o[shName] = hist;
   // Add this pointer to the stack
   fHStacks["hsPolarVsMeas_" + sBeamE]->Add(hist);

   shName = "hPolarVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->GetYaxis()->SetRangeUser(0, 100);
   hist->SetTitle("; Fill Id; Polarization, %;");
   hist->SetOption("E1 GRIDX");
   hist->GetListOfFunctions()->Add(grPolarVsMeas, "p");
   styleMarker.Copy(*hist); oc.o[shName] = hist;
   // Add this pointer to the stack
   fHStacks["hsPolarVsFill_" + sBeamE]->Add(hist);

   shName = "hPolarFirstMeasVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->GetYaxis()->SetRangeUser(0, 100);
   //hist->GetXaxis()->SetNdivisions(1005);
   hist->SetTitle("; Fill Id; Polarization, %;");
   hist->SetOption("E1 GRIDX");
   hist->GetListOfFunctions()->Add(grPolarVsMeas, "p");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   shName = "hPolarHJVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->GetYaxis()->SetRangeUser(0, 100);
   //hist->GetXaxis()->SetNdivisions(1005);
   hist->SetTitle("; Fill Id; Polarization (H-jet), %;");
   hist->SetOption("E1 GRIDX GRIDY");
   styleMarker.Copy(*hist);
   oc.o[shName] = hist;

   shName = "hPolarPCVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->GetYaxis()->SetRangeUser(0, 100);
   //hist->GetXaxis()->SetNdivisions(1005);
   hist->SetTitle("; Fill Id; Polarization (p-Carbon), %;");
   hist->SetOption("E1");
   styleMarker.Copy(*hist);
   oc.o[shName] = hist;

   // H-Jet/p-Carbon polarization ratio
   shName = "hPolarHJPCRatioVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle("; Fill Id; P(H-jet)/P(p-Carbon);");
   hist->SetOption("E1");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   // polarization from pol profile
   shName = "hPolarHJPCRatioSystVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle("; Fill Id; Profile Polarization Rel. Diff., %;");
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


   shName = "hPolarPCScaledVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->GetYaxis()->SetRangeUser(0, 100);
   hist->SetTitle("; Fill Id; Polarization (p-Carbon, scaled), %;");
   hist->SetOption("E1 GRIDX GRIDY");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   shName = "hPolarPCScaledVsFill_HJOnly_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->GetYaxis()->SetRangeUser(0, 100);
   hist->SetTitle("; Fill Id; Polarization (p-Carbon, scaled), %;");
   hist->SetOption("E1 GRIDX");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   // polarization from pol profile
   shName = "hProfPolarVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->GetYaxis()->SetRangeUser(0, 100);
   hist->SetTitle("; Fill Id; Prof. Polarization, %;");
   hist->SetOption("E1 GRIDX");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   // polarization from pol profile
   shName = "hProfPolarRatioVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->GetYaxis()->SetRangeUser(0, 100);
   hist->SetTitle("; Fill Id; Polar/Prof. Polar;");
   hist->SetOption("E1 GRIDX");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   // polarization from pol profile
   shName = "hProfPolarRatioSystVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle("; Fill Id; Profile Polarization Rel. Diff., %;");
   hist->SetOption("E1 GRIDX");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   // polarization from pol profile
   shName = "hProfPolarRelDiff_" + sPolId + "_" + sBeamE;
   oc.o[shName] = new TH1F(shName.c_str(), shName.c_str(), 100, 0, 50);
   ((TH1*) oc.o[shName])->SetTitle("; Profile Polarization Rel. Diff., %; ;");
   ((TH1*) oc.o[shName])->SetLineWidth(2);
   ((TH1*) oc.o[shName])->SetLineColor(color);
   ((TH1*) oc.o[shName])->SetOption("hist");

   // polarization from pol profile
   shName = "hProfPolarRelDiffCumul_" + sPolId + "_" + sBeamE;
   oc.o[shName] = new TH1F(shName.c_str(), shName.c_str(), 100, 0, 50);
   ((TH1*) oc.o[shName])->GetYaxis()->SetRangeUser(0, 1);
   ((TH1*) oc.o[shName])->SetTitle("; Cumul. Profile Polarization Rel. Diff., %; ;");
   ((TH1*) oc.o[shName])->SetLineWidth(2);
   ((TH1*) oc.o[shName])->SetLineColor(color);
   ((TH1*) oc.o[shName])->SetOption("hist");

   // Ratio Hjet to pC
   shName = "hNormJCVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle("; Fill Id; P_{H-jet}/P_{p-Carbon};");
   hist->SetOption("E1 GRIDX");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   // Polarization vs time
   TGraphErrors *grPolarVsTime = new TGraphErrors();
   grPolarVsTime->SetName("grPolarVsTime");
   styleMarker.Copy(*grPolarVsTime);

   shName = "hPolarVsTime_" + sPolId + "_" + sBeamE;
   hist = new TH2C(shName.c_str(), shName.c_str(), 1, 0, 1, 1, 0, 100);
   hist->SetTitle("; Date & Time; Polarization, %;");
   hist->SetOption("DUMMY");
   hist->GetListOfFunctions()->Add(grPolarVsTime, "p");
   hist->GetXaxis()->SetNdivisions(10, kFALSE);
   hist->GetXaxis()->SetTimeOffset(0, "gmt");
   hist->GetXaxis()->SetTimeDisplay(1);
   hist->GetXaxis()->SetTimeFormat("%m/%d");
   //hist->GetXaxis()->SetTimeFormat("%b %d, %H:%M:%S");
   //hist->GetXaxis()->SetTimeFormat("%b %d, %H:%M");
   //hist->GetXaxis()->SetTimeFormat("%H:%M");
   oc.o[shName] = hist;

   // Profile R
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
   hist = new TH2C(shName.c_str(), shName.c_str(), 100, 0, 1, 100, 0, 1);
   hist->SetTitle("; Measurement Id; R;");
   hist->SetOption("DUMMY");
   hist->GetListOfFunctions()->Add(grRVsMeasH, "p");
   hist->GetListOfFunctions()->Add(grRVsMeasV, "p");
   oc.o[shName] = hist;

   shName = "hRVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH2C(shName.c_str(), shName.c_str(), 100, 0, 1, 100, 0, 1);
   hist->SetTitle("; Fill Id; R;");
   hist->SetOption("DUMMY GRIDX");
   hist->GetListOfFunctions()->Add(grRVsMeas, "p");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   shName = "hRVsFill_H_" + sPolId + "_" + sBeamE;
   hist = new TH2C(shName.c_str(), shName.c_str(), 100, 0, 1, 100, 0, 1);
   hist->SetTitle("; Fill Id; R;");
   hist->SetMarkerStyle(kFullTriangleUp);
   hist->SetMarkerSize(2);
   hist->SetMarkerColor(color-3);
   hist->SetOption("DUMMY GRIDX");
   hist->GetListOfFunctions()->Add(grRVsMeasH, "p");
   oc.o[shName] = hist;

   shName = "hRVsFill_V_" + sPolId + "_" + sBeamE;
   hist = new TH2C(shName.c_str(), shName.c_str(), 100, 0, 1, 100, 0, 1);
   hist->SetTitle("; Fill Id; R;");
   hist->SetMarkerStyle(kFullTriangleDown);
   hist->SetMarkerSize(2);
   hist->SetMarkerColor(color+2);
   hist->SetOption("DUMMY GRIDX");
   hist->GetListOfFunctions()->Add(grRVsMeasV, "p");
   oc.o[shName] = hist;

   // Polarization profile slope
   TGraphErrors *grRSlopeVsFill_H_ = new TGraphErrors();
   grRSlopeVsFill_H_->SetName("grRSlopeVsFill_H_");
   RunConfig::AsMarker(kTARGET_H, polId).Copy(*grRSlopeVsFill_H_);

   TGraphErrors *grRSlopeVsFill_V_ = new TGraphErrors();
   grRSlopeVsFill_V_->SetName("grRSlopeVsFill_V_");
   RunConfig::AsMarker(kTARGET_V, polId).Copy(*grRSlopeVsFill_V_);

   shName = "hRSlopeVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH2C(shName.c_str(), shName.c_str(), 1, 0, 1, 1, -0.2, 0.2);
   hist->SetTitle("; Fill Id; R Slope;");
   hist->SetOption("DUMMY GRIDX");
   hist->GetListOfFunctions()->Add(grRSlopeVsFill_H_, "p");
   hist->GetListOfFunctions()->Add(grRSlopeVsFill_V_, "p");
   oc.o[shName] = hist;


   TGraphErrors *grRSlopeVsPolarSlope_H_ = new TGraphErrors();
   grRSlopeVsPolarSlope_H_->SetName("grRSlopeVsPolarSlope_H_");
   RunConfig::AsMarker(kTARGET_H, polId).Copy(*grRSlopeVsPolarSlope_H_);

   TGraphErrors *grRSlopeVsPolarSlope_V_ = new TGraphErrors();
   grRSlopeVsPolarSlope_V_->SetName("grRSlopeVsPolarSlope_V_");
   RunConfig::AsMarker(kTARGET_V, polId).Copy(*grRSlopeVsPolarSlope_V_);

   shName = "hRSlopeVsPolarSlope_" + sPolId + "_" + sBeamE;
   hist = new TH2C(shName.c_str(), shName.c_str(), 100, 0, 1, 100, 0, 1);
   hist->SetTitle("; dP/dt; dR/dt;");
   hist->SetOption("DUMMY GRIDX GRIDY");
   hist->GetListOfFunctions()->Add(grRSlopeVsPolarSlope_H_, "p");
   hist->GetListOfFunctions()->Add(grRSlopeVsPolarSlope_V_, "p");
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
   hist = new TH2C(shName.c_str(), shName.c_str(), 1, 0, 1, 1, t0Lo, t0Hi);
   hist->SetTitle("; Measurement Id; t_{0}, ns;");
   hist->SetOption("DUMMY");
   hist->GetListOfFunctions()->Add(grT0VsMeas, "p");
   styleMarker.Copy(*hist);
   oc.o[shName] = hist;

   shName = "hT0VsFill_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   //hist->GetYaxis()->SetRangeUser(t0Lo, t0Hi);
   hist->SetTitle("; Fill Id; t_{0}, ns;");
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
   hist->SetTitle("; Date & Time; t_{0}, ns;");
   hist->SetOption("DUMMY");
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
   hist = new TH2C(shName.c_str(), shName.c_str(), 1, 0, 1, dlHi-dlLo, dlLo, dlHi);
   hist->SetTitle("; Measurement Id; Dead Layer, #mug/cm^{2};");
   hist->SetOption("DUMMY");
   hist->GetListOfFunctions()->Add(grDLVsMeas, "p");
   oc.o[shName] = hist;

   shName = "hDLVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle("; Fill Id; Dead Layer, #mug/cm^{2};");
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
   hist = new TH2C(shName.c_str(), shName.c_str(), 1, 0, 1, 100, 0, 100);
   hist->SetTitle("; Date & Time; Dead Layer, #mug/cm^{2};");
   hist->SetOption("DUMMY");
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
   hist->SetTitle("; Measurement Id; #chi^{2}/ndf;");
   hist->GetListOfFunctions()->Add(grBananaChi2Ndf, "p");
   oc.o[shName] = hist;

   // per channel histograms

   ChannelSetIter iCh = gMeasInfo->fSiliconChannels.begin();

   for (; iCh!=gMeasInfo->fSiliconChannels.end(); ++iCh)
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
      ((TH1*) oc_ch->o[shName])->SetTitle("; Measurement Id; t_{0}, ns;");
      ((TH1*) oc_ch->o[shName])->SetOption("NOIMG");
      ((TH1*) oc_ch->o[shName])->GetListOfFunctions()->Add(grT0VsMeas, "p");

      shName = "hT0_" + sPolId + "_" + sBeamE + "_" + sChId;
      oc_ch->o[shName] = new TH1F(shName.c_str(), shName.c_str(), t0Hi-t0Lo, t0Lo, t0Hi);
      ((TH1*) oc_ch->o[shName])->SetTitle("; t_{0}, ns; Events;");
      ((TH1*) oc_ch->o[shName])->SetOption("hist NOIMG");

      // DL
      grDLVsMeas = new TGraphErrors();
      grDLVsMeas->SetName("grDLVsMeas");
      styleMarker.Copy(*grDLVsMeas);

      shName = "hDLVsMeas_" + sPolId + "_" + sBeamE + "_" + sChId;
      oc_ch->o[shName] = new TH2F(shName.c_str(), shName.c_str(), 1, 0, 1, dlHi-dlLo, dlLo, dlHi);
      ((TH1*) oc_ch->o[shName])->SetOption("NOIMG");
      ((TH1*) oc_ch->o[shName])->SetTitle("; Measurement Id; Dead Layer, #mug/cm^{2};");
      ((TH1*) oc_ch->o[shName])->GetListOfFunctions()->Add(grDLVsMeas, "p");

      shName = "hDL_" + sPolId + "_" + sBeamE + "_" + sChId;
      oc_ch->o[shName] = new TH1F(shName.c_str(), shName.c_str(), dlHi-dlLo, dlLo, dlHi);
      ((TH1*) oc_ch->o[shName])->SetOption("hist NOIMG");
      ((TH1*) oc_ch->o[shName])->SetTitle("; Dead Layer, #mug/cm^{2}; Events;");

      // t0 vs DL
      TGraphErrors* grT0VsDL = new TGraphErrors();
      grT0VsDL->SetName("grT0VsDL");
      styleMarker.Copy(*grT0VsDL);

      shName = "hT0VsDL_" + sPolId + "_" + sBeamE + "_" + sChId;
      oc_ch->o[shName] = new TH2F(shName.c_str(), shName.c_str(), dlHi-dlLo, dlLo, dlHi, t0Hi-t0Lo, t0Lo, t0Hi);
      ((TH1*) oc_ch->o[shName])->SetOption("NOIMG");
      ((TH1*) oc_ch->o[shName])->SetTitle("; Dead Layer, #mug/cm^{2}; t_{0}, ns;");
      ((TH1*) oc_ch->o[shName])->GetListOfFunctions()->Add(grT0VsDL, "p");

      // If this is a new directory then we need to add it to the list
      if ( isubdir == oc.d.end()) {
         oc.d[sDirName] = oc_ch;
      }
   }

   shName = "hT0VsChannel_" + sPolId + "_" + sBeamE;
   hist= new TH1F(shName.c_str(), shName.c_str(), N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   hist->SetOption("GRIDX");
   hist->SetTitle("; Channel; Mean t_{0}, ns;");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   shName = "hDLVsChannel_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   hist->SetOption("GRIDX");
   hist->SetTitle("; Channel; Mean Dead Layer, #mug/cm^{2};");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   // t0 vs DL
   TGraphErrors* grT0VsDLMean = new TGraphErrors();
   grT0VsDLMean->SetName("grT0VsDLMean");
   styleMarker.Copy(*grT0VsDLMean);

   shName = "hT0VsDLMean_" + sPolId + "_" + sBeamE;
   hist = new TH2C(shName.c_str(), shName.c_str(), dlHi-dlLo, dlLo, dlHi, t0Hi-t0Lo, t0Lo, t0Hi);
   hist->SetTitle("; Dead Layer, #mug/cm^{2}; Mean t_{0}, ns;");
   hist->SetOption("DUMMY");
   hist->GetListOfFunctions()->Add(grT0VsDLMean, "p");
   styleMarker.Copy(*hist);
   oc.o[shName] = hist;


   // Disabled channels per fill
   shName = "hDisabledChVsFill_" + sPolId + "_" + sBeamE;
   hist = new TH2C(shName.c_str(), shName.c_str(), 1, 0, 1, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   hist->SetTitle("; Fill Id; Channel; ");
   hist->SetOption("col DUMMY");
   oc.o[shName] = hist;


   shName = "hPolarSlopeVsFill_" + sPolId + "_" + sBeamE;
   hist = utils::ConstructTH1CWithTGraphErrors(shName.c_str(), "; Fill Id; Polar. Decay #frac{dP}{dt}, %/hour;", (TStyle*) &styleMarker);
   oc.o[shName] = hist;


   // Combined asymmetry histogram
   shName = "hKinEnergyAChAsym_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 25, 22.5, 1172.2);
   hist->SetOption("E1");
   hist->SetBit(TH1::kIsAverage);
   hist->SetTitle("; Kinematic Energy, keV; Asymmetry;");
   styleMarker.Copy(*hist);
   oc.o[shName] = hist;

   Double_t xbins[8] = {-20, -5, -3, -1, +1, +3, +5, +20};

   shName = "hLongiChAsym_" + sPolId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 7, xbins);
   hist->SetOption("E1");
   hist->SetBit(TH1::kIsAverage);
   hist->SetTitle("; Longi. Time Diff, ns; Asymmetry;");
   styleMarker.Copy(*hist);
   oc.o[shName] = hist;


   shName = "hNormHJ2PCVsTargetId_" + sPolId + "_" + sBeamE;
   map<string, TStyle*> sfx2styles;
   sfx2styles["_H"] = (TStyle*) new TAttMarker(RunConfig::AsMarker(kTARGET_H, polId));
   sfx2styles["_V"] = (TStyle*) new TAttMarker(RunConfig::AsMarker(kTARGET_V, polId));
   hist = utils::ConstructTH1CWithTGraphErrorsMap(shName.c_str(), "; Target Id; H-jet/p-Carbon Pol. Ratio;", sfx2styles, 6, 0.5, 6.5, 10, 0, 1.5);
   oc.o[shName] = hist;
}


/** */
void MAsymRunHists::BookHistsByRing(DrawObjContainer &oc, ERingId ringId, EBeamEnergy beamE)
{
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
   hist = utils::ConstructTH1CWithTGraphErrors(shName.c_str(), "; Fill Id; Polarization (H-jet), %;", (TStyle*) &styleMarker);
   oc.o[shName] = hist;

   shName = "hAsymHJVsFill_" + sRingId + "_" + sBeamE;
   hist = utils::ConstructTH1CWithTGraphErrors(shName.c_str(), "; Fill Id; Asymmetry (H-jet);", (TStyle*) &styleMarker);
   oc.o[shName] = hist;

   shName = "hPolarVsFill_" + sRingId + "_"  + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle("; Fill Id; Beam Polarization, %;");
   hist->SetOption("E1 GRIDX GRIDY");
   styleMarker.Copy(*hist);
   oc.o[shName] = hist;

   shName = "hPolarRatioVsFill_" + sRingId + "_"  + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0.5, 1.5);
   hist->SetTitle("; Fill Id; Polarization Ratio;");
   hist->SetOption("E1");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   shName = "hPolarRatioVsFill_HJOnly_" + sRingId + "_"  + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0.5, 1.5);
   hist->SetTitle("; Fill Id; Polarization Ratio;");
   hist->SetOption("E1");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   shName = "hPolarUDRatioVsFill_" + sRingId + "_"  + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0.5, 1.5);
   hist->SetTitle("; Fill Id; Polarization Rel. Diff., %;");
   hist->SetOption("E1");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   shName = "hPolarRelDiff_" + sRingId + "_"  + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 140, 0, 70);
   hist->SetTitle("; Polarization Rel. Diff., %; ;");
   hist->SetLineWidth(2);
   hist->SetLineColor(color);
   hist->SetOption("hist");
   o[shName] = hist;

   shName = "hPolarRelDiffCumul_" + sRingId + "_"  + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 140, 0, 70);
   hist->SetTitle("; Cumulative Pol. Rel. Diff., %; ;");
   hist->SetLineWidth(2);
   hist->SetLineColor(color);
   hist->SetOption("hist");
   oc.o[shName] = hist;

   shName = "hPolarSlopeVsFill_" + sRingId + "_" + sBeamE;
   hist = utils::ConstructTH1CWithTGraphErrors(shName.c_str(), "; Fill Id; Polar. Decay #frac{dP}{dt}, %/hour;", (TStyle*) &styleMarker);
   oc.o[shName] = hist;

   // By target orientation
   shName = "hProfPMaxVsFill_H_" + sRingId + "_"  + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle("; Fill Id; Prof. P_max;");
   hist->SetOption("E1");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   shName = "hProfPolarVsFill_H_" + sRingId + "_"  + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->SetTitle("; Fill Id; Prof. P;");
   hist->SetOption("E1");
   styleMarker.Copy(*hist); oc.o[shName] = hist;

   // decay
   shName = "hIntensDecayVsFill_" + sRingId + "_" + sBeamE;
   hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
   hist->GetYaxis()->SetRangeUser(0, 100);
   hist->SetTitle("; Fill Id; Lifetime, hour;");
   hist->SetOption("E1");
   styleMarker.Copy(*hist);
   oc.o[shName] = hist;
}


/** */
void MAsymRunHists::Fill(const EventConfig &rc)
{
   char     hName[256];
   string   shName;
   TH1     *hist;

   Double_t runId            = rc.fMeasInfo->RUNID;
   UInt_t   beamEnergy       = rc.fMeasInfo->GetBeamEnergy();
   Short_t  polId            = rc.fMeasInfo->fPolId;
   time_t   measStartTime    = rc.fMeasInfo->fStartTime;
   //Float_t  profileRatio     = rc.fAnaMeasResult->fProfilePolarR.first;
   //Float_t  profileRatioErr  = rc.fAnaMeasResult->fProfilePolarR.second;
   Float_t  max_rate         = rc.fAnaMeasResult->max_rate;

   ChannelCalib chCalib      = rc.fCalibrator->GetMeanChannel();

   Float_t t0    = chCalib.fT0Coef;
   Float_t t0Err = chCalib.fT0CoefErr;
   Float_t dl    = chCalib.fDLWidth;
   Float_t dlErr = chCalib.fDLWidthErr;

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

   if (rc.fMeasInfo->GetTargetOrient() == kTARGET_H) {
      graph = (TGraph*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grHTargetVsMeas");
   }

   if (rc.fMeasInfo->GetTargetOrient() == kTARGET_V) {
      graph = (TGraph*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grVTargetVsMeas");
   }

   graph->SetPoint(graph->GetN(), runId, rc.fMeasInfo->GetTargetId());

   // Get sine modulation fit results
   TFitResultPtr fitResAsymPhi  = rc.fAnaMeasResult->fFitResAsymPhi;
   TFitResultPtr fitResPolarPhi = rc.fAnaMeasResult->fFitResPolarPhi;

   // Expect valid results
   //assert(fitResAsymPhi.Get());
   //assert(fitResPolarPhi.Get());

   Float_t spinAngle       = 0;
   Float_t spinAngleErr    = 0;
   Float_t polarization    = 0;
   Float_t polarizationErr = 0;

   if ( fitResAsymPhi.Get() && fitResPolarPhi.Get() ) {
      spinAngle    = fitResAsymPhi->Value(1) / TMath::Pi() * 180;
      spinAngleErr = fitResAsymPhi->FitResult::Error(1) / TMath::Pi() * 180;

      polarization    = fitResPolarPhi->Value(0) * 100.;
      polarizationErr = fitResPolarPhi->FitResult::Error(0) * 100.;
   }


   UInt_t nPoints = 0;

   // Spin Angle
   shName = "hSpinAngleVsMeas_" + sPolId + "_" + sBeamE;
   graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[shName])->GetListOfFunctions()->FindObject("grSpinAngleVsMeas");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runId,  spinAngle);
   graphErrs->SetPointError(nPoints, 0, spinAngleErr);

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
   graphErrs->SetPoint(nPoints, measStartTime, polarization);
   graphErrs->SetPointError(nPoints, 0, polarizationErr);

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
   graphErrs->SetPoint(nPoints, measStartTime, t0);
   graphErrs->SetPointError(nPoints, 0, t0Err);

   // Dead layer
   sprintf(hName, "hDLVsMeas_%s_%s", sPolId.c_str(), sBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grDLVsMeas");
   utils::AppendToGraph(graphErrs, runId, dl, 0, dlErr);

   // DL vs time
   sprintf(hName, "hDLVsTime_%s_%s", sPolId.c_str(), sBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grDLVsTime");
   utils::AppendToGraph(graphErrs, measStartTime, dl, 0, dlErr);

   // Banana fit params
   Float_t bananaChi2Ndf = chCalib.fBananaChi2Ndf;

   sprintf(hName, "hBananaChi2NdfVsMeas_%s_%s", sPolId.c_str(), sBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grBananaChi2Ndf");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runId, bananaChi2Ndf);

   // per channel hists
   //ChannelSetIter iCh = gMeasInfo->fSiliconChannels.begin();
   //for (; iCh!=gMeasInfo->fSiliconChannels.end(); ++iCh)
   for (UShort_t iCh=1; iCh<=N_SILICON_CHANNELS; iCh++)
   {
      string sChId("  ");
      sprintf(&sChId[0], "%02d", iCh);

      // Find corresponding sub directory
      DrawObjContainer *oc_ch = oc_pol->d.find("channel" + sChId)->second;

      // T0
      t0    = rc.fCalibrator->fChannelCalibs[iCh].fT0Coef;
      t0Err = rc.fCalibrator->fChannelCalibs[iCh].fT0CoefErr;

      // XXX also should be removed when real QA is available
      if (isnan(t0) || isinf(t0) || isnan(t0Err) || isinf(t0Err)) continue;

      shName = "hT0VsMeas_" + sPolId + "_" + sBeamE + "_" + sChId;
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


   // Disabled channels per fill
   shName = "hDisabledChVsFill_" + sPolId + "_" + sBeamE;
   hist = (TH1*) oc_pol->o[shName];

   UInt_t fillId = (UInt_t) runId;

   ChannelSet &disabledChs = rc.fMeasInfo->fDisabledChannels;
   ChannelSetConstIter iDisCh = disabledChs.begin();

   for ( ; iDisCh != disabledChs.end(); ++iDisCh)
   {
      hist->SetBinContent(fillId - fMinFill + 1, *iDisCh, 1);
   }
}


/** */
void MAsymRunHists::Fill(EventConfig &rc, DrawObjContainer &oc)
{
   Short_t  polId            = rc.fMeasInfo->fPolId;
   UInt_t   beamEnergy       = rc.fMeasInfo->GetBeamEnergy();

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

}


/** */
void MAsymRunHists::Print(const Option_t* opt) const
{
   DrawObjContainer::Print();
}


/** */
void MAsymRunHists::PostFill(AnaGlobResult &agr)
{
   // Loop over all fill results
   AnaFillResultMapIter iafr = agr.fAnaFillResults.begin();
   for ( ; iafr != agr.fAnaFillResults.end(); ++iafr)
   {
      BeamEnergySetIter iBE = gRunConfig.fBeamEnergies.begin();
      for ( ; iBE != gRunConfig.fBeamEnergies.end(); ++iBE)
      {
         PolarimeterIdSetIter iPolId = gRunConfig.fPolarimeters.begin();
         for (; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
         {
            PostFillByPolarimeter(agr, iafr, *iPolId, *iBE);
         }

         RingIdSetIter iRingId = gRunConfig.fRings.begin();
         for ( ; iRingId != gRunConfig.fRings.end(); ++iRingId)
         {
            PostFillByRing(agr, iafr, *iRingId, *iBE);
         }
      }
   }

   TGraph *graph;
   TH1    *hist;

   BeamEnergySetIter iBE = gRunConfig.fBeamEnergies.begin();
   for ( ; iBE != gRunConfig.fBeamEnergies.end(); ++iBE)
   {
      string sBeamE = RunConfig::AsString(*iBE);

      PolarimeterIdSetIter iPolId = gRunConfig.fPolarimeters.begin();
      for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
      {
         EPolarimeterId polId     = *iPolId;
         string sPolId            = RunConfig::AsString(polId);
         DrawObjContainer *oc_pol = d.find(sPolId)->second;

         TH1F* hPolarHJVsFill_  = (TH1F*) oc_pol->o["hPolarHJVsFill_" + sPolId + "_" + sBeamE];
         hPolarHJVsFill_->Print();
         hPolarHJVsFill_->Fit("pol0");

         TH1F* hPolarPCVsFill_ = (TH1F*) oc_pol->o["hPolarPCVsFill_" + sPolId + "_" + sBeamE];
         hPolarPCVsFill_->Print();
         hPolarPCVsFill_->Fit("pol0");

         TH1F* hPolarPCScaledVsFill_HJOnly_ = (TH1F*) oc_pol->o["hPolarPCScaledVsFill_HJOnly_" + sPolId + "_" + sBeamE];
         hPolarPCScaledVsFill_HJOnly_->Print();
         hPolarPCScaledVsFill_HJOnly_->Fit("pol0");

         // Ratio of Hjet over pC
         TH1F* hNormJCVsFill_ = (TH1F*) oc_pol->o["hNormJCVsFill_" + sPolId + "_" + sBeamE];
         hNormJCVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
         hNormJCVsFill_->Divide(hPolarHJVsFill_, hPolarPCVsFill_);
         hNormJCVsFill_->Print();
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

         TH1F* hPolarHJPCRatioVsFill_ = (TH1F*) oc_pol->o["hPolarHJPCRatioVsFill_" + sPolId + "_" + sBeamE];
         hPolarHJPCRatioVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
         hPolarHJPCRatioVsFill_->Divide(hPolarHJVsFill_, hPolarPCScaledVsFill_HJOnly_); // same as hPolarHJPCRatioSystVsFill_
         hPolarHJPCRatioVsFill_->Print();
         hPolarHJPCRatioVsFill_->Fit("pol0");

         TH1F* hPolarHJPCRatioSystVsFill_ = (TH1F*) oc_pol->o["hPolarHJPCRatioSystVsFill_" + sPolId + "_" + sBeamE];
         hPolarHJPCRatioSystVsFill_->Print();
         hPolarHJPCRatioSystVsFill_->Fit("pol0");

         TH1F* hPolarPCScaledVsFill_ = (TH1F*) oc_pol->o["hPolarPCScaledVsFill_" + sPolId + "_" + sBeamE];
         hPolarPCScaledVsFill_->Print();
         hPolarPCScaledVsFill_->Fit("pol0");

         TH1F* hProfPolarVsFill_ = (TH1F*) oc_pol->o["hProfPolarVsFill_" + sPolId + "_" + sBeamE];
         hProfPolarVsFill_->Print();
         hProfPolarVsFill_->Fit("pol0");

         TH1F* hProfPolarRatioSystVsFill_ = (TH1F*) oc_pol->o["hProfPolarRatioSystVsFill_" + sPolId + "_" + sBeamE];
         hProfPolarRatioSystVsFill_->Print();
         hProfPolarRatioSystVsFill_->Fit("pol0");

         TH1* hProfPolarRatioVsFill_ = (TH1*) oc_pol->o["hProfPolarRatioVsFill_" + sPolId + "_" + sBeamE];
         utils::Divide(hProfPolarVsFill_, hPolarPCScaledVsFill_, 1, hProfPolarRatioVsFill_);
         hProfPolarRatioVsFill_->Print();
         hProfPolarRatioVsFill_->Fit("pol0");
         //hProfPolarRatioVsFill_->GetYaxis()->UnZoom();

         hist = (TH1*) oc_pol->o["hPolarSlopeVsFill_" + sPolId + "_" + sBeamE];
         utils::SetXAxisIntBinsLabels(hist, agr.GetMinFill(), agr.GetMaxFill());
         utils::UpdateLimitsFromGraphs(hist, 2);
         graph = utils::ExtractTGraph(*hist);
         graph->Fit("pol0");

         ERingId ringId  = RunConfig::GetRingId(polId);
         string  sRingId = RunConfig::AsString(ringId);
         DrawObjContainer *oc_ring = d.find(sRingId)->second;

         hist = (TH1*) oc_ring->o["hPolarSlopeVsFill_" + sRingId + "_" + sBeamE];
         utils::SetXAxisIntBinsLabels(hist, agr.GetMinFill(), agr.GetMaxFill());
         TGraph* graphMerged = utils::ExtractTGraph(*hist);
         utils::MergeGraphs(graphMerged, graph);
         graphMerged->Fit("pol0");
         utils::UpdateLimitsFromGraphs(hist, 2);

         TH1* hRSlopeVsFill_ = (TH1F*) oc_pol->o["hRSlopeVsFill_" + sPolId + "_" + sBeamE];
         hRSlopeVsFill_->GetXaxis()->SetLimits(fMinFill, fMaxFill);

         TGraph* grRSlopeVsFill_H_ = (TGraph*) hRSlopeVsFill_->GetListOfFunctions()->FindObject("grRSlopeVsFill_H_");
         grRSlopeVsFill_H_->Fit("pol0");
         TGraph* grRSlopeVsFill_V_ = (TGraph*) hRSlopeVsFill_->GetListOfFunctions()->FindObject("grRSlopeVsFill_V_");
         grRSlopeVsFill_V_->Fit("pol0");

         TH1* hRSlopeVsPolarSlope_ = (TH1F*) oc_pol->o["hRSlopeVsPolarSlope_" + sPolId + "_" + sBeamE];
         utils::UpdateLimitsFromGraphs(hRSlopeVsPolarSlope_, 1);
         utils::UpdateLimitsFromGraphs(hRSlopeVsPolarSlope_, 2);

         TGraphErrors* grRSlopeVsPolarSlope_H_ = (TGraphErrors*) hRSlopeVsPolarSlope_->GetListOfFunctions()->FindObject("grRSlopeVsPolarSlope_H_");
         grRSlopeVsPolarSlope_H_->Fit("pol1", "EX0");
         TGraphErrors* grRSlopeVsPolarSlope_V_ = (TGraphErrors*) hRSlopeVsPolarSlope_->GetListOfFunctions()->FindObject("grRSlopeVsPolarSlope_V_");
         grRSlopeVsPolarSlope_V_->Fit("pol1", "EX0");

         TargetOrientSetIter iTgtOrient = gRunConfig.fTargetOrients.begin();
         for ( ; iTgtOrient != gRunConfig.fTargetOrients.end(); ++iTgtOrient)
         {
            ETargetOrient tgtOrient  = *iTgtOrient;
            string        sTgtOrient = RunConfig::AsString(tgtOrient);

            for ( UShort_t iTgtId=1; iTgtId<=6; iTgtId++)
            {
               TargetUId targetUId(polId, tgtOrient, iTgtId);
               ValErrPair norm = agr.fNormJetCarbonByTarget2.find(targetUId)->second;

               if ( norm.second < 0) continue;

               Float_t tinyOffset = tgtOrient == kTARGET_H ? -0.1 : +0.1;

               TH1* hist = (TH1*) oc_pol->o["hNormHJ2PCVsTargetId_" + sPolId + "_" + sBeamE];
               utils::AppendToTGraph(*hist, iTgtId+tinyOffset, norm.first, 0, norm.second, "_" + sTgtOrient);
               utils::UpdateLimitsFromGraphs(hist, 2);
            }

            TH1* hist = (TH1*) oc_pol->o["hNormHJ2PCVsTargetId_" + sPolId + "_" + sBeamE];
            utils::ExtractTGraph(*hist, string("_" + sTgtOrient).c_str())->Fit("pol0");
         }
      }


      RingIdSetIter iRingId = gRunConfig.fRings.begin();
      for ( ; iRingId != gRunConfig.fRings.end(); ++iRingId)
      {
         string sRingId  = RunConfig::AsString(*iRingId);
         DrawObjContainer *oc_ring = d.find(sRingId)->second;

         hist = (TH1*) oc_ring->o["hPolarHJVsFill_" + sRingId + "_" + sBeamE];
         utils::SetXAxisIntBinsLabels(hist, agr.GetMinFill(), agr.GetMaxFill());
         utils::UpdateLimitsFromGraphs(hist, 2);
         utils::ExtractTGraph(*hist)->Fit("pol0");

         hist = (TH1*) oc_ring->o["hAsymHJVsFill_" + sRingId + "_" + sBeamE];
         utils::SetXAxisIntBinsLabels(hist, agr.GetMinFill(), agr.GetMaxFill());
         utils::UpdateLimitsFromGraphs(hist, 2);
         utils::ExtractTGraph(*hist)->Fit("pol0");

         TH1F* hPolarRelDiff_       = (TH1F*) oc_ring->o["hPolarRelDiff_"       + sRingId + "_" + sBeamE];
         TH1F* hPolarRelDiffCumul_  = (TH1F*) oc_ring->o["hPolarRelDiffCumul_"  + sRingId + "_" + sBeamE];
         TH1F* hPolarUDRatioVsFill_ = (TH1F*) oc_ring->o["hPolarUDRatioVsFill_" + sRingId + "_" + sBeamE];

         utils::ConvertToProfile(hPolarUDRatioVsFill_, hPolarRelDiff_, kFALSE);
         utils::ConvertToProfile(hPolarUDRatioVsFill_, hPolarRelDiffCumul_, kFALSE);
         utils::ConvertToCumulative(hPolarRelDiffCumul_);

         hPolarUDRatioVsFill_->Print();
         hPolarUDRatioVsFill_->Fit("pol0");

         TH1F* hPolarU = 0, *hPolarU_HJOnly = 0;
         TH1F* hPolarD = 0, *hPolarD_HJOnly = 0;

         if (*iRingId == kBLUE_RING) {
            hPolarU        = (TH1F*) d["B1U"]->o["hPolarPCScaledVsFill_B1U_" + sBeamE];
            hPolarD        = (TH1F*) d["B2D"]->o["hPolarPCScaledVsFill_B2D_" + sBeamE];
            hPolarU_HJOnly = (TH1F*) d["B1U"]->o["hPolarPCScaledVsFill_HJOnly_B1U_" + sBeamE];
            hPolarD_HJOnly = (TH1F*) d["B2D"]->o["hPolarPCScaledVsFill_HJOnly_B2D_" + sBeamE];
         } else if (*iRingId == kYELLOW_RING) {
            hPolarU        = (TH1F*) d["Y2U"]->o["hPolarPCScaledVsFill_Y2U_" + sBeamE];
            hPolarD        = (TH1F*) d["Y1D"]->o["hPolarPCScaledVsFill_Y1D_" + sBeamE];
            hPolarU_HJOnly = (TH1F*) d["Y2U"]->o["hPolarPCScaledVsFill_HJOnly_Y2U_" + sBeamE];
            hPolarD_HJOnly = (TH1F*) d["Y1D"]->o["hPolarPCScaledVsFill_HJOnly_Y1D_" + sBeamE];
         }

         TH1F* hPolarRatioVsFill_ = (TH1F*) oc_ring->o["hPolarRatioVsFill_" + sRingId + "_"  + sBeamE];
         hPolarRatioVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

         // 100% correlation between U and D is a more conservative assumption ...ah, not really a good idea
         //utils::Divide(hPolarU, hPolarD, 1, hPolarRatioVsFill_);
         utils::Divide(hPolarU, hPolarD, 0, hPolarRatioVsFill_);

         hPolarRatioVsFill_->Print();
         utils::UpdateLimits((TH1*) hPolarRatioVsFill_);
         hPolarRatioVsFill_->Fit("pol0");

         TH1F* hPolarRatioVsFill_HJOnly_ = (TH1F*) oc_ring->o["hPolarRatioVsFill_HJOnly_" + sRingId + "_"  + sBeamE];
         hPolarRatioVsFill_HJOnly_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

         // 100% correlation between U and D is a more conservative assumption
         //utils::Divide(hPolarU_HJOnly, hPolarD_HJOnly, 1, hPolarRatioVsFill_HJOnly_);
         utils::Divide(hPolarU_HJOnly, hPolarD_HJOnly, 0, hPolarRatioVsFill_HJOnly_);

         hPolarRatioVsFill_HJOnly_->Print();
         hPolarRatioVsFill_HJOnly_->Fit("pol0");

         TH1* hIntensDecayVsFill_ = (TH1*) oc_ring->o["hIntensDecayVsFill_" + sRingId + "_" + sBeamE];
         hIntensDecayVsFill_->Fit("pol0");

         hist = (TH1*) oc_ring->o["hPolarSlopeVsFill_" + sRingId + "_" + sBeamE];
         graph = utils::ExtractTGraph(*hist);

         hist = (TH1*) o["hPolarSlopeVsFill_" + sBeamE];
         utils::SetXAxisIntBinsLabels(hist, agr.GetMinFill(), agr.GetMaxFill());
         hist->GetListOfFunctions()->Add(graph, "p");
         utils::UpdateLimitsFromGraphs(hist, 2);
      }
   }

   //TF1 *systRatioFitFunc = new TF1("systRatioFitFunc", new SystRatioFitFunctor(), fMinFill, fMaxFill, 1, "SystRatioFitFunctor");
   //utils::SystRatioFitFunctor *srff = new utils::SystRatioFitFunctor(*hRatio);
   //TF1 *systRatioFitFunc = new TF1("systRatioFitFunc", srff, -10, 10, 0, "SystRatioFitFunctor");
}


/** */
void MAsymRunHists::PostFill()
{
   string  shName;
   TH1    *hist;

   PolarimeterIdSetIter iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      string sPolId = RunConfig::AsString(*iPolId);

      // Find corresponding sub directory
      DrawObjContainer *oc_pol = d.find(sPolId)->second;

      // Adjust axis range
      Double_t xmin,   ymin,   xmax,   ymax,   xdelta, ydelta;
      Double_t xminDL, yminDL, xmaxDL, ymaxDL;

      BeamEnergySetIter iBE = gRunConfig.fBeamEnergies.begin();

      for ( ; iBE != gRunConfig.fBeamEnergies.end(); ++iBE) {

         string sBeamE = RunConfig::AsString(*iBE);

         hist  = (TH1*) oc_pol->o["hT0VsMeas_" + sPolId + "_" + sBeamE];
         TGraphErrors *graphErrs = (TGraphErrors*) hist->GetListOfFunctions()->FindObject("grT0VsMeas");
         graphErrs->ComputeRange(xmin, ymin, xmax, ymax);
         ydelta = (ymax - ymin) <= 0 ? 0.1 : (ymax - ymin)*0.1;
         hist->GetYaxis()->SetLimits(ymin-ydelta, ymax+ydelta);

         //sprintf(hName, "hT0VsFill_%s_%s", sPolId.c_str(), sBeamE.c_str());
         //graphErrs = (TGraphErrors*) ((TH1*) oc_pol->o[hName])->GetListOfFunctions()->FindObject("grT0VsMeas");
         //((TH1*) oc_pol->o[hName])->GetYaxis()->SetLimits(ymin-ydelta, ymax+ydelta);
         //((TH1*) oc_pol->o[hName])->GetYaxis()->SetRangeUser(ymin-ydelta, ymax+ydelta);

         hist  = (TH1*) oc_pol->o["hT0VsTime_" + sPolId + "_" + sBeamE];
         graphErrs = (TGraphErrors*) hist->GetListOfFunctions()->FindObject("grT0VsTime");
         graphErrs->ComputeRange(xmin, ymin, xmax, ymax);
         Double_t ymean = graphErrs->GetMean(2);
         //xdelta = fabs(xmax - xmin)*0.1;
         //ydelta = fabs(ymax - ymin)*0.1;
         hist->GetXaxis()->SetLimits(xmin, xmax);
         //hist->GetYaxis()->SetLimits(ymin - ydelta, ymax + ydelta);
         hist->GetYaxis()->SetLimits(ymean - 5, ymean + 5);

         hist  = (TH1*) oc_pol->o["hDLVsTime_" + sPolId + "_" + sBeamE];
         graphErrs = (TGraphErrors*) hist->GetListOfFunctions()->FindObject("grDLVsTime");
         utils::UpdateLimitsFromGraphs(hist, 2);
         //graphErrs->ComputeRange(xmin, ymin, xmax, ymax);
         //ymean = graphErrs->GetMean(2);
         ////xdelta = fabs(xmax - xmin)*0.1;
         //ydelta = (ymax - ymin) <= 0 ? 0.1 : (ymax - ymin)*0.1;
         ////hist->GetXaxis()->SetLimits(xmin, xmax);
         ////hist->GetYaxis()->SetLimits(ymin - ydelta, ymax + ydelta);
         //hist->GetYaxis()->SetLimits(ymean - 20, ymean + 20);

         TGraphErrors *graph;

         // Spin Angle
         hist  = (TH1*) oc_pol->o["hSpinAngleVsMeas_" + sPolId + "_" + sBeamE];
         graph = (TGraphErrors*) hist->GetListOfFunctions()->FindObject("grSpinAngleVsMeas");
         graph->Fit("pol0");

         // Polarization
         hist  = (TH1*) oc_pol->o["hPolarVsFill_" + sPolId + "_" + sBeamE];
         graph = (TGraphErrors*) hist->GetListOfFunctions()->FindObject("grPolarVsMeas");
         hist->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
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

         // Profiles R
         hist  = (TH1*) oc_pol->o["hRVsFill_" + sPolId + "_" + sBeamE];
         graph = (TGraphErrors*) hist->GetListOfFunctions()->FindObject("grRVsMeas");
         graph->Fit("pol0");

         // H target = vert profile
         hist  = (TH1*) oc_pol->o["hRVsFill_H_" + sPolId + "_" + sBeamE];
         graph = (TGraphErrors*) hist->GetListOfFunctions()->FindObject("grRVsMeasH");
         graph->Fit("pol0");

         // V target = horiz profile
         hist  = (TH1*) oc_pol->o["hRVsFill_V_" + sPolId + "_" + sBeamE];
         graph = (TGraphErrors*) hist->GetListOfFunctions()->FindObject("grRVsMeasV");
         graph->Fit("pol0");

         // t0
         hist  = (TH1*) oc_pol->o["hT0VsFill_" + sPolId + "_" + sBeamE];
         graph = (TGraphErrors*) hist->GetListOfFunctions()->FindObject("grT0VsMeas");
         hist->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
         //utils::RemoveOutliers(graph, 2, 3);
         utils::BinGraph(graph, hist);
         hist->GetListOfFunctions()->Remove(graph);
         hist->Fit("pol0");

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

         //ChannelSetIter iCh = gMeasInfo->fSiliconChannels.begin();

         //for (; iCh!=gMeasInfo->fSiliconChannels.end(); ++iCh)
         for (UShort_t iCh=1; iCh<=N_SILICON_CHANNELS; iCh++)
         {
            string sChId("  ");
            sprintf(&sChId[0], "%02d", iCh);

            // Find corresponding sub directory
            DrawObjContainer *oc_ch = oc_pol->d.find("channel" + sChId)->second;

            hist      = (TH1*) oc_ch->o["hT0VsMeas_" + sPolId + "_" + sBeamE + "_" + sChId];
            graphErrs = (TGraphErrors*) hist->GetListOfFunctions()->FindObject("grT0VsMeas");
            graphErrs->ComputeRange(xmin, ymin, xmax, ymax);
            ydelta    = (ymax - ymin) <= 0 ? 0.1 : (ymax - ymin)*0.1;
            hist->GetYaxis()->SetLimits(ymin-ydelta, ymax+ydelta);

            hist      = (TH1*) oc_ch->o["hDLVsMeas_" + sPolId + "_" + sBeamE + "_" + sChId];
            graphErrs = (TGraphErrors*) hist->GetListOfFunctions()->FindObject("grDLVsMeas");
            graphErrs->ComputeRange(xminDL, yminDL, xmaxDL, ymaxDL);
            Double_t ydeltaDL  = (ymaxDL - yminDL)*0.1;

            // T0
            TH1* hT0 = (TH1*) oc_ch->o["hT0_" + sPolId + "_" + sBeamE + "_" + sChId];
            //utils::RemoveOutliers(hT0);

            hT0VsChannel_->SetBinContent(iCh, hT0->GetMean());
            hT0VsChannel_->SetBinError(iCh,   hT0->GetRMS());
            //hT0VsChannel_->SetBinError(*iCh, fitres->Parameter(2));

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
         ydelta = (ymax - ymin) <= 0 ? 0.1 : (ymax - ymin)*0.1;
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
      xdelta = (xmax - xmin) <= 0 ? 0.1 : (xmax - xmin)*0.1;
      ydelta = (ymax - ymin) <= 0 ? 0.1 : (ymax - ymin)*0.1;
      hT0VsDLDiff_->GetXaxis()->SetLimits(xmin-xdelta, xmax+xdelta);
      hT0VsDLDiff_->GetYaxis()->SetLimits(ymin-ydelta, ymax+ydelta);

      // Ratio of inj to first meas
      TH1F* hPolarFirstMeasRatioVsFill_ = (TH1F*) oc_pol->o["hPolarFirstMeasRatioVsFill_" + sPolId];
      TH1F* hPolarInj                   = (TH1F*) oc_pol->o["hPolarVsFill_" + sPolId + "_024"];
      //TH1F* hPolarFst                   = (TH1F*) oc_pol->o["hPolarFirstMeasVsFill_" + sPolId + "_100"];
      //TH1F* hPolarFst                   = (TH1F*) oc_pol->o["hPolarFirstMeasVsFill_" + sPolId + "_250"];
      TH1F* hPolarFst                   = (TH1F*) oc_pol->o["hPolarFirstMeasVsFill_" + sPolId + "_255"];

      hPolarFirstMeasRatioVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
      hPolarFirstMeasRatioVsFill_->Divide(hPolarFst, hPolarInj);
      hPolarFirstMeasRatioVsFill_->Fit("pol0");
   }

   TH1F* hPolarVsFill_B1U_024        = (TH1F*) d["B1U"]->o["hPolarVsFill_B1U_024"];
   TH1F* hPolarVsFill_B2D_024        = (TH1F*) d["B2D"]->o["hPolarVsFill_B2D_024"];
   TH1F* hPolarUDRatioVsFill_BLU_024 = (TH1F*) d["BLU"]->o["hPolarUDRatioVsFill_BLU_024"];
   hPolarUDRatioVsFill_BLU_024->Divide(hPolarVsFill_B1U_024, hPolarVsFill_B2D_024);
   hPolarUDRatioVsFill_BLU_024->Fit("pol0");

   TH1F* hPolarVsFill_Y2U_024        = (TH1F*) d["Y2U"]->o["hPolarVsFill_Y2U_024"];
   TH1F* hPolarVsFill_Y1D_024        = (TH1F*) d["Y1D"]->o["hPolarVsFill_Y1D_024"];
   TH1F* hPolarUDRatioVsFill_YEL_024 = (TH1F*) d["YEL"]->o["hPolarUDRatioVsFill_YEL_024"];
   hPolarUDRatioVsFill_YEL_024->Divide(hPolarVsFill_Y2U_024, hPolarVsFill_Y1D_024);
   hPolarUDRatioVsFill_YEL_024->Fit("pol0");
}


/** */
void MAsymRunHists::PostFillByPolarimeter(AnaGlobResult &agr, AnaFillResultMapIter iafr, EPolarimeterId polId, EBeamEnergy beamE)
{
   UInt_t fillId     = iafr->first;
   AnaFillResult afr = iafr->second;

   if (afr.fPCPolars.find(polId) == afr.fPCPolars.end()) return;
   //if (afr.fProfPCPolars.find(polId) == afr.fProfPCPolars.end()) continue;

   string sPolId = RunConfig::AsString(polId);
   string sBeamE = RunConfig::AsString(beamE);

   // Find corresponding sub directory
   DrawObjContainer *oc_pol = d.find(sPolId)->second;

   TH1F* hPolarHJVsFill_ = (TH1F*) oc_pol->o["hPolarHJVsFill_" + sPolId + "_" + sBeamE];
   hPolarHJVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

   TH1F* hPolarPCVsFill_ = (TH1F*) oc_pol->o["hPolarPCVsFill_" + sPolId + "_" + sBeamE];
   hPolarPCVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

   TH1F* hPolarPCScaledVsFill_ = (TH1F*) oc_pol->o["hPolarPCScaledVsFill_" + sPolId + "_" + sBeamE];
   hPolarPCScaledVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

   TH1F* hPolarPCScaledVsFill_HJOnly_ = (TH1F*) oc_pol->o["hPolarPCScaledVsFill_HJOnly_" + sPolId + "_" + sBeamE];
   hPolarPCScaledVsFill_HJOnly_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

   TH1F* hProfPolarVsFill_ = (TH1F*) oc_pol->o["hProfPolarVsFill_" + sPolId + "_" + sBeamE];
   hProfPolarVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

   TH1F* hPolarHJPCRatioSystVsFill_ = (TH1F*) oc_pol->o["hPolarHJPCRatioSystVsFill_" + sPolId + "_" + sBeamE];
   hPolarHJPCRatioSystVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

   TH1F* hProfPolarRatioSystVsFill_ = (TH1F*) oc_pol->o["hProfPolarRatioSystVsFill_" + sPolId + "_" + sBeamE];
   hProfPolarRatioSystVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

   Int_t ib = hPolarPCScaledVsFill_->FindBin(fillId);

   // Consider P and P_prof by polarimeter
   ValErrPair polarHJ         = afr.GetHJPolar(polId);
   ValErrPair polarPC         = afr.GetPCPolar(polId);                       // unnormalized value
   //ValErrPair polarPCNorm     = afr.GetPCPolar(polId, &agr.fNormJetCarbon);  // ratio of average
   //ValErrPair polarProfNorm   = afr.GetPCPolar(polId, &agr.fNormProfPolar);
   ValErrPair polarPCNorm     = afr.GetPCPolar(polId, &agr.fNormJetCarbon2); // average of ratio
   //ValErrPair polarProfNorm   = afr.GetPCPolar(polId, &agr.fNormProfPolar2);

   //ValErrPair normJC2 = agr.GetNormJetCarbon(polId);
   //cout << endl;
   //cout << MapAsPhpArray<ERingId, ValErrPair>(afr.fHjetPolars) << endl;
   //printf("%s, %s: %f, %f\n", sPolId.c_str(), sBeamE.c_str(), normJC2.first, normJC2.second);
   //printf("%s, %s: %f, %f, %f, %f, %f, %f\n", sPolId.c_str(), sBeamE.c_str(),
   //       polarHJ.first, polarHJ.second, polarPC.first, polarPC.second,
   //       polarPCNorm.first, polarPCNorm.second);

   if (polarPCNorm.second >= 0) //if (polarPC.second >= 0)
   {
      // Save HJ only if corresponding pC is available
      if (polarHJ.second >= 0) {
         hPolarHJVsFill_->SetBinContent(ib, polarHJ.first*100);
         hPolarHJVsFill_->SetBinError(  ib, polarHJ.second*100);

         hPolarPCScaledVsFill_HJOnly_->SetBinContent(ib, polarPCNorm.first*100);
         hPolarPCScaledVsFill_HJOnly_->SetBinError(  ib, polarPCNorm.second*100);
      }

      hPolarPCVsFill_->SetBinContent(ib, polarPC.first*100);
      hPolarPCVsFill_->SetBinError(  ib, polarPC.second*100);

      hPolarPCScaledVsFill_->SetBinContent(ib, polarPCNorm.first*100);
      hPolarPCScaledVsFill_->SetBinError(  ib, polarPCNorm.second*100);
   }

   ValErrPair profPolar = afr.fPCProfPolars[polId];

   if (profPolar.second >= 0) {
     hProfPolarVsFill_->SetBinContent(ib, profPolar.first*100);
     hProfPolarVsFill_->SetBinError(  ib, profPolar.second*100);
   }

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

   // Decay plots
   ValErrPair pcPolarSlope = afr.GetPCPolarSlope(polId);

   if (pcPolarSlope.second > 0) { // reasonable results only
      TGraph* graph = utils::ExtractTGraph( (TH1&) *oc_pol->o["hPolarSlopeVsFill_" + sPolId + "_" + sBeamE] );
      utils::AppendToGraph(graph, fillId, pcPolarSlope.first*100., 0, pcPolarSlope.second*100.);
   }

   // Polarization profiles and decays
   TH1*    hRVsFill_            = (TH1*) oc_pol->o["hRVsFill_" + sPolId + "_" + sBeamE];
   TGraph* grRVsMeas            = (TGraph*) hRVsFill_->GetListOfFunctions()->FindObject("grRVsMeas");
   TH1*    hRSlopeVsFill_       = (TH1F*) oc_pol->o["hRSlopeVsFill_" + sPolId + "_" + sBeamE];
   TH1*    hRSlopeVsPolarSlope_ = (TH1F*) oc_pol->o["hRSlopeVsPolarSlope_" + sPolId + "_" + sBeamE];

   TargetOrientSetIter iTgtOrient = gRunConfig.fTargetOrients.begin();
   for ( ; iTgtOrient != gRunConfig.fTargetOrients.end(); ++iTgtOrient)
   {
      ETargetOrient tgtOrient  = *iTgtOrient;
      string        sTgtOrient = RunConfig::AsString(tgtOrient);

      TH1*    hRVsFill_O_      = (TH1*) oc_pol->o["hRVsFill_" +sTgtOrient + "_" + sPolId + "_" + sBeamE];
      TGraph* grRVsMeas_O_     = (TGraph*) hRVsFill_O_->GetListOfFunctions()->FindObject(("grRVsMeas" + sTgtOrient).c_str());

      ValErrPair pcProfR = beamE == kINJECTION ? afr.GetPCProfRInj(polId, tgtOrient) : afr.GetPCProfR(polId, tgtOrient);

      if (pcProfR.second >= 0) {
         utils::AppendToGraph(grRVsMeas, fillId, pcProfR.first, 0, pcProfR.second);
         utils::AppendToGraph(grRVsMeas_O_, fillId, pcProfR.first, 0, pcProfR.second);
      }

      // decays...
      ValErrPair pcProfRSlope = afr.GetPCProfRSlope(polId, tgtOrient);

      if (pcProfRSlope.second > 0 && fabs(pcProfRSlope.first) < 1)
      { // require some reasonable number
         string  ssRSlopeVsFill_ = "grRSlopeVsFill_" + sTgtOrient + "_";
         TGraph* grRSlopeVsFill_ = (TGraph*) hRSlopeVsFill_->GetListOfFunctions()->FindObject(ssRSlopeVsFill_.c_str());
         utils::AppendToGraph(grRSlopeVsFill_, fillId, pcProfRSlope.first, 0, pcProfRSlope.second);

         if (pcPolarSlope.second > 0)
         { // require some reasonable number
            string  ssRSlopeVsPolarSlope_ = "grRSlopeVsPolarSlope_" + sTgtOrient + "_";
            TGraph* grRSlopeVsPolarSlope_ = (TGraph*) hRSlopeVsPolarSlope_->GetListOfFunctions()->FindObject(ssRSlopeVsPolarSlope_.c_str());
            utils::AppendToGraph(grRSlopeVsPolarSlope_, pcPolarSlope.first*100., pcProfRSlope.first, pcPolarSlope.second*100., pcProfRSlope.second);
         }
      }
   }
}


/** */
void MAsymRunHists::PostFillByRing(AnaGlobResult &agr, AnaFillResultMapIter iafr, ERingId ringId, EBeamEnergy beamE)
{
   UInt_t fillId     = iafr->first;
   AnaFillResult afr = iafr->second;

   string sRingId = RunConfig::AsString(ringId);
   string sBeamE  = RunConfig::AsString(beamE);

   DrawObjContainer *oc_ring = d.find(sRingId)->second;

   TH1F* hPolarVsFill_ = (TH1F*) oc_ring->o["hPolarVsFill_" + sRingId + "_" + sBeamE];
   hPolarVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

   TH1F* hPolarUDRatioVsFill_ = (TH1F*) oc_ring->o["hPolarUDRatioVsFill_" + sRingId + "_" + sBeamE];
   hPolarUDRatioVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

   TH1F* hProfPMaxVsFill_H_ = (TH1F*) oc_ring->o["hProfPMaxVsFill_H_" + sRingId + "_" + sBeamE];
   hProfPMaxVsFill_H_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

   TH1F* hProfPolarVsFill_H_ = (TH1F*) oc_ring->o["hProfPolarVsFill_H_" + sRingId + "_" + sBeamE];
   hProfPolarVsFill_H_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

   // Fill graphs in histograms by ring
   TH1* hist;

   ValErrPair polarHJ = afr.GetHJPolar(ringId);
   if (polarHJ.second >= 0) {
      hist = (TH1*) oc_ring->o["hPolarHJVsFill_" + sRingId + "_" + sBeamE];
      utils::AppendToTGraph(*hist, fillId, polarHJ.first*100, 0, polarHJ.second*100);
   }

   ValErrPair asymHJ = afr.GetHJAsym(ringId);
   if (asymHJ.second >= 0) {
      hist = (TH1*) oc_ring->o["hAsymHJVsFill_" + sRingId + "_" + sBeamE];
      utils::AppendToTGraph(*hist, fillId, asymHJ.first, 0, asymHJ.second);
   }

   Int_t ib = hPolarVsFill_->FindBin(fillId);

   ValErrPair beamPolar = afr.GetBeamPolar(ringId);
   if (beamPolar.second >= 0) {
      hPolarVsFill_->SetBinContent(ib, beamPolar.first);
      hPolarVsFill_->SetBinError(  ib, beamPolar.second);
   }

   ValErrPair polarUDRatio = afr.GetPolarUDRatio(ringId);
   if (polarUDRatio.second >= 0) { // expect some reasonable number
      hPolarUDRatioVsFill_->SetBinContent(ib, polarUDRatio.first);
      hPolarUDRatioVsFill_->SetBinError(  ib, polarUDRatio.second);
   }

   // Get polarization from profile meas
   ValErrPair vePMax = afr.GetPCProfPMax(ringId, kTARGET_H);
   if (vePMax.second >= 0) {
      hProfPMaxVsFill_H_->SetBinContent(ib, vePMax.first);
      hProfPMaxVsFill_H_->SetBinError(  ib, vePMax.second);
   }

   ValErrPair veP = afr.GetPCProfP(ringId, kTARGET_H);

   if (veP.second >= 0) {
      hProfPolarVsFill_H_->SetBinContent(ib, veP.first);
      hProfPolarVsFill_H_->SetBinError(  ib, veP.second);
   }

   // Slope plots
   ValErrPair intensDecay = afr.GetIntensDecay(ringId);

   if (intensDecay.second >= 0) {
      TH1* hIntensDecayVsFill_ = (TH1F*) oc_ring->o["hIntensDecayVsFill_" + sRingId + "_" + sBeamE];
      hIntensDecayVsFill_->SetBinContent(ib, intensDecay.first);
      hIntensDecayVsFill_->SetBinError(ib, intensDecay.second);
   }
}


/** */
void MAsymRunHists::UpdateLimits()
{
   Info("UpdateLimits", "fMinFill: %f, fMaxFill: %f", fMinFill, fMaxFill);
   Info("UpdateLimits", "fMinTime: %ld, fMaxTime: %ld", fMinTime, fMaxTime);

   char    hName[256];
   string  shName;
   TH1    *hist;

   PolarimeterIdSetIter iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      string sPolId = RunConfig::AsString(*iPolId);

      Info("UpdateLimits", "sPolId = %s", sPolId.c_str());

      // Find corresponding sub directory
      DrawObjContainer *oc_pol = d.find(sPolId)->second;

      Double_t minFill, maxFill, marginFill;
      Double_t minMeas, maxMeas, marginMeas;

      BeamEnergySetIter iBE = gRunConfig.fBeamEnergies.begin();

      for (; iBE != gRunConfig.fBeamEnergies.end(); ++iBE)
      {
         string sBeamE = RunConfig::AsString(*iBE);
         Info("UpdateLimits", "sBeamE = %s", sBeamE.c_str());

         minFill    = utils::GetNonEmptyMinimum(fHStacks["hsPolarVsFill_" + sBeamE], "nostack");
         maxFill    = utils::GetNonEmptyMaximum(fHStacks["hsPolarVsFill_" + sBeamE], "nostack");
         //shName = "hPolarVsFill_" + sPolId + "_" + sBeamE;
         //minFill    = utils::GetNonEmptyMinimum((TH1*) oc_pol->o[shName]);
         //maxFill    = utils::GetNonEmptyMaximum((TH1*) oc_pol->o[shName]);
         marginFill = (maxFill - minFill) * 0.0;

         minMeas    = utils::GetNonEmptyMinimum(fHStacks["hsPolarVsMeas_" + sBeamE], "nostack");
         maxMeas    = utils::GetNonEmptyMaximum(fHStacks["hsPolarVsMeas_" + sBeamE], "nostack");
         marginMeas = (maxMeas - minMeas) <= 0 ? 0.1 : (maxMeas - minMeas) * 0.1;

         //if (fabs(maxMeas - minMeas) < 0.01*maxMeas) { maxMeas = 1.01*maxMeas; minMeas = 0.99*minMeas; }

         shName = "hMaxRateVsMeas_" + sPolId + "_" + sBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

         shName = "hTargetVsMeas_" + sPolId + "_" + sBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

         shName = "hPolarVsFill_" + sPolId + "_" + sBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         utils::UpdateLimits((TH1*) oc_pol->o[shName]);

         shName = "hPolarFirstMeasVsFill_" + sPolId + "_" + sBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         ((TH1*) oc_pol->o[shName])->GetYaxis()->SetRangeUser(minFill-marginFill, maxFill+marginFill);

         hist = (TH1*) oc_pol->o["hPolarHJVsFill_" + sPolId + "_" + sBeamE];
         //hist->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         //hist->GetYaxis()->SetRangeUser(minFill-marginFill, maxFill+marginFill);
         utils::UpdateLimits((TH1*) hist);

         hist = (TH1*) oc_pol->o["hPolarPCVsFill_" + sPolId + "_" + sBeamE];
         //hist->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         //hist->GetYaxis()->SetRangeUser(minFill-marginFill, maxFill+marginFill);
         utils::UpdateLimits((TH1*) hist);

         hist = (TH1*) oc_pol->o["hPolarPCScaledVsFill_" + sPolId + "_" + sBeamE];
         //hist->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         //hist->GetYaxis()->SetRangeUser(minFill-marginFill, maxFill+marginFill);
         utils::UpdateLimits((TH1*) hist);

         shName = "hPolarPCScaledVsFill_HJOnly_" + sPolId + "_" + sBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         ((TH1*) oc_pol->o[shName])->GetYaxis()->SetRangeUser(minFill-marginFill, maxFill+marginFill);

         shName = "hProfPolarVsFill_" + sPolId + "_" + sBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         ((TH1*) oc_pol->o[shName])->GetYaxis()->SetRangeUser(minFill-marginFill, maxFill+marginFill);

         shName = "hSpinAngleVsMeas_" + sPolId + "_" + sBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         //((TH1*) oc_pol->o[shName])->GetYaxis()->SetLimits(minMeas-marginMeas, maxMeas+marginMeas);

         shName = "hPolarVsMeas_" + sPolId + "_" + sBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         ((TH1*) oc_pol->o[shName])->GetYaxis()->SetLimits(minMeas-marginMeas, maxMeas+marginMeas);
         //utils::UpdateLimitsFromGraphs((TH1*) oc_pol->o[shName], 2);

         shName = "hPolarVsTime_" + sPolId + "_" + sBeamE;
         //utils::UpdateLimitsFromGraphs((TH1*) oc_pol->o[shName], 1);
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinTime, fMaxTime);
         ((TH1*) oc_pol->o[shName])->GetYaxis()->SetLimits(minMeas-marginMeas, maxMeas+marginMeas);

         hist = (TH1*) oc_pol->o["hRVsMeas_" + sPolId + "_" + sBeamE];
         hist->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         utils::UpdateLimitsFromGraphs(hist, 2);

         hist = (TH1F*) oc_pol->o["hRVsFill_" + sPolId + "_" + sBeamE];
         hist->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         utils::UpdateLimitsFromGraphs(hist, 2);

         hist = (TH1F*) oc_pol->o["hRVsFill_H_" + sPolId + "_" + sBeamE];
         hist->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         utils::UpdateLimitsFromGraphs(hist, 2);

         hist = (TH1F*) oc_pol->o["hRVsFill_V_" + sPolId + "_" + sBeamE];
         hist->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         utils::UpdateLimitsFromGraphs(hist, 2);


         shName = "hT0VsFill_" + sPolId + "_" + sBeamE;
         ((TH1*) oc_pol->o[shName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

         if (*iPolId == kB1U || *iPolId == kY2U) {
            minMeas  = utils::GetNonEmptyMinimum(fHStacks["hsT0VsFill_U_" + sBeamE], "nostack");
            maxMeas  = utils::GetNonEmptyMaximum(fHStacks["hsT0VsFill_U_" + sBeamE], "nostack");
         } else {
            minMeas  = utils::GetNonEmptyMinimum(fHStacks["hsT0VsFill_D_" + sBeamE], "nostack");
            maxMeas  = utils::GetNonEmptyMaximum(fHStacks["hsT0VsFill_D_" + sBeamE], "nostack");
         }

         //marginMeas = (maxMeas - minMeas) * 0.1;
         marginMeas = (maxMeas - minMeas) <= 0 ? 0.1 : (maxMeas - minMeas) * 0.1;
         ((TH1*) oc_pol->o[shName])->GetYaxis()->SetRangeUser(minMeas-marginMeas, maxMeas+marginMeas);

         sprintf(hName, "hT0VsTime_%s_%s", sPolId.c_str(), sBeamE.c_str());
         ((TH1*) oc_pol->o[hName])->GetXaxis()->SetLimits(fMinTime, fMaxTime);
         ((TH1*) oc_pol->o[hName])->GetYaxis()->SetLimits(minMeas-marginMeas, maxMeas+marginMeas);

         sprintf(hName, "hT0VsMeas_%s_%s", sPolId.c_str(), sBeamE.c_str());
         ((TH1*) oc_pol->o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         ((TH1*) oc_pol->o[hName])->GetYaxis()->SetLimits(minMeas-marginMeas, maxMeas+marginMeas);


         sprintf(hName, "hDLVsFill_%s_%s", sPolId.c_str(), sBeamE.c_str());
         ((TH1*) oc_pol->o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

         if (*iPolId == kB1U || *iPolId == kY2U) {
            minMeas  = utils::GetNonEmptyMinimum(fHStacks["hsDLVsFill_U_" + sBeamE], "nostack");
            maxMeas  = utils::GetNonEmptyMaximum(fHStacks["hsDLVsFill_U_" + sBeamE], "nostack");
         } else {
            minMeas  = utils::GetNonEmptyMinimum(fHStacks["hsDLVsFill_D_" + sBeamE], "nostack");
            maxMeas  = utils::GetNonEmptyMaximum(fHStacks["hsDLVsFill_D_" + sBeamE], "nostack");
         }

         //marginMeas = (maxMeas - minMeas) * 0.1;
         marginMeas = (maxMeas - minMeas) <= 0 ? 0.1 : (maxMeas - minMeas) * 0.1;
         ((TH1*) oc_pol->o[hName])->GetYaxis()->SetRangeUser(minMeas-marginMeas, maxMeas+marginMeas);

         sprintf(hName, "hDLVsMeas_%s_%s", sPolId.c_str(), sBeamE.c_str());
         ((TH1*) oc_pol->o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         ((TH1*) oc_pol->o[hName])->GetYaxis()->SetLimits(minMeas-marginMeas, maxMeas+marginMeas);

         sprintf(hName, "hDLVsTime_%s_%s", sPolId.c_str(), sBeamE.c_str());
         ((TH1*) oc_pol->o[hName])->GetXaxis()->SetLimits(fMinTime, fMaxTime);
         ((TH1*) oc_pol->o[hName])->GetYaxis()->SetLimits(minMeas-marginMeas, maxMeas+marginMeas);


         sprintf(hName, "hBananaChi2NdfVsMeas_%s_%s", sPolId.c_str(), sBeamE.c_str());
         ((TH1*) oc_pol->o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);


         ChannelSetIter iCh = gMeasInfo->fSiliconChannels.begin();

         for (; iCh!=gMeasInfo->fSiliconChannels.end(); ++iCh) {

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

   //BeamEnergySetIter iBE=gRunConfig.fBeamEnergies.begin();

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

}


/** */
void MAsymRunHists::UpdMinMaxFill(UInt_t fillId)
{
   if (fillId < fMinFill ) fMinFill = fillId;
   if (fillId > fMaxFill ) fMaxFill = fillId;
}


/** */
void MAsymRunHists::SetMinMaxFill(UInt_t minFillId, UInt_t maxFillId)
{
   fMinFill = minFillId;
   fMaxFill = maxFillId;
}

/** */
void MAsymRunHists::AdjustMinMaxFill()
{
   fMinFill -= 0.5;
   fMaxFill += 0.5;

   // Set bin
   BeamEnergySetIter iBE = gRunConfig.fBeamEnergies.begin();

   for ( ; iBE != gRunConfig.fBeamEnergies.end(); ++iBE)
   {
      string sBeamE = RunConfig::AsString(*iBE);

      PolarimeterIdSetIter iPolId = gRunConfig.fPolarimeters.begin();
      for (; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
      {
         string sPolId   = RunConfig::AsString(*iPolId);
         string sDirName = sPolId;

         // Find corresponding sub directory
         DrawObjContainer *oc_pol = d.find(sDirName)->second;

         TH1* hDisabledChVsFill_ = (TH1*) oc_pol->o["hDisabledChVsFill_" + sPolId + "_" + sBeamE];
         hDisabledChVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      }

      RingIdSetIter iRingId = gRunConfig.fRings.begin();
      for ( ; iRingId != gRunConfig.fRings.end(); ++iRingId)
      {
         string sRingId  = RunConfig::AsString(*iRingId);
         string sDirName = sRingId;

         // Find corresponding sub directory
         DrawObjContainer *oc_ring = d.find(sDirName)->second;

         TH1* hIntensDecayVsFill_ = (TH1*) oc_ring->o["hIntensDecayVsFill_" + sRingId + "_" + sBeamE];
         hIntensDecayVsFill_->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
      }
   }

}


/** */
void MAsymRunHists::UpdMinMaxTime(time_t time)
{
   if (time < fMinTime ) fMinTime = time;
   if (time > fMaxTime ) fMaxTime = time;
}


/** */
void MAsymRunHists::SetMinMaxTime(time_t minTime, time_t maxTime)
{
   fMinTime = minTime;
   fMaxTime = maxTime;
}
