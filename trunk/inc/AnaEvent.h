/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef AnaEvent_h
#define AnaEvent_h

#include <map>
#include <set>
#include <vector>

#include "TObject.h"

#include "AnaEventId.h"
#include "EventConfig.h"
#include "ChannelData.h"

//class EventConfig;
class AnaEvent;

typedef std::vector<AnaEvent> AnaEventVec;
typedef std::map<AnaEventId, AnaEvent> AnaEventMap;
typedef std::set<AnaEvent, AnaEvent> AnaEventSet;

/**
 *
 */
class AnaEvent : public TObject
{
public:

	AnaEventId     fEventId;
   //GntChannelVec fChannels;
   ChannelDataMap fChannels;

public:

   AnaEvent();
   ~AnaEvent();

   float GetEnergy(EventConfig *ec, ChannelDataPair &ch);
   float GetEnergyIntegral(EventConfig *ec, ChannelDataPair &ch);
   float GetTime(EventConfig *ec, ChannelDataPair &ch);
   virtual void Print(const Option_t* opt="") const;

   ClassDef(AnaEvent, 1)
};

#endif
