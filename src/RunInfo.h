/**
 *
 * 24 Dec, 2010 - Dmitri Smirnov
 *    - Created class
 *
 */

#ifndef RunInfo_h
#define RunInfo_h

#include <string>
#include <vector>

#include "TBuffer.h"
#include "TPRegexp.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TString.h"

#include "rpoldata.h"

#include "AsymHeader.h"
#include "DbEntry.h"

class MseRunInfoX;


/** */
class RunInfo
{
public:

   // Different measurement types
   enum MeasType {MEASTYPE_UNKNOWN = 0x00,
                  MEASTYPE_ALPHA   = 0x01,
                  MEASTYPE_SWEEP   = 0x02,
                  MEASTYPE_FIXED   = 0x04,
                  MEASTYPE_RAMP    = 0x08,
                  MEASTYPE_EMIT    = 0x10};

   int          Run;
   double       RUNID;
   std::string  fRunName;
   time_t       StartTime;
   time_t       StopTime;
   float        RunTime;
   int          fDataFormatVersion;
   std::string  fAsymVersion;
   MeasType     fMeasType;
   float        GoodEventRate;
   float        EvntRate;
   float        ReadRate;
   float        WcmAve;
   float        WcmSum;
   double       BeamEnergy;
   Short_t      fPolId;
   UShort_t     fPolBeam;
   UShort_t     fPolStream;
   int          PolarimetryID;
   int          MaxRevolution;
   char         fTargetOrient;
   char         targetID;
   char         TgtOperation[16];
   int          ActiveDetector[NDETECTOR];
   int          ActiveStrip[NSTRIP];
   int          NActiveStrip;
   int          NDisableStrip;
   int          fDisabledChannels[NSTRIP];
   //std::vector<UShort_t>   fDisabledChannels;
   int          NFilledBunch;
   int          NActiveBunch;
   int          NDisableBunch;
   int          DisableBunch[NBUNCH];

public:

   RunInfo();
   ~RunInfo();

   std::string GetAlphaCalibFileName() const;
   std::string GetDlCalibFileName() const;
   void  Streamer(TBuffer &buf);
   void  Print(const Option_t* opt="") const;
   void  PrintAsPhp(FILE *f=stdout) const;
   void  PrintConfig(recordConfigRhicStruct *cfginfo);
   short GetPolarimeterId();
   short GetPolarimeterId(short beamId, short streamId);
   void  GetBeamIdStreamId(Short_t polId, UShort_t &beamId, UShort_t &streamId);
   void  Update(DbEntry &rundb);
   void  Update(MseRunInfoX& run);
   void  ConfigureActiveStrip(int mask);
};

TBuffer & operator<<(TBuffer &buf, RunInfo *&rec);
TBuffer & operator>>(TBuffer &buf, RunInfo *&rec);

#endif
