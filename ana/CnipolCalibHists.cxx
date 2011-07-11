/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolCalibHists.h"
#include "ChannelEventId.h"

ClassImp(CnipolCalibHists)

using namespace std;

/** Default constructor. */
CnipolCalibHists::CnipolCalibHists() : DrawObjContainer()
{
   BookHists();
}



CnipolCalibHists::CnipolCalibHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
}


/** Default destructor. */
CnipolCalibHists::~CnipolCalibHists()
{
}


void CnipolCalibHists::BookHists(std::string cutid)
{ //{{{
   char hName[256];

   sprintf(hName, "hDLVsChannel");
   o[hName]      = new TH1F(hName, hName, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   ((TH1*) o[hName])->SetTitle("Amplitude, ADC;Dead Layer");
   // Amplitude with a cut applied on TDC and then loose cut on amplitude itself
   //o["hAmpltd_cut1"] = new TH1F("hAmpltd_cut1", "hAmpltd_cut1", 35, 165, 200);

} //}}}


/** */
void CnipolCalibHists::Fill(ChannelEvent *ch, string sid)
{ //{{{
   UChar_t      chId = ch->GetChannelId();
} //}}}


/** */
void CnipolCalibHists::FillPreProcess(ChannelEvent *ch) { }


/** */
void CnipolCalibHists::PostFill()
{ //{{{
   // Adjust axis ranges
   Int_t  maxBinA = ((TH1F*) o["hAmpltd_cut1"])->GetMaximumBin();
   Double_t xminA = ((TH1F*) o["hAmpltd_cut1"])->GetXaxis()->GetXmin();
   Double_t xmaxA = ((TH1F*) o["hAmpltd_cut1"])->GetXaxis()->GetXmax();

   xminA = maxBinA - 50 < xminA ? xminA : maxBinA - 50;
   xmaxA = maxBinA + 50 > xmaxA ? xmaxA : maxBinA + 50;

   ((TH1F*) o["hAmpltd_cut1"])->SetAxisRange(xminA, xmaxA);

   Int_t  maxBinI = ((TH1F*) o["hIntgrl_cut1"])->GetMaximumBin();
   Double_t xminI = ((TH1F*) o["hIntgrl_cut1"])->GetXaxis()->GetXmin();
   Double_t xmaxI = ((TH1F*) o["hIntgrl_cut1"])->GetXaxis()->GetXmax();

   xminI = maxBinI - 50 < xminI ? xminI : maxBinI - 50;
   xmaxI = maxBinI + 50 > xmaxI ? xmaxI : maxBinI + 50;

   ((TH1F*) o["hIntgrl_cut1"])->SetAxisRange(xminI, xmaxI);

   printf("xminA, xmaxA, xminI, xmaxI: %f, %f, %f, %f\n", xminA, xmaxA, xminI, xmaxI);

   string  sSi("  ");

   for (int i=1; i<=NSTRIP; i++) {
      sprintf(&sSi[0], "%02d", i);

      //maxBin = ((TH1F*) d["channel"+sSi].o["hAmpltd_cut1_st"+sSi])->GetMaximumBin();
      //xmin   = ((TH1F*) d["channel"+sSi].o["hAmpltd_cut1_st"+sSi])->GetXaxis()->GetXmin();
      //xmax   = ((TH1F*) d["channel"+sSi].o["hAmpltd_cut1_st"+sSi])->GetXaxis()->GetXmax();
      //xmin   = maxBin - 50 < xmin ? xmin : maxBin - 50;
      //xmax   = maxBin + 50 > xmax  ? xmax : maxBin + 50;

      ((TH1F*) d["channel"+sSi]->o["hAmpltd_cut1_st"+sSi])->SetAxisRange(xminA, xmaxA);
      ((TH1F*) d["channel"+sSi]->o["hIntgrl_cut1_st"+sSi])->SetAxisRange(xminI, xmaxI);
   }
} //}}}
