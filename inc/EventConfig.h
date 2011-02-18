/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef EventConfig_h
#define EventConfig_h

#include <stdio.h>
#include <vector>

#include "TObject.h"
#include "TRandom.h"

#include "rpoldata.h"
#include "rhicpol.h"

//#include "Asym.h"
#include "AnaInfo.h"
#include "AsymRunDB.h"
#include "RunInfo.h"
#include "AnaResult.h"

#include "Calibrator.h"


/** */
class EventConfig : public TObject
{
public:
   
   TRandom                 *fRandom;
   TRecordConfigRhicStruct *fConfigInfo;
   TStructRunInfo          *fRunInfo;
   TDatprocStruct          *fDatproc;
   TStructRunDB            *fRunDB;
   Calibrator              *fCalibrator;
   StructAnalysis          *fAnaResult;

public:

   EventConfig();
   ~EventConfig();

   //virtual void Print(const Option_t* opt="") const;
   void  Print(const Option_t* opt="") const;
   void  PrintAsPhp(FILE *f=stdout) const;
   void  PrintAsConfig(FILE *f=stdout) const;
   float ConvertToEnergy(UShort_t adc, UShort_t chId);

   ClassDef(EventConfig, 2)
};

#endif
