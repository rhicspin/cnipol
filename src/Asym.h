// Header file for Asym
// Author   : Itaru Nakagawa
// Creation : 11/18/2005         

/**
 *
 * 15 Oct, 2010 - Dmitri Smirnov
 *    - Modified readloop to take an object of Root class as an argument 
 *
 */

#ifndef ASYM_STREAM_H
#define ASYM_STREAM_H

#include <bitset>
#include <map>
#include <set>
#include <string>

#include "rhicpol.h"
#include "rpoldata.h"

#include "TBuffer.h"
#include "TString.h"

#include "AsymHeader.h"

#ifdef NULL
#undef NULL
#endif

#define NULL 0

#define NWORDS_PAWC 20000000       // paw memory size 
#define RECLEN 1024
#define HMINENE 100.               // minimum energy for histograms 
#define HMAXENE 1500.              // maximum energy for histograms
#define HENEBIN 180                // number of energy bin in banana plot
#define NTLIMIT 100000000

// whole info for one event
typedef struct {
    int stN;   // strip number
    int amp;   // amplitude of signal
    int tdc;   // time of flight
    int tdcmax;  // tdc max amplitude
    int intg;  // integral of signal
    int bid;   // bunch crossing number
    long delim;
    int rev0;
    int rev;
} processEvent;

typedef struct {
    float phys;         // physics asymmetry
    float physE;        // physics asymmetry error
    float acpt;         // acceptance asymmetry
    float acptE;        // acceptance asymmetry error
    float lumi;         // luminosity asymmetry
    float lumiE;        // luminosity asymmetry error
} asymStruct;


typedef struct TDatprocStruct
{
   // Constraint parameter for Data processing 
   int            enel;               // lower kinetic energy threshold (keV)
   int            eneu;               // upper kinetic energy threshold (keV)
   int            widthl;             // lower banana cut (ns)
   int            widthu;             // upper banana cut (ns)
   int            FEEDBACKMODE;       // fit 12C peak first and feedback tshift and sigma
   int            RAWHISTOGRAM;       // Fill raw histograms
   int            CMODE;              // Calibration mode 0:off 1:on
   int            DMODE;              // dead layer study mode 0:off 1:on
   int            TMODE;              // T0 study mode 0:off 1:on
   int            AMODE;              // A0,A1 study (signal Amp vs. Int) mode 0:off 1:on
   int            BMODE;              // create banana curve (E-T) plots 0:off 1:on
   int            ZMODE;              // with/out T0 subtraction 0:with 1:without
   int            MESSAGE;            // message mode 1: exit just after run begin
   int            CBANANA;            // constant width banana cut :1, <sigma> Mass Cut :2
   int            UPDATE;             // 1: keep update of the histogram
   int            MMODE;              // mass mode
   int            NTMODE;             // if 1 store NTUPLEv
   int            RECONFMODE;         // if 1 reconfigure from file
   int            RAMPMODE;           // if 1 prepare the histograms for ramp measurement
   int            STUDYMODE;          // if 1 study mode
   std::bitset<3> SAVETREES;          // bitmask telling which ROOT trees to save
   float          MassSigma;          // banana curve cut within <MassSigma> away from the 12C mass
   float          MassSigmaAlt;       // banana curve alternative cut within
                                      // <MassSigmaAlt> away from the 12C mass
   float          OneSigma;           // 1-sigma of 12C mass distribution in [keV]
   float          tshift;             // time shift in [ns]
   float          inj_tshift;         // time shift in [ns] for injection w.r.t. flattop
   float          dx_offset;          // additional deadlayer offset [ug/cm2]
   float          WCMRANGE;           // Wall Current Monitor process Fill range
   float          MassLimit;          // Lower Mass limit for peak position adjustment fit
   UInt_t         nEventsProcessed;   // number of events processed from raw data file
   UInt_t         nEventsTotal;       // number of total events in raw data file
   UInt_t         thinout;            // Every <thinout> event to be feed into feedback routine
   float          reference_rate;     // Expected universal rate for given target
   float          target_count_mm;    // Target count/mm conversion
   time_t         procDateTime;       // Date/time when processing started
   Double_t       procTimeReal;       // Time in seconds to process input raw file
   Double_t       procTimeCpu;        // Time in seconds to process input raw file
   std::string    userCalibFile;    // Calibration file pass by user as argument

   void Streamer(TBuffer &buf);
};

