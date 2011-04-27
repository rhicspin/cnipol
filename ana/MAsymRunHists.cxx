/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2F.h"

#include "utils/utils.h"

#include "MAsymRunHists.h"

//#include "RunInfo.h"
#include "MseRunInfo.h"


ClassImp(MAsymRunHists)

using namespace std;

/** Default constructor. */
MAsymRunHists::MAsymRunHists() : DrawObjContainer(), fHTargetVsRun(), fVTargetVsRun()
{
   BookHists();
}


MAsymRunHists::MAsymRunHists(TDirectory *dir) : DrawObjContainer(dir), fHTargetVsRun(), fVTargetVsRun()
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

   for (UInt_t i=0; i!=N_POLARIMETERS; i++) {
      for (IterBeamEnergy iBE=gRunConfig.fBeamEnergies.begin(); iBE!=gRunConfig.fBeamEnergies.end(); ++iBE) {
         BookHistsPolarimeter((EPolarimeterId) i, *iBE);
      }
   }

} //}}}


/** */
void MAsymRunHists::BookHistsPolarimeter(EPolarimeterId polId, EBeamEnergy beamE)
{ //{{{
   char hName[256];
   //char hTitle[256];
   string  strPolId = RunConfig::AsString(polId);
   string  strBeamE = RunConfig::AsString(beamE);
   Color_t color    = RunConfig::AsColor(polId);

   //sprintf(hTitle, "%.3f: Raw Counts per Bunch ", gRunInfo.RUNID);
   //o[hName] = new TH1F(hName, "", NBUNCH, -0.5, NBUNCH-0.5);
   //((TH2F*) o[hName])->GetXaxis()->SetTitle("Bunch Id");
   //((TH2F*) o[hName])->GetYaxis()->SetTitle("Events");
   //((TH2F*) o[hName])->SetFillColor(kGray);

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
   grVTargetVsMeas->SetMarkerColor(color+3);

   sprintf(hName, "hTargetVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH2F(hName, hName, 1, 14900, 15500, 1, 0, 7);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Fill");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Target Id");
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
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Measurement");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Polarization, %");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grPolarVsMeas, "p");

   sprintf(hName, "hPolarVsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH1F(hName, hName, 600, 14900, 15500);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 100);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Fill");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Polarization, %");
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

   sprintf(hName, "hRVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH2F(hName, hName, 1, 14900, 15500, 1, -0.1, 1);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Measurement");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("r");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grRVsMeas, "p");

   sprintf(hName, "hRVsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH1F(hName, hName, 600, 14900, 15500);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(-0.1, 1);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Fill");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("r");
   ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(color);
   ((TH1*) o[hName])->SetOption("E1");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grRVsMeas, "p");

   // t0
   TGraphErrors *grT0VsMeas = new TGraphErrors();
   grT0VsMeas->SetName("grT0VsMeas");
   grT0VsMeas->SetMarkerStyle(kFullCircle);
   grT0VsMeas->SetMarkerSize(1);
   grT0VsMeas->SetMarkerColor(color);

   sprintf(hName, "hT0VsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH2F(hName, hName, 1, 14900, 15500, 1, -20, 20);
   ((TH1*) o[hName])->SetTitle(";Measurement;t_{0};");
   //((TH1*) o[hName])->GetXaxis()->SetTitle("Measurement");
   //((TH1*) o[hName])->GetYaxis()->SetTitle("t_{0}");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grT0VsMeas, "p");

   sprintf(hName, "hT0VsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH1F(hName, hName, 600, 14900, 15500);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(-20, 20);
   ((TH1*) o[hName])->SetTitle(";Fill;t_{0};");
   ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(color);
   ((TH1*) o[hName])->SetOption("E1");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grT0VsMeas, "p");

} //}}}


/** */
void MAsymRunHists::Fill(EventConfig &rc)
{
   Double_t runId            = rc.fRunInfo->RUNID;
   UInt_t   fillId           = (UInt_t) runId;
   UInt_t   beamEnergy       = (UInt_t) (rc.fRunInfo->BeamEnergy + 0.5);
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
   Float_t  tzero            = rc.fCalibrator->fChannelCalibs[7].fT0Coef;
   Float_t  tzeroErr         = rc.fCalibrator->fChannelCalibs[7].fT0CoefErr;

   if (gRunConfig.fBeamEnergies.find((EBeamEnergy) beamEnergy) == gRunConfig.fBeamEnergies.end())
      return;

   char hName[256];

   string strPolId = RunConfig::AsString((EPolarimeterId) polId);
   string strBeamE = RunConfig::AsString((EBeamEnergy) beamEnergy);

   // Targets
   sprintf(hName, "hTargetVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());

   TGraph *graph = 0;

   if (targetOrient == 'H') {
      graph = (TGraph*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grHTargetVsMeas");
   }

   if (targetOrient == 'V') {
      graph = (TGraph*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grVTargetVsMeas");
   }

   graph->SetPoint(graph->GetN(), runId, targetId);

   TGraphErrors* graphErrs;
   UInt_t graphNEntries;

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

   // t0
   sprintf(hName, "hT0VsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grT0VsMeas");

   graphNEntries = graphErrs->GetN();
   
   graphErrs->SetPoint(graphNEntries, runId, tzero);
   graphErrs->SetPointError(graphNEntries, 0, tzeroErr);
}


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
         sprintf(hName, "hRVsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
         graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grRVsMeas");

         utils::RemoveOutliers(graph, 2, 3);
         utils::BinGraph(graph, (TH1*) o[hName]);

         ((TH1*) o[hName])->GetListOfFunctions()->Remove(graph);

         TF1 *funcRVsFill = new TF1("funcRVsFill", "[0]");
         funcRVsFill->SetParameter(0, 0.02);
         ((TH1*) o[hName])->Fit("funcRVsFill");
         delete funcRVsFill;

         // Profiles r
         //sprintf(hName, "hT0VsFill_%s_%s", strPolId.c_str(), strBeamE.c_str());
         //graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grT0VsMeas");

         ////utils::RemoveOutliers(graph, 2, 3);
         //utils::BinGraph(graph, (TH1*) o[hName]);

         //((TH1*) o[hName])->GetListOfFunctions()->Remove(graph);
      }
   }
}
