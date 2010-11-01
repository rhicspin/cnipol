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


bool ChannelEvent::operator()(const ChannelEvent &ch1, const ChannelEvent &ch2)
{
   if (ch1.fEventId < ch2.fEventId) return true;

   return false;
}
