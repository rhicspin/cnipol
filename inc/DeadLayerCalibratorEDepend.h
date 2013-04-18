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
public:


public:

   DeadLayerCalibratorEDepend();
   ~DeadLayerCalibratorEDepend();

   virtual void Calibrate(DrawObjContainer *c);
   //virtual void CalibrateFast(DrawObjContainer *c);
   virtual void Calibrate(TH1 *h, TH1 *hMeanTime, UShort_t chId=0, TObjArray* fitResultHists=0, Bool_t wideLimits=false);
   void         PostCalibrate();
   void         Print(const Option_t* opt="") const;


   static RunConst sRunConst;
   static std::map<UShort_t, RunConst> sRunConsts;
   static Double_t BananaFitFunc(Double_t *x, Double_t *p);

   static const Double_t cp0[4];
   static const Double_t cp1[4];
   static const Double_t cp2[4];
   static const Double_t cp3[4];
   static const Double_t cp4[4];

   ClassDef(DeadLayerCalibratorEDepend, 1)
};


class BananaFitFunctor
{
public:

   Double_t fCp0[4];
   Double_t fCp1[4];
   Double_t fCp2[4];
   Double_t fCp3[4];
   Double_t fCp4[4];

   RunConst fRunConst;
  
public:

   BananaFitFunctor(UShort_t chId);
   ~BananaFitFunctor();

   Double_t operator()(double *x, double *p);
};

#endif
