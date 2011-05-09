/**
 *
 * 24 Dec, 2010 - Dmitri Smirnov
 *    - Created class
 *
 */

#ifndef RunInfo_h
#define RunInfo_h

#include <vector>
#include <set>
#include <string>

#include "TBuffer.h"
#include "TColor.h"
#include "TPRegexp.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TString.h"

#include "rpoldata.h"

#include "AsymHeader.h"
#include "RunConfig.h"
#include "DbEntry.h"

class MseRunInfoX;


/** */
class RunInfo
{
protected:

   double       fBeamEnergy;
   Short_t      fExpectedGlobalTdcOffset;
   Float_t      fExpectedGlobalTimeOffset;

public:

   int          Run;
   double       RUNID;
   std::string  fRunName;
   time_t       StartTime;
   time_t       StopTime;
   float        RunTime;
   int          fDataFormatVersion;
   std::string  fAsymVersion;
   EMeasType    fMeasType;
   float        GoodEventRate;
   float        EvntRate;
   float        ReadRate;
   float        WcmAve;
   float        WcmSum;
   Short_t      fPolId;
   UShort_t     fPolBeam;
   UShort_t     fPolStream;
   int          PolarimetryID;
   int          MaxRevolution;
   char         fTargetOrient;
   char         targetID;
   char         TgtOperation[16];
   int          ActiveDetector[N_DETECTORS];
   int          ActiveStrip[N_CHANNELS];
   int          NActiveStrip;
   int          NDisableStrip;
   int          fDisabledChannels[N_CHANNELS];
   //std::vector<UShort_t>   fDisabledChannels;
   int          NFilledBunch;
   int          NActiveBunch;
   int          NDisableBunch;
   int          DisableBunch[N_BUNCHES];
	float        fProtoCutSlope;
	float        fProtoCutOffset;

public:

   RunInfo();
   ~RunInfo();

   std::string GetAlphaCalibFileName() const;
   std::string GetDlCalibFileName() const;
   void        Streamer(TBuffer &buf);
   void        Print(const Option_t* opt="") const;
   void        PrintAsPhp(FILE *f=stdout) const;
   void        PrintConfig();
   short       GetPolarimeterId();
   short       GetPolarimeterId(short beamId, short streamId);
   void        GetBeamIdStreamId(Short_t polId, UShort_t &beamId, UShort_t &streamId);
   void        Update(DbEntry &rundb);
   void        Update(MseRunInfoX& run);
   void        ConfigureActiveStrip(int mask);
   void        SetBeamEnergy(Float_t beamEnergy);
   Float_t     GetBeamEnergy();
   void        SetPolarimetrIdRhicBeam(const char* RunID);
   Float_t     GetExpectedGlobalTimeOffset();
   Short_t     GetExpectedGlobalTdcOffset();
};

TBuffer & operator<<(TBuffer &buf, RunInfo *&rec);
TBuffer & operator>>(TBuffer &buf, RunInfo *&rec);

#endif
