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

#include "TBuffer.h"
#include "TDirectory.h"
#include "TH1F.h"
#include "TH1I.h"
#include "TH2F.h"
#include "TF1.h"
#include "TLine.h"

#include "rpoldata.h"
#include "AsymHeader.h"

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

extern std::map<std::string, std::string> gEnv;

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
    int RAWHISTOGRAM;  //Fill raw histograms 
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
std::bitset<3> SAVETREES;   // bitmask telling which ROOT trees to save
  float MassSigma;     // banana curve cut within <MassSigma> away from the 12C mass
  float MassSigmaAlt;  // banana curve alternative cut within <MassSigmaAlt> away from the 12C mass
  float OneSigma;      // 1-sigma of 12C mass distribution in [keV]
  float tshift;        // time shift in [ns]
  float inj_tshift;    // time shift in [ns] for injection w.r.t. flattop
  float dx_offset;     // additional deadlayer offset [ug/cm2]
  float WCMRANGE;      // Wall Current Monitor process Fill range
  float MassLimit;     // Lower Mass limit for peak position adjustment fit
  int thinout;         // Every <thinout> event to be feed into feedback routine
  float reference_rate;// Expected universal rate for given target
  float target_count_mm;//Target count/mm conversion
} datprocStruct; 


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



typedef struct StructRunInfo {
  int Run;
  double RUNID;
  int StartTime;
  int StopTime;
  float RunTime;
  float GoodEventRate;
  float EvntRate;
  float ReadRate;
  float WcmAve;
  float WcmSum;
  double BeamEnergy;
  int RHICBeam;
  int PolarimetryID;
  int MaxRevolution;
  char target;
  char targetID;
  //char * TgtOperation;
  char TgtOperation[16];
  int ActiveDetector[NDETECTOR];
  int ActiveStrip[NSTRIP];
  int NActiveStrip;
  int NDisableStrip;
  int DisableStrip[NSTRIP];
  int NFilledBunch;
  int NActiveBunch;
  int NDisableBunch;
  int DisableBunch[NBUNCH];

  void Streamer(TBuffer &buf);

} StructRunInfo;

TBuffer & operator<<(TBuffer &buf, StructRunInfo *&rec);
TBuffer & operator>>(TBuffer &buf, StructRunInfo *&rec);


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
  string reference_rate_s;
  string target_count_mm_s;
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

int GetPolarimetryID_and_RHICBeam(char RunID[]);
int printConfig(recordConfigRhicStruct *);
void reConfig(recordConfigRhicStruct *);
void calcRunConst(recordConfigRhicStruct *);
int ExclusionList(int i, int j, int RHICBeam);
int calcAsymmetry(int a, int b, int atot, int btot, float &Asym, float &dAsym);
//int DisabledDet(int det);
int ConfigureActiveStrip(int);
void SpecificLuminosity(float&, float&, float&);
float TshiftFinder(int, int);
int BunchSelect(int);
void CalcAsymmetry(float);
void PrintRunResults(StructHistStat);
void checkForBadBunches();


// GLOBAL VARIABLES

extern int spinpat[120]; // spin pattern 120 bunches
extern int fillpat[120]; // fill pattern 120 bunches
extern int ActiveBunch[NBUNCH];
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
extern int gMaxEventsUser;         // number of events to process
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
extern int toto;
extern StructCounter cntr;
extern StructCounterTgt cntr_tgt;
extern StructRunConst runconst;
extern StructMask mask;
extern StructFlag Flag;
extern StructReadFlag ReadFlag;
extern StructAnalysis anal;
extern StructTarget tgt;
extern StructBunchPattern phx, str;

extern float phiRun5[NSTRIP];   // phi-angle for each strips of Run5 (l=18.5cm)
extern float phiRun6[NSTRIP];   // phi-angle for each strips of Run6 (l=18.0cm)
extern float phit[NSTRIP];      // phi-angle for each strips in approximation 45,90,135... 
extern float phi[NSTRIP];       // phi-angle


// target position infomation 
extern int ndelim ;
extern int TgtIndex[MAXDELIM];
extern int nTgtIndex;


struct StructHist {
  int nxbin;
  float xmin;
  float xmax;
  int nybin;
  float ymin;
  float ymax;
};

