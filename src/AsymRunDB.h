#ifndef ASYM_RUNDB_H
#define ASYM_RUNDB_H

#include <fstream>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "TObjArray.h"
#include "TObjString.h"
#include "TPRegexp.h"

#include "rhicpol.h"
#include "rpoldata.h"

#include "AsymGlobals.h"


int  readdb(double RUNID);
void PrintDB();
//const char * GetVariables(std::string str);
std::string GetVariables(std::string str);
//const char* GetVariables(std::string str);
int SetDefault();
float GetVariablesFloat(std::string str);
int ContinueScan(double ThisRunID, double RunID);
int MatchPolarimetry(double ThisRunID, double RefRunID);
void PrintRunDB();
int StripHandler(int, int);
int FindDisableStrip();
int BunchHandler(int, int);
int FindDisableBunch();



#endif

