/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "DeadLayerCalibratorEDepend.h"

#include "RunInfo.h"

ClassImp(DeadLayerCalibratorEDepend)

using namespace std;
using namespace ROOT::Fit;

/** Default constructor. */
DeadLayerCalibratorEDepend::DeadLayerCalibratorEDepend() : DeadLayerCalibrator()
{
}


/** Default destructor. */
DeadLayerCalibratorEDepend::~DeadLayerCalibratorEDepend()
{
}


/** */
void DeadLayerCalibratorEDepend::Calibrate(DrawObjContainer *c)
{ //{{{
   TH2F*  htemp     = 0;
   TH1D*  hMeanTime = 0;
   string sSt("  ");
   string cutid     = "_cut2";

   for (UShort_t iCh=1; iCh<=N_SILICON_CHANNELS; iCh++) {
      //if (iCh != 28) continue;

      if (gRunInfo->IsDisabledChannel(iCh)) continue;

      sprintf(&sSt[0], "%02d", iCh);

      htemp     = (TH2F*) c->d["std"]->d["channel"+sSt]->o["hTimeVsEnergyA"+cutid+"_st"+sSt];
      hMeanTime = (TH1D*) c->d["std"]->d["channel"+sSt]->o["hFitMeanTimeVsEnergyA"+cutid+"_st"+sSt];

      if (!htemp || !hMeanTime) {
         Error("Calibrate", "Histogram channel%02d/hTimeVsEnergyA%s_st%02d does not exist",
               iCh, cutid.c_str(), iCh);
         Error("Calibrate", "Histogram channel%02d/hFitMeanTimeVsEnergyA%s_st%02d does not exist",
               iCh, cutid.c_str(), iCh);
         continue;
      }

      if (htemp->Integral() < 1000) {
         Error("Calibrate", "Too few entries in histogram channel%02d/hTimeVsEnergyA%s_st%02d. Skipped",
               iCh, cutid.c_str(), iCh);
         continue;
      }

      ChannelCalib *chCalib;
      ChannelCalibMap::iterator iChCalib = fChannelCalibs.find(iCh);

      if (iChCalib != fChannelCalibs.end())  {
         chCalib = &iChCalib->second;
      } else {
         ChannelCalib newChCalib;
         fChannelCalibs[iCh] = newChCalib;
         chCalib = &fChannelCalibs[iCh];
      }

      // Set static RunConst with current channel RunConst
      //sRunConst = gRunConsts[iCh];

      TFitResultPtr fitres = Calibrate(htemp, hMeanTime, iCh, false);

      if (fitres.Get()) {
         chCalib->fBananaChi2Ndf = fitres->Ndf() > 0 ? fitres->Chi2()/fitres->Ndf() : -1;
         chCalib->fT0Coef        = fitres->Value(0);
         chCalib->fT0CoefErr     = fitres->FitResult::Error(0);
         chCalib->fDLWidth       = fitres->Value(1);
         chCalib->fDLWidthErr    = fitres->FitResult::Error(1);
      } else {
         Error("Calibrate", "Empty TFitResultPtr");
      }

      //delete fitres.Get();
   }

   PostCalibrate();
} //}}}


