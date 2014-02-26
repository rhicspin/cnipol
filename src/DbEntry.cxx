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

#include "AsymDbFile.h"


using namespace std;


/** */
DbEntry::DbEntry() : fPolId(UCHAR_MAX), timeStamp(0), fFields(), fFieldFlags()
{
   for (UShort_t i=0; i<AsymDbFile::sNFields; i++) {
      //printf("%s\n", AsymDbFile::sFieldNames[i]);
      fFields[AsymDbFile::sFieldNames[i]] = "none";
      fFieldFlags[AsymDbFile::sFieldNames[i]] = false;
   }
   //printf("%s\n", AsymDbFile::sFieldNames[0].c_str());
   //cout << AsymDbFile::sFieldNames[0]<< endl;

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
      }
   }
}


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
{
   PrintAsDbEntry(cout);
}


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
{
   DbFieldMap::const_iterator ifld;
   DbFieldMap::const_iterator bfld = fFields.begin();
   DbFieldMap::const_iterator efld = fFields.end();

   for (ifld=bfld; ifld!=efld; ifld++) {

      string &field_name  = (string &) ifld->first;
      string &field_value = (string &) ifld->second;
      fprintf(f, "$rc['%s']                = \"%s\";\n", field_name.c_str(), field_value.c_str());
   }
}
