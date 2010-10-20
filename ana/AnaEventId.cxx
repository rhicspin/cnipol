/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "AnaEventId.h"

ClassImp(AnaEventId)

using namespace std;

/** Default constructor. */
AnaEventId::AnaEventId() : TObject(), fRevolutionId(ULONG_MAX), fBunchId(UCHAR_MAX)
{
};


/** Default destructor. */
AnaEventId::~AnaEventId()
{
};


/**
 *
 */
void AnaEventId::Print(const Option_t* opt) const
{
   opt = "";

   printf("AnaEventId:\n");
}


AnaEventId & AnaEventId::operator=(const AnaEventId &evId)
{
   this->fRevolutionId = evId.fRevolutionId;
   this->fBunchId      = evId.fBunchId;

   return *this;
}


//bool AnaEventId::operator<(const AnaEventId &anaEvId, const ChannelEventId &evId)
bool AnaEventId::operator<(const ChannelEventId &evId)
{
   if (this->fRevolutionId < evId.fRevolutionId) return true;
   else if (this->fRevolutionId == evId.fRevolutionId) {

      if (this->fBunchId < evId.fBunchId) return true;
      else return false;

   } else return false;
}


AnaEventId & AnaEventId::operator=(const ChannelEventId &evId)
{
   this->fRevolutionId = evId.fRevolutionId;
   this->fBunchId      = evId.fBunchId;

   return *this;
}


void AnaEventId::Assign(const ChannelEventId &evId)
{
   fRevolutionId = evId.fRevolutionId;
   fBunchId      = evId.fBunchId;
}
