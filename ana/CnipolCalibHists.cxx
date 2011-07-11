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
} //}}}
