// Header file for Asym
// Author   : Itaru Nakagawa
// Creation : 11/18/2005         

#ifndef ASYM_STREAM_H
#define ASYM_STREAM_H
#include <string.h>       // This is funny, but need to be included here to 
using namespace std;      // declare string in structure

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


#define RHIC_REVOLUTION_FREQ 78e3  // RHIC Revolution Frequency 78 [kHz]
#define RHIC_MAX_FILL 120          // RHIC Maximum Fill Bunches
#define RAMPTIME 350               // duration of the ramp measurement (sec)
#define MAXDELIM 410               // maximum number of delimiter (dynamic motion only)
#define TARGETINDEX 1000           // maximum target full array size including static motion
#define NTBIN 14                   // number of -t bin

#define NSTRIP 72                  // Number of strip channels
#define NTGTWFD 4                  // Number of target WFD channels
#define NDETECTOR 6                // Number of detectors
#define NBUNCH 120                 // Maximum bunch number
#define R2D 57.29577951            // [rad] -> [degree]
#define G2k 1e6                    // GeV -> keV
#define k2G 1e-6                   // keV -> GeV
#define MASS_12C 11.187e6          // Mass Carbon in [keV]
#define C_CMNS 29.98               // Speed of Light in [cm/ns]
#define TGT_STEP 0.11              // target motion [mm]/step
#define TGT_COUNT_MM 0.1           // target [count] -> [mm]
const int TGT_OPERATION=6;         // if nTgtIndex>TGT_OPERATION, then "scan", otherwise "fixed"
const int TOT_WFD_CH=NSTRIP+NTGTWFD;//Total WFD channels including target WFD channels.

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

typedef struct {
    // Constraint parameter for Data processing 
    int enel;          // lower kinetic energy threshold (keV) 
    int eneu;          // upper kinetic energy threshold (keV)
    int widthl;        // lower banana cut (ns)
    int widthu;        // upper banana cut (ns)
    int FEEDBACKMODE;  // fit 12C peak first and feedback tshift and sigma
    int CMODE;         // Calibration mode 0:off 1:on
    int DMODE;         // dead layer study mode 0:off 1:on
    int TMODE;         // T0 study mode 0:off 1:on
    int AMODE;         // A0,A1 study (signal Amp vs. Int) mode 0:off 1:on
    int BMODE;         // create banana curve (E-T) plots 0:off 1:on
    int ZMODE;         // with/out T0 subtraction 0:with 1:without
    int MESSAGE;       // message mode 1: exit just after run begin 
    int CBANANA;       // constant width banana cut :1, <sigma> Mass Cut :2
    int UPDATE;        // 1: keep update of the histogram
    int MMODE;         // mass mode 
    int NTMODE;        // if 1 store NTUPLEv
    int RECONFMODE;    // if 1 reconfigure from file 
    int RAMPMODE;      // if 1 prepare the histograms for ramp measurement
    int STUDYMODE;     // if 1 study mode
  float MassSigma;     // banana curve cut within <MassSigma> away from the 12C mass
  float MassSigmaAlt;  // banana curve alternative cut within <MassSigmaAlt> away from the 12C mass
  float OneSigma;      // 1-sigma of 12C mass distribution in [keV]
  float tshift;        // time shift in [ns]
  float inj_tshift;    // time shift in [ns] for injection w.r.t. flattop
  float WCMRANGE;      // Wall Current Monitor process Fill range
  float MassLimit;     // Lower Mass limit for peak position adjustment fit
  int thinout;         // Every <thinout> event to be feed into feedback routine
} datprocStruct; 


typedef struct {
  float MASS_DEV_ALLOWANCE;
  float MASS_CHI2_ALLOWANCE;
  float MASS_ENERGY_CORR_ALLOWANCE;
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
  int nstrip;
  int st[NSTRIP];
  float bad_st_rate;
  int nbunch;
  int bunch[NBUNCH];
  float bad_bunch_rate;
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
  float TshiftAve;
  float A_N[2];
  float P[2];
  float P_sigma_ratio[2];
  float P_sigma_ratio_norm[2];
  float energy_slope[2];    // Slope for energy spectrum (detectors sum) [0]:slope [1]:error
  StructSinPhi sinphi[2];   // [0]: regular,  [1]: alternative sigma cut
  StructAnomaly anomaly;
  StructUnrecognized unrecog;
} StructAnalysis;



typedef struct {
  int Run;
  double RUNID;
  int StartTime;
  int StopTime;
  int RunTime;
  float EvntRate;
  float ReadRate;
  float WcmAve;
  double BeamEnergy;
  int RHICBeam;
  int MaxRevolution;
  char target;
  char * TgtOperation;
  int NDisableStrip;
  int DisableStrip[NSTRIP];
  int NFilledBunch;
  int NDisableBunch;
  int DisableBunch[NBUNCH];
} StructRunInfo;

