#include "AnaEventId.h"

ClassImp(AnaEventId)

using namespace std;

/** Default constructor. */
AnaEventId::AnaEventId() : TObject(), fRevolutionId(UINT_MAX), fBunchId(UCHAR_MAX)
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
   printf("AnaEventId:\n");
   printf("RevId, BunchId: %12d, %12d", fRevolutionId, fBunchId);
}


AnaEventId & AnaEventId::operator=(const AnaEventId &evId)
{
   this->fRevolutionId = evId.fRevolutionId;
   this->fBunchId      = evId.fBunchId;

   return *this;
}


bool AnaEventId::operator<(const ChannelEventId &chEvId)
{
   if (this->fRevolutionId < chEvId.fRevolutionId) return true;
   else if (this->fRevolutionId == chEvId.fRevolutionId) {

      if (this->fBunchId < chEvId.fBunchId) return true;
      else return false;

   } else return false;
}


AnaEventId & AnaEventId::operator=(const ChannelEventId &chEvId)
{
   this->fRevolutionId = chEvId.fRevolutionId;
   this->fBunchId      = chEvId.fBunchId;

   return *this;
}


void AnaEventId::Assign(const ChannelEventId &chEvId)
{
   fRevolutionId = chEvId.fRevolutionId;
   fBunchId      = chEvId.fBunchId;
}
