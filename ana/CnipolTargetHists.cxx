/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolTargetHists.h"

#include "AsymGlobals.h"


ClassImp(CnipolTargetHists)

using namespace std;

/** Default constructor. */
CnipolTargetHists::CnipolTargetHists() : DrawObjContainer()
{
   BookHists();
}


CnipolTargetHists::CnipolTargetHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
}


/** Default destructor. */
CnipolTargetHists::~CnipolTargetHists()
{
}


/** */
void CnipolTargetHists::BookHists(string sid)
{ //{{{
   fDir->cd();

   if (!sid.empty()) sid = "_" + sid;

   char hName[256];

   sprintf(hName, "hTargetChanVertYel");
   o[hName] = new TH1F(hName, hName, 255, 0, 255);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("ADC");

   sprintf(hName, "hTargetChanHorzYel");
   o[hName] = new TH1F(hName, hName, 255, 0, 255);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("ADC");

   sprintf(hName, "hTargetChanVertBlu");
   o[hName] = new TH1F(hName, hName, 255, 0, 255);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("ADC");

   sprintf(hName, "hTargetChanHorzBlu");
   o[hName] = new TH1F(hName, hName, 255, 0, 255);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("ADC");

   sprintf(hName, "hTargetHorzLinear");
   o[hName] = new TH1F(hName, hName, 100, 0, 100);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Time");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Position");
   //((TH1*) o[hName])->SetBit(TH1::kCanRebin);

   sprintf(hName, "hTargetHorzRotary");
   o[hName] = new TH1F(hName, hName, 100, 0, 100);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Time");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Position");
   //((TH1*) o[hName])->SetBit(TH1::kCanRebin);

   sprintf(hName, "hTargetVertLinear");
   o[hName] = new TH1F(hName, hName, 100, 0, 100);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Time");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Position");
   //((TH1*) o[hName])->SetBit(TH1::kCanRebin);

   sprintf(hName, "hTargetVertRotary");
   o[hName] = new TH1F(hName, hName, 100, 0, 100);
   //o[hName] = new TH1F(hName, hName, 1, 0, 1);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Time");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Position");
   //((TH1*) o[hName])->SetBit(TH1::kCanRebin);
} //}}}


/** */
void CnipolTargetHists::Fill(ChannelEvent *ch, string sid)
{ //{{{
   UChar_t chId  = ch->GetChannelId();
   //UChar_t detId = ch->GetDetectorId();

   //string sDetId(" ");
   //sprintf(&sDetId[0], "%1d", detId);
   //char hName[256];

   //if (!ch->PassCutTargetChannel()) return;

   UChar_t chIndex = chId - cfginfo->data.NumChannels + 4;
   //UChar_t chIndex = chId - NSTRIP;

   switch (chIndex) {
   case 1:
      ((TH1F*) o["hTargetChanVertYel"])->Fill(ch->fChannel.fAmpltd);
      break;
   case 2:
      ((TH1F*) o["hTargetChanHorzYel"])->Fill(ch->fChannel.fAmpltd);
      break;
   case 3:
      ((TH1F*) o["hTargetChanVertBlu"])->Fill(ch->fChannel.fAmpltd);
      break;
   case 4:
      ((TH1F*) o["hTargetChanHorzBlu"])->Fill(ch->fChannel.fAmpltd);
      break;
   }
} //}}}


/**
 * The size of hData is 4*n+4
 */
void CnipolTargetHists::Fill(Int_t n, Double_t* hData)
{ //{{{
   ((TH1*) o["hTargetHorzLinear"])->SetBins(n, 0, n);
   ((TH1*) o["hTargetHorzLinear"])->SetContent(hData);

   ((TH1*) o["hTargetHorzRotary"])->SetBins(n, 0, n);
   ((TH1*) o["hTargetHorzRotary"])->SetContent(hData + (n+2) - 1);

   ((TH1*) o["hTargetVertLinear"])->SetBins(n, 0, n);
   ((TH1*) o["hTargetVertLinear"])->SetContent(hData + 2*(n+2) - 1);

   ((TH1*) o["hTargetVertRotary"])->SetBins(n, 0, n);
   ((TH1*) o["hTargetVertRotary"])->SetContent(hData + 3*(n+2) - 1);
} //}}}
