/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef DeadLayerCalibratorEDepend_h
#define DeadLayerCalibratorEDepend_h

#include <map>
#include <set>
#include <vector>
#include <float.h>
#include <math.h>

#include "TF1.h"
#include "TF2.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TH1.h"
#include "TH1D.h"
#include "TMath.h"

#include "Asym.h"
#include "AsymHeader.h"
#include "AsymGlobals.h"

#include "DeadLayerCalibrator.h"
#include "DrawObjContainer.h"


/** */
class DeadLayerCalibratorEDepend : public DeadLayerCalibrator
{
public:


public:

   DeadLayerCalibratorEDepend();
   ~DeadLayerCalibratorEDepend();

   virtual void Calibrate(DrawObjContainer *c);
   virtual void CalibrateFast(DrawObjContainer *c);
   TFitResultPtr Calibrate(TH1 *h, TH1D *hMeanTime, Bool_t wideLimits=false);
   void PostCalibrate();
   void Print(const Option_t* opt="") const;

   static RunConst sRunConst;
   static Double_t BananaFitFunc(Double_t *x, Double_t *par);

   static const Double_t cp0[4];
   static const Double_t cp1[4];
   static const Double_t cp2[4];
   static const Double_t cp3[4];
   static const Double_t cp4[4];

   ClassDef(DeadLayerCalibratorEDepend, 1)
};

#endif