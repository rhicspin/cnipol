
#include "globals.h"

//#include "rhicpol.h"

// dsmirnov thinks this is wrong as it does not agree with the previous run idexes
//char polCDEVName[4][20]             = {"polarimeter.blu1", "polarimeter.blu2", "polarimeter.yel1", "polarimeter.yel2"};
//char wcmCDEVName[2][20]             = {"yi2-wcm3","bo2-wcm3"}; // pre 2013 names
//char wcmCDEVName[2][20]             = {"wcm.yellow","wcm.blue"}; // post 2013 names
//char specCDEVName[2][20]            = {"ringSpec.yellow", "ringSpec.blue"};
//char bucketsCDEVName[2][20]         = {"buckets.yellow", "buckets.blue"};
char polCDEVName[4][20]             = {"polarimeter.blu1", "polarimeter.yel1", "polarimeter.blu2", "polarimeter.yel2"};
char wcmCDEVName[2][20]             = {"wcm.blue", "wcm.yellow"}; // post 2013 names
char specCDEVName[2][20]            = {"ringSpec.blue", "ringSpec.yellow"};
char bucketsCDEVName[2][20]         = {"buckets.blue", "buckets.yellow"};
char gCavityCdevNames[2][30]        = {"cavTuneLoop.4a-rf-b197-1.3", "cavTuneLoop.4a-rf-y197-1.3"};
char gSnakeCdevNames[2][30]         = {"bo3-snk7-1.4-ps.current", "yi3-snk7-1.4-ps.current"};
char gStarRotatorCdevNames[2][30]   = {"bi5-rot3-1.4-ps.current", "yo5-rot3-1.4-ps.current"};
char gPhenixRotatorCdevNames[2][30] = {"bo7-rot3-1.4-ps.current", "yi7-rot3-1.4-ps.current"};

char ourTargetCDEVName[20]          = "None";   // we will write what is appropriate here in getAdoInfo()

bool gUseCdev       = true;                  // don't take anything from CDEV

EMeasType gMeasType = kMEASTYPE_UNDEF;   // measurement type must be provided

wcmDataStruct         wcmData;               // Wall current monitor data from CDEV
wcmDataStruct         wcmOtherData;          // we need both beams for hjet
RecordWcm             gRecordWcm;
RecordMachineParams   gRecordMachineParams;
RecordTargetParams    gRecordTargetParams;
int                   recRing = 0;           // data mask with ring information etc.
