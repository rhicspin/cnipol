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
void CnipolRawHists::BookHists(string sid)
{ //{{{
   char hName[256];
   char htitle[256];

   fDir->cd();

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

   sprintf(hName, "adc_raw");
   sprintf(htitle, "%.3f: Raw ADC (All Strips)", gRunInfo->RUNID);
   o[hName] = new TH1F(hName, htitle, 255, 0, 255);
   ((TH2F*) o[hName])->GetXaxis()->SetTitle("ADC");
   ((TH2F*) o[hName])->GetYaxis()->SetTitle("Events");
   ((TH2F*) o[hName])->SetFillColor(kGray);

   sprintf(hName, "tdc_vs_adc_raw");
   sprintf(htitle, "%.3f: Raw TDC vs ADC (All Strips)", gRunInfo->RUNID);
   o[hName] = new TH2F(hName, htitle, 255, 0, 255, 100, 0, 100);
   ((TH2F*) o[hName])->GetXaxis()->SetTitle("ADC");
   ((TH2F*) o[hName])->GetYaxis()->SetTitle("TDC");
   ((TH2F*) o[hName])->SetFillColor(kGray);

   sprintf(hName, "tdc_vs_adc_false_bunch_raw");
   sprintf(htitle, "%.3f: Raw TDC vs ADC (All Strips) false bunch", gRunInfo->RUNID);
   o[hName] = new TH2F(hName, htitle, 255, 0, 255, 100, 0, 100);
   ((TH2F*) o[hName])->GetXaxis()->SetTitle("ADC");
   ((TH2F*) o[hName])->GetYaxis()->SetTitle("TDC");
   ((TH2F*) o[hName])->SetFillColor(kRed);
} //}}}


/** */
void CnipolRawHists::Print(const Option_t* opt) const
{ //{{{
   opt = "";

   //printf("CnipolRawHists:\n");
   //DrawObjContainer::Print();
} //}}}


/** */
void CnipolRawHists::Fill(ChannelEvent *ch, string sid)
{ //{{{
   // Fill events with no cuts applied
   if (sid == "") {

      ((TH1*) o["bunch_dist_raw"])->Fill(ch->GetChannelId());
      ((TH1*) o["strip_dist_raw"])->Fill(ch->GetBunchId());
      ((TH1*) o["tdc_raw"])       ->Fill(ch->GetTdc());
      ((TH1*) o["adc_raw"])       ->Fill(ch->GetAmpltd());
      ((TH1*) o["tdc_vs_adc_raw"])->Fill(ch->GetAmpltd(), ch->GetTdc());

   } else if (sid == "cut_false_bunch") {

      ((TH1*) o["tdc_vs_adc_false_bunch_raw"])->Fill(ch->GetAmpltd(), ch->GetTdc());
   }
} //}}}
