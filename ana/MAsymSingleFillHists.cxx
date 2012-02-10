/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "TDirectoryFile.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2F.h"

#include "utils/utils.h"

#include "MAsymSingleFillHists.h"

//#include "MeasInfo.h"
#include "MseMeasInfo.h"


ClassImp(MAsymSingleFillHists)

using namespace std;

/** Default constructor. */
MAsymSingleFillHists::MAsymSingleFillHists() : DrawObjContainer()
{
   BookHists();
}


MAsymSingleFillHists::MAsymSingleFillHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
}


/** Default destructor. */
MAsymSingleFillHists::~MAsymSingleFillHists()
{
   //delete grHTargetVsMeas;
   //delete grVTargetVsMeas;
}


/** */
void MAsymSingleFillHists::BookHists(string sid)
{ //{{{
   fDir->cd();

   for (UInt_t i=0; i!=N_POLARIMETERS; i++) {
      BookHistsPolarimeter((EPolarimeterId) i);
   }

} //}}}


/** */
void MAsymSingleFillHists::BookHistsPolarimeter(EPolarimeterId polId)
{ //{{{
   char hName[256];
   string  strPolId = RunConfig::AsString(polId);
   //string  strBeamE = RunConfig::AsString(beamE);
   Color_t color    = RunConfig::AsColor(polId);

   // Get a unique dir name which should be also the same as the fill number
   string strDirName(fDir->GetName());

   // Asymmetry
   sprintf(hName, "grAsymVsEnergy");
   TGraphErrors *grAsymVsEnergy = new TGraphErrors();
   grAsymVsEnergy->SetName(hName);
   grAsymVsEnergy->SetMarkerStyle(kFullCircle);
   grAsymVsEnergy->SetMarkerSize(1);
   grAsymVsEnergy->SetMarkerColor(color);

   sprintf(hName, "hAsymVsEnergy_%s_%s", strDirName.c_str(), strPolId.c_str());
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

   sprintf(hName, "hAnaPowerVsEnergy_%s_%s", strDirName.c_str(), strPolId.c_str());
   o[hName] = new TH2F(hName, hName, 1, 0, 450, 1, 0.01, 0.02);
   ((TH1*) o[hName])->SetTitle(";Beam Energy;A_{N};");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grAnaPowerVsEnergy, "p");

   sprintf(hName, "hAnaPowerVsEnergyBinned_%s_%s", strDirName.c_str(), strPolId.c_str());
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

   sprintf(hName, "hPolarVsEnergy_%s_%s", strDirName.c_str(), strPolId.c_str());
   o[hName] = new TH2F(hName, hName, 1, 0, 450, 1, 0, 100);
   ((TH1*) o[hName])->SetTitle(";Beam Energy;Polarization, %;");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grPolarVsEnergy, "p");

   TGraphErrors *grPolarVsFillTime = new TGraphErrors();
   grPolarVsFillTime->SetName("grPolarVsFillTime");
   grPolarVsFillTime->SetMarkerStyle(kFullCircle);
   grPolarVsFillTime->SetMarkerSize(1);
   grPolarVsFillTime->SetMarkerColor(color);

   sprintf(hName, "hPolarVsFillTime_%s_%s", strDirName.c_str(), strPolId.c_str());
   o[hName] = new TH2F(hName, hName, 12, 0, 12*3600, 100, 20, 100);
   ((TH1*) o[hName])->SetTitle(";Time in Fill, hours;Polarization, %;");
   ((TH1*) o[hName])->GetXaxis()->SetTimeOffset(3600*6, "gmt");
   //((TH1*) o[hName])->GetXaxis()->SetTimeOffset(0, "local");
   ((TH1*) o[hName])->GetXaxis()->SetTimeDisplay(1);
   ((TH1*) o[hName])->GetXaxis()->SetTimeFormat("%H");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grPolarVsFillTime, "p");

   // Profile r
   TGraphErrors *grRVsEnergy = new TGraphErrors();
   grRVsEnergy->SetName("grRVsEnergy");
   grRVsEnergy->SetMarkerStyle(kFullCircle);
   grRVsEnergy->SetMarkerSize(1);
   grRVsEnergy->SetMarkerColor(color);

   sprintf(hName, "hRVsEnergy_%s_%s", strDirName.c_str(), strPolId.c_str());
   o[hName] = new TH2F(hName, hName, 1, 0, 450, 1, -0.1, 1);
   ((TH1*) o[hName])->SetTitle(";Beam Energy;r;");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grRVsEnergy, "p");

   sprintf(hName, "hRVsEnergyBinned_%s_%s", strDirName.c_str(), strPolId.c_str());
   o[hName] = new TH1F(hName, hName, 50, 0, 450);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(-0.1, 1);
   ((TH1*) o[hName])->SetTitle(";Beam Energy;r;");
   ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(color);
   ((TH1*) o[hName])->SetOption("E1");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grRVsEnergy, "p");

} //}}}


