/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2F.h"

#include "utils/utils.h"

#include "MAsymFillHists.h"

//#include "RunInfo.h"
#include "MseRunInfo.h"


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
      for (IterBeamEnergy iBE=gRunConfig.fBeamEnergies.begin(); iBE!=gRunConfig.fBeamEnergies.end(); ++iBE) {
         BookHistsPolarimeter((EPolarimeterId) i, *iBE);
      }
   }

   char hName[256];

   for (IterBeamEnergy iBE=gRunConfig.fBeamEnergies.begin(); iBE!=gRunConfig.fBeamEnergies.end(); ++iBE)
   {
      string  strBeamE = RunConfig::AsString(*iBE);
      Color_t color    = RunConfig::AsColor(kB1U);

      sprintf(hName, "hPolarBlueRatioVsFill_%s", strBeamE.c_str());
      o[hName] = new TH1F(hName, hName, 600, 14900, 15500);
      ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 100);
      ((TH1*) o[hName])->SetTitle(";Fill;Polarization Ratio;");
      ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
      ((TH1*) o[hName])->SetMarkerSize(1);
      ((TH1*) o[hName])->SetMarkerColor(color);
      ((TH1*) o[hName])->SetOption("E1");

      color    = RunConfig::AsColor(kY2U);

      sprintf(hName, "hPolarYellowRatioVsFill_%s", strBeamE.c_str());
      o[hName] = new TH1F(hName, hName, 600, 14900, 15500);
      ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 100);
      ((TH1*) o[hName])->SetTitle(";Fill;Polarization Ratio;");
      ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
      ((TH1*) o[hName])->SetMarkerSize(1);
      ((TH1*) o[hName])->SetMarkerColor(color);
      ((TH1*) o[hName])->SetOption("E1");
   }

} //}}}


/** */
void MAsymFillHists::BookHistsPolarimeter(EPolarimeterId polId, EBeamEnergy beamE)
{ //{{{
   char hName[256];
   //char hTitle[256];
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
   o[hName] = new TH2F(hName, hName, 1, 14900, 15500, 1, 0, 100);
   ((TH1*) o[hName])->SetTitle(";Measurement;Target Id;");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grMaxRateVsMeas, "p");
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

} //}}}


/** */
void MAsymFillHists::Fill(EventConfig &rc)
{
   Double_t runId            = rc.fRunInfo->RUNID;
   UInt_t   fillId           = (UInt_t) runId;
   UInt_t   beamEnergy       = (UInt_t) (rc.fRunInfo->GetBeamEnergy() + 0.5);
   Short_t  polId            = rc.fRunInfo->fPolId;
   Short_t  targetId         = rc.fMseRunInfoX->target_id;
   Char_t   targetOrient     = rc.fMseRunInfoX->target_orient[0];
   Float_t  ana_power        = rc.fAnaResult->A_N[1];
   Float_t  asymmetry        = rc.fAnaResult->sinphi[0].P[0] * rc.fAnaResult->A_N[1];
   Float_t  asymmetryErr     = rc.fAnaResult->sinphi[0].P[1] * rc.fAnaResult->A_N[1];
   Float_t  polarization     = rc.fAnaResult->sinphi[0].P[0] * 100.;
   Float_t  polarizationErr  = rc.fAnaResult->sinphi[0].P[1] * 100.;
   Float_t  profileRatio     = rc.fAnaResult->fIntensPolarR;
   Float_t  profileRatioErr  = rc.fAnaResult->fIntensPolarRErr;
   Float_t  max_rate         = rc.fAnaResult->max_rate;
   Float_t  tzero            = rc.fCalibrator->fChannelCalibs[0].fT0Coef;
   Float_t  tzeroErr         = rc.fCalibrator->fChannelCalibs[0].fT0CoefErr;
   Float_t  dl               = rc.fCalibrator->fChannelCalibs[0].fDLWidth;
   Float_t  dlErr            = rc.fCalibrator->fChannelCalibs[0].fDLWidthErr;

   if (gRunConfig.fBeamEnergies.find((EBeamEnergy) beamEnergy) == gRunConfig.fBeamEnergies.end())
      return;

   char hName[256];

   string strPolId = RunConfig::AsString((EPolarimeterId) polId);
   string strBeamE = RunConfig::AsString((EBeamEnergy) beamEnergy);

   TGraphErrors *graphErrs = 0;
   TGraph       *graph = 0;
   UInt_t        graphNEntries;

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

   // Polarization
   sprintf(hName, "hPolarVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grPolarVsMeas");

   graphNEntries = graphErrs->GetN();
   
   graphErrs->SetPoint(graphNEntries, runId, polarization);
   graphErrs->SetPointError(graphNEntries, 0, polarizationErr);

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
}


