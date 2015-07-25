#include "AlphaCalibrator.h"
#include "AsymHeader.h"
#include "AsymAnaInfo.h"
#include "AsymGlobals.h"
#include <fstream>

#include "TMath.h"

#include "utils/utils.h"

ClassImp(AlphaCalibrator)

using namespace std;


/** Default constructor. */
AlphaCalibrator::AlphaCalibrator() : Calibrator()
{
}


/** Default destructor. */
AlphaCalibrator::~AlphaCalibrator()
{
}


/** */
void AlphaCalibrator::Calibrate(DrawObjContainer *c)
{
   Info("Calibrate", "Called...");

   string    sCh("  ");
   TH1F     *htemp     = 0;
   TFitResultPtr fitres;
   bool     fit_gadolinium = gMeasInfo->GetAlphaSourceCount() == 2;

   for (UShort_t i = 1; i <= NSTRIP; i++) {
      if (gMeasInfo->IsDisabledChannel(i))
         continue;

      sprintf(&sCh[0], "%02d", i);

      ChannelCalib *chCalib;
      ChannelCalibMap::iterator iChCalib = fChannelCalibs.find(i);

      if (iChCalib != fChannelCalibs.end()) {
         chCalib = &iChCalib->second;
      }
      else {
         ChannelCalib newChCalib;
         fChannelCalibs[i] = newChCalib;
         chCalib = &fChannelCalibs[i];
      }

      // Amplitude
      htemp = (TH1F*) c->d["alpha"]->d["channel" + sCh]->o["hAmpltd_ch" + sCh];

      if (!htemp) {
         Error("Calibrate", "Histogram channel%02d/hAmpltd_ch%02d does not exist", i, i);
         continue;
      }

      bool gadolinium_found;
      gadolinium_found = false;
      fitres = Calibrate(htemp, fit_gadolinium, &gadolinium_found);

      if (fitres.Get()) {
         chCalib->fAmAmp = CoefExtract(fitres, kAmericium, c, i, "");
         if (fit_gadolinium && gadolinium_found) {
            chCalib->fGdAmp = CoefExtract(fitres, kGadolinium, c, i, "");
            AmGdPlot(chCalib, c, i, sCh);
         }
      }

      // Integral
      htemp = (TH1F*) c->d["alpha"]->d["channel" + sCh]->o["hIntgrl_ch" + sCh];

      if (!htemp) {
         Error("Calibrate", "Histogram channel%02d/hIntgrl_ch%02d does not exist", i, i);
         continue;
      }

      gadolinium_found = false;
      fitres = Calibrate(htemp, fit_gadolinium, &gadolinium_found);

      if (fitres.Get()) {
         chCalib->fAmInt = CoefExtract(fitres, kAmericium, c, i, "Int");
         if (fit_gadolinium && gadolinium_found) {
            chCalib->fGdInt = CoefExtract(fitres, kGadolinium, c, i, "Int");
         }
      } else {
         Error("Calibrate", "Empty TFitResultPtr");
      }
   }

   const double	STOPPING_POWER_GD = 190, STOPPING_POWER_AM = 140; // keV/(\mu m)
   c->d["alpha"]->o["hDeadLayerSize"] = new TH1F(
       SILICON_DENSITY*100 * AM_ALPHA_E * GD_ALPHA_E * ((*(TH1F*) c->d["alpha"]->o["hGdGain"]) - (*(TH1F*) c->d["alpha"]->o["hAmGain"]))
       /
       (
          GD_ALPHA_E * (*(TH1F*) c->d["alpha"]->o["hGdGain"]) * STOPPING_POWER_AM
          -
          AM_ALPHA_E * (*(TH1F*) c->d["alpha"]->o["hAmGain"]) * STOPPING_POWER_GD
       )
   );
   ((TH1F*) c->d["alpha"]->o["hDeadLayerSize"])->SetName("hDeadLayerSize");
   ((TH1F*) c->d["alpha"]->o["hDeadLayerSize"])->SetTitle("hDeadLayerSize");
   ((TH1F*) c->d["alpha"]->o["hDeadLayerSize"])->GetXaxis()->SetTitle("Channel");
   ((TH1F*) c->d["alpha"]->o["hDeadLayerSize"])->GetYaxis()->SetTitle("\\text{Dead layer size, }\\mu g/cm^2");
   ((TH1F*) c->d["alpha"]->o["hDeadLayerSize"])->SetOption("E1 GRIDX GRIDY");

   // Dead layer size value are invalid in the channels where Gd peak wasn't found
   // We now remove such values
   for(map<UShort_t, ChannelCalib>::iterator it = fChannelCalibs.begin(); it != fChannelCalibs.end(); it++)
   {
      UInt_t channelId = it->first;
      const ChannelCalib &channelCalib = it->second;

      if (channelId == 0) continue; // channel id 0 is used to store "mean calibration"
      if (channelCalib.fGdAmp.fCoef < 0)
      {
         ((TH1F*) c->d["alpha"]->o["hDeadLayerSize"])->SetBinContent(channelId, 0);
         ((TH1F*) c->d["alpha"]->o["hDeadLayerSize"])->SetBinError(channelId, 0);
      }
   }

   CalibrateBadChannels(c);
}


