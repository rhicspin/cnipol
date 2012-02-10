/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "TDirectoryFile.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2F.h"
#include "TStyle.h"

#include "utils/utils.h"

#include "MAsymFillHists.h"
#include "MAsymSingleFillHists.h"

#include "MseMeasInfo.h"


ClassImp(MAsymFillHists)

using namespace std;

/** Default constructor. */
MAsymFillHists::MAsymFillHists() : DrawObjContainer(), fHTargetVsRun(), fVTargetVsRun()
{
   BookHists();
}


MAsymFillHists::MAsymFillHists(TDirectory *dir) : DrawObjContainer(dir), fHTargetVsRun(), fVTargetVsRun()
{
   BookHists();
}


/** Default destructor. */
MAsymFillHists::~MAsymFillHists()
{
   //delete grHTargetVsMeas;
   //delete grVTargetVsMeas;
}


/** */
void MAsymFillHists::BookHists(string sid)
{ //{{{
   fDir->cd();

   for (UInt_t i=0; i!=N_POLARIMETERS; i++) {
      BookHistsPolarimeter((EPolarimeterId) i);
   }

} //}}}


/** */
void MAsymFillHists::BookHistsPolarimeter(EPolarimeterId polId)
{ //{{{
   char    hName[256];
   string  shName;
   TH1    *hist;
   //char    hTitle[256];
   string  strPolId = RunConfig::AsString(polId);
   //string  strBeamE = RunConfig::AsString(beamE);
   Color_t color    = RunConfig::AsColor(polId);

   //TStyle  styleMarker;
   TAttMarker  styleMarker;

   styleMarker.SetMarkerStyle(kFullCircle);
   styleMarker.SetMarkerSize(1);
   styleMarker.SetMarkerColor(color);


   // Asymmetry
   sprintf(hName, "grAsymVsEnergy");
   TGraphErrors *grAsymVsEnergy = new TGraphErrors();
   grAsymVsEnergy->SetName(hName);
   grAsymVsEnergy->SetMarkerStyle(kFullCircle);
   grAsymVsEnergy->SetMarkerSize(1);
   grAsymVsEnergy->SetMarkerColor(color);

   sprintf(hName, "hAsymVsEnergy_%s", strPolId.c_str());
   o[hName] = new TH2F(hName, hName, 1, 0, 450, 1, 0, 0.01);
   ((TH1*) o[hName])->SetTitle(";Beam Energy;Asymmetry;");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grAsymVsEnergy, "p");

   // Analyzing power
   sprintf(hName, "grAnaPowerVsEnergy");
   TGraphErrors *grAnaPowerVsEnergy = new TGraphErrors();
   grAnaPowerVsEnergy->SetName(hName);
   grAnaPowerVsEnergy->SetMarkerStyle(kFullCircle);
   grAnaPowerVsEnergy->SetMarkerSize(1);
   grAnaPowerVsEnergy->SetMarkerColor(color);

   sprintf(hName, "hAnaPowerVsEnergy_%s", strPolId.c_str());
   o[hName] = new TH2F(hName, hName, 1, 0, 450, 1, 0.01, 0.02);
   ((TH1*) o[hName])->SetTitle(";Beam Energy;A_{N};");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grAnaPowerVsEnergy, "p");

   sprintf(hName, "hAnaPowerVsEnergyBinned_%s", strPolId.c_str());
   Float_t energyBins[10] = {0, 14, 34, 90, 110, 240, 260, 390, 410, 450};
   o[hName] = new TH1F(hName, hName, 9, energyBins);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0.01, 0.02);
   ((TH1*) o[hName])->SetTitle(";Beam Energy;A_{N};");
   ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(color);
   ((TH1*) o[hName])->SetOption("hist");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grAnaPowerVsEnergy, "p");

   // Polarization
   TGraphErrors *grPolarVsEnergy = new TGraphErrors();
   grPolarVsEnergy->SetName("grPolarVsEnergy");
   grPolarVsEnergy->SetMarkerStyle(kFullCircle);
   grPolarVsEnergy->SetMarkerSize(1);
   grPolarVsEnergy->SetMarkerColor(color);

   sprintf(hName, "hPolarVsEnergy_%s", strPolId.c_str());
   o[hName] = new TH2F(hName, hName, 1, 0, 450, 1, 0, 100);
   ((TH1*) o[hName])->SetTitle(";Beam Energy;Polarization, %;");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grPolarVsEnergy, "p");

   TGraphErrors *grPolarVsFillTime = new TGraphErrors();
   grPolarVsFillTime->SetName("grPolarVsFillTime");
   grPolarVsFillTime->SetMarkerStyle(kFullCircle);
   grPolarVsFillTime->SetMarkerSize(1);
   grPolarVsFillTime->SetMarkerColor(color);

   sprintf(hName, "hPolarVsFillTime_%s", strPolId.c_str());
   //o[hName] = new TH2F(hName, hName, 12, 0, 12*3600, 60, 20, 80);
   o[hName] = new TH2F(hName, hName, 12, 0, 12*3600, 100, 20, 100);
   ((TH1*) o[hName])->SetTitle(";Time in Fill, hours;Polarization, %;");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grPolarVsFillTime, "p");
   ((TH1*) o[hName])->GetXaxis()->SetTimeOffset(3600*6, "gmt");
   //((TH1*) o[hName])->GetXaxis()->SetTimeOffset(0, "local");
   ((TH1*) o[hName])->GetXaxis()->SetTimeDisplay(1);
   ((TH1*) o[hName])->GetXaxis()->SetTimeFormat("%H");

   // Polarization per measurement number (first five measurements)
   shName = "hPolarVsMeasId_" + strPolId;
   hist = new TH1D(shName.c_str(), shName.c_str(), 10, 0.5, 10.5);
   hist->GetYaxis()->SetRangeUser(0.7, 1.3);
   hist->SetTitle(";Meas. Id;P/P_{first meas.};");
   hist->SetOption("E1");
   styleMarker.Copy(*hist); o[shName] = hist;

   // Polarization per measurement number (first five measurements)
   shName = "hProfilePolarMaxVsMeasId_" + strPolId;
   o[shName] = new TH1D(shName.c_str(), shName.c_str(), 10, 0.5, 10.5);
   ((TH1*) o[shName])->GetYaxis()->SetRangeUser(0.7, 1.3);
   ((TH1*) o[shName])->SetTitle(";Meas. Id;P_{0}/P_{0, first meas.};");

   // Profile r
   TGraphErrors *grRVsEnergy = new TGraphErrors();
   grRVsEnergy->SetName("grRVsEnergy");
   grRVsEnergy->SetMarkerStyle(kFullCircle);
   grRVsEnergy->SetMarkerSize(1);
   grRVsEnergy->SetMarkerColor(color);

   sprintf(hName, "hRVsEnergy_%s", strPolId.c_str());
   o[hName] = new TH2F(hName, hName, 1, 0, 450, 1, -0.1, 1);
   ((TH1*) o[hName])->SetTitle(";Beam Energy;r;");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grRVsEnergy, "p");

   sprintf(hName, "hRVsEnergyBinned_%s", strPolId.c_str());
   o[hName] = new TH1F(hName, hName, 50, 0, 450);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(-0.1, 1);
   ((TH1*) o[hName])->SetTitle(";Beam Energy;r;");
   ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(color);
   ((TH1*) o[hName])->SetOption("E1");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grRVsEnergy, "p");

   TGraphErrors *grRVsFillTime = new TGraphErrors();
   grRVsFillTime->SetName("grRVsFillTime");
   grRVsFillTime->SetMarkerStyle(kFullCircle);
   grRVsFillTime->SetMarkerSize(1);
   grRVsFillTime->SetMarkerColor(color);

   sprintf(hName, "hRVsFillTime_%s", strPolId.c_str());
   o[hName] = new TH2F(hName, hName, 12, 0, 12*3600, 100, -0.1, 1);
   ((TH1*) o[hName])->SetTitle(";Time in Fill, hours;r;");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grRVsFillTime, "p");
   ((TH1*) o[hName])->GetXaxis()->SetTimeOffset(3600*6, "gmt");
   //((TH1*) o[hName])->GetXaxis()->SetTimeOffset(0, "local");
   ((TH1*) o[hName])->GetXaxis()->SetTimeDisplay(1);
   ((TH1*) o[hName])->GetXaxis()->SetTimeFormat("%H");

   sprintf(hName, "hRVsFillTimeBinned_%s", strPolId.c_str());
   o[hName] = new TH1F(hName, hName, 36, 0, 12*3600);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(-0.1, 1);
   ((TH1*) o[hName])->SetTitle(";Time in Fill, hours;r;");
   ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(color);
   ((TH1*) o[hName])->SetOption("E1");
   //((TH1*) o[hName])->GetListOfFunctions()->Add(grRVsFillTime, "p");
   ((TH1*) o[hName])->GetXaxis()->SetTimeOffset(3600*6, "gmt");
   //((TH1*) o[hName])->GetXaxis()->SetTimeOffset(0, "local");
   ((TH1*) o[hName])->GetXaxis()->SetTimeDisplay(1);
   ((TH1*) o[hName])->GetXaxis()->SetTimeFormat("%H");

   // Profile R
   shName = "hProfileRVsMeasId_" + strPolId;
   hist = new TH1D(shName.c_str(), shName.c_str(), 10, 0.5, 10.5);
   hist->GetYaxis()->SetRangeUser(-0.5, 0.5);
   hist->SetTitle(";Meas. Id;R;");
   hist->SetOption("E1");
   styleMarker.Copy(*hist); o[shName] = hist;

