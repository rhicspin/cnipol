#ifndef POLADO_H
#define POLADO_H

#define VERSION         20300
#define DAQVERSION      20309
#define MAXCRATES       4
#define MAXSTATIONS     24

#define WFDMEMSIZE      0x4000000
#define MAXERR          1000
#define MAXNTEVENTS     2000000

#define SHELL           "/bin/bash"
#define CAMAC_NAME      "/dev/cc32_%1.1d"
#define LOCK_FILENAME   "/tmp/rhicpol.lock"

#define STATUS_ERROR    0x80000000  // this bit indicates that data is not
                                    // valid. It should be explained by other
                                    // bit(s) if not some unknown program
                                    // error happened
#define ERR_BUSY        0x04000000  // device is busy (jet interlock)
#define ERR_TARGET      0x02000000  // target is not in or its status is bad
                                    // or both targets are in
#define ERR_FAILSTART   0x01000000
#define ERR_NOADO       0x00400000  // unable to connect to RHIC ADO
#define ERR_RING        0x00200000  // unable to switch rings
#define ERR_CONF        0x00100000
#define ERR_CAMAC       0x00080000  // unable to access CAMAC
#define ERR_WFD         0x00040000  // severe WFD internal error or unreliable readout
#define ERR_INT         0x00020000  // severe internal error, see logfile
#define ERR_TEST        0x00010000  // this is a test run not an actual
                                    // measurement
#define WARN_INT        0x00000001  // internal warning, nonfatal, see logfile
#define WARN_CANCELLED  0x00000002  // no timeadjustment run was done
#define WARN_NOBEAM     0x00000004  // no fill number is available
#define WARN_WFD        0x00000008  // internal warning, nonfatal, see logfile
#define WARN_CNTS       0x00000010  // small internal scalers/histograms inconsistency

#define CMD_STOP        0x0001
#define CMD_CAN         0x0002
#define CMD_START       0x0100
#define CMD_DATA        0x0200
#define CMD_TEST        0x0300
#define CMD_YELLOW      0x1000
#define CMD_BLUE        0x2000
#define CMD_DONE        0x4000

#define MAXTARGTIME             300     // sec, maximum moving time of the target
#define MAXANALYSISTIME         120     // sec
#define RTIMEPEREVENT           60.E-6  // sec/event for readout from memory
                                        // including that fact that memory has more events than scalers

#define CFG_INIT        0
#define CFG_UPDATE      1


char * stat2str(int stat);

typedef struct {
    double runIdS;
    long startTimeS;
    long stopTimeS;
    char daqVersionS[80];
    char cutIdS[80];
    char targetIdS[80];
    long encoderPositionS[2];
    long statusS;
    char statusStringS[80];
//
    long totalCountsS;
    long upCountsS;
    long downCountsS;
    long unpolCountsS;
    long countsUpLeftS[360];
    long countsLeftS[360];
    long countsDownLeftS[360];
    long countsDownRightS[360];
    long countsRightS[360];
    long countsUpRightS[360];
    float avgAsymXS;
    float avgAsymX45S;
    float avgAsymX90S;
    float avgAsymYS;
    float avgAsymErrorXS;
    float avgAsymErrorX45S;
    float avgAsymErrorX90S;
    float avgAsymErrorYS;
    float bunchAsymXS[360];
    float bunchAsymYS[360];
    float bunchAsymErrorXS[360];
    float bunchAsymErrorYS[360];
//
    float beamEnergyS;
    float analyzingPowerS;
    float analyzingPowerErrorS;
    long numberEventsS;
    long maxTimeS;
} polDataStruct;

typedef struct {
    long movingM;
    long positionEncM[2];
    char statusM[80];
    char positionS[80];
} targetDataStruct;


typedef struct{
    long VertLinear ; // vertical target linear encorder
    long VertRotary ; // vertical target rotary encorder
    long HorLinear  ; // horizontal target linear encorder
    long HorRotary  ; // horizontal target linear encorder
} pCTargetStruct ;

typedef struct {
    double beamEnergyM;
    long fillNumberM;
    short int measuredFillPatternM[360];
    float bunchLengthS;
    float synchrotronTuneS;
    short int polarizationFillPatternS[360];
} beamDataStruct;

typedef struct {
    float fillDataM[360];
    float wcmBeamM;
} wcmDataStruct;

// Bits in output register
#define OUT_SWITCH      0x0001
#define OUT_DELIM       0x0002
#define OUT_INHIBIT     0x0004
#define OUT_MUXA        0x0008
#define OUT_MUXB        0x0010
#define OUT_CLRLAM      0x0020
#define OUT_JETINH      0x0040
// values for OUT_SWITCH
#define OUT_YELLOW      1
#define OUT_BLUE        0

// masks for input register
#define JET_VETO        0x0008
#define JET_PLUS        0x0200
#define JET_MINUS       0x0400

