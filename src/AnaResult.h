
#ifndef AnaResult_h
#define AnaResult_h

#include "TObject.h"

#include "AsymHeader.h"
#include "AsymGlobals.h"


/** */
class AnaResult : public TObject
{
public:

   float              max_rate;
   float              TshiftAve;
   float              wcm_norm_event_rate; 
   float              UniversalRate;
   float              A_N[2];                 // Analyzing power
   float              P[2];  
   float              fAvrgPMAsym;             // Plus-Minus (+/-) spin state asymmetry
   float              fAvrgPMAsymErr;  
   float              P_sigma_ratio[2];
   float              P_sigma_ratio_norm[2];
   float              energy_slope[2];        // Slope for energy spectrum (detectors sum) [0]:slope [1]:error
   float              profile_error;          // profile error
   StructSinPhi       sinphi[100+MAXDELIM];   // [0]: regular,  [1]: alternative sigma cut [3]: PHENIX bunch [4]:STAR bunch; 100... target pos
   StructSinPhi       basym[100+MAXDELIM];    // [0]: regular,  [1]: alternative sigma cut [3]: PHENIX bunch [4]:STAR bunch; 100... target pos
   StructAnomaly      anomaly; //!
   StructUnrecognized unrecog; //!
   float              fIntensPolarR;        // Scale factor to define the ratio of intensity and polarization profiles
   float              fIntensPolarRErr;
   float              fPmtV1T0;
   float              fPmtV1T0Err;
   float              fPmtS1T0;
   float              fPmtS1T0Err;

public:
   AnaResult();
   ~AnaResult();
   void PrintAsPhp(FILE *f=stdout) const;

   ClassDef(AnaResult, 1)
};

#endif
