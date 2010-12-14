/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef AlphaCalibrator_h
#define AlphaCalibrator_h

#include <map>
#include <set>
#include <vector>

#include "TF1.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TObject.h"

#include "Calibrator.h"
#include "DrawObjContainer.h"


/** */
class AlphaCalibrator : public Calibrator
{
public:

public:

   AlphaCalibrator();
   ~AlphaCalibrator();

   virtual void Calibrate(DrawObjContainer *c);
   //TFitResultPtr Calibrate(TH1 *h, TH1D *hMeanTime);
   TFitResultPtr Calibrate(TH1 *h, TF1 *f);
   void CalibrateBadChannels(DrawObjContainer *c);
   void Print(const Option_t* opt="") const;

   ClassDef(AlphaCalibrator, 1)
};

#endif
