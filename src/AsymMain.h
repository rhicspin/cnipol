/**
 * Asymmetry Analysis for RHIC p-Carbon Polarimeter
 *
 * Author(s):   Dmitri Smirnov
 *
 */

#ifndef ASYM_MAIN_H
#define ASYM_MAIN_H

#include <iostream>
#include <sstream>
#include <getopt.h>
#include <sys/stat.h>
#include <time.h>

#include "TStopwatch.h"
#include "TTimeStamp.h"

#include "EventConfig.h"

#include "AsymGlobals.h"
#include "AsymRoot.h"
#include "AsymRead.h"
#include "AsymRunDB.h"
#include "AsymHbook.h"


int  BunchSelect(int bid);
int  GetPolarimetryID_and_RHICBeam(char RunID[]);
void ReadRampTiming(char *filename);
void reConfig(recordConfigRhicStruct *cfginfo);
void ConfigureActiveStrip(int mask);
//int DisabledDet(int det);
void Initialization();

#endif
