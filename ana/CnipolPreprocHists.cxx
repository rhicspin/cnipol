/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolPreprocHists.h"

#include "TDirectoryFile.h"
#include "TFitResultPtr.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TPaveStats.h"

#include "utils/utils.h"

#include "AnaInfo.h"
#include "AsymRoot.h"
//#include "CnipolPulserHists.h"
#include "MeasInfo.h"

ClassImp(CnipolPreprocHists)

using namespace std;


/** Default constructor. */
CnipolPreprocHists::CnipolPreprocHists() : DrawObjContainer()
{
   BookHists();
}


CnipolPreprocHists::CnipolPreprocHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
}


/** Default destructor. */
CnipolPreprocHists::~CnipolPreprocHists()
{
}


/** */
void CnipolPreprocHists::BookHists()
{
   string shName;
   TH1*   hist;

   fDir->cd();

   // Data from all enabled silicon channels
   //shName = "hTimeVsEnergyA";
   //hist = new TH2S(shName.c_str(), shName.c_str(), 80, 100, 1700, 80, 20, 100);
   //hist->SetTitle("; Deposited Energy, keV; Time, ns;");
   //hist->SetOption("colz LOGZ NOIMG");
   //o[shName] = hist;

   shName = "hsTimeVsEnergyACumul";
   o[shName] = new THStack(shName.c_str(), shName.c_str());

   ChannelSetIter iCh = gMeasInfo->fSiliconChannels.begin();

   for (; iCh!=gMeasInfo->fSiliconChannels.end(); ++iCh)
   {
      UShort_t chId = *iCh;
      string   sChId("  ");
      sprintf(&sChId[0], "%02d", chId);

      // Time vs energy from amplitude
      // Time vs energy from amplitude special binning
      shName = "hTimeVsEnergyA_ch" + sChId;
      hist = new TH2S(shName.c_str(), shName.c_str(), 1, 0, 1, 1, 0, 1);
      hist->SetTitle("; Deposited Energy, keV; Time, ns;");
      hist->SetOption("colz LOGZ NOIMG");
      o[shName] = hist;
      fhTimeVsEnergyA_ch[chId-1] = hist;

      shName = "hFitMeanTimeVsEnergyA_ch" + sChId;
      hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
      hist->SetTitle("; Deposited Energy, keV; Mean Time, ns;");
      hist->SetOption("E1 GRIDX NOIMG");
      hist->GetYaxis()->SetRangeUser(10, 110);
      o[shName] = hist;
      fhFitMeanTimeVsEnergyA_ch[chId-1] = hist;

      shName = "hFitChi2NdfVsEnergyA_ch" + sChId;
      hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
      hist->SetTitle("; Deposited Energy, keV; #chi^2/ndf;");
      hist->SetOption("P GRIDX");
      hist->SetMarkerStyle(kFullCircle);
      hist->SetMarkerSize(0.8);
      hist->SetMarkerColor(kGreen);
      o[shName] = hist;
      fhFitChi2NdfVsEnergyA_ch[chId-1] = hist;

      shName = "hFitChi2NdfLogVsEnergyA_ch" + sChId;
      hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
      hist->SetTitle("; Deposited Energy, keV; log(#chi^2/ndf);");
      hist->SetOption("P GRIDX");
      hist->SetMarkerStyle(kFullCircle);
      hist->SetMarkerSize(0.8);
      hist->SetMarkerColor(kGreen);
      o[shName] = hist;
      fhFitChi2NdfLogVsEnergyA_ch[chId-1] = hist;

      shName = "hTimeVsEnergyACumul_ch" + sChId;
      hist = new TH1F(shName.c_str(), shName.c_str(), 100, 0, 1);
      hist->SetOption("hist");
      hist->SetTitle("; Digi Channel Frac; Event Frac;");
      hist->SetLineWidth(2);
      hist->SetLineColor(RunConfig::AsColor(chId));
      hist->GetYaxis()->SetRangeUser(0, 1);
      o[shName]                       = hist;
      fhTimeVsEnergyACumul_ch[chId-1] = hist;
      ((THStack*) o["hsTimeVsEnergyACumul"])->Add(fhTimeVsEnergyACumul_ch[chId-1]);
   }
}


