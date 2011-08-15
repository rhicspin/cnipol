
#ifndef AnaResult_h
#define AnaResult_h

#include "TObject.h"

#include "AsymCalculator.h"
#include "AsymHeader.h"
#include "AsymGlobals.h"


/** */
class AnaResult : public TObject
{
public:

   Float_t              max_rate;
   Float_t              TshiftAve;
   Float_t              wcm_norm_event_rate; 
   Float_t              UniversalRate;
   Float_t              A_N[2];                 // Analyzing power
   Float_t              P[2];  
   ValErrPair           fAnaPower;              // Analyzin power A_N
   ValErrPair           fPol;
   ValErrPair           fAvrgPMAsym;            // deprecated. Plus-Minus (+/-) spin state asymmetry
   TFitResultPtr        fFitResAsymPhi;
   TFitResultPtr        fFitResPolarPhi;
   Spin2FitResMap       fFitResAsymBunchX90;
   Spin2FitResMap       fFitResAsymBunchX45;
   Spin2FitResMap       fFitResAsymBunchY45;
   ValErrMap            fAsymX90;
   ValErrMap            fAsymX45;
   ValErrMap            fAsymY45;
   //Float_t              fBananaCount;         // The number of events in the final sample
   Float_t              P_sigma_ratio[2];
   Float_t              P_sigma_ratio_norm[2];
   Float_t              energy_slope[2];        // Slope for energy spectrum (detectors sum) [0]:slope [1]:error
   Float_t              profile_error;          // profile error
   StructSinPhi         sinphi[100+MAXDELIM];   // [0]: regular,  [1]: alternative sigma cut [3]: PHENIX bunch [4]:STAR bunch; 100... target pos
   StructSinPhi         basym[100+MAXDELIM];    // [0]: regular,  [1]: alternative sigma cut [3]: PHENIX bunch [4]:STAR bunch; 100... target pos
   StructAnomaly        anomaly;                //!
   StructUnrecognized   unrecog;                //!

   ValErrPair           fProfilePolarMax;        // The peak polarization from the P vs I fit 
   ValErrPair           fProfilePolarR;          // The R profile defines the ratio of intensity and polarization profiles

   Float_t              fPmtV1T0;
   Float_t              fPmtV1T0Err;
   Float_t              fPmtS1T0;
   Float_t              fPmtS1T0Err;

public:
   AnaResult();
   ~AnaResult();
   void PrintAsPhp(FILE *f=stdout) const;

   ClassDef(AnaResult, 1)
};

#endif
