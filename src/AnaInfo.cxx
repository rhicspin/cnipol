#include <getopt.h>

#include "AnaInfo.h"

#include "TString.h"
#include "TSystem.h"

#include "AsymGlobals.h"
#include "MseMeasInfo.h"
#include "MeasInfo.h"

#include "revision-export.h"

ClassImp(AnaInfo)

using namespace std;


/** */
AnaInfo::AnaInfo() : TObject(),
   fOutputName(""),
   fAsymVersion(ASYM_VERSION),
   fSuffix(""),
   fModes(0),
   fAnaDateTime(0),
   fAnaTimeReal(0),
   fAnaTimeCpu (0),
   fAsymEnv(),
   fFileMeasInfo(0), fFileStdLog(0),
   fFileStdLogName("stdoe"), fFlagCopyResults(kFALSE), fFlagUseDb(true),
   fFlagUpdateDb(kFALSE),    fFlagCreateThumbs(kFALSE),
   fUserGroup(),
   fAlphaSourceCount(-1)
{
   Init();
}


/** */
AnaInfo::~AnaInfo()
{
   if (fFileMeasInfo) fclose(fFileMeasInfo);
   if (fFileStdLog)   fclose(fFileStdLog);
}


/** */
void AnaInfo::Init()
{
   const char* tmpEnv;

   tmpEnv = getenv("CNIPOL_DIR");

   if (tmpEnv) fAsymEnv["CNIPOL_DIR"] = tmpEnv;
   else        fAsymEnv["CNIPOL_DIR"] = ".";

   tmpEnv = getenv("CNIPOL_DATA_DIR");

   if (tmpEnv) fAsymEnv["CNIPOL_DATA_DIR"] = tmpEnv;
   else        fAsymEnv["CNIPOL_DATA_DIR"] = ".";

   tmpEnv = getenv("CNIPOL_RESULTS_DIR");

   if (tmpEnv) fAsymEnv["CNIPOL_RESULTS_DIR"] = tmpEnv;
   else        fAsymEnv["CNIPOL_RESULTS_DIR"] = ".";

   tmpEnv = getenv("SLOW_CONTROL_LOG_DIR");

   if (tmpEnv) fAsymEnv["SLOW_CONTROL_LOG_DIR"] = tmpEnv;
   else        fAsymEnv["SLOW_CONTROL_LOG_DIR"] = "/eicdata/eic0005/cdev_log";

   fUserGroup = *gSystem->GetUserInfo();
}


/** */
void AnaInfo::MakeOutDir()
{
   if (GetOutDir().size() > 200) {
      Error("MakeOutDir", "Output directory name is too long");
   }

   const bool kACCESSABLE = false; // bizarre convention of return value
   if (gSystem->AccessPathName(GetOutDir().c_str()) == kACCESSABLE)
   {
      Warning("MakeOutDir", "Directory %s already exists", GetOutDir().c_str());
   }
   else if (gSystem->mkdir(GetOutDir().c_str()) < 0)
   {
      Fatal("MakeOutDir", "Can't create directory %s", GetOutDir().c_str());
   }
   else
   {
      Info("MakeOutDir", "Created directory %s", GetOutDir().c_str());
      gSystem->Chmod(GetOutDir().c_str(), 0775);
   }
}


/**
 * This function provides a version string where git commit id is shortened.
 * Should be useful for plots.
 */
string AnaInfo::GetShortAsymVersion() const
{
   // find divider between human-readable version and git hash
   size_t len = fAsymVersion.find(";");
   if (len != std::string::npos)
   {
      len += 1 + 10; // set limit at ten more characters of git hash
   }
   return fAsymVersion.substr(0, len);
}


string AnaInfo::GetSuffix()           const { return !fSuffix.empty() ? "_" + fSuffix : "" ; }
string AnaInfo::GetImageDir()         const { return GetOutDir() + "/images" + GetSuffix(); }
string AnaInfo::GetAnaInfoFileName()  const { return GetOutDir() + "/anainfo" + GetSuffix() + ".php"; }
string AnaInfo::GetStdLogFileName()   const { return GetOutDir() + "/" + fFileStdLogName + GetSuffix() + ".log"; }
string AnaInfo::GetRootFileName()     const { return GetOutDir() + "/" + fOutputName + GetSuffix() + ".root"; }
FILE*  AnaInfo::GetAnaInfoFile()      const { return fFileMeasInfo; }

Bool_t AnaInfo::HasGraphBit()         const { return (fModes & AnaInfo::MODE_GRAPH) == AnaInfo::MODE_GRAPH; }


string AnaInfo::GetResultsDir() const
{
   return fAsymEnv.find("CNIPOL_RESULTS_DIR")->second;
}


