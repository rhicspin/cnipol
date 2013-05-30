/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include <algorithm>
#include <math.h>

#include "CnipolSpinStudyHists.h"

#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TGraphErrors.h"
#include "TStyle.h"

#include "utils/utils.h"

#include "AsymGlobals.h"
#include "AnaMeasResult.h"
#include "TargetInfo.h"
#include "CnipolAsymHists.h"


ClassImp(CnipolSpinStudyHists)

using namespace std;

/** Default constructor. */
CnipolSpinStudyHists::CnipolSpinStudyHists() : DrawObjContainer()
{
   BookHists();
}


CnipolSpinStudyHists::CnipolSpinStudyHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
}


/** Default destructor. */
CnipolSpinStudyHists::~CnipolSpinStudyHists()
{
}


/** */
void CnipolSpinStudyHists::BookHists()
{
   fDir->cd();

   string      shName;
   TH1        *hist;
   TAttMarker  styleMarker;

   styleMarker.SetMarkerStyle(kFullCircle);
   styleMarker.SetMarkerSize(1);
   styleMarker.SetMarkerColor(kGreen+2);

   shName = "hChVsOscillPhase";
   hist = new TH2I(shName.c_str(), shName.c_str(), 8, 0, _TWO_PI, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   hist->SetTitle("; Oscill. Phase; Channel Id;");
   hist->SetOption("colz NOIMG");
   o[shName] = hist;

   shName = "hDetVsOscillPhase";
   hist = new TH2I(shName.c_str(), shName.c_str(), 8, 0, _TWO_PI, N_DETECTORS, 0.5, N_DETECTORS+0.5);
   hist->SetTitle("; Oscill. Phase; Detector Id;");
   hist->SetOption("colz NOIMG");
   o[shName] = hist;

   shName = "hAsymVsOscillPhase_X45";
   hist = new TH1D(shName.c_str(), shName.c_str(), 8, 0, _TWO_PI);
   hist->SetTitle("; Oscill. Phase; Asymmetry;");
   hist->SetOption("E1");
   o[shName] = hist;

   shName = "hAsymVsOscillPhase_Y45";
   hist = new TH1D(shName.c_str(), shName.c_str(), 8, 0, _TWO_PI);
   hist->SetTitle("; Oscill. Phase; Asymmetry;");
   hist->SetOption("E1");
   o[shName] = hist;

   shName = "hAsymVsOscillPhase_sine_X45";
   hist = new TH1D(shName.c_str(), shName.c_str(), 8, 0, _TWO_PI);
   hist->SetTitle("; Oscill. Phase; Asymmetry;");
   hist->SetOption("E1 NOIMG");
   o[shName] = hist;

   shName = "hAsymVsOscillPhase_sine_Y45";
   hist = new TH1D(shName.c_str(), shName.c_str(), 8, 0, _TWO_PI);
   hist->SetTitle("; Oscill. Phase; Asymmetry;");
   hist->SetOption("E1 NOIMG");
   o[shName] = hist;

   SpinStateSetIter iSS = gRunConfig.fSpinStates.begin();
   for (; iSS!=gRunConfig.fSpinStates.end(); ++iSS)
   {
      string sSS = gRunConfig.AsString(*iSS);

      shName = "hChVsOscillPhase_" + sSS;
      hist = new TH2I(shName.c_str(), shName.c_str(), 8, 0, _TWO_PI, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      hist->SetTitle("; Oscill. Phase; Channel Id;");
      hist->SetOption("colz NOIMG");
      o[shName] = hist;

      shName = "hChVsOscillPhase_sine_" + sSS;
      hist = new TH2F(shName.c_str(), shName.c_str(), 8, 0, _TWO_PI, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      hist->SetTitle("; Oscill. Phase; Channel Id;");
      hist->SetOption("colz NOIMG");
      hist->Sumw2();
      o[shName] = hist;

      // Detector Id
      shName = "hDetVsOscillPhase_" + sSS;
      hist = new TH2I(shName.c_str(), shName.c_str(), 8, 0, _TWO_PI, N_DETECTORS, 0.5, N_DETECTORS+0.5);
      hist->SetTitle("; Oscill. Phase; Detector Id;");
      hist->SetOption("colz NOIMG");
      o[shName] = hist;

      // Detector Id
      shName = "hDetVsOscillPhase_sine_" + sSS;
      hist = new TH2F(shName.c_str(), shName.c_str(), 8, 0, _TWO_PI, N_DETECTORS, 0.5, N_DETECTORS+0.5);
      hist->SetTitle("; Oscill. Phase; Detector Id;");
      hist->SetOption("colz NOIMG");
      hist->Sumw2();
      o[shName] = hist;
   }
}


/** */
void CnipolSpinStudyHists::Fill(ChannelEvent *ch)
{
   UChar_t  chId = ch->GetChannelId();
   UChar_t  bId  = ch->GetBunchId() + 1;
   string   sSS  = gRunConfig.AsString( gMeasInfo->GetBunchSpin(bId) );
   //Double_t time = ((Double_t) ch->GetRevolutionId())/RHIC_REVOLUTION_FREQ;

   Double_t phase = _TWO_PI * RHIC_SPIN_FLIPPER_REV_FRAC * ((Double_t) ch->GetRevolutionId() + (bId-1)/120.);
   //Double_t phase = _TWO_PI * ((Double_t) ch->GetRevolutionId() + bId/120.)/ RHIC_SPIN_FLIPPER_REV_FRAC;
   phase = fmod(phase, _TWO_PI);

   ((TH2*) o.find("hChVsOscillPhase_" + sSS)->second) -> Fill(phase, chId);
   ((TH2*) o.find("hChVsOscillPhase_sine_" + sSS)->second) -> Fill(phase, chId, sin(phase)+1);
}


/** */
void CnipolSpinStudyHists::FillDerived()
{
   Info("FillDerived()", "Called");

   TH2* hChVsOscillPhase  = (TH2*) o["hChVsOscillPhase"];
   TH2* hDetVsOscillPhase = (TH2*) o["hDetVsOscillPhase"];

   SpinStateSetIter iSS = gRunConfig.fSpinStates.begin();
   for ( ; iSS!=gRunConfig.fSpinStates.end(); ++iSS)
   {
      string sSS = gRunConfig.AsString(*iSS);

      TH2* hChVsOscillPhase_       = (TH2*) o["hChVsOscillPhase_"  + sSS];
      TH2* hChVsOscillPhase_sine_  = (TH2*) o["hChVsOscillPhase_sine_"  + sSS];
      TH2* hDetVsOscillPhase_      = (TH2*) o["hDetVsOscillPhase_" + sSS];
      TH2* hDetVsOscillPhase_sine_ = (TH2*) o["hDetVsOscillPhase_sine_" + sSS];

      for (int iCh=1; iCh<=N_SILICON_CHANNELS; iCh++)
      {
         // Fill detector histograms from the channel ones
         // Skip disabled channels
         if (gMeasInfo->IsDisabledChannel(iCh)) continue;

         UShort_t iDet = RunConfig::GetDetectorId(iCh);

         for (int iBin=1; iBin<=hDetVsOscillPhase_->GetNbinsX(); iBin++)
         {
            Double_t bc_ch  = hChVsOscillPhase_->GetBinContent(iBin, iCh);
            Double_t bc_det = hDetVsOscillPhase_->GetBinContent(iBin, iDet);
            hDetVsOscillPhase_->SetBinContent(iBin, iDet, bc_det + bc_ch);

            bc_ch  = hChVsOscillPhase_sine_->GetBinContent(iBin, iCh);
            bc_det = hDetVsOscillPhase_sine_->GetBinContent(iBin, iDet);
            hDetVsOscillPhase_sine_->SetBinContent(iBin, iDet, bc_det + bc_ch);
         }
      }

      hChVsOscillPhase->Add(hChVsOscillPhase_);
      hDetVsOscillPhase->Add(hDetVsOscillPhase_);
   }
}


/** */
void CnipolSpinStudyHists::PostFill()
{
   TH2& hDetVsOscillPhase_up   = (TH2&) *o.find("hDetVsOscillPhase_up")->second;
   TH2& hDetVsOscillPhase_down = (TH2&) *o.find("hDetVsOscillPhase_down")->second;
   TH1& hAsymVsOscillPhase_X45 = (TH1&) *o.find("hAsymVsOscillPhase_X45")->second;
   TH1& hAsymVsOscillPhase_Y45 = (TH1&) *o.find("hAsymVsOscillPhase_Y45")->second;

   gAsymCalculator.CalcOscillPhaseAsymSqrtFormula(hDetVsOscillPhase_up, hDetVsOscillPhase_down, hAsymVsOscillPhase_X45, AsymCalculator::X45Dets);
   gAsymCalculator.CalcOscillPhaseAsymSqrtFormula(hDetVsOscillPhase_up, hDetVsOscillPhase_down, hAsymVsOscillPhase_Y45, AsymCalculator::Y45Dets);

   TF1 fitFunc("fitFunc", "[0] + [1]*sin(x + [2])", 0, _TWO_PI);

   hAsymVsOscillPhase_X45.Fit(&fitFunc);
   hAsymVsOscillPhase_Y45.Fit(&fitFunc);

   TH2& hDetVsOscillPhase_sine_up   = (TH2&) *o.find("hDetVsOscillPhase_sine_up")->second;
   TH2& hDetVsOscillPhase_sine_down = (TH2&) *o.find("hDetVsOscillPhase_sine_down")->second;
   TH1& hAsymVsOscillPhase_sine_X45 = (TH1&) *o.find("hAsymVsOscillPhase_sine_X45")->second;
   TH1& hAsymVsOscillPhase_sine_Y45 = (TH1&) *o.find("hAsymVsOscillPhase_sine_Y45")->second;

   gAsymCalculator.CalcOscillPhaseAsymSqrtFormula(hDetVsOscillPhase_sine_up, hDetVsOscillPhase_sine_down, hAsymVsOscillPhase_sine_X45, AsymCalculator::X45Dets);
   gAsymCalculator.CalcOscillPhaseAsymSqrtFormula(hDetVsOscillPhase_sine_up, hDetVsOscillPhase_sine_down, hAsymVsOscillPhase_sine_Y45, AsymCalculator::Y45Dets);
}