/** */
void CnipolPreprocHists::FillPassOne(ChannelEvent *ch)
{
   //UChar_t chId = ch->GetChannelId();

   //fhTimeVsEnergyA_ch[chId-1] -> Fill(ch->GetEnergyA(), ch->GetTime());
}


/** */
void CnipolPreprocHists::FillDerivedPassOne()
{
   // Fill derivative histograms first
   //TH1* hTimeVsEnergyA = (TH1*) o["hTimeVsEnergyA"];
   //
   //ChannelSetIter iCh = gMeasInfo->fSiliconChannels.begin();

   //for (; iCh!=gMeasInfo->fSiliconChannels.end(); ++iCh)
   //{
   //   UShort_t chId = *iCh;

   //   string sChId(MAX_CHANNEL_DIGITS, ' ');
   //   sprintf(&sChId[0], "%02d", chId);

   //   TH2* hTimeVsEnergyA_channel = (TH2*) fhTimeVsEnergyA_ch[chId-1];
   //   hTimeVsEnergyA->Add(hTimeVsEnergyA_channel);
   //}
}


/** */
void CnipolPreprocHists::PostFillPassOne(DrawObjContainer *oc)
{
   Info("PostFillPassOne", "Called");

   //// We expect empty bunch histogram container of the same class
   //if (!oc || oc->d.find("preproc_eb") == oc->d.end() ) {
   //   Error("PostFillPassOne", "No empty bunch container 'preproc_eb' found");
   //} else {
   //   CnipolPreprocHists* ebHists = (CnipolPreprocHists*) oc->d.find("preproc_eb")->second;
   //   PostFillPassOne_SubtractEmptyBunch(ebHists);
   //}

   // A raw histogram container is required to fill TvsE histograms
   if ( !oc || oc->d.find("raw_neb") == oc->d.end() )
   {
      Error("PostFillPassOne", "No 'raw_neb' histogram container found");
   } else {
      CnipolRawHists *rawHists   = (CnipolRawHists*) oc->d.find("raw_neb")->second;
      PostFillPassOne_FillFromRawHists(rawHists);
   }

   //CnipolPulserHists *pulserHists = (CnipolPulserHists *) oc->d.find("pulser")->second;

   // Find proper normalization scale for pulser histograms
   //TH1* hPulserTdc  = (TH1*) pulserHists->o.find("hTdc")->second;
   //TH2* hPulserTvsA = (TH2*) pulserHists->o.find("hTvsA")->second;

   //TH2* hPulserTvsA = (TH2*) pulserHists->o.find("hTimeVsEnergyA")->second;
   //TH1* hPulserTdc  = hPulserTvsA->ProjectionY("hTimeVsEnergyA_projy");

   //Int_t cntTdcBin = hPulserTdc->GetMaximumBin();

   //Int_t minTdcBin = cntTdcBin - 1;
   //Int_t maxTdcBin = cntTdcBin + 1;

   //// Fit 3 bins with a gaussian func
   //TH1F* hPulserAdcMainPeak = hPulserTvsA->ProjectionX("hPulserAdcMainPeak", minTdcBin, maxTdcBin, "eo");

   ////hPulserTvsA->Print("all");
   ////hPulserAdcMainPeak->Print("all");

   //TFitResultPtr fitres = hPulserAdcMainPeak->Fit("gaus", "E S", "");

   //Double_t pulserAdcMean=0, pulserAdcMeanErr, pulserAdcSigma=0, pulserAdcSigmaErr;

   //if (fitres.Get()) {
   //   pulserAdcMean     = fitres->Value(1);
   //   pulserAdcMeanErr  = fitres->FitResult::Error(1);
   //   pulserAdcSigma    = fitres->Value(2);
   //   pulserAdcSigmaErr = fitres->FitResult::Error(2);
   //}

   //Int_t minAdcBin = hPulserAdcMainPeak->FindBin(pulserAdcMean - 2*pulserAdcSigma);
   //Int_t maxAdcBin = hPulserAdcMainPeak->FindBin(pulserAdcMean + 2*pulserAdcSigma);

   //delete hPulserAdcMainPeak;

   //printf("%d, %d, %d, %d, %f, %f\n", minAdcBin, maxAdcBin, minTdcBin, maxTdcBin, pulserAdcMean, pulserAdcSigma);

   // Fit 3 bins with a gaussian func
   //hPulserTdc->FitSlicesX(0, maxValBin-1, maxValBin+1, 0, "QNR G3", fitResHists);

}


