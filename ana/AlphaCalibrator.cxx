/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "AlphaCalibrator.h"

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
	Info("Calibrate", "Executing Calibrate()");

   string    sCh("  ");
   //float     chi2_max  = 0;
   //Double_t  stat_max  = 0;
   TH1F     *htemp     = 0;
   TF1      *fitfunc   = new TF1("fitfunc", "gaus");
   TFitResultPtr fitres;

   fitfunc->SetLineColor(2);
   fitfunc->SetLineWidth(3);

   for (UShort_t i=1; i<=NSTRIP; i++) {
      //if (i != 28) continue;

      sprintf(&sCh[0], "%02d", i);

      ChannelCalib *chCalib;
      ChannelCalibMap::iterator iChCalib = fChannelCalibs.find(i);

      if (iChCalib != fChannelCalibs.end())  {
			chCalib = &iChCalib->second;
      } else {
         ChannelCalib newChCalib;
         fChannelCalibs[i] = newChCalib;
			chCalib = &fChannelCalibs[i];
      }

      // Amplitude
      htemp = (TH1F*) c->d["alpha"]->d["channel"+sCh]->o["hAmpltd_cut1_st"+sCh];

      if (!htemp) {
         Error("Calibrate", "Histogram channel%02d/hAmpltd_cut1_st%02d does not exist", i, i);
         continue;
      }

      fitres = Calibrate(htemp, fitfunc);

      if (fitres) {
         //fitres->Print();
         //aCoef    = (ALPHA_KEV/fitfunc->GetParameter(1)) * ATTEN;
         //aCoefErr = aCoef * fitfunc->GetParameter(2)/fitfunc->GetParameter(1);
         //aChi2    = fitfunc->GetChisquare();

         //chCalib.fACoef    = fitres->Value(1);
         //chCalib.fACoefErr = fitres->FitResult::Error(1);

         chCalib->fACoef    = (ALPHA_KEV/fitres->Value(1)) * ATTEN;
         //chCalib.fACoefErr = aCoef * fitres->Value(2)/fitres->Value(1);
         chCalib->fACoefErr = chCalib->fACoef * fitres->FitResult::Error(1)/fitres->Value(1);
         chCalib->fAChi2Ndf = fitres->Ndf() > 0 ? fitres->Chi2()/fitres->Ndf() : -1;

         ((TH1F*) c->d["alpha"]->o["hAmpltdW"])->SetBinContent(i, 100*fitres->Value(2)/fitres->Value(1));
         ((TH1F*) c->d["alpha"]->o["hAmpltdW"])->SetBinError(i, 100*fitres->FitResult::Error(2)/(fitres->Value(1)));

         ((TH1F*) c->d["alpha"]->o["hACoef"])->SetBinContent(i, chCalib->fACoef);
         ((TH1F*) c->d["alpha"]->o["hACoef"])->SetBinError(i, chCalib->fACoefErr);

         ((TH1F*) c->d["alpha"]->o["hACoefDisp"])->Fill(chCalib->fACoef);
      }

      // Integral
      htemp = (TH1F*) c->d["alpha"]->d["channel"+sCh]->o["hIntgrl_cut1_st"+sCh];

      if (!htemp) {
         Error("Calibrate", "Histogram channel%02d/hIntgrl_cut1_st%02d does not exist", i, i);
         continue;
      }

      fitres = Calibrate(htemp, fitfunc);

      if (fitres.Get()) {
         //fitres->Print();

         chCalib->fICoef    = (ALPHA_KEV/fitres->Value(1)) * ATTEN;
         //chCalib->fACoefErr = aCoef * fitres->Value(2)/fitres->Value(1);
         chCalib->fICoefErr = chCalib->fICoef * fitres->FitResult::Error(1)/fitres->Value(1);
         chCalib->fIChi2Ndf = fitres->Ndf() > 0 ? fitres->Chi2()/fitres->Ndf() : -1;

         ((TH1F*) c->d["alpha"]->o["hIntgrlW"])->SetBinContent(i, 100*fitres->Value(2)/fitres->Value(1));
         ((TH1F*) c->d["alpha"]->o["hIntgrlW"])->SetBinError(i, 100*fitres->FitResult::Error(2)/(fitres->Value(1)));

         ((TH1F*) c->d["alpha"]->o["hICoef"])->SetBinContent(i, chCalib->fICoef);
         ((TH1F*) c->d["alpha"]->o["hICoef"])->SetBinError(i, chCalib->fICoefErr);

         ((TH1F*) c->d["alpha"]->o["hICoefDisp"])->Fill(chCalib->fICoef);
      } else {
         Error("Calibrate", "Empty TFitResultPtr");
      }

      //printf("%6.4f %6.4f %6.2f %6.2f %4.1f %3d  SUCCESSFUL\n",
      //        cal, ecal, amp, eamp, chi2/ndf, ndf);
      //fprintf(pf0,"%2d %6.4f %6.4f %6.2f %6.2f %4.1f %3d SUCCESSFUL\n",
      //        i,cal[i],ecal[i],amp[i],eamp[i],chi2[i]/ndf,ndf);

      //Double_t stat = hAmp->GetEntries();
      //if (stat > stat_max) stat_max = stat;
      //if (chi2 > chi2_max) chi2_max = chi2;
   }

   CalibrateBadChannels(c);
}


