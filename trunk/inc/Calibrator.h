/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef Calibrator_h
#define Calibrator_h

#include <map>
#include <set>
#include <vector>

#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TH1.h"
#include "TH1D.h"
#include "TRandom.h"
#include "TObject.h"

#include "DrawObjContainer.h"
#include "ChannelCalib.h"


/** */
class Calibrator : public TObject
{
public:

   TRandom         *fRandom;
   ChannelCalibMap  fChannelCalibs;

public:

   Calibrator();
   Calibrator(TRandom *random);
   ~Calibrator();

   virtual ChannelCalib*  GetAverage();
   virtual void           CopyAlphaCoefs(Calibrator &calibrator);
   virtual void           Calibrate(DrawObjContainer *c);
   virtual void           CalibrateFast(DrawObjContainer *c);
   virtual Float_t        GetDLWidth(UShort_t chId) const;
   virtual Float_t        GetDLWidthErr(UShort_t chId) const;
   virtual Float_t        GetT0Coef(UShort_t chId) const;
   virtual Float_t        GetT0CoefErr(UShort_t chId) const;
   virtual Float_t        GetBananaChi2Ndf(UShort_t chId) const;
   virtual UInt_t         GetFitStatus(UShort_t chId) const;
   virtual Float_t        GetEnergyA(UShort_t adc, UShort_t chId) const;
   virtual Float_t        GetKinEnergyA(UShort_t adc, UShort_t chId) const;
   virtual Float_t        GetTime(UShort_t tdc) const;
   virtual Float_t        GetTimeOfFlight(UShort_t tdc, UShort_t chId) const;
   virtual TFitResultPtr  CalibrateOld(TH1 *h, TH1D *hMeanTime, UShort_t chId=0, Bool_t wideLimits=false);
   virtual void           Calibrate(TH1 *h, TH1D *hMeanTime, UShort_t chId=0, Bool_t wideLimits=false);
   void                   Print(const Option_t* opt="") const;
   virtual void           PrintAsPhp(FILE *f=stdout) const;
   void                   PrintAsConfig(FILE *f=stdout) const;

   ClassDef(Calibrator, 1)
};

#endif
