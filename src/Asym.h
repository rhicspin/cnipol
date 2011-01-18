// Header file for Asym
// Author   : Itaru Nakagawa
// Creation : 11/18/2005         

/**
 *
 * 15 Oct, 2010 - Dmitri Smirnov
 *    - Modified readloop to take an object of Root class as an argument 
 *
 */

#ifndef Asym_h
#define Asym_h

#include <bitset>
#include <map>
#include <set>
#include <string>

#include "rhicpol.h"
#include "rpoldata.h"

#include "AsymHeader.h"


// whole info for one event
struct processEvent {
    int stN;   // strip number
    int amp;   // amplitude of signal
    int tdc;   // time of flight
    int tdcmax;  // tdc max amplitude
    int intg;  // integral of signal
    int bid;   // bunch crossing number
    long delim;
    int rev0;
    int rev;
};

struct asymStruct {
    float phys;         // physics asymmetry
    float physE;        // physics asymmetry error
    float acpt;         // acceptance asymmetry
    float acptE;        // acceptance asymmetry error
    float lumi;         // luminosity asymmetry
    float lumiE;        // luminosity asymmetry error
};

struct ErrorDetector {
  float MASS_WIDTH_DEV_ALLOWANCE;
  float MASS_POSITION_ALLOWANCE;
  float MASS_CHI2_ALLOWANCE;
  float MASS_ENERGY_CORR_ALLOWANCE;
  float GOOD_CARBON_EVENTS_ALLOWANCE;
  float BUNCH_RATE_SIGMA_ALLOWANCE;
  float BUNCH_ASYM_SIGMA_ALLOWANCE;
  int   NBUNCH_REQUIREMENT;
};

struct atdata_struct {
    long ia;
    long it;
    long is;
    long ib;
    long id;
    int strip;
    float e;
    float tof;
    int spin;
};


class RunConst
{
public:
   float L;     // Distance from target to detector
   float Ct;    // TDC Count unit (ns/channel)
   float E2T;   // kinetic energy -> tof
   float M2T;   // Mass->tof     [ns/keV^(1/2)]
   float T2M;   // ToF -> Mass

   RunConst(float lL=CARBON_PATH_DISTANCE, float lCt=WFD_TIME_UNIT_HALF);
   void Update(recordConfigRhicStruct *cfginfo=0, UShort_t ch=0);
   void Print(const Option_t* opt="") const;
};


struct StructMask {
  int detector;
};

struct StructAnomaly {
  int nstrip;
  int st[NSTRIP];
  float bad_st_rate;
  int strip_err_code;
  int nbunch;
  int bunch[NBUNCH];
  float bad_bunch_rate;
  int bunch_err_code;
};

struct StructUnrecognized {
  StructAnomaly anomaly;
};

struct StructSinPhi {
  float P[2];
  float dPhi[2];
  float chi2;
};

struct StructAnalysis {
  float max_rate;
  float TshiftAve;
  float wcm_norm_event_rate; 
  float UniversalRate;
  float A_N[2];
  float P[2];
  float P_sigma_ratio[2];
  float P_sigma_ratio_norm[2];
  float energy_slope[2];    // Slope for energy spectrum (detectors sum) [0]:slope [1]:error
  float profile_error;      // profile error
  StructSinPhi sinphi[100+MAXDELIM];   // [0]: regular,  [1]: alternative sigma cut [3]: PHENIX bunch [4]:STAR bunch; 100... target pos
  StructSinPhi basym[100+MAXDELIM];   // [0]: regular,  [1]: alternative sigma cut [3]: PHENIX bunch [4]:STAR bunch; 100... target pos
  StructAnomaly anomaly;
  StructUnrecognized unrecog;
};

struct StructExtInput {
  int CONFIG;
  int MASSCUT;
  int TSHIFT;
};


struct StructAverage {
  float total;
  float average;
  int counter;
};

struct StructHistStat {
  float mean;
  float RMS;
  float RMSnorm;
  float maxbin;
};


struct StructFeedBack {
  float RMS[NSTRIP];
  float mdev[NSTRIP];
  float tedev[NSTRIP];
  float err[NSTRIP];
  float chi2[NSTRIP];
  float strip[NSTRIP];
};


struct StructReadFlag {
  int RECBEGIN;
  int PCTARGET;
  int WCMADO;
  int BEAMADO;
  int RHICCONF;
};


struct StructFlag {
  int VERBOSE;
  int feedback;
  int spin_pattern;
  int fill_pattern;
  int mask_bunch;
  int EXE_ANOMALY_CHECK;
};


struct StructStripCounter {
  long int NStrip[3][NSTRIP];   // strip counters for 3 different spin states
};

struct StructStripCounterTgt {
  long int NStrip[MAXDELIM][3][NSTRIP]; // strip counters for 3 different spin states
};

struct StructCounter {
   long int good_event;
   long int revolution;
   long int tgtMotion;
   long int good[MAXDELIM];
   StructStripCounter reg, alt, phx, str;
};

struct StructCounterTgt {
  long int good_event;
  long int revolution;
  long int tgtMotion;
  long int good[MAXDELIM];
  StructStripCounterTgt reg;
};

struct StructTarget {
  float x;                    // (arbitarary) target postion [mm]
  int vector;
  long int eventID;
  int VHtarget;               // Vertical:[0], Horizontal:[1]
  int Index[MAXDELIM];
  int Linear[MAXDELIM][2];   
  int Rotary[MAXDELIM][2];
  float X[MAXDELIM];          // target position [mm] array excluding static position 
  float Interval[MAXDELIM];   // time interval of given target postiion [sec]
  float Time[MAXDELIM];       // duration from measurement start in [sec]
  struct StructAll {
    float x[TARGETINDEX];  // target position in [mm] including static position
  } all;
};

struct StructBunchPattern {
  int bunchpat[NBUNCH];
};

struct StructHist {
  int nxbin;
  float xmin;
  float xmax;
  int nybin;
  float ymin;
  float ymax;
};

// Hbook Associated Stuff
extern "C" {

  void  hhbook1d_(int*, char*, int*, float*, float*,  int);
  void  hhbook2d_(int*, char*, int*, float*, float*, int*, float*, float*,  int);
  void  hhf1_(int*, float*, float*);
  void  hhf2_(int*, float*, float*, float*);
  void  hhlimit_(int*);
  void  hhropen_(int*, char*, char*, char*, int*, int*,  int,int,int);
  void  hhrend_(char*, int);
  void  hhrout_(int*, int*, char*, int);
  void  hhrammar_(float*, int*);
  void  hhrebin_(int*, float*, float*, float*, float*, int*, int*, int*);
  void  hhpak_(int*, float*);
  void  hhpake_(int*, float*);
  float hhmax_(int*);
  float hhstati_(int*, int*, char*, int*, int);
  //void  hhkind_(int*, int*, char*, int);
  void  hfith_(int*, char*, char*, int*, float*, float*, float*, float*, float*, float*, int, int);
  void  hfithn_(int*, char*, char*, int*, float*, float*, float*, float*, float*, float*, int, int);
}

#endif
