
#include <getopt.h>

#include "MAsymAnaInfo.h"

#include "TString.h"
#include "TSystem.h"

#include "AsymGlobals.h"
#include "DbEntry.h"
#include "MseMeasInfo.h"
#include "MeasInfo.h"

ClassImp(MAsymAnaInfo)

using namespace std;


/** */
MAsymAnaInfo::MAsymAnaInfo() : TObject(),
   fMListFileName(""),
   fSuffix(""),
   fModes            (MODE_NORMAL),
   fDisabledDetectors(0),
   tshift            (0),
   dx_offset         (0),
   WCMRANGE          (999.05),
   MassLimit         (8),
   fThinout          (1),
   fAnaDateTime      (0),
   fAnaTimeReal      (0),
   fAnaTimeCpu       (0),
   fAlphaCalibRun(""), fDlCalibRun(""), fAsymEnv(),
   fFileMeasInfo(0), fFileRunConf(0), fFileStdLog(0),
   fFileStdLogName("stdoe"), fFlagCopyResults(kFALSE), fFlagUseDb(kFALSE), fFlagUpdateDb(kFALSE), fFlagCreateThumbs(kFALSE),
   fUserGroup(*gSystem->GetUserInfo())
{
   Init();
}


/** */
MAsymAnaInfo::~MAsymAnaInfo()
{
   if (fFileMeasInfo) fclose(fFileMeasInfo);
   if (fFileRunConf)  fclose(fFileRunConf);
   if (fFileStdLog)   fclose(fFileStdLog);

   //delete fUserGroup; fUserGroup = 0;
}


/** */
void MAsymAnaInfo::Init()
{ //{{{
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
} //}}}


/** */
void MAsymAnaInfo::MakeOutDir()
{ //{{{
   if (GetOutDir().size() > 200) {
      printf("ERROR: Output directory name is too long\n");
   }

   //printf("s: %s, %s, %s\n", ifile, RunID, fAsymEnv["CNIPOL_RESULTS_DIR"].c_str());

   //umask(0);
   //if (mkdir(GetOutDir().c_str(), 0777) < 0)
   //   printf("WARNING: Perhaps dir already exists: %s\n", GetOutDir().c_str());

   if (gSystem->mkdir(GetOutDir().c_str()) < 0)
      Warning("MakeOutDir", "Directory %s already exists", GetOutDir().c_str());
   else {
      Info("MakeOutDir", "Created directory %s", GetOutDir().c_str());
      gSystem->Chmod(GetOutDir().c_str(), 0775);
   }
} //}}}


void   MAsymAnaInfo::SetMListFileName(std::string runName) { fMListFileName = runName; }
string MAsymAnaInfo::GetMListFileName() { return fMListFileName; }
string MAsymAnaInfo::GetMListFullPath() { return "/eic/u/dsmirnov/run/" + fMListFileName; }

string MAsymAnaInfo::GetRunName()          const { return fMListFileName; }
string MAsymAnaInfo::GetSuffix()           const { return !fSuffix.empty() ? "_" + fSuffix : "" ; }
string MAsymAnaInfo::GetRawDataFileName()  const { return fAsymEnv.find("CNIPOL_DATA_DIR")->second + "/" + fMListFileName + ".data"; }
string MAsymAnaInfo::GetImageDir()         const { return GetOutDir() + "/images" + GetSuffix(); }
string MAsymAnaInfo::GetMeasInfoFileName() const { return GetOutDir() + "/runconfig" + GetSuffix() + ".php"; }
string MAsymAnaInfo::GetRunConfFileName()  const { return GetOutDir() + "/config_calib" + GetSuffix() + ".dat"; }
string MAsymAnaInfo::GetStdLogFileName()   const { return GetOutDir() + "/" + fFileStdLogName + GetSuffix() + ".log"; }
string MAsymAnaInfo::GetRootFileName()     const { return GetOutDir() + "/" + fMListFileName + GetSuffix() + ".root"; }
FILE*  MAsymAnaInfo::GetMeasInfoFile()     const { return fFileMeasInfo; }
FILE*  MAsymAnaInfo::GetRunConfFile()      const { return fFileRunConf; }

string MAsymAnaInfo::GetAlphaCalibRun()   const { return fAlphaCalibRun; }
string MAsymAnaInfo::GetDlCalibRun()      const { return fDlCalibRun; }

