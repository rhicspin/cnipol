#include "DeadLayerCalibratorEDepend.h"

#include "TFitResult.h"
#include "TFitResultPtr.h"

#include "utils/utils.h"

#include "MeasInfo.h"

ClassImp(DeadLayerCalibratorEDepend)

using namespace std;
using namespace ROOT::Fit;


/**
 * This method is mainly used for data calibration
 */
void DeadLayerCalibratorEDepend::Calibrate(DrawObjContainer *c)
{
   string strChId("  ");

   // Now calibrate individual active channels
   ChannelSetConstIter iCh;
   ChannelSetConstIter iChB = gMeasInfo->fSiliconChannels.begin();
   ChannelSetConstIter iChE = gMeasInfo->fSiliconChannels.end();

   for (iCh=iChB; iCh!=iChE; ++iCh)
   {
      // Skip if channel is disabled
      if (gMeasInfo->IsDisabledChannel(*iCh)) continue;

      sprintf(&strChId[0], "%02d", *iCh);

      TH1* hTimeVsE    = (TH1*) c->d["preproc"]->o["hTimeVsEnergyA_ch"+strChId];
      TH1* hMeanTime   = (TH1*) c->d["preproc"]->o["hFitMeanTimeVsEnergyA_ch"+strChId];
      TH1* hChi2Ndf    = (TH1*) c->d["preproc"]->o["hFitChi2NdfVsEnergyA_ch"+strChId];
      TH1* hChi2NdfLog = (TH1*) c->d["preproc"]->o["hFitChi2NdfLogVsEnergyA_ch"+strChId];

      TObjArray fitResultHists;

      // Single histogram calibration is done here
      CalibrateChannel(*iCh, hTimeVsE, hMeanTime, &fitResultHists); // extract also fitResultHists

      TH1* hChi2Ndf_tmp    = (TH1*) fitResultHists.At(3);
      TH1* hChi2NdfLog_tmp = (TH1*) fitResultHists.At(4);

      if (!hChi2Ndf_tmp) continue;

      for (Int_t ib=1; ib<=hChi2Ndf_tmp->GetNbinsX(); ++ib)
      {
         Float_t chi2 = hChi2Ndf_tmp->GetBinContent(ib);
         hChi2Ndf->SetBinContent(ib, chi2);

         Float_t logChi2 = hChi2NdfLog_tmp->GetBinContent(ib);
         hChi2NdfLog->SetBinContent(ib, logChi2 );
      }

      hChi2Ndf->GetListOfFunctions()->AddAll((TList*) hChi2Ndf_tmp->GetListOfFunctions()->Clone());
      hChi2Ndf->GetListOfFunctions()->SetOwner(kTRUE);

      hChi2NdfLog->GetListOfFunctions()->AddAll((TList*) hChi2NdfLog_tmp->GetListOfFunctions()->Clone());
      hChi2NdfLog->GetListOfFunctions()->SetOwner(kTRUE);
   }

   // The fitting of all channels is over. Calculate the Mean values
   Calibrator::UpdateMeanChannel();

   ChannelCalib const& chCalibMean       = GetMeanChannel();
   ChannelCalib const& chCalibMeanOfLogs = GetMeanOfLogsChannel();

   // Check for bad fits to avoid interference with the channel disabling
   // procedure. Also exclude channels which are not in line with the Mean
   // chi2
   ChannelCalibMapIter iChCalib = fChannelCalibs.begin();

   for ( ; iChCalib != fChannelCalibs.end(); ++iChCalib)
   {
      UShort_t chId = iChCalib->first;   
      ChannelCalib &chCalib = iChCalib->second;
      Double_t chChi2NdfLog = TMath::Log(chCalib.GetBananaChi2Ndf());

      // skip the first "channel" chId=0 with the sum
      if (chId == 0) continue;

      if ( chCalib.GetFitStatus() != kDLFIT_OK ) {
         gMeasInfo->DisableChannel(chId);
         continue;
      }

      // if this t0 is 2*RMS far from mean t0
      if ( TMath::Abs(chCalib.fT0Coef - chCalibMean.fT0Coef) > 3.0*chCalibMean.fT0CoefErr)
      {
         gMeasInfo->DisableChannel(chId);
         chCalib.fFitStatus = kT0_OUTLIER;
         continue;
      }

      if ( TMath::Abs(chCalib.fDLWidth - chCalibMean.fDLWidth) > 3.0*chCalibMean.fDLWidthErr)
      {
         gMeasInfo->DisableChannel(chId);
         chCalib.fFitStatus = kDL_OUTLIER;
         continue;
      }

      if ( chChi2NdfLog > 0 && 
           chChi2NdfLog > chCalibMeanOfLogs.GetBananaChi2Ndf() + 5.0*GetRMSOfLogsBananaChi2Ndf() )
      {
         gMeasInfo->DisableChannel(chId);
         chCalib.fFitStatus = kCHI2_OUTLIER;
         continue;
      }
   }

}


