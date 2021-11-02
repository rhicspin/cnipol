#include <algorithm>
#include <math.h>

#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TGraphErrors.h"
#include "TStyle.h"

#include "utils/utils.h"
#include "utils/binary.h"

#include "AsymRoot.h"
#include "AsymGlobals.h"
#include "AnaMeasResult.h"
#include "TargetInfo.h"
#include "CnipolAsymHists.h"

#include "CnipolSpinTuneMotorHists.h"

ClassImp(CnipolSpinTuneMotorHists)

using namespace std;

/** Default constructor. */
CnipolSpinTuneMotorHists::CnipolSpinTuneMotorHists() : DrawObjContainer()
{
   BookHists();
}


CnipolSpinTuneMotorHists::CnipolSpinTuneMotorHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
}


/** Default destructor. */
CnipolSpinTuneMotorHists::~CnipolSpinTuneMotorHists()
{
}


/** */
void CnipolSpinTuneMotorHists::BookHists()
{
   fDir->cd();

   string      shName;
   //TH1        *hist;
   TAttMarker  styleMarker;

   styleMarker.SetMarkerStyle(kFullCircle);
   styleMarker.SetMarkerSize(1);
   styleMarker.SetMarkerColor(kGreen+2);

   shName = "hTimeInterval";
   o[shName] = new TH1F(shName.c_str(), shName.c_str(), 300, 0, 300);
   ((TH1*) o[shName])->SetTitle("; interval in bunch; counts");

   shName = "hChVsPhase";
   o[shName] = new TH2F(shName.c_str(), shName.c_str(), 300, 0, 300, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   ((TH1*) o[shName])->SetTitle("; Phase in bunch; Channel Id;");
   ((TH1*) o[shName])->SetOption("colz");
   
   shName = "hDetVsPhase";
   o[shName] = new TH2F(shName.c_str(), shName.c_str(), 300, 0, 300, N_DETECTORS, 0.5, N_DETECTORS+0.5);
   ((TH1*) o[shName])->SetTitle("; Phase in bunch; Detector Id;");
   ((TH1*) o[shName])->SetOption("colz");

   for(SpinStateSetIter iSS = gRunConfig.fSpinStates.begin(); iSS != gRunConfig.fSpinStates.end(); ++iSS){
     string sSS = gRunConfig.AsString(*iSS);
     //Color_t color = RunConfig::AsColor(*iSS);
     //
     shName = "hChVsPhase_" + sSS;
     o[shName] = new TH2F(shName.c_str(), shName.c_str(), 300, 0, 300, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
     ((TH1*) o[shName])->SetTitle("; Phase in bunch; Channel Id;");
     ((TH1*) o[shName])->SetOption("colz");    

     shName = "hDetVsPhase_" + sSS;
     o[shName] = new TH2F(shName.c_str(), shName.c_str(), 300, 0, 300, N_DETECTORS, 0.5, N_DETECTORS+0.5);
     ((TH1*) o[shName])->SetTitle("; Phase in bunch; Detector Id;");
     ((TH1*) o[shName])->SetOption("colz ");
   }
   
   /*
   shName = "hAsymVsPhase_X45";
   o[shName] = new TH2F(shName.c_str(), shName.c_str(), 300, 0, 300, 100, 0, 1);
   o[shName]->SetTitle("; Phase; Asymmetry;");
   o[shName]->SetOption("E1");

   shName = "hAsymVsPhase_Y45";
   o[shName] = new TH2F(shName.c_str(), shName.c_str(), 300, 0, 300, 100, 0, 1);
   o[shName]->SetTitle("; Phase; Asymmetry;");
   o[shName]->SetOption("E1");
   */
}


/** */
void CnipolSpinTuneMotorHists::Fill(ChannelEvent *ch)
{
   UChar_t  chId = ch->GetChannelId();
   UChar_t  bId  = ch->GetBunchId();
   string   sSS  = gRunConfig.AsString( gMeasInfo->GetBunchSpin(bId+1) );
   UInt_t rev = ch->GetRevolutionId();
   UShort_t detId = RunConfig::GetDetectorId(chId);
   //Double_t time = ((Double_t) ch->GetRevolutionId())/RHIC_REVOLUTION_FREQ;
   //UInt_t rev = gAsymRoot->fChannelEvent->GetRevolutionId();
   //UChar_t bx = gAsymRoot->fChannelEvent->GetBunchId();
   unsigned long long mark = 120*rev + bId;
   if(gAsymRoot->fSpinTuneMotor){
     long long phase = mark - binary(mark, gAsymRoot->fSpinTuneMotor->Array(), 0, gAsymRoot->fSpinTuneMotor->Size()-1);
     
     ((TH1*) o.find("hChVsPhase")->second) -> Fill(phase, chId);
     ((TH1*) o.find( "hChVsPhase_" + sSS)->second)->Fill(phase, chId);

     ((TH1*) o.find("hDetVsPhase")->second) -> Fill(phase, detId);
     ((TH1*) o.find("hDetVsPhase_" + sSS)->second) -> Fill(phase, detId);
   }
}



void CnipolSpinTuneMotorHists::FillDerived()
{

   Info("FillDerived()", "Called");
   if(gAsymRoot->fSpinTuneMotor){
     if(gAsymRoot->fSpinTuneMotor->Size() < 2) return;
     for(unsigned int i = 1; i < gAsymRoot->fSpinTuneMotor->Size(); i++){
       ((TH1 *) o.find("hTimeInterval")->second)->Fill(gAsymRoot->fSpinTuneMotor->At(i) - gAsymRoot->fSpinTuneMotor->At(i-1));
     }
   }
   /*
   TH2* hChVsOscillPhase  = (TH2*) o["hChVsOscillPhase"];
   TH2* hDetVsOscillPhase = (TH2*) o["hDetVsOscillPhase"];

   SpinStateSetIter iSS = gRunConfig.fSpinStates.begin();
   for ( ; iSS!=gRunConfig.fSpinStates.end(); ++iSS)
   {
      string sSS = gRunConfig.AsString(*iSS);

      TH2* hChVsOscillPhase_       = (TH2*) o["hChVsOscillPhase_"  + sSS];
      TH2* hDetVsOscillPhase_      = (TH2*) o["hDetVsOscillPhase_" + sSS];

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
         }
      }

      hChVsOscillPhase->Add(hChVsOscillPhase_);
      hDetVsOscillPhase->Add(hDetVsOscillPhase_);
   }
*/
}

void CnipolSpinTuneMotorHists::PostFill()
{
  /*
  TH2& hDetVsPhase_up   = (TH2&) *o.find("hDetVsPhase_up")->second;
  TH2& hDetVsPhase_dw = (TH2&) *o.find("hDetVsPhase_down")->second;
  TH1& hAsymVsPhase_X45 = (TH1&) *o.find("hAsymVsPhase_X45")->second;
  TH1& hAsymVsPhase_Y45 = (TH1&) *o.find("hAsymVsPhase_Y45")->second;

  Double_t LU = 0., RU = 0., LD = 0., RD = 0.;
  Int_t left = 1, right = 3;
  for(int ib = 0; ib <hDetVsPhase_up->GetNbinsX(); ib++){
    LU = hDetVsPhase_up->GetBinContent(ib+1, left);
    RU = hDetVsPhase_up->GetBinContent(ib+1, right);
    LD = hDetVsPhase_dw->GetBinContent(ib+1, left);
    RD = hDetVsPhase_dw->GetBinContent(ib+1, right);
    ValErrPair asym = gAsymCalculator.CalcAsymSqrtFormula(RU, LD, RD, LU);
  }
  */
/**
   TH2& hDetVsOscillPhase_up   = (TH2&) *o.find("hDetVsOscillPhase_up")->second;
   TH2& hDetVsOscillPhase_down = (TH2&) *o.find("hDetVsOscillPhase_down")->second;
   TH1& hAsymVsOscillPhase_X45 = (TH1&) *o.find("hAsymVsOscillPhase_X45")->second;
   TH1& hAsymVsOscillPhase_Y45 = (TH1&) *o.find("hAsymVsOscillPhase_Y45")->second;

   gAsymCalculator.CalcOscillPhaseAsymSqrtFormula(hDetVsOscillPhase_up, hDetVsOscillPhase_down, hAsymVsOscillPhase_X45, AsymCalculator::X45Dets);
   gAsymCalculator.CalcOscillPhaseAsymSqrtFormula(hDetVsOscillPhase_up, hDetVsOscillPhase_down, hAsymVsOscillPhase_Y45, AsymCalculator::Y45Dets);

   TF1 fitFunc("fitFunc", "[0] + [1]*sin(x + [2])", 0, _TWO_PI);

   hAsymVsOscillPhase_X45.Fit(&fitFunc);
   //hAsymVsOscillPhase_Y45.Fit(&fitFunc);
   fitFunc.SetParNames("Offset", "Amplitude", "Phase");

   AsymTypeSetIter iAsymType = gRunConfig.fAsymTypes.begin();
   for (; iAsymType!=gRunConfig.fAsymTypes.end(); ++iAsymType)
   {
      string sAsymType        = gRunConfig.AsString(*iAsymType);
      TGraphErrors* graphErrs;

      // Fit every asymmetry type graph for X45
      string shName = "gr" + string(hAsymVsOscillPhase_X45.GetName()) + "_" + sAsymType;
      graphErrs = (TGraphErrors*) hAsymVsOscillPhase_X45.GetListOfFunctions()->FindObject(shName.c_str());
      graphErrs->Fit(&fitFunc);

      // Add graphs for each bin to a single histograms
      TH1* hist = (TH1*) o["hAsymVsOscillPhase_X45_" + sAsymType];
      hist->GetListOfFunctions()->Add(graphErrs->Clone(), "p");

      utils::UpdateLimitsFromGraphs(hist, 2);

      // Fit every asymmetry type graph for Y45
      shName = "gr" + string(hAsymVsOscillPhase_Y45.GetName()) + "_" + sAsymType;
      graphErrs = (TGraphErrors*) hAsymVsOscillPhase_Y45.GetListOfFunctions()->FindObject(shName.c_str());
      graphErrs->Fit(&fitFunc);

      // Add graphs for each bin to a single histograms
      hist = (TH1*) o["hAsymVsOscillPhase_Y45_" + sAsymType];
      hist->GetListOfFunctions()->Add(graphErrs->Clone(), "p");

      utils::UpdateLimitsFromGraphs(hist, 2);
   }

   utils::UpdateLimitsFromGraphs(&hAsymVsOscillPhase_X45, 2);
   utils::UpdateLimitsFromGraphs(&hAsymVsOscillPhase_Y45, 2);
*/
}
