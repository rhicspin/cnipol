/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "DeadLayerCalibrator.h"

ClassImp(DeadLayerCalibrator)

using namespace std;
using namespace ROOT::Fit;

/** Default constructor. */
DeadLayerCalibrator::DeadLayerCalibrator() : Calibrator()
{
}


/** Default destructor. */
DeadLayerCalibrator::~DeadLayerCalibrator()
{
}


/** */
void DeadLayerCalibrator::Calibrate(DrawObjContainer *c)
{
   TH2F*  htemp = 0;
   TH1D*  hMeanTime = 0;
   string sSt("  ");
   string cutid = "_cut2";

   for (UShort_t i=1; i<=NSTRIP; i++) {
      //if (i != 28) continue;

      sprintf(&sSt[0], "%02d", i);

      htemp     = (TH2F*) c->d["channel"+sSt].o["hTimeVsEnergyA"+cutid+"_st"+sSt];
      hMeanTime = (TH1D*) c->d["channel"+sSt].o["hFitMeanTimeVsEnergyA"+cutid+"_st"+sSt];

      if (!htemp || !hMeanTime) {
         Error("Calibrate", "Histogram channel%02d/hTimeVsEnergyA%s_st%02d does not exist",
               i, cutid.c_str(), i);
         Error("Calibrate", "Histogram channel%02d/hFitMeanTimeVsEnergyA%s_st%02d does not exist",
               i, cutid.c_str(), i);
         continue;
      }

      if (htemp->Integral() < 1000) {
         Error("Calibrate", "Too few entries in histogram channel%02d/hTimeVsEnergyA%s_st%02d. Skipped",
               i, cutid.c_str(), i);
         continue;
      }

      ChannelCalib *chCalib;
      ChannelCalibMap::iterator iChCalib = fChannelCalibs.find(i);

      if (iChCalib != fChannelCalibs.end())  {
         chCalib = &iChCalib->second;
      } else {
         ChannelCalib newChCalib;
         fChannelCalibs[i] = newChCalib;
         chCalib = &fChannelCalibs[i];
      }

      TFitResultPtr fitres = Calibrate(htemp, hMeanTime);

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
}


/** */
void DeadLayerCalibrator::CalibrateFast(DrawObjContainer *c)
{ //{{{
   TH2F *htemp     = (TH2F*) c->d["preproc"].o["hTimeVsEnergyA"];
   TH1D *hMeanTime = (TH1D*) c->d["preproc"].o["hFitMeanTimeVsEnergyA"];

   if (!htemp || !hMeanTime) {
      Error("CalibrateFast", "Histogram preproc/hTimeVsEnergyA does not exist");
      Error("CalibrateFast", "Histogram preproc/hFitMeanTimeVsEnergyA does not exist");
      return;
   }

   if (htemp->Integral() < 1000) {
      Error("CalibrateFast", "Too few entries in histogram preproc/hTimeVsEnergyA. Skipped");
      return;
   }

   TFitResultPtr fitres = Calibrate(htemp, hMeanTime);

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
      Error("CalibrateFast", "Empty TFitResultPtr");
   }
} //}}}


