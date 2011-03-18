#ifndef ASYM_RUNDB_H
#define ASYM_RUNDB_H

#include <errno.h>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>

#include "TObject.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TPRegexp.h"
#include "TString.h"
#include "TSystem.h"

#include "rhicpol.h"
#include "rpoldata.h"

#include "AsymGlobals.h"
#include "AsymHeader.h"
#include "AnaInfo.h"
#include "DbEntry.h"
#include "RunInfo.h"


typedef std::set<TStructRunDB> DbRunSet;
typedef std::map<UShort_t, TStructRunDB*> DbCommonRunMap;

class AsymRunDB : public TObject {

public:

   static const UShort_t sNFields;
   static const char* sFieldNames[];

   std::string fDbFileName;
   FILE *fDbFile;
   DbCommonRunMap fCommonRunDB;
   DbRunSet fDBRuns; // container for run info read from run.db

public:

   AsymRunDB();
   ~AsymRunDB();

   TStructRunDB* Select(std::string runName="");
   void DeleteRun(std::string runName);
   void Clear(Option_t* opt="");
   void Save();
   void Append(TStructRunDB *dbrun);
   void Insert(TStructRunDB *dbrun);
   void UpdateCommonFields(TStructRunDB &dbrun);
   void DropCommonFields(TStructRunDB *dbrun);
   void Print(const Option_t* opt="") const;
   void PrintCommon();
};

void  readdb(double RUNID);
std::string GetVariables(std::string str);
void  SetDefault();
//float GetVariablesFloat(std::string str);
int   ContinueScan(double ThisRunID, double RunID);
int   MatchPolarimetry(double ThisRunID, double RefRunID);
void  PrintRunDB();
void  printConfig(recordConfigRhicStruct *);
void  StripHandler(int, int);
int   FindDisableStrip();
void  BunchHandler(int, int);
int   FindDisableBunch();

#endif
