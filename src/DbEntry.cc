/**
 *
 *  10 Mar, 2011 - Dmitri Smirnov
 *    - Created class
 *
 */

#include "DbEntry.h"

#include <limits.h>
#include <sstream>

#include "TObjString.h"
#include "TPRegexp.h"

#include "AsymRunDB.h"


using namespace std;


/** */
DbEntry::DbEntry() : fPolId(UCHAR_MAX), timeStamp(0), fFields(), fFieldFlags()
{
   for (UShort_t i=0; i<AsymRunDB::sNFields; i++) {
      //printf("%s\n", AsymRunDB::sFieldNames[i]);
      fFields[AsymRunDB::sFieldNames[i]] = "none";
      fFieldFlags[AsymRunDB::sFieldNames[i]] = false;
   }
   //printf("%s\n", AsymRunDB::sFieldNames[0].c_str());
   //cout << AsymRunDB::sFieldNames[0]<< endl;

   // Persistant fields
   fFieldFlags["ASYM_VERSION"]      = true;
   fFieldFlags["POLARIMETER_ID"]    = true;
   fFieldFlags["POLARIZATION"]      = true;
   fFieldFlags["PROFILE_RATIO"]     = true;
   fFieldFlags["START_TIME"]        = true;
   fFieldFlags["STOP_TIME"]         = true;
   fFieldFlags["NEVENTS_TOTAL"]     = true;
   fFieldFlags["NEVENTS_PROCESSED"] = true;
   fFieldFlags["BEAM_ENERGY"]       = true;
   fFieldFlags["TARGET_ID"]         = true;
}


/** */
DbEntry::~DbEntry()
{
   //printf("dest \n");
   fFields.clear();
   fFieldFlags.clear();
}


/** */
/*
void DbEntry::PrintAsDbEntry(FILE *f) const
{
   fprintf(f, "\n[%s]\n", fRunName.c_str());

   DbFieldMap::const_iterator ifld;
   DbFieldMap::const_iterator bfld = fFields.begin();
   DbFieldMap::const_iterator efld = fFields.end();

   for (ifld=bfld; ifld!=efld; ifld++) {
      if (ifld->second != "none")
         fprintf(f, "\t%s = %s;\n", ifld->first.c_str(), ifld->second.c_str());
   }
   
   fprintf(f, "\n");
}
*/


/**
 * Prints all fields except those having a default "none" value.
 */
void DbEntry::PrintAsDbEntry(ostream &o, Bool_t printCommonFields) const
{
   o << "["+fRunName+"]" << endl;

   DbFieldMap::const_iterator ifld;
   DbFieldMap::const_iterator bfld = fFields.begin();
   DbFieldMap::const_iterator efld = fFields.end();

   DbFieldSaveFlagMap::const_iterator ifldFlag = fFieldFlags.begin();

   for (ifld=bfld; ifld!=efld; ifld++, ifldFlag++) {
      
      if (ifld->second != "none") {
         string star = ifldFlag->second ? " *" : " ";
         o << "\t" << ifld->first << star << "= " << ifld->second << ";" << endl;
      }
   }

   o << endl;
}


/** */
void DbEntry::ProcessLine(std::string sline)
{
   DbFieldMap::iterator ifld;
   DbFieldMap::iterator bfld = fFields.begin();
   DbFieldMap::iterator efld = fFields.end();

   for (ifld=bfld; ifld!=efld; ifld++) {
      //printf("%s=%s\n", ifld->first.c_str(), ifld->second.c_str());
      TObjArray *subStrL = TPRegexp("^\\s+"+ifld->first+"\\s*(\\*?)=\\s*(.*)$").MatchS(sline);

      if (subStrL && subStrL->GetEntriesFast() >= 3) {

         // first pattern
         TString subStr = ((TObjString *) subStrL->At(1))->GetString();

         if (subStr == "*") fFieldFlags[ifld->first] = true;

         // second pattern
         subStr = ((TObjString *) subStrL->At(2))->GetString();
         delete subStrL;

         // remove comments
         Ssiz_t npos = subStr.Index("//");
         if (npos > 0) subStr.Remove(npos);

         // remove other characters
         subStr.Remove(TString::kBoth, ' ');
         subStr.Remove(TString::kBoth, '\t');
         subStr.Remove(TString::kBoth, ' ');
         subStr.Remove(TString::kBoth, '@');
         subStr.Remove(TString::kBoth, ';');
         subStr.Remove(TString::kBoth, '\"');
         subStr.Remove(TString::kBoth, ' ');

         if (subStr.Length() == 0) {
            ifld->second = "";
         } else if (ifld->first == "DISABLED_BUNCHES" || ifld->first == "EnableBunch" ||
            ifld->first == "DISABLED_CHANNELS" || ifld->first == "EnableStrip")
         {
            if (ifld->second == "none") ifld->second = "";
            ifld->second += " ";
            ifld->second += subStr;
         } else
            ifld->second = subStr;

         //printf("XXX found field: %s = |%s| %d\n", ifld->first.c_str(), ifld->second.c_str(), fFieldFlags[ifld->first]);
         //printf("XXX found field: %s = |%s|\n", ifld->first.c_str(), ifld->second.c_str());
      }
   }
}


/** */
TBuffer & operator<<(TBuffer &buf, DbEntry *&rec)
{
   if (!rec) return buf;
   //printf("operator<<(TBuffer &buf, DbEntry *rec) : \n");
   rec->Streamer(buf);
   return buf;
}


/** */
TBuffer & operator>>(TBuffer &buf, DbEntry *&rec)
{
   //if (!rec) return buf;
   //printf("operator<<(TBuffer &buf, DbEntry *rec) : \n");
   // if object has been created already delete it
   //free(rec);
   //rec = (DbEntry *) realloc(rec, sizeof(DbEntry ));
   rec->Streamer(buf);
   return buf;
}