/** */
void DeadLayerCalibratorEDepend::PostCalibrate()
{ //{{{

   // calculate average T0 and DL per detector
   // XXX need to calculate weighted average! although with current stat
   // errors it's ok
   map<UShort_t, Float_t> detT0Coef;
   map<UShort_t, Float_t> detDLWidth;

   ChannelCalibMap::iterator mi, mi2;
   ChannelCalibMap::iterator mb  = fChannelCalibs.begin();
   ChannelCalibMap::iterator me  = fChannelCalibs.end();
    
   // This needs to be optimized! Unacceptable double loops
   for (mi=mb; mi!=me; ++mi) {

      if (mi->first <= 0) continue;

      ChannelCalib &ch = mi->second;
      short iDet = (short) floor((mi->first-1)/12);

      //printf("\niDet: %d, %d\n", iDet, mi->first);
      //printf("ch: %f, %f\n", detT0Coef[iDet], detDLWidth[iDet]);
      //ch.Print();

      // Check if the average was already found
      map<UShort_t, Float_t>::iterator iDetT0Coef = detT0Coef.find(iDet);

      if (iDetT0Coef == detT0Coef.end())  {

         detT0Coef[iDet]  = 0;
         detDLWidth[iDet] = 0;
         UShort_t nChDet  = 0;

         for (mi2=mb; mi2!=me; ++mi2) {

            if (mi2->first <= 0) continue;

            ChannelCalib &ch2 = mi2->second;
            short iDet2 = (short) floor((mi2->first-1)/12);

            if (iDet == iDet2) {
               //printf("iDet2: %d\n", iDet2);
               //ch2.Print();
               if (ch2.fT0Coef != 0 && ch2.fDLWidth != 0) {
                  detT0Coef[iDet]  += ch2.fT0Coef;
                  detDLWidth[iDet] += ch2.fDLWidth;
                  nChDet++;
               }
            }
         }

         detT0Coef[iDet]  /= nChDet;
         detDLWidth[iDet] /= nChDet;

         //printf("nChDet: %d, %f, %f\n", nChDet, detT0Coef[iDet], detDLWidth[iDet]);
      }

      // the following comes from run 9 analysis, see macro/KinFit.C
      ch.fAvrgEMiss   = 0.235 + 1.1013 * detDLWidth[iDet] +
                        0.0075 * detDLWidth[iDet] * detDLWidth[iDet];

      ch.fEMeasDLCorr = 1.0098 + 0.0036 * detDLWidth[iDet];

      // Assign average to channels with no values
      if (ch.fT0Coef == 0) ch.fT0Coef = detT0Coef[iDet];
   }
} //}}}


/** */
void DeadLayerCalibratorEDepend::CalibrateFast(DrawObjContainer *c)
{ //{{{

/*
   TH2F *htemp     = (TH2F*) c->d["preproc"]->o["hTimeVsEnergyA"];
   TH1D *hMeanTime = (TH1D*) c->d["preproc"]->o["hFitMeanTimeVsEnergyA"];

   if (!htemp || !hMeanTime) {
      Error("CalibrateFast", "Histogram preproc/hTimeVsEnergyA does not exist");
      Error("CalibrateFast", "Histogram preproc/hFitMeanTimeVsEnergyA does not exist");
      return;
   }

   if (htemp->Integral() < 1000) {
      Error("CalibrateFast", "Too few entries in histogram preproc/hTimeVsEnergyA. Skipped");
      return;
   }

   //sRunConst = gRunConsts[0];

   TFitResultPtr fitres = Calibrate(htemp, hMeanTime, 0, true);

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
      chCalib->fDLWidth       = fitres->Value(1);
      chCalib->fDLWidthErr    = fitres->FitResult::Error(1);

      // the following comes from run9 analysis, namely macro/KinFit.C
      chCalib->fAvrgEMiss     = 0.235 + 1.1013 * chCalib->fDLWidth +
                                0.0075 * chCalib->fDLWidth * chCalib->fDLWidth;
      chCalib->fEMeasDLCorr   = 1.0098 + 0.0036 * chCalib->fDLWidth;
   
      //fitres.Get()->Delete();

   } else {
      Error("CalibrateFast", "Empty TFitResultPtr");
   }
*/

   TH2F*  htemp     = 0;
   TH1D*  hMeanTime = 0;
   string strChId("  ");

   // iCh 0 is the sum of all channels
   for (UShort_t iCh=0; iCh<=N_SILICON_CHANNELS; iCh++) {

      sprintf(&strChId[0], "%02d", iCh);

      if (iCh == 0) {
         htemp     = (TH2F*) c->d["preproc"]->o["hTimeVsEnergyA"];
         hMeanTime = (TH1D*) c->d["preproc"]->o["hFitMeanTimeVsEnergyA"];
      } else {

         if (gRunInfo->IsDisabledChannel(iCh)) continue;

         htemp     = (TH2F*) c->d["preproc"]->o["hTimeVsEnergyA_ch"+strChId];
         hMeanTime = (TH1D*) c->d["preproc"]->o["hFitMeanTimeVsEnergyA_ch"+strChId];
      }

      if (!htemp || !hMeanTime) {
         Error("Calibrate", "Histogram preproc/hTimeVsEnergyA_ch%02d does not exist", iCh, iCh);
         Error("Calibrate", "Histogram preproc/hFitMeanTimeVsEnergyA_ch%02d does not exist", iCh, iCh);
         continue;
      }

      if (htemp->Integral() < 1000) {
         Error("Calibrate", "Too few entries in histogram preproc/hTimeVsEnergyA_ch%02d. Skipped", iCh, iCh);

         gRunInfo->SetDisabledChannel(iCh);

         continue;
      }

      ChannelCalib *chCalib;
      ChannelCalibMap::iterator iChCalib = fChannelCalibs.find(iCh);

      if (iChCalib != fChannelCalibs.end())  {
         chCalib = &iChCalib->second;
      } else {
         ChannelCalib newChCalib;
         fChannelCalibs[iCh] = newChCalib;
         chCalib = &fChannelCalibs[iCh];
      }

      // Set static RunConst with current channel RunConst
      //sRunConst = gRunConsts[iCh];

      TFitResultPtr fitres = Calibrate(htemp, hMeanTime, iCh, false);

      if (fitres.Get()) {
         chCalib->fBananaChi2Ndf = fitres->Ndf() > 0 ? fitres->Chi2()/fitres->Ndf() : -1;
         chCalib->fT0Coef        = fitres->Value(0);
         chCalib->fT0CoefErr     = fitres->FitResult::Error(0);
         chCalib->fDLWidth       = fitres->Value(1);
         chCalib->fDLWidthErr    = fitres->FitResult::Error(1);

         // the following comes from run9 analysis, namely macro/KinFit.C
         chCalib->fAvrgEMiss     = 0.235 + 1.1013 * chCalib->fDLWidth +
                                   0.0075 * chCalib->fDLWidth * chCalib->fDLWidth;
         chCalib->fEMeasDLCorr   = 1.0098 + 0.0036 * chCalib->fDLWidth;
      } else {
         Error("Calibrate", "Empty TFitResultPtr");
      }
   }

   //PostCalibrate();
} //}}}


