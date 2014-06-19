#include "CnipolPulserHists.h"

#include "utils/utils.h"


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
void CnipolPulserHists::BookHists()
{
   char hName[256];

   fDir->cd();

   sprintf(hName, "hAdcAmpltd"); // former adc_raw
   o[hName] = new TH1F(hName, hName, 255, 0, 255);
   ((TH1*) o[hName])->SetOption("hist NOIMG");
   ((TH1*) o[hName])->SetTitle("Raw ADC (All strips);Amplitude, ADC;Events;");
   ((TH1*) o[hName])->SetFillColor(kGray);

   sprintf(hName, "hTdc"); // former tdc_raw
   o[hName] = new TH1F(hName, hName, 80, 10, 90);
   ((TH1*) o[hName])->SetOption("hist NOIMG");
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

   sprintf(hName, "hIvsA");
   o[hName] = new TH2F(hName, hName, 255, 0, 255, 255, 0, 255);
   ((TH1*) o[hName])->SetOption("colz LOGZ");
   ((TH1*) o[hName])->SetTitle(";Integral, ADC;TDC;");

   // Time vs Energy from amplitude
   sprintf(hName, "hTimeVsEnergyA");
   o[hName] = new TH2F(hName, hName, 80, 100, 1700, 80, 20, 100);
   ((TH1*) o[hName])->SetOption("colz LOGZ NOIMG");
   ((TH1*) o[hName])->SetTitle(";Deposited Energy, keV;Time, ns;");

   // Time of flight vs kin energy from amplitude
   sprintf(hName, "hTofVsKinEnergyA");
   o[hName] = new TH2F(hName, hName, 80, 100, 1700, 60, 10, 110);
   ((TH1*) o[hName])->SetOption("colz LOGZ NOIMG");
   ((TH1*) o[hName])->SetTitle(";Kinematic Energy, keV;ToF, ns;");

   sprintf(hName, "hBunchCounts"); //former bunch_dist_raw
   o[hName] = new TH1F(hName, hName, N_BUNCHES, 0.5, N_BUNCHES+0.5);
   ((TH1*) o[hName])->SetTitle(";Bunch Id;Events;");
   ((TH1*) o[hName])->SetFillColor(kGray);

   sprintf(hName, "hStripCounts"); // former strip_dist_raw
   o[hName] = new TH1F(hName, hName, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   //((TH1*) o[hName])->SetOption("hist");
   ((TH1*) o[hName])->SetTitle(";Channel Id;Events;");
   ((TH1*) o[hName])->SetFillColor(kGray);

   DrawObjContainer        *oc;
   DrawObjContainerMapIter  isubdir;

   ChannelSetIter iCh = gMeasInfo->fSiliconChannels.begin();

   for (; iCh!=gMeasInfo->fSiliconChannels.end(); ++iCh) {

      string sChId(MAX_CHANNEL_DIGITS, ' ');
      sprintf(&sChId[0], "%02d", *iCh);

      string dName = "channel" + sChId;

      isubdir = d.find(dName);

      if ( isubdir == d.end()) { // if dir not found
         oc = new DrawObjContainer();
         oc->fDir = new TDirectoryFile(dName.c_str(), dName.c_str(), "", fDir);
      } else {
         oc = isubdir->second;
      }

      oc->fDir->cd();

      sprintf(hName, "hAdcAmpltd_ch%02d", *iCh);
      oc->o[hName] = new TH1F(hName, hName, 255, 0, 255);
      ((TH1*) oc->o[hName])->SetOption("hist NOIMG");
      ((TH1*) oc->o[hName])->SetTitle(";Amplitude, ADC;Events;");
      ((TH1*) oc->o[hName])->SetFillColor(kGray);

      sprintf(hName, "hTdc_ch%02d", *iCh);
      oc->o[hName] = new TH1F(hName, hName, 80, 10, 90);
      ((TH1*) oc->o[hName])->SetOption("hist NOIMG");
      ((TH1*) oc->o[hName])->SetTitle(";TDC;Events;");
      ((TH1*) oc->o[hName])->SetFillColor(kGray);

      sprintf(hName, "hTvsA_ch%02d", *iCh);
      oc->o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
      ((TH1*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH1*) oc->o[hName])->SetTitle(";Amplitude, ADC;TDC;");

      sprintf(hName, "hTvsI_ch%02d", *iCh);
      oc->o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
      ((TH1*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH1*) oc->o[hName])->SetTitle(";Integral, ADC;TDC;");

      sprintf(hName, "hIvsA_ch%02d", *iCh);
      oc->o[hName] = new TH2F(hName, hName, 255, 0, 255, 255, 0, 255);
      ((TH1*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH1*) oc->o[hName])->SetTitle(";Amplitude, ADC;Integral, ADC;");

      // Time vs Energy from amplitude
      sprintf(hName, "hTimeVsEnergyA_ch%02d", *iCh);
      oc->o[hName] = new TH2F(hName, hName, 80, 100, 1700, 80, 20, 100);
      ((TH1*) oc->o[hName])->SetOption("colz LOGZ NOIMG");
      ((TH1*) oc->o[hName])->SetTitle(";Deposited Energy, keV;Time, ns;");

      // Time of flight vs kin energy from amplitude
      sprintf(hName, "hTofVsKinEnergyA_ch%02d", *iCh);
      oc->o[hName] = new TH2F(hName, hName, 80, 100, 1700, 60, 10, 110);
      ((TH1*) oc->o[hName])->SetOption("colz LOGZ NOIMG");
      ((TH1*) oc->o[hName])->SetTitle(";Kinematic Energy, keV;ToF, ns;");

      // If this is a new directory then we need to add it to the list
      if ( isubdir == d.end()) {
         d[dName] = oc;
      }
   }

   // Speed up
   fhBunchCounts = (TH1*) o.find("hBunchCounts")->second;
   fhStripCounts = (TH1*) o.find("hStripCounts")->second;

   iCh = gMeasInfo->fSiliconChannels.begin();

   for (; iCh!=gMeasInfo->fSiliconChannels.end(); ++iCh) {

      string sChId("  ");
      sprintf(&sChId[0], "%02d", *iCh);

      DrawObjContainer *oc_ch = d.find("channel" + sChId)->second;

      fhTvsA_ch[*iCh-1]          = (TH2*) oc_ch->o.find("hTvsA_ch"          + sChId)->second;
      fhTvsI_ch[*iCh-1]          = (TH2*) oc_ch->o.find("hTvsI_ch"          + sChId)->second;
      fhIvsA_ch[*iCh-1]          = (TH2*) oc_ch->o.find("hIvsA_ch"          + sChId)->second;
      fhTimeVsEnergyA_ch[*iCh-1] = (TH2*) oc_ch->o.find("hTimeVsEnergyA_ch" + sChId)->second;
   }

}


/** */
void CnipolPulserHists::FillPassOne(ChannelEvent *ch)
{
   UChar_t  chId     = ch->GetChannelId();
   UShort_t adcA_bin = ch->GetAmpltd() + 1;
   UShort_t adcI_bin = ch->GetIntgrl() + 1;
   UShort_t tdc_bin  = ch->GetTdc() - 10 + 1;

   string sChId("  ");
   sprintf(&sChId[0], "%02d", chId);

   //DrawObjContainer *sd = d["channel" + sChId];

   // Speed up the filling process by getting the global bin number
   TH2* hTmp_ch;
   Int_t gbin;

   //hTmp_ch = (TH2*) sd->o.find("hTvsA_ch" + sChId)->second;
   hTmp_ch = fhTvsA_ch[chId-1];
   gbin    = hTmp_ch->GetBin(adcA_bin, tdc_bin);
   hTmp_ch->AddBinContent(gbin);
   hTmp_ch->SetEntries(hTmp_ch->GetEntries()+1);

   //hTmp_ch = (TH2*) sd->o.find("hTvsI_ch" + sChId)->second;
   hTmp_ch = fhTvsI_ch[chId-1];
   gbin    = hTmp_ch->GetBin(adcI_bin, tdc_bin);
   hTmp_ch->AddBinContent(gbin);
   hTmp_ch->SetEntries(hTmp_ch->GetEntries()+1);

   //hTmp_ch = (TH2*) sd->o.find("hIvsA_ch" + sChId)->second;
   hTmp_ch = fhIvsA_ch[chId-1];
   gbin    = hTmp_ch->GetBin(adcA_bin, adcI_bin);
   hTmp_ch->AddBinContent(gbin);
   hTmp_ch->SetEntries(hTmp_ch->GetEntries()+1);

   //((TH1*) sd->o.find("hTimeVsEnergyA_ch" + sChId)->second) -> Fill(ch->GetEnergyA(), ch->GetTime());
   fhTimeVsEnergyA_ch[chId-1] -> Fill(ch->GetEnergyA(), ch->GetTime());

   TH1* h1Tmp_ch;

   //h1Tmp_ch = (TH1*) o.find("hBunchCounts")->second;
   h1Tmp_ch = fhBunchCounts;
   h1Tmp_ch->AddBinContent(ch->GetBunchId() + 1);
   h1Tmp_ch->SetEntries(h1Tmp_ch->GetEntries() + 1);

   //h1Tmp_ch = (TH1*) o.find("hStripCounts")->second;
   h1Tmp_ch = fhStripCounts;
   h1Tmp_ch->AddBinContent(chId);
   h1Tmp_ch->SetEntries(h1Tmp_ch->GetEntries() + 1);

}


/** */
void CnipolPulserHists::FillDerivedPassOne()
{
   TH1* hAdcAmpltd       = (TH1*) o["hAdcAmpltd"];
   TH1* hTdc             = (TH1*) o["hTdc"];
   TH1* hTvsA            = (TH1*) o["hTvsA"];
   TH1* hTvsI            = (TH1*) o["hTvsI"];
   TH1* hIvsA            = (TH1*) o["hIvsA"];
   TH1* hTimeVsEnergyA   = (TH1*) o["hTimeVsEnergyA"];
   TH1* hTofVsKinEnergyA = (TH1*) o["hTofVsKinEnergyA"];
   
   for (UShort_t iCh=1; iCh<=N_SILICON_CHANNELS; iCh++)
   {
      string sChId("  ");
      sprintf(&sChId[0], "%02d", iCh);

      DrawObjContainer *oc = d.find("channel" + sChId)->second;

      TH2* hTVsA_ch = (TH2*) oc->o["hTvsA_ch" + sChId];
      hTvsA->Add(hTVsA_ch);

      TH2* hTVsI_ch = (TH2*) oc->o["hTvsI_ch" + sChId];
      hTvsI->Add(hTVsI_ch);

      TH2* hIVsA_ch = (TH2*) oc->o["hIvsA_ch" + sChId];
      hIvsA->Add(hIVsA_ch);

      TH2* hTimeVsEnergyA_ch = (TH2*) oc->o["hTimeVsEnergyA_ch" + sChId];
      hTimeVsEnergyA->Add(hTimeVsEnergyA_ch);

      TH2* hTofVsKinEnergyA_ch = (TH2*) oc->o["hTofVsKinEnergyA_ch" + sChId];
      hTofVsKinEnergyA->Add(hTofVsKinEnergyA_ch);

      // Fill one dimensional hists from two dimensional ones
      TH1D* hProjTmp;

      // AdcAmpltd
      TH1* hAdcAmpltd_ch = (TH1*) oc->o["hAdcAmpltd_ch" + sChId];
      hProjTmp = hTVsA_ch->ProjectionX();

      utils::CopyBinContentError(hProjTmp, hAdcAmpltd_ch);

      hAdcAmpltd->Add(hAdcAmpltd_ch);

      // Tdc
      TH1* hTdc_ch  = (TH1*) oc->o["hTdc_ch"+sChId];
      hProjTmp = hTVsA_ch->ProjectionY();

      utils::CopyBinContentError(hProjTmp, hTdc_ch);

      hTdc->Add(hTdc_ch);
   }
}


/** */
void CnipolPulserHists::Fill(ChannelEvent *ch)
{
   UChar_t  chId     = ch->GetChannelId();
   //UShort_t adcA_bin = ch->GetAmpltd() + 1;
   //UShort_t adcI_bin = ch->GetIntgrl() + 1;
   //UShort_t tdc_bin  = ch->GetTdc() - 10 + 1;

   string sChId("  ");
   sprintf(&sChId[0], "%02d", chId);

   DrawObjContainer *sd = d["channel" + sChId];

   Float_t kinEnergy = ch->GetKinEnergyAEDepend();
   Float_t tof       = ch->GetTimeOfFlight();

   ((TH1*) sd->o["hTofVsKinEnergyA_ch" + sChId]) -> Fill(kinEnergy, tof);

}