/** */
void DeadLayerCalibratorEDepend::CalibrateChannel(UShort_t chId, TH1 *hTimeVsE, TH1 *hMeanTime, TObjArray* fitResultHists, Bool_t wideLimits)
{
   ChannelCalib *chCalib;

   ChannelCalibMap::iterator iChCalib = fChannelCalibs.find(chId);

   if ( iChCalib != fChannelCalibs.end() ) {
      chCalib = &iChCalib->second;
   } else {
      ChannelCalib ch;
      fChannelCalibs[chId] = ch;
      chCalib = &fChannelCalibs[chId];
   }

   if (!hTimeVsE || !hMeanTime) {
      Error("Calibrate", "Histogram (i.e. Time vs Energy) for channel %2d does not exist", chId);
      return;
   }

   if (hTimeVsE->Integral() < 2000) {
      Error("Calibrate", "Too few entries (Integral = %f) in histogram %s. Skipping calibration", hTimeVsE->Integral(), hTimeVsE->GetName());
      return;
   }

   Double_t xmin = hTimeVsE->GetXaxis()->GetXmin();
   Double_t xmax = hTimeVsE->GetXaxis()->GetXmax();

   ((TH2S*) hTimeVsE)->FitSlicesY(0, 0, -1, 0, "QNR", fitResultHists);

   // Reject points based on chi2
   TH1* hchi2       = (TH1*) fitResultHists->At(3);
   TH1* hchi2Log    = (TH1*) hchi2->Clone("hchi2Log");

   fitResultHists->Add(hchi2Log);   

   TF1* logFunc = new TF1("logFunc", "TMath::Log(x)", 0, 1e10);
   utils::Apply(hchi2Log, logFunc);

   TH1* hchi2_profy = new TH1F("p", "p", 20, hchi2Log->GetMinimum(), hchi2Log->GetMaximum());
   utils::ConvertToProfile(hchi2Log, hchi2_profy, kFALSE);
   Double_t hchi2_profy_mean = hchi2_profy->GetMean() <= 0 ? 0 : hchi2_profy->GetMean();
   Double_t hchi2_profy_rms  = hchi2_profy->GetRMS(); // was: use only half of the RMS

   delete logFunc;
   delete hchi2_profy;

   TLine* lineMean = new TLine(xmin, hchi2_profy_mean, xmax, hchi2_profy_mean);
   lineMean->SetLineWidth(2);
   lineMean->SetLineColor(kGreen);

   TLine* lineRMS  = new TLine(xmin, hchi2_profy_mean+hchi2_profy_rms, xmax, hchi2_profy_mean+hchi2_profy_rms);
   lineRMS->SetLineWidth(2);
   lineRMS->SetLineColor(kMagenta);

   hchi2Log->GetListOfFunctions()->Add(lineMean);
   hchi2Log->GetListOfFunctions()->Add(lineRMS);
   hchi2Log->GetListOfFunctions()->SetOwner(kTRUE);

   TH1* hmeans  = (TH1D*) fitResultHists->At(1);

   for (Int_t ib=1; ib<=hmeans->GetNbinsX(); ++ib)
   {
      Double_t chi2  = hchi2Log->GetBinContent(ib);

      // skip points with bad chi2
      if ( chi2 > hchi2_profy_mean + hchi2_profy_rms ) continue;

      Double_t bcont = hmeans->GetBinContent(ib);
      Double_t berr  = hmeans->GetBinError(ib);

      hMeanTime->SetBinContent(ib, bcont);
      hMeanTime->SetBinError(ib, berr);
   }

   Double_t frac = utils::GetNonEmptyFraction(hMeanTime);
   printf("Non empty bin fraction: %f\n", frac);
   
   if (frac < 0.50) {
      Error("Calibrate", "Too many bad slice fits in histogram %s. Skipping DL-T0 calibration", hMeanTime->GetName());
      gMeasInfo->DisableChannel(chId);
      return;
   }

   if (hMeanTime->GetEntries() <= 0) {
      Error("Calibrate", "Zero entries in histogram %s. Skipping DL-T0 calibration", hMeanTime->GetName());
      return;
   }

   BananaFitFunctor *bff = new BananaFitFunctor(chId);

   TF1 *bananaFitFunc = new TF1("bananaFitFunc", bff, xmin, xmax, 2, "BananaFitFunctor");

   bananaFitFunc->SetNpx(1000);

   // Set expected values and limits
   float meanT0  = 0;
   float meanDLW = 60;

   if ( fChannelCalibs[0].GetFitStatus() == kDLFIT_OK ) {
      meanT0  = fChannelCalibs[0].fT0Coef;
      meanDLW = fChannelCalibs[0].fDLWidth;
   }

   Float_t meanT0_low   = -30;
   Float_t meanT0_high  =  30;
   Float_t meanDLW_low  = 0;
   Float_t meanDLW_high = 200;

   bananaFitFunc->SetParameters(meanT0, meanDLW);
   bananaFitFunc->SetParNames("t_{0}, ns", "DL, #mug/cm^{2}");

   bananaFitFunc->SetParLimits(0, meanT0_low,  meanT0_high);
   bananaFitFunc->SetParLimits(1, meanDLW_low, meanDLW_high);

   printf("\n");
   Info("Calibrate", "Fitting histogram...");

   hMeanTime->Print();

   TFitResultPtr fitres = hMeanTime->Fit(bananaFitFunc, "E S R", "");

   delete bananaFitFunc;

   if (fitres.Get()) {

      // If something is wrong with the channel mark it accordingly
      if (fitres->FitResult::Status() != 0) {
         chCalib->fFitStatus = kDLFIT_FAIL;
         return;
      }

      chCalib->fBananaChi2Ndf = fitres->Ndf() > 0 ? fitres->Chi2()/fitres->Ndf() : -1;

      chCalib->fFitStatus     = kDLFIT_OK;
      chCalib->fT0Coef        = fitres->Value(0);
      chCalib->fT0CoefErr     = fitres->FitResult::Error(0);
      chCalib->fDLWidth       = fitres->Value(1);
      chCalib->fDLWidthErr    = fitres->FitResult::Error(1);

      // The following numbers come from run 9 (and probably earlier) analysis,
      // namely macro/KinFit.C
      chCalib->fAvrgEMiss     = 0.235 + 1.1013 * chCalib->fDLWidth +
                                0.0075 * chCalib->fDLWidth * chCalib->fDLWidth;
      chCalib->fEMeasDLCorr   = 1.0098 + 0.0036 * chCalib->fDLWidth;

   } else {
      Error("Calibrate", "Empty TFitResultPtr");
      chCalib->fFitStatus = kDLFIT_FAIL;
   }
}


