/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef ChannelEventId_h
#define ChannelEventId_h

#include <limits.h>

#include "TObject.h"


/** */
class ChannelEventId : public TObject
{
public:

   UInt_t   fRevolutionId;
   UChar_t  fBunchId;
   UChar_t  fChannelId;

public:

   ChannelEventId();
   ~ChannelEventId();

   virtual void Print(const Option_t* opt="") const;
   //static bool  Compare(const ChannelEventId &id1, const ChannelEventId &id2);
   bool operator()(const ChannelEventId &id1, const ChannelEventId &id2);
   //bool operator<(const ChannelEventId &chEvId) const;
   friend bool operator<(const ChannelEventId &chEvId1, const ChannelEventId &chEvId2);

   ClassDef(ChannelEventId, 1)
};

#endif
