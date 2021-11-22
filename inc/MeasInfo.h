#ifndef MeasInfo_h
#define MeasInfo_h

#include <bitset>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "TColor.h"
#include "TPRegexp.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TObject.h"
#include "TString.h"

#include "globals.h"
#include "rpoldata.h"

#include "AsymAnaInfo.h"
#include "AsymHeader.h"
#include "AsymOnline.h"
#include "BeamBunch.h"
#include "RunConfig.h"

class MseMeasInfoX;
class MseRunPeriodX;
class RunPeriod;

class MeasInfo;

typedef std::map<std::string, MeasInfo>  MeasInfoMap;
typedef MeasInfoMap::iterator            MeasInfoMapIter;
typedef MeasInfoMap::const_iterator      MeasInfoMapConstIter;


/** */
class MeasInfo : public TObject
{
protected:

   Float_t             fBeamEnergy;
   RecordMachineParams_t fMachineParams;
   Bool_t              fHasMachineParamsInRawData;
   RecordTargetParams_t  fTargetParams;
   Short_t             fExpectedGlobalTdcOffset;
   Float_t             fExpectedGlobalTimeOffset;
   std::string         fRunName;
   UShort_t            fAlphaSourceCount;
   Float_t             fProtoCutSlope;
   Float_t             fProtoCutOffset;
   UShort_t            fProtoCutWidth;
   UShort_t            fProtoCutAdcMin;
   UShort_t            fProtoCutAdcMax;
   UShort_t            fProtoCutTdcMin;
   UShort_t            fProtoCutTdcMax;
   UShort_t            fPulserCutAdcMin;
   UShort_t            fPulserCutAdcMax;
   UShort_t            fPulserCutTdcMin;
   UShort_t            fPulserCutTdcMax;
   std::vector<UInt_t> fSpinFlipperMarkers; // revolution id's corresponding to sync pulses for spin flipper studies
   Double_t            fSpinFlipperPhase;   //
   UInt_t              fFirstRevolution;    //
   UInt_t              fLastRevolution;     //
   uint32_t            fAcDipolePeriod;     //! AC dipole magnet period in revolutions

public:

   int                         fRunId;
   double                      RUNID;
   time_t                      fStartTime;
   time_t                      fStopTime;
   Float_t                     fRunTime;
   int                         fDataFormatVersion;
   std::string                 fAsymVersion;       // XXX should go... moved to AnaInfo
   EMeasType                   fMeasType;
   UInt_t                      fNEventsProcessed;  // number of events processed from raw data file
   UInt_t                      fNEventsTotal;      // number of total events in raw data file
   UInt_t                      fNEventsSilicon;
   Float_t                     GoodEventRate;
   Float_t                     EvntRate;
   Float_t                     ReadRate;
   std::map<UShort_t, Float_t> fWallCurMon;
   Float_t                     fWallCurMonAve;
   Float_t                     fWallCurMonSum;
   Short_t                     fPolId;
   UShort_t                    fPolBeam;
   UShort_t                    fPolStream;
   int                         PolarimetryID;
   int                         MaxRevolution;           // XXX get rid of it when fFirstRevolution and fLastRevolution are enabled
   char                        fTargetOrient;
   char                        fTargetId;
   UInt_t                      fTargetVelocity;         // from CDEV
   UInt_t                      fProfileStartPosition;   // from CDEV
   UInt_t                      fProfileEndPosition;     // from CDEV
   ChannelSet                  fSiliconChannels;        // a list of channels for silicon detectors. Normaly, just everything from 1 to 72
   ChannelSet                  fDisabledChannels;       // a list of disabled channels. not just silicon channels
   BeamBunchMap                fBeamBunches;
   Double_t                    fGainSlope[N_DETECTORS];

public:

   MeasInfo();
   ~MeasInfo();

   void            Print(const Option_t* opt="") const;
   void            PrintAsPhp(FILE *f=stdout) const;
   void            PrintConfig();
   void            PrintBunchPatterns() const;
   void            SetRunName(std::string runName);
   std::string     GetRunName() const;

