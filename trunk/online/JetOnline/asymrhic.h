// Header file for asymrhic 
// Author   : Osamu Jinnouchi    
// Creation : 4/4/2003         

#ifndef ASYM_STREAM_H
#define ASYM_STREAM_H


// about NULL variable
#ifdef NULL
#undef NULL
#endif
#define NULL 0

#define NWORDS_PAWC 10000000  // paw memory size 
#define RECLEN 1024
#define HMINENE 100.            // minimum energy for histograms 
#define HMAXENE 1400.         // maximum energy for histograms
#define NTLIMIT 100000000
#define RAMPTIME 350     // duration of the ramp measurement (sec)

// Constants assumption
// Carbon mass Mc: 11.18 GeV/c^2
// speed of light c: 3.00x10^8 m/s 
// distance target to detectors L: 25 cm
// t = L* sqrt(Mc/2E)
#define KINC 1182.

// prottype declaration
//#define f2cFortran

// whole info for one event
typedef struct {
    int stN;   // strip number
    int amp;   // amplitude of signal
    int tdc;   // time of flight
    int intg;  // integral of signal
    int bid;   // bunch crossing number
    long delim;
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
    // Cut condition parameter for E-T
    // polinominal function coeficients
    // p[0] + p[1]*x + p[2]*x^2 + ...+ p[5]*gcc^5
    float t0f[6];  // t0 as a function of Pbeam[GeV]
    float t0w[6];  // tof width as a function of Pbeam[GeV]
} cutparamStruct;

typedef struct {
    // Constraint parameter for Data processing 
    int enel; // lower kinetic energy threshold (keV) 
    int eneu; // upper kinetic energy threshold (keV)
    int widthl; // lower banana cut (ns)
    int widthu; // upper banana cut (ns)
    int CMODE; // Calibration mode 0:off 1:on
    int DMODE; // dead layer study mode 0:off 1:on
    int TMODE; // T0 study mode 0:off 1:on
    int AMODE; // A0,A1 study (signal Amp vs. Int) mode 0:off 1:on
    int BMODE; // create banana curve (E-T) plots 0:off 1:on
    int ZMODE; // with/out T0 subtraction 0:with 1:without
    int CFGMODE; // 1: provide cfg info from file  
    int MESSAGE; // message mode 1: exit just after run begin 
    int CBANANA; // constant width banana cut :1
    int UPDATE; // 1: keep update of the histogram
    int MMODE;   // mass mode 
    int NTMODE;  // if 1 store NTUPLE
    int READT0MODE; 
    int RAMPMODE;    // if 1 prepare the histograms for ramp measurement
    int WAVE_FIT_MODE;// if 1 do not FIT and output waveform itself (very fas mode same output with AT mode)
    int WAVE_OUT_MODE;// output of Waveform file
    int JET_EVENT_MODE;//output sorted ntuple file
    int RAW_EVENT_MODE;//output raw-typ ntuple
    int SELECT_CHAN;//select_chan
} datprocStruct; 

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
}

int readloop(void);

int hist_book(char *);
int hist_close(char *);
int read_param(char *);
int read_rebin(char *);

int spill_end(recordConfigRhicStruct *);
int sqass(float, float, float, float, float *, float *);
int event_process(processEvent *, recordConfigRhicStruct *);
int end_process(recordConfigRhicStruct *);

int printConfig(recordConfigRhicStruct *);
void readConfig(recordConfigRhicStruct *);
void readT0Config(recordConfigRhicStruct *);
float ekin(float, float);

// GLOBAL VARIABLES

extern int tshift;
extern int spinpat[120]; // spin pattern 120 bunches
extern int fillpat[120]; // spin pattern 120 bunches
extern float wcmdist[120];  // wall current monitor 120 bunches

extern long int Ncounts[6][120]; // counts 6detectors 120 bunches
extern long int NTcounts[6][120][6];  // counts 6detectors 120 bunches 6 tranges
extern long int NRcounts[6][120][RAMPTIME];// counts 6det 120bunch RAMPTIME sec

extern char datafile[256];   // data file name 
extern char t0_conf[256];           // update configuration file for T0 info
extern char conf_file[256];           // update configuration file for T0 info

extern cutparamStruct LookUpTable;  // look up table cut parameters 

extern long int Nevcut;   // number of events after 1st cut (whole data)
extern long int Nevtot;   // number of total events (whole data) 
extern long int Nread;    // real total events (completely everything)
extern int Nskip;         // number of events to be skipped in data process 

extern long int Ngood[120];   // number of evts after carbon cut 
extern long int Ntotal[120];  // number of evts before carbon cut 
extern long int Nback[120];   // number of evts below the curbon cut
extern datprocStruct dproc;
extern atdata_struct atdata;

#endif









