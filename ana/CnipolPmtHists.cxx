/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolPmtHists.h"

#include "RunInfo.h"


ClassImp(CnipolPmtHists)

using namespace std;

/** Default constructor. */
CnipolPmtHists::CnipolPmtHists() : DrawObjContainer()
{
   BookHists();
}


CnipolPmtHists::CnipolPmtHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
}


/** Default destructor. */
CnipolPmtHists::~CnipolPmtHists()
{
}


/** */
void CnipolPmtHists::BookHists(string cutid)
{ //{{{
   char hName[256];

   fDir->cd();

   sprintf(hName, "hTvsA");
   o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
   ((TH1*) o[hName])->SetOption("colz LOGZ NOIMG");
   ((TH1*) o[hName])->SetTitle(";Amplitude, ADC;TDC;");

   sprintf(hName, "hTvsI");
   o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
   ((TH1*) o[hName])->SetOption("colz LOGZ NOIMG");
   ((TH1*) o[hName])->SetTitle(";Integral, ADC;TDC;");

   char htitle[256];

   sprintf(hName, "bunch_dist_raw");
   sprintf(htitle, "%.3f: Raw Counts per Bunch ", gRunInfo->RUNID);
   o[hName] = new TH1F(hName, htitle, NBUNCH, -0.5, NBUNCH-0.5);
   ((TH1*) o[hName])->SetOption("NOIMG");
   ((TH1*) o[hName])->SetTitle(";Bunch Id;Events;");
   ((TH1*) o[hName])->SetFillColor(kGray);

   sprintf(hName, "strip_dist_raw");
   sprintf(htitle, "%.3f: Raw Counts per Strip ", gRunInfo->RUNID);
   o[hName] = new TH1F(hName, htitle, NSTRIP, 1, NSTRIP);
   ((TH1*) o[hName])->SetOption("NOIMG");
   ((TH1*) o[hName])->SetTitle(";Strip Id;Events;");
   ((TH1*) o[hName])->SetFillColor(kGray);

   sprintf(hName, "tdc_vs_adc_false_bunch_raw");
   sprintf(htitle, "%.3f: Raw TDC vs ADC (All Strips) false bunch", gRunInfo->RUNID);
   o[hName] = new TH2F(hName, htitle, 255, 0, 255, 100, 0, 100);
   ((TH1*) o[hName])->SetOption("NOIMG");
   ((TH1*) o[hName])->SetTitle(";ADC;TDC;");
   ((TH1*) o[hName])->SetFillColor(kRed);

   DrawObjContainer        *oc;
   DrawObjContainerMapIter  isubdir;

   for (int iChId=N_SILICON_CHANNELS+1; iChId<=N_SILICON_CHANNELS+4; iChId++) {

      string sChId("  ");
      sprintf(&sChId[0], "%02d", iChId);

      sprintf(hName, "hTdc_ch%02d", iChId);
      o[hName] = new TH1F(hName, hName, 100, 0, 100);
      ((TH1*) o[hName])->SetOption("colz LOGZ NOIMG");
      ((TH1*) o[hName])->SetTitle(";TDC;Events;");
      ((TH1*) o[hName])->SetFillColor(kGray);

      sprintf(hName, "hAdc_ch%02d", iChId);
      o[hName] = new TH1F(hName, hName, 255, 0, 255);
      ((TH1*) o[hName])->SetOption("colz LOGZ NOIMG");
      ((TH1*) o[hName])->SetTitle(";ADC;Events;");
      ((TH1*) o[hName])->SetFillColor(kGray);

      sprintf(hName, "hTvsA_ch%02d", iChId);
      o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
      ((TH1*) o[hName])->SetOption("colz LOGZ NOIMG");
      ((TH1*) o[hName])->SetTitle(";Amplitude, ADC;TDC;");

      sprintf(hName, "hTvsI_ch%02d", iChId);
      o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
      ((TH1*) o[hName])->SetOption("colz LOGZ NOIMG");
      ((TH1*) o[hName])->SetTitle(";Integral, ADC;TDC;");
   }
} //}}}


/** */
void CnipolPmtHists::FillPreProcess(ChannelEvent *ch)
{ //{{{
   // Fill events with no cuts applied
   //if (cutid != "") return;

   UChar_t chId  = ch->GetChannelId();

   string sChId("  ");
   sprintf(&sChId[0], "%02d", chId);

   //DrawObjContainer *sd = d["channel"+sChId];

   ((TH1*) o["hAdc_ch"+sChId])  -> Fill(ch->GetAmpltd());
   ((TH1*) o["hTdc_ch"+sChId])  -> Fill(ch->GetTime2());
   ((TH1*) o["hTvsA_ch"+sChId]) -> Fill(ch->GetAmpltd(), ch->GetTime2());
   ((TH1*) o["hTvsI_ch"+sChId]) -> Fill(ch->GetIntgrl(), ch->GetTime2());

   ((TH1*) o["bunch_dist_raw"])->Fill(ch->GetChannelId());
   ((TH1*) o["strip_dist_raw"])->Fill(ch->GetBunchId());

   //if (cutid == "cut_false_bunch")
   //   ((TH1*) o["tdc_vs_adc_false_bunch_raw"])->Fill(ch->GetAmpltd(), ch->GetTime2());

} //}}}


/** */
void CnipolPmtHists::PostFill()
{ //{{{
   //TH1* hTvsA = (TH1*) o["hTvsA"];
   //TH1* hTvsI = (TH1*) o["hTvsI"];
   //
   //for (UShort_t iCh=1; iCh<=N_SILICON_CHANNELS; iCh++) {

   //   string sChId("  ");
   //   sprintf(&sChId[0], "%02d", iCh);

   //   DrawObjContainer *oc = d.find("channel"+sChId)->second;

   //   TH2* hTVsA_channel = (TH2*) oc->o["hTvsA_ch"+sChId];
   //   hTvsA->Add(hTVsA_channel);

   //   TH2* hTVsI_channel = (TH2*) oc->o["hTvsI_ch"+sChId];
   //   hTvsI->Add(hTVsI_channel);
   //}
} //}}}


/** */
void CnipolPmtHists::PostPreProcess()
{ //{{{

   for (int iChId=N_SILICON_CHANNELS+1; iChId<=N_SILICON_CHANNELS+4; iChId++) {

      string sChId("  ");
      sprintf(&sChId[0], "%02d", iChId);

      //sprintf(hName, "hTdc_ch%02d", iChId);
      TFitResultPtr fitres = ((TH1*) o["hTdc_ch"+sChId])->Fit("gaus", "I M S R");

      if (fitres.Get()) {
         //chi2Ndf  = fitres->Ndf() > 0 ? fitres->Chi2()/fitres->Ndf() : -1;
         //sigma    = fitres->Value(0);
         if (iChId == 73) {
            gAnaResult->fPmtV1T0    = fitres->Value(1);
            gAnaResult->fPmtV1T0Err = fitres->FitResult::Error(1);
         } 

         if (iChId == 75) {
            gAnaResult->fPmtS1T0    = fitres->Value(1);
            gAnaResult->fPmtS1T0Err = fitres->FitResult::Error(1);
         } 
         //mean2    = fitres->Value(2);
         //mean2Err = fitres->FitResult::Error(2);
      }
   }

} //}}}