/*
   // Rate
   sprintf(hName, "grMaxRateVsMeas");
   TGraphErrors *grMaxRateVsMeas = new TGraphErrors();
   grMaxRateVsMeas->SetName(hName);
   grMaxRateVsMeas->SetMarkerStyle(kFullCircle);
   grMaxRateVsMeas->SetMarkerSize(1);
   grMaxRateVsMeas->SetMarkerColor(color);

   sprintf(hName, "hMaxRateVsMeas_%s", strPolId.c_str());
   o[hName] = new TH2F(hName, hName, 1, 14900, 15500, 1, 0, 100);
   ((TH1*) o[hName])->SetTitle(";Measurement;Target Id;");
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
   o[hName] = new TH2F(hName, hName, 1, 14900, 15500, 1, 0, 7);
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
   o[hName] = new TH2F(hName, hName, 1, 14900, 15500, 1, 0, 100);
   ((TH1*) o[hName])->SetTitle(";Measurement;Polarization, %;");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grPolarVsMeas, "p");

   sprintf(hName, "hPolarVsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH1F(hName, hName, 600, 14900, 15500);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 100);
   ((TH1*) o[hName])->SetTitle(";Fill;Polarization, %;");
   ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(color);
   ((TH1*) o[hName])->SetOption("E1");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grPolarVsMeas, "p");

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
   o[hName] = new TH2F(hName, hName, 1, 14900, 15500, 1, -0.1, 1);
   ((TH1*) o[hName])->SetTitle(";Measurement;r;");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grRVsMeasH, "p");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grRVsMeasV, "p");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grRVsMeas, "p");

   sprintf(hName, "hRVsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH1F(hName, hName, 600, 14900, 15500);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(-0.1, 1);
   ((TH1*) o[hName])->SetTitle(";Fill;r;");
   ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(color);
   ((TH1*) o[hName])->SetOption("E1");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grRVsMeas, "p");

   sprintf(hName, "hRVsFill_H_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH1F(hName, hName, 600, 14900, 15500);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(-0.1, 1);
   ((TH1*) o[hName])->SetTitle(";Fill;r;");
   ((TH1*) o[hName])->SetMarkerStyle(kFullTriangleUp);
   ((TH1*) o[hName])->SetMarkerSize(2);
   ((TH1*) o[hName])->SetMarkerColor(color-3);
   ((TH1*) o[hName])->SetOption("E1");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grRVsMeasH, "p");

   sprintf(hName, "hRVsFill_V_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH1F(hName, hName, 600, 14900, 15500);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(-0.1, 1);
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

   Int_t dlLo = 30;
   Int_t dlHi = 70;

   if (beamE == 250) {
      if (polId == kB1U || polId == kY2U) { t0Lo = -10; t0Hi = 5; }
      if (polId == kB2D || polId == kY1D) { t0Lo = -20; t0Hi = 5; }

      //if (polId == kB2D) { dlLo = 40; dlHi = 60; }

   } else if (beamE == 24) {
      if (polId == kB1U || polId == kY2U) { t0Lo = -20; t0Hi = 0; }
      if (polId == kB2D || polId == kY1D) { t0Lo = -30; t0Hi = 0; }

      //if (polId == kY1D) { dlLo = 60; dlHi = 80; }
   }

   sprintf(hName, "hT0VsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH2F(hName, hName, 1, 14900, 15500, 1, t0Lo, t0Hi);
   ((TH1*) o[hName])->SetTitle(";Measurement;t_{0};");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grT0VsMeas, "p");

   sprintf(hName, "hT0VsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH1F(hName, hName, 600, 14900, 15500);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(t0Lo, t0Hi);
   ((TH1*) o[hName])->SetTitle(";Fill;t_{0};");
   ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(color);
   ((TH1*) o[hName])->SetOption("E1");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grT0VsMeas, "p");

   // DL
   TGraphErrors *grDLVsMeas = new TGraphErrors();
   grDLVsMeas->SetName("grDLVsMeas");
   grDLVsMeas->SetMarkerStyle(kFullCircle);
   grDLVsMeas->SetMarkerSize(1);
   grDLVsMeas->SetMarkerColor(color);

   sprintf(hName, "hDLVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH2F(hName, hName, 1, 14900, 15500, 1, dlLo, dlHi);
   ((TH1*) o[hName])->SetTitle(";Measurement;Dead Layer #mug/cm^{2};");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grDLVsMeas, "p");

   sprintf(hName, "hDLVsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH1F(hName, hName, 600, 14900, 15500);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(dlLo, dlHi);
   ((TH1*) o[hName])->SetTitle(";Fill;Dead Layer #mug/cm^{2};");
   ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(color);
   ((TH1*) o[hName])->SetOption("E1");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grDLVsMeas, "p");
*/

} //}}}


