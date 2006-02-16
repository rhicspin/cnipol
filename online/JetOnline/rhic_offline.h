// Header file for Asymrhic.c 
// Author   : Osamu Jinnouchi    
// Creation : 4/4/2003           

#ifndef ASYM_OFFLINE_H
#define ASYM_OFFLINE_H

// about NULL variable
#ifdef NULL
#undef NULL
#endif
#define NULL 0

#define NWORDS_PAWC 10000000  // paw memory size 
#define HMINENE 100.            // minimum energy for histograms 
#define HMAXENE 1400.         // maximum energy for histograms
#define NTLIMIT 100000000
#define RAMPTIME 350     // duration of the ramp measurement (sec)

#define LWAN  78.9     // used in AN calculation 
#define UPAN  1206.0
#define BINAN 80    

// Constants assumption
// Carbon mass Mc: 11.18 GeV/c^2
// speed of light c: 3.00x10^8 m/s 
// distance target to detectors L: 25 cm
// t = L* sqrt(Mc/2E)
#define KINC 1182.

// prottype declaration
//#define f2cFortran

// ====================================================
//  Structure declaration
// ====================================================
typedef struct {
    int strip_no;   // strip number
    int amp;   // amplitude of signal
    int t_time;   // time of flight
    int square;  // integral of signal
    int bunch;   // bunch crossing number
    int our_bunchY;   // bunch crossing number
    int our_bunchB;   // bunch crossing number
    int WCMcount;   //fill count 
    int delim;
    //long delim;
    int dummy1;
    int dummy2;
    int tmax;
    int revolution;
    int rev1;
    int rev0;
    int geo;//Geo_channel
    int intg;
    double timeG;
//GetAts output
    int amp2;
    int t_time2;
    int tmax2;
    int square2;
    int region;
//Other
    double amp3;
    double t_time3;
    double tmax3;
    double square3;
//Energy, nseconds
    double Energy;
    double ToF;
    double ToF0;
    double InE;
    double AveE;
//Fitting result
    double f_amp;
    double f_t_time;
    double f_tmax;
    double f_square;
    int f_ndf;
    double f_chi;
    double f_cndf;
    int f_start;
    double f_stop;
    double f_tau;
    double f_n;
//Target info
    int hjet_count;
    int target_inf;
    double Mx;
    double Mp;
    double Theta0;
    double Theta;
} processWEvent;

typedef struct{
    double TargetPhys[32][4];
    double BeamPhys[32][4];
    double TargetLumi[32][4];
    double BeamLumi[32][4];
    double TargetAcc[32][4];
    double BeamAcc[32][4];
    double TargetErr[32][4];
    double BeamErr[32][4];

    double DoublePhys[32][4];
    double DoubleLumi[32][4];
    double DoubleAcc[32][4];
    double DoublePhysErr[32][4];
    double DoubleLumiErr[32][4];
    double DoubleBeamErr[32][4];
} processAsym;

typedef struct{
    long int count;
    int rawY[32][6][3][3];
    int BGrawY[32][6][3][3];
    int MMrawY[32][6][3][3];
    int rawB[32][6][3][3];
    int BGrawB[32][6][3][3];
    int MMrawB[32][6][3][3];
    int eachY[32][6][3][3];
    int BGeachY[32][6][3][3];
    int MMeachY[32][6][3][3];
    int eachB[32][6][3][3];
    int BGeachB[32][6][3][3];
    int MMeachB[32][6][3][3];
    int valanceY[32][6][3][3];
    int BGvalanceY[32][6][3][3];
    int MMvalanceY[32][6][3][3];
    int valanceB[32][6][3][3];
    int BGvalanceB[32][6][3][3];
    int MMvalanceB[32][6][3][3];
    int plus_c;
    int minus_c;
    int zero_c;
    int plus;
    int plus_f;
    int minus;
    int minus_f;
    int zero;
    int valance_check;
    double Energy_mean[32];
    double InEnergy_mean[32];
    int Evet_count[32];
    int FillPatY[120];
    int FillPatB[120];
    int PolPatY[120];
    int PolPatB[120];
} process8Elements;

typedef struct {
    int stN;   // strip number
    int amp;   // amplitude of signal
    int amp0;   // raw amplitude of signal for calibration
    int tdc;   // time of flight
    int tmax;  //max time
    int intg;  // integral of signal
    int geo;  //geometrical channel 
    int bid;   // bunch crossing number
    long delim;
    int rev0;
    int rev;
    double Mx;
    double Mp;
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
    int select_minY[32][6];//bin,si
    int select_maxY[32][6];//bin,si
    int select_minY2[32][6];//bin,si
    int select_maxY2[32][6];//bin,si
    int select_minB[32][6];//bin,si
    int select_maxB[32][6];//bin,si
    int select_minB2[32][6];//bin,si
    int select_maxB2[32][6];//bin,si
    int term;
} MxChan;

typedef struct {
    double Ebin[32];
}EnergyBin;

typedef struct {
    double offset[7];
    double slope[7];
}ThetaOffset;

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
    int BUNCHMODE; // create bunch by bunch info
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
    int LMODE;   // if 1 use Look Up Table for cut
    int WAVE_FIT_MODE;// if 1 do not FIT and output waveform itself (very fas mode same output with AT mode)
    int WAVE_OUT_MODE;// output of Waveform file
    int JET_EVENT_MODE;//output sorted ntuple file
    int RAW_EVENT_MODE;//output raw-typ ntuple
    int SELECT_CHAN;//waveform output for select_chan
    int BIN_SELECT;//Energy BIN select
    int ToF_W;//Banana Cut ToF width
    int beam;//beam selection 1== yellow 2== blue
} datprocStruct; 

typedef struct{
    double MAP_max_Am[96];
    int MAP_tmax_Am[96];
    //double MAP_Am[96][900];
    double MAP_Am[96][1];
    double MAP_max_Gd[96];
    int MAP_tmax_Gd[96];
    //double MAP_Gd[96][900];
    double MAP_Gd[96][1];
    double ratio_All[96];
    double ratio_Am[96];
    double ratio_Gd[96];
    double seppen;
} WaveFormMap;

typedef struct{
   double wf_tau[97];
   double wf_n[97];
} WaveForm2Par;


extern struct{
  int event;
  int charge[5];
  int peak[5];
  int tdc[5];
  int cfd[5];
}counter_;
extern "C" {

    void hhbookn_(int*, char*);
    void hhfn_(int*, float event[]);
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
    int PawInit(char *);
    int PawFill();
    int hist_close(char *);

#endif






