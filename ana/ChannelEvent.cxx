/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "ChannelEvent.h"

ClassImp(ChannelEvent)

using namespace std;

/** Default constructor. */
ChannelEvent::ChannelEvent() : TObject(), fEventConfig(0), fEventId(),
   fChannel()
{
}


/** Default destructor. */
ChannelEvent::~ChannelEvent()
{
}


/** */
Float_t ChannelEvent::GetEnergyA()
{
   UChar_t chId = fEventId.fChannelId;
   return fEventConfig->fConfigInfo->data.chan[chId].acoef * fChannel.fAmpltd;

   //return (fEventConfig->fConfigInfo->data.WFDTUnit/2.) *
   //       (fChannel.fAmpltd + fEventConfig->fRandom->Rndm() - 0.5);
}


/** */
Float_t ChannelEvent::GetEnergyI()
{
   UChar_t chId = fEventId.fChannelId;
   // XXX acoef has to be changed to ... icoef
   return fEventConfig->fConfigInfo->data.chan[chId].acoef * fChannel.fIntgrl;
}


/** */
Float_t ChannelEvent::GetTime()
{
   return (fEventConfig->fConfigInfo->data.WFDTUnit/2.) *
          (fChannel.fTdc + fEventConfig->fRandom->Rndm() - 0.5);
}


/** */
void ChannelEvent::Print(const Option_t* opt) const
{
   opt = "";

   printf("ChannelEvent:");
   printf("\n\t");
   fEventId.Print();
   printf("\n\t");
   fChannel.Print();
   printf("\n");
}


/** */
bool ChannelEvent::operator()(const ChannelEvent &ch1, const ChannelEvent &ch2)
{
   if (ch1.fEventId < ch2.fEventId) return true;

   return false;
}


/** */
Bool_t ChannelEvent::PassQACut1()
{
   //if (GetEnergyA() < 150) return false;  // keV
   if (GetEnergyA() > 1150) return false; // keV

   if (GetTime() < 15) return false; // ns
   if (GetTime() > 75) return false; // ns

   return true;
}
