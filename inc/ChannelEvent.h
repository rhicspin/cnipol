/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef ChannelEvent_h
#define ChannelEvent_h

#include <limits.h>
#include <map>
#include <set>
#include <vector>

#include "TObject.h"

#include "ChannelData.h"
#include "ChannelEventId.h"

class ChannelEvent;

typedef std::vector<ChannelEvent> ChannelEventVec;
typedef std::map<ChannelEventId, ChannelEvent, ChannelEventId> ChannelEventMap;
typedef std::set<ChannelEvent, ChannelEvent> ChannelEventSet;

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

   //virtual void Print(const Option_t* opt="") const;
   void Print(const Option_t* opt="") const;
   bool operator()(const ChannelEvent &ch1, const ChannelEvent &ch2);

   ClassDef(ChannelEvent, 1)
};

#endif
