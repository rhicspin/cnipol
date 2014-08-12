#ifndef CalibCoefSet_h
#define CalibCoefSet_h


/**
 * A basic data structure to hold a set of calibration parameters extracted from
 * a detector response curve of a known source of alpha particles. A gaussian
 * shape is assumed in the case of RHIC polarimeters which is fitted to the data
 * acquired from a single detector channel. See AlphaCalibrator for details.
 */
class CalibCoefSet : public TObject
{
public:

   Float_t fPeakPos;      ///< The mean of a gaussian function in ADC units
   Float_t fPeakPosErr;   ///< Error on the mean in ADC units
   Float_t fCoef;         ///< Inverse channel gain, keV/ADC * channel attenuation
   Float_t fCoefErr;      ///< Error on inverse channel gain, keV/ADC * channel attenuation
   Float_t fChi2Ndf;      ///< Chi^2 per d.f. from the fit

public:

   CalibCoefSet() : fCoef(-1), fCoefErr(-1), fChi2Ndf(0) { };

   ClassDef(CalibCoefSet, 1)
};

#endif
