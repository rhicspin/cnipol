
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
#include "MeasInfo.h"

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
      DrawObjContainer *c, UShort_t i, std::string suffix
   );
   void AmGdPlot(
      const ChannelCalib *chCalib, DrawObjContainer *c, int iCh, const std::string &sCh
   );
   Int_t         GetMaximumBinEx(TH1F *h, int blur_radius);
   TFitResultPtr Calibrate(TH1F *h, bool fit_gadolinium);

public:

   AlphaCalibrator();
   ~AlphaCalibrator();

   virtual void  Calibrate(DrawObjContainer *c);
   void CalibrateBadChannels(DrawObjContainer *c);
   void Print(const Option_t* opt="") const;

   ClassDef(AlphaCalibrator, 1)
};

#endif
