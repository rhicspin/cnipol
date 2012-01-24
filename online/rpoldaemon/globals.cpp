
#include "globals.h"

//#include "rhicpol.h"

// dsmirnov thinks this is wrong as it does not agree with the previous run idexes
//char polCDEVName[4][20] = {"polarimeter.blu1", "polarimeter.blu2", "polarimeter.yel1", "polarimeter.yel2"};
// this should be better
char polCDEVName[4][20] = {"polarimeter.blu1", "polarimeter.yel1", "polarimeter.blu2", "polarimeter.yel2"};

EMeasType gMeasType = kMEASTYPE_UNDEF;   // measurement type must be provided
