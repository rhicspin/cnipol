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
#include "MeasInfo.h"
#include "DbEntry.h"
#include "AnaMeasResult.h"
//#include "MseMeasInfo.h"

#include "Calibrator.h"

class MseMeasInfoX;

/** */
class EventConfig : public TObject
{
public:
   
   TRandom       *fRandom;
   MeasInfo      *fMeasInfo;
   AnaInfo       *fAnaInfo;
   //DbEntry       *fDbEntry; //!
   Calibrator    *fCalibrator;
   AnaMeasResult *fAnaMeasResult;
   MseMeasInfoX  *fMseMeasInfoX;

public:

   EventConfig();
   ~EventConfig();

   MeasInfo*      GetMeasInfo();
   AnaInfo*       GetAnaInfo();
   Calibrator*    GetCalibrator();
   AnaMeasResult* GetAnaMeasResult();
   MseMeasInfoX*  GetMseMeasInfoX();

   //virtual void Print(const Option_t* opt="") const;
   void  Print(const Option_t* opt="") const;
   void  PrintAsPhp(FILE *f=stdout) const;
   void  PrintAsConfig(FILE *f=stdout) const;
   float ConvertToEnergy(UShort_t adc, UShort_t chId);
   //void  Streamer(TBuffer &R__b);
   std::string GetSignature();

   ClassDef(EventConfig, 5)
};

//TBuffer & operator<<(TBuffer &buf, EventConfig *&rec);
//TBuffer & operator>>(TBuffer &buf, EventConfig *&rec);

#endif
