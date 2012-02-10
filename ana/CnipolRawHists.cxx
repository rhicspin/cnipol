/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolRawHists.h"

#include "utils/utils.h"


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
   string shName;

   TH1* hist;

   fDir->cd();

   sprintf(hName, "hAdcAmpltd"); // former adc_raw
   o[hName] = new TH1F(hName, hName, 255, 0, 255);
   ((TH1*) o[hName])->SetTitle(";Amplitude, ADC;Events;");
   ((TH1*) o[hName])->SetOption("hist");
   ((TH1*) o[hName])->SetFillColor(kGray);

   sprintf(hName, "hAdcIntgrl");
   o[hName] = new TH1F(hName, hName, 255, 0, 255);
   ((TH1*) o[hName])->SetTitle(";Integral, ADC;Events;");
   ((TH1*) o[hName])->SetOption("hist");
   ((TH1*) o[hName])->SetFillColor(kGray);

   sprintf(hName, "hTdc"); // former tdc_raw
   o[hName] = new TH1F(hName, hName, 80, 10, 90);
   ((TH1*) o[hName])->SetOption("hist");
   ((TH1*) o[hName])->SetTitle(";TDC;Events;");
   ((TH1*) o[hName])->SetFillColor(kGray);

   sprintf(hName, "hTvsA");
   o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
   ((TH1*) o[hName])->SetTitle(";Amplitude, ADC;TDC;");
   ((TH1*) o[hName])->SetOption("colz LOGZ");

   sprintf(hName, "hTvsI");
   o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
   ((TH1*) o[hName])->SetTitle(";Integral, ADC;TDC;");
   ((TH1*) o[hName])->SetOption("colz LOGZ");

   sprintf(hName, "hIvsA");
   o[hName] = new TH2F(hName, hName, 255, 0, 255, 255, 0, 255);
   ((TH1*) o[hName])->SetTitle(";Integral, ADC;TDC;");
   ((TH1*) o[hName])->SetOption("colz LOGZ");

   sprintf(hName, "hBunchCounts"); //former bunch_dist_raw
   o[hName] = new TH1F(hName, hName, N_BUNCHES, 0.5, N_BUNCHES+0.5);
   ((TH1*) o[hName])->SetTitle(";Bunch Id;Events;");
   ((TH1*) o[hName])->SetOption("colz LOGZ GRIDX");
   ((TH1*) o[hName])->SetFillColor(kGray);

   sprintf(hName, "hStripCounts"); // former strip_dist_raw
   o[hName] = new TH1F(hName, hName, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   ((TH1*) o[hName])->SetOption("hist XY GRIDX");
   ((TH1*) o[hName])->SetTitle(";Channel Id;Events;");
   ((TH1*) o[hName])->SetFillColor(kGray);

   shName = "hRevolutionId";
   hist = new TH1F(shName.c_str(), shName.c_str(), 1000, 0, 1);
   hist->SetTitle(";Revolution Id;Events;");
   hist->SetFillColor(kGray);
   hist->SetBit(TH1::kCanRebin);
   o[shName] = hist;

   DrawObjContainer        *oc;
   DrawObjContainerMapIter  isubdir;

   for (int iChId=1; iChId<=N_SILICON_CHANNELS; iChId++)
   {
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

      sprintf(hName, "hAdcIntgrl_ch%02d", iChId);
      oc->o[hName] = new TH1F(hName, hName, 255, 0, 255);
      ((TH1*) oc->o[hName])->SetOption("hist NOIMG");
      ((TH1*) oc->o[hName])->SetTitle(";Integral, ADC;Events;");
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

      sprintf(hName, "hIvsA_ch%02d", iChId);
      oc->o[hName] = new TH2F(hName, hName, 255, 0, 255, 255, 0, 255);
      ((TH1*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH1*) oc->o[hName])->SetTitle(";Amplitude, ADC;Integral, ADC;");

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

   UChar_t  chId     = ch->GetChannelId();
   UShort_t adcA_bin = ch->GetAmpltd() + 1;
   UShort_t adcI_bin = ch->GetIntgrl() + 1;
   UShort_t tdc_bin  = ch->GetTdc() - 10 + 1;

   string sChId("  ");
   sprintf(&sChId[0], "%02d", chId);

   DrawObjContainer *sd = d["channel" + sChId];

   // Speed up the filling process by getting the global bin number
   TH2* hTmp_ch;
   Int_t gbin;

   hTmp_ch = (TH2*) sd->o["hTvsA_ch" + sChId];
   gbin    = hTmp_ch->GetBin(adcA_bin, tdc_bin);
   hTmp_ch->AddBinContent(gbin);
   hTmp_ch->SetEntries(hTmp_ch->GetEntries()+1);

   hTmp_ch = (TH2*) sd->o["hTvsI_ch" + sChId];
   gbin    = hTmp_ch->GetBin(adcI_bin, tdc_bin);
   hTmp_ch->AddBinContent(gbin);
   hTmp_ch->SetEntries(hTmp_ch->GetEntries()+1);

   hTmp_ch = (TH2*) sd->o["hIvsA_ch" + sChId];
   gbin    = hTmp_ch->GetBin(adcA_bin, adcI_bin);
   hTmp_ch->AddBinContent(gbin);
   hTmp_ch->SetEntries(hTmp_ch->GetEntries()+1);

   //((TH1*) sd->o["hAdcAmpltd_ch" + sChId]) -> Fill(ch->GetAmpltd());
   //((TH1*) sd->o["hTdc_ch"       + sChId]) -> Fill(ch->GetTdc());
   //((TH1*) sd->o["hTvsA_ch" + sChId]) -> Fill(adcA, tdc);
   //((TH1*) sd->o["hTvsI_ch" + sChId]) -> Fill(adcI, tdc);
   //((TH1*) sd->o["hIvsA_ch" + sChId]) -> Fill(adcA, adcI);

   TH1* hist;

   hist = (TH1*) o["hBunchCounts"];
   hist->AddBinContent(ch->GetBunchId() + 1);
   hist->SetEntries(hist->GetEntries()+1);

   hist = ((TH1*) o["hStripCounts"]);
   hist->AddBinContent(chId);
   hist->SetEntries(hist->GetEntries()+1);

   hist = ((TH1*) o["hRevolutionId"]);
   hist->Fill(ch->GetRevolutionId());

} //}}}


/** */
void CnipolRawHists::FillDerived()
{ //{{{
   TH1* hAdcAmpltd = (TH1*) o["hAdcAmpltd"];
   TH1* hAdcIntgrl = (TH1*) o["hAdcIntgrl"];
   TH1* hTdc       = (TH1*) o["hTdc"];
   TH1* hTvsA      = (TH1*) o["hTvsA"];
   TH1* hTvsI      = (TH1*) o["hTvsI"];
   TH1* hIvsA      = (TH1*) o["hIvsA"];
   
   for (UShort_t iCh=1; iCh<=N_SILICON_CHANNELS; iCh++) {

      string sChId("  ");
      sprintf(&sChId[0], "%02d", iCh);

      DrawObjContainer *oc = d.find("channel"+sChId)->second;

      TH2* hTVsA_channel = (TH2*) oc->o["hTvsA_ch" + sChId];
      hTVsA_channel->Sumw2();
      hTvsA->Add(hTVsA_channel);

      TH2* hTVsI_channel = (TH2*) oc->o["hTvsI_ch" + sChId];
      hTvsI->Add(hTVsI_channel);

      TH2* hIVsA_channel = (TH2*) oc->o["hIvsA_ch" + sChId];
      hIvsA->Add(hIVsA_channel);

      // Create projections of
      TH1D* hProjTmp;

      // AdcAmplitude, ...
      TH1* hAdcAmpltd_channel = (TH1*) oc->o["hAdcAmpltd_ch" + sChId];
      hProjTmp = hTVsA_channel->ProjectionX();

      utils::CopyBinContentError(hProjTmp, hAdcAmpltd_channel);

      hAdcAmpltd->Add(hAdcAmpltd_channel);

      //  ... AdcIntegral, ...
      TH1* hAdcIntgrl_channel = (TH1*) oc->o["hAdcIntgrl_ch" + sChId];
      hProjTmp = hTVsI_channel->ProjectionX();

      utils::CopyBinContentError(hProjTmp, hAdcIntgrl_channel);

      hAdcIntgrl->Add(hAdcIntgrl_channel);

      // ... TDC
      TH1* hTdc_channel = (TH1*) oc->o["hTdc_ch" + sChId];
      hProjTmp = hTVsA_channel->ProjectionY();

      utils::CopyBinContentError(hProjTmp, hTdc_channel);

      hTdc->Add(hTdc_channel);
   }
} //}}}
