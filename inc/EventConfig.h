/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef EventConfig_h
#define EventConfig_h

#include <stdio.h>
#include <vector>

#include "TObject.h"
#include "TBuffer.h"
#include "TRandom.h"

#include "rpoldata.h"
#include "rhicpol.h"

#include "AsymGlobals.h"
#include "AnaInfo.h"
#include "RunInfo.h"
#include "DbEntry.h"
#include "AnaMeasResult.h"
//#include "MseRunInfo.h"

#include "Calibrator.h"

class MseRunInfoX;

/** */
class EventConfig : public TObject
{
public:
   
   TRandom                 *fRandom;
   //TRecordConfigRhicStruct *fConfigInfo;
   RunInfo                 *fRunInfo;
   AnaInfo                 *fAnaInfo;
   //DbEntry                 *fDbEntry; //!
   Calibrator              *fCalibrator;
   AnaMeasResult               *fAnaMeasResult;
   MseRunInfoX             *fMseRunInfoX;

public:

   EventConfig();
   ~EventConfig();

   RunInfo*     GetRunInfo();
   AnaInfo*     GetAnaInfo();
   Calibrator*  GetCalibrator();
   AnaMeasResult*   GetAnaMeasResult();
   MseRunInfoX* GetMseRunInfoX();

   //virtual void Print(const Option_t* opt="") const;
   void  Print(const Option_t* opt="") const;
   void  PrintAsPhp(FILE *f=stdout) const;
   void  PrintAsConfig(FILE *f=stdout) const;
   float ConvertToEnergy(UShort_t adc, UShort_t chId);
   //void  Streamer(TBuffer &R__b);
   std::string GetSignature();

   ClassDef(EventConfig, 3)
};

TBuffer & operator<<(TBuffer &buf, EventConfig *&rec);
TBuffer & operator>>(TBuffer &buf, EventConfig *&rec);

#endif