/** */
string AnaInfo::GetOutDir() const
{
   return fAsymEnv.find("CNIPOL_RESULTS_DIR")->second + "/" + fOutputName;
}


std::string AnaInfo::GetSlowControlLogDir() const
{
   return fAsymEnv.find("SLOW_CONTROL_LOG_DIR")->second;
}


/** */
void AnaInfo::ProcessOptions(int argc, char **argv)
{
}


/** */
void AnaInfo::VerifyOptions()
{
   // The run name must be specified
   if (fOutputName.empty()) {
      Error("VerifyOptions", "Output name must be specified");
      PrintUsage();
      exit(EXIT_FAILURE);
   }

   MakeOutDir();

   fFileMeasInfo = fopen(this->GetAnaInfoFileName().c_str(), "w");
   gSystem->Chmod(this->GetAnaInfoFileName().c_str(), 0775);

   // Set default standard log output
   if (!fFileStdLogName.empty()) {
      freopen(GetStdLogFileName().c_str(), "w", stdout);
      fclose(stderr);
      stderr = stdout;
      gSystem->Chmod(GetStdLogFileName().c_str(), 0775);
   }
}


/** */
void AnaInfo::Print(const Option_t* opt) const
{
   Info("Print", "Print members:");
   PrintAsPhp();
}


/** */
void AnaInfo::PrintAsPhp(FILE *f) const
{
   fprintf(f, "$rc['fOutputName']                  = \"%s\";\n",  fOutputName.c_str());
   fprintf(f, "$rc['fAsymVersion']                 = \"%s\";\n",  fAsymVersion.c_str());
   fprintf(f, "$rc['fSuffix']                      = \"%s\";\n",  fSuffix.c_str());
   fprintf(f, "$rc['fModes']                       = %#010lx;\n", fModes);
   fprintf(f, "$rc['fAnaDateTime']                 = %u;\n",      (UInt_t) fAnaDateTime);
   fprintf(f, "$rc['fAnaTimeReal']                 = %f;\n",      fAnaTimeReal);
   fprintf(f, "$rc['fAnaTimeCpu']                  = %f;\n",      fAnaTimeCpu);

   stringstream ssEnvs("");

   ssEnvs << "array(";

   for (Str2StrMap::const_iterator ienv=fAsymEnv.begin(); ienv!=fAsymEnv.end(); ienv++) {
      ssEnvs << "'" << ienv->first << "'"  << " => " << "\"" << ienv->second << "\"";
      ssEnvs << (ienv != (--fAsymEnv.end()) ? ", " : "");
   }

   ssEnvs << ")";

   fprintf(f, "$rc['fAsymEnv']                     = %s;\n", ssEnvs.str().c_str());
   fprintf(f, "$rc['fFileStdLogName']              = \"%s\";\n", fFileStdLogName.c_str());
   fprintf(f, "$rc['fFlagCopyResults']             = %d;\n", fFlagCopyResults);
   fprintf(f, "$rc['fFlagUseDb']                   = %d;\n", fFlagUseDb);
   fprintf(f, "$rc['fFlagUpdateDb']                = %d;\n", fFlagUpdateDb);
   fprintf(f, "$rc['fFlagCreateThumbs']            = %d;\n", fFlagCreateThumbs);
   fprintf(f, "$rc['fUserGroup_fUser']             = \"%s\";\n", fUserGroup.fUser.Data());
   fprintf(f, "$rc['fUserGroup_fRealName']         = \"%s\";\n", fUserGroup.fRealName.Data());

   fprintf(f, "\n");
}


/** */
void AnaInfo::CopyResults()
{
   if (!fFlagCopyResults) return;

   string cmd = "rsync --stats --exclude=*.root -rlpgoDv " + GetOutDir() + " pc2pc.phy.bnl.gov:/usr/local/polarim/root/";
   Info("CopyResults", "Copying results...\n%s", cmd.c_str());

   char result[1000];
   FILE *fp = popen( cmd.c_str(), "r");

   while (fgets(result, sizeof(result), fp) != NULL ) { printf("%s", result); }

   pclose(fp);
}


/** */
void AnaInfo::PrintUsage()
{
   cout << endl;
   cout << "Options:" << endl;
   cout << " -h, -?                               : Print this help" << endl;
   cout << " -l, --log=[filename]                 : Optional log file to redirect stdout and stderr" << endl;
   cout << " -g, --graph                          : Save histograms as images" << endl;
   cout << "     --copy                           : Copy results to server (?)" << endl;
   cout << "     --use-db                         : Run info will be retrieved from and saved into database" << endl;
   cout << "     --update-db                      : Update run info in database" << endl;
   cout << endl;
}
