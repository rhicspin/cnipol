#ifndef globals_h
#define globals_h

#include "rpoldata.h"


extern char polCDEVName[4][20];
extern char wcmCDEVName[2][20];
extern char specCDEVName[2][20];
extern char bucketsCDEVName[2][20];
extern char gCavityCdevNames[2][30];
extern char gSnakeCdevNames[2][30];
extern char gStarRotatorCdevNames[2][30];
extern char gPhenixRotatorCdevNames[2][30];

extern char ourTargetCDEVName[20];

extern bool gUseCdev;

extern EMeasType gMeasType;   // measurement type must be provided

extern wcmDataStruct         wcmData;
extern wcmDataStruct         wcmOtherData;
extern RecordWcm             gRecordWcm;
extern RecordMachineParams   gRecordMachineParams;
extern RecordTargetParams    gRecordTargetParams;
extern int recRing;


#endif
