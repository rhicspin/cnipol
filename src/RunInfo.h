/**
 *
 * 24 Dec, 2010 - Dmitri Smirnov
 *    - Created class
 *
 */

#ifndef RunInfo_h
#define RunInfo_h

#include <string>

#include "TBuffer.h"
#include "TString.h"

#include "AsymHeader.h"


/** */
class TStructRunInfo
{
public:

   int          Run;
   double       RUNID;
   std::string  runName;
   int          StartTime;
   int          StopTime;
   float        RunTime;
   float        GoodEventRate;
   float        EvntRate;
   float        ReadRate;
   float        WcmAve;
   float        WcmSum;
   double       BeamEnergy;
   int          RHICBeam;
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
   int          DisableStrip[NSTRIP];
   int          NFilledBunch;
   int          NActiveBunch;
   int          NDisableBunch;
   int          DisableBunch[NBUNCH];

public:

   TStructRunInfo();
   ~TStructRunInfo();

   void Streamer(TBuffer &buf);
   void PrintAsPhp(FILE *f=stdout) const;
};

TBuffer & operator<<(TBuffer &buf, TStructRunInfo *&rec);
TBuffer & operator>>(TBuffer &buf, TStructRunInfo *&rec);

#endif
