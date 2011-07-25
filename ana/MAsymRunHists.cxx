/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2F.h"
#include "TFitResult.h"

#include "utils/utils.h"

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
   string sName;   

   for (UShort_t i=0; i!=N_POLARIMETERS; i++) {

      IterBeamEnergy iBE=gRunConfig.fBeamEnergies.begin();

      for ( ; iBE!=gRunConfig.fBeamEnergies.end(); ++iBE) {
         BookHistsPolarimeter((EPolarimeterId) i, *iBE);
      }

      string strPolId = RunConfig::AsString((EPolarimeterId) i);
      Color_t color   = RunConfig::AsColor((EPolarimeterId) i);

      sprintf(hName, "hT0VsChannelDiff_%s", strPolId.c_str());
      o[hName] = new TH1F(hName, hName, N_SILICON_CHANNELS, 1, N_SILICON_CHANNELS+1);
      ((TH1*) o[hName])->SetTitle(";Channel;t_{0} Diff, ns;");

      sprintf(hName, "hDLVsChannelDiff_%s", strPolId.c_str());
      o[hName] = new TH1F(hName, hName, N_SILICON_CHANNELS, 1, N_SILICON_CHANNELS+1);
      ((TH1*) o[hName])->SetTitle(";Channel;Dead Layer Diff, #mug/cm^{2};");

      // t0 vs DL diff
      TGraphErrors* grT0VsDLDiff = new TGraphErrors();
      grT0VsDLDiff->SetName("grT0VsDLDiff");
      grT0VsDLDiff->SetMarkerStyle(kFullCircle);
      grT0VsDLDiff->SetMarkerSize(1);
      grT0VsDLDiff->SetMarkerColor(color);

      sprintf(hName, "hT0VsDLDiff_%s", strPolId.c_str());
      o[hName] = new TH2F(hName, hName, 1000, 0, 1, 1000, 0, 1);
      ((TH1*) o[hName])->SetTitle(";Dead Layer Diff, #mug/cm^{2};t_{0} Diff, ns;");
      ((TH1*) o[hName])->GetListOfFunctions()->Add(grT0VsDLDiff, "p");
   }

   IterBeamEnergy iBE=gRunConfig.fBeamEnergies.begin();

   for ( ; iBE!=gRunConfig.fBeamEnergies.end(); ++iBE)
   {
      string  strBeamE = RunConfig::AsString(*iBE);
      Color_t color    = RunConfig::AsColor(kB1U);

      sprintf(hName, "hPolarBlueRatioVsFill_%s", strBeamE.c_str());
      o[hName] = new TH1F(hName, hName, 1, 0, 1);
      ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 100);
      ((TH1*) o[hName])->SetTitle(";Fill;Polarization Ratio;");
      ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
      ((TH1*) o[hName])->SetMarkerSize(1);
      ((TH1*) o[hName])->SetMarkerColor(color);
      ((TH1*) o[hName])->SetOption("E1");

      color = RunConfig::AsColor(kY2U);

      sprintf(hName, "hPolarYellowRatioVsFill_%s", strBeamE.c_str());
      o[hName] = new TH1F(hName, hName, 1, 0, 1);
      ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 100);
      ((TH1*) o[hName])->SetTitle(";Fill;Polarization Ratio;");
      ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
      ((TH1*) o[hName])->SetMarkerSize(1);
      ((TH1*) o[hName])->SetMarkerColor(color);
      ((TH1*) o[hName])->SetOption("E1");

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
void MAsymRunHists::BookHistsPolarimeter(EPolarimeterId polId, EBeamEnergy beamE)
{ //{{{
   char    hName[256];
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
   o[hName] = new TH2F(hName, hName, 1, 0, 1, 1, 0, 50);
   ((TH1*) o[hName])->SetTitle(";Measurement;Max Rate;");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grMaxRateVsMeas, "p");

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
   o[hName] = new TH2F(hName, hName, 1, 0, 1, 1, 0, 7);
   ((TH1*) o[hName])->SetTitle(";Measurement;Target Id;");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grHTargetVsMeas, "p");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grVTargetVsMeas, "p");

   // Polarization
   TGraphErrors *grPolarVsMeas = new TGraphErrors();
   grPolarVsMeas->SetName("grPolarVsMeas");
   grPolarVsMeas->SetMarkerStyle(kFullCircle);
   grPolarVsMeas->SetMarkerSize(1);
   grPolarVsMeas->SetMarkerColor(color);

   sprintf(hName, "hPolarVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH2F(hName, hName, 1, 0, 1, 1, 0, 100);
   ((TH1*) o[hName])->SetTitle(";Measurement;Polarization, %;");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grPolarVsMeas, "p");

   sprintf(hName, "hPolarVsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH1F(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 100);
   ((TH1*) o[hName])->SetTitle(";Fill;Polarization, %;");
   ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(color);
   ((TH1*) o[hName])->SetOption("E1");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grPolarVsMeas, "p");

   // Polarization vs time
   TGraphErrors *grPolarVsTime = new TGraphErrors();
   grPolarVsTime->SetName("grPolarVsTime");
   grPolarVsTime->SetMarkerStyle(kFullCircle);
   grPolarVsTime->SetMarkerSize(1);
   grPolarVsTime->SetMarkerColor(color);

   sprintf(hName, "hPolarVsTime_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH2F(hName, hName, 1, 0, 1, 1, 0, 100);
   ((TH1*) o[hName])->SetTitle(";Date & Time;Polarization, %;");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grPolarVsTime, "p");
   ((TH1*) o[hName])->GetXaxis()->SetNdivisions(10, kFALSE);
   ((TH1*) o[hName])->GetXaxis()->SetTimeOffset(0, "gmt");
   ((TH1*) o[hName])->GetXaxis()->SetTimeDisplay(1);
   //((TH1*) o[hName])->GetXaxis()->SetTimeFormat("%b %d, %H:%M:%S");
   //((TH1*) o[hName])->GetXaxis()->SetTimeFormat("%b %d, %H:%M");
   ((TH1*) o[hName])->GetXaxis()->SetTimeFormat("%m/%d");
   //((TH1*) o[hName])->GetXaxis()->SetTimeFormat("%H:%M");

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
   o[hName] = new TH2F(hName, hName, 1, 0, 1, 1, -0.3, 1);
   ((TH1*) o[hName])->SetTitle(";Measurement;r;");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grRVsMeasH, "p");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grRVsMeasV, "p");

   sprintf(hName, "hRVsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH1F(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(-0.3, 1);
   ((TH1*) o[hName])->SetTitle(";Fill;r;");
   ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(color);
   ((TH1*) o[hName])->SetOption("E1");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grRVsMeas, "p");

   sprintf(hName, "hRVsFill_H_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH1F(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(-0.3, 1);
   ((TH1*) o[hName])->SetTitle(";Fill;r;");
   ((TH1*) o[hName])->SetMarkerStyle(kFullTriangleUp);
   ((TH1*) o[hName])->SetMarkerSize(2);
   ((TH1*) o[hName])->SetMarkerColor(color-3);
   ((TH1*) o[hName])->SetOption("E1");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grRVsMeasH, "p");

   sprintf(hName, "hRVsFill_V_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH1F(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(-0.3, 1);
   ((TH1*) o[hName])->SetTitle(";Fill;r;");
   ((TH1*) o[hName])->SetMarkerStyle(kFullTriangleDown);
   ((TH1*) o[hName])->SetMarkerSize(2);
   ((TH1*) o[hName])->SetMarkerColor(color+2);
   ((TH1*) o[hName])->SetOption("E1");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grRVsMeasV, "p");

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
   o[hName] = new TH2F(hName, hName, 1, 0, 1, 1, t0Lo, t0Hi);
   ((TH1*) o[hName])->SetTitle(";Measurement;t_{0}, ns;");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grT0VsMeas, "p");

   sprintf(hName, "hT0VsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH1F(hName, hName, 1, 0, 1);
   //((TH1*) o[hName])->GetYaxis()->SetRangeUser(t0Lo, t0Hi);
   ((TH1*) o[hName])->SetTitle(";Fill;t_{0}, ns;");
   ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(color);
   ((TH1*) o[hName])->SetOption("E1");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grT0VsMeas, "p");

   TGraphErrors *grT0VsTime = new TGraphErrors();
   grT0VsTime->SetName("grT0VsTime");
   grT0VsTime->SetMarkerStyle(kFullCircle);
   grT0VsTime->SetMarkerSize(1);
   grT0VsTime->SetMarkerColor(color);

   sprintf(hName, "hT0VsTime_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH2F(hName, hName, 1, 0, 1, 100, 0, 100);
   ((TH1*) o[hName])->SetTitle(";Date & Time;t_{0}, ns;");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grT0VsTime, "p");
   ((TH1*) o[hName])->GetXaxis()->SetNdivisions(10, kFALSE);
   ((TH1*) o[hName])->GetXaxis()->SetTimeOffset(0, "gmt");
   ((TH1*) o[hName])->GetXaxis()->SetTimeDisplay(1);
   ((TH1*) o[hName])->GetXaxis()->SetTimeFormat("%m/%d");

   // DL
   TGraphErrors *grDLVsMeas = new TGraphErrors();
   grDLVsMeas->SetName("grDLVsMeas");
   grDLVsMeas->SetMarkerStyle(kFullCircle);
   grDLVsMeas->SetMarkerSize(1);
   grDLVsMeas->SetMarkerColor(color);

   sprintf(hName, "hDLVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH2F(hName, hName, 1, 0, 1, dlHi-dlLo, dlLo, dlHi);
   ((TH1*) o[hName])->SetTitle(";Measurement;Dead Layer, #mug/cm^{2};");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grDLVsMeas, "p");

   sprintf(hName, "hDLVsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH1F(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->SetTitle(";Fill;Dead Layer, #mug/cm^{2};");
   ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(color);
   ((TH1*) o[hName])->SetOption("E1");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grDLVsMeas, "p");

   TGraphErrors *grDLVsTime = new TGraphErrors();
   grDLVsTime->SetName("grDLVsTime");
   grDLVsTime->SetMarkerStyle(kFullCircle);
   grDLVsTime->SetMarkerSize(1);
   grDLVsTime->SetMarkerColor(color);

   sprintf(hName, "hDLVsTime_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH2F(hName, hName, 1, 0, 1, 100, 0, 100);
   ((TH1*) o[hName])->SetTitle(";Date & Time;Dead Layer, #mug/cm^{2};");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grDLVsTime, "p");
   ((TH1*) o[hName])->GetXaxis()->SetNdivisions(10, kFALSE);
   ((TH1*) o[hName])->GetXaxis()->SetTimeOffset(0, "gmt");
   ((TH1*) o[hName])->GetXaxis()->SetTimeDisplay(1);
   ((TH1*) o[hName])->GetXaxis()->SetTimeFormat("%m/%d");

   // Banana fit params
   TGraphErrors *grBananaChi2Ndf = new TGraphErrors();
   grBananaChi2Ndf->SetName("grBananaChi2Ndf");
   grBananaChi2Ndf->SetMarkerStyle(kFullCircle);
   grBananaChi2Ndf->SetMarkerSize(1);
   grBananaChi2Ndf->SetMarkerColor(color);

   sprintf(hName, "hBananaChi2NdfVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH2F(hName, hName, 1, 0, 1, 1, 0, 700);
   ((TH1*) o[hName])->SetTitle(";Measurement;#chi^{2}/ndf;");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grBananaChi2Ndf, "p");

   // per channel histograms

   for (UShort_t iCh=1; iCh<=N_SILICON_CHANNELS; iCh++)
   {
      // t0
      grT0VsMeas = new TGraphErrors();
      grT0VsMeas->SetName("grT0VsMeas");
      grT0VsMeas->SetMarkerStyle(kFullCircle);
      grT0VsMeas->SetMarkerSize(1);
      grT0VsMeas->SetMarkerColor(color);

      sprintf(hName, "hT0VsMeas_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), iCh);
      o[hName] = new TH2F(hName, hName, 1, 0, 1, t0Hi-t0Lo, t0Lo, t0Hi);
      ((TH1*) o[hName])->SetOption("NOIMG");
      ((TH1*) o[hName])->SetTitle(";Measurement;t_{0}, ns;");
      ((TH1*) o[hName])->GetListOfFunctions()->Add(grT0VsMeas, "p");

      sprintf(hName, "hT0_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), iCh);
      o[hName] = new TH1F(hName, hName, t0Hi-t0Lo, t0Lo, t0Hi);
      //((TH1*) o[hName])->SetOption("hist");
      ((TH1*) o[hName])->SetOption("hist NOIMG");
      ((TH1*) o[hName])->SetTitle(";t_{0}, ns;Events;");
      //((TH1*) o[hName])->Sumw2();

      // DL
      grDLVsMeas = new TGraphErrors();
      grDLVsMeas->SetName("grDLVsMeas");
      grDLVsMeas->SetMarkerStyle(kFullCircle);
      grDLVsMeas->SetMarkerSize(1);
      grDLVsMeas->SetMarkerColor(color);

      sprintf(hName, "hDLVsMeas_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), iCh);
      o[hName] = new TH2F(hName, hName, 1, 0, 1, dlHi-dlLo, dlLo, dlHi);
      ((TH1*) o[hName])->SetOption("NOIMG");
      ((TH1*) o[hName])->SetTitle(";Measurement;Dead Layer, #mug/cm^{2};");
      ((TH1*) o[hName])->GetListOfFunctions()->Add(grDLVsMeas, "p");

      sprintf(hName, "hDL_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), iCh);
      o[hName] = new TH1F(hName, hName, dlHi-dlLo, dlLo, dlHi);
      ((TH1*) o[hName])->SetOption("hist NOIMG");
      ((TH1*) o[hName])->SetTitle(";Dead Layer, #mug/cm^{2};Events;");

      // t0 vs DL
      TGraphErrors* grT0VsDL = new TGraphErrors();
      grT0VsDL->SetName("grT0VsDL");
      grT0VsDL->SetMarkerStyle(kFullCircle);
      grT0VsDL->SetMarkerSize(1);
      grT0VsDL->SetMarkerColor(color);

      sprintf(hName, "hT0VsDL_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), iCh);
      o[hName] = new TH2F(hName, hName, dlHi-dlLo, dlLo, dlHi, t0Hi-t0Lo, t0Lo, t0Hi);
      ((TH1*) o[hName])->SetOption("NOIMG");
      ((TH1*) o[hName])->SetTitle(";Dead Layer, #mug/cm^{2};t_{0}, ns;");
      ((TH1*) o[hName])->GetListOfFunctions()->Add(grT0VsDL, "p");
   }

   sprintf(hName, "hT0VsChannel_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH1F(hName, hName, N_SILICON_CHANNELS, 1, N_SILICON_CHANNELS+1);
   ((TH1*) o[hName])->SetTitle(";Channel;Mean t_{0}, ns;");

   sprintf(hName, "hDLVsChannel_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH1F(hName, hName, N_SILICON_CHANNELS, 1, N_SILICON_CHANNELS+1);
   ((TH1*) o[hName])->SetTitle(";Channel;Mean Dead Layer, #mug/cm^{2};");

   // t0 vs DL
   TGraphErrors* grT0VsDLMean = new TGraphErrors();
   grT0VsDLMean->SetName("grT0VsDLMean");
   grT0VsDLMean->SetMarkerStyle(kFullCircle);
   grT0VsDLMean->SetMarkerSize(1);
   grT0VsDLMean->SetMarkerColor(color);

   sprintf(hName, "hT0VsDLMean_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH2F(hName, hName, dlHi-dlLo, dlLo, dlHi, t0Hi-t0Lo, t0Lo, t0Hi);
   ((TH1*) o[hName])->SetTitle(";Dead Layer, #mug/cm^{2};Mean t_{0}, ns;");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grT0VsDLMean, "p");

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
   Float_t  profileRatio     = rc.fAnaResult->fIntensPolarR;
   Float_t  profileRatioErr  = rc.fAnaResult->fIntensPolarRErr;
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

   if (profileRatio < 0.0001 || profileRatioErr < 0.001) {
	   Warning("Fill", "Didn't pass QA check");
      return;
	}

   //t0Err         = chCalib->fT0CoefErr;

   if (gRunConfig.fBeamEnergies.find((EBeamEnergy) beamEnergy) == gRunConfig.fBeamEnergies.end())
      return;

   char hName[256];

   string strPolId = RunConfig::AsString((EPolarimeterId) polId);
   string strBeamE = RunConfig::AsString((EBeamEnergy) beamEnergy);

   TGraphErrors *graphErrs = 0;
   TGraph       *graph = 0;

   // Rate
   sprintf(hName, "hMaxRateVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graph = (TGraph*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grMaxRateVsMeas");
   graph->SetPoint(graph->GetN(), runId, max_rate);

   // Targets
   sprintf(hName, "hTargetVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());

   if (targetOrient == 'H') {
      graph = (TGraph*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grHTargetVsMeas");
   }

   if (targetOrient == 'V') {
      graph = (TGraph*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grVTargetVsMeas");
   }

   graph->SetPoint(graph->GetN(), runId, targetId);

   UInt_t nPoints = 0;

   // Polarization
   sprintf(hName, "hPolarVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grPolarVsMeas");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runId, polarization);
   graphErrs->SetPointError(nPoints, 0, polarizationErr);

   // Polarization vs time
   sprintf(hName, "hPolarVsTime_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grPolarVsTime");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runStartTime, polarization);
   graphErrs->SetPointError(nPoints, 0, polarizationErr);

   // Profiles r
   sprintf(hName, "hRVsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grRVsMeas");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runId, profileRatio);
   graphErrs->SetPointError(nPoints, 0, profileRatioErr);

   sprintf(hName, "hRVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());

   if (targetOrient == 'H') {
      graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grRVsMeasH");
   }

   if (targetOrient == 'V') {
      graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grRVsMeasV");
   }

   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runId, profileRatio);
   graphErrs->SetPointError(nPoints, 0, profileRatioErr);

   // t0
   sprintf(hName, "hT0VsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grT0VsMeas");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runId, t0);
   graphErrs->SetPointError(nPoints, 0, t0Err);

   // t0 vs time
   sprintf(hName, "hT0VsTime_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grT0VsTime");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runStartTime, t0);
   graphErrs->SetPointError(nPoints, 0, t0Err);

   // Dead layer
   sprintf(hName, "hDLVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grDLVsMeas");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runId, dl);
   graphErrs->SetPointError(nPoints, 0, dlErr);

   // DL vs time
   sprintf(hName, "hDLVsTime_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grDLVsTime");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runStartTime, dl);
   graphErrs->SetPointError(nPoints, 0, dlErr);


   // Banana fit params
   //Float_t bananaChi2Ndf = rc.fCalibrator->fChannelCalibs[0].fBananaChi2Ndf;
   Float_t bananaChi2Ndf = chCalib->fBananaChi2Ndf;

   sprintf(hName, "hBananaChi2NdfVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grBananaChi2Ndf");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runId, bananaChi2Ndf);

   // per channel hists

   for (UShort_t iCh=1; iCh<=N_SILICON_CHANNELS; iCh++) {

      // T0
      t0    = rc.fCalibrator->fChannelCalibs[iCh].fT0Coef;
      t0Err = rc.fCalibrator->fChannelCalibs[iCh].fT0CoefErr;

      // XXX also should be removed when real QA is available
      if (isnan(t0) || isinf(t0) || isnan(t0Err) || isinf(t0Err)) continue;

      sprintf(hName, "hT0VsMeas_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), iCh);
      graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grT0VsMeas");
      nPoints = graphErrs->GetN();
      graphErrs->SetPoint(nPoints, runId, t0);
      graphErrs->SetPointError(nPoints, 0, t0Err);

      sprintf(hName, "hT0_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), iCh);
      ((TH1*) o[hName])->Fill(t0);

      // DL
      dl    = rc.fCalibrator->fChannelCalibs[iCh].fDLWidth;
      dlErr = rc.fCalibrator->fChannelCalibs[iCh].fDLWidthErr;

      sprintf(hName, "hDLVsMeas_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), iCh);
      graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grDLVsMeas");
      nPoints = graphErrs->GetN();
      graphErrs->SetPoint(nPoints, runId, dl);
      graphErrs->SetPointError(nPoints, 0, dlErr);

      sprintf(hName, "hDL_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), iCh);
      ((TH1*) o[hName])->Fill(dl);

      // t0 vs DL
      sprintf(hName, "hT0VsDL_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), iCh);
      graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grT0VsDL");
      nPoints = graphErrs->GetN();
      graphErrs->SetPoint(nPoints, dl, t0);
      graphErrs->SetPointError(nPoints, dlErr, t0Err);
   }
} //}}}


/** */
void MAsymRunHists::Print(const Option_t* opt) const
{ //{{{
   opt = ""; //printf("MAsymRunHists:\n");
   DrawObjContainer::Print();
} //}}}


/** */
void MAsymRunHists::Fill(ChannelEvent *ch, string sid)
{ //{{{
} //}}}


/** */
void MAsymRunHists::PostFill()
{ //{{{
   fMinFill -= 0.5;
   fMaxFill += 0.5;

   char hName[256];
   string shName;

   for (UInt_t i=0; i!=N_POLARIMETERS; i++) {

      string strPolId = RunConfig::AsString((EPolarimeterId) i);

      // Adjust axis range
      Double_t xmin, ymin, xmax, ymax, xdelta, ydelta;
      Double_t xmean, ymean;
      Double_t xminDL, yminDL, xmaxDL, ymaxDL, xdeltaDL, ydeltaDL;

      IterBeamEnergy iBE=gRunConfig.fBeamEnergies.begin();

      for (; iBE!=gRunConfig.fBeamEnergies.end(); ++iBE) {

         string strBeamE = RunConfig::AsString(*iBE);

         sprintf(hName, "hT0VsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
         TGraphErrors *graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grT0VsMeas");
         graphErrs->ComputeRange(xmin, ymin, xmax, ymax);
         ydelta = (ymax - ymin)*0.1;
         ((TH1*) o[hName])->GetYaxis()->SetLimits(ymin-ydelta, ymax+ydelta);

         //sprintf(hName, "hT0VsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
         //graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grT0VsMeas");
         //((TH1*) o[hName])->GetYaxis()->SetLimits(ymin-ydelta, ymax+ydelta);
         //((TH1*) o[hName])->GetYaxis()->SetRangeUser(ymin-ydelta, ymax+ydelta);

         sprintf(hName, "hT0VsTime_%s_%s", strPolId.c_str(), strBeamE.c_str());
         graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grT0VsTime");
         graphErrs->ComputeRange(xmin, ymin, xmax, ymax);
         ymean = graphErrs->GetMean(2);
         //xdelta = fabs(xmax - xmin)*0.1;
         //ydelta = fabs(ymax - ymin)*0.1;
         ((TH1*) o[hName])->GetXaxis()->SetLimits(xmin, xmax);
         //((TH1*) o[hName])->GetYaxis()->SetLimits(ymin - ydelta, ymax + ydelta);
         ((TH1*) o[hName])->GetYaxis()->SetLimits(ymean - 5, ymean + 5);

         sprintf(hName, "hDLVsTime_%s_%s", strPolId.c_str(), strBeamE.c_str());
         graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grDLVsTime");
         graphErrs->ComputeRange(xmin, ymin, xmax, ymax);
         ymean = graphErrs->GetMean(2);
         //xdelta = fabs(xmax - xmin)*0.1;
         ydelta = fabs(ymax - ymin)*0.1;
         //((TH1*) o[hName])->GetXaxis()->SetLimits(xmin, xmax);
         //((TH1*) o[hName])->GetYaxis()->SetLimits(ymin - ydelta, ymax + ydelta);
         ((TH1*) o[hName])->GetYaxis()->SetLimits(ymean - 20, ymean + 20);

         TGraphErrors *graph;

         // Polarization
         sprintf(hName, "hPolarVsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) o[hName])->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
         graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grPolarVsMeas");

         utils::RemoveOutliers(graph, 2, 3);
         utils::BinGraph(graph, (TH1*) o[hName]);

         ((TH1*) o[hName])->GetListOfFunctions()->Remove(graph);

         TF1 *funcPolarVsFill = new TF1("funcPolarVsFill", "[0]");
         ((TH1*) o[hName])->Fit("funcPolarVsFill");
         delete funcPolarVsFill;

         fHStacks["hsPolarVsFill_" + strBeamE]->Add( (TH1*) o[hName] );

         // Polarization vs time
         //sprintf(hName, "hPolarVsTime_%s_%s", strPolId.c_str(), strBeamE.c_str());
         //graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grPolarVsTime");

         //TF1 *funcPolarVsTime = new TF1("funcPolarVsTime", "[0] + [1]*x");
         ////funcPolarVsTime->SetParameter(0, 0);
         //funcPolarVsTime->SetParNames("offset", "slope");
         //graph->Fit("funcPolarVsTime");
         //delete funcPolarVsTime;

         // Profiles r
         shName = "hRVsFill_" + strPolId + "_" + strBeamE;
         graph = (TGraphErrors*) ((TH1*) o[shName])->GetListOfFunctions()->FindObject("grRVsMeas");
         ((TH1*) o[shName])->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);

         //utils::RemoveOutliers(graph, 2, 3);
         utils::BinGraph(graph, (TH1*) o[shName]);
         ((TH1*) o[shName])->GetListOfFunctions()->Remove(graph);
         ((TH1*) o[shName])->Fit("pol0");


         // H target = vert profile
         sprintf(hName, "hRVsFill_H_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) o[hName])->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
         graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grRVsMeasH");

         utils::RemoveOutliers(graph, 2, 3);
         utils::BinGraph(graph, (TH1*) o[hName]);

         ((TH1*) o[hName])->GetListOfFunctions()->Remove(graph);

         TF1 *funcRVsFill = new TF1("funcRVsFill", "[0]");
         funcRVsFill->SetParameter(0, 0.02);
         ((TH1*) o[hName])->Fit("funcRVsFill");
         //graph->Fit("funcRVsFill");
         delete funcRVsFill;

         // V target = horiz profile
         sprintf(hName, "hRVsFill_V_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) o[hName])->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
         graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grRVsMeasV");

         utils::RemoveOutliers(graph, 2, 3);
         utils::BinGraph(graph, (TH1*) o[hName]);

         ((TH1*) o[hName])->GetListOfFunctions()->Remove(graph);

         funcRVsFill = new TF1("funcRVsFill", "[0]");
         funcRVsFill->SetParameter(0, 0.02);
         ((TH1*) o[hName])->Fit("funcRVsFill");
         //graph->Fit("funcRVsFill");
         delete funcRVsFill;

         // t0
         sprintf(hName, "hT0VsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) o[hName])->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
         graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grT0VsMeas");

         //utils::RemoveOutliers(graph, 2, 3);
         utils::BinGraph(graph, (TH1*) o[hName]);

         ((TH1*) o[hName])->GetListOfFunctions()->Remove(graph);

         //TF1 *funcT0VsFill = new TF1("funcT0VsFill", "[0] + [1]*x");
         //funcT0VsFill->SetParameters(0, 0);
         //funcT0VsFill->SetParNames("offset", "slope");
         //((TH1*) o[hName])->Fit("funcT0VsFill");
         //delete funcT0VsFill;

         if (strPolId == "B1U" || strPolId == "Y2U" )
            fHStacks["hsT0VsFill_U_" + strBeamE]->Add( (TH1*) o[hName] );
         else
            fHStacks["hsT0VsFill_D_" + strBeamE]->Add( (TH1*) o[hName] );


         // DL
         sprintf(hName, "hDLVsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) o[hName])->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
         graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grDLVsMeas");

         utils::RemoveOutliers(graph, 2, 3);
         utils::BinGraph(graph, (TH1*) o[hName]);

         ((TH1*) o[hName])->GetListOfFunctions()->Remove(graph);

         TF1 *funcDLVsFill = new TF1("funcDLVsFill", "[0]");
         funcDLVsFill->SetParNames("const");
         ((TH1*) o[hName])->Fit("funcDLVsFill");
         delete funcDLVsFill;

         if (strPolId == "B1U" || strPolId == "Y2U" )
            fHStacks["hsDLVsFill_U_" + strBeamE]->Add( (TH1*) o[hName] );
         else
            fHStacks["hsDLVsFill_D_" + strBeamE]->Add( (TH1*) o[hName] );

         sprintf(hName, "hT0VsChannel_%s_%s", strPolId.c_str(), strBeamE.c_str());
         TH1* hT0VsChannel = (TH1*) o[hName];

         sprintf(hName, "hDLVsChannel_%s_%s", strPolId.c_str(), strBeamE.c_str());
         TH1* hDLVsChannel = (TH1*) o[hName];

         sprintf(hName, "hT0VsDLMean_%s_%s", strPolId.c_str(), strBeamE.c_str());
         TGraphErrors* grT0VsDLMean = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grT0VsDLMean");

         for (UShort_t iCh=1; iCh<=N_SILICON_CHANNELS; iCh++)
         {
            sprintf(hName, "hT0VsMeas_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), iCh);
            graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grT0VsMeas");
            graphErrs->ComputeRange(xmin, ymin, xmax, ymax);
            ydelta = (ymax - ymin)*0.1;
            ((TH1*) o[hName])->GetYaxis()->SetLimits(ymin-ydelta, ymax+ydelta);

            sprintf(hName, "hDLVsMeas_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), iCh);
            graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grDLVsMeas");
            graphErrs->ComputeRange(xminDL, yminDL, xmaxDL, ymaxDL);
            ydeltaDL = (ymaxDL - yminDL)*0.1;

            // T0
            sprintf(hName, "hT0_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), iCh);
            TH1* hT0 = (TH1*) o[hName];

            utils::RemoveOutliers(hT0);

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
            hT0VsChannel->SetBinError(iCh, hT0->GetRMS());
            //hT0VsChannel->SetBinError(iCh, fitres->Parameter(2));

            //   //delete fitres;
            //   delete myGaus;
            //}

            // Dead layer
            sprintf(hName, "hDL_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), iCh);
            TH1* hDL = (TH1*) o[hName];
            utils::RemoveOutliers(hDL);

            hDLVsChannel->SetBinContent(iCh, hDL->GetMean());
            hDLVsChannel->SetBinError(iCh, hDL->GetRMS());

            // t0 vs DL
            sprintf(hName, "hT0VsDL_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), iCh);
            //graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grT0VsDL");
            ((TH1*) o[hName])->GetXaxis()->SetLimits(yminDL-ydeltaDL, ymaxDL+ydeltaDL);
            ((TH1*) o[hName])->GetYaxis()->SetLimits(ymin-ydelta, ymax+ydelta);

            UInt_t nPoints = grT0VsDLMean->GetN();
            grT0VsDLMean->SetPoint(nPoints, hDL->GetMean(), hT0->GetMean());
            //grT0VsDLMean->SetPointError(nPoints, hDL->GetRMS(), hT0->GetRMS());
         }

         sprintf(hName, "hT0VsDLMean_%s_%s", strPolId.c_str(), strBeamE.c_str());
         grT0VsDLMean = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grT0VsDLMean");
         grT0VsDLMean->ComputeRange(xmin, ymin, xmax, ymax);
         ydelta = fabs(ymax - ymin)*0.1;
         ((TH1*) o[hName])->GetYaxis()->SetLimits(ymin-ydelta, ymax+ydelta);
      }

      TH1* hT0VsChannelDiff  = (TH1*) o["hT0VsChannelDiff_"+strPolId];
      TH1* hT0VsChannelInj   = (TH1*) o["hT0VsChannel_"+strPolId+"_024"];
      TH1* hT0VsChannelFlt   = (TH1*) o["hT0VsChannel_"+strPolId+"_250"];

      hT0VsChannelDiff->Add(hT0VsChannelInj, hT0VsChannelFlt, 1, -1);

      TH1* hDLVsChannelDiff  = (TH1*) o["hDLVsChannelDiff_"+strPolId];
      TH1* hDLVsChannelInj   = (TH1*) o["hDLVsChannel_"+strPolId+"_024"];
      TH1* hDLVsChannelFlt   = (TH1*) o["hDLVsChannel_"+strPolId+"_250"];

      hDLVsChannelDiff->Add(hDLVsChannelInj, hDLVsChannelFlt, 1, -1);

      TH1* hT0VsDLDiff  = (TH1*) o["hT0VsDLDiff_"+strPolId];
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
   }

   IterBeamEnergy iBE=gRunConfig.fBeamEnergies.begin();

   for (; iBE!=gRunConfig.fBeamEnergies.end(); ++iBE) {

      string strBeamE = RunConfig::AsString(*iBE);

      // Blue
      TH1* hPolarU = (TH1*) o["hPolarVsFill_B1U_" + strBeamE];
      TH1* hPolarD = (TH1*) o["hPolarVsFill_B2D_" + strBeamE];

      shName = "hPolarBlueRatioVsFill_" + strBeamE;
      ((TH1*) o[shName])->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
      ((TH1*) o[shName])->Divide(hPolarU, hPolarD);
      ((TH1*) o[shName])->Fit("pol0");

      // Yellow
      hPolarU = (TH1*) o["hPolarVsFill_Y2U_" + strBeamE];
      hPolarD = (TH1*) o["hPolarVsFill_Y1D_" + strBeamE];

      shName = "hPolarYellowRatioVsFill_" + strBeamE;
      ((TH1*) o[shName])->SetBins(fMaxFill-fMinFill, fMinFill, fMaxFill);
      ((TH1*) o[shName])->Divide(hPolarU, hPolarD);
      ((TH1*) o[shName])->Fit("pol0");
   }
} //}}}


/** */
void MAsymRunHists::UpdateLimits()
{ //{{{
   char hName[256];

   IterPolarimeterId iPolId=gRunConfig.fPolarimeters.begin();

   for (; iPolId!=gRunConfig.fPolarimeters.end(); ++iPolId)
   {

      Double_t min, max, marg;

      IterBeamEnergy iBE=gRunConfig.fBeamEnergies.begin();

      for (; iBE!=gRunConfig.fBeamEnergies.end(); ++iBE)
      {
         string strPolId = RunConfig::AsString(*iPolId);
         string strBeamE = RunConfig::AsString(*iBE);

         sprintf(hName, "hMaxRateVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

         sprintf(hName, "hTargetVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);


         sprintf(hName, "hPolarVsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
			//utils::UpdateLimitsFromGraphs((TH1*) o[hName], 1);
			//utils::RebinIntegerAxis((TH1*) o[hName]);
         ((TH1*) o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         min  = utils::GetNonEmptyMinimum(fHStacks["hsPolarVsFill_" + strBeamE], "nostack");
         max  = utils::GetNonEmptyMaximum(fHStacks["hsPolarVsFill_" + strBeamE], "nostack");
         marg = (max - min) * 0.1;
         ((TH1*) o[hName])->GetYaxis()->SetRangeUser(min-marg, max+marg);

         sprintf(hName, "hPolarVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
			//utils::UpdateLimitsFromGraphs((TH1*) o[hName], 1);
         ((TH1*) o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         ((TH1*) o[hName])->GetYaxis()->SetLimits(min-marg, max+marg);

         sprintf(hName, "hPolarVsTime_%s_%s", strPolId.c_str(), strBeamE.c_str());
			//utils::UpdateLimitsFromGraphs((TH1*) o[hName], 1);
         ((TH1*) o[hName])->GetXaxis()->SetLimits(fMinTime, fMaxTime);
         ((TH1*) o[hName])->GetYaxis()->SetLimits(min-marg, max+marg);


         sprintf(hName, "hRVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

         sprintf(hName, "hRVsFill_H_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

         sprintf(hName, "hRVsFill_V_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);


         sprintf(hName, "hT0VsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

         if (*iPolId == kB1U || *iPolId == kY2U) {
            min  = utils::GetNonEmptyMinimum(fHStacks["hsT0VsFill_U_" + strBeamE], "nostack");
            max  = utils::GetNonEmptyMaximum(fHStacks["hsT0VsFill_U_" + strBeamE], "nostack");
			} else {
            min  = utils::GetNonEmptyMinimum(fHStacks["hsT0VsFill_D_" + strBeamE], "nostack");
            max  = utils::GetNonEmptyMaximum(fHStacks["hsT0VsFill_D_" + strBeamE], "nostack");
		   }

         marg = (max - min) * 0.1;
         ((TH1*) o[hName])->GetYaxis()->SetRangeUser(min-marg, max+marg);

         sprintf(hName, "hT0VsTime_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) o[hName])->GetXaxis()->SetLimits(fMinTime, fMaxTime);
         ((TH1*) o[hName])->GetYaxis()->SetLimits(min-marg, max+marg);

         sprintf(hName, "hT0VsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         ((TH1*) o[hName])->GetYaxis()->SetLimits(min-marg, max+marg);


         sprintf(hName, "hDLVsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

         if (*iPolId == kB1U || *iPolId == kY2U) {
            min  = utils::GetNonEmptyMinimum(fHStacks["hsDLVsFill_U_" + strBeamE], "nostack");
            max  = utils::GetNonEmptyMaximum(fHStacks["hsDLVsFill_U_" + strBeamE], "nostack");
			} else {
            min  = utils::GetNonEmptyMinimum(fHStacks["hsDLVsFill_D_" + strBeamE], "nostack");
            max  = utils::GetNonEmptyMaximum(fHStacks["hsDLVsFill_D_" + strBeamE], "nostack");
			}

         marg = (max - min) * 0.1;
         ((TH1*) o[hName])->GetYaxis()->SetRangeUser(min-marg, max+marg);

         sprintf(hName, "hDLVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
         ((TH1*) o[hName])->GetYaxis()->SetLimits(min-marg, max+marg);

         sprintf(hName, "hDLVsTime_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) o[hName])->GetXaxis()->SetLimits(fMinTime, fMaxTime);
         ((TH1*) o[hName])->GetYaxis()->SetLimits(min-marg, max+marg);


         sprintf(hName, "hBananaChi2NdfVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
         ((TH1*) o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);


         for (UShort_t iCh=1; iCh<=N_SILICON_CHANNELS; iCh++)
         {
            sprintf(hName, "hT0VsMeas_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), iCh);
            ((TH1*) o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

            sprintf(hName, "hDLVsMeas_%s_%s_%02d", strPolId.c_str(), strBeamE.c_str(), iCh);
            ((TH1*) o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
	      }
      }
   }

   IterBeamEnergy iBE=gRunConfig.fBeamEnergies.begin();

   for (; iBE!=gRunConfig.fBeamEnergies.end(); ++iBE)
   {
      string strBeamE = RunConfig::AsString(*iBE);

      sprintf(hName, "hPolarBlueRatioVsFill_%s", strBeamE.c_str());
      ((TH1*) o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);

      sprintf(hName, "hPolarYellowRatioVsFill_%s", strBeamE.c_str());
      ((TH1*) o[hName])->GetXaxis()->SetLimits(fMinFill, fMaxFill);
   }

} //}}}


/** */
void MAsymRunHists::SetMinMaxFill(UInt_t fillId)
{
   if (fillId < fMinFill ) fMinFill = fillId;
   if (fillId > fMaxFill ) fMaxFill = fillId;
}


/** */
void MAsymRunHists::SetMinMaxTime(UInt_t time)
{
   if (time < fMinTime ) fMinTime = time;
   if (time > fMaxTime ) fMaxTime = time;
}