   Bool_t          HasMachineParamsInRawData()  const { return fHasMachineParamsInRawData; }
   UShort_t        GetAlphaSourceCount()        const { return fAlphaSourceCount; }
   Float_t         GetProtoCutSlope()           const { return fProtoCutSlope;    }
   Float_t         GetProtoCutOffset()          const { return fProtoCutOffset;   }
   UShort_t        GetProtoCutWidth()           const { return fProtoCutWidth;    }
   UShort_t        GetProtoCutAdcMin()          const { return fProtoCutAdcMin;   }
   UShort_t        GetProtoCutAdcMax()          const { return fProtoCutAdcMax;   }
   UShort_t        GetProtoCutTdcMin()          const { return fProtoCutTdcMin;   }
   UShort_t        GetProtoCutTdcMax()          const { return fProtoCutTdcMax;   }
   UShort_t        GetPulserCutAdcMin()         const { return fPulserCutAdcMin;  }
   UShort_t        GetPulserCutAdcMax()         const { return fPulserCutAdcMax;  }
   UShort_t        GetPulserCutTdcMin()         const { return fPulserCutTdcMin;  }
   UShort_t        GetPulserCutTdcMax()         const { return fPulserCutTdcMax;  }
   const std::vector<UInt_t>& GetSpinFlipperMarkers() const { return fSpinFlipperMarkers;  }
   Double_t        GetSpinFlipperPhase()        const { return fSpinFlipperPhase;  }
   UInt_t          GetFirstRevolution()         const { return fFirstRevolution;  }
   UInt_t          GetLastRevolution()          const { return fLastRevolution;  }
   uint32_t        GetAcDipolePeriod()          const { return fAcDipolePeriod;  }

   void            SetAlphaSourceCount(UShort_t alphaSourceCount) { fAlphaSourceCount = alphaSourceCount; }
   void            SetProtoCutSlope  (Float_t  protoCutSlope )    { fProtoCutSlope    = protoCutSlope;    }
   void            SetProtoCutOffset (Float_t  protoCutOffset)    { fProtoCutOffset   = protoCutOffset;   }
   void            SetProtoCutAdcMin (UShort_t protoCutAdcMin)    { fProtoCutAdcMin   = protoCutAdcMin;   }
   void            SetProtoCutAdcMax (UShort_t protoCutAdcMax)    { fProtoCutAdcMax   = protoCutAdcMax;   }
   void            SetProtoCutTdcMin (UShort_t protoCutTdcMin)    { fProtoCutTdcMin   = protoCutTdcMin;   }
   void            SetProtoCutTdcMax (UShort_t protoCutTdcMax)    { fProtoCutTdcMax   = protoCutTdcMax;   }
   void            SetPulserCutAdcMin(UShort_t pulserCutAdcMin)   { fPulserCutAdcMin  = pulserCutAdcMin;  }
   void            SetPulserCutAdcMax(UShort_t pulserCutAdcMax)   { fPulserCutAdcMax  = pulserCutAdcMax;  }
   void            SetPulserCutTdcMin(UShort_t pulserCutTdcMin)   { fPulserCutTdcMin  = pulserCutTdcMin;  }
   void            SetPulserCutTdcMax(UShort_t pulserCutTdcMax)   { fPulserCutTdcMax  = pulserCutTdcMax;  }
   void            SetAcDipolePeriod (uint32_t acDipolePeriod)    { fAcDipolePeriod   = acDipolePeriod;   }

   void            AddSpinFlipperMarker(UInt_t markerRevId) { fSpinFlipperMarkers.push_back(markerRevId); }
   Double_t        CalcSpinFlipperPhase();

