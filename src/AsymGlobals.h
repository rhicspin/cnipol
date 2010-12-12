#ifndef AsymGlobals_h
#define AsymGlobals_h

#include <map>
#include <string>

#include "TDirectory.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TLine.h"

#include "AsymHeader.h"
#include "Asym.h"

extern std::map<std::string, std::string> gAsymEnv;

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

extern long int Nevcut;         // number of events after 1st cut (whole data)
extern UInt_t   Nevtot;         // number of total events (whole data) 
extern UInt_t   Nread;          // actually read number of events
extern UInt_t   gMaxEventsUser; // number of events to process
//extern int      Nskip;          // number of events to be skipped in data process 
extern int      NFilledBunch;   // number of filled bunch

extern long int Ngood[120];   // number of evts after carbon cut 
extern long int Ntotal[120];  // number of evts before carbon cut 
extern long int Nback[120];   // number of evts below the curbon cut
extern TDatprocStruct      dproc;
extern ErrorDetector       errdet;
extern atdata_struct       atdata;
extern TStructRunDB        rundb;
extern TStructRunInfo      runinfo;
extern StructExtInput      extinput;
extern StructAverage       average;
extern StructHistStat      hstat;
extern StructFeedBack      feedback;
extern StructCounter       cntr;
extern StructCounterTgt    cntr_tgt;
extern StructRunConst      runconst;
extern StructMask          mask;
extern StructFlag          Flag;
extern StructReadFlag      ReadFlag;
extern StructAnalysis      anal;
extern StructTarget        tgt;
extern StructBunchPattern  phx, str;
extern recordConfigRhicStruct *cfginfo;

extern float phiRun5[NSTRIP];   // phi-angle for each strips of Run5 (l=18.5cm)
extern float phiRun6[NSTRIP];   // phi-angle for each strips of Run6 (l=18.0cm)
extern float phit[NSTRIP];      // phi-angle for each strips in approximation 45,90,135... 
extern float phi[NSTRIP];       // phi-angle

// target position infomation 
extern int ndelim;
extern int TgtIndex[MAXDELIM];
extern int nTgtIndex;

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
