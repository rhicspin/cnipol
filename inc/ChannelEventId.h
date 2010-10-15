/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef ChannelEventId_h
#define ChannelEventId_h

#include "TObject.h"

/**
 *
 */
class ChannelEventId : public TObject
{
public:

   Int_t    fRevolutionId;
   Short_t  fBunchId;
   Short_t  fChannelId;

public:

   ChannelEventId();
   ~ChannelEventId();

   virtual void Print(const Option_t* opt="") const;

   ClassDef(ChannelEventId, 1)
};

#endif
