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

#include "AnaInfo.h"
#include "CnipolPulserHists.h"
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
void CnipolPreprocHists::BookHists(string sid)
{ //{{{
   string shName;
   TH1*   hist;

   // Data from all enabled silicon channels
   shName = "hTimeVsEnergyA_noise";
   hist = new TH2F(shName.c_str(), shName.c_str(), 80, 100, 1700, 80, 20, 100);
   hist->SetTitle(";Deposited Energy, keV;Time, ns;");
   hist->SetOption("colz LOGZ NOIMG");
   o[shName] = hist;

   shName = "hTimeVsEnergyA";
   hist = new TH2F(shName.c_str(), shName.c_str(), 80, 100, 1700, 80, 20, 100);
   hist->SetTitle("; Deposited Energy, keV; Time, ns;");
   hist->SetOption("colz LOGZ NOIMG");
   o[shName] = hist;

   shName = "hFitMeanTimeVsEnergyA";
   hist = new TH1F(shName.c_str(), shName.c_str(), 80, 100, 1700);
   hist->SetTitle("; Deposited Energy, keV; Mean Time, ns;");
   hist->SetOption("E1 NOIMG");
   hist->GetYaxis()->SetRangeUser(10, 110);
   o[shName] = hist;

   ChannelSetIter iCh = gMeasInfo->fSiliconChannels.begin();

   for (; iCh!=gMeasInfo->fSiliconChannels.end(); ++iCh)
   {
      string sChId("  ");
      sprintf(&sChId[0], "%02d", *iCh);

      // Time vs Energy from amplitude
      shName = "hTimeVsEnergyA_ch" + sChId;
      hist = new TH2F(shName.c_str(), shName.c_str(), 80, 100, 1700, 80, 20, 100);
      hist->SetTitle("; Deposited Energy, keV; Time, ns;");
      hist->SetOption("colz LOGZ NOIMG");
      o[shName] = hist;
      fhTimeVsEnergyA_ch[*iCh-1] = hist;

      shName = "hFitMeanTimeVsEnergyA_ch" + sChId;
      hist = new TH1F(shName.c_str(), shName.c_str(), 80, 100, 1700);
      hist->SetTitle("; Deposited Energy, keV; Mean Time, ns;");
      hist->SetOption("E1 NOIMG");
      hist->GetYaxis()->SetRangeUser(10, 110);
      o[shName] = hist;
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
   // We need pulser histograms to proceed
   //if (!oc || oc->d.find("pulser") == oc->d.end())
   // We expect empty bunch histogram container of the same class
   if (!oc) {
      Error("PostFillPassOne", "No empty bunch container found");
      return;
   }

   CnipolPreprocHists* ebHists = (CnipolPreprocHists*) oc;

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

   TH2* hTimeVsEnergyA = (TH2*) o["hTimeVsEnergyA"];

   ChannelSetIter iCh = gMeasInfo->fSiliconChannels.begin();

   for (; iCh!=gMeasInfo->fSiliconChannels.end(); ++iCh) {

      TH2* fhTimeVsEnergyA_ch_this = (TH2*) fhTimeVsEnergyA_ch[*iCh-1];
      TH2* fhTimeVsEnergyA_ch_eb   = (TH2*) ebHists->fhTimeVsEnergyA_ch[*iCh-1];

      if ( !fhTimeVsEnergyA_ch_this || !fhTimeVsEnergyA_ch_eb ) {
         Error("PostFillPassOne", "No pulser histogram found %s", fhTimeVsEnergyA_ch_this->GetName());
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
      fhTimeVsEnergyA_ch_eb->Scale( (N_BUNCHES - gMeasInfo->GetNumEmptyBunches()) / (float) gMeasInfo->GetNumEmptyBunches());
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

   for (; iCh!=gMeasInfo->fSiliconChannels.end(); ++iCh) {

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
