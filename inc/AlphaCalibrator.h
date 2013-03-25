/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef AlphaCalibrator_h
#define AlphaCalibrator_h

#include <map>
#include <set>
#include <string>
#include <vector>

#include "TF1.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TObject.h"

#include "AsymHeader.h"
#include "Calibrator.h"
#include "DrawObjContainer.h"

//class DrawObjContainer;


/** */
class AlphaCalibrator : public Calibrator
{
private:

   enum ESource
   {
      kAmericium = 0, kGadolinium = 3
   };

   CalibCoefSet CoefExtract (
      const TFitResultPtr &fitres, ESource source_offset,
      DrawObjContainer *c, UShort_t i, std::string name
   );
   void AmGdPlot(
      const ChannelCalib *chCalib, DrawObjContainer *c, const std::string &sCh
   );
   TFitResultPtr Calibrate(TH1 *h, bool fit_gadolinium);

public:

   AlphaCalibrator();
   ~AlphaCalibrator();

   virtual void  Calibrate(DrawObjContainer *c);
   //virtual void CalibrateFast(DrawObjContainer *c);
   void CalibrateBadChannels(DrawObjContainer *c);
   void Print(const Option_t* opt="") const;

   ClassDef(AlphaCalibrator, 1)
};

#endif