/** */
void MAsymSingleFillHists::Fill(EventConfig &rc)
{
   Double_t runId            = rc.fMeasInfo->RUNID;
   UInt_t   fillId           = (UInt_t) runId;
   UInt_t   beamEnergy       = (UInt_t) (rc.fMeasInfo->GetBeamEnergy() + 0.5);
   Short_t  polId            = rc.fMeasInfo->fPolId;
   time_t   runStartTime     = rc.fMeasInfo->fStartTime;
   //Short_t  targetId         = rc.fMseMeasInfoX->target_id;
   //Char_t   targetOrient     = rc.fMseMeasInfoX->target_orient[0];
   Float_t  anaPower         = rc.fAnaMeasResult->A_N[1];
   Float_t  asymmetry        = rc.fAnaMeasResult->sinphi[0].P[0] * rc.fAnaMeasResult->A_N[1];
   Float_t  asymmetryErr     = rc.fAnaMeasResult->sinphi[0].P[1] * rc.fAnaMeasResult->A_N[1];
   Float_t  polarization     = rc.fAnaMeasResult->sinphi[0].P[0] * 100.;
   Float_t  polarizationErr  = rc.fAnaMeasResult->sinphi[0].P[1] * 100.;
   Float_t  profileRatio     = rc.fAnaMeasResult->fProfilePolarR.first;
   Float_t  profileRatioErr  = rc.fAnaMeasResult->fProfilePolarR.second;
   //Float_t  max_rate         = rc.fAnaMeasResult->max_rate;
   //Float_t  tzero            = rc.fCalibrator->fChannelCalibs[0].fT0Coef;
   //Float_t  tzeroErr         = rc.fCalibrator->fChannelCalibs[0].fT0CoefErr;
   //Float_t  dl               = rc.fCalibrator->fChannelCalibs[0].fDLWidth;
   //Float_t  dlErr            = rc.fCalibrator->fChannelCalibs[0].fDLWidthErr;

   // Get a unique dir name which should be also the same as the fill number
   string strDirName = fDir->GetName();

   // Process common histograms
   char hName[256];

   string strPolId = RunConfig::AsString((EPolarimeterId) polId);

   TGraphErrors *graphErrs = 0;
   UInt_t        graphNEntries;

   // Asymmetry
   sprintf(hName, "hAsymVsEnergy_%s_%s", strDirName.c_str(), strPolId.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grAsymVsEnergy");
   graphNEntries = graphErrs->GetN();
   graphErrs->SetPoint(graphNEntries, beamEnergy, asymmetry);
   graphErrs->SetPointError(graphNEntries, 0, asymmetryErr);

   // Analyzing power
   sprintf(hName, "hAnaPowerVsEnergy_%s_%s", strDirName.c_str(), strPolId.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grAnaPowerVsEnergy");
   graphNEntries = graphErrs->GetN();
   graphErrs->SetPoint(graphNEntries, beamEnergy, anaPower);
   graphErrs->SetPointError(graphNEntries, 1, 1);

   // Polarization
   sprintf(hName, "hPolarVsEnergy_%s_%s", strDirName.c_str(), strPolId.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grPolarVsEnergy");
   graphNEntries = graphErrs->GetN();
   graphErrs->SetPoint(graphNEntries, beamEnergy, polarization);
   graphErrs->SetPointError(graphNEntries, 0, polarizationErr);

   // polarization vs fill time
   //char grName[256];
   sprintf(hName, "hPolarVsFillTime_%s_%s", strDirName.c_str(), strPolId.c_str());
   //sprintf(grName, "grPolarVsFillTime_%05d_%s", fillId, strPolId.c_str());
   //graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject(grName);
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grPolarVsFillTime");

   // Each histogram of this type contains multiple graphs for each fill
   //if (!graphErrs) {
   //   TGraphErrors *graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grPolarVsFillTime");
   //   TGraphErrors *graphFill = new TGraphErrors(*graph); // Copy the default graph and its properties, style, colors, ...
   //   graphFill->SetName(grName);
   //   ((TH1*) o[hName])->GetListOfFunctions()->Add(graphFill, "p");
   //   graphErrs = graphFill;
   //}

   if ((EBeamEnergy) beamEnergy == kBEAM_ENERGY_250) {
      graphNEntries = graphErrs->GetN();
      graphErrs->SetPoint(graphNEntries, runStartTime, polarization);
      graphErrs->SetPointError(graphNEntries, 0, polarizationErr);
   }

   // Profiles r
   sprintf(hName, "hRVsEnergy_%s_%s", strDirName.c_str(), strPolId.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grRVsEnergy");
   graphNEntries = graphErrs->GetN();
   graphErrs->SetPoint(graphNEntries, beamEnergy, profileRatio);
   graphErrs->SetPointError(graphNEntries, 0, profileRatioErr);

}


/** */
void MAsymSingleFillHists::PostFill()
{
   string strDirName = fDir->GetName();

   char hName[256];

   for (UInt_t i=0; i!=N_POLARIMETERS; i++) {

      string strPolId = RunConfig::AsString((EPolarimeterId) i);

      TGraphErrors *graph;

      // Analyzing power
      sprintf(hName, "hAnaPowerVsEnergyBinned_%s_%s", strDirName.c_str(), strPolId.c_str());
      graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grAnaPowerVsEnergy");

      utils::BinGraph(graph, (TH1*) o[hName]);

      ((TH1*) o[hName])->GetListOfFunctions()->Remove(graph);

      // Profiles r
      sprintf(hName, "hRVsEnergyBinned_%s_%s", strDirName.c_str(), strPolId.c_str());
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
      sprintf(hName, "hPolarVsFillTime_%s_%s", strDirName.c_str(), strPolId.c_str());
      graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grPolarVsFillTime");
      //TList* list = ((TH1*) o[hName])->GetListOfFunctions();
      //graph->Merge(list);

      TF1 *funcPolarVsFillTime = new TF1("funcPolarVsFillTime", "[0] + [1]*x");
      //funcPolarVsFillTime->SetParameters(0, 0);
      funcPolarVsFillTime->SetParNames("offset", "slope");
      graph->Fit("funcPolarVsFillTime");
      delete funcPolarVsFillTime;

      //((TH1*) o[hName])->GetListOfFunctions()->Remove(graph);
   }
}


/** */
void MAsymSingleFillHists::UpdateLimits()
{
   string strDirName = fDir->GetName();

   char hName[256];

   for (IterPolarimeterId iPolId=gRunConfig.fPolarimeters.begin(); iPolId!=gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      //for (IterBeamEnergy iBE=gRunConfig.fBeamEnergies.begin(); iBE!=gRunConfig.fBeamEnergies.end(); ++iBE)
      //{
         string strPolId = RunConfig::AsString(*iPolId);
         //string strBeamE = RunConfig::AsString(*iBE);

         sprintf(hName, "hPolarVsFillTime_%s_%s", strDirName.c_str(), strPolId.c_str());
         TList* list = ((TH1*) o[hName])->GetListOfFunctions();
         TIter  next(list);

         while ( TGraphErrors *graphErrs = (TGraphErrors*) next() ) {
            utils::SubtractMinimum(graphErrs);

            //Double_t xmin, ymin, xmax, ymax;
            //graphErrs->ComputeRange(xmin, ymin, xmax, ymax);
         }

         //((TH1*) o[hName])->GetXaxis()->SetTimeOffset(xmin, "gmt");
         //((TH1*) o[hName])->GetXaxis()->SetLimits(xmin, xmax);

         //sprintf(grName, "grPolarVsFillTime_%05d_%s", fillId, strPolId.c_str());
         //sprintf(hName, "hPolarVsTime_%s_%s", strPolId.c_str(), strBeamE.c_str());

         //((TH1*) o[hName])->GetXaxis()->SetLimits(fMinTime, fMaxTime);
      //}
   }
}
