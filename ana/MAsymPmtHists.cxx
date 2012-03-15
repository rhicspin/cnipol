/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2F.h"

#include "utils/utils.h"

#include "MAsymPmtHists.h"

//#include "MeasInfo.h"
#include "MseMeasInfo.h"


ClassImp(MAsymPmtHists)

using namespace std;

/** Default constructor. */
MAsymPmtHists::MAsymPmtHists() : DrawObjContainer(), fHTargetVsRun(), fVTargetVsRun()
{
   BookHists();
}


MAsymPmtHists::MAsymPmtHists(TDirectory *dir) : DrawObjContainer(dir), fHTargetVsRun(), fVTargetVsRun()
{
   BookHists();
}


/** Default destructor. */
MAsymPmtHists::~MAsymPmtHists()
{ }


/** */
void MAsymPmtHists::BookHists(string sid)
{ //{{{
   fDir->cd();

   for (UInt_t i=0; i!=N_POLARIMETERS; i++) {
      for (IterBeamEnergy iBE=gRunConfig.fBeamEnergies.begin(); iBE!=gRunConfig.fBeamEnergies.end(); ++iBE) {
         BookHistsPolarimeter((EPolarimeterId) i, *iBE);
      }
   }
} //}}}


/** */
void MAsymPmtHists::BookHistsPolarimeter(EPolarimeterId polId, EBeamEnergy beamE)
{ //{{{
   char hName[256];
   //char hTitle[256];
   string  strPolId = RunConfig::AsString(polId);
   string  strBeamE = RunConfig::AsString(beamE);
   Color_t color    = RunConfig::AsColor(polId);

   // t0
   TGraphErrors *grPmtV1T0VsMeas = new TGraphErrors();
   grPmtV1T0VsMeas->SetName("grPmtV1T0VsMeas");
   grPmtV1T0VsMeas->SetMarkerStyle(kFullCircle);
   grPmtV1T0VsMeas->SetMarkerSize(1);
   grPmtV1T0VsMeas->SetMarkerColor(color);

   sprintf(hName, "hPmtV1T0VsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH2F(hName, hName, 1000, 0, 1, 1000, 0, 1);
   ((TH1*) o[hName])->SetTitle(";Measurement;V1 t_{0}, ns;");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grPmtV1T0VsMeas, "p");

   TGraphErrors *grPmtS1T0VsMeas = new TGraphErrors();
   grPmtS1T0VsMeas->SetName("grPmtS1T0VsMeas");
   grPmtS1T0VsMeas->SetMarkerStyle(kFullCircle);
   grPmtS1T0VsMeas->SetMarkerSize(1);
   grPmtS1T0VsMeas->SetMarkerColor(color);

   sprintf(hName, "hPmtS1T0VsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH2F(hName, hName, 1000, 0, 1, 1000, 0, 1);
   ((TH1*) o[hName])->SetTitle(";Measurement;S1 t_{0}, ns;");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grPmtS1T0VsMeas, "p");

} //}}}


