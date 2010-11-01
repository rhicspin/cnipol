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

   printf("ChannelEventId: \n");
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


/**
 *
 */
//bool ChannelEventId::operator<(const ChannelEventId &chEvId) const
//{
//   if (this->fRevolutionId < chEvId.fRevolutionId) return true;
//   else if (this->fRevolutionId == chEvId.fRevolutionId) {
//
//      if (this->fBunchId < chEvId.fBunchId) return true;
//      else if (this->fBunchId == chEvId.fBunchId) {
//
//         if (this->fChannelId < chEvId.fChannelId) return true;
//         else return false;
//
//      } else return false;
//
//   } else return false;
//}


/**
 *
 */
bool operator<(const ChannelEventId &chEvId1, const ChannelEventId &chEvId2)
{
   if (chEvId1.fRevolutionId < chEvId2.fRevolutionId) return true;
   else if (chEvId1.fRevolutionId == chEvId2.fRevolutionId) {

      if (chEvId1.fBunchId < chEvId2.fBunchId) return true;
      else if (chEvId1.fBunchId == chEvId2.fBunchId) {

         if (chEvId1.fChannelId < chEvId2.fChannelId) return true;
         else return false;

      } else return false;

   } else return false;
}
