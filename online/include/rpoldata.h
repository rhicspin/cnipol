#ifndef POLDATA_H
#define POLDATA_H

#include <time.h>

#include "TBuffer.h"

//#include "globals.h"
#include "rhicpol.h"

#define BSIZE 0x40000                   // 256 k

#define REC_BEGIN           0x00000001      // Begin of the file
#define REC_SUBRUN          0x00000002      // Begin of subrun
#define REC_POLADO          0x00000101      // Polarimeter ADO record
#define REC_TAGADO          0x00000102      // Target ADO record
#define REC_BEAMADO         0x00000103      // Beam ADO record
#define REC_WCMADO          0x00000104      // Wall current monitor ADO record
#define REC_WCM_NEW         0x00000109      // Wall current monitor ADO record
#define REC_HJPOSADO        0x00000105      // Beam position and HJET position ADO record
#define REC_TAGMOVEADO      0x00000106      // Target movement history ADO record
#define REC_HJCARBT         0x00000107      // Carbon polarimeter target status in JET data runs
#define REC_V124            0x00000108      // V124 parameters
#define REC_CONFIG          0x00000111      // obsolete
//#define REC_RHIC_CONF       0x00000112    // Instead of REC_CONFIG since Apr2003
#define REC_RHIC_CONF       0x00000113      // New parameters added, but I don't want to change the name. 14/04/04
#define REC_MEASTYPE        0x00000120
#define REC_VOLTAGE         0x00000121
#define REC_MACHINEPARAMS   0x00000121      // 197MHz cavity voltage, snake and rotator currents
//#define REC_READRAW         0x00000201
//#define REC_READSUB         0x00000202
//#define REC_READALL         0x00000203
//#define REC_READAT          0x00000211
#define REC_READRAW         0x00000221      // New parameters added, but I don't want to change the name. 14/04/04
#define REC_READSUB         0x00000222      // New parameters added, but I don't want to change the name. 14/04/04
#define REC_READALL         0x00000223      // New parameters added, but I don't want to change the name. 14/04/04
#define REC_READAT          0x00000231      // New parameters added, but I don't want to change the name. 14/04/04
#define REC_READLONG        0x00000241      // Long waveform
#define REC_WFDSCAL         0x00000301      // obsolete
#define REC_HISTAT          0x00000302      // obsolete
#define REC_WFDV8SCAL       0x00000303
#define REC_SCALERS         0x00000401
#define REC_HJETSWITCH      0x00000501      // jet switch record
#define REC_PCTARGET        0x00000502      // target history record
#define REC_COUNTRATE       0x00000503      // count rate history record
#define REC_END             0x00000999
#define REC_TYPEMASK        0x00007FFF
#define REC_MASK_BEAM       0x00030000
#define REC_YELLOW          0x00010000 // or'ed with the type
#define REC_BLUE            0x00020000 // or'ed with the type
#define REC_MASK_STREAM     0x00300000
#define REC_UPSTREAM        0x00100000 // or'ed with the type
#define REC_DOWNSTREAM      0x00200000 // or'ed with the type
#define REC_JET             0x00080000 // or'ed with the type and ring
#define REC_120BUNCH        0x00008000 // or'ed with the mask where applicable
#define REC_FROMMEMORY      0x00040000 // or'ed with the mask where applicable


enum EMeasType {kMEASTYPE_UNKNOWN     = 0x00000000,
                kMEASTYPE_UNDEF       = 0x11111111,
                kMEASTYPE_TEST        = 0x00010000,
                kMEASTYPE_ALPHA       = 0x00000001,
                kMEASTYPE_SWEEP       = 0x00000002,  // profile sweep
                kMEASTYPE_FIXED       = 0x00000004,
                kMEASTYPE_RAMP        = 0x00000008,
                kMEASTYPE_EMIT_SCAN   = 0x00000010,
                kMEASTYPE_TARGET_SCAN = 0x00000020,
                kMEASTYPE_PROFILE_T   = 0x00000040,  // profile by time
                kMEASTYPE_PROFILE_E   = 0x00000080}; // profile events

typedef struct {
    long len;           // total length = header size + data size
    long type;          // record type, see above
    long num;           // record number
    union {
        time_t time;    // local UNIX time in most cases
        long delim;     // when read from memory
    } timestamp;
} recordHeaderStruct;

typedef struct {
    recordHeaderStruct header;
    long version;
    char comment[256];
} recordBeginStruct;

typedef struct {
    recordHeaderStruct header;
    EMeasType          type;
} recordMeasTypeStruct;

struct RecordMachineParams {
    recordHeaderStruct header;
    Int_t   fCavity197MHzVoltage[N_BEAMS];
    Float_t fSnakeCurrents[N_BEAMS];
    Float_t fStarRotatorCurrents[N_BEAMS];
    Float_t fPhenixRotatorCurrents[N_BEAMS];
};

