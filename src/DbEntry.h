#ifndef DbEntry_h
#define DbEntry_h

#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdio.h>
#include <string>

#include "TObject.h"

#include "AsymRunDB.h"

class TStructRunDB;

typedef std::map<std::string, std::string> DbFieldMap;
typedef std::map<std::string, void*> DbFieldMapNew;
typedef std::map<std::string, Bool_t> DbFieldSaveFlagMap;


class TStructRunDB {

public:

   double      RunID;
   std::string fRunName;
   UInt_t      fillId;
   UShort_t    fPolId;
   UInt_t      measurementId;
   time_t      timeStamp;
   std::string dateTime;
   Bool_t      isCalibRun;
   std::string calib_file_s;
   std::string dl_calib_run_name;
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

   //bool operator()(const TStructRunDB &rec1, const TStructRunDB &rec2) const;
   bool operator<(const TStructRunDB &rhs) const;
   bool operator==(const TStructRunDB &rhs) const;
   void Streamer(TBuffer &buf);
   void Print(const Option_t* opt="") const;
   void PrintAsPhp(FILE *f=stdout) const;
   //void PrintAsDbEntry(FILE *f=stdout) const;
   void PrintAsDbEntry(std::ostream &o=std::cout, Bool_t printCommonFields=false) const;
   void ProcessLine(std::string sline);
   void UpdateFields(TStructRunDB &dbrun);
   void UpdateValues();
   void SetAsymVersion(std::string version);
};

TBuffer & operator<<(TBuffer &buf, TStructRunDB *&rec);
TBuffer & operator>>(TBuffer &buf, TStructRunDB *&rec);

//struct TStructRunDBCompare {
//}

#endif