typedef struct {
    short int CrateN;   // crate number
    short int CamacN;   // station number
    int VirtexN;        // Virtex number
    union {
        struct {
            unsigned char Beg;
            unsigned char End;
            unsigned short dummy;
        } split;
        long reg;
    } Window;           // Better to have it individually
    short unsigned int LookUp[512];
    float ETCutW;       // +- nanoseconds
    float IACutW;       // +- integral units
// form calibration file: t0 ec edead A0 A1 ealph dwidth pede C0 C1 C2 C3 C4
    float t0;           // nanoseconds
    float ecoef;        // keV/chan
    float edead;        // kev
                        // Energy = <edead> + <amplitude> * <ecoef>
    float A0;
    float A1;
                        // Int = <A0> + <A1> * Amp
    float acoef;        // keV Alpha source calibration constant
                        // Energy_DepositOnSi = acoef * <amplitude>
                        // New parameters from Osamu
    float dwidth;       // dead layer width ug/cm^2
    float pede;         // pedestal of unknown nature (unitS ?)
    float C[5];         // nonlinear fit parameters: Ein = C[0] + C[1]*<ADC> + C[2]*<ADC>^2 ...
    float TOFLength;    // cm. 2010: we now have it different per detector !
    long reserved[10];
} SiChanStruct;

#define CALIB_CONSTANTS 13

#define MOD_RAW         0x0000
#define MOD_SUB         0x0001
#define MOD_AT          0x0002
#define MOD_ALL         0x0003
#define MOD_MEM         0x0010
#define MOD_120         0x0400


typedef struct {
    short int CrSc;             // scaler module Crate
    short int NSc;              // scaler module N
    short int CrOut;            // output module Crate
    short int NOut;             // output module N
    short int CrIn;             // input  module Crate
    short int NIn;              // input  module N
    short int CrLam;            // LAM    module Crate
    short int NLam;             // LAM    module N
    int DelimPrescaler;         // 78 KHz are prescaled with this number to make the delimeter
    float WFDTUnit;             // 1/6 70 MHz period, ns, 2*LSB time measurement
    float TOFLength;            // cm
    float AtomicNumber;         // just atomic number of the recoiled particle (carbon or proton)
    union {
        struct {
            unsigned Mode:2;    // [1:0] 0-Raw, 1-SubMean, 2-AT, 3-ReadAll
            unsigned iDiv:2;    // [3:2] Integral divider parameter
                                // Int=IntRead*2^(iDiv+2)
            unsigned MemEn:1;   // [4] Output to memory enabled if 1
            unsigned Mod2D:1;   // [5] 0-coarse, 1-fine (in t)
            unsigned VirtN:2;   // [7:6] Channel number to appear in data stream to memory
            unsigned Filter:1;  // [8] Enable 3pt filter
            unsigned MaxCor:1;  // [9] Enable maximum correction (no effect unless the tables are programmed)
            unsigned B120:1;    // [10] Enable 120 bunch mode
            unsigned G140:1;    // [11] Dummy in this design
            unsigned RMemLUp:1; // [12] Selection of events to memory: 0-regular
                                // lookup, 1-rectangular lookup (ampl thresh=FineHistBeg,
                                // time thresh=Window.Beg)
            unsigned RevDelim:1;// [13] External (0)/ revolution (1) delimeter
            unsigned Hi2MemA:1; // [14] (RO) Histograms to memory procedure in progress
            unsigned Delim:1;   // [15] (RO) Delimeter active
            unsigned OvF:1;     // [16] (RO) FIFO Overflow
            unsigned ScClr:1;   // [17] (RO) Scaler/Histogram clear in progress
            unsigned IntOvF:1;  // [18] (RO) Integral dvided by 2^(iDiv+2) >255
            unsigned ScOvF:1;   // [19] (RO) Scalers/histograms overflow (counters wrap)
            unsigned dummy:3;
            unsigned DLLNLock:1; // [23] (RO) DLL not locked
        } split;
        long WFDMode;
        long reg;
    } CSR;
    union {
        struct {
            unsigned char TrigThreshold; // In ampl channels, thresh. for baseline subtraction
            unsigned char FineHistBeg;   // In ampl channels, LL of A in AThist in fine mode
            unsigned short dummy;
        } split;
        long reg;
    } TRG;
    char Pattern[120];      // Pattern: this is just for some crosscheck
                            // 0 - not filled;
                            // 1 - positive polarization;
                            // 2 - negative polarization.
                            // 3 - unpolarized;
//
    float Emin;         // LookUp table threshold, KeV
    float Emax;         // LookUp table Maximum, KeV
    int ETLookUp;       // How to fill lookup tables: 0-banana, 1-rectangle,
                        // N>1-threshold=N in WFD ampl units
    int IALookUp;       // 0-linear, 1-fully open
    int BZDelay;        // delay of BZ pulse in 70 MHz clocks (1-14)
                        // Bzdelay is not necessary at RHIC, but may be usefull
    int OnlyHist;       // If 1, no data is read out, only internal histograms
    float SleepFraction; // [0..1] - fraction of time to set inhibit to optimize memory usage
    float CicleTime;    // time period to test jet Veto
    int Dummy[38];      // For futher use to keep the whole length the same
    int NumChannels;            // number of silicon strips
    SiChanStruct chan[1];       // array of NumChannels size
} configRhicDataStruct;

#endif
