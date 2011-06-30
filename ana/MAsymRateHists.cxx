/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2F.h"

#include "utils/utils.h"

#include "MAsymRateHists.h"

//#include "RunInfo.h"
#include "MseRunInfo.h"


ClassImp(MAsymRateHists)

using namespace std;

/** Default constructor. */
MAsymRateHists::MAsymRateHists() : DrawObjContainer(), fHTargetVsRun(), fVTargetVsRun()
{
   BookHists();
}


MAsymRateHists::MAsymRateHists(TDirectory *dir) : DrawObjContainer(dir), fHTargetVsRun(), fVTargetVsRun()
{
   BookHists();
}


/** Default destructor. */
MAsymRateHists::~MAsymRateHists()
{ }


/** */
void MAsymRateHists::BookHists(string sid)
{ //{{{
   fDir->cd();

   for (UInt_t i=0; i!=N_POLARIMETERS; i++) {
      for (IterBeamEnergy iBE=gRunConfig.fBeamEnergies.begin(); iBE!=gRunConfig.fBeamEnergies.end(); ++iBE) {
         BookHistsPolarimeter((EPolarimeterId) i, *iBE);
      }
   }
} //}}}


/** */
void MAsymRateHists::BookHistsPolarimeter(EPolarimeterId polId, EBeamEnergy beamE)
{ //{{{
   char hName[256];
   //char hTitle[256];
   string  strPolId = RunConfig::AsString(polId);
   string  strBeamE = RunConfig::AsString(beamE);
   Color_t color    = RunConfig::AsColor(polId);

   // Dynamic limits
   Int_t t0Lo   = -30;
   Int_t t0Hi   = 30;
   Int_t dlLo   = 30;
   Int_t dlHi   = 70;
   Int_t hiRate = 55;

   if (beamE == 250) {
      if (polId == kB1U || polId == kY2U) { t0Lo = -10; t0Hi = 5; }
      if (polId == kB2D || polId == kY1D) { t0Lo = -20; t0Hi = 5; }

      //if (polId == kB2D) { dlLo = 40; dlHi = 60; }
      hiRate = 55;

   } else if (beamE == 24) {
      if (polId == kB1U || polId == kY2U) { t0Lo = -20; t0Hi = 0; }
      if (polId == kB2D || polId == kY1D) { t0Lo = -30; t0Hi = 0; }

      //if (polId == kY1D) { dlLo = 60; dlHi = 80; }
      hiRate = 22;
   }

   // Rate
   sprintf(hName, "grMaxRateVsMeas");
   TGraphErrors *grMaxRateVsMeas = new TGraphErrors();
   grMaxRateVsMeas->SetName(hName);
   grMaxRateVsMeas->SetMarkerStyle(kFullCircle);
   grMaxRateVsMeas->SetMarkerSize(1);
   grMaxRateVsMeas->SetMarkerColor(color);

   sprintf(hName, "hMaxRateVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH2F(hName, hName, 1, 14900, 15500, 1, 0, hiRate);
   ((TH1*) o[hName])->SetTitle(";Measurement;Max Rate, MHz;");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grMaxRateVsMeas, "p");

   // t0
   TGraphErrors *grT0VsMaxRate = new TGraphErrors();
   grT0VsMaxRate->SetName("grT0VsMaxRate");
   grT0VsMaxRate->SetMarkerStyle(kFullCircle);
   grT0VsMaxRate->SetMarkerSize(1);
   grT0VsMaxRate->SetMarkerColor(color);

   sprintf(hName, "hT0VsMaxRate_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH2F(hName, hName, 1, 0, hiRate, 1, t0Lo, t0Hi);
   ((TH1*) o[hName])->SetTitle(";Max Rate, MHz;t_{0};");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grT0VsMaxRate, "p");

   // DL
   TGraphErrors *grDLVsMaxRate = new TGraphErrors();
   grDLVsMaxRate->SetName("grDLVsMaxRate");
   grDLVsMaxRate->SetMarkerStyle(kFullCircle);
   grDLVsMaxRate->SetMarkerSize(1);
   grDLVsMaxRate->SetMarkerColor(color);

   sprintf(hName, "hDLVsMaxRate_%s_%s", strPolId.c_str(), strBeamE.c_str());
   o[hName] = new TH2F(hName, hName, 1, 0, hiRate, 1, dlLo, dlHi);
   ((TH1*) o[hName])->SetTitle(";Max Rate, MHz;Dead Layer #mug/cm^{2};");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grDLVsMaxRate, "p");

} //}}}


/** */
void MAsymRateHists::Fill(EventConfig &rc)
{ //{{{
   Double_t runId            = rc.fRunInfo->RUNID;
   UInt_t   fillId           = (UInt_t) runId;
   UInt_t   beamEnergy       = (UInt_t) (rc.fRunInfo->GetBeamEnergy() + 0.5);
   Short_t  polId            = rc.fRunInfo->fPolId;
   //Short_t  targetId         = rc.fMseRunInfoX->target_id;
   //Char_t   targetOrient     = rc.fMseRunInfoX->target_orient[0];
   //Float_t  ana_power        = rc.fAnaResult->A_N[1];
   //Float_t  asymmetry        = rc.fAnaResult->sinphi[0].P[0] * rc.fAnaResult->A_N[1];
   //Float_t  asymmetryErr     = rc.fAnaResult->sinphi[0].P[1] * rc.fAnaResult->A_N[1];
   //Float_t  polarization     = rc.fAnaResult->sinphi[0].P[0] * 100.;
   //Float_t  polarizationErr  = rc.fAnaResult->sinphi[0].P[1] * 100.;
   //Float_t  profileRatio     = rc.fAnaResult->fIntensPolarR;
   //Float_t  profileRatioErr  = rc.fAnaResult->fIntensPolarRErr;
   Float_t  max_rate         = rc.fAnaResult->max_rate;
   Float_t  tzero            = rc.fCalibrator->fChannelCalibs[0].fT0Coef;
   Float_t  tzeroErr         = rc.fCalibrator->fChannelCalibs[0].fT0CoefErr;
   Float_t  dl               = rc.fCalibrator->fChannelCalibs[0].fDLWidth;
   Float_t  dlErr            = rc.fCalibrator->fChannelCalibs[0].fDLWidthErr;

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
   TGraph       *graph = 0;

   // Rate
   sprintf(hName, "hMaxRateVsMeas_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graph = (TGraph*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grMaxRateVsMeas");
   graph->SetPoint(graph->GetN(), runId, max_rate);

   // t0
   sprintf(hName, "hT0VsMaxRate_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grT0VsMaxRate");
   graphErrs->SetPoint(graphErrs->GetN(), max_rate, tzero);
   graphErrs->SetPointError(graphErrs->GetN(), 0, tzeroErr);

   // Dead layer
   sprintf(hName, "hDLVsMaxRate_%s_%s", strPolId.c_str(), strBeamE.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grDLVsMaxRate");
   graphErrs->SetPoint(graphErrs->GetN(), max_rate, dl);
   graphErrs->SetPointError(graphErrs->GetN(), 0, dlErr);
} //}}}


/** */
void MAsymRateHists::Print(const Option_t* opt) const
{ //{{{
   opt = ""; //printf("MAsymRateHists:\n");
   DrawObjContainer::Print();
} //}}}


/** */
void MAsymRateHists::Fill(ChannelEvent *ch, string sid)
{ //{{{
} //}}}


/** */
void MAsymRateHists::PostFill()
{ //{{{

} //}}}
