/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "AlphaCalibrator.h"
#include "AsymHeader.h"
#include "AsymAnaInfo.h"
#include "AsymGlobals.h"
#include <fstream>

#include "TMath.h"

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
   bool     fit_gadolinium = gAsymAnaInfo->fAlphaSourceCount == 2;

   for (UShort_t i = 1; i <= NSTRIP; i++) {
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

      fitres = Calibrate(htemp, fit_gadolinium);

      if (fitres) {
         chCalib->fAmAmp = CoefExtract(fitres, kAmericium, c, i, "AmAmp");
         if (fit_gadolinium) {
            chCalib->fGdAmp = CoefExtract(fitres, kGadolinium, c, i, "GdAmp");
            AmGdPlot(chCalib, c, i, sCh);

            c->d["alpha"]->o["hAmGdAmpCoef_over_AmAmpCoef"] = new TH1F(
               (*(TH1F*) c->d["alpha"]->o["hAmGdAmpCoef"]) / (*(TH1F*) c->d["alpha"]->o["hAmAmpCoef"])
            );
            ((TH1F*) c->d["alpha"]->o["hAmGdAmpCoef_over_AmAmpCoef"])->SetName("hAmGdAmpCoef_over_AmAmpCoef");
            ((TH1F*) c->d["alpha"]->o["hAmGdAmpCoef_over_AmAmpCoef"])->SetTitle("hAmGdAmpCoef_over_AmAmpCoef");
         }
      }

      // Integral
      htemp = (TH1F*) c->d["alpha"]->d["channel" + sCh]->o["hIntgrl_ch" + sCh];

      if (!htemp) {
         Error("Calibrate", "Histogram channel%02d/hIntgrl_ch%02d does not exist", i, i);
         continue;
      }

      fitres = Calibrate(htemp, fit_gadolinium);

      if (fitres.Get()) {
         chCalib->fAmInt = CoefExtract(fitres, kAmericium, c, i, "AmInt");
         if (fit_gadolinium) {
            chCalib->fGdInt = CoefExtract(fitres, kGadolinium, c, i, "GdInt");
         }
      } else {
         Error("Calibrate", "Empty TFitResultPtr");
      }
   }

   CalibrateBadChannels(c);
}