/** */
TFitResultPtr DeadLayerCalibratorEDepend::Calibrate(TH1 *h, TH1D *hMeanTime, UShort_t chId, Bool_t wideLimits)
{ //{{{
   Double_t xmin = h->GetXaxis()->GetXmin();
   // Energy dependent fit function fails when E = 0
   //xmin = xmin == 0 ? 1 : xmin;
   Double_t xmax = h->GetXaxis()->GetXmax();

   Double_t ymin = h->GetYaxis()->GetXmin();
   Double_t ymax = h->GetYaxis()->GetXmax();

   TObjArray *fitResHists = new TObjArray();

   TF1* gausFitFunc = new TF1("gausFitFunc", "gaus", ymin, ymax);

   //if (wideLimits) { // This is for the fast calibration
   //   //((TH2F*) h)->FitSlicesY(gausFitFunc, 0, -1, 0, "QNR G5", &fitResHists);
   //   ((TH2F*) h)->FitSlicesY(gausFitFunc, 0, -1, 0, "QNR G2", fitResHists);
   //   //((TH2F*) h)->FitSlicesY(gausFitFunc, 0, -1, 0, "QNR", &fitResHists);
   //} else { // In case of the regular channel calibration
      //((TH2F*) h)->FitSlicesY(gausFitFunc, 0, -1, 0, "QNR G1", &fitResHists);
      ((TH2F*) h)->FitSlicesY(0, 0, -1, 0, "QNR", fitResHists);
   //}

   delete gausFitFunc;

   //hMeanTime->Set(((TH1D*)fitResHists[1])->GetNbinsX()+2, ((TH1D*) fitResHists[1])->GetArray());
   //hMeanTime->Set(((TH1D*)fitResHists[1])->GetNbinsX(), ((TH1D*) fitResHists[1])->GetArray());
   // Copy 
   TH1* hmeans  = (TH1D*) fitResHists->At(1);
   //TH1* hsigmas = (TH1D*) fitResHists[2];

   for (Int_t ib=1; ib<=hmeans->GetNbinsX(); ++ib) {
      Double_t bcntr = hmeans->GetBinCenter(ib);
      Double_t bcont = hmeans->GetBinContent(ib);
      Double_t berr  = hmeans->GetBinError(ib);
      //Double_t berr  = hsigmas->GetBinContent(ib);
      hMeanTime->SetBinContent(hMeanTime->FindBin(bcntr), bcont);
      hMeanTime->SetBinError(hMeanTime->FindBin(bcntr), berr);
   }

   delete fitResHists;

   //Double_t *errors = ((TH1D*) fitResHists[1])->GetSumw2()->GetArray();

   //if (errors)
   //   hMeanTime->SetError(errors);

   //hMeanTime->SetError(((TH1D*) fitResHists[2])->GetArray());

   // Set reasonable errors...
   //for (int i=1; i<=hMeanTime->GetNbinsX(); i++) {

   //   if (hMeanTime->GetBinContent(i) > 0 && hMeanTime->GetBinError(i) < 0.3)
   //      //hMeanTime->SetBinError(i, 0.3);
   //      //hMeanTime->SetBinError(i, 1);
   //}

   // Remember that for this fit the second parameter is the DL width
   //TF1 *bananaFitFunc = new TF1("bananaFitFunc", DeadLayerCalibratorEDepend::BananaFitFunc, xmin, xmax, 2);

   BananaFitFunctor *bff = new BananaFitFunctor(chId);

   TF1 *bananaFitFunc = new TF1("bananaFitFunc", bff, xmin, xmax, 2, "BananaFitFunctor");
   //TF1 *bananaFitFunc = new TF1("bananaFitFunc", bff, 0, 0, 2, "BananaFitFunctor");

   bananaFitFunc->SetNpx(1000);

   // Set expected values and limits
   float meanT0  = 0;
   float meanDLW = 60;

   // All channels are combined in the 0-th calib channel
   // Use these values as expected in the fit
   ChannelCalibMap::iterator iChCalib = fChannelCalibs.find(0);

   if ( iChCalib != fChannelCalibs.end() ) {
      meanT0  = fChannelCalibs[0].fT0Coef;
      meanDLW = fChannelCalibs[0].fDLWidth;
   }

   float meanT0_low   = meanT0 < 0 ? 1.5*meanT0 : 0.5*meanT0;
   float meanT0_high  = meanT0 < 0 ? 0.5*meanT0 : 1.5*meanT0;
   float meanDLW_low  = 0.5*meanDLW;
   float meanDLW_high = 1.5*meanDLW;

   //if (wideLimits) {
      meanT0_low   = -30;
      meanT0_high  = 30;
      meanDLW_low  = 0;
      meanDLW_high = 200;
   //}

   //bananaFitFunc->SetParameters(chId, meanT0, meanDLW);
   //bananaFitFunc->SetParNames("channel", "t_0, ns", "DL, #mug/cm^{2}");
   bananaFitFunc->SetParameters(meanT0, meanDLW);
   bananaFitFunc->SetParNames("t_{0}, ns", "DL, #mug/cm^{2}");

   //bananaFitFunc->FixParameter(0, chId);
   //bananaFitFunc->ReleaseParameter(1);
   //bananaFitFunc->ReleaseParameter(2);
   //bananaFitFunc->SetParLimits(0, chId,  chId);
   bananaFitFunc->SetParLimits(0, meanT0_low,  meanT0_high);
   bananaFitFunc->SetParLimits(1, meanDLW_low, meanDLW_high);

   //printf("T0, T0_err, Em, Em_err: %f, %f, %f, %f\n", meanT0, meanT0_err, meanDLW, meanDLW_err);

   hMeanTime->Print();
   TFitResultPtr fitres = hMeanTime->Fit(bananaFitFunc, "M E S R", "");

   //bananaFitFunc->ReleaseParameter(0);
   //bananaFitFunc->ReleaseParameter(1);
   //bananaFitFunc->ReleaseParameter(2);
   //fitres->Print("V");

   //h->Print();

   delete bananaFitFunc;

   return fitres;
} //}}}


