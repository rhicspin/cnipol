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


int  BunchSelect(int bid);
int  GetPolarimetryID_and_RHICBeam(char RunID[]);
int  read_ramptiming(char *filename);
void reConfig(recordConfigRhicStruct *cfginfo);
int  ConfigureActiveStrip(int mask);
//int DisabledDet(int det);
int  sqass(float A, float B, float C, float D, float *asym, float *easym);
int  Initialization();

#endif /* ASYM_MAIN_H */