typedef struct {
    recordHeaderStruct header;
    long subrun;
} recordSubrunStruct;


typedef struct {
    recordHeaderStruct header;
    polDataStruct data;
} recordPolAdoStruct;

typedef struct {
    recordHeaderStruct header;
    unsigned short Num;         // sequential number of the state
// we put here raw data read from our LC4448 latch register
    unsigned short data[3];     // just 3 16-bit words
    long reserved[16];          // just for future extensions
} recordHJetSwitchStruct;

typedef struct {
    int good;                   // 1 if all targets out of beam, zero otherwise
    char carbtarg[8][10];       // 8 carbon targets positions
} carbTargStat;

typedef struct {
    recordHeaderStruct header;
    carbTargStat targstat;
} recordHJetCarbTargStruct;

typedef struct {
    recordHeaderStruct header;
    targetDataStruct data[2];
} recordTagAdoStruct;

typedef struct {
    recordHeaderStruct header;
    pCTargetStruct data;
} recordpCTagAdoStruct;

typedef struct {
    recordHeaderStruct header;
    long data[];
} recordCountRate;

typedef struct {
    recordHeaderStruct header;
    beamDataStruct data;
} recordBeamAdoStruct;

typedef struct {
    recordHeaderStruct header;
    wcmDataStruct data;
} recordWcmAdoStruct;

struct RecordWcm
{
   recordHeaderStruct header;
   Float_t  fFillDataM[360];      // like in wcmDataStruct
   Float_t  fWcmBeamM;            // like in wcmDataStruct
   Double_t fBunchDataM[360];
   Double_t fBunchLengthM;
   Double_t fProfileHeaderM[32];
   //UChar_t* fProfileDataM;

	RecordWcm() : fFillDataM(), fWcmBeamM(0), fBunchDataM(), fBunchLengthM(0),
      fProfileHeaderM()//, fProfileDataM(0)
   {
      memset(fFillDataM,      0, 360*sizeof(Float_t));
      memset(fBunchDataM,     0, 360*sizeof(Double_t));
      memset(fProfileHeaderM, 0, 32 *sizeof(Double_t));
   }

   void Print()
   {
      for (short i=0; i<360; i++) printf("fBunchDataM[%02d]: %8.5f\n", i, fBunchDataM[i]);
      for (short i=0; i<32;  i++) printf("fProfileHeaderM[%02d]: %8.5f\n", i, fProfileHeaderM[i]);
   }
};

typedef struct {
    recordHeaderStruct header;
    V124Struct v124;
} recordV124Struct;


typedef struct TRecordConfigRhicStruct {
   recordHeaderStruct header;
   configRhicDataStruct data;
   SiChanStruct chanconf[1];   // chanconf[data.NumChannels-1]
                               // Actual length is defined from the record
                               // length or from the NumChannels member

   void Print(const Option_t* opt="") const;
   void Streamer(TBuffer &buf);

} recordConfigRhicStruct;

TBuffer & operator<<(TBuffer &buf, TRecordConfigRhicStruct *&rec);
TBuffer & operator>>(TBuffer &buf, TRecordConfigRhicStruct *&rec);

// New data structures for the V9 version, but keeping the same names

typedef struct {
    unsigned char a;      // amplitude
    unsigned char t;      // time (double resolution)
    unsigned char s;      // square (integral)
    unsigned char tmax;   // time of the maximum (single resoulution)
    unsigned char b:7;    // bunch number
    unsigned char rev0:1; // the LSB of revolution counter
    unsigned char rev;    // next 8 bits of revolution counter
} ATStruct;

typedef struct {
    unsigned char d[90];// waveform itself
    unsigned char b:7;  // bunch number
    unsigned char rev0:1; // the LSB of revolution counter
    unsigned char rev[4];       // nex 32 bits of revolution counter
    unsigned char dummy;
} waveformStruct;       // This is exactly as it comes from WFD,

typedef struct {
    unsigned char d[42];  // waveform itself
    unsigned char b:7;    // bunch number
    unsigned char rev0:1; // the LSB of revolution counter
    unsigned char rev[4]; // nex 32 bits of revolution counter
    unsigned char dummy;
} waveform120Struct;      // For 120 bunch mode the format from WFD is the same
                          // as for 60 bunch mode, bt it's not goog to store
                          // 48 dummy zeroes, so this is packed

typedef struct {
    unsigned char d[90];// waveform itself
    unsigned char a;    // amplitude
    unsigned char t;    // time (double resolution)
    unsigned char s;    // square (integral)
    unsigned char tmax; // time of the maximum (single resoulution)
    unsigned char b:7;  // bunch number
    unsigned char rev0:1; // the LSB of revolution counter
    unsigned char rev;  // next 8 bits of revolution counter
} ALLModeStruct;

