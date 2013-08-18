#ifndef POLADO_H
#define POLADO_H

#define VERSION         40200
#define DAQVERSION      40200
#define MAXSTATIONS     24

#define WFDMEMSIZE      0x4000000
#define MAXERR          10000
#define MAXNTEVENTS     2000000
#define V10BASELINE     8

#define SHELL           "/bin/bash"

#define N_BEAMS         2           // Number of beams/rings

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
#define CMD_EMIT        0x0300
#define CMD_TEST        0x0400
#define CMD_RAMP        0x0500
#define CMD_YELLOW      0x1000
#define CMD_BLUE        0x2000
#define CMD_DONE        0x4000

#define MAXTARGTIME             300     // sec, maximum moving time of the target
#define MAXANALYSISTIME         120     // sec
#define RTIMEPEREVENT           60.E-6  // sec/event for readout from memory
                                        // including that fact that memory has more events than scalers
#define JETWAIT4CARB            60      // seconds to wait in jet data runs for carbon polarimeter target retraction

#define CFG_INIT        0
#define CFG_UPDATE      1

#include <stdint.h>

#include "static_assert.h"

//extern char polCDEVName[4][20];// = {"polarimeter.blu1", "polarimeter.blu2", "polarimeter.yel1", "polarimeter.yel2"};

typedef struct {
    double runIdS;
    int32_t startTimeS;
    int32_t stopTimeS;
    char daqVersionS[80];
    char cutIdS[80];
    char targetIdS[80];
    int32_t encoderPositionS[2];
    int32_t statusS;
    char statusStringS[80];
    //
    int32_t totalCountsS;
    int32_t upCountsS;
    int32_t downCountsS;
    int32_t unpolCountsS;
    int32_t countsUpLeftS[360];
    int32_t countsLeftS[360];
    int32_t countsDownLeftS[360];
    int32_t countsDownRightS[360];
    int32_t countsRightS[360];
    int32_t countsUpRightS[360];
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
    int32_t numberEventsS;
    int32_t maxTimeS;
} polDataStruct;

typedef struct {
    int32_t movingM;
    int32_t positionEncM[2];
    char statusM[80];
    char positionS[80];
} targetDataStruct;


typedef struct{
    int32_t VertLinear ; // vertical target linear encorder
    int32_t VertRotary ; // vertical target rotary encorder
    int32_t HorLinear  ; // horizontal target linear encorder
    int32_t HorRotary  ; // horizontal target linear encorder
} pCTargetStruct;

#pragma pack(push)
#pragma pack(1)
typedef struct {
    double    beamEnergyM;
    int32_t   fillNumberM;
    int16_t   measuredFillPatternM[360];
    float     bunchLengthS;                  // half bunch length?
    float     synchrotronTuneS;
    int16_t   polarizationFillPatternS[360];
} beamDataStruct;
#pragma pack(pop)

typedef struct {
    float fillDataM[360];
    float wcmBeamM;
} wcmDataStruct;

typedef struct {
    int32_t jetAbsolutePosition;
    int32_t rbpm[8];
} jetPositionStruct;

typedef struct {
    uint16_t positionDelay;
    uint16_t flags;               // what parameters to be send to CDEV: 0x8000 - positionDelay, 8 ls-bits individual channels
    struct V124CHAN {
        uint16_t fineDelay;
        uint16_t pulseWidth;
        uint16_t bucketOffset;
        uint16_t bucketPeriod;
    } chan[8];
} V124Struct;

