#ifndef POLDATA_H
#define POLDATA_H

#include <time.h>

#include "rhicpol.h"

#include "TBuffer.h"

#define BSIZE 0x1000000			// 16 MByte

#define REC_BEGIN	0x00000001
#define REC_POLADO	0x00000101
#define REC_TAGADO	0x00000102
#define REC_BEAMADO	0x00000103
#define REC_WCMADO      0x00000104
#define REC_CONFIG	0x00000111	// obsolete
//#define REC_RHIC_CONF   0x00000112	// Instead of REC_CONFIG since Apr2003
#define REC_RHIC_CONF   0x00000113	// New parameters added, but I don't want to change the name. 14/04/04
//#define REC_READRAW	0x00000201
//#define REC_READSUB	0x00000202	
//#define REC_READALL	0x00000203
//#define REC_READAT	0x00000211
#define REC_READRAW	0x00000221	// New parameters added, but I don't want to change the name. 14/04/04
#define REC_READSUB	0x00000222	// New parameters added, but I don't want to change the name. 14/04/04	
#define REC_READALL	0x00000223	// New parameters added, but I don't want to change the name. 14/04/04
#define REC_READAT	0x00000231	// New parameters added, but I don't want to change the name. 14/04/04
#define REC_WFDSCAL	0x00000301	// obsolete
#define REC_HISTAT	0x00000302	// obsolete
#define REC_WFDV8SCAL   0x00000303
#define REC_SCALERS	0x00000401
#define REC_HJETSWITCH	0x00000501	// jet switch record
#define REC_PCTARGET    0x00000502        // target history record
#define REC_END		0x00000999
#define REC_TYPEMASK	0x00007FFF
#define REC_YELLOW	0x00010000 // or'ed with the type
#define REC_BLUE	0x00020000 // or'ed with the type
#define REC_JET		0x00080000 // or'ed with the type and ring
#define REC_120BUNCH	0x00008000 // or'ed with the mask where applicable
#define REC_FROMMEMORY  0x00040000 // or'ed with the mask where applicable

typedef struct {
    long len;		// total length
    long type;		// record type, see above
    long num;		// record number
    union {
	time_t time;	// local UNIX time in most cases
	long delim;	// when read from memory
    } timestamp;
} recordHeaderStruct;

typedef struct {
    recordHeaderStruct header;
    long version;
    char comment[256];
} recordBeginStruct;

typedef struct {
    recordHeaderStruct header;
    polDataStruct data;
} recordPolAdoStruct;

typedef struct {
    recordHeaderStruct header;
    unsigned short Num;		// sequential number of the state
// we put here raw data read from our LC4448 latch register    
    unsigned short data[3];	// just 3 16-bit words
    long reserved[16];		// just for fute=ure extensions
} recordHJetSwitchStruct;

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
    beamDataStruct data;
} recordBeamAdoStruct;

typedef struct {
    recordHeaderStruct header;
    wcmDataStruct data;
} recordWcmAdoStruct;

typedef struct TRecordConfigRhicStruct {
    recordHeaderStruct header;
    configRhicDataStruct data;
    SiChanStruct chanconf[1];	// chanconf[data.NumChannels-1]
				// Actual length is defined from the record
				// length or from NumChannels memeber

    void Streamer(TBuffer &buf);
} recordConfigRhicStruct;

TBuffer & operator<<(TBuffer &buf, TRecordConfigRhicStruct *&rec);
TBuffer & operator>>(TBuffer &buf, TRecordConfigRhicStruct *&rec);
//TBuffer & operator<<(TBuffer &buf, TRecordConfigRhicStruct *rec);
//TBuffer & operator>>(TBuffer &buf, TRecordConfigRhicStruct *rec);

//TBuffer & operator<<(TBuffer &buf, recordHeaderStruct &rec);
//TBuffer & operator>>(TBuffer &buf, recordHeaderStruct &rec);


// New data structures for the V9 version, but keeping the same names

typedef struct {
    unsigned char a;	// amplitude
    unsigned char t;	// time (double resolution)
    unsigned char s;	// square (integral)
    unsigned char tmax;	// time of the maximum (single resoulution)
    unsigned char b:7;	// bunch number    
    unsigned char rev0:1; // the LSB of revolution counter
    unsigned char rev;	// next 8 bits of revolution counter
} ATStruct;