Bool_t MAsymAnaInfo::HasAlphaBit() const  {
   return (fModes & (MAsymAnaInfo::MODE_ALPHA^MAsymAnaInfo::MODE_CALIB))  == (MAsymAnaInfo::MODE_ALPHA^MAsymAnaInfo::MODE_CALIB);
 }

Bool_t MAsymAnaInfo::HasCalibBit()       const { return (fModes & MAsymAnaInfo::MODE_CALIB)        == MAsymAnaInfo::MODE_CALIB; }
Bool_t MAsymAnaInfo::HasGraphBit()       const { return (fModes & MAsymAnaInfo::MODE_GRAPH)        == MAsymAnaInfo::MODE_GRAPH; }
Bool_t MAsymAnaInfo::HasNormalBit()      const { return (fModes & MAsymAnaInfo::MODE_NORMAL)       == MAsymAnaInfo::MODE_NORMAL; }
Bool_t MAsymAnaInfo::HasScalerBit()      const { return (fModes & MAsymAnaInfo::MODE_SCALER)       == MAsymAnaInfo::MODE_SCALER; }
Bool_t MAsymAnaInfo::HasRawBit()         const { return (fModes & MAsymAnaInfo::MODE_RAW)          == MAsymAnaInfo::MODE_RAW; }
Bool_t MAsymAnaInfo::HasRawExtendedBit() const { return (fModes & MAsymAnaInfo::MODE_RAW_EXTENDED) == MAsymAnaInfo::MODE_RAW_EXTENDED; }
Bool_t MAsymAnaInfo::HasRunBit()         const { return (fModes & MAsymAnaInfo::MODE_RUN)          == MAsymAnaInfo::MODE_RUN; }
Bool_t MAsymAnaInfo::HasTargetBit()      const { return (fModes & MAsymAnaInfo::MODE_TARGET)       == MAsymAnaInfo::MODE_TARGET; }
Bool_t MAsymAnaInfo::HasProfileBit()     const { return (fModes & MAsymAnaInfo::MODE_PROFILE)      == MAsymAnaInfo::MODE_PROFILE; }
Bool_t MAsymAnaInfo::HasAsymBit()        const { return (fModes & MAsymAnaInfo::MODE_ASYM)         == MAsymAnaInfo::MODE_ASYM; }
Bool_t MAsymAnaInfo::HasKinematBit()     const { return (fModes & MAsymAnaInfo::MODE_KINEMA)       == MAsymAnaInfo::MODE_KINEMA; }
Bool_t MAsymAnaInfo::HasPmtBit()         const { return (fModes & MAsymAnaInfo::MODE_PMT)          == MAsymAnaInfo::MODE_PMT; }
Bool_t MAsymAnaInfo::HasPulserBit()      const { return (fModes & MAsymAnaInfo::MODE_PULSER)       == MAsymAnaInfo::MODE_PULSER; }
Bool_t MAsymAnaInfo::HasOnlineBit()      const { return (fModes & MAsymAnaInfo::MODE_ONLINE)       == MAsymAnaInfo::MODE_ONLINE; }


string MAsymAnaInfo::GetResultsDir() const
{
   return fAsymEnv.find("CNIPOL_RESULTS_DIR")->second;
}


/** */
string MAsymAnaInfo::GetOutDir() const
{
   return fAsymEnv.find("CNIPOL_RESULTS_DIR")->second + "/" + fMListFileName;
}


/** */
void MAsymAnaInfo::ProcessOptions(int argc, char **argv)
{ //{{{
   //stringstream sstr;
   int          option_index = 0;

   static struct option long_options[] = {
      {"meas-list",           required_argument,   0,   MAsymAnaInfo::OPTION_MLIST},
      {"sfx",                 required_argument,   0,   MAsymAnaInfo::OPTION_SUFFIX},
      {0, 0, 0, 0}
   };

   int c;

   while ((c = getopt_long(argc, argv, "?hm:", long_options, &option_index)) != -1)
   {
      switch (c) {

      case '?':
      case 'h':
         PrintUsage();
         exit(0);

      case 'm':
      case MAsymAnaInfo::OPTION_MLIST:
         SetMListFileName(optarg);
         break;

      case MAsymAnaInfo::FLAG_USE_DB:
         fFlagUseDb    = kTRUE;
         fFlagUpdateDb = kTRUE;
         break;

      case MAsymAnaInfo::FLAG_CREATE_THUMBS:
         fFlagCreateThumbs = kTRUE;
         break;

      case MAsymAnaInfo::OPTION_POL_ID:
         gMeasInfo->fPolId = atoi(optarg); break;

      case MAsymAnaInfo::OPTION_SET_CALIB_ALPHA:
         fAlphaCalibRun = optarg;
         break;

      case MAsymAnaInfo::OPTION_SET_CALIB_DL:
         fDlCalibRun = optarg;
         break;

      default:
         gSystem->Error("int main", "Invalid Option");
         PrintUsage();
         exit(0);
      }
   }
} //}}}


