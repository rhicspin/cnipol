#include "DeadLayerCalibrator.h"


ClassImp(DeadLayerCalibrator)

using namespace std;
using namespace ROOT::Fit;


/** */
void DeadLayerCalibrator::Calibrate(DrawObjContainer *c)
{
   TH2F *htemp     = (TH2F*) c->d["preproc"]->o["hTimeVsEnergyA"];
   TH1D *hMeanTime = (TH1D*) c->d["preproc"]->o["hFitMeanTimeVsEnergyA"];

   if (!htemp || !hMeanTime) {
      Error("Calibrate", "Histogram preproc/hTimeVsEnergyA does not exist");
      Error("Calibrate", "Histogram preproc/hFitMeanTimeVsEnergyA does not exist");
      return;
   }

   if (htemp->Integral() < 1000) {
      Error("Calibrate", "Too few entries in histogram preproc/hTimeVsEnergyA. Skipped");
      return;
   }

   TFitResultPtr fitres = CalibrateOld(htemp, hMeanTime, 0, true);

   // Put results into "channel 0"
   UInt_t chId = 0;
   ChannelCalib *chCalib;
   ChannelCalibMap::iterator iChCalib = fChannelCalibs.find(chId);

   if (iChCalib != fChannelCalibs.end())  {
      chCalib = &iChCalib->second;
   } else {
      ChannelCalib newChCalib;
      fChannelCalibs[chId] = newChCalib;
      chCalib = &fChannelCalibs[chId];
   }

   if (fitres.Get()) {
      chCalib->fBananaChi2Ndf = fitres->Ndf() > 0 ? fitres->Chi2()/fitres->Ndf() : -1;
      chCalib->fT0Coef        = fitres->Value(0);
      chCalib->fT0CoefErr     = fitres->FitResult::Error(0);
      chCalib->fAvrgEMiss     = fitres->Value(1);
      chCalib->fAvrgEMissErr  = fitres->FitResult::Error(1);
   } else {
      Error("Calibrate", "Empty TFitResultPtr");
   }
}


/** */
TFitResultPtr DeadLayerCalibrator::CalibrateOld(TH1 *h, TH1D *hMeanTime, UShort_t chId, Bool_t wideLimits)
{
   Double_t xmin = h->GetXaxis()->GetXmin();
   // Energy dependent fit function fails when E = 0
   xmin = xmin == 0 ? 1 : xmin;
   Double_t xmax = h->GetXaxis()->GetXmax();

   Double_t ymin = h->GetYaxis()->GetXmin();
   Double_t ymax = h->GetYaxis()->GetXmax();

   TObjArray fitResHists;

   TF1* gausFitFunc = new TF1("gausFitFunc", "gaus", ymin, ymax);

   ((TH2F*) h)->FitSlicesY(gausFitFunc, 0, -1, 0, "QNR G5", &fitResHists);

   hMeanTime->Set(((TH1D*)fitResHists[1])->GetNbinsX()+2, ((TH1D*) fitResHists[1])->GetArray());
   hMeanTime->SetError(((TH1D*) fitResHists[1])->GetSumw2()->GetArray());

   for (int i=1; i<=hMeanTime->GetNbinsX(); i++) {

      if (hMeanTime->GetBinContent(i) > 0 && hMeanTime->GetBinError(i) < 0.20)
         hMeanTime->SetBinError(i, 0.20);   // works for blue2
   }

   TF1 *bananaFitFunc = new TF1("bananaFitFunc", DeadLayerCalibrator::BananaFitFunc, xmin, xmax, 2);

   bananaFitFunc->SetNpx(1000);

   // Set expected values and limits
   float meanT0      = 20;
   float meanEm      = 100;

   // All channels are combined in the 0-th calib channel
   // Use these values as expected in the fit
   ChannelCalibMap::iterator iChCalib = fChannelCalibs.find(0);

   if (iChCalib != fChannelCalibs.end())  {
      meanT0     = fChannelCalibs[0].fT0Coef;
      meanEm     = fChannelCalibs[0].fAvrgEMiss;
   }

   float meanT0_low   = meanT0 < 0 ? 1.5*meanT0 : 0.5*meanT0;
   float meanT0_high  = meanT0 < 0 ? 0.5*meanT0 : 1.5*meanT0;
   float meanEm_low   = 0.5*meanEm;
   float meanEm_high  = 1.5*meanEm;

   if (wideLimits) {
      meanT0_low   = -30;
      meanT0_high  = 30;
      meanEm_low   = 0;
      meanEm_high  = 200;
   }

   bananaFitFunc->SetParameters(meanT0, meanEm);

   bananaFitFunc->SetParLimits(0, meanT0_low, meanT0_high);
   bananaFitFunc->SetParLimits(1, meanEm_low, meanEm_high);

   hMeanTime->Print();
   TFitResultPtr fitres = hMeanTime->Fit(bananaFitFunc, "I M E S R", "");

   delete gausFitFunc;
   delete bananaFitFunc;

   return fitres;
}


Double_t DeadLayerCalibrator::BananaFitFunc(Double_t *x, Double_t *par)
{
   Double_t e_meas      = x[0];
   Double_t t0          = par[0];
   Double_t avrgELoss = par[1];
   Double_t e_kin = e_meas + avrgELoss;

   Double_t t_meas;

   if (e_kin != 0.0)
      t_meas = TMath::Sqrt(MASS_12C/2./e_kin) / C_CMNS * CARBON_PATH_DISTANCE - t0;
   else
      t_meas = DBL_MAX;

   return t_meas;
}



Double_t DeadLayerCalibrator::BananaFitFunc2(Double_t *x, Double_t *par)
{
   Double_t t_meas = x[0];
   Double_t e_meas = x[1];
   Double_t t0     = par[0];

   Double_t e_miss = fabs(e_meas -
      MASS_12C*CARBON_PATH_DISTANCE*CARBON_PATH_DISTANCE/ 2. / (t_meas + t0)/(t_meas + t0));

   return e_miss;
}
