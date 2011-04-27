/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "DeadLayerCalibratorEDepend.h"

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
   TH2F*  htemp = 0;
   TH1D*  hMeanTime = 0;
   string sSt("  ");
   string cutid = "_cut2";

   for (UShort_t i=1; i<=NSTRIP; i++) {
      //if (i != 28) continue;

      sprintf(&sSt[0], "%02d", i);

      htemp     = (TH2F*) c->d["std"]->d["channel"+sSt]->o["hTimeVsEnergyA"+cutid+"_st"+sSt];
      hMeanTime = (TH1D*) c->d["std"]->d["channel"+sSt]->o["hFitMeanTimeVsEnergyA"+cutid+"_st"+sSt];

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

      // Set static RunConst with current channel RunConst
      sRunConst = gRunConsts[i];

      TFitResultPtr fitres = Calibrate(htemp, hMeanTime);

      if (fitres.Get()) {
         chCalib->fBananaChi2Ndf = fitres->Ndf() > 0 ? fitres->Chi2()/fitres->Ndf() : -1;
         chCalib->fT0Coef        = fitres->Value(0);
         chCalib->fT0CoefErr     = fitres->FitResult::Error(0);
         chCalib->fDLWidth       = fitres->Value(1);
         chCalib->fDLWidthErr    = fitres->FitResult::Error(1);
      } else {
         Error("Calibrate", "Empty TFitResultPtr");
      }
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
   for (mi=mb; mi!=me; mi++) {

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

         for (mi2=mb; mi2!=me; mi2++) {

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

      // the following comes from run9 analysis, see macro/KinFit.C
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
   TH2F *htemp     = (TH2F*) c->d["std"]->d["preproc"]->o["hTimeVsEnergyA"];
   TH1D *hMeanTime = (TH1D*) c->d["std"]->d["preproc"]->o["hFitMeanTimeVsEnergyA"];

   if (!htemp || !hMeanTime) {
      Error("CalibrateFast", "Histogram preproc/hTimeVsEnergyA does not exist");
      Error("CalibrateFast", "Histogram preproc/hFitMeanTimeVsEnergyA does not exist");
      return;
   }

   if (htemp->Integral() < 1000) {
      Error("CalibrateFast", "Too few entries in histogram preproc/hTimeVsEnergyA. Skipped");
      return;
   }

   TFitResultPtr fitres = Calibrate(htemp, hMeanTime, true);

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

   } else {
      Error("CalibrateFast", "Empty TFitResultPtr");
   }
} //}}}


/** */
TFitResultPtr DeadLayerCalibratorEDepend::Calibrate(TH1 *h, TH1D *hMeanTime, Bool_t wideLimits)
{ //{{{
   Double_t xmin = h->GetXaxis()->GetXmin();
   // Energy dependent fit function fails when E = 0
   //xmin = xmin == 0 ? 1 : xmin;
   Double_t xmax = h->GetXaxis()->GetXmax();

   Double_t ymin = h->GetYaxis()->GetXmin();
   Double_t ymax = h->GetYaxis()->GetXmax();

   TObjArray fitResHists;

   TF1* gausFitFunc = new TF1("gausFitFunc", "gaus", ymin, ymax);

   //((TH2F*) h)->FitSlicesY(gausFitFunc, 0, -1, 0, "QNR G5", &fitResHists);
   ((TH2F*) h)->FitSlicesY(gausFitFunc, 0, -1, 0, "QNR G2", &fitResHists);

   hMeanTime->Set(((TH1D*)fitResHists[1])->GetNbinsX()+2, ((TH1D*) fitResHists[1])->GetArray());

   Double_t *errors = ((TH1D*) fitResHists[1])->GetSumw2()->GetArray();

   if (errors)
      hMeanTime->SetError(errors);

   //hMeanTime->SetError(((TH1D*) fitResHists[2])->GetArray());

   // Set reasonable errors...
   for (int i=1; i<=hMeanTime->GetNbinsX(); i++) {

      if (hMeanTime->GetBinContent(i) > 0 && hMeanTime->GetBinError(i) < 0.3)
         hMeanTime->SetBinError(i, 0.3);
         //hMeanTime->SetBinError(i, 1);
   }

   // Remember that for this fit the second parameter is the DL width
   TF1 *bananaFitFunc = new TF1("bananaFitFunc", DeadLayerCalibratorEDepend::BananaFitFunc, xmin, xmax, 2);

   bananaFitFunc->SetNpx(1000);

   //TF2 *bananaFitFunc2 = new TF2("bananaFitFunc2",
   //   DeadLayerCalibratorEDepend::BananaFitFunc2, xmin, xmax, ymin, ymax, 1);

   // Set expected values and limits
   float meanT0      = 20;
   //float meanT0_err  = 0.1*meanT0;
   float meanDLW      = 60;
   //float meanDLW_err  = 0.1*meanDLW;

   // All channels are combined in the 0-th calib channel
   // Use these values as expected in the fit
   ChannelCalibMap::iterator iChCalib = fChannelCalibs.find(0);

   if (iChCalib != fChannelCalibs.end())  {
      meanT0     = fChannelCalibs[0].fT0Coef;
      //meanT0_err = fChannelCalibs[0].fT0CoefErr;
      meanDLW     = fChannelCalibs[0].fDLWidth;
      //meanDLW_err = fChannelCalibs[0].fDLWidthErr;
   }

   float meanT0_low   = meanT0 < 0 ? 1.5*meanT0 : 0.5*meanT0;
   float meanT0_high  = meanT0 < 0 ? 0.5*meanT0 : 1.5*meanT0;
   float meanDLW_low  = 0.5*meanDLW;
   float meanDLW_high = 1.5*meanDLW;

   if (wideLimits) {
      meanT0_low   = -30;
      meanT0_high  = 30;
      meanDLW_low  = 0;
      meanDLW_high = 200;
   }

   bananaFitFunc->SetParameters(meanT0, meanDLW);
   bananaFitFunc->SetParNames("t_0, ns", "DL, #mug/cm^{2}");
   //bananaFitFunc->SetParLimits(0, meanT0-10*meanT0_err, meanT0+10*meanT0_err); // t0
   //bananaFitFunc->SetParLimits(1, meanDLW-10*meanDLW_err, meanDLW+10*meanDLW_err);

   bananaFitFunc->SetParLimits(0, meanT0_low,  meanT0_high);
   bananaFitFunc->SetParLimits(1, meanDLW_low, meanDLW_high);

   //printf("T0, T0_err, Em, Em_err: %f, %f, %f, %f\n", meanT0, meanT0_err, meanDLW, meanDLW_err);

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
} //}}}


/** */
void DeadLayerCalibratorEDepend::Print(const Option_t* opt) const
{
   opt = "";

   printf("DeadLayerCalibratorEDepend:\n");
}


RunConst DeadLayerCalibratorEDepend::sRunConst = RunConst();


const Double_t DeadLayerCalibratorEDepend::cp0[4] = {-0.5174     , 0.4172      , 0.3610E-02  , -0.1286E-05};
const Double_t DeadLayerCalibratorEDepend::cp1[4] = {1.0000      , 0.8703E-02  , 0.1252E-04  , 0.6948E-07};
const Double_t DeadLayerCalibratorEDepend::cp2[4] = {0.2990E-05  , -0.7937E-05 , -0.2219E-07 , -0.2877E-09};
const Double_t DeadLayerCalibratorEDepend::cp3[4] = {-0.8258E-08 , 0.4031E-08  , 0.9673E-12  , 0.3661E-12};
const Double_t DeadLayerCalibratorEDepend::cp4[4] = {0.3652E-11  , -0.8652E-12 , 0.4059E-14  , -0.1294E-15};


/** */
Double_t DeadLayerCalibratorEDepend::BananaFitFunc(Double_t *x, Double_t *par)
{ //{{{
   Double_t pp[5];
   Double_t x0 = x[0];
   Double_t x2 = x0*x0;

   Double_t par0 = 0;
   //if (par[0] >= 0) par0 = TMath::Abs(par[0]);
   //if (par[1] < 0) par0 = TMath::Abs(par[1]);
   par0 = TMath::Abs(par[1]);
   Double_t p2 = par0*par0;
   Double_t p3 = par0*par0*par0;

   pp[0] = cp0[0] + cp0[1]*par0 + cp0[2]*p2 + cp0[3]*p3;
   pp[1] = cp1[0] + cp1[1]*par0 + cp1[2]*p2 + cp1[3]*p3;
   pp[2] = cp2[0] + cp2[1]*par0 + cp2[2]*p2 + cp2[3]*p3;
   pp[3] = cp3[0] + cp3[1]*par0 + cp3[2]*p2 + cp3[3]*p3;
   pp[4] = cp4[0] + cp4[1]*par0 + cp4[2]*p2 + cp4[3]*p3;

   Double_t Ekin = pp[0] + pp[1]*x0 + pp[2]*x2 + pp[3]*x2*x0 + pp[4]*x2*x2;

   //Double_t tof = (Ekin != 0.0) ?  KinConst_E2T/sqrt(Ekin) + par[1] : 0.0;
   Double_t t_meas = (Ekin != 0.0) ?  sRunConst.E2T/sqrt(Ekin) - par[0] : 0.0;

   return t_meas;
} //}}}
