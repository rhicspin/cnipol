/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolPreprocHists.h"

#include "TDirectoryFile.h"
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

   sprintf(hName, "hTvsA");
   o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
   ((TH1*) o[hName])->SetOption("colz LOGZ NOIMG");
   ((TH1*) o[hName])->SetTitle(";Amplitude, ADC;TDC;");

   // Data from all enabled silicon channels
   sprintf(hName, "hTimeVsEnergyA");
   o[hName] = new TH2F("hTimeVsEnergyA", "hTimeVsEnergyA", 100, 0, 2000, 50, 10, 110);
   ((TH1*) o[hName])->SetOption("colz LOGZ NOIMG");
   ((TH1*) o[hName])->SetTitle(";Deposited Energy, keV;Time, ns;");

   sprintf(hName, "hFitMeanTimeVsEnergyA");
   o[hName] = new TH1D(hName, hName, 100, 0, 2000);
   ((TH1*) o[hName])->SetOption("E1 NOIMG");
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(10, 110);
   ((TH1*) o[hName])->SetTitle(";Deposited Energy, keV;Mean Time, ns;");

   for (UShort_t iCh=1; iCh<=N_SILICON_CHANNELS; iCh++) {

      string sChId("  ");

      sprintf(&sChId[0], "%02d", iCh);

      sprintf(hName, "hTvsA_ch%02d", iCh);
      o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
      ((TH1*) o[hName])->SetOption("colz LOGZ NOIMG");
      ((TH1*) o[hName])->SetTitle(";Amplitude, ADC;TDC;");

      // Time vs Energy from amplitude
      sprintf(hName, "hTimeVsEnergyA_ch%02d", iCh);
      o[hName] = new TH2F(hName, hName, 100, 0, 2000, 50, 10, 110);
      ((TH1*) o[hName])->SetOption("colz LOGZ NOIMG");
      ((TH1*) o[hName])->SetTitle(";Deposited Energy, keV;Time, ns;");

      sprintf(hName, "hFitMeanTimeVsEnergyA_ch%02d", iCh);
      o[hName] = new TH1D(hName, hName, 100, 0, 2000);
      ((TH1*) o[hName])->SetOption("E1 NOIMG");
      ((TH1*) o[hName])->GetYaxis()->SetRangeUser(10, 110);
      ((TH1*) o[hName])->SetTitle(";Deposited Energy, keV;Mean Time, ns;");
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
   ((TH1*) o["hTvsA"])->Fill(ch->GetAmpltd(), ch->GetTdc());
   ((TH1*) o["hTimeVsEnergyA"])->Fill(ch->GetEnergyA(), ch->GetTime());

   UChar_t chId  = ch->GetChannelId();

   string sChId("  ");
   sprintf(&sChId[0], "%02d", chId);

   ((TH1*) o["hTvsA_ch"          + sChId]) -> Fill(ch->GetAmpltd(), ch->GetTdc());
   ((TH1*) o["hTimeVsEnergyA_ch" + sChId]) -> Fill(ch->GetEnergyA(), ch->GetTime());
} //}}}


/** */
void CnipolPreprocHists::PostFillPassOne(DrawObjContainer *oc)
{
   if (!oc) return;

   //CnipolPulserHists *pulserHists = dynamic_cast<CnipolPulserHists*>(oc);
   //CnipolPulserHists *pulserHists = static_cast<CnipolPulserHists*>(oc);

   set<UShort_t>::const_iterator iCh;
   set<UShort_t>::const_iterator iChB = gRunInfo->fSiliconChannels.begin();
   set<UShort_t>::const_iterator iChE = gRunInfo->fSiliconChannels.end();

   for (iCh=iChB; iCh!=iChE; ++iCh) {

      string sCh(MAX_CHANNEL_DIGITS, ' ');
      sprintf(&sCh[0], "%02d", *iCh);

      string dName = "channel" + sCh;

      //DrawObjContainer *sdPreproc = d.find(dName)->second;
      //DrawObjContainer *sdPulser  = pulserHists->d.find(dName)->second;
      DrawObjContainer *sdPulser  = oc->d.find(dName)->second;

      string hName = "hTimeVsEnergyA_ch" + sCh;
      //string hName = "hTvsA_ch" + sCh;

      TH1* hPreproc = (TH1*) o[hName];
      TH1* hPulser  = (TH1*) sdPulser->o[hName];

      hPulser->Scale(gAnaInfo->fFastCalibThinout);
      hPulser->Scale(N_BUNCHES / (float) gRunInfo->GetNumEmptyBunches());
      hPreproc->Add(hPulser, -1);
   }
}


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
   set<UShort_t>::const_iterator iCh;
   set<UShort_t>::const_iterator iChB = gRunInfo->fSiliconChannels.begin();
   set<UShort_t>::const_iterator iChE = gRunInfo->fSiliconChannels.end();

   for (iCh=iChB; iCh!=iChE; ++iCh)
	{
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
