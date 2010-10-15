/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "ChannelEventId.h"

ClassImp(ChannelEventId)

using namespace std;

/** Default constructor. */
ChannelEventId::ChannelEventId() : TObject(), fRevolutionId(-1), fBunchId(-1), fChannelId(-1)
{
};


/** Default destructor. */
ChannelEventId::~ChannelEventId()
{
};


/**
 *
 */
void ChannelEventId::Print(const Option_t* opt) const
{
   opt = "";

   printf("ChannelEventId:\n");
}
