#include "AnaEvent.h"

ClassImp(AnaEvent)


/** Default constructor. */
AnaEvent::AnaEvent() : TObject(), fEventId(), fChannels()
{
};


/** Default destructor. */
AnaEvent::~AnaEvent()
{
};


/**
 *
 */
void AnaEvent::Print(const Option_t* opt) const
{
   printf("AnaEvent:\n");

   ChannelDataMap::const_iterator mi;
   ChannelDataMap::const_iterator mb = fChannels.begin();
   ChannelDataMap::const_iterator me = fChannels.end();

   for (mi=mb; mi!=me; mi++) {
		mi->second.Print();
      printf("\n");
   }
}