/** */
void MAsymAnaInfo::VerifyOptions()
{ //{{{
   // The run name must be specified
   if (fMListFileName.empty()) {
      Error("VerifyOptions", "File name has to be specified");
      PrintUsage();
      exit(0);
   }

   // Check whether the raw data file exists
	//TString fileName = fMListFileName + ".data";

   //if (!gSystem->FindFile(fAsymEnv["CNIPOL_DATA_DIR"].c_str(), fileName ) )
	//{
   //   Error("VerifyOptions",
	//	   "Raw data file \"%s.data\" not found in %s\n", fMListFileName.c_str(), fAsymEnv["CNIPOL_DATA_DIR"].c_str());
   //   exit(0);
	//}

   //MakeOutDir();

   //fFileMeasInfo = fopen(GetMeasInfoFileName().c_str(), "w");
   //gSystem->Chmod(GetMeasInfoFileName().c_str(), 0775);

   //fFileRunConf = fopen(GetRunConfFileName().c_str(), "w");
   //gSystem->Chmod(GetRunConfFileName().c_str(), 0775);

   // Set default standard log output
   //if (!fFileStdLogName.empty()) {
   //   freopen(GetStdLogFileName().c_str(), "w", stdout);
   //   fclose(stderr);
   //   stderr = stdout;
   //   gSystem->Chmod(GetStdLogFileName().c_str(), 0775);
   //}

   //freopen(GetStdLogFileName().c_str(), "w", stderr);
   //setbuf(stdout, NULL);
   //fFileStdLogBuf.open(GetStdLogFileName().c_str(), ios::out|ios::ate|ios::app);
} //}}}


/** */
void MAsymAnaInfo::Print(const Option_t* opt) const
{ //{{{
   Info("Print", "Print members:");
   PrintAsPhp();
} //}}}


/** */
void MAsymAnaInfo::PrintAsPhp(FILE *f) const
{ //{{{
   fprintf(f, "$rc['fMListFileName']               = \"%s\";\n", fMListFileName.c_str());
   fprintf(f, "$rc['fSuffix']                      = \"%s\";\n", fSuffix.c_str());
   fprintf(f, "$rc['fDisabledDetectors']           = \"%s\";\n", fDisabledDetectors.to_string().c_str());
   fprintf(f, "$rc['fThinout']                     = %f;\n",     fThinout);
   fprintf(f, "$rc['fAnaDateTime']                 = %u;\n",     (UInt_t) fAnaDateTime);
   fprintf(f, "$rc['fAnaTimeReal']                 = %f;\n",     fAnaTimeReal);
   fprintf(f, "$rc['fAnaTimeCpu']                  = %f;\n",     fAnaTimeCpu);
   fprintf(f, "$rc['fAlphaCalibRun']               = \"%s\";\n", fAlphaCalibRun.c_str());
   fprintf(f, "$rc['fDlCalibRun']                  = \"%s\";\n", fDlCalibRun.c_str());

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

   // Various printouts. Should be combined with Print()?
   //cout << "Input data file:               " << GetRawDataFileName() << endl;
   //cout << "Max events to process:         " << gMaxEventsUser << endl;
   fprintf(f, "\n");
} //}}}