// Bits in output register. Only OUT_INHIBIT, OUT_DELIM and OUT_CLRLAM are
// set from code, others are taken from config file. Given here as a description
#define OUT_INHIBIT     0x0001  // inhibit to WFD fron panel LEMO
#define OUT_DELIM       0x0002  // delimiter to WFD fron panel LEMO
#define OUT_SWITCH      0x0004  // switch V124 blue/yellow - clock and bunchZ
#define OUT_MUXA        0x0008  // Old ATT1. Was moved by unknown reason
#define OUT_MUXB        0x0010  // Old ATT1. Was moved by unknown reason
#define OUT_CLRLAM      0x0020  // Clear LAM and Inhibit flip/Flop for HJET
#define OUT_ATT1        0x0040  // select attenuation : 00 - 1/10, 01 - 1/5
#define OUT_ATT2        0x0080  // 10 - 1/3, 11 - 1/1
#define OUT_INHPULSE    0x0100  // Inhibit test pulse goiung to preamps
#define OUT_TEST        0x0200  // set shaper board to test mode. NEVER used.

// masks for input register
#define JET_VETO        0x0001
#define JET_PLUS        0x0002
#define JET_MINUS       0x0004

typedef struct {
    int16_t CrateN;     // crate number
    int16_t CamacN;     // station number
    int VirtexN;        // Virtex number
    union {
        struct {
            unsigned char Beg;
            unsigned char End;
            unsigned short dummy;
        } split;
        int32_t reg;
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
    int32_t reserved[9];
} SiChanStruct;

#define CALIB_CONSTANTS 13

#define MOD_RAW         0x0000
#define MOD_SUB         0x0001
#define MOD_AT          0x0002
#define MOD_ALL         0x0003
#define MOD_MEM         0x0010
#define MOD_120         0x0400


typedef struct {
    int16_t CrSc;               // scaler module Crate
    int16_t NSc;                // scaler module N
    int16_t CrOut;              // output module Crate
    int16_t NOut;               // output module N
    int16_t CrIn;               // input  module Crate
    int16_t NIn;                // input  module N
    int16_t CrLam;              // LAM    module Crate
    int16_t NLam;               // LAM    module N
    int32_t DelimPrescaler;     // 78 KHz are prescaled with this number to make the delimeter
    float WFDTUnit;             // 1/6 70 MHz period, ns, 2*LSB time measurement
    float TOFLength;            // cm. 2010: Obsolete - moved to individual channels
    float AtomicNumber;         // just atomic number of the recoiled particle (carbon or proton)
    union {
        struct {
            uint32_t Mode:2;    // [1:0] 0-Raw, 1-SubMean, 2-AT, 3-ReadAll
            uint32_t iDiv:2;    // [3:2] Integral divider parameter
                                // Int=IntRead*2^(iDiv+2)
            uint32_t MemEn:1;   // [4] Output to memory enabled if 1
            uint32_t Mod2D:1;   // [5] 0-coarse, 1-fine (in t)
            uint32_t VirtN:2;   // [7:6] Channel number to appear in data stream to memory
            uint32_t Filter:1;  // [8] Enable 3pt filter, CFD threshold for v.10
            uint32_t MaxCor:1;  // [9] Enable maximum correction (no effect unless the tables are programmed)
            uint32_t B120:1;    // [10] Enable 120 bunch mode
            uint32_t G140:1;    // [11] Dummy in this design, enable trig. window for jet mode v. 10
            uint32_t RMemLUp:1; // [12] Selection of events to memory: 0-regular
                                // lookup, 1-rectangular lookup (ampl thresh=FineHistBeg,
                                // time thresh=Window.Beg)
            uint32_t RevDelim:1;// [13] = 0 for external, = 1 for revolution triggered delimeter
            uint32_t Hi2MemA:1; // [14] (RO) Histograms to memory procedure in progress
            uint32_t Delim:1;   // [15] (RO) Delimeter active
            uint32_t OvF:1;     // [16] (RO) FIFO Overflow
            uint32_t ScClr:1;   // [17] (RO) Scaler/Histogram clear in progress
            uint32_t IntOvF:1;  // [18] (RO) Integral dvided by 2^(iDiv+2) >255
            uint32_t ScOvF:1;   // [19] (RO) Scalers/histograms overflow (counters wrap)
            uint32_t dummy:3;
            uint32_t DLLNLock:1; // [23] (RO) DLL not locked
        } split;
        int32_t WFDMode;
        int32_t reg;
    } CSR;
    union {
        struct {
            uint8_t TrigThreshold; // In ampl channels, thresh. for baseline subtraction
            uint8_t FineHistBeg;   // In ampl channels, LL of A in AThist in fine mode
            uint16_t dummy;
        } split;
        int32_t reg;
    } TRG;
    char Pattern[120];      // Pattern: this is just for some crosscheck
                            // 0 - not filled;
                            // 1 - positive polarization;
                            // 2 - negative polarization.
                            // 3 - unpolarized;
//
    float Emin;         // LookUp table threshold, KeV
    float Emax;         // LookUp table Maximum, KeV
    int32_t ETLookUp;   // How to fill lookup tables: 0-banana, 1-rectangle,
                        // N>1-threshold=N in WFD ampl units
    int32_t IALookUp;   // 0-linear, 1-fully open
    int32_t BZDelay;    // delay of BZ pulse in 70 MHz clocks (1-14)
                        // Bzdelay is not necessary at RHIC, but may be usefull
    int32_t OnlyHist;   // If 1, no data is read out, only internal histograms
    float SleepFraction; // [0..1] - fraction of time to set inhibit to optimize memory usage
    float CicleTime;    // time period to test jet Veto
    float TrigMin;      // trigger level for internal 2-dim histogram and for rectangular cut for memory
    int32_t JetDelay;   // Value to be written into delay register of version 10
    int32_t FPGAVersion;// FPGA version. We don't support various versions in one run
    float TshiftLow;    // tshift for injection
    float TshiftHigh;   // tshift for flattop
    int32_t Dummy[33];  // For futher use to keep the whole length the same
    int32_t NumChannels; // number of silicon strips
    SiChanStruct chan[1];       // array of NumChannels size
} configRhicDataStruct;

void rhicpol_print_usage();
void rhicpol_process_options();

#ifndef __CINT__
STATIC_ASSERT(sizeof(polDataStruct) == 8 + 4*2 + 80*3 + 4*(2+1) + 80 + 4*(4+360*6) + 4*8 + 4*360*4 + 4*3 + 4*2, polDataStruct_size)
STATIC_ASSERT(sizeof(targetDataStruct) == 4*(1+2) + 80*2, targetDataStruct_size)
STATIC_ASSERT(sizeof(pCTargetStruct) == 4*4, pCTargetStruct_size)
STATIC_ASSERT(sizeof(beamDataStruct) == 8 + 4 + 2*360 + 4*2 + 2*360, beamDataStruct_size)
STATIC_ASSERT(sizeof(wcmDataStruct) == 4*360 + 4, wcmDataStruct_size)
STATIC_ASSERT(sizeof(jetPositionStruct) == 4*(1+8), jetPositionStruct_size)
STATIC_ASSERT(sizeof(V124Struct) == 2*2 + 2*4*8, V124Struct_size)
STATIC_ASSERT(sizeof(SiChanStruct) == 2*2 + 4 + 4 + 2*512 + 4*(10+5+1) + 4*9, SiChanStruct_size)
#define CRDS_PTR ((configRhicDataStruct*)0)
STATIC_ASSERT(sizeof(CRDS_PTR->CSR) == 4, configRhicDataStruct_CSR_size)
STATIC_ASSERT(sizeof(CRDS_PTR->TRG) == 4, configRhicDataStruct_TRG_size)
STATIC_ASSERT(sizeof(configRhicDataStruct) == 2*8 + 4 + 4*3 + sizeof(CRDS_PTR->CSR) + sizeof(CRDS_PTR->TRG) + 120 + 4*2 + 4*4 + 4*3 + 4*2 + 4*2 + 4*(33+1) + sizeof(SiChanStruct), configRhicDataStruct_size)
#undef CRDS_PTR
#endif

#endif
