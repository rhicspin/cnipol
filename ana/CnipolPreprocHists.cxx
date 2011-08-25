/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolPreprocHists.h"

#include "TDirectoryFile.h"
#include "TFitResultPtr.h"
#include "TH2F.h"
#include "TH1D.h"
#include "TPaveStats.h"

#include "AnaInfo.h"
#include "CnipolPulserHists.h"
#include "RunInfo.h"

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
void CnipolPreprocHists::BookHists(string sid)
{ //{{{
   char hName[256];

   //sprintf(hName, "hTvsA");
   //o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
   //((TH1*) o[hName])->SetOption("colz LOGZ NOIMG");
   //((TH1*) o[hName])->SetTitle(";Amplitude, ADC;TDC;");

   // Data from all enabled silicon channels
   sprintf(hName, "hTimeVsEnergyA_noise");
   o[hName] = new TH2F("hTimeVsEnergyA_noise", "hTimeVsEnergyA_noise", 80, 100, 1700, 80, 20, 100);
   ((TH1*) o[hName])->SetOption("colz LOGZ NOIMG");
   ((TH1*) o[hName])->SetTitle(";Deposited Energy, keV;Time, ns;");

   sprintf(hName, "hTimeVsEnergyA");
   o[hName] = new TH2F("hTimeVsEnergyA", "hTimeVsEnergyA", 80, 100, 1700, 80, 20, 100);
   ((TH1*) o[hName])->SetOption("colz LOGZ NOIMG");
   ((TH1*) o[hName])->SetTitle(";Deposited Energy, keV;Time, ns;");

   sprintf(hName, "hFitMeanTimeVsEnergyA");
   o[hName] = new TH1D(hName, hName, 80, 100, 1700);
   ((TH1*) o[hName])->SetOption("E1 NOIMG");
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(10, 110);
   ((TH1*) o[hName])->SetTitle(";Deposited Energy, keV;Mean Time, ns;");

   ChannelSetIter iCh = gRunInfo->fSiliconChannels.begin();

   for (; iCh!=gRunInfo->fSiliconChannels.end(); ++iCh) {

      string sChId("  ");
      sprintf(&sChId[0], "%02d", *iCh);

      //sprintf(hName, "hTvsA_ch%02d", *iCh);
      //o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
      //((TH1*) o[hName])->SetOption("colz LOGZ NOIMG");
      //((TH1*) o[hName])->SetTitle(";Amplitude, ADC;TDC;");

      // Time vs Energy from amplitude
      sprintf(hName, "hTimeVsEnergyA_ch%02d", *iCh);
      o[hName] = new TH2F(hName, hName, 80, 100, 1700, 80, 20, 100);
      ((TH1*) o[hName])->SetOption("colz LOGZ NOIMG");
      ((TH1*) o[hName])->SetTitle(";Deposited Energy, keV;Time, ns;");

      sprintf(hName, "hFitMeanTimeVsEnergyA_ch%02d", *iCh);
      o[hName] = new TH1D(hName, hName, 80, 100, 1700);
      ((TH1*) o[hName])->SetOption("E1 NOIMG");
      ((TH1*) o[hName])->GetYaxis()->SetRangeUser(10, 110);
      ((TH1*) o[hName])->SetTitle(";Deposited Energy, keV;Mean Time, ns;");
   }

   // Speed up
   iCh = gRunInfo->fSiliconChannels.begin();

   for (; iCh!=gRunInfo->fSiliconChannels.end(); ++iCh) {

      string sChId("  ");
      sprintf(&sChId[0], "%02d", *iCh);

      fhTimeVsEnergyA_ch[*iCh-1] = (TH2*) o.find("hTimeVsEnergyA_ch" + sChId)->second;
   }
} //}}}


/** */
//void CnipolPreprocHists::Fill(ChannelEvent *ch, string sid)
//{ //{{{
//} //}}}


/** */
//void CnipolPreprocHists::PreFillPassOne()
//{
//}


/** */
void CnipolPreprocHists::FillPassOne(ChannelEvent *ch)
{ //{{{
   UChar_t chId  = ch->GetChannelId();

   string sChId("  ");
   sprintf(&sChId[0], "%02d", chId);

   //((TH1*) o["hTvsA_ch"          + sChId]) -> Fill(ch->GetAmpltd(), ch->GetTdc());
   //((TH1*) o.find("hTimeVsEnergyA_ch" + sChId)->second) -> Fill(ch->GetEnergyA(), ch->GetTime());
   fhTimeVsEnergyA_ch[chId-1] -> Fill(ch->GetEnergyA(), ch->GetTime());
} //}}}


/** */
void CnipolPreprocHists::FillDerivedPassOne()
{ //{{{

   // Fill derivative histograms first
   TH1* hTimeVsEnergyA_noise = (TH1*) o["hTimeVsEnergyA_noise"];
   
   ChannelSetIter iCh = gRunInfo->fSiliconChannels.begin();

   for (; iCh!=gRunInfo->fSiliconChannels.end(); ++iCh) {

      string sChId(MAX_CHANNEL_DIGITS, ' ');
      sprintf(&sChId[0], "%02d", *iCh);

      TH2* hTimeVsEnergyA_channel = (TH2*) o["hTimeVsEnergyA_ch" + sChId];
      hTimeVsEnergyA_noise->Add(hTimeVsEnergyA_channel);
   }
} //}}}


