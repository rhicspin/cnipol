/**
 *
 * 24 Dec, 2010 - Dmitri Smirnov
 *    - Created class
 *
 */

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

#include "AsymHeader.h"
#include "BeamBunch.h"
#include "RunConfig.h"
#include "DbEntry.h"

class MseMeasInfoX;
class MseRunPeriodX;


/** */
class MeasInfo : public TObject
{
protected:

   Double_t   fBeamEnergy;
   Short_t    fExpectedGlobalTdcOffset;
   Float_t    fExpectedGlobalTimeOffset;

   std::string                 fRunName;

public:

   int                         Run;
   double                      RUNID;
   time_t                      fStartTime;
   time_t                      fStopTime;
   float                       fRunTime;
   int                         fDataFormatVersion;
   std::string                 fAsymVersion;
   EMeasType                   fMeasType;
   UInt_t                      fNEventsProcessed;  // number of events processed from raw data file
   UInt_t                      fNEventsTotal;      // number of total events in raw data file
   float                       GoodEventRate;
   float                       EvntRate;
   float                       ReadRate;
	std::map<UShort_t, Float_t> fWallCurMon;
   Float_t                     fWallCurMonAve;
   Float_t                     fWallCurMonSum;
   Short_t                     fPolId;
   UShort_t                    fPolBeam;
   UShort_t                    fPolStream;
   int                         PolarimetryID;
   int                         MaxRevolution;
   char                        fTargetOrient;
   char                        fTargetId;
   int                         ActiveDetector[N_DETECTORS];
   int                         ActiveStrip[N_SILICON_CHANNELS];
   int                         NActiveStrip;
   int                         NDisableStrip;
   int                         fDisabledChannels[N_SILICON_CHANNELS];
   std::vector<UShort_t>       fDisabledChannelsVec; // should rename to fDisabledChannels when get rid of the plain array
   ChannelSet                  fSiliconChannels;
   ChannelSet                  fActiveSiliconChannels;
   BeamBunchMap                fBeamBunches;
   Int_t                       NDisableBunch;
   Int_t                       DisableBunch[N_BUNCHES];
	Float_t                     fProtoCutSlope;
	Float_t                     fProtoCutOffset;
	Float_t                     fProtoCutAdcMin;
	Float_t                     fProtoCutAdcMax;
	Float_t                     fProtoCutTdcMin;
	Float_t                     fProtoCutTdcMax;
	Float_t                     fPulserCutAdcMin;
	Float_t                     fPulserCutAdcMax;
	Float_t                     fPulserCutTdcMin;
	Float_t                     fPulserCutTdcMax;

public:

   MeasInfo();
   ~MeasInfo();

   void            Print(const Option_t* opt="") const;
   void            PrintAsPhp(FILE *f=stdout) const;
   void            PrintConfig();
   void            PrintBunchPatterns() const;
   void            SetRunName(std::string runName);
   std::string     GetRunName() const;
   Short_t         GetPolarimeterId();
   Short_t         GetPolarimeterId(short beamId, short streamId);
   UInt_t          GetFillId();
   void            Update(DbEntry &rundb);
   void            Update(MseMeasInfoX& run);
   void            Update(MseRunPeriodX& runPeriod);
   void            ConfigureActiveStrip(int mask);
   Float_t         GetBeamEnergy() const;
   Float_t         GetExpectedGlobalTimeOffset() const;
   Short_t         GetExpectedGlobalTdcOffset() const;
   EMeasType       GetMeasType() const;
   std::string     GetAlphaCalibFileName() const;
   std::string     GetDlCalibFileName() const;
   void            SetBeamEnergy(Float_t beamEnergy);
   void            SetMeasType(EMeasType measType);
   void            SetPolarimetrIdRhicBeam(const char* RunID);
	void            DisableChannel(UShort_t chId);
   void            DisableChannels(std::bitset<N_DETECTORS> &disabled_det);
	void            EnableChannel(UShort_t chId);
   void            EnableChannels(std::bitset<N_DETECTORS> &disabled_det);
	Bool_t          IsDisabledChannel(UShort_t chId);
	Bool_t          IsSiliconChannel(UShort_t chId);
	Bool_t          IsHamaChannel(UShort_t chId);
	Bool_t          IsPmtChannel(UShort_t chId);
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

   static void           GetBeamIdStreamId(Short_t polId, UShort_t &beamId, UShort_t &streamId);
   static EPolarimeterId ExtractPolarimeterId(std::string runName);

   ClassDef(MeasInfo, 3)
};

#endif
