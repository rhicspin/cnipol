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
void CnipolRawHists::BookHists()
{
   string shName;
   TH1*   hist;

   fDir->cd();

   shName = "hAdcAmpltd";
   hist = new TH1I(shName.c_str(), shName.c_str(), 255, 0, 255);
   hist->SetTitle("; Amplitude, ADC; Events;");
   hist->SetOption("hist");
   hist->SetFillColor(kGray);
   o[shName] = hist;

   shName = "hAdcIntgrl";
   hist = new TH1I(shName.c_str(), shName.c_str(), 255, 0, 255);
   hist->SetTitle("; Integral, ADC; Events;");
   hist->SetOption("hist");
   hist->SetFillColor(kGray);
   o[shName] = hist;

   shName = "hTdc";
   hist = new TH1I(shName.c_str(), shName.c_str(), 80, 10, 90);
   hist->SetOption("hist");
   hist->SetTitle("; TDC; Events;");
   hist->SetFillColor(kGray);
   o[shName] = hist;

   shName = "hTvsA";
   hist = new TH2I(shName.c_str(), shName.c_str(), 255, 0, 255, 80, 10, 90);
   hist->SetTitle("; Amplitude, ADC; TDC;");
   hist->SetOption("colz LOGZ");
   o[shName] = hist;

   shName = "hTvsI";
   hist = new TH2I(shName.c_str(), shName.c_str(), 255, 0, 255, 80, 10, 90);
   hist->SetTitle("; Integral, ADC; TDC;");
   hist->SetOption("colz LOGZ");
   o[shName] = hist;

   shName = "hIvsA";
   hist = new TH2I(shName.c_str(), shName.c_str(), 255, 0, 255, 255, 0, 255);
   hist->SetTitle("; Amplitude, ADC; Integral, ADC;");
   hist->SetOption("colz LOGZ");
   o[shName] = hist;

   shName = "hBunchCounts"; //former bunch_dist_raw
   hist = new TH1I(shName.c_str(), shName.c_str(), N_BUNCHES, 0.5, N_BUNCHES+0.5);
   hist->SetTitle("; Bunch Id; Events;");
   hist->SetOption("hist XY GRIDX");
   hist->SetFillColor(kGray);
   o[shName] = hist;

   shName = "hStripCounts"; // former strip_dist_raw
   hist = new TH1I(shName.c_str(), shName.c_str(), N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   hist->SetTitle("; Channel Id; Events;");
   hist->SetOption("hist XY GRIDX");
   hist->SetFillColor(kGray);
   o[shName] = hist;

   shName = "hRevolutionId";
   hist = new TH1I(shName.c_str(), shName.c_str(), 1000, 0, 1);
   hist->SetTitle("; Revolution Id; Events;");
   hist->SetFillColor(kGray);
   hist->SetBit(TH1::kCanRebin);
   o[shName] = hist;

   //shName = "hsTvsACumul";
   //o[shName] = new THStack(shName.c_str(), shName.c_str());

   DrawObjContainer        *oc;
   DrawObjContainerMapIter  isubdir;

   for (int iChId=1; iChId<=N_SILICON_CHANNELS; iChId++)
   {
      string sChId(MAX_CHANNEL_DIGITS, ' ');
      sprintf(&sChId[0], "%02d", iChId);

      string dName = "channel" + sChId;

      isubdir = d.find(dName);

      if ( isubdir == d.end()) { // if dir not found
         oc = new DrawObjContainer();
         oc->fDir = new TDirectoryFile(dName.c_str(), dName.c_str(), "", fDir);
      } else {
         oc = isubdir->second;
      }

      oc->fDir->cd();

      shName = "hAdcAmpltd_ch" + sChId;
      hist = new TH1I(shName.c_str(), shName.c_str(), 255, 0, 255);
      hist->SetOption("hist NOIMG");
      hist->SetTitle("; Amplitude, ADC; Events;");
      hist->SetFillColor(kGray);
      oc->o[shName] = hist;

      shName = "hAdcIntgrl_ch" + sChId;
      hist = new TH1I(shName.c_str(), shName.c_str(), 255, 0, 255);
      hist->SetOption("hist NOIMG");
      hist->SetTitle("; Integral, ADC; Events;");
      hist->SetFillColor(kGray);
      oc->o[shName] = hist;

      shName = "hTdc_ch" + sChId;
      hist = new TH1I(shName.c_str(), shName.c_str(), 80, 10, 90);
      hist->SetOption("hist NOIMG");
      hist->SetTitle("; TDC; Events;");
      hist->SetFillColor(kGray);
      oc->o[shName] = hist;

      shName = "hTvsA_ch" + sChId;
      hist = new TH2S(shName.c_str(), shName.c_str(), 255, 0, 255, 80, 10, 90);
      hist->SetOption("colz LOGZ");
      hist->SetTitle("; Amplitude, ADC; TDC;");
      oc->o[shName]      = hist;
      fhTvsA_ch[iChId-1] = hist;

      shName = "hTvsI_ch" + sChId;
      hist = new TH2S(shName.c_str(), shName.c_str(), 255, 0, 255, 80, 10, 90);
      hist->SetOption("colz LOGZ");
      hist->SetTitle("; Integral, ADC; TDC;");
      oc->o[shName]      = hist;
      fhTvsI_ch[iChId-1] = hist;

      shName = "hIvsA_ch" + sChId;
      hist = new TH2S(shName.c_str(), shName.c_str(), 255, 0, 255, 255, 0, 255);
      hist->SetOption("colz LOGZ");
      hist->SetTitle("; Amplitude, ADC; Integral, ADC;");
      oc->o[shName]      = hist;
      fhIvsA_ch[iChId-1] = hist;

      //shName = "hTvsACumul_ch" + sChId;
      //hist = new TH1F(shName.c_str(), shName.c_str(), 100, 0, 1);
      //hist->SetOption("hist NOIMG");
      //hist->SetTitle("; Digi Channel Frac; Event Frac;");
      //hist->SetLineWidth(2);
      //hist->SetLineColor(RunConfig::AsColor(iChId));
      //hist->GetYaxis()->SetRangeUser(0, 1);
      //oc->o[shName]           = hist;
      //fhTvsACumul_ch[iChId-1] = hist;
      //((THStack*) o["hsTvsACumul"])->Add(hist);

      // If this is a new directory then we need to add it to the list
      if ( isubdir == d.end()) {
         d[dName] = oc;
      }
   }
}


/** */
void CnipolRawHists::FillPassOne(ChannelEvent *ch)
{
   UChar_t  chId     = ch->GetChannelId();
   UShort_t adcA_bin = ch->GetAmpltd() + 1;
   UShort_t adcI_bin = ch->GetIntgrl() + 1;
   UShort_t tdc_bin  = ch->GetTdc() - 10 + 1; // 10 is the lowest edge of the TvsA histograms

   // Speed up the filling process by getting the global bin number
   TH1* hist;
   Int_t gbin;

   hist = fhTvsA_ch[chId-1];
   gbin = hist->GetBin(adcA_bin, tdc_bin);
   hist->AddBinContent(gbin);
   hist->SetEntries(hist->GetEntries()+1);

   hist = fhTvsI_ch[chId-1];
   gbin = hist->GetBin(adcI_bin, tdc_bin);
   hist->AddBinContent(gbin);
   hist->SetEntries(hist->GetEntries()+1);

   hist = fhIvsA_ch[chId-1];
   gbin = hist->GetBin(adcA_bin, adcI_bin);
   hist->AddBinContent(gbin);
   hist->SetEntries(hist->GetEntries()+1);

   hist = (TH1*) o["hBunchCounts"];
   hist->AddBinContent(ch->GetBunchId() + 1);
   hist->SetEntries(hist->GetEntries()+1);

   hist = ((TH1*) o["hStripCounts"]);
   hist->AddBinContent(chId);
   hist->SetEntries(hist->GetEntries()+1);

   hist = ((TH1*) o["hRevolutionId"]);
   hist->Fill(ch->GetRevolutionId());
}


/** */
void CnipolRawHists::FillDerivedPassOne()
{
   Info("FillDerivedPassOne", "Called");

   TH1* hAdcAmpltd = (TH1*) o["hAdcAmpltd"];
   TH1* hAdcIntgrl = (TH1*) o["hAdcIntgrl"];
   TH1* hTdc       = (TH1*) o["hTdc"];
   TH1* hTvsA      = (TH1*) o["hTvsA"];
   TH1* hTvsI      = (TH1*) o["hTvsI"];
   TH1* hIvsA      = (TH1*) o["hIvsA"];
   
   for (UShort_t iCh=1; iCh<=N_SILICON_CHANNELS; iCh++)
   {
      string sChId("  ");
      sprintf(&sChId[0], "%02d", iCh);

      DrawObjContainer *oc = d.find("channel"+sChId)->second;

      TH2* hTVsA_channel = (TH2*) fhTvsA_ch[iCh-1];
      hTVsA_channel->Sumw2();
      hTvsA->Add(hTVsA_channel);

      TH2* hTVsI_channel = (TH2*) fhTvsI_ch[iCh-1];
      hTvsI->Add(hTVsI_channel);

      TH2* hIVsA_channel = (TH2*) fhIvsA_ch[iCh-1];
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

      // Calculate cumulative histograms
      //utils::ConvertToCumulative2(hTVsA_channel, (TH1F*) fhTvsACumul_ch[iCh-1]);
   }
}


/** */
void CnipolRawHists::PostFillPassOne(DrawObjContainer *oc)
{
   Info("PostFillPassOne", "Called");

   // We expect empty bunch histogram container
   string thisDirName(fDir->GetName()); 

   if (!oc || thisDirName.compare("raw_neb") != 0 ||
        oc->d.find("raw") == oc->d.end() || oc->d.find("raw_eb") == oc->d.end() )
   {
      Error("PostFillPassOne", "Cannot proceed: No appropriate empty bunch container found");
      exit(-1);
      return;
   }

   CnipolRawHists* rawHists    = (CnipolRawHists*) oc->d.find("raw")->second;
   CnipolRawHists* rawHists_eb = (CnipolRawHists*) oc->d.find("raw_eb")->second;

   ChannelSetIter iCh = gMeasInfo->fSiliconChannels.begin();

   for (; iCh!=gMeasInfo->fSiliconChannels.end(); ++iCh)
   {
      UShort_t chId     = *iCh;
      TH1* hTvsA_ch     = (TH1F*) rawHists->GetHTvsA_ch(chId);
      TH1* hTvsA_ch_eb  = (TH1F*) rawHists_eb->GetHTvsA_ch(chId);
      TH1* hTvsA_ch_neb = (TH1F*) GetHTvsA_ch(chId);

      if ( !hTvsA_ch || !hTvsA_ch_eb ) {
         Error("PostFillPassOne", "Histogram %s not found", hTvsA_ch->GetName());
         continue;
      }

      //fhTvsA_ch->Print();
      //fhTvsA_ch_this_copy->Print();

      // Subtract empty bunch data from all bunch data
      //fhTvsA_ch_eb->Scale( );
      //fhTvsA_ch_this_copy->Add(fhTvsA_ch_eb, -1);
      //fhTvsA_ch_this_copy->Print();

      Float_t scale = N_BUNCHES / (Float_t) gMeasInfo->GetNumEmptyBunches();

      //hTvsA_ch_neb->Sumw2();
      //hTvsA_ch_neb->Add(hTvsA_ch, hTvsA_ch_eb, 1, -1*scale);

      // After the subtraction set bins with negative content to 0 including under/overflows
      for (Int_t ibx=0; ibx<=hTvsA_ch_neb->GetNbinsX()+1; ibx++) {
         for (Int_t iby=0; iby<=hTvsA_ch_neb->GetNbinsY()+1; iby++) {

            Double_t bc     = hTvsA_ch->GetBinContent(ibx, iby);
            Double_t bc_eb  = hTvsA_ch_eb->GetBinContent(ibx, iby);
            //Double_t bc_neb = hTvsA_ch_neb->GetBinContent(ibx, iby);
            Int_t    bc_neb = bc - gRandom->Poisson(bc_eb*scale);

            bc_neb = bc_neb < 0 ? 0 : bc_neb;

            hTvsA_ch_neb->SetBinContent(ibx, iby, bc_neb);
            hTvsA_ch_neb->SetBinError(ibx, iby, TMath::Sqrt(bc_neb));

            //if (bc_neb < 0) {
            //   hTvsA_ch_neb->SetBinContent(ibx, iby, 0);
            //   hTvsA_ch_neb->SetBinError(ibx, iby, 0);
            //}
         }
      }
   }
}


/** */
TH1* CnipolRawHists::GetHTvsA_ch(UShort_t chId)
{
   return fhTvsA_ch[chId-1];
}