/** */
void CnipolPreprocHists::PostFillPassOne(DrawObjContainer *oc)
{ //{{{
   // We need pulser histograms to proceed
   if (!oc || oc->d.find("pulser") == oc->d.end()) {
      Error("PostFillPassOne", "No pulser container found");
      return;
   }

   CnipolPulserHists *pulserHists = (CnipolPulserHists *) oc->d.find("pulser")->second;

   //CnipolPulserHists *pulserHists = dynamic_cast<CnipolPulserHists*>(pulserHists);
   //CnipolPulserHists *pulserHists = static_cast<CnipolPulserHists*>(pulserHists);

   // Find proper normalization scale for pulser histograms
   //TH1* hPulserTdc  = (TH1*) pulserHists->o.find("hTdc")->second;
   //TH2* hPulserTvsA = (TH2*) pulserHists->o.find("hTvsA")->second;

   //TH2* hPulserTvsA = (TH2*) pulserHists->o.find("hTimeVsEnergyA")->second;
   //TH1* hPulserTdc  = hPulserTvsA->ProjectionY("hTimeVsEnergyA_projy");

   //Int_t cntTdcBin = hPulserTdc->GetMaximumBin();

   //Int_t minTdcBin = cntTdcBin - 1;
   //Int_t maxTdcBin = cntTdcBin + 1;

   //// Fit 3 bins with a gaussian func
   //TH1D* hPulserAdcMainPeak = hPulserTvsA->ProjectionX("hPulserAdcMainPeak", minTdcBin, maxTdcBin, "eo");

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

   TH1* hTimeVsEnergyA = (TH1*) o["hTimeVsEnergyA"];

   set<UShort_t>::const_iterator iCh;
   set<UShort_t>::const_iterator iChB = gRunInfo->fSiliconChannels.begin();
   set<UShort_t>::const_iterator iChE = gRunInfo->fSiliconChannels.end();

   for (iCh=iChB; iCh!=iChE; ++iCh) {

      string sChId(MAX_CHANNEL_DIGITS, ' ');
      sprintf(&sChId[0], "%02d", *iCh);

      DrawObjContainer *sdPulser  = pulserHists->d.find("channel" + sChId)->second;

      // subtract pulser from raw data
      //string hName = "hTvsA_ch" + sChId;
      // subtract pulser from time vs energy data
      string hName = "hTimeVsEnergyA_ch" + sChId;

      TH2* hPreproc_ch = (TH2*) o[hName];
      TH2* hPulser_ch  = (TH2*) sdPulser->o[hName];

      if ( !hPulser_ch ) {
         Error("PostFillPassOne", "No pulser histogram found %s", hName.c_str());
         //return;
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

      hPulser_ch->Scale(gAnaInfo->fFastCalibThinout);
      hPulser_ch->Scale( (N_BUNCHES - gRunInfo->GetNumEmptyBunches()) / (float) gRunInfo->GetNumEmptyBunches());
      hPreproc_ch->Add(hPulser_ch, -1);

      // Set negative content to 0 including under/overflows
      for (Int_t ibx=0; ibx<=hPreproc_ch->GetNbinsX()+1; ibx++) {
         for (Int_t iby=0; iby<=hPreproc_ch->GetNbinsY()+1; iby++) {

            Double_t bc = hPreproc_ch->GetBinContent(ibx, iby);

            if (bc < 0) {
               hPreproc_ch->SetBinContent(ibx, iby, 0);
               hPreproc_ch->SetBinError(ibx, iby, 0);
            }
         }
      }

      hTimeVsEnergyA->Add(hPreproc_ch);
   }
} //}}}


/** */
void CnipolPreprocHists::SaveAllAs(TCanvas &c, string pattern, string path, Bool_t thumbs)
{ //{{{
   DrawObjContainer::SaveAllAs(c, pattern, path, thumbs);

   // Draw superimposed histos

   string strThumb = thumbs ? "_thumb" : "" ;

   string cName      = "c_combo";

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
   ChannelSetIter iCh = gRunInfo->fSiliconChannels.begin();

   for (; iCh!=gRunInfo->fSiliconChannels.end(); ++iCh) {

      string sSi("  ");
      sprintf(&sSi[0], "%02d", *iCh);
      string dName = "channel" + sSi;
      string cName = "c_combo_ch" + sSi + strThumb;

      //DrawObjContainer* oc = d.find(dName)->second;

      THStack hstack(cName.c_str(), cName.c_str());

      TH1* h1 = (TH1*) o["hTimeVsEnergyA_ch"+sSi];
		hstack.Add(h1);

      TH1* h2 = (TH1*) o["hFitMeanTimeVsEnergyA_ch"+sSi];
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
