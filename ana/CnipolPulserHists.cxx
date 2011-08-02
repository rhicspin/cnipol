/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolPulserHists.h"


ClassImp(CnipolPulserHists)

using namespace std;

/** Default constructor. */
CnipolPulserHists::CnipolPulserHists() : DrawObjContainer()
{
   BookHists();
}


CnipolPulserHists::CnipolPulserHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
}


/** Default destructor. */
CnipolPulserHists::~CnipolPulserHists()
{
}


/** */
void CnipolPulserHists::BookHists(string cutid)
{ //{{{
   char hName[256];

   fDir->cd();

   sprintf(hName, "hAdcAmpltd"); // former adc_raw
   o[hName] = new TH1F(hName, hName, 255, 0, 255);
   ((TH1*) o[hName])->SetTitle("Raw ADC (All strips);Amplitude, ADC;Events;");
   ((TH1*) o[hName])->SetFillColor(kGray);

   sprintf(hName, "hTdc"); // former tdc_raw
   o[hName] = new TH1F(hName, hName, 80, 10, 90);
   ((TH1*) o[hName])->SetTitle("Raw TDC (All strips);TDC;Events;");
   ((TH1*) o[hName])->SetFillColor(kGray);

   sprintf(hName, "hTvsA");
   o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
   ((TH1*) o[hName])->SetOption("colz LOGZ");
   ((TH1*) o[hName])->SetTitle(";Amplitude, ADC;TDC;");

   sprintf(hName, "hTvsI");
   o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
   ((TH1*) o[hName])->SetOption("colz LOGZ");
   ((TH1*) o[hName])->SetTitle(";Integral, ADC;TDC;");

   sprintf(hName, "hBunchCounts"); //former bunch_dist_raw
   o[hName] = new TH1F(hName, hName, N_BUNCHES, -0.5, N_BUNCHES-0.5);
   ((TH1*) o[hName])->SetTitle(";Bunch Id;Events;");
   ((TH1*) o[hName])->SetFillColor(kGray);

   sprintf(hName, "hStripCounts"); // former strip_dist_raw
   o[hName] = new TH1F(hName, hName, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   //((TH1*) o[hName])->SetOption("hist");
   ((TH1*) o[hName])->SetTitle(";Channel Id;Events;");
   ((TH1*) o[hName])->SetFillColor(kGray);

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

      sprintf(hName, "hAdcAmpltd_ch%02d", iChId);
      oc->o[hName] = new TH1F(hName, hName, 255, 0, 255);
      ((TH1*) oc->o[hName])->SetOption("hist NOIMG");
      ((TH1*) oc->o[hName])->SetTitle(";Amplitude, ADC;Events;");
      ((TH1*) oc->o[hName])->SetFillColor(kGray);

      sprintf(hName, "hTdc_ch%02d", iChId);
      oc->o[hName] = new TH1F(hName, hName, 80, 10, 90);
      ((TH1*) oc->o[hName])->SetOption("hist NOIMG");
      ((TH1*) oc->o[hName])->SetTitle(";TDC;Events;");
      ((TH1*) oc->o[hName])->SetFillColor(kGray);

      sprintf(hName, "hTvsA_ch%02d", iChId);
      oc->o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
      ((TH1*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH1*) oc->o[hName])->SetTitle(";Amplitude, ADC;TDC;");

      sprintf(hName, "hTvsI_ch%02d", iChId);
      oc->o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
      ((TH1*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH1*) oc->o[hName])->SetTitle(";Integral, ADC;TDC;");

      // If this is a new directory then we need to add it to the list
      if ( isubdir == d.end()) {
         d[dName] = oc;
      }
   }
} //}}}


/** */
void CnipolPulserHists::FillPreProcess(ChannelEvent *ch)
{ //{{{
   // Fill events with no cuts applied
   //if (cutid != "empty_bunch") return;

   UChar_t chId  = ch->GetChannelId();

   string sChId("  ");
   sprintf(&sChId[0], "%02d", chId);

   DrawObjContainer *sd = d["channel"+sChId];

   ((TH1*) sd->o["hAdcAmpltd_ch"+sChId]) -> Fill(ch->GetAmpltd());
   ((TH1*) sd->o["hTdc_ch"      +sChId]) -> Fill(ch->GetTdc());
   ((TH1*) sd->o["hTvsA_ch"     +sChId]) -> Fill(ch->GetAmpltd(), ch->GetTdc());
   ((TH1*) sd->o["hTvsI_ch"     +sChId]) -> Fill(ch->GetIntgrl(), ch->GetTdc());

   ((TH1*) o["hBunchCounts"])->Fill(ch->GetBunchId());
   ((TH1*) o["hStripCounts"])->Fill(ch->GetChannelId());

} //}}}


/** */
void CnipolPulserHists::PostFill()
{ //{{{
   TH1* hAdcAmpltd  = (TH1*) o["hAdcAmpltd"];
   TH1* hTdc  = (TH1*) o["hTdc"];
   TH1* hTvsA = (TH1*) o["hTvsA"];
   TH1* hTvsI = (TH1*) o["hTvsI"];
   
   for (UShort_t iCh=1; iCh<=N_SILICON_CHANNELS; iCh++) {

      string sChId("  ");
      sprintf(&sChId[0], "%02d", iCh);

      DrawObjContainer *oc = d.find("channel"+sChId)->second;

      TH1* hAdc_channel  = (TH1*) oc->o["hAdcAmpltd_ch"+sChId];
      hAdcAmpltd->Add(hAdc_channel);

      TH1* hTdc_channel  = (TH1*) oc->o["hTdc_ch"+sChId];
      hTdc->Add(hTdc_channel);

      TH2* hTVsA_channel = (TH2*) oc->o["hTvsA_ch"+sChId];
      hTvsA->Add(hTVsA_channel);

      TH2* hTVsI_channel = (TH2*) oc->o["hTvsI_ch"+sChId];
      hTvsI->Add(hTVsI_channel);
   }
} //}}}
