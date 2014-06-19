#include "CnipolAlphaHists.h"
#include "ChannelEventId.h"
#include <fstream>

#include "utils/utils.h"

ClassImp(CnipolAlphaHists)

using namespace std;


/** Default constructor. */
CnipolAlphaHists::CnipolAlphaHists() : DrawObjContainer()
{
   BookHists();
}


/** */
CnipolAlphaHists::CnipolAlphaHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
}


/** Default destructor. */
CnipolAlphaHists::~CnipolAlphaHists()
{
}


void CnipolAlphaHists::BookHists()
{
   char hName[256];

   fDir->cd();

   //o["hAmpltd"]      = new TH1F("hAmpltd", "hAmpltd", 255, 0, 255);
   //((TH2F*) o["hAmpltd"])->GetXaxis()->SetTitle("Amplitude, ADC");
   //((TH2F*) o["hAmpltd"])->GetYaxis()->SetTitle("Events");
   // Amplitude with a cut applied on TDC and then loose cut on amplitude itself
   //o["hAmpltd"] = new TH1F("hAmpltd", "hAmpltd", 35, 165, 200);

   // The axis range will be/can be adjusted later based on the peak position
   o["hAmpltd"] = new TH1F("hAmpltd", "hAmpltd", 255, 0, 255);
   ((TH1*) o["hAmpltd"])->SetTitle("; Amplitude, ADC; Events; ");

   o["hIntgrl"] = new TH1F("hIntgrl", "hIntgrl", 255, 0, 255);
   ((TH1*) o["hIntgrl"])->SetTitle("; Integral, ADC; Events; ");

   //o["hIntgrl"] = new TH1F("hIntgrl", "hIntgrl", 255, 0, 255);
   //((TH2F*) o["hIntgrl"])->SetTitle("; Integral, ADC; Events; ");

   o["hTdc"]    = new TH1F("hTdc", "hTdc",  80, 0, 80);
   ((TH1*) o["hTdc"])->SetTitle("; TDC; Events; ");

   o["hTvsA"] = new TH2F("hTvsA",   "hTvsA", 255, 0, 255, 80, 0, 80);
   ((TH1*) o["hTvsA"])->SetOption("colz LOGZ");
   ((TH1*) o["hTvsA"])->SetTitle("; Amplitude, ADC; TDC;");

   o["hTvsI"] = new TH2F("hTvsI",   "hTvsI", 255, 0, 255, 80, 0, 80);
   ((TH2F*) o["hTvsI"])->SetOption("colz LOGZ");
   ((TH2F*) o["hTvsI"])->GetXaxis()->SetTitle("Integral, ADC");
   ((TH2F*) o["hTvsI"])->GetYaxis()->SetTitle("TDC");

   o["hIvsA"]        = new TH2F("hIvsA",   "hIvsA", 255, 0, 255, 255, 0, 255);
   ((TH2F*) o["hIvsA"])->SetOption("colz LOGZ");
   ((TH2F*) o["hIvsA"])->GetXaxis()->SetTitle("Amplitude, ADC");
   ((TH2F*) o["hIvsA"])->GetYaxis()->SetTitle("Integral, ADC");

   // Americium

   o["hAmGain"]   = new TH1F("hAmGain", "hAmGain", 72, 1, 73);
   ((TH1*)  o["hAmGain"])->SetOption("E1 GRIDX GRIDY");
   ((TH2F*) o["hAmGain"])->GetXaxis()->SetTitle("Channel");
   ((TH2F*) o["hAmGain"])->GetYaxis()->SetTitle("Americium Amplitude Calib, ADC/keV");

   o["hAmGainDisp"] = new TH1F("hAmGainDisp", "hAmGainDisp", 100, 0, 1);
   ((TH2F*) o["hAmGainDisp"])->SetBit(TH1::kCanRebin);
   ((TH2F*) o["hAmGainDisp"])->GetXaxis()->SetTitle("Americium Amplitude Calib, ADC/keV");

   o["hAmIntGain"]   = new TH1F("hAmIntGain", "hAmIntGain", 72, 1, 73);
   ((TH1*)  o["hAmIntGain"])->SetOption("E1 GRIDX GRIDY");
   ((TH2F*) o["hAmIntGain"])->GetXaxis()->SetTitle("Channel");
   ((TH2F*) o["hAmIntGain"])->GetYaxis()->SetTitle("Americium Integral Calib, ADC/keV");

   o["hAmIntGainDisp"] = new TH1F("hAmIntGainDisp", "hAmIntGainDisp", 100, 0, 1);
   ((TH2F*) o["hAmIntGainDisp"])->SetBit(TH1::kCanRebin);
   ((TH2F*) o["hAmIntGainDisp"])->GetXaxis()->SetTitle("Americium Integral Calib, ADC/keV");

   o["hAmGainWidth"]   = new TH1F("hAmGainWidth", "hAmGainWidth", 72, 1, 73);
   ((TH1*) o["hAmGainWidth"])->SetOption("E1 GRIDX GRIDY");
   ((TH1*) o["hAmGainWidth"])->GetXaxis()->SetTitle("Channel");
   ((TH1*) o["hAmGainWidth"])->GetYaxis()->SetTitle("Americium Alpha Peak Width, % (A)");

   o["hAmIntGainWidth"]   = new TH1F("hAmIntGainWidth", "hAmIntGainWidth", 72, 1, 73);
   ((TH1*) o["hAmIntGainWidth"])->SetOption("E1 GRIDX GRIDY");
   ((TH1*) o["hAmIntGainWidth"])->GetXaxis()->SetTitle("Channel");
   ((TH1*) o["hAmIntGainWidth"])->GetYaxis()->SetTitle("Americium Alpha Peak Width, % (I)");

   // Gadolinium

   o["hGdGain"]   = new TH1F("hGdGain", "hGdGain", 72, 1, 73);
   ((TH1*)  o["hGdGain"])->SetOption("E1 GRIDX GRIDY");
   ((TH2F*) o["hGdGain"])->GetXaxis()->SetTitle("Channel");
   ((TH2F*) o["hGdGain"])->GetYaxis()->SetTitle("Gadolinium Amplitude Calib, ADC/keV");

   o["hGdGainDisp"] = new TH1F("hGdGainDisp", "hGdGainDisp", 100, 0, 1);
   ((TH2F*) o["hGdGainDisp"])->SetBit(TH1::kCanRebin);
   ((TH2F*) o["hGdGainDisp"])->GetXaxis()->SetTitle("Gadolinium Amplitude Calib, ADC/keV");

   o["hGdIntGain"]   = new TH1F("hGdIntGain", "hGdIntGain", 72, 1, 73);
   ((TH1*)  o["hGdIntGain"])->SetOption("E1 GRIDX GRIDY");
   ((TH2F*) o["hGdIntGain"])->GetXaxis()->SetTitle("Channel");
   ((TH2F*) o["hGdIntGain"])->GetYaxis()->SetTitle("Gadolinium Integral Calib, ADC/keV");

   o["hGdIntGainDisp"] = new TH1F("hGdIntGainDisp", "hGdIntGainDisp", 100, 0, 1);
   ((TH2F*) o["hGdIntGainDisp"])->SetBit(TH1::kCanRebin);
   ((TH2F*) o["hGdIntGainDisp"])->GetXaxis()->SetTitle("Gadolinium Integral Calib, ADC/keV");

   o["hGdGainWidth"]   = new TH1F("hGdGainWidth", "hGdGainWidth", 72, 1, 73);
   ((TH1*) o["hGdGainWidth"])->SetOption("E1 GRIDX GRIDY");
   ((TH1*) o["hGdGainWidth"])->GetXaxis()->SetTitle("Channel");
   ((TH1*) o["hGdGainWidth"])->GetYaxis()->SetTitle("Gadolinium Alpha Peak Width, % (A)");

   o["hGdIntGainWidth"]   = new TH1F("hGdIntGainWidth", "hGdIntGainWidth", 72, 1, 73);
   ((TH1*) o["hGdIntGainWidth"])->SetOption("E1 GRIDX GRIDY");
   ((TH1*) o["hGdIntGainWidth"])->GetXaxis()->SetTitle("Channel");
   ((TH1*) o["hGdIntGainWidth"])->GetYaxis()->SetTitle("Gadolinium Alpha Peak Width, % (I)");


   o["hAmGdGain"] = new TH1F("hAmGdGain", "hAmGdGain", 72, 1, 73);
   ((TH1*)  o["hAmGdGain"])->SetOption("E1 GRIDX GRIDY");
   ((TH2F*) o["hAmGdGain"])->GetXaxis()->SetTitle("Channel");
   ((TH2F*) o["hAmGdGain"])->GetYaxis()->SetTitle("Americium+Gadolinium Amplitude Calib, ADC/keV");

   o["hDeadLayerEnergy"] = new TH1F("hDeadLayerEnergy", "hDeadLayerEnergy", 72, 1, 73);
   ((TH1F*) o["hDeadLayerEnergy"])->GetXaxis()->SetTitle("Channel");
   ((TH1F*) o["hDeadLayerEnergy"])->GetYaxis()->SetTitle("Dead layer energy, keV");
   ((TH1F*) o["hDeadLayerEnergy"])->SetOption("E1 GRIDX GRIDY");


   o["Detector1_Events"]   = new TH1F("Detector1_Events", "Detector1_Events", 12, 1, 12);
   ((TH1*) o["Detector1_Events"])->SetOption("E1 NOIMG");
   ((TH1*) o["Detector1_Events"])->GetXaxis()->SetTitle("Channel");
   ((TH1*) o["Detector1_Events"])->GetYaxis()->SetTitle("# of Events");

   o["Detector2_Events"]   = new TH1F("Detector2_Events", "Detector2_Events", 12, 13, 24);
   ((TH1*) o["Detector2_Events"])->SetOption("E1 NOIMG");
   ((TH1*) o["Detector2_Events"])->GetXaxis()->SetTitle("Channel");
   ((TH1*) o["Detector2_Events"])->GetYaxis()->SetTitle("# of Events");

   o["Detector3_Events"]   = new TH1F("Detector3_Events", "Detector3_Events", 12, 25, 36);
   ((TH1*) o["Detector3_Events"])->SetOption("E1 NOIMG");
   ((TH1*) o["Detector3_Events"])->GetXaxis()->SetTitle("Channel");
   ((TH1*) o["Detector3_Events"])->GetYaxis()->SetTitle("# of Events");

   o["Detector4_Events"]   = new TH1F("Detector4_Events", "Detector4_Events", 12, 37, 48);
   ((TH1*) o["Detector4_Events"])->SetOption("E1 NOIMG");
   ((TH1*) o["Detector4_Events"])->GetXaxis()->SetTitle("Channel");
   ((TH1*) o["Detector4_Events"])->GetYaxis()->SetTitle("# of Events");

   o["Detector5_Events"]   = new TH1F("Detector5_Events", "Detector5_Events", 12, 49, 60);
   ((TH1*) o["Detector5_Events"])->SetOption("E1 NOIMG");
   ((TH1*) o["Detector5_Events"])->GetXaxis()->SetTitle("Channel");
   ((TH1*) o["Detector5_Events"])->GetYaxis()->SetTitle("# of Events");

   o["Detector6_Events"]   = new TH1F("Detector6_Events", "Detector6_Events", 12, 61, 72);
   ((TH1*) o["Detector6_Events"])->SetOption("E1 NOIMG");
   ((TH1*) o["Detector6_Events"])->GetXaxis()->SetTitle("Channel");
   ((TH1*) o["Detector6_Events"])->GetYaxis()->SetTitle("# of Events");

   o["Detector1_Events_Dsp"]   = new TH1F("Detector1_Events_Dsp", "Detector1_Events_Dsp", 20000, 0, 20000);
   ((TH1*) o["Detector1_Events_Dsp"])->SetOption("E1 NOIMG");
   ((TH1*) o["Detector1_Events_Dsp"])->GetXaxis()->SetTitle("Events");
   ((TH1*) o["Detector1_Events_Dsp"])->GetYaxis()->SetTitle("#");

   o["Detector2_Events_Dsp"]   = new TH1F("Detector2_Events_Dsp", "Detector2_Events_Dsp", 20000, 0, 20000);
   ((TH1*) o["Detector2_Events_Dsp"])->SetOption("E1 NOIMG");
   ((TH1*) o["Detector2_Events_Dsp"])->GetXaxis()->SetTitle("Events");
   ((TH1*) o["Detector2_Events_Dsp"])->GetYaxis()->SetTitle("#");

   o["Detector3_Events_Dsp"]   = new TH1F("Detector3_Events_Dsp", "Detector3_Events_Dsp", 20000, 0, 20000);
   ((TH1*) o["Detector3_Events_Dsp"])->SetOption("E1 NOIMG");
   ((TH1*) o["Detector3_Events_Dsp"])->GetXaxis()->SetTitle("Events");
   ((TH1*) o["Detector3_Events_Dsp"])->GetYaxis()->SetTitle("#");

   o["Detector4_Events_Dsp"]   = new TH1F("Detector4_Events_Dsp", "Detector4_Events_Dsp", 20000, 0, 20000);
   ((TH1*) o["Detector4_Events_Dsp"])->SetOption("E1 NOIMG");
   ((TH1*) o["Detector4_Events_Dsp"])->GetXaxis()->SetTitle("Events");
   ((TH1*) o["Detector4_Events_Dsp"])->GetYaxis()->SetTitle("#");

   o["Detector5_Events_Dsp"]   = new TH1F("Detector5_Events_Dsp", "Detector5_Events_Dsp", 20000, 0, 20000);
   ((TH1*) o["Detector5_Events_Dsp"])->SetOption("E1 NOIMG");
   ((TH1*) o["Detector5_Events_Dsp"])->GetXaxis()->SetTitle("Events");
   ((TH1*) o["Detector5_Events_Dsp"])->GetYaxis()->SetTitle("#");

   o["Detector6_Events_Dsp"]   = new TH1F("Detector6_Events_Dsp", "Detector6_Events_Dsp", 20000, 0, 20000);
   ((TH1*) o["Detector6_Events_Dsp"])->SetOption("E1 NOIMG");
   ((TH1*) o["Detector6_Events_Dsp"])->GetXaxis()->SetTitle("Events");
   ((TH1*) o["Detector6_Events_Dsp"])->GetYaxis()->SetTitle("#");

   DrawObjContainer        *oc;
   DrawObjContainerMapIter  isubdir;

   for (UShort_t iCh = 1; iCh <= TOT_WFD_CH; iCh++) {

      string sChId("  ");
      sprintf(&sChId[0], "%02d", iCh);

      string dName = "channel" + sChId;

      isubdir = d.find(dName);

      if ( isubdir == d.end()) { // if dir not found
         oc = new DrawObjContainer();
         oc->fDir = new TDirectoryFile(dName.c_str(), dName.c_str(), "", fDir);
      }
      else {
         oc = isubdir->second;
      }

      oc->fDir->cd();

      //sprintf(hName,"mass_feedback_st%d", iCh+1);
      //sprintf(hTitle,"%.3f : Invariant Mass for Strip-%d ", gMeasInfo->RUNID, iCh+1);
      //sprintf(hTitle,"Invariant Mass for Strip-%d ", iCh+1);
      //feedback.o[hName] = new TH1F(hName, hTitle, 100, 0, 20);
      //( (TH1F*) feedback.o[hName]) -> GetXaxis() -> SetTitle("Mass [GeV/c^2]");
      //( (TH1F*) feedback.o[hName]) -> SetLineColor(2);

      sprintf(hName, "hAmpltd_ch%02d", iCh);
      oc->o[hName] = new TH1F(hName, hName, 255, 0, 255);
      ((TH1*) oc->o[hName])->SetTitle("; Amplitude, ADC; Events; ");

      sprintf(hName, "hIntgrl_ch%02d", iCh);
      oc->o[hName] = new TH1F(hName, hName, 255, 0, 255);
      ((TH1*) oc->o[hName])->SetTitle("; Integral, ADC; Events; ");

      sprintf(hName, "hTdc_ch%02d", iCh);
      oc->o[hName] = new TH1F(hName, hName, 80, 0, 80);
      ((TH1*) oc->o[hName])->SetTitle("; TDC; Events; ");

      sprintf(hName, "hTvsA_ch%02d", iCh);
      oc->o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 0, 80);
      ((TH1*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH1*) oc->o[hName])->SetTitle("; Amplitude, ADC; TDC;");

      sprintf(hName, "hTvsA_zoom_ch%02d", iCh);
      oc->o[hName] = new TH2F(hName, hName, 70, 130, 200, 30, 20, 50);
      ((TH1*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH1*) oc->o[hName])->SetTitle("; Amplitude, ADC; TDC; ");

      sprintf(hName, "hTvsI_ch%02d", iCh);
      oc->o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 0, 80);
      ((TH1*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH1*) oc->o[hName])->SetTitle("; Integral, ADC; TDC; ");

      sprintf(hName, "hIvsA_ch%02d", iCh);
      oc->o[hName] = new TH2F(hName, hName, 255, 0, 255, 255, 0, 255);
      ((TH1*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH1*) oc->o[hName])->SetTitle("; Amplitude, ADC; Integral, ADC; ");

      sprintf(hName, "hAmGd_ch%02d", iCh);
      oc->o[hName] = new TH2F(hName, hName, 0, 0, 0, 0, 0, 0);

      //sprintf(hName,"t_vs_e_ch%d", iCh);
      //kinema.oc->o[hName] = new TH2F();

      // If this is a new directory then we need to add it to the list
      if ( isubdir == d.end()) {
         d[dName] = oc;
      }
   }
}


