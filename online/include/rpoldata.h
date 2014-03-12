#ifndef POLDATA_H
#define POLDATA_H

#include <stdint.h>

#include "TBuffer.h"

#include "rhicpol.h"
#include "static_assert.h"

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
#define REC_MACHINEPARAMS_X 0x00000121      // old version. there was a problem with how the cavity voltage was saved
#define REC_MACHINEPARAMS   0x00000122      // 197MHz cavity voltage, snake and rotator currents
#define REC_TARGETPARAMS    0x00000130      // target start, end positions, velocity
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


struct RecordHeaderStruct
{
   int32_t len;           // total length = header size + data size
   int32_t type;          // record type, see above
   int32_t num;           // record number
   union {
       int32_t time;      // local UNIX time in most cases
       int32_t delim;     // when read from memory
   } timestamp;

   RecordHeaderStruct & operator=(const RecordHeaderStruct &rec);
   void Streamer(TBuffer &buf);

};


typedef struct
{
   int32_t len;           // total length = header size + data size
   int32_t type;          // record type, see above
   int32_t num;           // record number
   union {
       int32_t time;      // local UNIX time in most cases
       int32_t delim;     // when read from memory
   } timestamp;

} recordHeaderStruct;


typedef struct
{
    recordHeaderStruct header;
    int32_t version;
    char comment[256];
} recordBeginStruct;


typedef struct
{
    recordHeaderStruct header;
    EMeasType          type;
} recordMeasTypeStruct;


struct RecordMachineParams
{
   RecordHeaderStruct header;
   Int_t   fCavity197MHzVoltage[N_BEAMS];
   Float_t fSnakeCurrents[N_BEAMS];
   Float_t fStarRotatorCurrents[N_BEAMS];
   Float_t fPhenixRotatorCurrents[N_BEAMS];

   RecordMachineParams & operator=(const RecordMachineParams &rec);
   void Streamer(TBuffer &buf);
};


typedef struct {
    recordHeaderStruct header;
    int32_t subrun;
} recordSubrunStruct;


typedef struct {
    recordHeaderStruct header;
    polDataStruct data;
} recordPolAdoStruct;

struct RecordTargetParams
{
   RecordHeaderStruct header;
   Int_t fVelocity;
   Int_t fProfileStartPosition;
   Int_t fProfileEndPosition;
   Int_t fProfilePeakPosition;

   RecordTargetParams& operator=(const RecordTargetParams &rec);
   void Streamer(TBuffer &buf);
};

typedef struct {
    recordHeaderStruct header;
    uint16_t Num;         // sequential number of the state
// we put here raw data read from our LC4448 latch register
    uint16_t data[3];     // just 3 16-bit words
    int32_t reserved[16];          // just for future extensions
} recordHJetSwitchStruct;

typedef struct {
    int32_t good;                   // 1 if all targets out of beam, zero otherwise
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
    pCTargetStruct data[];
} recordpCTagAdoStruct;

typedef struct {
    recordHeaderStruct header;
    int32_t data[];
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
   RecordHeaderStruct header;
   Float_t  fFillDataM[360];      // like in wcmDataStruct
   Float_t  fWcmBeamM;            // like in wcmDataStruct
   Double_t fBunchDataM[360];
   Double_t fBunchLengthM;
   Double_t fProfileHeaderM[32];
   //UChar_t* fProfileDataM;

   RecordWcm();
   RecordWcm& operator=(const RecordWcm &rec);
   void Print();
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
    uint32_t siNum:8;           // silicon number
    uint32_t Events:24;         // number events-1, so that 0 designates 1 event
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
    int32_t siNum;                  // silicon number
    uint16_t csr;                  // F1A1
    uint16_t win;                  // F1A4
    int32_t scalers[8];            // 8 internal scalers
    int32_t hist[512];             // internal histogram memory.
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
    int32_t siNum;                  // silicon number
    uint16_t csr;               // F1A1 CSR5=0 --coarase, 1 --fine
    uint16_t win;               // F1A3 WIN[7:0]*2 is the lower hist limit in time
    uint16_t trg;               // F1A2 TRG[15:8] is the lower hist limit in ampl
                                // in fine mode, otherwise ll=0
    uint16_t dummy;       // just to aline
    int32_t scalers[8];            // 8 internal scalers
    int32_t hist[1536];            // internal histogram memory.
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
    int64_t data[6];  // 6 48-bit scalers (module LeCroy 2551)
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
    int32_t nofsubruns;
    int32_t timestamp[500];
    float asymX[500];
    float asymErrX[500];
    float asymX90[500];
    float asymErrX90[500];
    float asymX45[500];
    float asymErrX45[500];
};

#ifndef __CINT__
STATIC_ASSERT(sizeof(RecordHeaderStruct) == 16, RecordHeaderStruct_size)
STATIC_ASSERT(sizeof(recordHeaderStruct) == 16, recordHeaderStruct_size)
STATIC_ASSERT(sizeof(recordBeginStruct) == sizeof(recordHeaderStruct) + 4 + 256, recordBeginStruct_size)
STATIC_ASSERT(sizeof(recordMeasTypeStruct) == sizeof(recordHeaderStruct) + 4, recordMeasTypeStruct_size)
STATIC_ASSERT(sizeof(recordSubrunStruct) == sizeof(recordHeaderStruct) + 4, recordSubrunStruct_size)
STATIC_ASSERT(sizeof(subheadStruct) == 4, subheadStruct_size)
STATIC_ASSERT(sizeof(recordHJetSwitchStruct) == sizeof(recordHeaderStruct) + 2 + 2*3 + 4*16, recordHJetSwitchStruct_size)
STATIC_ASSERT(sizeof(carbTargStat) == 4 + 8*10, carbTargStat_size)
STATIC_ASSERT(sizeof(recordWFDArrayStruct) == sizeof(recordHeaderStruct) + 4 + 2 + 2 + 4*8 + 4*512, recordWFDArrayStruct_size)
STATIC_ASSERT(sizeof(recordWFDV8ArrayStruct) == sizeof(recordHeaderStruct) + 4 + 8 + 4*8 + 4*1536, recordWFDV8ArrayStruct_size)
STATIC_ASSERT(sizeof(recordScalersStruct) == sizeof(recordHeaderStruct) + 8*6, recordScalersStruct_size)
STATIC_ASSERT(sizeof(SubRun) == 4 + 4*500 + 6*4*500, SubRun_size)
#endif

#endif