/** */
void MAsymAnaInfo::PrintUsage()
{ //{{{
   cout << endl;
   cout << "Options:" << endl;
   cout << " -h, -?                               : Print this help" << endl;
   cout << " -r, -f, --run-name <run_name>        : Name of run with raw data in $CNIPOL_DATA_DIR directory" << endl;
   cout << " -n <number>                          : Maximum number of events to process"
        << " (default \"-n 0\" all events)" << endl;
   cout << " -s <real>                            : Approximate fraction of events to read/process (default \"-s 1\" no events skipped)" << endl;
   cout << " -o <filename>                        : Output hbk file (!)" << endl;
   //cout << " -r <filename>                        : ramp timing file" << endl;
   cout << " -l, --log=[filename]                 : Optional log file to redirect stdout and stderr" << endl;
   cout << " -t <time shift>                      : TOF timing shift in [ns], addition to TSHIFT defined in run.db (!)" << endl;
   cout << " -e <lower:upper>                     : Kinetic energy range (default [400:900] keV) (!)" << endl;
   //cout << " -B                                   : create banana curve on" << endl;
   //cout << " -G                                   : mass mode on " << endl;
   cout << " -a, --no-error-detector              : Anomaly check off (!)" << endl;
   cout << " -b                                   : Feedback mode on (!)" << endl;
   cout << " -D                                   : Dead layer mode on (!)" << endl;
   cout << " -d <dlayer>                          : Additional deadlayer thickness [ug/cm2] (!)" << endl;
   //cout << " -T                                   : T0 study    mode on " << endl;
   //cout << " -A                                   : A0,A1 study mode on " << endl;
   //cout << " -Z                                   : without T0 subtraction" << endl;
   cout << " -F <file>                            : Overwrite conf file defined in run.db (!)" << endl;
   cout << " -W <lower:upper>                     : Const width banana cut (!)" << endl;
   cout << " -m <sigma>                           : Banana cut by <sigma> from 12C mass [def]:3 sigma (!)" << endl;
   cout << " -U                                   : Update histogram" << endl;
   cout << " -N                                   : Store Ntuple events (!)" << endl;
   cout << " -R <bitmask>                         : Save events in Root trees, " <<
           "e.g. \"-R 101\"" << endl;
   cout << " -q, --quick                          : Skips the main loop. Use for a quick check" << endl;
   cout << " -C, --mode-alpha, --alpha            : Use when run over alpha run data" << endl;
   cout << "     --mode-calib, --calib            : Update calibration constants" << endl;
   cout << "     --mode-normal                    : Default set of histograms" << endl;
   cout << "     --mode-no-normal                 : Turn off the default set of histograms" << endl;
   cout << "     --mode-scaler, --scaler          : Fill and save scaler histograms (from V124 memory)" << endl;
   cout << "     --mode-raw, --raw                : Fill and save raw histograms" << endl;
   cout << "     --mode-raw-extended, --raw-ext   : Fill and save more detailed raw histograms" << endl;
   cout << "     --mode-run                       : Fill and save bunch, lumi and other run related histograms" << endl;
   cout << "     --mode-target, --target          : Fill and save target histograms" << endl;
   cout << "     --mode-full                      : Fill and save all histograms" << endl;
   cout << "     --set-calib-alpha                : Provide a root file with alpha calibrations" << endl;
   cout << " -g, --graph                          : Save histograms as images" << endl;
   cout << "     --copy                           : Copy results to server (?)" << endl;
   cout << "     --use-db                         : Run info will be retrieved from and saved into database" << endl;
   cout << "     --update-db                      : Update run info in database" << endl;
   cout << "     --disable-det <bitmask>          : Exclude some detectors from the analysis, e.g. \"000100\" excludes detector 3" << endl;
   cout << endl;
   cout << "Options marked with (!) are not really supported" << endl;
   cout << "Options marked with (?) need more work" << endl;
   cout << endl;
} //}}}


/** */
void MAsymAnaInfo::CopyResults()
{ //{{{
   if (!fFlagCopyResults) return;

   string cmd = "rsync -rlpgoDv " + GetOutDir() + " bluepc:/usr/local/polarim/root/";
   Info("CopyResults", "Copying results...\n%s", cmd.c_str());
   //string cmd = "ls -l";

   //system(cmd.c_str());
   char result[1000];
   FILE *fp = popen( cmd.c_str(), "r");

   while (fgets(result, sizeof(result), fp) != NULL ) { printf("%s", result); }
   //fread(result, 1, sizeof(result), fp);
   //printf("%s", result);

   pclose(fp);
} //}}}
