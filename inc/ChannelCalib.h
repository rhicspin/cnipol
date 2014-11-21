
#ifndef ChannelCalib_h
#define ChannelCalib_h

#include <map>
#include <vector>

#include "TObject.h"

#include "CalibCoefSet.h"

enum EFitStatus {
   kUNKNOWN,
   kDLFIT_OK,
   kDLFIT_FAIL,
   kDL_OUTLIER,
   kT0_OUTLIER,
   kCHI2_OUTLIER
};

class ChannelCalib;

typedef std::vector<ChannelCalib>                        ChannelCalibVec;
typedef std::map<UShort_t, ChannelCalib>                 ChannelCalibMap;
typedef std::map<UShort_t, ChannelCalib>::iterator       ChannelCalibMapIter;
typedef std::map<UShort_t, ChannelCalib>::const_iterator ChannelCalibMapConstIter;
typedef std::pair<const UShort_t, ChannelCalib>          ChannelCalibPair;


/**
 * A data container to hold a complete set of calibration parameters for each
 * detector channel. Depending on the calibration procedure the data members
 * are filled by either AlphaCalibrator or DeadLayerCalibratorEDepend.
 */
class ChannelCalib : public TObject
{
public:

   CalibCoefSet fAmAmp;       ///< Basic data structure to hold energy calibration parameters from an Americium alpha source
   CalibCoefSet fAmInt;       ///< Basic data structure to hold energy calibration parameters from an Americium alpha source
   CalibCoefSet fGdAmp;       ///< Basic data structure to hold energy calibration parameters from an Gadolinium alpha source
   CalibCoefSet fGdInt;       ///< Basic data structure to hold energy calibration parameters from an Gadolinium alpha source
   Float_t    fZeroBiasGain;  ///< Gain value adjusted to zero bias current
   Float_t    fEffectiveGain; ///< Gain value ready for production use
   Float_t    fDLWidth;       ///< Width of the detector channel's dead layer
   Float_t    fDLWidthErr;    ///< Error on the width of the detector channel's dead layer
   Float_t    fT0Coef;        ///< Average time offset for all triggers
   Float_t    fT0CoefErr;     ///< Error on the average time offset for all triggers
   Float_t    fAvrgEMiss;     ///< Missing energy not registered by the detector channel. Estimated from the banana fit
   Float_t    fAvrgEMissErr;  ///< Error on fAvrgEMiss estimated from the banana fit
   Float_t    fEMeasDLCorr;   ///< Correction factor for E_meas = C_alpha * ADC * fEMeasDLCorr
   Float_t    fBananaChi2Ndf;
   EFitStatus fFitStatus;     ///< Status code indicating a problem (if any) with calculation of calibration parameters for that channel

public:

   ChannelCalib();
   ~ChannelCalib();

   EFitStatus GetFitStatus() const;   
   Float_t    GetBananaChi2Ndf() const { return fBananaChi2Ndf; }
   void       CopyAlphaCoefs(const ChannelCalib &chCalib);
   void       Print(const Option_t* opt="") const;
   void       PrintAsPhp(FILE *f=stdout) const;
   void       ResetToZero();

   ClassDef(ChannelCalib, 2)
};

#endif