   time_t          GetStartTime() const { return fStartTime; }
   void            SetStartTime(time_t time) { fStartTime = time; }
   Short_t         GetPolarimeterId();
   Short_t         GetPolarimeterId(short beamId, short streamId);
   UInt_t          GetFillId();
   bool            IsRunYear(int year) const;
   void            Update(const MseMeasInfoX& run);
   void            Update(const RunPeriod& runPeriod);
   void            Update(const AsymAnaInfo& anaInfo);
   void            UpdateRevolutions(UInt_t revId);
   Float_t         GetBeamEnergyReal() const;
   EBeamEnergy     GetBeamEnergy() const;
   Float_t         GetExpectedGlobalTimeOffset() const;
   Short_t         GetExpectedGlobalTdcOffset() const;
   Bool_t          IsStarRotatorOn() const;
   EMeasType       GetMeasType() const;
   std::string     GetAlphaCalibFileName() const;
   void            SetBeamEnergy(Float_t beamEnergy);
   void            SetMachineParams(const RecordMachineParams &rec);
   void            SetTargetParams(const RecordTargetParams &rec);
   void            SetMeasType(EMeasType measType);
   void            SetPolarimetrIdRhicBeam(const char* RunID);
   void            DisableChannel(UShort_t chId);
   void            DisableChannels(std::bitset<N_DETECTORS> &disabled_det);
   void            EnableChannel(UShort_t chId);
   Bool_t          IsDisabledChannel(UShort_t chId);
   Bool_t          IsSiliconChannel(UShort_t chId);
   Bool_t          IsSpinTuneChannel(UShort_t chId);
   UShort_t        GetNumActiveSiChannels() const;
   UShort_t        GetNumDisabledChannels() const;
   Bool_t          IsPmtChannel(UShort_t chId) const;
   Bool_t          IsStepperChannel(UShort_t chId) const;
   Bool_t          IsSpinFlipperMarkerChannel(UShort_t chId) const;
   BeamBunchMap    GetBunches() const;
   BeamBunchMap    GetFilledBunches() const;
   BeamBunchMap    GetEmptyBunches() const;
   UShort_t        GetNumFilledBunches() const;
   UShort_t        GetNumEmptyBunches() const;
   Bool_t          IsEmptyBunch(UShort_t bid) const;
   ESpinState      GetBunchSpin(UShort_t bid) const;
   ERingId         GetRingId() const;
   ETargetOrient   GetTargetOrient() const;
   UShort_t        GetTargetId() const;
   std::vector<double> GetBiasCurrents() const;

   static void           GetBeamIdStreamId(Short_t polId, UShort_t &beamId, UShort_t &streamId);
   static EPolarimeterId ExtractPolarimeterId(std::string runName);

   ClassDef(MeasInfo, 6)
};


inline bool operator==(const MeasInfo& lhs, const MeasInfo& rhs) { return lhs.GetStartTime() == rhs.GetStartTime(); }
inline bool operator!=(const MeasInfo& lhs, const MeasInfo& rhs) { return !operator==(lhs,rhs); }
inline bool operator< (const MeasInfo& lhs, const MeasInfo& rhs) { return lhs.GetStartTime() < rhs.GetStartTime(); }
inline bool operator> (const MeasInfo& lhs, const MeasInfo& rhs) { return  operator< (rhs,lhs); }
inline bool operator<=(const MeasInfo& lhs, const MeasInfo& rhs) { return !operator> (lhs,rhs); }
inline bool operator>=(const MeasInfo& lhs, const MeasInfo& rhs) { return !operator< (lhs,rhs); }


struct CompareMeasInfo
{
   bool operator()(const MeasInfo& lhs, const MeasInfo& rhs) const { return lhs < rhs; }
};

struct CompareMeasInfoPtr
{
   bool operator()(const MeasInfo* lhs, const MeasInfo* rhs) const { return (*lhs) < (*rhs); }
};


typedef std::set<MeasInfo, CompareMeasInfo>       MeasInfoSet;
typedef MeasInfoSet::iterator                     MeasInfoSetIter;
typedef MeasInfoSet::const_iterator               MeasInfoSetConstIter;

typedef std::set<MeasInfo*, CompareMeasInfoPtr>   MeasInfoPtrSet;
typedef MeasInfoPtrSet::iterator                  MeasInfoPtrSetIter;
typedef MeasInfoPtrSet::const_iterator            MeasInfoPtrSetConstIter;

typedef std::map<std::string, MeasInfo>           MeasInfoMap;
typedef MeasInfoMap::iterator                     MeasInfoMapIter;
typedef MeasInfoMap::const_iterator               MeasInfoMapConstIter;

typedef std::map<EPolarimeterId, MeasInfoPtrSet>  PolId2MeasInfoPtrSetMap;
typedef PolId2MeasInfoPtrSetMap::iterator         PolId2MeasInfoPtrSetMapIter;
typedef PolId2MeasInfoPtrSetMap::const_iterator   PolId2MeasInfoPtrSetMapConstIter;

#endif
