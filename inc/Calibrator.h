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
#include "TObject.h"

#include "DrawObjContainer.h"
#include "ChannelCalib.h"


/** */
class Calibrator : public TObject
{
public:

   ChannelCalibMap fChannelCalibs;

public:

   Calibrator();
   ~Calibrator();

   virtual void Calibrate(DrawObjContainer *c);
   virtual TFitResultPtr Calibrate(TH1 *h, TH1D *hMeanTime);
   void Print(const Option_t* opt="") const;
   void PrintAsPhp(FILE *f=stdout) const;
   void PrintAsConfig(FILE *f=stdout) const;

   ClassDef(Calibrator, 1)
};

#endif
