/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef AnaEventId_h
#define AnaEventId_h

#include <limits.h>

#include "TObject.h"

#include "ChannelEventId.h"

/**
 *
 */
class AnaEventId : public TObject
{
public:

   UInt_t  fRevolutionId;
   UChar_t fBunchId;

public:

   AnaEventId();
   ~AnaEventId();

   virtual void Print(const Option_t* opt="") const;
   AnaEventId & operator=(const AnaEventId &evId);
   //bool operator<(const AnaEventId &anaEvId, const ChannelEventId &chEvId);
   bool operator<(const ChannelEventId &chEvId);
   AnaEventId & operator=(const ChannelEventId &evId);
   void Assign(const ChannelEventId &evId);

   ClassDef(AnaEventId, 1)
};

#endif
