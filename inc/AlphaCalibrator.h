/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef AlphaCalibrator_h
#define AlphaCalibrator_h

#include <map>
#include <set>
#include <vector>

#include "TObject.h"

#include "Calibrator.h"


/**
 *
 */
class AlphaCalibrator : public Calibrator
{
public:

public:

   AlphaCalibrator();
   ~AlphaCalibrator();

   virtual void Print(const Option_t* opt="") const;

   ClassDef(AlphaCalibrator, 1)
};

#endif