/** */
void DbEntry::Streamer(TBuffer &buf)
{
   if (buf.IsReading()) {
      TString tstr;
      //printf("reading DbEntry::Streamer(TBuffer &buf) \n");
      buf >> RunID;
      buf >> isCalibRun;
      buf >> tstr; calib_file_s         = tstr.Data();
      buf >> tstr; dl_calib_run_name    = tstr.Data();
      buf >> tstr; alpha_calib_run_name = tstr.Data();
      buf >> tstr; config_file_s        = tstr.Data();
   } else {
      TString tstr;
      //printf("writing DbEntry::Streamer(TBuffer &buf) \n");
      buf << RunID;
      buf << isCalibRun;
      tstr = calib_file_s;         buf << tstr;
      tstr = dl_calib_run_name;    buf << tstr;
      tstr = alpha_calib_run_name; buf << tstr;
      tstr = config_file_s;        buf << tstr;
   }
}


/** */
//bool DbEntry::operator()(const DbEntry &rec1, const DbEntry &rec2) const
//{
//  return (rec1.RunID < rec2.RunID);
//}


/** */
bool DbEntry::operator<(const DbEntry &rhs) const
{
   // First, compare by start time
   if (timeStamp < rhs.timeStamp) return true;
   else if (timeStamp > rhs.timeStamp) return false;

   // If no start time defined (should be 0), compare by run ID
   stringstream ss1;
   stringstream ss2;

   ss1 << setfill(' ') << setw(20) << right << fRunName;
   ss2 << setfill(' ') << setw(20) << right << rhs.fRunName;

   return ss1.str().compare(ss2.str()) < 0;
   //return strcmp(fRunName.c_str(), rhs.fRunName.c_str()) < 0;
   //return false;
}


/** */
bool DbEntry::operator==(const DbEntry &rhs) const
{
   // First, compare by start time
   //if (timeStamp < rhs.timeStamp) return true;
   //else if (timeStamp > rhs.timeStamp) return false;

   // If no start time defined (should be 0), compare by run ID
   stringstream ss1;
   stringstream ss2;

   ss1 << setfill(' ') << setw(20) << right << fRunName;
   ss2 << setfill(' ') << setw(20) << right << rhs.fRunName;

   return ss1.str().compare(ss2.str()) == 0;
   //return strcmp(fRunName.c_str(), rhs.fRunName.c_str()) < 0;
   //return false;
}


/** */
void DbEntry::Print(const Option_t* opt) const
{ //{{{
   //printf("RunID:             %f\n", RunID);
   //printf("isCalibRun:        %d\n", isCalibRun);
   //printf("calib_file_s:      %s\n", calib_file_s.c_str());
   //printf("alpha_calib_run_name: %s\n", alpha_calib_run_name.c_str());
   //printf("config_file_s:     %s\n", config_file_s.c_str());
   //printf("masscut_s:         %s\n", masscut_s.c_str());
   //printf("comment_s:         %s\n", comment_s.c_str());
   //cout << "RunID: " <<

   PrintAsDbEntry(cout);

} //}}}


/**
 * Copies values from dbrun to this. All fields except default ("none") and
 * proprietary ("*") * ones.
 */
void DbEntry::UpdateFields(DbEntry &dbrun)
{
   DbFieldMap::iterator ifld;
   DbFieldMap::iterator bfld = dbrun.fFields.begin();
   DbFieldMap::iterator efld = dbrun.fFields.end();

   for (ifld=bfld; ifld!=efld; ifld++) {

      string &field_name  = (string &) ifld->first;
      string &field_value = (string &) ifld->second;

      //printf("name, value: %s, %s\n", field_name.c_str(), field_value.c_str());

      bool flag_from = dbrun.fFieldFlags[field_name];
      bool flag_to   = fFieldFlags[field_name];

      // For the same run all values are copied
      if (fRunName == dbrun.fRunName) {
         fFields[field_name] = field_value;
         fFieldFlags[field_name] = flag_from;

      } else {
         if (!flag_from && !flag_to && field_value != "none") {
            fFields[field_name] = field_value;
         }
      }
   }

   UpdateValues();
}


/** */
void DbEntry::UpdateValues()
{
   stringstream sstr;
   sstr.str(""); sstr << fFields["POLARIMETER_ID"]; sstr >> fPolId;
   sstr.str(""); sstr << fFields["START_TIME"];     sstr >> timeStamp;
}


/** */
void DbEntry::SetAsymVersion(std::string version)
{
   fFields["ASYM_VERSION"] = version;
}


/** */
void DbEntry::PrintAsPhp(FILE *f) const
{ //{{{
   //fprintf(f, "$rc['calib_file_s']                 = \"%s\";\n", calib_file_s.c_str());
   //fprintf(f, "$rc['dl_calib_run_name']            = \"%s\";\n", dl_calib_run_name.c_str());
   //fprintf(f, "$rc['alpha_calib_run_name']         = \"%s\";\n", alpha_calib_run_name.c_str());
   //fprintf(f, "$rc['config_file_s']                = \"%s\";\n", config_file_s.c_str());

   DbFieldMap::const_iterator ifld;
   DbFieldMap::const_iterator bfld = fFields.begin();
   DbFieldMap::const_iterator efld = fFields.end();

   for (ifld=bfld; ifld!=efld; ifld++) {

      string &field_name  = (string &) ifld->first;
      string &field_value = (string &) ifld->second;
      fprintf(f, "$rc['%s']                = \"%s\";\n", field_name.c_str(), field_value.c_str());
   }
} //}}}