/** */
void MAsymFillHists::Print(const Option_t* opt) const
{ //{{{
   opt = ""; //printf("MAsymFillHists:\n");
   DrawObjContainer::Print();
} //}}}


/** */
void MAsymFillHists::Fill(ChannelEvent *ch, string sid)
{ //{{{
} //}}}


/** */
void MAsymFillHists::PostFill()
{
   char hName[256];

   //Double_t *arrHX = new Double_t[fHTargetVsRun.size()];
   //Double_t *arrHY = new Double_t[fHTargetVsRun.size()];

   //map<Double_t, UInt_t>::iterator im;
   //UInt_t i = 0;

   //for (im=fHTargetVsRun.begin(); im!=fHTargetVsRun.end(); ++im, ++i) {
   //   *(arrHX + i) = im->first;   
   //   *(arrHY + i) = im->second;   
   //}

   //Double_t *arrVX = new Double_t[fVTargetVsRun.size()];
   //Double_t *arrVY = new Double_t[fVTargetVsRun.size()];

   //for (im=fVTargetVsRun.begin(), i=0; im!=fVTargetVsRun.end(); ++im, ++i) {
   //   *(arrVX + i) = im->first;   
   //   *(arrVY + i) = im->second;   
   //}

   for (UInt_t i=0; i!=N_POLARIMETERS; i++) {

      for (IterBeamEnergy iBE=gRunConfig.fBeamEnergies.begin(); iBE!=gRunConfig.fBeamEnergies.end(); ++iBE) {

         string strPolId = RunConfig::AsString((EPolarimeterId) i);
         string strBeamE = RunConfig::AsString(*iBE);

         Float_t fillFirst = 14900, fillLast = 15500;

         switch ((EPolarimeterId) i) {
         case kB1U:
            fillFirst = 15154;
            break;
         case kY1D:
            fillFirst = 15154;
            //fillFirst = 15260;
            //fillLast  = 15420;
            break;
         case kB2D:
            fillFirst = 15154;
            break;
         case kY2U:
            fillFirst = 15154;
            break;
         }

         TGraphErrors *graph;

         // Polarization
         sprintf(hName, "hPolarVsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
         graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grPolarVsMeas");

         utils::RemoveOutliers(graph, 2, 3);
         utils::BinGraph(graph, (TH1*) o[hName]);

         ((TH1*) o[hName])->GetListOfFunctions()->Remove(graph);

         TF1 *funcPolarVsFill = new TF1("funcPolarVsFill", "[0]");
         funcPolarVsFill->SetParameter(0, 50);
         ((TH1*) o[hName])->Fit("funcPolarVsFill");
         delete funcPolarVsFill;

         // Profiles r
         // H target = vert profile
         sprintf(hName, "hRVsFill_H_%s_%s", strPolId.c_str(), strBeamE.c_str());
         graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grRVsMeasH");

         utils::RemoveOutliers(graph, 2, 3);
         utils::BinGraph(graph, (TH1*) o[hName]);

         ((TH1*) o[hName])->GetListOfFunctions()->Remove(graph);

         TF1 *funcRVsFill = new TF1("funcRVsFill", "[0]", fillFirst, fillLast);
         funcRVsFill->SetParameter(0, 0.02);
         ((TH1*) o[hName])->Fit("funcRVsFill", "R");
         delete funcRVsFill;

         // V target = horiz profile
         sprintf(hName, "hRVsFill_V_%s_%s", strPolId.c_str(), strBeamE.c_str());
         graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grRVsMeasV");

         utils::RemoveOutliers(graph, 2, 3);
         utils::BinGraph(graph, (TH1*) o[hName]);

         ((TH1*) o[hName])->GetListOfFunctions()->Remove(graph);

         funcRVsFill = new TF1("funcRVsFill", "[0]", fillFirst, fillLast);
         funcRVsFill->SetParameter(0, 0.02);
         ((TH1*) o[hName])->Fit("funcRVsFill", "R");
         delete funcRVsFill;

         // t0
         sprintf(hName, "hT0VsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
         graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grT0VsMeas");

         //utils::RemoveOutliers(graph, 2, 3);
         utils::BinGraph(graph, (TH1*) o[hName]);

         ((TH1*) o[hName])->GetListOfFunctions()->Remove(graph);

         if ((EPolarimeterId) i == kB1U || (EPolarimeterId) i == kY2U) {
            TF1 *funcT0VsFill = new TF1("funcT0VsFill", "[0] + [1]*x");
            funcT0VsFill->SetParameters(0, 0);
            funcT0VsFill->SetParNames("offset", "slope");
            ((TH1*) o[hName])->Fit("funcT0VsFill");
            delete funcT0VsFill;
         }

         // DL
         sprintf(hName, "hDLVsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
         graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grDLVsMeas");

         utils::RemoveOutliers(graph, 2, 3);
         utils::BinGraph(graph, (TH1*) o[hName]);

         ((TH1*) o[hName])->GetListOfFunctions()->Remove(graph);

         //if ((EPolarimeterId) i == kB1U || (EPolarimeterId) i == kY2U) {
         //TF1 *funcDLVsFill = new TF1("funcDLVsFill", "[0] + [1]*x", fillFirst, fillLast);
         TF1 *funcDLVsFill = new TF1("funcDLVsFill", "[0]", fillFirst, fillLast);
         funcDLVsFill->SetParameters(0, 0);
         //funcDLVsFill->SetParNames("offset", "slope");
         funcDLVsFill->SetParNames("const");
         ((TH1*) o[hName])->Fit("funcDLVsFill", "R");
         delete funcDLVsFill;
         //}
      }
   }


   for (IterBeamEnergy iBE=gRunConfig.fBeamEnergies.begin(); iBE!=gRunConfig.fBeamEnergies.end(); ++iBE) {

      string strBeamE = RunConfig::AsString(*iBE);

      // Blue
      TH1* hPolarU = (TH1*) o["hPolarVsFill_B1U_" + strBeamE];
      TH1* hPolarD = (TH1*) o["hPolarVsFill_B2D_" + strBeamE];

      ((TH1*) o["hPolarBlueRatioVsFill_" + strBeamE])->Divide(hPolarU, hPolarD);

      TF1 *funcRatioVsFill = new TF1("funcRatioVsFill", "[0]", 15154, 15500);
      funcRatioVsFill->SetParNames("const");
      ((TH1*) o["hPolarBlueRatioVsFill_" + strBeamE])->Fit("funcRatioVsFill", "R");
      delete funcRatioVsFill;

      // Yellow
      hPolarU = (TH1*) o["hPolarVsFill_Y2U_" + strBeamE];
      hPolarD = (TH1*) o["hPolarVsFill_Y1D_" + strBeamE];

      ((TH1*) o["hPolarYellowRatioVsFill_" + strBeamE])->Divide(hPolarU, hPolarD);

      funcRatioVsFill = new TF1("funcRatioVsFill", "[0]", 15220, 15500);
      funcRatioVsFill->SetParNames("const");
      ((TH1*) o["hPolarYellowRatioVsFill_" + strBeamE])->Fit("funcRatioVsFill", "R");
      delete funcRatioVsFill;
   }
}
