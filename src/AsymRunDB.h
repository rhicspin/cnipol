#ifndef ASYM_RUNDB_H
#define ASYM_RUNDB_H

#include <errno.h>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>

#include "TObject.h"

#include "rhicpol.h"
#include "rpoldata.h"

#include "AsymHeader.h"
#include "AsymGlobals.h"
#include "DbEntry.h"


typedef std::set<DbEntry> DbRunSet;
typedef std::map<UShort_t, DbEntry*> DbCommonRunMap;

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

   DbEntry* Select(std::string runName="");
   void Delete(std::string runName);
   void Clear();
   void Dump();
   void Append(DbEntry *dbrun);
   void Insert(DbEntry *dbrun);
   void UpdateCommonFields(DbEntry &dbrun);
   void DropCommonFields(DbEntry *dbrun);
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