/** */
CalibCoefSet AlphaCalibrator::CoefExtract (
   const TFitResultPtr &fitres, ESource source_offset,
   DrawObjContainer *c, UShort_t i, std::string suffix
)
{
   // Typical value for long_name is "Intgrl", for short_name it is "I"

   CalibCoefSet result;

   float peak_energy = AM_ALPHA_E;
   std::string  name;

   if (source_offset == kAmericium)
   {
      peak_energy = AM_ALPHA_E;
      name = "Am" + suffix;
   } else if (source_offset == kGadolinium) {
      peak_energy = GD_ALPHA_E;
      name = "Gd" + suffix;
   } else {
      Error("CoefExtract", "Bad source type");
   }

   result.fPeakPos    = fitres->Value(source_offset + 1);
   result.fPeakPosErr = fitres->FitResult::Error(source_offset + 1);
   result.fCoef       = (peak_energy / result.fPeakPos) * ATTEN;
   result.fCoefErr    = result.fCoef * result.fPeakPosErr / result.fPeakPos;
   result.fChi2Ndf    = fitres->Ndf() > 0 ? fitres->Chi2() / fitres->Ndf() : -1;

   float width_value = 100 * fitres->Value(source_offset + 2) / result.fPeakPos;
   float width_error = 100 * fitres->FitResult::Error(source_offset + 2) / result.fPeakPos;
   ((TH1F*) c->d["alpha"]->o["h" + name + "GainWidth"])->SetBinContent(i, width_value);
   ((TH1F*) c->d["alpha"]->o["h" + name + "GainWidth"])->SetBinError(i, width_error);

   float gain_value = 1 / result.fCoef;
   float gain_error = gain_value * (result.fCoefErr / result.fCoef);
   ((TH1F*) c->d["alpha"]->o["h" + name + "Gain"])->SetBinContent(i, gain_value);
   ((TH1F*) c->d["alpha"]->o["h" + name + "Gain"])->SetBinError(i, gain_error);

   ((TH1F*) c->d["alpha"]->o["h" + name + "GainDisp"])->Fill(gain_value);

   return result;
}