/** */
TFitResultPtr DeadLayerCalibrator::Calibrate(TH1 *h, TH1D *hMeanTime)
{
   Double_t xmin = h->GetXaxis()->GetXmin();
   Double_t xmax = h->GetXaxis()->GetXmax();

   Double_t ymin = h->GetYaxis()->GetXmin();
   Double_t ymax = h->GetYaxis()->GetXmax();

   TObjArray fitResHists;

   TF1* gausFitFunc = new TF1("gausFitFunc", "gaus", ymin, ymax);

   ((TH2F*) h)->FitSlicesY(gausFitFunc, 0, -1, 0, "QNR G5", &fitResHists);

   hMeanTime->Set(((TH1D*)fitResHists[1])->GetNbinsX()+2, ((TH1D*) fitResHists[1])->GetArray());
   //hMeanTime->SetError(((TH1D*) fitResHists[1])->GetSumw2()->GetArray());
   hMeanTime->SetError(((TH1D*) fitResHists[2])->GetArray());

   TF1 *bananaFitFunc = new TF1("bananaFitFunc", DeadLayerCalibrator::BananaFitFunc, xmin, xmax, 2);

   bananaFitFunc->SetNpx(1000);

   //TF2 *bananaFitFunc2 = new TF2("bananaFitFunc2",
   //   DeadLayerCalibrator::BananaFitFunc2, xmin, xmax, ymin, ymax, 1);

   ChannelCalibMap::iterator iChCalib = fChannelCalibs.find(0);

   float meanT0     = 20;
   float meanT0_err = 5;
   float meanEm     = 130;
   float meanEm_err = 20;

   if (iChCalib != fChannelCalibs.end())  {
      meanT0     = fChannelCalibs[0].fT0Coef;
      meanT0_err = fChannelCalibs[0].fT0CoefErr;
      meanEm     = fChannelCalibs[0].fAvrgEMiss;
      meanEm_err = fChannelCalibs[0].fAvrgEMissErr;
   }

   printf("T0, T0_err, Em, Em_err: %f, %f, %f, %f\n", meanT0, meanT0_err, meanEm, meanEm_err);

   bananaFitFunc->SetParameters(meanT0, meanEm);
   bananaFitFunc->SetParLimits(0, meanT0-3*meanT0_err, meanT0+3*meanT0_err); // t0
   bananaFitFunc->SetParLimits(1, meanEm-3*meanEm_err, meanEm+3*meanEm_err);

   //bananaFitFunc->SetParameters(20, 100);
   //bananaFitFunc->SetParameters(0, 100, 0.5, 0.5, 0.5);
   //bananaFitFunc->SetParLimits(0, 5, 30); // t0
   //bananaFitFunc->SetParLimits(1, 50, 200);
   //bananaFitFunc->SetParLimits(2, -1, 1);
   //bananaFitFunc->SetParLimits(3, -1, 1);
   //bananaFitFunc->SetParLimits(4, -1, 1);

   //bananaFitFunc->SetParameters(10);
   //bananaFitFunc->SetParLimits(0, -10, 30);

   hMeanTime->Print();
   TFitResultPtr fitres = hMeanTime->Fit(bananaFitFunc, "I M E S R", "");
   //fitres->Print("V");

   //h->Print();
   //h->Fit(bananaFitFunc2, "M E", "");

   delete gausFitFunc;
   delete bananaFitFunc;
   //delete bananaFitFunc2;

   return fitres;
}


/** */
void DeadLayerCalibrator::Print(const Option_t* opt) const
{
   opt = "";

   printf("DeadLayerCalibrator:\n");
}


/**
 *
 * par[0] - dead layer
 * par[1] - t0
 *
 */
Double_t DeadLayerCalibrator::BananaFitFunc(Double_t *x, Double_t *par)
{
   Double_t e_meas      = x[0];
   Double_t t0          = par[0];
   Double_t e_miss_avrg = par[1];
   //Double_t e_miss_frac = par[1];
   //Double_t e_miss_frac  = par[2];
   //Double_t e_miss_frac2 = par[3];
   //Double_t e_miss_frac3 = par[4];

   Double_t e_kin = e_meas + e_miss_avrg;
   //Double_t e_kin = e_meas + e_miss + e_miss_frac*e_meas + e_miss_frac2*e_meas*e_meas + e_miss_frac3*e_meas*e_meas*e_meas;
   //Double_t e_kin = e_meas * (1 + e_miss_frac);
   //Double_t e_kin = e_meas / (1 - e_miss_frac);

   //if (par[0] >= 0) par0 = TMath::Abs(par[0]);

   Double_t tof;

   if (e_kin != 0.0)
      tof = TMath::Sqrt(MASS_12C/2./e_kin) / C_CMNS * CARBON_PATH_DISTANCE - t0;
   else
      tof = DBL_MAX;

   return tof;
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