/** */
CalibCoefSet AlphaCalibrator::CoefExtract (
   const TFitResultPtr &fitres, ESource source_offset,
   DrawObjContainer *c, UShort_t i, std::string name
)
{
   // Typical value for long_name is "Intgrl", for short_name it is "I"

   CalibCoefSet result;

   float peak_energy = AM_ALPHA_E;

   if (source_offset == kAmericium)
   {
      peak_energy = AM_ALPHA_E;
   } else if (source_offset == kGadolinium) {
      peak_energy = GD_ALPHA_E;
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
   ((TH1F*) c->d["alpha"]->o["h" + name + "Width"])->SetBinContent(i, width_value);
   ((TH1F*) c->d["alpha"]->o["h" + name + "Width"])->SetBinError(i, width_error);

   ((TH1F*) c->d["alpha"]->o["h" + name + "Coef"])->SetBinContent(i, result.fCoef);
   ((TH1F*) c->d["alpha"]->o["h" + name + "Coef"])->SetBinError(i, result.fCoefErr);

   ((TH1F*) c->d["alpha"]->o["h" + name + "CoefDisp"])->Fill(result.fCoef);

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

   TFitResultPtr fitres;
   fitres = gAmGd->Fit("pol1", "S", "", 0, AM_ALPHA_E / 1000); // S: return fitres

   float fit1, fit1_err, coef, coefErr;
   fit1 = fitres->Value(1);
   fit1_err = fitres->FitResult::Error(1);
   coef = ATTEN / fit1 * 1000;
   coefErr = coef * fit1_err / fit1;

   ((TH1F*) c->d["alpha"]->o["hAmGdAmpCoef"])->SetBinContent(iCh, coef);
   ((TH1F*) c->d["alpha"]->o["hAmGdAmpCoef"])->SetBinError(iCh, coefErr);

   ((TH1F*) c->d["alpha"]->o["hAmGdFit0Coef"])->SetBinContent(iCh, fitres->Value(0));
   ((TH1F*) c->d["alpha"]->o["hAmGdFit0Coef"])->SetBinError(iCh, fitres->FitResult::Error(0));
}


/** Gets maximum bin in blurred hist */
Int_t AlphaCalibrator::GetMaximumBinEx(TH1F *h, int blur_radius)
{
   TH1F *blurred = (TH1F*)h->Clone("blurred");
   blurred->Reset();
   Int_t xfirst  = blurred->GetXaxis()->GetFirst();
   Int_t xlast   = blurred->GetXaxis()->GetLast();
   for (Int_t bin = xfirst; bin <= xlast; bin++) {
      Int_t start = TMath::Max(bin - blur_radius, xfirst);
      Int_t end   = TMath::Min(bin + blur_radius, xlast);
      Double_t value = 0;
      for (int i = start; i <= end; i++) {
         value += h->GetBinContent(i);
      }
      blurred->SetBinContent(bin, value);
   }
   return blurred->GetMaximumBin();
}


/** */
TFitResultPtr AlphaCalibrator::Calibrate(TH1F *h, bool fit_gadolinium)
{
   TF1 f_amer("f_amer", "gaus");
   TF1 f_two_peak("fitfunc", "gaus(0) + gaus(3)");
   TFitResultPtr fitres = 0;

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
   int   mbin_amer = GetMaximumBinEx(h, 1);
   float norm_amer = h->GetBinContent(mbin_amer);
   float mean_amer = h->GetBinCenter(mbin_amer);
   float expectedSigma = 0.7;

   if (norm_amer < 3) {
      Error("Calibrate", "Peaks are too small in histogram %s. Skipped", h->GetName());
      return 0;
   }

   f_amer.SetParameters(norm_amer, mean_amer, expectedSigma);
   f_amer.SetParLimits(0, 0.8 * norm_amer, 1.2 * norm_amer); // norm
   f_amer.SetParLimits(1, xmin, xmax); // mean
   f_amer.SetParLimits(2, 0.1, 3 * expectedSigma); // sigma

   fitres = h->Fit(&f_amer, "BMS"); // B: use limits, M: improve fit, S: return fitres

   if (fit_gadolinium) {
      Double_t par[6]; // params for more general fit
      f_amer.GetParameters(&par[0]); // copy found params there

      mean_amer = par[1];
      float sigma_amer = par[2];

      // Now let's find initial params for gadolinium peak
      float xmax_gad = mean_amer - 5 * sigma_amer;
      // Use predicted gad peak position to track the right peak
      float predicted_gad = mean_amer / AM_ALPHA_E * GD_ALPHA_E;
      float xmin_gad = predicted_gad - 4 * sigma_amer;
      h->GetXaxis()->SetRangeUser(xmin_gad, xmax_gad);
      int   mbin_gad = h->GetMaximumBin();
      h->GetXaxis()->SetRange(); // reset range
      float norm_gad = h->GetBinContent(mbin_gad);
      float mean_gad = h->GetBinCenter(mbin_gad);

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

      fitres = h->Fit(&f_two_peak, "BMS"); // B: use limits, M: improve fit, S: return fitres
   }

   return fitres;
}


/** */
void AlphaCalibrator::CalibrateBadChannels(DrawObjContainer *c)
{
   Double_t aMean = ((TH1F*) c->d["alpha"]->o["hAmAmpCoefDisp"])->GetMean();
   Double_t aRMS  = ((TH1F*) c->d["alpha"]->o["hAmAmpCoefDisp"])->GetRMS();
   //Double_t iMean = ((TH1F*) c->d["alpha"]->o["hAmIntCoefDisp"])->GetMean();
   //Double_t iRMS  = ((TH1F*) c->d["alpha"]->o["hAmIntCoefDisp"])->GetRMS();

   Double_t aEntries  = 0;

   Double_t Det1eRMS  = 0;
   Double_t Det2eRMS  = 0;
   Double_t Det3eRMS  = 0;
   Double_t Det4eRMS  = 0;
   Double_t Det5eRMS  = 0;
   Double_t Det6eRMS  = 0;
   Double_t Det1eMean = 0;
   Double_t Det2eMean = 0;
   Double_t Det3eMean = 0;
   Double_t Det4eMean = 0;
   Double_t Det5eMean = 0;
   Double_t Det6eMean = 0;

   string sSi("  ");

   for (int i = 1; i <= NSTRIP; i++)
   {
      sprintf(&sSi[0], "%02d", i);
      aEntries = ((TH1F*) c->d["alpha"]->d["channel" + sSi]->o["hAmpltd_ch" + sSi])->GetEntries();

      if (i > 0 && i < 13) {
         ((TH1F*) c->d["alpha"]->o["Detector1_Events"])->SetBinContent(i, aEntries);
         ((TH1F*) c->d["alpha"]->o["Detector1_Events_Dsp"])->Fill(aEntries);

      }
      if (i > 12 && i < 25) {
         ((TH1F*) c->d["alpha"]->o["Detector2_Events"])->SetBinContent(i - 12, aEntries);
         ((TH1F*) c->d["alpha"]->o["Detector2_Events_Dsp"])->Fill(aEntries);

      }
      if (i > 24 && i < 37) {
         ((TH1F*) c->d["alpha"]->o["Detector3_Events"])->SetBinContent(i - 24, aEntries);
         ((TH1F*) c->d["alpha"]->o["Detector3_Events_Dsp"])->Fill(aEntries);

      }
      if (i > 36 && i < 49) {
         ((TH1F*) c->d["alpha"]->o["Detector4_Events"])->SetBinContent(i - 36, aEntries);
         ((TH1F*) c->d["alpha"]->o["Detector4_Events_Dsp"])->Fill(aEntries);

      }
      if (i > 48 && i < 61) {
         ((TH1F*) c->d["alpha"]->o["Detector5_Events"])->SetBinContent(i - 48, aEntries);
         ((TH1F*) c->d["alpha"]->o["Detector5_Events_Dsp"])->Fill(aEntries);

      }
      if (i > 60 && i < 73) {
         ((TH1F*) c->d["alpha"]->o["Detector6_Events"])->SetBinContent(i - 60, aEntries);
         ((TH1F*) c->d["alpha"]->o["Detector6_Events_Dsp"])->Fill(aEntries);

      }
   }

   Det1eMean = ((TH1F*) c->d["alpha"]->o["Detector1_Events_Dsp"])->GetMean();
   Det1eRMS  = ((TH1F*) c->d["alpha"]->o["Detector1_Events_Dsp"])->GetRMS();
   Det2eMean = ((TH1F*) c->d["alpha"]->o["Detector2_Events_Dsp"])->GetMean();
   Det2eRMS  = ((TH1F*) c->d["alpha"]->o["Detector2_Events_Dsp"])->GetRMS();
   Det3eMean = ((TH1F*) c->d["alpha"]->o["Detector3_Events_Dsp"])->GetMean();
   Det3eRMS  = ((TH1F*) c->d["alpha"]->o["Detector3_Events_Dsp"])->GetRMS();
   Det4eMean = ((TH1F*) c->d["alpha"]->o["Detector4_Events_Dsp"])->GetMean();
   Det4eRMS  = ((TH1F*) c->d["alpha"]->o["Detector4_Events_Dsp"])->GetRMS();
   Det5eMean = ((TH1F*) c->d["alpha"]->o["Detector5_Events_Dsp"])->GetMean();
   Det5eRMS  = ((TH1F*) c->d["alpha"]->o["Detector5_Events_Dsp"])->GetRMS();
   Det6eMean = ((TH1F*) c->d["alpha"]->o["Detector6_Events_Dsp"])->GetMean();
   Det6eRMS  = ((TH1F*) c->d["alpha"]->o["Detector6_Events_Dsp"])->GetRMS();

   ChannelCalibMap::iterator mi, mi2;
   ChannelCalibMap::iterator mb  = fChannelCalibs.begin();
   ChannelCalibMap::iterator me  = fChannelCalibs.end();
   int i;

   for (i = 1, mi = mb; mi != me; mi++, i++)
   {
      sprintf(&sSi[0], "%02d", i);
      aEntries = ((TH1F*) c->d["alpha"]->d["channel" + sSi]->o["hAmpltd_ch" + sSi])->GetEntries();

      ChannelCalib &ch = mi->second;

      // Invalidate channels even if the fitter returned some results
      if (i > 0 && i <= 12) {
         if (fabs(aMean - ch.fAmAmp.fCoef) > 2*aRMS ) {
            if (aEntries<1000 && aEntries >= 0 && fabs(Det1eMean - aEntries) > 2 * Det1eRMS && (aEntries - Det1eMean < 0)) {
               ch.fAmAmp.fCoefErr = -1;
            }
         }
      }
      if (i > 12 && i <= 24) {
         if (fabs(aMean - ch.fAmAmp.fCoef) > 2*aRMS ) {
            if (aEntries<1000 && aEntries >= 0 && fabs(Det2eMean - aEntries) > 2 * Det2eRMS && (aEntries - Det2eMean < 0)) {
               ch.fAmAmp.fCoefErr = -1;
            }
         }
      }
      if (i > 24 && i <= 36) {
         if (fabs(aMean - ch.fAmAmp.fCoef) > 2*aRMS ) {
            if (aEntries<1000 && aEntries >= 0 && fabs(Det3eMean - aEntries) > 2 * Det3eRMS && (aEntries - Det3eMean < 0)) {
               ch.fAmAmp.fCoefErr = -1;
            }
         }
      }
      if (i > 36 && i <= 48) {
         if (fabs(aMean - ch.fAmAmp.fCoef) > 2*aRMS ) {
            if (aEntries<1000 && aEntries >= 0 && fabs(Det4eMean - aEntries) > 2 * Det4eRMS && (aEntries - Det4eMean < 0)) {
               ch.fAmAmp.fCoefErr = -1;
            }
         }
      }
      if (i > 48 && i <= 60) {
         if (fabs(aMean - ch.fAmAmp.fCoef) > 2*aRMS ) {
            if (aEntries<1000 && aEntries >= 0 && fabs(Det5eMean - aEntries) > 2 * Det5eRMS && (aEntries - Det5eMean < 0)) {
               ch.fAmAmp.fCoefErr = -1;
            }
         }
      }
      if (i > 60 && i <= 72) {
         if (fabs(aMean - ch.fAmAmp.fCoef) > 2*aRMS ) {
            if (aEntries<1000 && aEntries >= 0 && fabs(Det6eMean - aEntries) > 2 * Det6eRMS && (aEntries - Det6eMean < 0)) {
               ch.fAmAmp.fCoefErr = -1;
            }
         }
      }
   }

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