/** */
void MAsymFillHists::Fill(EventConfig &rc)
{ //{{{
   Double_t runId              = rc.fMeasInfo->RUNID;
   UInt_t   fillId             = (UInt_t) runId;
   UInt_t   beamEnergy         = (UInt_t) (rc.fMeasInfo->GetBeamEnergy() + 0.5);
   Short_t  polId              = rc.fMeasInfo->fPolId;
   time_t   measStartTime       = rc.fMeasInfo->fStartTime;
   //Short_t  targetId           = rc.fMseMeasInfoX->target_id;
   //Char_t   targetOrient       = rc.fMseMeasInfoX->target_orient[0];
   Float_t  anaPower           = rc.fAnaMeasResult->A_N[1];
   Float_t  asymmetry          = rc.fAnaMeasResult->sinphi[0].P[0] * rc.fAnaMeasResult->A_N[1];
   Float_t  asymmetryErr       = rc.fAnaMeasResult->sinphi[0].P[1] * rc.fAnaMeasResult->A_N[1];
   Float_t  polar              = rc.fAnaMeasResult->sinphi[0].P[0] * 100.;
   Float_t  polarErr           = rc.fAnaMeasResult->sinphi[0].P[1] * 100.;
   Float_t  profilePolarMax    = rc.fAnaMeasResult->fProfilePolarMax.first;
   Float_t  profilePolarMaxErr = rc.fAnaMeasResult->fProfilePolarMax.second;
   Float_t  profileRatio       = rc.fAnaMeasResult->fProfilePolarR.first;
   Float_t  profileRatioErr    = rc.fAnaMeasResult->fProfilePolarR.second;
   //Float_t  max_rate           = rc.fAnaMeasResult->max_rate;
   //Float_t  tzero              = rc.fCalibrator->fChannelCalibs[0].fT0Coef;
   //Float_t  tzeroErr           = rc.fCalibrator->fChannelCalibs[0].fT0CoefErr;
   //Float_t  dl                 = rc.fCalibrator->fChannelCalibs[0].fDLWidth;
   //Float_t  dlErr              = rc.fCalibrator->fChannelCalibs[0].fDLWidthErr;

   // Set individual fill sub dirs
   DrawObjContainer        *oc;
   DrawObjContainerMapIter  isubdir;

   char dName[256];

   sprintf(dName, "%05d", fillId);

   isubdir = d.find(dName);

   //if ( isubdir == d.end()) { // if dir not found
   //   TDirectoryFile *tdir = new TDirectoryFile(dName, dName, "", fDir);
   //   oc = new MAsymSingleFillHists(tdir);
   //   d[dName] = oc;
   //   oc = d[dName];
   //} else {
   //   oc = isubdir->second;
   //}

   //oc->Fill(rc);


   // Process common histograms
   char hName[256];

   string strPolId = RunConfig::AsString((EPolarimeterId) polId);

   TGraphErrors *graphErrs = 0;
   UInt_t        graphNEntries;

   // Asymmetry
   sprintf(hName, "hAsymVsEnergy_%s", strPolId.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grAsymVsEnergy");
   graphNEntries = graphErrs->GetN();
   graphErrs->SetPoint(graphNEntries, beamEnergy, asymmetry);
   graphErrs->SetPointError(graphNEntries, 0, asymmetryErr);

   // Analyzing power
   sprintf(hName, "hAnaPowerVsEnergy_%s", strPolId.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grAnaPowerVsEnergy");
   graphNEntries = graphErrs->GetN();
   graphErrs->SetPoint(graphNEntries, beamEnergy, anaPower);
   graphErrs->SetPointError(graphNEntries, 1, 1);

   // Polarization
   sprintf(hName, "hPolarVsEnergy_%s", strPolId.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grPolarVsEnergy");
   graphNEntries = graphErrs->GetN();
   graphErrs->SetPoint(graphNEntries, beamEnergy, polar);
   graphErrs->SetPointError(graphNEntries, 0, polarErr);

   // polarization vs fill time
   char grName[256];
   sprintf(hName, "hPolarVsFillTime_%s", strPolId.c_str());
   sprintf(grName, "grPolarVsFillTime_%05d_%s", fillId, strPolId.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject(grName);

   // Each histogram of this type contains a graph for each fill
   if (!graphErrs) {
      TGraphErrors *graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grPolarVsFillTime");
      // Create new graph for this fill
      TGraphErrors *graphFill = new TGraphErrors(*graph); // Copy the default graph and its properties, style, colors, ...
      graphFill->SetName(grName);
      ((TH1*) o[hName])->GetListOfFunctions()->Add(graphFill, "p");
      graphErrs = graphFill;
   }

   if ((EBeamEnergy) beamEnergy == kBEAM_ENERGY_250) {
      graphNEntries = graphErrs->GetN();
      graphErrs->SetPoint(graphNEntries, measStartTime, polar);
      graphErrs->SetPointError(graphNEntries, 0, polarErr);
   }

   // Profiles r
   sprintf(hName, "hRVsEnergy_%s", strPolId.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grRVsEnergy");
   graphNEntries = graphErrs->GetN();
   graphErrs->SetPoint(graphNEntries, beamEnergy, profileRatio);
   graphErrs->SetPointError(graphNEntries, 0, profileRatioErr);


   // r vs fill time
   sprintf(hName, "hRVsFillTime_%s", strPolId.c_str());
   sprintf(grName, "grRVsFillTime_%05d_%s", fillId, strPolId.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject(grName);

   // Each histogram of this type contains multiple graphs for each fill
   if (!graphErrs) {
      TGraphErrors *graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grRVsFillTime");
      TGraphErrors *graphFill = new TGraphErrors(*graph); // Copy the default graph and its properties, style, colors, ...
      graphFill->SetName(grName);
      ((TH1*) o[hName])->GetListOfFunctions()->Add(graphFill, "p");
      graphErrs = graphFill;
   }

   if ((EBeamEnergy) beamEnergy == kBEAM_ENERGY_250) {
      graphNEntries = graphErrs->GetN();
      graphErrs->SetPoint(graphNEntries, measStartTime, profileRatio);
      graphErrs->SetPointError(graphNEntries, 0, profileRatioErr);
   }

/*
   //
   if (dl < 45) cout << "XXX DL: " << dl << endl;
   //

   if (gRunConfig.fBeamEnergies.find((EBeamEnergy) beamEnergy) == gRunConfig.fBeamEnergies.end())
      return;

   char hName[256];

   string strPolId = RunConfig::AsString((EPolarimeterId) polId);
   string strBeamE = RunConfig::AsString((EBeamEnergy) beamEnergy);

   TGraph       *graph = 0;

   // Rate
   sprintf(hName, "hMaxRateVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graph = (TGraph*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grMaxRateVsMeas");
   graphNEntries = graph->GetN();
   graph->SetPoint(graphNEntries, runId, max_rate);

   // Targets
   sprintf(hName, "hTargetVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());

   if (targetOrient == 'H') {
      graph = (TGraph*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grHTargetVsMeas");
   }

   if (targetOrient == 'V') {
      graph = (TGraph*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grVTargetVsMeas");
   }

   graph->SetPoint(graph->GetN(), runId, targetId);

   // Profiles r
   sprintf(hName, "hRVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());

   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grRVsMeas");
   graphNEntries = graphErrs->GetN();
   graphErrs->SetPoint(graphNEntries, runId, profileRatio);
   graphErrs->SetPointError(graphNEntries, 0, profileRatioErr);

   if (targetOrient == 'H') {
      graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grRVsMeasH");
   }

   if (targetOrient == 'V') {
      graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grRVsMeasV");
   }

   graphNEntries = graphErrs->GetN();
   graphErrs->SetPoint(graphNEntries, runId, profileRatio);
   graphErrs->SetPointError(graphNEntries, 0, profileRatioErr);

   // t0
   sprintf(hName, "hT0VsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grT0VsMeas");

   graphNEntries = graphErrs->GetN();
   
   graphErrs->SetPoint(graphNEntries, runId, tzero);
   graphErrs->SetPointError(graphNEntries, 0, tzeroErr);

   // Dead layer
   sprintf(hName, "hDLVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grDLVsMeas");

   graphNEntries = graphErrs->GetN();
   
   graphErrs->SetPoint(graphNEntries, runId, dl);
   graphErrs->SetPointError(graphNEntries, 0, dlErr);
*/
} //}}}


/** */
void MAsymFillHists::PostFill()
{ //{{{
   char hName[256];

   for (UInt_t i=0; i!=N_POLARIMETERS; i++) {

      string strPolId = RunConfig::AsString((EPolarimeterId) i);

      TGraphErrors *graph;

      // Analyzing power
      sprintf(hName, "hAnaPowerVsEnergyBinned_%s", strPolId.c_str());
      graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grAnaPowerVsEnergy");

      utils::BinGraph(graph, (TH1*) o[hName]);

      ((TH1*) o[hName])->GetListOfFunctions()->Remove(graph);

      // Profiles r
      sprintf(hName, "hRVsEnergyBinned_%s", strPolId.c_str());
      graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grRVsEnergy");

      //utils::RemoveOutliers(graph, 2, 3);
      utils::BinGraph(graph, (TH1*) o[hName]);

      ((TH1*) o[hName])->GetListOfFunctions()->Remove(graph);

      TF1 *funcRVsEnergy = new TF1("funcRVsEnergy", "[0] + [1]*x");
      funcRVsEnergy->SetParameters(0, 0);
      funcRVsEnergy->SetParNames("offset", "slope");
      ((TH1*) o[hName])->Fit("funcRVsEnergy");
      delete funcRVsEnergy;

      // Polarization vs fill time
      sprintf(hName, "hPolarVsFillTime_%s", strPolId.c_str());
      TList* list = ((TH1*) o[hName])->GetListOfFunctions();

      // For each fill subtract the minimum time from all graph values
      TIter next(list);
      while ( TGraphErrors *graphErrs = (TGraphErrors*) next() ) {
         utils::SubtractMinimum(graphErrs);
      }

      // Fill a histogram with values per consequent measurement number
      utils::BinGraphsByMeasId(list, (TH1*) o["hPolarVsMeasId_" + strPolId], kTRUE);

      // Create a single graph from graphs for individual fills and fit it
      graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grPolarVsFillTime");
      //graph->Merge(list);
      TGraphErrors *graphMerged = new TGraphErrors(*graph);
      utils::MergeGraphs(graphMerged, list, kTRUE);
      ((TH1*) o[hName])->GetListOfFunctions()->Clear();
      ((TH1*) o[hName])->GetListOfFunctions()->Add(graphMerged, "p");
      //delete graphMerged;

      graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grPolarVsFillTime");

      TF1 *funcPolarVsFillTime = new TF1("funcPolarVsFillTime", "[0] + [1]*x");
      funcPolarVsFillTime->SetParNames("offset", "slope");
      graph->Fit("funcPolarVsFillTime");
      delete funcPolarVsFillTime;

      // r vs fill time
      sprintf(hName, "hRVsFillTime_%s", strPolId.c_str());
      list = ((TH1*) o[hName])->GetListOfFunctions();

      // For each fill subtract the minimum time from all graph values
      TIter nextRlist(list);
      while ( TGraphErrors *graphErrs = (TGraphErrors*) nextRlist() ) {
         utils::SubtractMinimum(graphErrs);
      }

      // Fill a histogram with values per consequent measurement number
      utils::BinGraphsByMeasId(list, (TH1*) o["hProfileRVsMeasId_" + strPolId], kFALSE);

      graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grRVsFillTime");
      //graph->Merge(list);
      graphMerged = new TGraphErrors(*graph);
      utils::MergeGraphs(graphMerged, list, kTRUE);
      ((TH1*) o[hName])->GetListOfFunctions()->Clear();
      ((TH1*) o[hName])->GetListOfFunctions()->Add(graphMerged, "p");
      //*graph = *graphMerged;
      //delete graphMerged;

      graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grRVsFillTime");

      //TF1 *funcRVsFillTime = new TF1("funcRVsFillTime", "[0] + [1]*x");
      TF1 *funcRVsFillTime = new TF1("funcRVsFillTime", "[0]");
      //funcRVsFillTime->SetParameters(0, 0);
      //funcRVsFillTime->SetParNames("offset", "slope");
      funcRVsFillTime->SetParNames("const");
      graph->Fit("funcRVsFillTime");
      delete funcRVsFillTime;


      sprintf(hName, "hRVsFillTimeBinned_%s", strPolId.c_str());

      utils::BinGraph(graph, (TH1*) o[hName]);

      TF1 *funcRVsFillTimeBinned = new TF1("funcRVsFillTimeBinned", "[0]");
      funcRVsFillTimeBinned->SetParNames("const");
      ((TH1*) o[hName])->Fit("funcRVsFillTimeBinned");
      delete funcRVsFillTimeBinned;
   }

   DrawObjContainer::PostFill();
} //}}}


/** */
void MAsymFillHists::UpdateLimits()
{ //{{{
   char hName[256];

   for (IterPolarimeterId iPolId=gRunConfig.fPolarimeters.begin(); iPolId!=gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      //for (IterBeamEnergy iBE=gRunConfig.fBeamEnergies.begin(); iBE!=gRunConfig.fBeamEnergies.end(); ++iBE)
      //{
         string strPolId = RunConfig::AsString(*iPolId);
         //string strBeamE = RunConfig::AsString(*iBE);

         sprintf(hName, "hPolarVsFillTime_%s", strPolId.c_str());
         TList* list = ((TH1*) o[hName])->GetListOfFunctions();
         TIter  next(list);

         while ( TGraphErrors *graphErrs = (TGraphErrors*) next() ) {
            utils::SubtractMinimum(graphErrs);
         }

         // Profile r
         sprintf(hName, "hRVsFillTime_%s", strPolId.c_str());
         list = ((TH1*) o[hName])->GetListOfFunctions();
         TIter nextRlist(list);

         while ( TGraphErrors *graphErrs = (TGraphErrors*) nextRlist() ) {
            utils::SubtractMinimum(graphErrs);
         }

         //((TH1*) o[hName])->GetXaxis()->SetTimeOffset(xmin, "gmt");
         //((TH1*) o[hName])->GetXaxis()->SetLimits(xmin, xmax);

         //sprintf(grName, "grPolarVsFillTime_%05d_%s", fillId, strPolId.c_str());
         //sprintf(hName, "hPolarVsTime_%s_%s", strPolId.c_str(), strBeamE.c_str());

         //((TH1*) o[hName])->GetXaxis()->SetLimits(fMinTime, fMaxTime);
      //}
   }

   DrawObjContainer::UpdateLimits();
} //}}}