TBuffer & operator<<(TBuffer &buf, TDatprocStruct *&rec);
TBuffer & operator>>(TBuffer &buf, TDatprocStruct *&rec);


typedef struct {
  float MASS_WIDTH_DEV_ALLOWANCE;
  float MASS_POSITION_ALLOWANCE;
  float MASS_CHI2_ALLOWANCE;
  float MASS_ENERGY_CORR_ALLOWANCE;
  float GOOD_CARBON_EVENTS_ALLOWANCE;
  float BUNCH_RATE_SIGMA_ALLOWANCE;
  float BUNCH_ASYM_SIGMA_ALLOWANCE;
  int NBUNCH_REQUIREMENT;
}ErrorDetector;


typedef struct {
    long ia;
    long it;
    long is;
    long ib;
    long id;
    int strip;
    float e;
    float tof;
    int spin;
} atdata_struct;

typedef struct {
  float Ct;    // TDC Count unit (ns/channel)
  float E2T;   // kinetic energy -> tof
  float M2T;   // Mass->tof
  float T2M;   // ToF -> Mass
} StructRunConst;

typedef struct {
  int detector;
} StructMask;

typedef struct {
  int nstrip;
  int st[NSTRIP];
  float bad_st_rate;
  int strip_err_code;
  int nbunch;
  int bunch[NBUNCH];
  float bad_bunch_rate;
  int bunch_err_code;
} StructAnomaly;

typedef struct {
  StructAnomaly anomaly;
} StructUnrecognized;


typedef struct {
  float P[2];
  float dPhi[2];
  float chi2;
} StructSinPhi;

typedef struct {
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
} StructAnalysis;


struct TStructRunInfo {
   int          Run;
   double       RUNID;
   std::string  runName;
   int          StartTime;
   int          StopTime;
   float        RunTime;
   float        GoodEventRate;
   float        EvntRate;
   float        ReadRate;
   float        WcmAve;
   float        WcmSum;
   double       BeamEnergy;
   int          RHICBeam;
   int          PolarimetryID;
   int          MaxRevolution;
   char         target;
   char         targetID;
   char         TgtOperation[16];
   int          ActiveDetector[NDETECTOR];
   int          ActiveStrip[NSTRIP];
   int          NActiveStrip;
   int          NDisableStrip;
   int          DisableStrip[NSTRIP];
   int          NFilledBunch;
   int          NActiveBunch;
   int          NDisableBunch;
   int          DisableBunch[NBUNCH];

   TStructRunInfo();
   ~TStructRunInfo();
   void Streamer(TBuffer &buf);

};// StructRunInfo;

TBuffer & operator<<(TBuffer &buf, TStructRunInfo *&rec);
TBuffer & operator>>(TBuffer &buf, TStructRunInfo *&rec);


typedef struct TStructRunDB {
   double      RunID;
   std::string runName;
   UInt_t      fillId;
   UInt_t      polarimeterId;
   UInt_t      measurementId;
   time_t      timeStamp;
   std::string dateTime;
   Bool_t      isCalibRun;
   std::string calib_file_s;
   std::string alpha_calib_run_name;
   std::string config_file_s;
   std::string masscut_s;
   std::string tshift_s;
   std::string inj_tshift_s;
   std::string run_status_s; 
   std::string measurement_type_s;
   std::string disable_strip_s;
   std::string enable_strip_s;
   std::string disable_bunch_s;
   std::string enable_bunch_s;
   std::string define_spin_pattern_s;
   std::string define_fill_pattern_s;
   std::string reference_rate_s;
   std::string target_count_mm_s;
   std::string comment_s;

   bool operator()(const TStructRunDB &rec1, const TStructRunDB &rec2) const;
   void Streamer(TBuffer &buf);
   void Print(const Option_t* opt="") const;

} StructRunDB;

