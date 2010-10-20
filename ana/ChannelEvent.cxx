/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "ChannelEvent.h"

ClassImp(ChannelEvent)

using namespace std;

/** Default constructor. */
ChannelEvent::ChannelEvent() : TObject(), fEventId(), fChannel()
{
};


/** Default destructor. */
ChannelEvent::~ChannelEvent()
{
};


/**
 *
 */
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