/** */
void CnipolPreprocHists::SaveAllAs(TCanvas &c, string pattern, string path, Bool_t thumbs)
{
   DrawObjContainer::SaveAllAs(c, pattern, path, thumbs);

   // Draw superimposed histos

   string strThumb = thumbs ? "_thumb" : "" ;
   string cName = "c_combo";

   // Draw superimposed for all channels
   ChannelSetIter iCh = gMeasInfo->fSiliconChannels.begin();

   for (; iCh!=gMeasInfo->fSiliconChannels.end(); ++iCh)
   {
      UShort_t chId = *iCh;

      string sSi("  ");
      sprintf(&sSi[0], "%02d", chId);
      string cName = "c_combo_ch" + sSi + strThumb;

      THStack hstack(cName.c_str(), cName.c_str());

      TH1* h1 = (TH1*) fhTimeVsEnergyA_ch[chId-1];
		hstack.Add(h1);

      TH1* h2 = (TH1*) fhFitMeanTimeVsEnergyA_ch[chId-1];
		hstack.Add(h2);

		SaveHStackAs(c, hstack, path);
   }
}


/** This method is not used for now. */
void CnipolPreprocHists::PostFillPassOne_SubtractEmptyBunch(CnipolPreprocHists *ebHists)
{
/*
   TH2* hTimeVsEnergyA = (TH2*) o["hTimeVsEnergyA"];

   ChannelSetIter iCh = gMeasInfo->fSiliconChannels.begin();

   for (; iCh!=gMeasInfo->fSiliconChannels.end(); ++iCh)
   {
      UShort_t chId = *iCh;

      TH2* fhTimeVsEnergyA_ch_this = (TH2*) fhTimeVsEnergyA_ch[chId-1];
      TH2* fhTimeVsEnergyA_ch_eb   = (TH2*) ebHists->fhTimeVsEnergyA_ch[chId-1];

      if ( !fhTimeVsEnergyA_ch_this || !fhTimeVsEnergyA_ch_eb ) {
         Error("PostFillPassOne_SubtractEmptyBunch", "No empty bunch histogram found %s", fhTimeVsEnergyA_ch_this->GetName());
         continue;
      }

      //Double_t sumPreproc = hPreproc_ch->Integral(minAdcBin, maxAdcBin, minTdcBin, maxTdcBin);
      //Double_t sumPulser  = hPulser_ch ->Integral(minAdcBin, maxAdcBin, minTdcBin, maxTdcBin);

      //Double_t area = (maxAdcBin + 1 - minAdcBin) * (maxTdcBin + 1 - minTdcBin);

      ////printf("%d, %d, %d, %d, %d, %f, %f, %f\n", chId, minAdcBin, maxAdcBin, minTdcBin, maxTdcBin, area, sumPreproc, sumPulser);

      //for (Int_t ibx=minAdcBin; ibx<=maxAdcBin; ibx++) {
      //   for (Int_t iby=minTdcBin; iby<=maxTdcBin; iby++) {
      //      hPreproc_ch->SetBinContent(ibx, iby, sumPreproc/area);
      //      hPulser_ch ->SetBinContent(ibx, iby, sumPulser/area);
      //   }
      //}

      // Subtract empty bunch data from all bunch data
      fhTimeVsEnergyA_ch_eb->Scale( N_BUNCHES / (float) gMeasInfo->GetNumEmptyBunches());
      fhTimeVsEnergyA_ch_this->Add(fhTimeVsEnergyA_ch_eb, -1);

      // After the subtraction set bins with negative content to 0 including under/overflows
      for (Int_t ibx=0; ibx<=fhTimeVsEnergyA_ch_this->GetNbinsX()+1; ibx++) {
         for (Int_t iby=0; iby<=fhTimeVsEnergyA_ch_this->GetNbinsY()+1; iby++) {

            Double_t bc = fhTimeVsEnergyA_ch_this->GetBinContent(ibx, iby);

            if (bc < 0) {
               fhTimeVsEnergyA_ch_this->SetBinContent(ibx, iby, 0);
               fhTimeVsEnergyA_ch_this->SetBinError(ibx, iby, 0);
            }
         }
      }

      hTimeVsEnergyA->Add(fhTimeVsEnergyA_ch_this);
   }
*/
}


