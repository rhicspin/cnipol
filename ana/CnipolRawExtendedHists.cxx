/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolRawExtendedHists.h"

#include "utils/utils.h"


ClassImp(CnipolRawExtendedHists)

using namespace std;

/** Default constructor. */
CnipolRawExtendedHists::CnipolRawExtendedHists() : CnipolRawHists()
{
   BookHists();
}


CnipolRawExtendedHists::CnipolRawExtendedHists(TDirectory *dir) : CnipolRawHists(dir)
{
   BookHists();
}


/** Default destructor. */
CnipolRawExtendedHists::~CnipolRawExtendedHists()
{
}


/** */
void CnipolRawExtendedHists::BookHists()
{
   string shName;
   TH1*   hist;

   fDir->cd();

   shName = "hChIdVsBunchId";
   hist = new TH2I(shName.c_str(), shName.c_str(), N_BUNCHES, 0.5, N_BUNCHES+0.5, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   hist->SetTitle("; Bunch Id; Channel Id; ");
   hist->SetOption("colz LOGZ");
   o[shName] = hist;


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


      for (UShort_t iBunchId=1; iBunchId<=N_BUNCHES; iBunchId++)
      {
         string sBunchId("   ");
         sprintf(&sBunchId[0], "%03d", iBunchId);

         // bunched histograms start here
         shName = "hTvsA_ch" + sChId + "_b" + sBunchId;
         hist = new TH2S(shName.c_str(), shName.c_str(), 255, 0, 255, 80, 10, 90);
         hist->SetOption("colz LOGZ NOIMG");
         hist->SetTitle(";Amplitude, ADC;TDC;");
         oc->o[shName] = hist;
         fhTvsA_ch_b[iChId-1][iBunchId-1] = hist;

         //shName = "hTvsACumul_ch" + sChId + "_b" + sBunchId;
         //hist = new TH1F(shName.c_str(), shName.c_str(), 100, 0, 1);
         //hist->SetOption("hist NOIMG");
         //hist->SetTitle("; Digi Channel Frac; Event Frac;");
         //hist->GetYaxis()->SetRangeUser(0, 1);
         //oc->o[shName] = hist;
         //fhTvsACumul_ch_b[iChId-1][iBunchId-1] = hist;
      }

      // If this is a new directory then we need to add it to the list
      if ( isubdir == d.end()) {
         d[dName] = oc;
      }
   }
}


/** */
void CnipolRawExtendedHists::FillPassOne(ChannelEvent *ch)
{
   // Fill parent histograms
   //CnipolRawHists::FillPassOne(ch);

   UChar_t  chId     = ch->GetChannelId();
   UShort_t adcA_bin = ch->GetAmpltd() + 1;
   UShort_t adcI_bin = ch->GetIntgrl() + 1;
   UShort_t tdc_bin  = ch->GetTdc() - 10 + 1; // 10 is the lowest edge of the TvsA histograms
   UChar_t  bId      = ch->GetBunchId() + 1;

   // Speed up the filling process by getting the global bin number
   TH1* hist;
   Int_t gbin;

   //hist = fhTvsA_ch[chId-1];
   //gbin = hist->GetBin(adcA_bin, tdc_bin);
   //hist->AddBinContent(gbin);
   //hist->SetEntries(hist->GetEntries()+1);

   hist = fhTvsA_ch_b[chId-1][bId-1];
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

   //hist = (TH1*) o["hBunchCounts"];
   //hist->AddBinContent(ch->GetBunchId() + 1);
   //hist->SetEntries(hist->GetEntries()+1);

   //hist = ((TH1*) o["hStripCounts"]);
   //hist->AddBinContent(chId);
   //hist->SetEntries(hist->GetEntries()+1);

   hist = ((TH1*) o["hRevolutionId"]);
   hist->Fill(ch->GetRevolutionId());
}


/** */
void CnipolRawExtendedHists::FillDerivedPassOne()
{
   Info("FillDerivedPassOne", "Called");

   for (UShort_t iCh=1; iCh<=N_SILICON_CHANNELS; iCh++)
   {
      TH1* hTvsA_ch = (TH1*) fhTvsA_ch[iCh-1];

      for (UShort_t iBunchId=1; iBunchId<=N_BUNCHES; iBunchId++)
      {
         TH2* hTvsA_ch_b = (TH2*) fhTvsA_ch_b[iCh-1][iBunchId-1];

         hTvsA_ch->Add(hTvsA_ch_b);

         Double_t nEntries = hTvsA_ch_b->GetEntries();
         ((TH2I*) o["hChIdVsBunchId"])->SetBinContent(iBunchId, iCh, nEntries);
         ((TH2I*) o["hChIdVsBunchId"])->SetEntries(((TH2I*) o["hChIdVsBunchId"])->GetEntries() + nEntries);

         TH1* hist = ((TH1*) o["hBunchCounts"]);
         hist->AddBinContent(iBunchId, nEntries);
         hist->SetEntries(hist->GetEntries()+nEntries);

         //utils::ConvertToCumulative2(hTvsA_ch_b, (TH1F*) fhTvsACumul_ch_b[iCh-1][iBunchId-1]);
      }

      ((TH1I*) o["hStripCounts"])->SetBinContent(iCh, hTvsA_ch->GetEntries());
      ((TH1I*) o["hStripCounts"])->SetEntries(((TH1I*) o["hStripCounts"])->GetEntries() + hTvsA_ch->GetEntries());

      //if (!gCh2WfdMap) continue;

      //((TH1I*) o["hWfdCounts"])->AddBinContent(gCh2WfdMap[iCh-1], hTvsA_ch->GetEntries());
      //((TH1I*) o["hWfdCounts"])->SetEntries(((TH1I*) o["hWfdCounts"])->GetEntries() + hTvsA_ch->GetEntries());
   }

   // Fill parent class histograms
   CnipolRawHists::FillDerivedPassOne();
}


/** */
//void CnipolRawExtendedHists::PostFillPassOne(DrawObjContainer *oc=0)
//{
//}
