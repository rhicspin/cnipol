/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "ChannelEventId.h"

ClassImp(ChannelEventId)

using namespace std;

/** Default constructor. */
ChannelEventId::ChannelEventId() : TObject(), fRevolutionId(ULONG_MAX),
   fBunchId(UCHAR_MAX), fChannelId(UCHAR_MAX)
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

   //printf("ChannelEventId: ");
   printf("RevId, BuId, ChId: %12d, %12d, %12d", fRevolutionId, fBunchId, fChannelId);
}


/**
 *
 */
//static bool ChannelEventId::Compare(const ChannelEventId &id1, const ChannelEventId &id2)
bool ChannelEventId::operator()(const ChannelEventId &id1, const ChannelEventId &id2)
{
   if (id1.fRevolutionId < id2.fRevolutionId) return true;
   else if (id1.fRevolutionId == id2.fRevolutionId) {

      if (id1.fBunchId < id2.fBunchId) return true;
      else if (id1.fBunchId == id2.fBunchId) {

         if (id1.fChannelId < id2.fChannelId) return true;
         else return false;

      } else return false;

   } else return false;
}
