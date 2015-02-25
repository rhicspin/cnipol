#include "CnipolAlphaHists.h"
#include "ChannelEventId.h"
#include <fstream>

#include "utils/utils.h"

ClassImp(CnipolAlphaHists)

using namespace std;


const double CnipolAlphaHists::ALPHA_TDC_CUT = 70.0; //! cut out events that fall out of the WFD window

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
   o["hAmpltd"] = new TH1F("hAmpltd", "hAmpltd", 220, 0, 220);
   ((TH1*) o["hAmpltd"])->SetTitle("; Amplitude, ADC; Events; ");

   o["hIntgrl"] = new TH1F("hIntgrl", "hIntgrl", 255, 0, 255);
   ((TH1*) o["hIntgrl"])->SetTitle("; Integral, ADC; Events; ");

   //o["hIntgrl"] = new TH1F("hIntgrl", "hIntgrl", 255, 0, 255);
   //((TH2F*) o["hIntgrl"])->SetTitle("; Integral, ADC; Events; ");

   o["hTdc"]    = new TH1F("hTdc", "hTdc",  80, 0, 80);
   ((TH1*) o["hTdc"])->SetTitle("; TDC; Events; ");

   o["hTvsA"] = new TH2F("hTvsA",   "hTvsA", 220, 0, 220, 80, 0, 80);
   ((TH1*) o["hTvsA"])->SetOption("colz LOGZ");
   ((TH1*) o["hTvsA"])->SetTitle("; Amplitude, ADC; TDC;");

   o["hTvsI"] = new TH2F("hTvsI",   "hTvsI", 255, 0, 255, 80, 0, 80);
   ((TH2F*) o["hTvsI"])->SetOption("colz LOGZ");
   ((TH2F*) o["hTvsI"])->GetXaxis()->SetTitle("Integral, ADC");
   ((TH2F*) o["hTvsI"])->GetYaxis()->SetTitle("TDC");

   o["hIvsA"]        = new TH2F("hIvsA",   "hIvsA", 220, 0, 220, 255, 0, 255);
   ((TH2F*) o["hIvsA"])->SetOption("colz LOGZ");
   ((TH2F*) o["hIvsA"])->GetXaxis()->SetTitle("Amplitude, ADC");
   ((TH2F*) o["hIvsA"])->GetYaxis()->SetTitle("Integral, ADC");

   // Americium

   o["hAmGain"]   = new TH1F("hAmGain", "hAmGain", 72, 1, 73);
   ((TH1*)  o["hAmGain"])->SetOption("E1 GRIDX GRIDY");
   ((TH2F*) o["hAmGain"])->GetXaxis()->SetTitle("Channel");
   ((TH2F*) o["hAmGain"])->GetYaxis()->SetTitle("Americium gain, ADC/keV");

   o["hAmGainDisp"] = new TH1F("hAmGainDisp", "hAmGainDisp", 100, 0, 1);
   ((TH2F*) o["hAmGainDisp"])->SetBit(TH1::kCanRebin);
   ((TH2F*) o["hAmGainDisp"])->GetXaxis()->SetTitle("Americium gain, ADC/keV");

   o["hAmIntGain"]   = new TH1F("hAmIntGain", "hAmIntGain", 72, 1, 73);
   ((TH1*)  o["hAmIntGain"])->SetOption("E1 GRIDX GRIDY");
   ((TH2F*) o["hAmIntGain"])->GetXaxis()->SetTitle("Channel");
   ((TH2F*) o["hAmIntGain"])->GetYaxis()->SetTitle("Americium gain (from integral), ADC/keV");

   o["hAmIntGainDisp"] = new TH1F("hAmIntGainDisp", "hAmIntGainDisp", 100, 0, 1);
   ((TH2F*) o["hAmIntGainDisp"])->SetBit(TH1::kCanRebin);
   ((TH2F*) o["hAmIntGainDisp"])->GetXaxis()->SetTitle("Americium gain (from integral), ADC/keV");

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
      oc->o[hName] = new TH1F(hName, hName, 220, 0, 220);
      ((TH1*) oc->o[hName])->SetTitle("; Amplitude, ADC; Events; ");

      sprintf(hName, "hIntgrl_ch%02d", iCh);
      oc->o[hName] = new TH1F(hName, hName, 255, 0, 255);
      ((TH1*) oc->o[hName])->SetTitle("; Integral, ADC; Events; ");

      sprintf(hName, "hTdc_ch%02d", iCh);
      oc->o[hName] = new TH1F(hName, hName, 80, 0, 80);
      ((TH1*) oc->o[hName])->SetTitle("; TDC; Events; ");

      sprintf(hName, "hTvsA_ch%02d", iCh);
      oc->o[hName] = new TH2F(hName, hName, 220, 0, 220, 80, 0, 80);
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
      oc->o[hName] = new TH2F(hName, hName, 220, 0, 220, 255, 0, 255);
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

   ((TH1F*) o["hTdc"])   ->Fill(data.fTdc);
   ((TH2F*) o["hTvsA"])  ->Fill(data.fAmpltd, data.fTdc);
   ((TH2F*) o["hTvsI"])  ->Fill(data.fIntgrl, data.fTdc);

   DrawObjContainer *sd = d["channel" + sSi];

   ((TH1F*) sd->o["hTdc_ch"       + sSi])->Fill(data.fTdc);
   ((TH2F*) sd->o["hTvsA_ch"      + sSi])->Fill(data.fAmpltd, data.fTdc);
   ((TH2F*) sd->o["hTvsA_zoom_ch" + sSi])->Fill(data.fAmpltd, data.fTdc);
   ((TH2F*) sd->o["hTvsI_ch"      + sSi])->Fill(data.fIntgrl, data.fTdc);
}


