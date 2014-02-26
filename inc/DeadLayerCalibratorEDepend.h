#ifndef DeadLayerCalibratorEDepend_h
#define DeadLayerCalibratorEDepend_h

#include <map>
#include <set>
#include <vector>
#include <float.h>
#include <math.h>

#include "TF1.h"
#include "TF2.h"
#include "TH1.h"
#include "TH1D.h"
#include "TMath.h"

#include "AsymCommon.h"
#include "AsymHeader.h"
#include "AsymGlobals.h"

#include "DeadLayerCalibrator.h"
#include "DrawObjContainer.h"


/** */
class DeadLayerCalibratorEDepend : public DeadLayerCalibrator
{
private:

   void         CalibrateChannel(UShort_t chId, TH1 *h, TH1 *hMeanTime, TObjArray* fitResultHists=0, Bool_t wideLimits=false);

public:

   virtual void Calibrate(DrawObjContainer *c);
   void         Print(const Option_t* opt="") const;

   static RunConst sRunConst;
   static std::map<UShort_t, RunConst> sRunConsts;

   ClassDef(DeadLayerCalibratorEDepend, 2)
};


class BananaFitFunctor
{
public:

   static const Double_t cp0[4];
   static const Double_t cp1[4];
   static const Double_t cp2[4];
   static const Double_t cp3[4];
   static const Double_t cp4[4];

   RunConst fRunConst;
  
   BananaFitFunctor(UShort_t chId);

   Double_t operator()(double *x, double *p);
};

#endif
