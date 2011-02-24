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

#include "AsymHeader.h"
#include "AsymRunDB.h"


/** */
class TStructRunInfo
{
public:

   int          Run;
   double       RUNID;
   std::string  runName;
   time_t       StartTime;
   time_t       StopTime;
   float        RunTime;
   int          fDataFormatVersion;
   std::string  fAsymVersion;
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
   char         target;
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

   TStructRunInfo();
   ~TStructRunInfo();

   void  Streamer(TBuffer &buf);
   void  Print(const Option_t* opt="") const;
   void  PrintAsPhp(FILE *f=stdout) const;
   std::string GetAlphaCalibFileName() const;
   std::string GetDlCalibFileName() const;
   short GetPolarimeterId();
   short GetPolarimeterId(short beamId, short streamId);
   void  GetBeamIdStreamId(Short_t polId, UShort_t &beamId, UShort_t &streamId);
   void  Update(TStructRunDB &rundb);
};

TBuffer & operator<<(TBuffer &buf, TStructRunInfo *&rec);
TBuffer & operator>>(TBuffer &buf, TStructRunInfo *&rec);

#endif