RunConst DeadLayerCalibratorEDepend::sRunConst  = RunConst();
map<UShort_t, RunConst> DeadLayerCalibratorEDepend::sRunConsts = gRunConsts;

const Double_t BananaFitFunctor::cp0[4] = {-0.5174     ,  0.4172     ,  0.3610E-02 , -0.1286E-05};
const Double_t BananaFitFunctor::cp1[4] = { 1.0000     ,  0.8703E-02 ,  0.1252E-04 ,  0.6948E-07};
const Double_t BananaFitFunctor::cp2[4] = { 0.2990E-05 , -0.7937E-05 , -0.2219E-07 , -0.2877E-09};
const Double_t BananaFitFunctor::cp3[4] = {-0.8258E-08 ,  0.4031E-08 ,  0.9673E-12 ,  0.3661E-12};
const Double_t BananaFitFunctor::cp4[4] = { 0.3652E-11 , -0.8652E-12 ,  0.4059E-14 , -0.1294E-15};


BananaFitFunctor::BananaFitFunctor(UShort_t chId)
{
   fRunConst = gRunConsts[chId];
}


/** */
Double_t BananaFitFunctor::operator()(double *x, double *p)
{
   Double_t pp[5];
   Double_t x0 = x[0];
   Double_t x2 = x0*x0;

   Double_t p1 = TMath::Abs(p[1]);
   Double_t p2 = p1*p1;
   Double_t p3 = p1*p1*p1;

   pp[0] = cp0[0] + cp0[1]*p1 + cp0[2]*p2 + cp0[3]*p3;
   pp[1] = cp1[0] + cp1[1]*p1 + cp1[2]*p2 + cp1[3]*p3;
   pp[2] = cp2[0] + cp2[1]*p1 + cp2[2]*p2 + cp2[3]*p3;
   pp[3] = cp3[0] + cp3[1]*p1 + cp3[2]*p2 + cp3[3]*p3;
   pp[4] = cp4[0] + cp4[1]*p1 + cp4[2]*p2 + cp4[3]*p3;

   Double_t Ekin = pp[0] + pp[1]*x0 + pp[2]*x2 + pp[3]*x2*x0 + pp[4]*x2*x2;

   Double_t t_meas = (Ekin != 0.0) ?  fRunConst.E2T/sqrt(Ekin) - p[0] : 0.0;

   return t_meas;
}
