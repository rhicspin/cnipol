/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef ChannelEvent_h
#define ChannelEvent_h

#include <map>
#include <vector>

#include "TObject.h"

#include "ChannelData.h"
#include "ChannelEventId.h"

class ChannelEvent;

typedef std::vector<ChannelEvent> ChannelEventVec;
typedef std::map<ChannelEventId, ChannelEvent, ChannelEventId> ChannelEventMap;

/**
 *
 */
class ChannelEvent : public TObject
{
public:

	ChannelEventId fEventId;
   ChannelData    fChannel;

public:

   ChannelEvent();
   ~ChannelEvent();

   virtual void Print(const Option_t* opt="") const;

   ClassDef(ChannelEvent, 1)
};

#endif
