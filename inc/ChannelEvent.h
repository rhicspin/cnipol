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

class ChannelEvent;


typedef std::vector<ChannelEvent> ChannelEventVec;
typedef std::map<ChannelEventId, ChannelEvent, ChannelEventId> ChannelEventMap;
typedef std::set<ChannelEvent, ChannelEvent> ChannelEventSet;


/** */
class ChannelEvent : public TObject
{
public:

   EventConfig    *fEventConfig; //!
	ChannelEventId  fEventId;
   ChannelData     fChannel;

public:

   ChannelEvent();
   ~ChannelEvent();

   UChar_t  GetDetectorId()   const { return (UShort_t) (fEventId.fChannelId / NSTRIP_PER_DETECTOR) + 1; }
   UInt_t   GetRevolutionId() const { return fEventId.fRevolutionId; }
   UChar_t  GetChannelId()    const { return fEventId.fChannelId + 1; }
   UChar_t  GetBunchId()      const { return fEventId.fBunchId; }
   UShort_t GetDelimiterId()  const { return fEventId.fDelimiterId; }
   UChar_t  GetAmpltd()       const { return fChannel.fAmpltd; }
   UChar_t  GetIntgrl()       const { return fChannel.fIntgrl; }
   UChar_t  GetTdc()          const { return fChannel.fTdc; }
   UChar_t  GetTdcAMax()      const { return fChannel.fTdcAMax; }
   Float_t  GetEnergyA() const;
   Float_t  GetKinEnergyA();
   Float_t  GetKinEnergyAEDepend();
   Float_t  GetKinEnergyAEDependAverage();
   Float_t  GetKinEnergyAEstimate();
   Float_t  GetKinEnergyADLCorrEstimate();
   Float_t  GetKinEnergyAEstimateEDepend();
   Float_t  GetFunnyEnergyA();
   Float_t  GetEnergyI();
   //Float_t GetTotalEnergyI();
   Float_t  GetTime() const;
   Float_t  GetTime2() const;
   Float_t  GetTimeOfFlight();
   Float_t  GetTimeOfFlight2();
   Float_t  GetTimeOfFlightEstimate();
   Float_t  GetCarbonMass();
   Float_t  GetCarbonMassEstimate();
   Float_t  GetMandelstamT();
   Float_t  GetTdcAdcTimeDiff();

   //virtual void Print(const Option_t* opt="") const;
   void Print(const Option_t* opt="") const;
   bool operator()(const ChannelEvent &ch1, const ChannelEvent &ch2);
   Bool_t PassCutRawAlpha();
   Bool_t PassCutSiliconChannel();
   Bool_t PassCutDepEnergyTime();
   Bool_t PassCutKinEnergyAEDepend();
   Bool_t PassCutKinEnergyAEDependAverage();
   Bool_t PassCutKinEnergyADLCorrEstimate();
   Bool_t PassCutCarbonMass();
   Bool_t PassCutCarbonMassEstimate();
   Bool_t PassCutPulser();
   Bool_t PassCutNoise();
   Bool_t PassCutEnabledChannel();
   Bool_t PassCutTargetChannel();
   Bool_t PassCutPmtChannel();
   Bool_t PassCutHamaChannel();
   Bool_t PassCutPmtNoise();
   Bool_t PassCutEmptyBunch() const;

   ClassDef(ChannelEvent, 1)
};

#endif