/** */
void AlphaCalibrator::CalibrateFast(DrawObjContainer *c)
{
	Info("CalibrateFast", "Executing CalibrateFast()");
}


/** */
TFitResultPtr AlphaCalibrator::Calibrate(TH1 *h, TF1 *f, Bool_t wideLimits)
{
   TFitResultPtr fitres = 0;

   h->Print();

   if (h->Integral() < 10) {
      Error("Calibrate", "Too few entries in histogram %s. Skipped", h->GetName());
      //f->SetParameters(0, 1, 0);
      //f->SetParError(2, 0);
      return 0;
   }

   int   mbin = h->GetMaximumBin();
   float norm = h->GetBinContent(mbin);
   float mean = h->GetBinCenter(mbin);
   float xmin = h->GetXaxis()->GetXmin();
   float xmax = h->GetXaxis()->GetXmax();
   //float ymin = h->GetYaxis()->GetXmin();
   //float ymax = h->GetYaxis()->GetXmax();
   //float intg = h->Integral();
   float expectedSigma = 0.7;

   //h->SetAxisRange(apeak-awidth,apeak+awidth);
   f->SetParameters(norm, mean, expectedSigma);
   f->SetParLimits(0, 0.8*norm, 1.2*norm); // norm
   //f->SetParLimits(0, ymin, ymax); // norm
   //f->SetParLimits(1, mean-5*expectedSigma, mean+5*expectedSigma); // mean
   f->SetParLimits(1, xmin, xmax); // mean
   f->SetParLimits(2, 0.1, 3*expectedSigma); // sigma

   //h->Fit(f, "", "", apeak-awidth, apeak+awidth);
   //h->Print();
   fitres = h->Fit(f, "BMS"); // B: use limits, M: improve fit, S: return fitres
   //fitres->Print("V");

   return fitres;
}


/** */
//TFitResultPtr AlphaCalibrator::Calibrate(TH1 *h, TF1 *f)
//{
//}


/** */
void AlphaCalibrator::CalibrateBadChannels(DrawObjContainer *c)
{
   Double_t aMean = ((TH1F*) c->d["alpha"]->o["hACoefDisp"])->GetMean();
   Double_t aRMS  = ((TH1F*) c->d["alpha"]->o["hACoefDisp"])->GetRMS();
   Double_t iMean = ((TH1F*) c->d["alpha"]->o["hICoefDisp"])->GetMean();
   Double_t iRMS  = ((TH1F*) c->d["alpha"]->o["hICoefDisp"])->GetRMS();

   ChannelCalibMap::iterator mi, mi2;
   ChannelCalibMap::iterator mb  = fChannelCalibs.begin();
   ChannelCalibMap::iterator me  = fChannelCalibs.end();

   for (mi=mb; mi!=me; mi++) {

      ChannelCalib &ch = mi->second;

      // Invalidate channels even if the fitter returned some results
      if ( ch.fACoef >= 0 && fabs(aMean - ch.fACoef) > 5*aRMS ) {
         //ch.fACoef    = -1;
         ch.fACoefErr = -1;
      }// else
       //  ch.fACoefErr = -1;

      if ( ch.fICoef >= 0 && fabs(iMean - ch.fICoef) > 5*iRMS ) {
         //ch.fICoef    = -1;
         ch.fICoefErr = -1;
      }
   }

   // Now assign detector average
   // Ampltitude
   for (mi=mb; mi!=me; mi++) {

      ChannelCalib &ch = mi->second;
      if (ch.fACoefErr >= 0) continue;

      short iDet = (short) floor((mi->first-1)/12);
      //printf("iDet: %d\n", iDet);

      float detAve = 0;
      short nChDet = 0;

      for (mi2=mb; mi2!=me; mi2++) {
         ChannelCalib &ch2 = mi2->second;
         short iDet2 = (short) floor((mi2->first-1)/12);
         if (iDet == iDet2 && ch2.fACoefErr >=0) {
            //printf("iDet2: %d\n", iDet2);
            detAve += ch2.fACoef;
            nChDet++;
         }
      }

      detAve /= nChDet;

      ch.fACoef = detAve;
   }

   // Now assign detector average
   // Integral
   for (mi=mb; mi!=me; mi++) {

      ChannelCalib &ch = mi->second;
      if (ch.fICoefErr >= 0) continue;

      short iDet = (short) floor((mi->first-1)/12);
      //printf("iDet: %d\n", iDet);

      float detAve = 0;
      short nChDet = 0;

      for (mi2=mb; mi2!=me; mi2++) {
         ChannelCalib &ch2 = mi2->second;
         short iDet2 = (short) floor((mi2->first-1)/12);
         if (iDet == iDet2 && ch2.fICoefErr >=0) {
            //printf("iDet2: %d\n", iDet2);
            detAve += ch2.fICoef;
            nChDet++;
         }
      }

      detAve /= nChDet;

      ch.fICoef = detAve;
   }
}


/** */
void AlphaCalibrator::Print(const Option_t* opt) const
{
   printf("AlphaCalibrator:\n");

   Calibrator::Print(opt);
}
