#ifndef AnaEvent_h
#define AnaEvent_h

#include <map>
#include <set>
#include <vector>

#include "TObject.h"

#include "AnaEventId.h"
#include "EventConfig.h"
#include "ChannelData.h"

class AnaEvent;

typedef std::vector<AnaEvent> AnaEventVec;
typedef std::map<AnaEventId, AnaEvent> AnaEventMap;
typedef std::set<AnaEvent, AnaEvent> AnaEventSet;


/**
 * A data container to hold data from a single event delivered by the RHIC
 * polarimeters. This class inherits from the ROOT's TObject so, the objects of
 * this class can be naturally saved in a ROOT file.
 */
class AnaEvent : public TObject
{
public:

   AnaEventId     fEventId;    ///< Unique event number
   ChannelDataMap fChannels;   ///< An std::map container with data collected from all polarimeter channels

public:

   AnaEvent();
   ~AnaEvent();

   float GetEnergy(EventConfig *ec, ChannelDataPair &ch);
   float GetTime(EventConfig *ec, ChannelDataPair &ch);
   virtual void Print(const Option_t* opt="") const;

   ClassDef(AnaEvent, 1)
};

#endif
