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
{ //{{{
   string shName;
   TH1*   hist;

   // Data from all enabled silicon channels
   shName = "hTimeVsEnergyA_noise";
   hist = new TH2S(shName.c_str(), shName.c_str(), 80, 100, 1700, 80, 20, 100);
   hist->SetTitle(";Deposited Energy, keV;Time, ns;");
   hist->SetOption("colz LOGZ NOIMG");
   o[shName] = hist;

   shName = "hTimeVsEnergyA";
   hist = new TH2S(shName.c_str(), shName.c_str(), 80, 100, 1700, 80, 20, 100);
   hist->SetTitle("; Deposited Energy, keV; Time, ns;");
   hist->SetOption("colz LOGZ NOIMG");
   o[shName] = hist;

   shName = "hFitMeanTimeVsEnergyA";
   hist = new TH1F(shName.c_str(), shName.c_str(), 80, 100, 1700);
   hist->SetTitle("; Deposited Energy, keV; Mean Time, ns;");
   hist->SetOption("E1 NOIMG");
   hist->GetYaxis()->SetRangeUser(10, 110);
   o[shName] = hist;

   shName = "hsTimeVsEnergyACumul";
   o[shName] = new THStack(shName.c_str(), shName.c_str());

   ChannelSetIter iCh = gMeasInfo->fSiliconChannels.begin();

   for (; iCh!=gMeasInfo->fSiliconChannels.end(); ++iCh)
   {
      UShort_t chId = *iCh;
      string   sChId("  ");
      sprintf(&sChId[0], "%02d", chId);

      // Time vs energy from amplitude
      shName = "hTimeVsEnergyA_ch" + sChId;
      hist = new TH2S(shName.c_str(), shName.c_str(), 80, 100, 1700, 80, 20, 100);
      hist->SetTitle("; Deposited Energy, keV; Time, ns;");
      hist->SetOption("colz LOGZ NOIMG");
      o[shName] = hist;
      fhTimeVsEnergyA_ch[*iCh-1] = hist;

      // Time vs energy from amplitude special binning
      shName = "hTimeVsEnergyA_raw_ch" + sChId;
      hist = new TH2S(shName.c_str(), shName.c_str(), 1, 0, 1, 1, 0, 1);
      hist->SetTitle("; Deposited Energy, keV; Time, ns;");
      hist->SetOption("colz LOGZ NOIMG");
      o[shName] = hist;
      fhTimeVsEnergyA_raw_ch[*iCh-1] = hist;

      shName = "hFitMeanTimeVsEnergyA_ch" + sChId;
      hist = new TH1F(shName.c_str(), shName.c_str(), 80, 100, 1700);
      hist->SetTitle("; Deposited Energy, keV; Mean Time, ns;");
      hist->SetOption("E1 NOIMG");
      hist->GetYaxis()->SetRangeUser(10, 110);
      o[shName] = hist;

      shName = "hFitMeanTimeVsEnergyA_raw_ch" + sChId;
      hist = new TH1F(shName.c_str(), shName.c_str(), 1, 0, 1);
      hist->SetTitle("; Deposited Energy, keV; Mean Time, ns;");
      hist->SetOption("E1 NOIMG");
      hist->GetYaxis()->SetRangeUser(10, 110);
      o[shName] = hist;
      fhFitMeanTimeVsEnergyA_raw_ch[*iCh-1] = hist;

      shName = "hTimeVsEnergyACumul_ch" + sChId;
      hist = new TH1F(shName.c_str(), shName.c_str(), 100, 0, 1);
      hist->SetOption("hist NOIMG");
      hist->SetTitle("; Digi Channel Frac; Event Frac;");
      hist->SetLineWidth(2);
      hist->SetLineColor(RunConfig::AsColor(chId));
      hist->GetYaxis()->SetRangeUser(0, 1);
      o[shName]                       = hist;
      fhTimeVsEnergyACumul_ch[chId-1] = hist;
      ((THStack*) o["hsTimeVsEnergyACumul"])->Add(hist);
   }
} //}}}