/** */
void MAsymPmtHists::Fill(const EventConfig &rc)
{ //{{{
   Double_t runId            = rc.fMeasInfo->RUNID;
   //UInt_t   fillId           = (UInt_t) runId;
   UInt_t   beamEnergy       = (UInt_t) (rc.fMeasInfo->GetBeamEnergy() + 0.5);
   Short_t  polId            = rc.fMeasInfo->fPolId;
   //Short_t  targetId         = rc.fMseMeasInfoX->target_id;
   //Char_t   targetOrient     = rc.fMseMeasInfoX->target_orient[0];
   Float_t  pmtV1T0          = rc.fAnaMeasResult->fPmtV1T0    * WFD_TIME_UNIT_HALF;
   Float_t  pmtV1T0Err       = rc.fAnaMeasResult->fPmtV1T0Err * WFD_TIME_UNIT_HALF;
   Float_t  pmtS1T0          = rc.fAnaMeasResult->fPmtS1T0    * WFD_TIME_UNIT_HALF;
   Float_t  pmtS1T0Err       = rc.fAnaMeasResult->fPmtS1T0Err * WFD_TIME_UNIT_HALF;

   // XXX
   //if (polId == 3 && dl < 45) cout << "XXX DL: " << dl << endl;
   //

   if (gRunConfig.fBeamEnergies.find((EBeamEnergy) beamEnergy) == gRunConfig.fBeamEnergies.end())
      return;

   //printf("XXX: %8.3f, %3d, %f, %f, %f, %f, %f\n", runId, 
   //   beamEnergy, asymmetry, asymmetryErr, ana_power, polarization,
   //   polarizationErr);

   char hName[256];

   string strPolId = RunConfig::AsString((EPolarimeterId) polId);
   string strBeamE = RunConfig::AsString((EBeamEnergy) beamEnergy);

   TGraphErrors *graphErrs = 0;
   UInt_t        nPoints   = 0;

   // t0
   sprintf(hName, "hPmtV1T0VsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grPmtV1T0VsMeas");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runId, pmtV1T0);
   graphErrs->SetPointError(nPoints, 0, pmtV1T0Err);

   sprintf(hName, "hPmtS1T0VsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grPmtS1T0VsMeas");
   nPoints = graphErrs->GetN();
   graphErrs->SetPoint(nPoints, runId, pmtS1T0);
   graphErrs->SetPointError(nPoints, 0, pmtS1T0Err);

} //}}}


/** */
void MAsymPmtHists::PostFill()
{ //{{{

} //}}}


void MAsymPmtHists::UpdateLimits()
{

   for (IterPolarimeterId iPolId=gRunConfig.fPolarimeters.begin(); iPolId!=gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      for (IterBeamEnergy iBE=gRunConfig.fBeamEnergies.begin(); iBE!=gRunConfig.fBeamEnergies.end(); ++iBE)
      {
         string strPolId = RunConfig::AsString(*iPolId);
         string strBeamE = RunConfig::AsString(*iBE);

         char hName[256];
         Double_t xmin, ymin, xmax, ymax, xdelta, ydelta;
         Double_t yMean, yRms;
         TH1* h = 0;

         //sprintf(hName, "hPmtV1T0VsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
         h = (TH1*) o["hPmtV1T0VsMeas_"+strPolId+"_"+strBeamE];

         TGraphErrors *graph = (TGraphErrors*) h->GetListOfFunctions()->FindObject("grPmtV1T0VsMeas");

         utils::RemoveOutliers(graph, 2, 2);

         graph->ComputeRange(xmin, ymin, xmax, ymax);
         //xdelta = fabs(xmax - xmin)*0.1;
         //ydelta = fabs(ymax - ymin)*0.1;

         yMean = graph->GetMean(2);
         yRms  = graph->GetRMS(2);

         h->GetXaxis()->SetLimits(xmin, xmax);
         //h->GetYaxis()->SetLimits(ymin-ydelta, ymax+ydelta);
         h->GetYaxis()->SetLimits(yMean-3*yRms, yMean+3*yRms);
         //h->GetYaxis()->SetRangeUser(yMean-3*yRms, yMean+3*yRms);

         //
         h = (TH1*) o["hPmtS1T0VsMeas_"+strPolId+"_"+strBeamE];

         graph = (TGraphErrors*) h->GetListOfFunctions()->FindObject("grPmtS1T0VsMeas");

         utils::RemoveOutliers(graph, 2, 2);

         graph->ComputeRange(xmin, ymin, xmax, ymax);
         //xdelta = fabs(xmax - xmin)*0.1;
         //ydelta = fabs(ymax - ymin)*0.1;

         yMean = graph->GetMean(2);
         yRms  = graph->GetRMS(2);

         h->GetXaxis()->SetLimits(xmin, xmax);
         //h->GetYaxis()->SetLimits(ymin-ydelta, ymax+ydelta);
         h->GetYaxis()->SetLimits(yMean-3*yRms, yMean+3*yRms);
         //h->GetYaxis()->SetRangeUser(yMean-3*yRms, yMean+3*yRms);
      }
   }
}