void AlphaCalibrator::AmGdPlot(
   const ChannelCalib *chCalib, DrawObjContainer *c, int iCh, const string &sCh
)
{
   TH2F         *hAmGd = (TH2F*) c->d["alpha"]->d["channel" + sCh]->o["hAmGd_ch" + sCh];
   TGraphErrors *gAmGd = new TGraphErrors(2);
   gAmGd->SetName(string("gAmGd_ch" + sCh).c_str());
   gAmGd->SetPoint(0, AM_ALPHA_E / 1000, chCalib->fAmAmp.fPeakPos);
   gAmGd->SetPointError(0, 0, chCalib->fAmAmp.fPeakPosErr);
   gAmGd->SetPoint(1, GD_ALPHA_E / 1000, chCalib->fGdAmp.fPeakPos);
   gAmGd->SetPointError(1, 0, chCalib->fGdAmp.fPeakPosErr);
   hAmGd->GetListOfFunctions()->Add(gAmGd, "p");
   hAmGd->SetOption("DUMMY GRIDX GRIDY");
   hAmGd->SetTitle("; Energy, MeV; Peak, ADC Counts;");
   hAmGd->GetXaxis()->Set(1, 0, AM_ALPHA_E / 1000 * 1.1);
   hAmGd->GetYaxis()->Set(1, 0, chCalib->fAmAmp.fPeakPos * 1.3);

   TF1	func("func", "(x-[0])*[1]", 0, AM_ALPHA_E / 1000);
   TFitResultPtr fitres;
   fitres = gAmGd->Fit(&func, "S"); // S: return fitres

   float fit1, fit1_err, coef, coefErr, gain, gainErr;
   fit1 = fitres->Value(1);
   fit1_err = fitres->FitResult::Error(1);
   coef = ATTEN / fit1 * 1000;
   coefErr = coef * fit1_err / fit1;
   gain = 1 / coef;
   gainErr = gain * (coefErr / coef);

   ((TH1F*) c->d["alpha"]->o["hAmGdGain"])->SetBinContent(iCh, gain);
   ((TH1F*) c->d["alpha"]->o["hAmGdGain"])->SetBinError(iCh, gainErr);

   ((TH1F*) c->d["alpha"]->o["hDeadLayerEnergy"])->SetBinContent(iCh, fitres->Value(0) * 1000);
   ((TH1F*) c->d["alpha"]->o["hDeadLayerEnergy"])->SetBinError(iCh, fitres->FitResult::Error(0) * 1000);
}


/** */
TFitResultPtr AlphaCalibrator::Calibrate(TH1F *h, bool fit_gadolinium, bool *gadolinium_found)
{
   *gadolinium_found = false;
   TF1 f_amer("f_amer", "gaus");
   TF1 f_two_peak("fitfunc", "gaus(0) + gaus(3)");

   f_amer.SetLineColor(kRed);
   f_amer.SetLineWidth(3);
   f_amer.SetNpx(500);
   f_two_peak.SetLineColor(kRed);
   f_two_peak.SetLineWidth(3);
   f_two_peak.SetNpx(500);

   h->Print();
   float xmin = h->GetXaxis()->GetXmin();
   float xmax = h->GetXaxis()->GetXmax();

   if (h->Integral() < 10) {
      Error("Calibrate", "Too few entries in histogram %s. Skipped", h->GetName());
      return 0;
   }

   // First fit is to find americium peak
   // Will start from guessing initial params
   float norm_amer;
   int   mbin_amer = utils::FindMaximumBinEx(h, 2, &norm_amer);
   float mean_amer = h->GetBinCenter(mbin_amer);
   float expectedSigma = 0.7;

   // N.B.: This cut is on height of Am peak blured by FindMaximumBinEx
   if (norm_amer <= 4) {
      Error("Calibrate", "Am peak is too small in histogram %s. Skipping channel.", h->GetName());
      return 0;
   }

   f_amer.SetParameters(norm_amer, mean_amer, expectedSigma);
   f_amer.SetParLimits(0, 0.8 * norm_amer, 1.2 * norm_amer); // norm
   f_amer.SetParLimits(1, xmin, xmax); // mean
   f_amer.SetParLimits(2, 0.1, 3 * expectedSigma); // sigma

   TFitResultPtr am_fitres = h->Fit(&f_amer, "BMS"); // B: use limits, M: improve fit, S: return fitres

   if (!fit_gadolinium) {
      return am_fitres;
   } else {
      Double_t par[6]; // params for more general fit
      f_amer.GetParameters(&par[0]); // copy found params there

      mean_amer = par[1];
      float sigma_amer = par[2];

      // Now let's find initial params for gadolinium peak
      float xmax_gad = min(mean_amer - 5 * sigma_amer, mean_amer * 3/4);
      // Use predicted gad peak position to track the right peak
      float predicted_gad = mean_amer / AM_ALPHA_E * GD_ALPHA_E;
      float xmin_gad = predicted_gad - 4 * sigma_amer;
      h->GetXaxis()->SetRangeUser(xmin_gad, xmax_gad);
      int   mbin_gad = h->GetMaximumBin();
      h->GetXaxis()->SetRange(); // reset range
      float norm_gad = h->GetBinContent(mbin_gad);
      float mean_gad = h->GetBinCenter(mbin_gad);

      if (norm_gad <= 3)
      {
         Error("Calibrate", "Gd peak is too small in histogram %s. Using only Am peak fit.", h->GetName());
         return am_fitres;
      }

      par[3] = norm_gad;
      par[4] = mean_gad;
      par[5] = expectedSigma;
      f_two_peak.SetParameters(par);

      f_two_peak.SetParLimits(3, 0.8 * norm_gad, 1.2 * norm_gad); // norm
      f_two_peak.SetParLimits(4, xmin_gad, xmax_gad); // mean
      f_two_peak.SetParLimits(5, 0.1, 3 * expectedSigma); // sigma

      // And make sure that our existing peak won't too far
      f_two_peak.SetParLimits(0, 0.8 * norm_amer, 1.2 * norm_amer); // norm
      f_two_peak.SetParLimits(1, mean_amer - sigma_amer, mean_amer + sigma_amer); // mean
      f_two_peak.SetParLimits(2, 0.8 * sigma_amer, 1.2 * sigma_amer); // sigma

      TFitResultPtr gd_fitres = h->Fit(&f_two_peak, "BMS"); // B: use limits, M: improve fit, S: return fitres

      if (!gd_fitres)
      {
         Error("Calibrate", "Two peak fit failed at %s. Using only Am peak.", h->GetName());
         return am_fitres;
      }

      *gadolinium_found = true;
      return gd_fitres;
   }
}


