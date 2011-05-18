#ifndef AsymGlobals_h
#define AsymGlobals_h

#include <map>
#include <string>
#include <vector>

#include "TDirectory.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TLine.h"

#include "Asym.h"
#include "AsymHeader.h"
#include "AsymCalculator.h"

class  AsymDb;
class  AsymRoot;
class  AsymDbFile;
class  AnaInfo;
struct ErrorDetector;
struct atdata_struct;
class  DbEntry;
class  RunInfo;
class  RunConfig;
struct StructExtInput;
struct StructAverage;
struct StructHistStat;
struct StructFeedBack;
struct StructCounter;
struct StructCounterTgt;
class  RunConst;
struct StructRunConst;
struct StructMask;
struct StructFlag;
struct StructReadFlag;
class  AnaResult;
class  TargetInfo;
struct StructBunchPattern;
struct StructHist;
struct StructSpeLumi;
struct AsymCalculator;
struct BunchAsym;
struct StructStripCheck;
struct StructBunchCheck;

extern std::map<std::string, std::string> gAsymEnv;

extern BunchAsym        gBunchAsym;
extern StructSpeLumi    SpeLumi;
extern AsymCalculator   gAsymCalculator;
extern StructStripCheck strpchk;
extern StructBunchCheck bnchchk;

// global constants
extern const int ASYM_DEFAULT;
extern const float MSIZE; // marker size

extern int   gSpinPattern[N_BUNCHES]; // spin pattern 120 bunches
extern int   gFillPattern[N_BUNCHES]; // fill pattern 120 bunches
extern int   ActiveBunch[N_BUNCHES];
extern int   wcmfillpat[N_BUNCHES];   // fill pattern within the Wall Current Monitor Average Ragne 

extern long int Ncounts[N_DETECTORS][N_BUNCHES];          // counts per detector per bunch
extern long int NTcounts[N_DETECTORS][N_BUNCHES][NTBIN];  // counts 6detectors 120 bunches 6 tranges
extern long int NDcounts[N_DETECTORS][N_BUNCHES][MAXDELIM];  // counts 6detectors 120 bunches per delimiter
extern long int NStrip[NUM_SPIN_STATES][N_CHANNELS]; // counts 72 strips 3 spin states
 
extern long int NRcounts[N_DETECTORS][N_BUNCHES][RAMPTIME]; // counts 6det 120bunch RAMPTIME sec

extern char *confdir;
extern char *calibdir;
//extern std::string gDataFileName;   // data file name 
extern char reConfFile[256];         // update configuration file for T0 info
extern char conf_file[256];          // update configuration file for T0 info
extern char CalibFile[256];          // Energy calibration file

extern float ramptshift[500]; // ramp timing shift 

extern long int Nevcut;         // number of events after 1st cut (whole data)
extern UInt_t   Nevtot;         // number of total events (whole data) 
extern UInt_t   Nread;          // actually read number of events
extern UInt_t   gMaxEventsUser; // number of events to process
//extern int      Nskip;          // number of events to be skipped in data process 
extern int      NFilledBunch;   // number of filled bunch

extern long int Ngood[N_BUNCHES];   // number of evts after carbon cut 
extern long int Ntotal[N_BUNCHES];  // number of evts before carbon cut 
extern long int Nback[N_BUNCHES];   // number of evts below the curbon cut

extern AnaInfo                      *gAnaInfo;
extern RunInfo                      *gRunInfo;
extern AnaResult                    *gAnaResult;
extern AsymRoot                     *gAsymRoot;
extern AsymDb                       *gAsymDb;
extern AsymDb                       *gAsymDb2;
extern ErrorDetector                 errdet;
extern atdata_struct                 atdata;
extern DbEntry                       gRunDb;
extern RunConfig                     gRunConfig;
extern StructExtInput                extinput;
extern StructAverage                 average;
extern StructHistStat                gHstat;
extern StructFeedBack                feedback;
extern StructCounter                 cntr;
extern StructCounterTgt              cntr_tgt;
//extern RunConst                    gRunConst;
extern std::map<UShort_t, RunConst>  gRunConsts;
extern StructMask                    mask;
extern StructFlag                    Flag;
extern StructReadFlag                gReadFlag;
extern TargetInfo                    tgt;
extern StructBunchPattern            phx, str;
extern TRecordConfigRhicStruct      *gConfigInfo;

