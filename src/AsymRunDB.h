#ifndef ASYM_RUNDB_H
#define ASYM_RUNDB_H

#include <errno.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <math.h>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "TObject.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TPRegexp.h"
#include "TString.h"

#include "rhicpol.h"
#include "rpoldata.h"

#include "AsymGlobals.h"
#include "AsymHeader.h"
#include "AnaInfo.h"
#include "RunInfo.h"


class TStructRunDB;

typedef std::set<TStructRunDB, TStructRunDB> DbRunSet;
typedef std::map<std::string, std::string> DbFieldMap;
typedef std::map<std::string, UShort_t> DbFieldSaveFlagMap;


class TStructRunDB {

public:

   double      RunID;
   std::string fRunName;
   UInt_t      fillId;
   UShort_t    polarimeterId;
   UInt_t      measurementId;
   time_t      timeStamp;
   std::string dateTime;
   Bool_t      isCalibRun;
   std::string calib_file_s;
   std::string alpha_calib_run_name;
   std::string config_file_s;
   std::string masscut_s;
   std::string tshift_s;
   std::string inj_tshift_s;
   std::string run_status_s; 
   std::string measurement_type_s;
   std::string disable_strip_s;
   std::string enable_strip_s;
   std::string disable_bunch_s;
   std::string enable_bunch_s;
   std::string define_spin_pattern_s;
   std::string define_fill_pattern_s;
   std::string reference_rate_s;
   std::string target_count_mm_s;
   std::string comment_s;

   DbFieldMap fFields;
   DbFieldSaveFlagMap fFieldFlags;

public:

   TStructRunDB();
   ~TStructRunDB();

   bool operator()(const TStructRunDB &rec1, const TStructRunDB &rec2) const;
   void Streamer(TBuffer &buf);
   void Print(const Option_t* opt="") const;
   void PrintAsPhp(FILE *f=stdout) const;
   void PrintAsDbEntry(FILE *f=stdout) const;
   void PrintAsDbEntry(std::ostream &o=std::cout) const;
   void UpdateFields(TStructRunDB *dbrun);
};

TBuffer & operator<<(TBuffer &buf, TStructRunDB *&rec);
TBuffer & operator>>(TBuffer &buf, TStructRunDB *&rec);

//struct TStructRunDBCompare {
//}



class AsymRunDB : public TObject {

public:

   static const UShort_t sNFields;
   static const char* sFieldNames[];

   std::string fDbFileName;
   FILE *fDbFile;
   std::map<UShort_t, TStructRunDB*> commonRunDB;

public:

   AsymRunDB();
   ~AsymRunDB();

   TStructRunDB* SelectRun(std::string runName);
   void DeleteRun(std::string runName);
   void Insert(TStructRunDB *dbrun);
   void UpdateCommonFields(TStructRunDB *dbrun);
   void PrintCommon();
};

int   readdb(double RUNID);
void  PrintDB();
std::string GetVariables(std::string str);
int   SetDefault();
float GetVariablesFloat(std::string str);
int   ContinueScan(double ThisRunID, double RunID);
int   MatchPolarimetry(double ThisRunID, double RefRunID);
void  PrintRunDB();
int   printConfig(recordConfigRhicStruct *);
int   StripHandler(int, int);
int   FindDisableStrip();
int   BunchHandler(int, int);
int   FindDisableBunch();

#endif
