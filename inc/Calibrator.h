/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef Calibrator_h
#define Calibrator_h

#include <map>
#include <set>
#include <vector>

#include "TObject.h"

#include "ChannelCalib.h"
#include "DrawObjContainer.h"


/**
 *
 */
class Calibrator : public TObject
{
public:

   ChannelCalibMap fChannelCalibs;

public:

   Calibrator();
   ~Calibrator();

   virtual void Calibrate(DrawObjContainer *c);
   virtual void Print(const Option_t* opt="") const;
   virtual void PrintAsPhp(FILE *f=stdout) const;

   ClassDef(Calibrator, 1)
};

#endif