typedef struct {
  double RunID;
  string calib_file_s;
  string config_file_s;
  string masscut_s;
  string tshift_s;
  string inj_tshift_s;
  string run_status_s; 
  string measurement_type_s;
  string disable_strip_s;
  string enable_strip_s;
  string disable_bunch_s;
  string enable_bunch_s;
  string define_spin_pattern_s;
  string define_fill_pattern_s;
  string comment_s;
}StructRunDB ;


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
} StructFlag;


typedef struct {
  long int NStrip[3][NSTRIP];    // strip counters for 3 different spin states
} StructStripCounter;

typedef struct {
  long int revolution;
  long int tgtMotion;
  StructStripCounter reg, alt;
} StructCounter;


typedef struct {
    float x;                     // (arbitarary) target postion [mm]
    int vector;
    long int eventID;
    int VHtarget;
    int Index[MAXDELIM];
    int Linear[MAXDELIM][2];   // Vertical:[0], Horizontal:[1]
    int Rotary[MAXDELIM][2];
    struct StructAll {
        float x[TARGETINDEX];  // target position in [mm]
    } all;
} StructTarget;



// Hbook Associated Stuff
extern "C" {

  void hhbook1d_(int*, char*, int*, float*, float*,  int);
  void hhbook2d_(int*, char*, 
		 int*, float*, float*, int*, float*, float*,  int);
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


int readloop(void);

int readdb(double RUNID);
void tgtHistBook();
int hist_book(char *);
int hist_close(char *);
int read_rebin(char *);
int read_ramptiming(char *);
int Initialization();
int spill_end(recordConfigRhicStruct *);
int sqass(float, float, float, float, float *, float *);
int end_process(recordConfigRhicStruct *);

int printConfig(recordConfigRhicStruct *);
void reConfig(recordConfigRhicStruct *);
void calcRunConst(recordConfigRhicStruct *);
int ExclusionList(int i, int j, int RHICBeam);
int calcAsymmetry(int a, int b, int atot, int btot, float &Asym, float &dAsym);
int DisabledDet(int det);
void SpecificLuminosity(float&, float&, float&);
float TshiftFinder(int, int);
int BunchSelect(int);
void CalcAsymmetry(float);
void PrintRunResults(StructHistStat);
void checkForBadBunches();


// GLOBAL VARIABLES

extern int spinpat[120]; // spin pattern 120 bunches
extern int fillpat[120]; // fill pattern 120 bunches
extern int wcmfillpat[120]; //  fill pattern within the Wall Current Monitor Average Ragne 
extern float wcmdist[120];  // wall current monitor 120 bunches

extern long int Ncounts[6][120]; // counts 6detectors 120 bunches
extern long int NTcounts[6][120][NTBIN];  // counts 6detectors 120 bunches 6 tranges
extern long int NDcounts[6][120][MAXDELIM];  // counts 6detectors 120 bunches per delimiter
extern long int NStrip[3][72]; // counts 72 strips 3 spin states
 
extern long int NRcounts[6][120][RAMPTIME];// counts 6det 120bunch RAMPTIME sec

extern char * confdir;
extern char * calibdir;
extern char datafile[256];   // data file name 
extern char reConfFile[256];         // update configuration file for T0 info
extern char conf_file[256];          // update configuration file for T0 info
extern char CalibFile[256];          // Energy calibration file

extern float ramptshift[500]; // ramp timing shift 

extern long int Nevcut;   // number of events after 1st cut (whole data)
extern long int Nevtot;   // number of total events (whole data) 
extern long int Nread;    // real total events (completely everything)
extern long int Nwcmtot;  // number of total wall current monitor data.
extern int Nskip;         // number of events to be skipped in data process 
extern int NFilledBunch;  // number of filled bunch

extern long int Ngood[120];   // number of evts after carbon cut 
extern long int Ntotal[120];  // number of evts before carbon cut 
extern long int Nback[120];   // number of evts below the curbon cut
extern datprocStruct dproc;
extern ErrorDetector errdet;
extern atdata_struct atdata;
extern StructRunDB rundb;
extern StructRunInfo runinfo;
extern StructExtInput extinput;
extern StructAverage average;
extern StructHistStat hstat;
extern StructFeedBack feedback;
extern StructCounter cntr;
extern StructRunConst runconst;
extern StructFlag Flag;
extern StructReadFlag ReadFlag;
extern StructAnalysis anal;
extern StructTarget tgt;

extern float phiRun5[NSTRIP];   // phi-angle for each strips of Run5 (l=18.5cm)
extern float phiRun6[NSTRIP];   // phi-angle for each strips of Run6 (l=18.0cm)
extern float phit[NSTRIP];      // phi-angle for each strips in approximation 45,90,135... 
extern float phi[NSTRIP];       // phi-angle


// target position infomation 
extern int ndelim ;
extern int TgtIndex[MAXDELIM];
extern int nTgtIndex;

#endif









