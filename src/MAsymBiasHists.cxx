#include "MAsymBiasHists.h"

ClassImp(MAsymBiasHists)

using namespace std;

MAsymBiasHists::MAsymBiasHists() : DrawObjContainer()
{
   BookHists();
}


MAsymBiasHists::MAsymBiasHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
}


void MAsymBiasHists::BookHists(string sid)
{
   fDir->cd();

   for (UInt_t i = 0; i < N_POLARIMETERS; i++) {
      BookHistsPolarimeter((EPolarimeterId) i);
   }
}


void MAsymBiasHists::BookHistsPolarimeter(EPolarimeterId polId)
{
   string  strPolId = RunConfig::AsString(polId);

   for(int det = 0; det < N_DETECTORS; det++) {
      TGraphErrors *g = new TGraphErrors();
      g->SetMarkerStyle(kFullCircle);
      g->SetMarkerSize(1);
      g->SetMarkerColor(RunConfig::DetAsColor(det));

      const char *hName = Form("hBiasCurrent_%s_det%i", strPolId.c_str(), det + 1);
      o[hName] = new TH2F(hName, ";Measurement;\\text{Bias current, }\\mu A;", 1, 0., 1., 1, 0., 1.);
      ((TH2F*) o[hName])->SetOption("DUMMY");
      ((TH2F*) o[hName])->GetListOfFunctions()->Add(g, "p");

      grBiasCurrent[polId][det] = g;
      hBiasCurrent[polId][det] = (TH2F*) o[hName];
   }
}


void MAsymBiasHists::Fill(const EventConfig &rc)
{
   Double_t runId = rc.fMeasInfo->RUNID;
   Short_t  polId = rc.fMeasInfo->fPolId;
   vector<double> bc = rc.fMeasInfo->GetBiasCurrents();

   for(int det = 0; det < N_DETECTORS; det++) {
      TGraphErrors *g = grBiasCurrent[polId][det];

      if (isnan(bc[det])) continue;

      int nPoints = g->GetN();
      g->SetPoint(nPoints, runId, bc[det]);
      g->SetPointError(nPoints, 0.0, 0.0);
   }
}


void MAsymBiasHists::UpdateLimits()
{
   Double_t xmin, ymin, xmax, ymax;

   for (PolarimeterIdSetIter iPolId=gRunConfig.fPolarimeters.begin(); iPolId!=gRunConfig.fPolarimeters.end(); ++iPolId) {
      string strPolId = RunConfig::AsString(*iPolId);

      for(int det = 0; det < N_DETECTORS; det++) {
         TH1* h = hBiasCurrent[*iPolId][det];
         TGraphErrors *g = grBiasCurrent[*iPolId][det];

         g->ComputeRange(xmin, ymin, xmax, ymax);
         h->GetXaxis()->SetLimits(xmin, xmax);
         h->GetYaxis()->SetLimits(-50., 0.);
      }
   }
}