/** */
void DeadLayerCalibratorEDepend::Print(const Option_t* opt) const
{
   opt = "";

   printf("DeadLayerCalibratorEDepend:\n");
}


RunConst DeadLayerCalibratorEDepend::sRunConst  = RunConst();
map<UShort_t, RunConst> DeadLayerCalibratorEDepend::sRunConsts = gRunConsts;

const Double_t DeadLayerCalibratorEDepend::cp0[4] = {-0.5174     , 0.4172      , 0.3610E-02  , -0.1286E-05};
const Double_t DeadLayerCalibratorEDepend::cp1[4] = {1.0000      , 0.8703E-02  , 0.1252E-04  , 0.6948E-07};
const Double_t DeadLayerCalibratorEDepend::cp2[4] = {0.2990E-05  , -0.7937E-05 , -0.2219E-07 , -0.2877E-09};
const Double_t DeadLayerCalibratorEDepend::cp3[4] = {-0.8258E-08 , 0.4031E-08  , 0.9673E-12  , 0.3661E-12};
const Double_t DeadLayerCalibratorEDepend::cp4[4] = {0.3652E-11  , -0.8652E-12 , 0.4059E-14  , -0.1294E-15};


/** */
Double_t DeadLayerCalibratorEDepend::BananaFitFunc(Double_t *x, Double_t *p)
{ //{{{
   Double_t pp[5];
   Double_t x0 = x[0];
   Double_t x2 = x0*x0;

   //UShort_t chId = (UShort_t) p[0];

   //Double_t p1 = 0;
   //if (p[0] >= 0) p1 = TMath::Abs(p[0]);
   //if (p[1] < 0) p1 = TMath::Abs(p[1]);
   Double_t p1 = TMath::Abs(p[1]);
   Double_t p2 = p1*p1;
   Double_t p3 = p1*p1*p1;

   pp[0] = cp0[0] + cp0[1]*p1 + cp0[2]*p2 + cp0[3]*p3;
   pp[1] = cp1[0] + cp1[1]*p1 + cp1[2]*p2 + cp1[3]*p3;
   pp[2] = cp2[0] + cp2[1]*p1 + cp2[2]*p2 + cp2[3]*p3;
   pp[3] = cp3[0] + cp3[1]*p1 + cp3[2]*p2 + cp3[3]*p3;
   pp[4] = cp4[0] + cp4[1]*p1 + cp4[2]*p2 + cp4[3]*p3;

   Double_t Ekin = pp[0] + pp[1]*x0 + pp[2]*x2 + pp[3]*x2*x0 + pp[4]*x2*x2;

   //Double_t tof = (Ekin != 0.0) ?  KinConst_E2T/sqrt(Ekin) + p[2] : 0.0;
   //Double_t t_meas = (Ekin != 0.0) ?  sRunConsts[chId].E2T/sqrt(Ekin) - p[1] : 0.0;
   Double_t t_meas = (Ekin != 0.0) ?  sRunConst.E2T/sqrt(Ekin) - p[0] : 0.0;

   return t_meas;
} //}}}