// global constants
extern const float MSIZE;

// global declarations
extern StructHist Eslope;

// Direcotories
extern TDirectory * Run;
extern TDirectory * Raw;
extern TDirectory * FeedBack;
extern TDirectory * Kinema;
extern TDirectory * Bunch;
extern TDirectory * ErrDet;
extern TDirectory * Asymmetry;


//
//  Histogram Definitions 
//
//  Number arrays are TOT_WFD_CH, not NSTRIP, because there are events with strip>72,73,74,75
//  in Run06 which are target events. These histograms are deleted before ROOT file closing 
//  anyway though, need to be declared to aviod crash in histogram filling rouitne in process_event()
//
// Run Dir
extern TH2F * rate_vs_delim;

// FeedBack Dir
extern TH2F  * mdev_feedback;
extern TH1F  * mass_feedback[TOT_WFD_CH];   // invariant mass for feedback 

// Raw Directory
extern TH1F * bunch_dist_raw;              // counts per bunch (raw)
extern TH1F * strip_dist_raw;              // counts per strip (raw)
extern TH1F * tdc_raw;                     // tdc (raw)
extern TH1F * adc_raw;                     // adc (raw)
extern TH2F * tdc_vs_adc_raw;              // tdc vs. adc (raw)
extern TH2F * tdc_vs_adc_false_bunch_raw;  // tdc vs. adc (raw) for false bunch

// Kinema Dir
extern TH2F  * t_vs_e[TOT_WFD_CH];          // t vs. 12C Kinetic Energy (banana with/o cut)
extern TH2F  * t_vs_e_yescut[TOT_WFD_CH];   // t vs. 12C Kinetic Energy (banana with cut)
extern TH2F  * mass_vs_e_ecut[TOT_WFD_CH];  // Mass vs. 12C Kinetic Energy 
extern TF1   * banana_cut_l[NSTRIP][2];     // banana cut low     [0]: regular [1] alternative sigma cut
extern TF1   * banana_cut_h[NSTRIP][2];     // banana cut high    [0]: regular [1] alternative sigma cut
extern TLine * energy_cut_l[NSTRIP];        // energy cut low 
extern TLine * energy_cut_h[NSTRIP];        // energy cut high
extern TH1F  * energy_spectrum[NDETECTOR];  // energy spectrum per detector
extern TH1F  * energy_spectrum_all;         // energy spectrum for all detector sum
extern TH1F  * mass_nocut[TOT_WFD_CH];      // invariant mass without banana cut
extern TH1F  * mass_yescut[TOT_WFD_CH];     // invariant mass with banana cut


// Bunch Distribution
extern TH1F * bunch_dist;                  // counts per bunch
extern TH1F * wall_current_monitor;        // wall current monitor
extern TH1F * specific_luminosity;         // specific luminosity

// ErrDet dir
extern TH2F * mass_chi2_vs_strip;          // Mass Gaussian fit chi2 vs. strip 
extern TH2F * mass_sigma_vs_strip;         // Mass sigma width vs. strip 
extern TH2F * mass_e_correlation_strip;    // Mass-energy correlation vs. strip
extern TH2F * mass_pos_dev_vs_strip;       // Mass position deviation vs. strip
extern TH1I * good_carbon_events_strip;    // number of good carbon events per strip
extern TH2F * spelumi_vs_bunch;                    // Specific Luminosity vs. bunch
extern TH1F * bunch_spelumi;                       // Specific Luminosity bunch hisogram
extern TH1F * asym_bunch_x45;                      // Bunch asymmetry histogram for x45 
extern TH1F * asym_bunch_x90;                      // Bunch asymmetry histogram for x90 
extern TH1F * asym_bunch_y45;                      // Bunch asymmetry histogram for y45 

// Asymmetry dir
extern TH2F * asym_vs_bunch_x45;                   // Asymmetry vs. bunch (x45)
extern TH2F * asym_vs_bunch_x90;                   // Asymmetry vs. bunch (x90)
extern TH2F * asym_vs_bunch_y45;                   // Asymmetry vs. bunch (y45)
extern TH2F * asym_sinphi_fit;                     // strip asymmetry and sin(phi) fit 
extern TH2F * scan_asym_sinphi_fit;                // scan asymmetry and sin(phi) fit 

#endif