typedef struct {
    unsigned char d[42];// waveform itself
    unsigned char a;    // amplitude
    unsigned char t;    // time (double resolution)
    unsigned char s;    // square (integral)
    unsigned char tmax; // time of the maximum (single resoulution)
    unsigned char b:7;  // bunch number
    unsigned char rev0:1; // the LSB of revolution counter
    unsigned char rev;  // next 8 bits of revolution counter
} ALLMode120Struct;

typedef struct {
    unsigned char length; // total waveform length in 70 MHz clocks (6 points)
    unsigned char before; // length in 70 MHz clocks (6 points) before trigger
    unsigned char trt:4;  // 4 bit trigger time from bunch pulse in 70 MHz clocks
    unsigned char bl:4;   // bunch number
    unsigned char bh:3;   // bunch number
    unsigned char revl:5; // revolution number
    unsigned char revm;   // revolution number
    unsigned char revh;   // revolution number
    unsigned char d[1];   // actual length of data is <length>*6
} longWaveStruct;

/*
    All data structures have the following format:
*/

typedef struct {
    recordHeaderStruct header;
    unsigned char rec[1];       // variable length records with data of the same type
                                // from different strips
} recordDataStruct;

/*  The number of the records could be determined from the total
    length (header.len) and each record length */

typedef struct {                // each record has its subhead
    unsigned siNum:8;           // silicon number
    unsigned Events:24;         // number events-1, so that 0 designates 1 event
} subheadStruct;                // the length of the record is determined from
                                // number of events

/*
    List of possible records is here. type of record is determined from
    header.type field
*/

typedef struct {
    subheadStruct subhead;
    waveformStruct data[1];
} recordReadWaveStruct;

typedef struct {
    subheadStruct subhead;
    char data[1];               // we don't know event length !!!
} recordLongWaveStruct;

typedef struct {
    subheadStruct subhead;
    waveform120Struct data[1];
} recordReadWave120Struct;

typedef struct {
    subheadStruct subhead;
    ALLModeStruct data[1];
} recordALLModeStruct;

typedef struct {
    subheadStruct subhead;
    ALLMode120Struct data[1];
} recordALLMode120Struct;

typedef struct {
    subheadStruct subhead;
    ATStruct data[1];
} recordReadATStruct;

typedef struct {                        // obsolete
    recordHeaderStruct header;
    int siNum;                  // silicon number
    unsigned short csr;         // F1A1
    unsigned short win;         // F1A4
    long scalers[8];            // 8 internal scalers
    long hist[512];             // internal histogram memory.
//  Partitioning according header.type:
//      REC_WFDSCAL -   [0-59]          bunch C counts
//                      [64-127]        amplitude histogram, unpolarized
//                      [128-191]       amplitude histogram, positive
//                      [192-255]       amplitude histogram, negative
//                      [256-511]       amplitude vs time histogram:
//                      [8][32]         (8) amplitude slices of time
//      REC_HISTAT -    [16][32]        (16) amplitude slices of time
} recordWFDArrayStruct;

typedef struct {
    recordHeaderStruct header;
    int siNum;                  // silicon number
    unsigned short csr;         // F1A1 CSR5=0 --coarase, 1 --fine
    unsigned short win;         // F1A3 WIN[7:0]*2 is the lower hist limit in time
    unsigned short trg;         // F1A2 TRG[15:8] is the lower hist limit in ampl
                                // in fine mode, otherwise ll=0
    unsigned short dummy;       // just to aline
    long scalers[8];            // 8 internal scalers
    long hist[1536];            // internal histogram memory.
//  Partitioning according header.type:
//      REC_WFDSCAL -   [0-119]         bunch C(arbon?) counts (within look up table?)
//                      [128-255]       amplitude histogram, unpolarized
//                      [256-383]       amplitude histogram, positive
//                      [384-511]       amplitude histogram, negative
//                      [512-1536]      amplitude vs time histogram:
// either               [32][32]        coarse: 32 amplitude slices of time
//                      ampl BW=8, LL=0, time BW=2, LL=WIN[7:0]*2
// or                   [16][64]        fine: 16 amplitude slices of time
//                      ampl BW=8, LL=TRG[15:8], time BW=1, LL=WIN[7:0]*2
} recordWFDV8ArrayStruct;


typedef struct {
    recordHeaderStruct header;
    long long data[6];  // 6 48-bit scalers (module LeCroy 2551)
} recordScalersStruct;

typedef struct {
    recordHeaderStruct header;
    char comment[256];
} recordEndStruct;

typedef struct {
    recordHeaderStruct   header;
    char                 data[1];
} ReadBufferStruct;

struct SubRun {
    int nofsubruns;
    long timestamp[500];
    float asymX[500];
    float asymErrX[500];
    float asymX90[500];
    float asymErrX90[500];
    float asymX45[500];
    float asymErrX45[500];
};

#endif