/** */
void CnipolPreprocHists::FillPassOne(ChannelEvent *ch)
{ //{{{
   UChar_t chId = ch->GetChannelId();

   fhTimeVsEnergyA_ch[chId-1] -> Fill(ch->GetEnergyA(), ch->GetTime());
} //}}}


/** */
void CnipolPreprocHists::FillDerivedPassOne()
{ //{{{
   // Fill derivative histograms first
   TH1* hTimeVsEnergyA_noise = (TH1*) o["hTimeVsEnergyA_noise"];
   
   ChannelSetIter iCh = gMeasInfo->fSiliconChannels.begin();

   for (; iCh!=gMeasInfo->fSiliconChannels.end(); ++iCh) {

      string sChId(MAX_CHANNEL_DIGITS, ' ');
      sprintf(&sChId[0], "%02d", *iCh);

      TH2* hTimeVsEnergyA_channel = (TH2*) o["hTimeVsEnergyA_ch" + sChId];
      hTimeVsEnergyA_noise->Add(hTimeVsEnergyA_channel);
   }
} //}}}


/** */
void CnipolPreprocHists::PostFillPassOne(DrawObjContainer *oc)
{ //{{{
   Info("PostFillPassOne", "Starting...");

   // We expect empty bunch histogram container of the same class
   if (!oc || oc->d.find("preproc_eb") == oc->d.end() ) {
      Error("PostFillPassOne", "No empty bunch container found");
   } else {
      CnipolPreprocHists* ebHists = (CnipolPreprocHists*) oc->d.find("preproc_eb")->second;
      PostFillPassOne_SubtractEmptyBunch(ebHists);
   }

   // A raw histogram container is required to fill TvsE histograms
   if (!oc || oc->d.find("raw") == oc->d.end() )
   {
      Error("PostFillPassOne", "No raw histogram container found");
   } else {
      CnipolRawHists *rawHists = (CnipolRawHists*) oc->d.find("raw")->second;
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

} //}}}


/** */
void CnipolPreprocHists::SaveAllAs(TCanvas &c, string pattern, string path, Bool_t thumbs)
{ //{{{
   DrawObjContainer::SaveAllAs(c, pattern, path, thumbs);

   // Draw superimposed histos

   string strThumb = thumbs ? "_thumb" : "" ;

   string cName    = "c_combo";

   TH1* h1 = (TH1*) o["hTimeVsEnergyA"];
   TH1* h2 = (TH1*) o["hFitMeanTimeVsEnergyA"];

   char *l = strstr(h1->GetOption(), "LOGZ");

   if (l) {
      c.SetLogz(kTRUE);
   } else {
      c.SetLogz(kFALSE);
   }

   c.cd();
   h1->Draw();
   h2->Draw("sames");

   c.Modified();
   c.Update();

   TPaveStats *stats = (TPaveStats*) h2->FindObject("stats");

   if (stats) {
      stats->SetX1NDC(0.84);
      stats->SetX2NDC(0.99);
      stats->SetY1NDC(0.10);
      stats->SetY2NDC(0.50);
   } else {
      printf("could not find stats\n");
      return;
   }

   string fName      = path + "/c_combo" + strThumb + ".png";
   printf("path: %s\n", fName.c_str());

   c.SetName(cName.c_str());
   c.SetTitle(cName.c_str());

   TText signature;
   signature.SetTextSize(0.03);
   signature.DrawTextNDC(0, 0.01, fSignature.c_str());

   if (TPRegexp(pattern).MatchB(fName.c_str())) {
      c.SaveAs(fName.c_str());
      gSystem->Chmod(fName.c_str(), 0775);
   } else {
      //Warning("SaveAllAs", "Histogram %s name does not match pattern. Skipped", fName.c_str());
   }

   // Draw superimposed for all channels
   ChannelSetIter iCh = gMeasInfo->fSiliconChannels.begin();

   for (; iCh!=gMeasInfo->fSiliconChannels.end(); ++iCh)
   {
      UShort_t chId = *iCh;

      string sSi("  ");
      sprintf(&sSi[0], "%02d", *iCh);
      string dName = "channel" + sSi;
      string cName = "c_combo_ch" + sSi + strThumb;

      //DrawObjContainer* oc = d.find(dName)->second;

      THStack hstack(cName.c_str(), cName.c_str());

      //TH1* h1 = (TH1*) o["hTimeVsEnergyA_ch"+sSi];
      TH1* h1 = (TH1*) fhTimeVsEnergyA_raw_ch[chId-1];
		hstack.Add(h1);

      //TH1* h2 = (TH1*) o["hFitMeanTimeVsEnergyA_ch"+sSi];
      TH1* h2 = (TH1*) fhFitMeanTimeVsEnergyA_raw_ch[chId-1];
		hstack.Add(h2);

      string subPath = path;// + "/" + dName;

		SaveHStackAs(c, hstack, subPath);
		//DrawObjContainer::SaveHStackAs(c, hstack, subPath);
   }
} //}}}


/** */
//void CnipolPreprocHists::ConvertRawToKin(TH2* hRaw, TH2* hKin, UShort_t chId)
//{ //{{{
//   for (Int_t ibx=0; ibx<=hRaw->GetNbinsX(); ++ibx) {
//      for (Int_t iby=0; iby<=hRaw->GetNbinsY(); ++iby) {
//
//         Double_t bcont  = hRaw->GetBinContent(ibx, iby);
//         Double_t bAdc   = hRaw->GetXaxis()->GetBinCenter(ibx);
//         Double_t bTdc   = hRaw->GetYaxis()->GetBinCenter(iby);
//
//         Double_t kinE   = gAsymRoot->fEventConfig->fCalibrator->GetEnergyA(bAdc, chId);
//         Double_t kinToF = gAsymRoot->fEventConfig->fCalibrator->GetTimeOfFlight(bTdc, chId);
//         
//         //Int_t bin = hKin->FindBin(kinE, kinToF);
//         hKin->Fill(kinE, kinToF, bcont);
//      }
//   }
//} //}}}


/** */
void CnipolPreprocHists::PostFillPassOne_SubtractEmptyBunch(CnipolPreprocHists *ebHists)
{ //{{{
   TH2* hTimeVsEnergyA = (TH2*) o["hTimeVsEnergyA"];

   ChannelSetIter iCh = gMeasInfo->fSiliconChannels.begin();

   for (; iCh!=gMeasInfo->fSiliconChannels.end(); ++iCh) {

      TH2* fhTimeVsEnergyA_ch_this = (TH2*) fhTimeVsEnergyA_ch[*iCh-1];
      TH2* fhTimeVsEnergyA_ch_eb   = (TH2*) ebHists->fhTimeVsEnergyA_ch[*iCh-1];

      if ( !fhTimeVsEnergyA_ch_this || !fhTimeVsEnergyA_ch_eb ) {
         Error("PostFillPassOne", "No empty bunch histogram found %s", fhTimeVsEnergyA_ch_this->GetName());
         continue;
      }

      //Double_t sumPreproc = hPreproc_ch->Integral(minAdcBin, maxAdcBin, minTdcBin, maxTdcBin);
      //Double_t sumPulser  = hPulser_ch ->Integral(minAdcBin, maxAdcBin, minTdcBin, maxTdcBin);

      //Double_t area = (maxAdcBin + 1 - minAdcBin) * (maxTdcBin + 1 - minTdcBin);

      ////printf("%d, %d, %d, %d, %d, %f, %f, %f\n", *iCh, minAdcBin, maxAdcBin, minTdcBin, maxTdcBin, area, sumPreproc, sumPulser);

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
} //}}}


/** */
void CnipolPreprocHists::PostFillPassOne_FillFromRawHists(CnipolRawHists *rawHists)
{ //{{{
   ChannelSetIter iCh = gMeasInfo->fSiliconChannels.begin();

   for (; iCh!=gMeasInfo->fSiliconChannels.end(); ++iCh)
   {
      UShort_t chId                         = *iCh;
      TH1*     hTvsA_ch                     = (TH1*) rawHists->GetHTvsA_ch(chId);
      TH1*     hTimeVsEnergyA_raw_ch        = (TH1*) fhTimeVsEnergyA_raw_ch[chId-1];
      TH1*     hFitMeanTimeVsEnergyA_raw_ch = (TH1*) fhFitMeanTimeVsEnergyA_raw_ch[chId-1];

      UShort_t adcMin = TMath::Max(gMeasInfo->GetProtoCutAdcMin(), (UShort_t) hTvsA_ch->GetXaxis()->GetXmin());
      UShort_t adcMax = TMath::Min(gMeasInfo->GetProtoCutAdcMax(), (UShort_t) hTvsA_ch->GetXaxis()->GetXmax());
      UShort_t tdcMin = TMath::Max(gMeasInfo->GetProtoCutTdcMin(), (UShort_t) hTvsA_ch->GetYaxis()->GetXmin());
      UShort_t tdcMax = TMath::Min(gMeasInfo->GetProtoCutTdcMax(), (UShort_t) hTvsA_ch->GetYaxis()->GetXmax());

      Int_t nXBins = adcMax - adcMin;
      Int_t nYBins = tdcMax - tdcMin;
      //Int_t nXBins = gMeasInfo->GetProtoCutAdcMax() - gMeasInfo->GetProtoCutAdcMin();
      //Int_t nYBins = gMeasInfo->GetProtoCutTdcMax() - gMeasInfo->GetProtoCutTdcMin();
      //Int_t nXBins = hTvsA_ch->GetNbinsX();
      //Int_t nYBins = hTvsA_ch->GetNbinsY();

      Calibrator *calibrator = gAsymRoot->GetCalibrator();

      Float_t xMin = calibrator->GetEnergyA(adcMin, chId);
      Float_t xMax = calibrator->GetEnergyA(adcMax, chId);
      Float_t yMin = calibrator->GetTime(tdcMin);
      Float_t yMax = calibrator->GetTime(tdcMax);

      //Float_t xMin = calibrator->GetEnergyA(0, chId);
      //Float_t xMax = calibrator->GetEnergyA(255, chId);
      //Float_t yMin = calibrator->GetTime(10);
      //Float_t yMax = calibrator->GetTime(90);

      hTimeVsEnergyA_raw_ch->SetBins(nXBins, xMin, xMax, nYBins, yMin, yMax);
      hFitMeanTimeVsEnergyA_raw_ch->SetBins(nXBins, xMin, xMax);

      Short_t extraOffset = 0;

      if ( UInt_t(gMeasInfo->GetBeamEnergy() + 0.5) != kINJECTION)
         extraOffset = -8; // 8 TDC units ~= 9 ns

      //utils::CopyBinContentError(hTvsA_ch, hTimeVsEnergyA_raw_ch);
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

            Double_t bc  = hTvsA_ch->GetBinContent(binAdc, binTdc);
            Double_t be  = hTvsA_ch->GetBinError(binAdc, binTdc);

            hTimeVsEnergyA_raw_ch->SetBinContent(binx, biny, bc);
            hTimeVsEnergyA_raw_ch->SetBinError(binx, biny, be);
         }
      }


      // Calculate cumulative histograms
      utils::ConvertToCumulative2(hTimeVsEnergyA_raw_ch, (TH1F*) fhTimeVsEnergyACumul_ch[chId-1]);

      // 15% of bins contain > 75% of events
      if (fhTimeVsEnergyACumul_ch[chId-1]->GetBinContent(15) > 0.75) {
         gMeasInfo->DisableChannel(chId);
         continue;
      }

      // 2% of bins contain > 30% of events - Is this a stronger requirement?
      if (fhTimeVsEnergyACumul_ch[chId-1]->GetBinContent(2) > 0.30) {
         gMeasInfo->DisableChannel(chId);
         continue;
      }
   }
} //}}}
