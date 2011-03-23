/**
 * Asymmetry Analysis for RHIC p-Carbon Polarimeter
 *
 * Author(s):   Dmitri Smirnov
 *
 */

#ifndef ASYM_MAIN_H
#define ASYM_MAIN_H

#include "rpoldata.h"

int  BunchSelect(int bid);
int  GetPolarimetryID_and_RHICBeam(char RunID[]);
void ReadRampTiming(char *filename);
void reConfig(recordConfigRhicStruct *cfginfo);
void ConfigureActiveStrip(int mask);
//int DisabledDet(int det);
void Initialization();

#endif