/** */
void CnipolAlphaHists::PostFillPassOne(DrawObjContainer *oc)
{
   Info("PostFillPassOne", "Called");

   // determine TDC distribution baseline
   TH1F *hTdc = (TH1F*)o["hTdc"];
   TFitResultPtr fitres = hTdc->Fit("pol0", "S"); // S: return fitres
   if (fitres.Get()) {
      double baseline = fitres->Value(0);
      set<int> bins;

      Int_t xfirst = hTdc->GetXaxis()->GetFirst();
      Int_t xlast  = hTdc->GetXaxis()->GetLast();
      for (Int_t bin = xfirst; bin <= xlast; bin++) {
         double value = hTdc->GetBinContent(bin);
         if (value > baseline*2)
         {
            Info("PostFillPassOne", "bad TDC bin %i", bin);
            bad_tdc_bins.insert(bin - 1);
            bad_tdc_bins.insert(bin);
            bad_tdc_bins.insert(bin + 1);
         }
      }
   }
}


/** */
void CnipolAlphaHists::Fill(ChannelEvent *ch)
{
   UChar_t      chId = ch->GetChannelId();
   ChannelData &data = ch->fChannel;

   string sSi("  ");
   sprintf(&sSi[0], "%02d", chId);
   DrawObjContainer *sd = d["channel" + sSi];

   if ((data.fTdc <= ALPHA_TDC_CUT) && !bad_tdc_bins.count(data.fTdc))
   {
      ((TH1F*) o["hAmpltd"])->Fill(data.fAmpltd);
      ((TH1F*) o["hIntgrl"])->Fill(data.fIntgrl);
      ((TH2F*) o["hIvsA"])->Fill(data.fAmpltd, data.fIntgrl);
      ((TH1F*) sd->o["hAmpltd_ch" + sSi])->Fill(data.fAmpltd);
      ((TH1F*) sd->o["hIntgrl_ch" + sSi])->Fill(data.fIntgrl);
      ((TH2F*) sd->o["hIvsA_ch"   + sSi])->Fill(data.fAmpltd, data.fIntgrl);
   }
}


/** */
void CnipolAlphaHists::PostFill()
{
   Info("PostFill", "Called");

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

   xminI = maxBinI - 50 < xminI ? xminI : maxBinI - 50;
   xmaxI = maxBinI + 50 > xmaxI ? xmaxI : maxBinI + 50;

   ((TH1F*) o["hIntgrl"])->SetAxisRange(xminI, xmaxI);

   // Update axis range
   utils::UpdateLimits((TH1*) o["hAmGain"]);
   utils::UpdateLimits((TH1*) o["hAmGainWidth"]);
   utils::UpdateLimits((TH1*) o["hAmIntGain"]);
   utils::UpdateLimits((TH1*) o["hAmIntGainWidth"]);
   utils::UpdateLimits((TH1*) o["hGdGain"]);
   utils::UpdateLimits((TH1*) o["hGdGainWidth"]);
   utils::UpdateLimits((TH1*) o["hGdIntGain"]);
   utils::UpdateLimits((TH1*) o["hGdIntGainWidth"]);

   // Visualize the TDC cuts
   TVirtualPad *backup = gPad;
   TLine *l;
   TH2F *hTvsA = (TH2F*)o["hTvsA"];
   o["hTvsA"] = new TCanvas("hTvsA", "");
   hTvsA->Draw();
   l = new TLine(0, ALPHA_TDC_CUT, 255, ALPHA_TDC_CUT);
   l->SetLineWidth(3);
   l->Draw();
   TH2F *hTvsI = (TH2F*)o["hTvsI"];
   o["hTvsI"] = new TCanvas("hTvsI", "");
   hTvsI->Draw();
   l = new TLine(0, ALPHA_TDC_CUT, 255, ALPHA_TDC_CUT);
   l->SetLineWidth(3);
   l->Draw();
   for(set<int>::const_iterator it = bad_tdc_bins.begin(); it != bad_tdc_bins.end(); it++)
   {
      const Style_t FILL_STYLE = 3013;
      const Color_t FILL_COLOR = kBlack;
      int bin = *it;
      double up, low;
      TBox *b;

      ((TCanvas*) o["hTvsA"])->cd();
      low = hTvsA->GetXaxis()->GetBinLowEdge(bin);
      up = hTvsA->GetXaxis()->GetBinUpEdge(bin);
      b = new TBox(hTvsA->GetXaxis()->GetXmin(), low, hTvsA->GetXaxis()->GetXmax(), up);
      b->SetFillStyle(FILL_STYLE);
      b->SetFillColor(FILL_COLOR);
      b->Draw();

      ((TCanvas*) o["hTvsI"])->cd();
      low = hTvsI->GetXaxis()->GetBinLowEdge(bin);
      up = hTvsI->GetXaxis()->GetBinUpEdge(bin);
      b = new TBox(hTvsI->GetXaxis()->GetXmin(), low, hTvsI->GetXaxis()->GetXmax(), up);
      b->SetFillStyle(FILL_STYLE);
      b->SetFillColor(FILL_COLOR);
      b->Draw();
   }
   gPad = backup;
}