BananaFitFunctor::BananaFitFunctor(UShort_t chId)
{
   for (int i=0; i<=4; i++) {
      fCp0[i] = DeadLayerCalibratorEDepend::cp0[i];
      fCp1[i] = DeadLayerCalibratorEDepend::cp1[i];
      fCp2[i] = DeadLayerCalibratorEDepend::cp2[i];
      fCp3[i] = DeadLayerCalibratorEDepend::cp3[i];
      fCp4[i] = DeadLayerCalibratorEDepend::cp4[i];
   }

   fRunConst = gRunConsts[chId];
}


/** */
BananaFitFunctor::~BananaFitFunctor()
{ }


/** */
Double_t BananaFitFunctor::operator()(double *x, double *p)
{
   Double_t pp[5];
   Double_t x0 = x[0];
   Double_t x2 = x0*x0;

   //UShort_t chId = (UShort_t) p[0];

   //Double_t p1 = 0;
   //if (p[0] >= 0) p1 = TMath::Abs(p[0]);
   //if (p[1] < 0) p1 = TMath::Abs(p[1]);
   Double_t p1 = TMath::Abs(p[1]);
   Double_t p2 = p1*p1;
   Double_t p3 = p1*p1*p1;

   pp[0] = fCp0[0] + fCp0[1]*p1 + fCp0[2]*p2 + fCp0[3]*p3;
   pp[1] = fCp1[0] + fCp1[1]*p1 + fCp1[2]*p2 + fCp1[3]*p3;
   pp[2] = fCp2[0] + fCp2[1]*p1 + fCp2[2]*p2 + fCp2[3]*p3;
   pp[3] = fCp3[0] + fCp3[1]*p1 + fCp3[2]*p2 + fCp3[3]*p3;
   pp[4] = fCp4[0] + fCp4[1]*p1 + fCp4[2]*p2 + fCp4[3]*p3;

   Double_t Ekin = pp[0] + pp[1]*x0 + pp[2]*x2 + pp[3]*x2*x0 + pp[4]*x2*x2;

   //Double_t tof = (Ekin != 0.0) ?  KinConst_E2T/sqrt(Ekin) + p[2] : 0.0;
   //Double_t t_meas = (Ekin != 0.0) ?  sRunConsts[chId].E2T/sqrt(Ekin) - p[1] : 0.0;
   Double_t t_meas = (Ekin != 0.0) ?  fRunConst.E2T/sqrt(Ekin) - p[0] : 0.0;

   return t_meas;
}
