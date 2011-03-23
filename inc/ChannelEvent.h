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

#include "AsymGlobals.h"

#include "ChannelData.h"
#include "ChannelEventId.h"
#include "EventConfig.h"

//class EventConfig;
//class ChannelData;
//class Calibrator;
class ChannelEvent;


typedef std::vector<ChannelEvent> ChannelEventVec;
typedef std::map<ChannelEventId, ChannelEvent, ChannelEventId> ChannelEventMap;
typedef std::set<ChannelEvent, ChannelEvent> ChannelEventSet;


/** */
class ChannelEvent : public TObject
{
public:

   EventConfig    *fEventConfig;
	ChannelEventId  fEventId;
   ChannelData     fChannel;

public:

   ChannelEvent();
   ~ChannelEvent();

   UChar_t  GetDetectorId()   { return (UShort_t) (fEventId.fChannelId / NSTRIP_PER_DETECTOR) + 1; }
   UInt_t   GetRevolutionId() { return fEventId.fRevolutionId; }
   UChar_t  GetChannelId()    { return fEventId.fChannelId + 1; }
   UChar_t  GetBunchId()      { return fEventId.fBunchId; }
   UShort_t GetDelimiterId()  { return fEventId.fDelimiterId; }
   UChar_t  GetAmpltd()       { return fChannel.fAmpltd; }
   UChar_t  GetIntgrl()       { return fChannel.fIntgrl; }
   UChar_t  GetTdc()          { return fChannel.fTdc; }
   UChar_t  GetTdcAMax()      { return fChannel.fTdcAMax; }
   Float_t  GetEnergyA();
   Float_t  GetKinEnergyA();
   Float_t  GetKinEnergyAEDepend();
   Float_t  GetKinEnergyAEstimate();
   Float_t  GetKinEnergyADLCorrEstimate();
   Float_t  GetKinEnergyAEstimateEDepend();
   Float_t  GetFunnyEnergyA();
   Float_t  GetEnergyI();
   //Float_t GetTotalEnergyI();
   Float_t  GetTime();
   Float_t  GetTimeOfFlight();
   Float_t  GetTimeOfFlightEstimate();
   Float_t  GetCarbonMass();
   Float_t  GetCarbonMassEstimate();
   Float_t  GetMandelstamT();

   //virtual void Print(const Option_t* opt="") const;
   void Print(const Option_t* opt="") const;
   bool operator()(const ChannelEvent &ch1, const ChannelEvent &ch2);
   Bool_t PassCutRawAlpha();
   Bool_t PassCutDetectorChannel();
   Bool_t PassCutDepEnergyTime();
   Bool_t PassCutKinEnergyADLCorrEstimate();
   Bool_t PassQACutCarbonMass();
   Bool_t PassCutPulser();
   Bool_t PassCutNoise();
   Bool_t PassCutEnabledChannel();
   Bool_t PassCutTargetChannel();

   ClassDef(ChannelEvent, 1)
};

#endif