/** */
void AlphaCalibrator::CalibrateBadChannels(DrawObjContainer *c)
{
   ChannelCalibMap::iterator mi, mi2;
   ChannelCalibMap::iterator mb  = fChannelCalibs.begin();
   ChannelCalibMap::iterator me  = fChannelCalibs.end();

   // Now assign detector average
   // Ampltitude
   for (mi = mb; mi != me; mi++) {

      ChannelCalib &ch = mi->second;
      if (ch.fAmAmp.fCoefErr >= 0) continue;

      short iDet = (short) floor((mi->first - 1) / 12);
      //printf("iDet: %d\n", iDet);

      float detAve = 0;
      short nChDet = 0;

      for (mi2 = mb; mi2 != me; mi2++) {

         ChannelCalib &ch2 = mi2->second;
         short iDet2 = (short) floor((mi2->first - 1) / 12);
         if (iDet == iDet2 && ch2.fAmAmp.fCoefErr >= 0) {

            //printf("iDet2: %d\n", iDet2);
            detAve += ch2.fAmAmp.fCoef;
            nChDet++;
         }
      }

      detAve    /= nChDet;
      ch.fAmAmp.fCoef  = detAve;
   }

   // Now assign detector average
   // Integral
   for (mi = mb; mi != me; mi++) {

      ChannelCalib &ch = mi->second;
      if (ch.fAmInt.fCoefErr >= 0) continue;

      short iDet = (short) floor((mi->first - 1) / 12);
      //printf("iDet: %d\n", iDet);

      float detAve = 0;
      short nChDet = 0;

      for (mi2 = mb; mi2 != me; mi2++) {
         ChannelCalib &ch2 = mi2->second;
         short iDet2 = (short) floor((mi2->first - 1) / 12);
         if (iDet == iDet2 && ch2.fAmInt.fCoefErr >= 0) {
            //printf("iDet2: %d\n", iDet2);
            detAve += ch2.fAmInt.fCoef;
            nChDet++;
         }
      }

      detAve    /= nChDet;
      ch.fAmInt.fCoef  = detAve;
   }
}


/** */
void AlphaCalibrator::Print(const Option_t* opt) const
{
   Info("Print", " ");
   Calibrator::Print(opt);
}
