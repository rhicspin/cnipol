
#ifndef AnaMeasResult_h
#define AnaMeasResult_h

#include <set>
#include <string>

#include "TObject.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"

#include "AsymCalculator.h"
#include "AsymHeader.h"
#include "AsymGlobals.h"


class AnaMeasResult;

//typedef std::set<AnaMeasResult>             AnaMeasResultSet;
//typedef std::set<AnaMeasResult>::iterator   AnaMeasResultSetIter;
typedef std::map<std::string, AnaMeasResult>                   AnaMeasResultMap;
typedef std::map<std::string, AnaMeasResult>::iterator         AnaMeasResultMapIter;
typedef std::map<std::string, AnaMeasResult>::const_iterator   AnaMeasResultMapConstIter;


/** */
class AnaMeasResult : public TObject
{
public:

   Float_t              max_rate;
   Float_t              TshiftAve;
   Float_t              wcm_norm_event_rate; 
   Float_t              UniversalRate;
   Float_t              A_N[2];                 // Analyzing power
   Float_t              P[2];  
   ValErrPair           fAnaPower;              // Analyzin power A_N
   ValErrPair           fPolar;
   ValErrPair           fAvrgPMAsym;            // deprecated. Plus-Minus (+/-) spin state asymmetry
   TFitResultPtr        fFitResAsymPhi;         // Fit result from asymmetry vs phi plot
   TFitResultPtr        fFitResPolarPhi;        // Fit result from polarization vs phi plot
   Spin2FitResMap       fFitResAsymBunchX90;    // Fit results from asymmetry vs bunch histograms for different spin states
   Spin2FitResMap       fFitResAsymBunchX45;    //
   Spin2FitResMap       fFitResAsymBunchY45;    //
   ValErrMap            fAsymX90;               // Results from classic square root formula. Different definitions of asymmetry: phys, lumi, geom
   ValErrMap            fAsymX45;               //
   ValErrMap            fAsymX45T;              //! upper/top 45-deg detectors
   ValErrMap            fAsymX45B;              //! lower/bottom 45-deg detectors
   ValErrMap            fAsymY45;               //
   TFitResultPtr        fFitResProfilePvsI;     // Fit result from profile polarization vs intensity plot
   TFitResultPtr        fFitResEnergySlope;     // Fit result from energy distribution histogram
   TFitResultPtr        fFitResPseudoMass;      // Fit result from mass distribution histogram
   //Float_t              fBananaCount;         // The number of events in the final sample
   Float_t              P_sigma_ratio[2];
   Float_t              P_sigma_ratio_norm[2];
   Float_t              energy_slope[2];        // Slope for energy spectrum (detectors sum) [0]:slope [1]:error
   Float_t              profile_error;          // profile error
   StructSinPhi         sinphi[100+MAXDELIM];   // [0]: regular,  [1]: alternative sigma cut [3]: PHENIX bunch [4]:STAR bunch; 100... target pos
   StructSinPhi         basym[100+MAXDELIM];    // [0]: regular,  [1]: alternative sigma cut [3]: PHENIX bunch [4]:STAR bunch; 100... target pos
   StructAnomaly        anomaly;                //!
   StructUnrecognized   unrecog;                //!

   ValErrPair           fProfilePolarMax;       // The peak polarization from the P vs I fit 
   ValErrPair           fProfilePolarR;         // The R profile defines the ratio of intensity and polarization profiles

   Float_t              fPmtV1T0;
   Float_t              fPmtV1T0Err;
   Float_t              fPmtS1T0;
   Float_t              fPmtS1T0Err;

public:
   AnaMeasResult();
   ~AnaMeasResult();
   void PrintAsPhp(FILE *f=stdout) const;

   ValErrPair GetPolar() const;

   ClassDef(AnaMeasResult, 1)
};

#endif