/** */
void CnipolPreprocHists::PostFillPassOne_FillFromRawHists(CnipolRawHists *rawHists)
{
   TH1 *hNoiseReject;
   
   ChannelSetIter iCh = gMeasInfo->fSiliconChannels.begin();

   for (; iCh!=gMeasInfo->fSiliconChannels.end(); ++iCh)
   {
      UShort_t chId                       = *iCh;
      TH1*     hTvsA_ch                   = (TH1*) rawHists->GetHTvsA_ch(chId);   // raw hist all - empty bunches
      TH1*     hTimeVsEnergyA_ch          = (TH1*) fhTimeVsEnergyA_ch[chId-1];
      TH1*     hFitMeanTimeVsEnergyA_ch   = (TH1*) fhFitMeanTimeVsEnergyA_ch[chId-1];
      TH1*     hFitChi2NdfVsEnergyA_ch    = (TH1*) fhFitChi2NdfVsEnergyA_ch[chId-1];
      TH1*     hFitChi2NdfLogVsEnergyA_ch = (TH1*) fhFitChi2NdfLogVsEnergyA_ch[chId-1];

      UShort_t adcMin = TMath::Max(gMeasInfo->GetProtoCutAdcMin(), (UShort_t) hTvsA_ch->GetXaxis()->GetXmin());
      UShort_t adcMax = TMath::Min(gMeasInfo->GetProtoCutAdcMax(), (UShort_t) hTvsA_ch->GetXaxis()->GetXmax());
      UShort_t tdcMin = TMath::Max(gMeasInfo->GetProtoCutTdcMin(), (UShort_t) hTvsA_ch->GetYaxis()->GetXmin());
      UShort_t tdcMax = TMath::Min(gMeasInfo->GetProtoCutTdcMax(), (UShort_t) hTvsA_ch->GetYaxis()->GetXmax());

      Int_t nXBins = adcMax - adcMin;
      Int_t nYBins = tdcMax - tdcMin;

      Calibrator *calibrator = gAsymRoot->GetCalibrator();

      Float_t xMin = calibrator->GetEnergyA(adcMin, chId);
      Float_t xMax = calibrator->GetEnergyA(adcMax, chId);
      Float_t yMin = calibrator->GetTime(tdcMin);
      Float_t yMax = calibrator->GetTime(tdcMax);

      hTimeVsEnergyA_ch->SetBins(nXBins, xMin, xMax, nYBins, yMin, yMax);
      hFitMeanTimeVsEnergyA_ch->SetBins(nXBins, xMin, xMax);
      hFitChi2NdfVsEnergyA_ch->SetBins(nXBins, xMin, xMax);
      hFitChi2NdfLogVsEnergyA_ch->SetBins(nXBins, xMin, xMax);

      Short_t extraOffset = 0;

      if ( UInt_t(gMeasInfo->GetBeamEnergy() + 0.5) != kINJECTION)
         extraOffset = -8; // 8 TDC units ~= 9 ns

      //utils::CopyBinContentError(hTvsA_ch, hTimeVsEnergyA_ch);
      for (Int_t biny=1; biny<=nYBins; biny++) {
         for (Int_t binx=1; binx<=nXBins; binx++) {

            Int_t binAdc = gMeasInfo->GetProtoCutAdcMin() - 1 + binx;
            Int_t binTdc = gMeasInfo->GetProtoCutTdcMin() - 1 + biny;

            // Apply the "proto slope" cut
            if ( fabs( binTdc - ( gMeasInfo->GetProtoCutSlope() * binAdc + gMeasInfo->GetProtoCutOffset() + extraOffset) ) > 20 )
               continue;

            //printf("binx, biny, slope, offset: %d, %d, %f, %f\n", binx, biny, gMeasInfo->GetProtoCutSlope(), gMeasInfo->GetProtoCutOffset());
            //if ( fabs( (Float_t) biny - ( gMeasInfo->GetProtoCutSlope() * (Float_t) binx + gMeasInfo->GetProtoCutOffset() + extraOffset) ) > 20 )
            //   continue;

            Double_t bc = hTvsA_ch->GetBinContent(binAdc, binTdc);
            Double_t be = hTvsA_ch->GetBinError(binAdc, binTdc);

            hTimeVsEnergyA_ch->SetBinContent(binx, biny, bc);
            hTimeVsEnergyA_ch->SetBinError(binx, biny, be);
         }
      }

      // Calculate cumulative histograms
      utils::ConvertToCumulative2(hTimeVsEnergyA_ch, (TH1F*) fhTimeVsEnergyACumul_ch[chId-1]);

      // Set energy-dependent noise rejection
      Float_t areaFrac1, evntFrac1, areaFrac2, evntFrac2;

      if (gMeasInfo->GetBeamEnergy() > 200) {
         // 10% of bins contain > 90% of events
         // 2%  of bins contain > 40% of events
         areaFrac1 = 0.12; evntFrac1 = 0.80; areaFrac2 = 0.02; evntFrac2 = 0.40;
      } else {
         // 15% of bins contain > 75% of events
         // 2%  of bins contain > 30% of events
         areaFrac1 = 0.15; evntFrac1 = 0.75; areaFrac2 = 0.02; evntFrac2 = 0.30;
      }

      // Add graphical representation of the above requirements to the
      // histogram stack

      TAttLine lineStyle(kViolet, 3, 4);

      TLine *line1_horz = new TLine(0, evntFrac1, areaFrac1, evntFrac1);
      TLine *line1_vert = new TLine(areaFrac1, 0, areaFrac1, evntFrac1);
      TLine *line2_horz = new TLine(0, evntFrac2, areaFrac2, evntFrac2);
      TLine *line2_vert = new TLine(areaFrac2, 0, areaFrac2, evntFrac2);

      lineStyle.Copy(*line1_horz);
      lineStyle.Copy(*line1_vert);
      lineStyle.Copy(*line2_horz);
      lineStyle.Copy(*line2_vert);

      ((TH1*) fhTimeVsEnergyACumul_ch[chId-1])->GetListOfFunctions()->Add(line1_horz);
      ((TH1*) fhTimeVsEnergyACumul_ch[chId-1])->GetListOfFunctions()->Add(line1_vert);
      ((TH1*) fhTimeVsEnergyACumul_ch[chId-1])->GetListOfFunctions()->Add(line2_horz);
      ((TH1*) fhTimeVsEnergyACumul_ch[chId-1])->GetListOfFunctions()->Add(line2_vert);

      string   sChId("  ");
      sprintf(&sChId[0], "%02d", chId);
      string shName = "noise_reject" + sChId;
      TH1 *hist = new TH2C(shName.c_str(), shName.c_str(), 1, 0, 1, 1, 0, 1);
      hist->GetListOfFunctions()->Add(line1_horz);
      hist->GetListOfFunctions()->Add(line1_vert);
      hist->GetListOfFunctions()->Add(line2_horz);
      hist->GetListOfFunctions()->Add(line2_vert);

      hNoiseReject = hist;

      // Point 1
      if (fhTimeVsEnergyACumul_ch[chId-1]->GetBinContent(areaFrac1*100) > evntFrac1) {
         gMeasInfo->DisableChannel(chId);
         continue;
      }

      // Point 2
      if (fhTimeVsEnergyACumul_ch[chId-1]->GetBinContent(areaFrac2*100) > evntFrac2) {
         gMeasInfo->DisableChannel(chId);
         continue;
      }

      // now check if the "area" (i.e. the number of bins) filled with events
      // is ridicuolosly small
      //hTvsA_ch->Print("all");
      //hTimeVsEnergyA_ch->Print("all");
      Double_t frac = utils::GetNonEmptyFraction(hTimeVsEnergyA_ch);
      Info("PostFillPassOne_FillFromRawHists", "Non empty bin fraction %f. Channel %d (disable if < 0.10)", frac, chId);

      if ( frac < 0.10 ) {
         gMeasInfo->DisableChannel(chId);
         continue;
      }
   }

   // use the last noise reject histo
   ((THStack*) o["hsTimeVsEnergyACumul"])->Add((TH1*) hNoiseReject->Clone("noise_reject_hs"));
}