extern float phiRun5[N_CHANNELS];   // phi-angle for each strips of Run5 (l=18.5cm)
extern float phiRun6[N_CHANNELS];   // phi-angle for each strips of Run6 (l=18.0cm)
extern float phit[N_CHANNELS];      // phi-angle for each strips in approximation 45,90,135... 
extern float gPhi[N_CHANNELS];      // phi-angle

// target position infomation 
extern int gNDelimeters;
extern int TgtIndex[MAXDELIM];
extern int nTgtIndex;

// global declarations
extern StructHist Eslope;

// Direcotories
extern TDirectory *Raw;
extern TDirectory *FeedBack;
extern TDirectory *Kinema;
extern TDirectory *ErrDet;
extern TDirectory *Asymmetry;

//  Histogram Definitions 
//
//  Number arrays are TOT_WFD_CH, not N_CHANNELS, because there are events with strip>72,73,74,75
//  in Run06 which are target events. These histograms are deleted before ROOT file closing 
//  anyway though, need to be declared to aviod crash in histogram filling rouitne in process_event()

// FeedBack Dir
extern TH2F *mdev_feedback;
extern TH1F *mass_feedback_all;
extern TH1F *mass_feedback[TOT_WFD_CH];   // invariant mass for feedback 

// Kinema Dir
extern TH2F  *t_vs_e[TOT_WFD_CH];         // t vs. 12C Kinetic Energy (banana with/o cut)
extern TH2F  *t_vs_e_yescut[TOT_WFD_CH];  // t vs. 12C Kinetic Energy (banana with cut)
extern TH2F  *mass_vs_e_ecut[TOT_WFD_CH]; // Mass vs. 12C Kinetic Energy 
extern TF1   *banana_cut_l[N_CHANNELS][2];    // banana cut low     [0]: regular [1] alternative sigma cut
extern TF1   *banana_cut_h[N_CHANNELS][2];    // banana cut high    [0]: regular [1] alternative sigma cut
extern TLine *energy_cut_l[N_CHANNELS];       // energy cut low 
extern TLine *energy_cut_h[N_CHANNELS];       // energy cut high
extern TH1F  *energy_spectrum[N_DETECTORS]; // energy spectrum per detector
extern TH1F  *energy_spectrum_all;        // energy spectrum for all detector sum
extern TH1F  *mass_nocut_all;
extern TH1F  *mass_nocut[TOT_WFD_CH];     // invariant mass without banana cut
extern TH1F  *mass_yescut_all;
extern TH1F  *mass_yescut[TOT_WFD_CH];    // invariant mass with banana cut

// ErrDet dir
extern TH2F *mass_chi2_vs_strip;          // Mass Gaussian fit chi2 vs. strip 
extern TH2F *mass_sigma_vs_strip;         // Mass sigma width vs. strip 
extern TH2F *mass_e_correlation_strip;    // Mass-energy correlation vs. strip
extern TH2F *mass_pos_dev_vs_strip;       // Mass position deviation vs. strip
extern TH1I *good_carbon_events_strip;    // number of good carbon events per strip
extern TH2F *spelumi_vs_bunch;            // Specific Luminosity vs. bunch
extern TH1F *bunch_spelumi;               // Specific Luminosity bunch hisogram
extern TH1F *asym_bunch_x45;              // Bunch asymmetry histogram for x45 
extern TH1F *asym_bunch_x90;              // Bunch asymmetry histogram for x90 
extern TH1F *asym_bunch_y45;              // Bunch asymmetry histogram for y45 

// Asymmetry dir
extern TH2F *asym_vs_bunch_x45;           // Asymmetry vs. bunch (x45)
extern TH2F *asym_vs_bunch_x90;           // Asymmetry vs. bunch (x90)
extern TH2F *asym_vs_bunch_y45;           // Asymmetry vs. bunch (y45)
extern TH2F *asym_sinphi_fit;             // strip asymmetry and sin(phi) fit 
extern TH2F *scan_asym_sinphi_fit;        // scan asymmetry and sin(phi) fit 

#endif