TBuffer & operator<<(TBuffer &buf, TStructRunDB *&rec);
TBuffer & operator>>(TBuffer &buf, TStructRunDB *&rec);

//struct TStructRunDBCompare {
//}

typedef std::set<TStructRunDB, TStructRunDB> DBRunSet;


typedef struct {
  int CONFIG;
  int MASSCUT;
  int TSHIFT;
}StructExtInput;


typedef struct {
  float total;
  float average;
  int counter;
} StructAverage;

typedef struct {
  float mean;
  float RMS;
  float RMSnorm;
  float maxbin;
} StructHistStat;

typedef struct {
  float RMS[NSTRIP];
  float mdev[NSTRIP];
  float tedev[NSTRIP];
  float err[NSTRIP];
  float chi2[NSTRIP];
  float strip[NSTRIP];
} StructFeedBack;


typedef struct {
  int RECBEGIN;
  int PCTARGET;
  int WCMADO;
  int BEAMADO;
  int RHICCONF;
} StructReadFlag ;


typedef struct {
  int VERBOSE;
  int feedback;
  int spin_pattern;
  int fill_pattern;
  int mask_bunch;
  int EXE_ANOMALY_CHECK;
} StructFlag;


typedef struct {
  long int NStrip[3][NSTRIP];    // strip counters for 3 different spin states
} StructStripCounter;

typedef struct {
  long int NStrip[MAXDELIM][3][NSTRIP];    // strip counters for 3 different spin states
} StructStripCounterTgt;

typedef struct {
  long int good_event;
  long int revolution;
  long int tgtMotion;
  long int good[MAXDELIM];
  StructStripCounter reg, alt, phx, str;
} StructCounter;

typedef struct {
  long int good_event;
  long int revolution;
  long int tgtMotion;
  long int good[MAXDELIM];
  StructStripCounterTgt reg;
} StructCounterTgt;

typedef struct {
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
} StructTarget;


typedef struct {
  int bunchpat[NBUNCH];
} StructBunchPattern;


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

  void hhbook1d_(int*, char*, int*, float*, float*,  int);
  void hhbook2d_(int*, char*, int*, float*, float*, int*, float*, float*,  int);
  void hhf1_(int*, float*, float*);
  void hhf2_(int*, float*, float*, float*);
  void hhlimit_(int*);
  void hhropen_(int*, char*, char*, char*, int*, int*,  int,int,int);
  void hhrend_(char*, int);
  void hhrout_(int*, int*, char*, int);
  void hhrammar_(float*, int*);
  void hhrebin_(int*, float*, float*, float*, float*, int*, int*, int*);
  void hhpak_(int*, float*);
  void hhpake_(int*, float*);
  float hhmax_(int*);
  float hhstati_(int*, int*, char*, int*, int);
  //  void hhkind_(int*, int*, char*, int);
  void hfith_(int*, char*, char*, int*, float*, float*, float*, float*, float*, float*, int, int);
  void hfithn_(int*, char*, char*, int*, float*, float*, float*, float*, float*, float*, int, int);
}

void tgtHistBook();
int hist_book(char *);
int hist_close(char *);
int read_rebin(char *);
int read_ramptiming(char *);
int Initialization();
int spill_end(recordConfigRhicStruct *);
int sqass(float, float, float, float, float *, float *);
int end_process(recordConfigRhicStruct *);

//int GetPolarimetryID_and_RHICBeam(char RunID[]);
int printConfig(recordConfigRhicStruct *);
//void reConfig(recordConfigRhicStruct *);
//void calcRunConst(recordConfigRhicStruct *);
int ExclusionList(int i, int j, int RHICBeam);
int calcAsymmetry(int a, int b, int atot, int btot, float &Asym, float &dAsym);
//int DisabledDet(int det);
//int ConfigureActiveStrip(int);
void SpecificLuminosity(float&, float&, float&);
float TshiftFinder(int, int);
//int BunchSelect(int);
void CalcAsymmetry(float);
void PrintRunResults(StructHistStat);
void checkForBadBunches();

#endif