/** */
void CnipolAlphaHists::FillPassOne(ChannelEvent *ch)
{
   UChar_t      chId = ch->GetChannelId();
   ChannelData &data = ch->fChannel;

   string sSi("  ");
   sprintf(&sSi[0], "%02d", chId);

   ((TH1F*) o["hAmpltd"])->Fill(data.fAmpltd);
   ((TH1F*) o["hIntgrl"])->Fill(data.fIntgrl);
   ((TH1F*) o["hTdc"])   ->Fill(data.fTdc);
   ((TH2F*) o["hTvsA"])  ->Fill(data.fAmpltd, data.fTdc);
   ((TH2F*) o["hTvsI"])  ->Fill(data.fIntgrl, data.fTdc);
   ((TH2F*) o["hIvsA"])  ->Fill(data.fAmpltd, data.fIntgrl);

   DrawObjContainer *sd = d["channel" + sSi];

   ((TH1F*) sd->o["hAmpltd_ch"    + sSi])->Fill(data.fAmpltd);
   ((TH1F*) sd->o["hIntgrl_ch"    + sSi])->Fill(data.fIntgrl);
   ((TH1F*) sd->o["hTdc_ch"       + sSi])->Fill(data.fTdc);
   ((TH2F*) sd->o["hTvsA_ch"      + sSi])->Fill(data.fAmpltd, data.fTdc);
   ((TH2F*) sd->o["hTvsA_zoom_ch" + sSi])->Fill(data.fAmpltd, data.fTdc);
   ((TH2F*) sd->o["hTvsI_ch"      + sSi])->Fill(data.fIntgrl, data.fTdc);
   ((TH2F*) sd->o["hIvsA_ch"      + sSi])->Fill(data.fAmpltd, data.fIntgrl);
}


