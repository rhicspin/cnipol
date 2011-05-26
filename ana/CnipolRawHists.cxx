/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolRawHists.h"

#include "RunInfo.h"


ClassImp(CnipolRawHists)

using namespace std;

/** Default constructor. */
CnipolRawHists::CnipolRawHists() : DrawObjContainer()
{
   BookHists();
}


CnipolRawHists::CnipolRawHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
}


/** Default destructor. */
CnipolRawHists::~CnipolRawHists()
{
}


/** */
void CnipolRawHists::BookHists(string cutid)
{ //{{{
   char hName[256];
   char htitle[256];

   fDir->cd();

   sprintf(hName, "hTvsA");
   o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
   ((TH1*) o[hName])->SetOption("colz LOGZ");
   ((TH1*) o[hName])->SetTitle(";Amplitude, ADC;TDC;");

   sprintf(hName, "hTvsI");
   o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
   ((TH1*) o[hName])->SetOption("colz LOGZ NOIMG");
   ((TH1*) o[hName])->SetTitle(";Integral, ADC;TDC;");

   sprintf(hName, "bunch_dist_raw");
   sprintf(htitle, "%.3f: Raw Counts per Bunch ", gRunInfo->RUNID);
   o[hName] = new TH1F(hName, htitle, NBUNCH, -0.5, NBUNCH-0.5);
   ((TH2F*) o[hName])->GetXaxis()->SetTitle("Bunch Id");
   ((TH2F*) o[hName])->GetYaxis()->SetTitle("Events");
   ((TH2F*) o[hName])->SetFillColor(kGray);

   sprintf(hName, "strip_dist_raw");
   sprintf(htitle, "%.3f: Raw Counts per Strip ", gRunInfo->RUNID);
   o[hName] = new TH1F(hName, htitle, NSTRIP, 1, NSTRIP);
   ((TH2F*) o[hName])->GetXaxis()->SetTitle("Strip Id");
   ((TH2F*) o[hName])->GetYaxis()->SetTitle("Events");
   ((TH2F*) o[hName])->SetFillColor(kGray);

   sprintf(hName, "tdc_raw");
   sprintf(htitle, "%.3f: Raw TDC (All Strips)", gRunInfo->RUNID);
   o[hName] = new TH1F(hName, htitle, 100, 0, 100);
   ((TH2F*) o[hName])->GetXaxis()->SetTitle("TDC");
   ((TH2F*) o[hName])->GetYaxis()->SetTitle("Events");
   ((TH2F*) o[hName])->SetFillColor(kGray);

   sprintf(hName, "tdc_vs_adc_false_bunch_raw");
   sprintf(htitle, "%.3f: Raw TDC vs ADC (All Strips) false bunch", gRunInfo->RUNID);
   o[hName] = new TH2F(hName, htitle, 255, 0, 255, 100, 0, 100);
   ((TH2F*) o[hName])->GetXaxis()->SetTitle("ADC");
   ((TH2F*) o[hName])->GetYaxis()->SetTitle("TDC");
   ((TH2F*) o[hName])->SetFillColor(kRed);

   DrawObjContainer        *oc;
   DrawObjContainerMapIter  isubdir;

   for (int iChId=1; iChId<=N_SILICON_CHANNELS; iChId++) {

      string sChId("  ");
      sprintf(&sChId[0], "%02d", iChId);

      string dName = "channel" + sChId;

      isubdir = d.find(dName);

      if ( isubdir == d.end()) { // if dir not found
         oc = new DrawObjContainer();
         oc->fDir = new TDirectoryFile(dName.c_str(), dName.c_str(), "", fDir);
      } else {
         oc = isubdir->second;
      }

      sprintf(hName, "hTvsA_ch%02d", iChId);
      oc->o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
      ((TH1*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH1*) oc->o[hName])->SetTitle(";Amplitude, ADC;TDC;");

      sprintf(hName, "hTvsI_ch%02d", iChId);
      oc->o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
      ((TH1*) oc->o[hName])->SetOption("colz LOGZ NOIMG");
      ((TH1*) oc->o[hName])->SetTitle(";Integral, ADC;TDC;");

      // If this is a new directory then we need to add it to the list
      if ( isubdir == d.end()) {
         d[dName] = oc;
      }
   }
} //}}}


/** */
void CnipolRawHists::Fill(ChannelEvent *ch, string cutid)
{ //{{{
   // Fill events with no cuts applied
   if (cutid != "") return;

   UChar_t chId  = ch->GetChannelId();

   string sChId("  ");
   sprintf(&sChId[0], "%02d", chId);

   DrawObjContainer *sd = d["channel"+sChId];

   ((TH1*) sd->o["hTvsA_ch"+sChId]) -> Fill(ch->GetAmpltd(), ch->GetTdc());
   ((TH1*) sd->o["hTvsI_ch"+sChId]) -> Fill(ch->GetIntgrl(), ch->GetTdc());

   ((TH1*) o["bunch_dist_raw"])->Fill(ch->GetChannelId());
   ((TH1*) o["strip_dist_raw"])->Fill(ch->GetBunchId());
   ((TH1*) o["tdc_raw"])       ->Fill(ch->GetTdc());
   ((TH1*) o["adc_raw"])       ->Fill(ch->GetAmpltd());

   if (cutid == "cut_false_bunch")
      ((TH1*) o["tdc_vs_adc_false_bunch_raw"])->Fill(ch->GetAmpltd(), ch->GetTdc());

} //}}}


/** */
void CnipolRawHists::PostFill()
{
   TH1* hTvsA = (TH1*) o["hTvsA"];
   TH1* hTvsI = (TH1*) o["hTvsI"];
   
   for (UShort_t iCh=1; iCh<=N_SILICON_CHANNELS; iCh++) {

      string sChId("  ");
      sprintf(&sChId[0], "%02d", iCh);

      DrawObjContainer *oc = d.find("channel"+sChId)->second;

      TH2* hTVsA_channel = (TH2*) oc->o["hTvsA_ch"+sChId];
      hTvsA->Add(hTVsA_channel);

      TH2* hTVsI_channel = (TH2*) oc->o["hTvsI_ch"+sChId];
      hTvsI->Add(hTVsI_channel);
   }
} //}}}
