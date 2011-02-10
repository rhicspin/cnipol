/**
 *
 * 15 Oct, 2010 - Dmitri Smirnov
 *    - Modified readloop to take an object of Root class as an argument 
 *
 * 18 Oct, 2010 - Dmitri Smirnov
 *    - Made the Root object global, redefined readloop back
 *
 */

#ifndef ASYM_READ_H
#define ASYM_READ_H

#include <errno.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "rhicpol.h"
#include "rpoldata.h"

#include "AsymCalc.h"
#include "AsymGlobals.h"
#include "AsymHbook.h"
#include "AsymMain.h"
#include "AsymProcess.h"
#include "AsymRecover.h"
#include "AsymRootGlobals.h"

void readDataFast();
void readRecBegin(TStructRunInfo &ri);
int  readloop();
void UpdateRunConst(TRecordConfigRhicStruct *ci);
void PrintBunchPattern(int *pattern);
void calcRunConst(recordConfigRhicStruct *cfginfo);
void DecodeTargetID(polDataStruct poldat);
void PrepareCollidingBunchPattern();

void ProcessRecord(recordPolAdoStruct &rec);
void ProcessRecord(recordWFDV8ArrayStruct &rec);

#endif