typedef struct {
    unsigned char d[90];// waveform itself
    unsigned char b:7;	// bunch number    
    unsigned char rev0:1; // the LSB of revolution counter
    unsigned char rev[4];	// nex 32 bits of revolution counter
    unsigned char dummy;
} waveformStruct; 	// This is exactly as it comes from WFD, 

typedef struct {
    unsigned char d[42];  // waveform itself
    unsigned char b:7;	  // bunch number    
    unsigned char rev0:1; // the LSB of revolution counter
    unsigned char rev[4]; // nex 32 bits of revolution counter
    unsigned char dummy;
} waveform120Struct;	  // For 120 bunch mode the format from WFD is the same
			  // as for 60 bunch mode, bt it's not goog to store
			  // 48 dummy zeroes, so this is packed

typedef struct {
    unsigned char d[90];// waveform itself
    unsigned char a;	// amplitude
    unsigned char t;	// time (double resolution)
    unsigned char s;	// square (integral)
    unsigned char tmax;	// time of the maximum (single resoulution)
    unsigned char b:7;	// bunch number    
    unsigned char rev0:1; // the LSB of revolution counter
    unsigned char rev;	// next 8 bits of revolution counter
} ALLModeStruct;

typedef struct {
    unsigned char d[42];// waveform itself
    unsigned char a;	// amplitude
    unsigned char t;	// time (double resolution)
    unsigned char s;	// square (integral)
    unsigned char tmax;	// time of the maximum (single resoulution)
    unsigned char b:7;	// bunch number    
    unsigned char rev0:1; // the LSB of revolution counter
    unsigned char rev;	// next 8 bits of revolution counter
} ALLMode120Struct;

/*
    All data structures have the following format:
*/

typedef struct {
    recordHeaderStruct header;
    unsigned char rec[1];	// variable length records with data of the same type
				// from different strips
} recordDataStruct;			

/*  The number of the records could be determined from the total 
    length (header.len) and each record length */

typedef struct {		// each record has its subhead
    unsigned siNum:8;		// silicon number
    unsigned Events:24;		// number events-1, so that 0 designates 1 event
} subheadStruct;		// the length of the record is determined from
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

typedef struct {			// obsolete
    recordHeaderStruct header;
    int siNum;			// silicon number
    unsigned short csr;		// F1A1
    unsigned short win;		// F1A4
    long scalers[8]; 		// 8 internal scalers 
    long hist[512];		// internal histogram memory.
//  Partitioning according header.type:
//	REC_WFDSCAL - 	[0-59] 		bunch C counts
//			[64-127] 	amplitude histogram, unpolarized
//			[128-191] 	amplitude histogram, positive
//			[192-255] 	amplitude histogram, negative
//			[256-511]	amplitude vs time histogram:
//			[8][32]		(8) amplitude slices of time
//	REC_HISTAT - 	[16][32]	(16) amplitude slices of time
} recordWFDArrayStruct;

typedef struct {
    recordHeaderStruct header;
    int siNum;			// silicon number
    unsigned short csr;		// F1A1 CSR5=0 --coarase, 1 --fine
    unsigned short win;		// F1A3 WIN[7:0]*2 is the lower hist limit in time
    unsigned short trg;		// F1A2 TRG[15:8] is the lower hist limit in ampl
				// in fine mode, otherwise ll=0
    unsigned short dummy;	// just to aline
    long scalers[8]; 		// 8 internal scalers 
    long hist[1536];		// internal histogram memory.
//  Partitioning according header.type:
//	REC_WFDSCAL - 	[0-119] 	bunch C counts
//			[128-255] 	amplitude histogram, unpolarized
//			[256-383] 	amplitude histogram, positive
//			[384-511] 	amplitude histogram, negative
//			[512-1536]	amplitude vs time histogram:
// either		[32][32]	coarse: 32 amplitude slices of time
//			ampl BW=8, LL=0, time BW=2, LL=WIN[7:0]*2
// or			[16][64]	fine: 16 amplitude slices of time
//			ampl BW=8, LL=TRG[15:8], time BW=1, LL=WIN[7:0]*2
} recordWFDV8ArrayStruct;


typedef struct {
    recordHeaderStruct header;
    long long data[6];	// 6 48-bit scalers (module LeCroy 2551)
} recordScalersStruct;

typedef struct {
    recordHeaderStruct header;
    char comment[256];
} recordEndStruct;

#endif