/** */
void CnipolAlphaHists::PostFill()
{
}


/** */
void CnipolAlphaHists::PostFillPassOne(DrawObjContainer *oc)
{
   Info("PostFillPassOne", "Called");

   // Adjust axis ranges
   Int_t    maxBinA = ((TH1F*) o["hAmpltd"])->GetMaximumBin();
   Double_t   xminA = ((TH1F*) o["hAmpltd"])->GetXaxis()->GetXmin();
   Double_t   xmaxA = ((TH1F*) o["hAmpltd"])->GetXaxis()->GetXmax();

   xminA = maxBinA - 50 < xminA ? xminA : maxBinA - 50;
   xmaxA = maxBinA + 50 > xmaxA ? xmaxA : maxBinA + 50;

   ((TH1F*) o["hAmpltd"])->SetAxisRange(xminA, xmaxA);

   Int_t  maxBinI  = ((TH1F*) o["hIntgrl"])->GetMaximumBin();
   Double_t xminI  = ((TH1F*) o["hIntgrl"])->GetXaxis()->GetXmin();
   Double_t xmaxI  = ((TH1F*) o["hIntgrl"])->GetXaxis()->GetXmax();
   //Int_t tmaxBinI  = 0;
   Int_t tmaxBinA  = 0;
   Double_t valueA = 0;
   Int_t maxBinAa  = 0;

   //Int_t maxBin = 0;

   xminI = maxBinI - 50 < xminI ? xminI : maxBinI - 50;
   xmaxI = maxBinI + 50 > xmaxI ? xmaxI : maxBinI + 50;

   ((TH1F*) o["hIntgrl"])->SetAxisRange(xminI, xmaxI);

   //Info("PostFillPassOne", "xminA, xmaxA, xminI, xmaxI: %f, %f, %f, %f", xminA, xmaxA, xminI, xmaxI);

   string sChId("  ");

   for (int i = 1; i <= NSTRIP; i++)
   {
      sprintf(&sChId[0], "%02d", i);

      maxBinA = ((TH1F*) d["channel" + sChId]->o["hAmpltd_ch" + sChId])->GetMaximumBin();
      maxBinI = ((TH1F*) d["channel" + sChId]->o["hIntgrl_ch" + sChId])->GetMaximumBin();

      valueA = ((TH1F*) d["channel" + sChId]->o["hAmpltd_ch" + sChId])->GetBinContent(maxBinA);

      for (int j = 0; j <= 255; j++) {
         tmaxBinA = ((TH1F*) d["channel" + sChId]->o["hAmpltd_ch" + sChId])->GetBinContent(j);
         if (tmaxBinA >= (valueA * .95)) maxBinAa = j;
      }

      maxBinA = maxBinAa;

      // if (i == 9 || i == 10) {
      //     for (int j = 0; j <= 255; j++) 
      //      tmaxBinI = ((TH1F*) d["channel" + sChId]->o["hIntgrl_ch" + sChId])->GetBinContent(j);
      //    if (tmaxBinI >= maxBinI) maxBinI = tmaxBinI;
      // }

      //xmin   = ((TH1F*) d["channel"+sChId].o["hAmpltd_ch"+sChId])->GetXaxis()->GetXmin();
      //xmax   = ((TH1F*) d["channel"+sChId].o["hAmpltd_ch"+sChId])->GetXaxis()->GetXmax();
      //xmin   = maxBin - 50 < xmin ? xmin : maxBin - 50;
      //xmax   = maxBin + 50 > xmax  ? xmax : maxBin + 50;

      //xminA = maxBinA - 50;
      //xmaxA = maxBinA + 50;
      //xminI = maxBinI - 50;
      //xmaxI = maxBinI + 50;

      //((TH1F*) d["channel" + sChId]->o["hAmpltd_ch" + sChId])->SetAxisRange(xminA, xmaxA);
      //((TH1F*) d["channel" + sChId]->o["hIntgrl_ch" + sChId])->SetAxisRange(xminI, xmaxI);
   }

   // Update axis range
   utils::UpdateLimits((TH1*) o["hAmGain"]);
   utils::UpdateLimits((TH1*) o["hAmGainWidth"]);
   utils::UpdateLimits((TH1*) o["hAmIntGain"]);
   utils::UpdateLimits((TH1*) o["hAmIntGainWidth"]);
   utils::UpdateLimits((TH1*) o["hGdGain"]);
   utils::UpdateLimits((TH1*) o["hGdGainWidth"]);
   utils::UpdateLimits((TH1*) o["hGdIntGain"]);
   utils::UpdateLimits((TH1*) o["hGdIntGainWidth"]);
}
