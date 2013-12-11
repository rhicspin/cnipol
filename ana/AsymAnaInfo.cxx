
#include <getopt.h>

#include "AsymAnaInfo.h"

#include "TString.h"
#include "TSystem.h"

#include "AsymGlobals.h"
#include "DbEntry.h"
#include "MseMeasInfo.h"
#include "MeasInfo.h"

ClassImp(AsymAnaInfo)

using namespace std;


/** */
AsymAnaInfo::AsymAnaInfo() : AnaInfo(),
   fRunName          (""),
   fAsymModes        (MODE_NORMAL|MODE_CALIB|MODE_PROFILE|MODE_TARGET|MODE_RAW_EXTENDED|MODE_ASYM|MODE_PMT|MODE_KINEMA),
   enel              (400),
   eneu              (900),
   widthl            (-30),
   widthu            (3),
   fSaveTrees        (0),
   fDisabledDetectors(0),
   MassSigma         (3),
   MassSigmaAlt      (2),
   OneSigma          (CARBON_MASS_PEAK_SIGMA),
   tshift            (0),
   dx_offset         (0),
   WCMRANGE          (999.05),
   MassLimit         (8),
   fThinout          (1),
   fMaxEventsUser    (0),
   reference_rate    (1),
   //target_count_mm   (0.11),
   target_count_mm   (1),   // Need to get the real value
   fAlphaCalibRun(""),
   fDlCalibRun(""),
   fFileRunConf(0),
   fAcDipolePeriod(0)
{
   Init();
}


/** */
AsymAnaInfo::~AsymAnaInfo()
{
   if (fFileRunConf) fclose(fFileRunConf);
}


void   AsymAnaInfo::SetRunName(std::string runName) { fRunName = runName; }

string AsymAnaInfo::GetRunName()          const { return fRunName; }
string AsymAnaInfo::GetRawDataFileName()  const { return fAsymEnv.find("CNIPOL_DATA_DIR")->second + "/" + fRunName + ".data"; }
string AsymAnaInfo::GetAnaInfoFileName()  const { return GetOutDir() + "/runconfig" + GetSuffix() + ".php"; }
string AsymAnaInfo::GetRunConfFileName()  const { return GetOutDir() + "/config_calib" + GetSuffix() + ".dat"; }
FILE*  AsymAnaInfo::GetRunConfFile()      const { return fFileRunConf; }

string AsymAnaInfo::GetAlphaCalibRun()    const { return fAlphaCalibRun + GetSuffix(); }
string AsymAnaInfo::GetDlCalibRun()       const { return fDlCalibRun; }

Bool_t AsymAnaInfo::HasAlphaBit() const  {
   return (fAsymModes & (AsymAnaInfo::MODE_ALPHA^AsymAnaInfo::MODE_CALIB))  == (AsymAnaInfo::MODE_ALPHA^AsymAnaInfo::MODE_CALIB);
 }

Bool_t AsymAnaInfo::HasCalibBit()       const { return (fAsymModes & AsymAnaInfo::MODE_CALIB)        == AsymAnaInfo::MODE_CALIB; }
Bool_t AsymAnaInfo::HasNormalBit()      const { return (fAsymModes & AsymAnaInfo::MODE_NORMAL)       == AsymAnaInfo::MODE_NORMAL; }
Bool_t AsymAnaInfo::HasScalerBit()      const { return (fAsymModes & AsymAnaInfo::MODE_SCALER)       == AsymAnaInfo::MODE_SCALER; }
Bool_t AsymAnaInfo::HasRawBit()         const { return (fAsymModes & AsymAnaInfo::MODE_RAW)          == AsymAnaInfo::MODE_RAW; }
Bool_t AsymAnaInfo::HasRawExtendedBit() const { return (fAsymModes & AsymAnaInfo::MODE_RAW_EXTENDED) == AsymAnaInfo::MODE_RAW_EXTENDED; }
Bool_t AsymAnaInfo::HasRunBit()         const { return (fAsymModes & AsymAnaInfo::MODE_RUN)          == AsymAnaInfo::MODE_RUN; }
Bool_t AsymAnaInfo::HasTargetBit()      const { return (fAsymModes & AsymAnaInfo::MODE_TARGET)       == AsymAnaInfo::MODE_TARGET; }
Bool_t AsymAnaInfo::HasProfileBit()     const { return (fAsymModes & AsymAnaInfo::MODE_PROFILE)      == AsymAnaInfo::MODE_PROFILE; }
Bool_t AsymAnaInfo::HasAsymBit()        const { return (fAsymModes & AsymAnaInfo::MODE_ASYM)         == AsymAnaInfo::MODE_ASYM; }
Bool_t AsymAnaInfo::HasKinematBit()     const { return (fAsymModes & AsymAnaInfo::MODE_KINEMA)       == AsymAnaInfo::MODE_KINEMA; }
Bool_t AsymAnaInfo::HasPmtBit()         const { return (fAsymModes & AsymAnaInfo::MODE_PMT)          == AsymAnaInfo::MODE_PMT; }
Bool_t AsymAnaInfo::HasPulserBit()      const { return (fAsymModes & AsymAnaInfo::MODE_PULSER)       == AsymAnaInfo::MODE_PULSER; }
Bool_t AsymAnaInfo::HasStudiesBit()     const { return (fAsymModes & AsymAnaInfo::MODE_STUDIES)      == AsymAnaInfo::MODE_STUDIES; }
Bool_t AsymAnaInfo::HasOnlineBit()      const { return (fAsymModes & AsymAnaInfo::MODE_ONLINE)       == AsymAnaInfo::MODE_ONLINE; }
Bool_t AsymAnaInfo::HasNoSshBit()       const { return (fAsymModes & AsymAnaInfo::MODE_NO_SSH)       == AsymAnaInfo::MODE_NO_SSH; }


/** */
string AsymAnaInfo::GetAlphaCalibFile() const
{
   if (fAlphaCalibRun.empty()) {
      cout << "Alpha calibration run not defined" << endl;
      return "";
   }

   string path = fAsymEnv.find("CNIPOL_RESULTS_DIR")->second;
   path += "/" + fAlphaCalibRun + "/" + GetAlphaCalibRun() + ".root";
   return path;
}


/** */
string AsymAnaInfo::GetDlCalibFile() const
{
   if (fDlCalibRun.empty()) {
      Warning("GetDlCalibFile", "Dead layer calibration run not defined");
      return "";
   }

   string path = fAsymEnv.find("CNIPOL_RESULTS_DIR")->second;
   path += "/" + fDlCalibRun + "/" + fDlCalibRun + ".root";

   return path;
}


/** */
string AsymAnaInfo::GetRootTreeFileName(UShort_t trid) const
{
   string filename;
   filename.reserve(GetOutDir().size() + fRunName.size() + 20);
   sprintf(&filename[0], "%s/%s%s_tree_%02d.root", GetOutDir().c_str(), fRunName.c_str(), GetSuffix().c_str(), trid);
   return filename;
}


/** */
void AsymAnaInfo::ProcessOptions(int argc, char **argv)
{
   //extern char *optarg;

   int          option_index = 0;
   char         cfile[32];
   char         enerange[20], cwidth[20], *ptr;

   static struct option long_options[] = {
      {"log",                 optional_argument,   0,   'l'},
      {"sfx",                 required_argument,   0,   AnaInfo::OPTION_SUFFIX},
      {"graph",               no_argument,         0,   AnaInfo::MODE_GRAPH},
      //{"no-graph",            no_argument,         0,   AnaInfo::MODE_NO_GRAPH},
      //{"mode-graph",          no_argument,         0,   AnaInfo::MODE_GRAPH},
      //{"mode-no-graph",       no_argument,         0,   AnaInfo::MODE_NO_GRAPH},

      {"copy",                no_argument,         0,   AnaInfo::FLAG_COPY},
      {"copy-results",        no_argument,         0,   AnaInfo::FLAG_COPY},
      {"update-db",           no_argument,         0,   AnaInfo::FLAG_UPDATE_DB},
      {"no-update-db",        no_argument,         0,   AnaInfo::FLAG_NO_UPDATE_DB},
      {"use-db",              no_argument,         0,   AnaInfo::FLAG_USE_DB},
      {"thumb",               no_argument,         0,   AnaInfo::FLAG_CREATE_THUMBS},

      {"run-name",            required_argument,   0,   'r'},
      {"pol-id",              required_argument,   0,   AsymAnaInfo::OPTION_POL_ID},
      {"alpha",               no_argument,         0,   AsymAnaInfo::MODE_ALPHA},
      {"calib",               no_argument,         0,   AsymAnaInfo::MODE_CALIB},
      {"scaler",              no_argument,         0,   AsymAnaInfo::MODE_SCALER},
      {"raw",                 no_argument,         0,   AsymAnaInfo::MODE_RAW},
      {"raw-ext",             no_argument,         0,   AsymAnaInfo::MODE_RAW_EXTENDED},
      {"target",              no_argument,         0,   AsymAnaInfo::MODE_TARGET},
      {"profile",             no_argument,         0,   AsymAnaInfo::MODE_PROFILE},
      {"asym",                no_argument,         0,   AsymAnaInfo::MODE_ASYM},
      {"kinema",              no_argument,         0,   AsymAnaInfo::MODE_KINEMA},
      {"pmt",                 no_argument,         0,   AsymAnaInfo::MODE_PMT},
      {"pulser",              no_argument,         0,   AsymAnaInfo::MODE_PULSER},
      {"studies",             no_argument,         0,   AsymAnaInfo::MODE_STUDIES},
      {"online",              no_argument,         0,   AsymAnaInfo::MODE_ONLINE},
      {"no-ssh",              no_argument,         0,   AsymAnaInfo::MODE_NO_SSH},
      {"mode-alpha",          no_argument,         0,   AsymAnaInfo::MODE_ALPHA},
      {"mode-calib",          no_argument,         0,   AsymAnaInfo::MODE_CALIB},
      {"mode-normal",         no_argument,         0,   AsymAnaInfo::MODE_NORMAL},
      {"mode-no-normal",      no_argument,         0,   AsymAnaInfo::MODE_NO_NORMAL},
      {"mode-scaler",         no_argument,         0,   AsymAnaInfo::MODE_SCALER},
      {"mode-raw",            no_argument,         0,   AsymAnaInfo::MODE_RAW},
      {"mode-raw-extended",   no_argument,         0,   AsymAnaInfo::MODE_RAW_EXTENDED},
      {"mode-run",            no_argument,         0,   AsymAnaInfo::MODE_RUN},
      {"mode-target",         no_argument,         0,   AsymAnaInfo::MODE_TARGET},
      {"mode-profile",        no_argument,         0,   AsymAnaInfo::MODE_PROFILE},
      {"mode-asym",           no_argument,         0,   AsymAnaInfo::MODE_ASYM},
      {"mode-kinema",         no_argument,         0,   AsymAnaInfo::MODE_KINEMA},
      {"mode-pmt",            no_argument,         0,   AsymAnaInfo::MODE_PMT},
      {"mode-online",         no_argument,         0,   AsymAnaInfo::MODE_ONLINE},
      {"mode-full",           no_argument,         0,   AsymAnaInfo::MODE_FULL},
      {"set-calib",           required_argument,   0,   AsymAnaInfo::OPTION_SET_CALIB},
      {"set-calib-alpha",     required_argument,   0,   AsymAnaInfo::OPTION_SET_CALIB_ALPHA},
      {"set-calib-dl",        required_argument,   0,   AsymAnaInfo::OPTION_SET_CALIB_DL},
      {"disable-det",         required_argument,   0,   AsymAnaInfo::OPTION_DET_DISABLE},
      {"alpha-sources",       required_argument,   0,   AsymAnaInfo::OPTION_ALPHA_SOURCES},
      {"ac-dipole-period",    required_argument,   0,   AsymAnaInfo::OPTION_AC_DIPOLE_PERIOD},
      {0, 0, 0, 0}
   };

   int c;

   while ((c = getopt_long(argc, argv, "?hl::gr:f:n:s:o:t:e:m:d:baCDTBZF:MNW:UGR:",
                           long_options, &option_index)) != -1)
   {
      stringstream sstr;
      switch (c) {

      case '?':
      case 'h':
         PrintUsage();
         exit(EXIT_FAILURE);

      case 'l':
         fFileStdLogName = (optarg != 0 ? optarg : "");
         break;

      case 'g':
      case AnaInfo::MODE_GRAPH:
         fModes |= AnaInfo::MODE_GRAPH;
         break;

      //case AsymAnaInfo::MODE_NO_GRAPH:
      //   fModes &= ~AsymAnaInfo::MODE_GRAPH;
      //   break;

      case AnaInfo::OPTION_SUFFIX:
         fSuffix = optarg;
         break;

      case AnaInfo::FLAG_USE_DB:
         fFlagUseDb    = kTRUE;
         fFlagUpdateDb = kTRUE;
         break;

      case AnaInfo::FLAG_CREATE_THUMBS:
         fFlagCreateThumbs = kTRUE;
         break;

      case AnaInfo::FLAG_COPY:
         fFlagCopyResults = kTRUE;
         break;

      case AnaInfo::FLAG_UPDATE_DB:
         fFlagUpdateDb = kTRUE;
         break;

      case AnaInfo::FLAG_NO_UPDATE_DB:
         fFlagUpdateDb = kFALSE;
         break;

      case 'r':
      case 'f':
         SetRunName(optarg);
         gRunDb.fRunName = optarg; // deprecated, should be removed later
         break;

      case 'n':
         fMaxEventsUser = atoi(optarg);
         break;

      case 's':
         sstr.str("");
         sstr << optarg;
         sstr >> fThinout;
         break;

      case 't': // set timing shift in banana cut
         tshift = atoi(optarg);
         extinput.TSHIFT = 1;
         break;

      case 'd': // set timing shift in banana cut
         dx_offset = atoi(optarg);
         break;

      case 'e': // set energy range
         strncpy(enerange, optarg, sizeof(enerange));

         if ((ptr = strrchr(enerange, ':'))) {
            ptr++;
            eneu = atoi(ptr);
            strtok(enerange, ":");
            enel = atoi(enerange);
            if (enel == 0 || enel < 0)     { enel = 0;}
            if (eneu == 0 || eneu > 12000) { eneu = 2000;}
            fprintf(stdout, "ENERGY RANGE LOWER:UPPER = %d:%d\n", enel, eneu);
         } else {
            cout << "Wrong specification for energy threshold" << endl;
         }
         break;

      case 'F':
         strncpy(cfile, optarg, sizeof(cfile));
         strncat(reConfFile, cfile, sizeof(cfile) - 1 - strlen(cfile));
         extinput.CONFIG = 1;
         break;

      case 'W': // constant width banana cut
         strcpy(cwidth, optarg);
         if ((ptr = strrchr(cwidth,':'))) {
            ptr++;
            widthu = atoi(ptr);
            strtok(cwidth,":");
            widthl = atoi(cwidth);
            fprintf(stdout,"CONSTANT BANANA CUT LOWER:UPPER = %d:%d\n",
                    widthl,widthu);
            if (widthu == widthl)
               fprintf(stdout, "WARNING: Banana Lower = Upper Cut\a\n");
         } else {
            fprintf(stdout, "Wrong specification constant banana cut\n");
         }
         fprintf(stdout,"BANANA Cut : %d <==> %d \n",
                 widthl,widthu);
         break;

      case 'm':
         MassSigma = atof(optarg);
         extinput.MASSCUT = 1;
         break;

      case 'R':
         sstr.str("");
         sstr << optarg;
         sstr >> fSaveTrees;
         break;

      case AsymAnaInfo::OPTION_POL_ID:
         gMeasInfo->fPolId = atoi(optarg);
         break;

      case AsymAnaInfo::OPTION_SET_CALIB_ALPHA:
         fAlphaCalibRun = optarg;
         break;

      case AsymAnaInfo::OPTION_SET_CALIB_DL:
         fDlCalibRun = optarg;
         break;

      case AsymAnaInfo::OPTION_SET_CALIB:
         fAlphaCalibRun = optarg;
         fDlCalibRun    = optarg;
         break;

      case AsymAnaInfo::OPTION_DET_DISABLE:
         sstr.str("");
         sstr << optarg;
         sstr >> fDisabledDetectors;
         break;

      case AsymAnaInfo::OPTION_ALPHA_SOURCES:
         sstr.str("");
         sstr << optarg;
         sstr >> fAlphaSourceCount;
         if ((fAlphaSourceCount > 2) || (fAlphaSourceCount < 1))
         {
            cerr << "Invalid alpha source count setting!" << endl;
            exit(EXIT_FAILURE);
         }
         break;

      case AsymAnaInfo::OPTION_AC_DIPOLE_PERIOD:
         sstr.clear();
         sstr.str(string(optarg));
         sstr >> fAcDipolePeriod;
         break;

      case 'C':
      case AsymAnaInfo::MODE_ALPHA:
         fAsymModes |= AsymAnaInfo::MODE_ALPHA;
         fAsymModes &= ~AsymAnaInfo::MODE_NORMAL; // turn off normal mode
         break;

      case AsymAnaInfo::MODE_CALIB:
         fAsymModes |= AsymAnaInfo::MODE_CALIB;
         break;

      case AsymAnaInfo::MODE_NO_NORMAL:
         fAsymModes &= ~AsymAnaInfo::MODE_NORMAL;
         break;

      case AsymAnaInfo::MODE_SCALER:
         fAsymModes |= AsymAnaInfo::MODE_SCALER;
         break;

      case AsymAnaInfo::MODE_RAW:
         fAsymModes |= AsymAnaInfo::MODE_RAW;
         break;

      case AsymAnaInfo::MODE_RAW_EXTENDED:
         fAsymModes |= AsymAnaInfo::MODE_RAW_EXTENDED;
         break;

      case AsymAnaInfo::MODE_RUN:
         fAsymModes |= AsymAnaInfo::MODE_RUN;
         break;

      case AsymAnaInfo::MODE_TARGET:
         fAsymModes |= AsymAnaInfo::MODE_TARGET;
         break;

      case AsymAnaInfo::MODE_PROFILE:
         fAsymModes |= AsymAnaInfo::MODE_PROFILE;
         fAsymModes |= AsymAnaInfo::MODE_TARGET; // profile hists depend on target ones
         break;

      case AsymAnaInfo::MODE_ASYM:
         fAsymModes |= AsymAnaInfo::MODE_ASYM;
         break;

      case AsymAnaInfo::MODE_KINEMA:
         fAsymModes |= AsymAnaInfo::MODE_KINEMA;
         break;

      case AsymAnaInfo::MODE_PMT:
         fAsymModes |= AsymAnaInfo::MODE_PMT;
         break;

      case AsymAnaInfo::MODE_PULSER:
         fAsymModes |= AsymAnaInfo::MODE_PULSER;
         break;

      case AsymAnaInfo::MODE_STUDIES:
         fAsymModes |= AsymAnaInfo::MODE_STUDIES;
         break;

      case AsymAnaInfo::MODE_ONLINE:
         fAsymModes |= AsymAnaInfo::MODE_ONLINE;
         break;

      case AsymAnaInfo::MODE_NO_SSH:
         fAsymModes |= AsymAnaInfo::MODE_NO_SSH;
         break;

      case AsymAnaInfo::MODE_FULL:
         fAsymModes |= AsymAnaInfo::MODE_FULL;
         break;

      default:
         Error("ProcessOptions", "Unknown option provided");
         PrintUsage();
         exit(EXIT_FAILURE);
         break;
      }
   }
}


/** */
void AsymAnaInfo::VerifyOptions()
{
   // The run name must be specified
   if (fRunName.empty()) {
      Error("VerifyOptions", "Run name has to be specified");
      PrintUsage();
      exit(EXIT_FAILURE);
   }

   fOutputName = fRunName;
   gMeasInfo->SetRunName(fRunName);

   // Check whether the raw data file exists
	TString fileName = fRunName + ".data";

   if ( !gSystem->FindFile(fAsymEnv["CNIPOL_DATA_DIR"].c_str(), fileName ) )
	{
      Error("VerifyOptions", "Raw data file \"%s.data\" not found in %s\n", fRunName.c_str(), fAsymEnv["CNIPOL_DATA_DIR"].c_str());
      exit(EXIT_FAILURE);
	}

   if (HasAlphaBit()) {
      fAlphaCalibRun     = fRunName;
      fDlCalibRun        = "";
      gMeasInfo->SetMeasType(kMEASTYPE_ALPHA);
   }

   // Disable channels if requested by user
   if ( fDisabledDetectors.any() ) {
      gMeasInfo->DisableChannels(fDisabledDetectors);
   }

   // The output dir is created here
   AnaInfo::VerifyOptions();

   fFileRunConf = fopen(GetRunConfFileName().c_str(), "w");
   gSystem->Chmod(GetRunConfFileName().c_str(), 0775);
}


/** */
void AsymAnaInfo::Print(const Option_t* opt) const
{
   Info("Print", "Print members:");
   PrintAsPhp();
}


/** */
void AsymAnaInfo::PrintAsPhp(FILE *f) const
{
   AnaInfo::PrintAsPhp(f);
   fprintf(f, "$rc['fRunName']                     = \"%s\";\n",  fRunName.c_str());
   fprintf(f, "$rc['fAsymModes']                   = %#010lx;\n", fAsymModes);
   fprintf(f, "$rc['enel']                         = %d;\n",      enel);
   fprintf(f, "$rc['eneu']                         = %d;\n",      eneu);
   fprintf(f, "$rc['widthl']                       = %d;\n",      widthl);
   fprintf(f, "$rc['widthu']                       = %d;\n",      widthu);
   fprintf(f, "$rc['fSaveTrees']                   = \"%s\";\n",  fSaveTrees.to_string().c_str());
   fprintf(f, "$rc['fDisabledDetectors']           = \"%s\";\n",  fDisabledDetectors.to_string().c_str());
   fprintf(f, "$rc['fThinout']                     = %f;\n",      fThinout);
   fprintf(f, "$rc['fMaxEventsUser']               = %u;\n",      fMaxEventsUser);
   fprintf(f, "$rc['fAlphaCalibRun']               = \"%s\";\n",  GetAlphaCalibRun().c_str());
   fprintf(f, "$rc['fDlCalibRun']                  = \"%s\";\n",  fDlCalibRun.c_str());
   fprintf(f, "$rc['fAlphaSourceCount']            = %i;\n",      fAlphaSourceCount);
   fprintf(f, "$rc['fAcDipolePeriod']              = %i;\n",      fAcDipolePeriod);
   fprintf(f, "\n");
}


/** */
void AsymAnaInfo::PrintUsage()
{
   cout << endl;

   AnaInfo::PrintUsage();

   cout << "Options:" << endl;
   cout << " -h, -?                               : Print this help" << endl;
   cout << " -r, -f, --run-name <run_name>        : Name of run with raw data in $CNIPOL_DATA_DIR directory" << endl;
   cout << " -n <number>                          : Maximum number of events to process"
        << " (default \"-n 0\" all events)" << endl;
   cout << " -s <real>                            : Approximate fraction of events to read/process (default \"-s 1\" no events skipped)" << endl;
   cout << " -o <filename>                        : Output hbk file (!)" << endl;
   cout << " -l, --log=[filename]                 : Optional log file to redirect stdout and stderr" << endl;
   cout << " -t <time shift>                      : TOF timing shift in [ns], addition to TSHIFT defined in run.db (!)" << endl;
   cout << " -e <lower:upper>                     : Kinetic energy range (default [400:900] keV) (!)" << endl;
   cout << " -D                                   : Dead layer mode on (!)" << endl;
   cout << " -d <dlayer>                          : Additional deadlayer thickness [ug/cm2] (!)" << endl;
   cout << " -F <file>                            : Overwrite conf file defined in run.db (!)" << endl;
   cout << " -W <lower:upper>                     : Const width banana cut (!)" << endl;
   cout << " -m <sigma>                           : Banana cut by <sigma> from 12C mass [def]:3 sigma (!)" << endl;
   cout << " -U                                   : Update histogram" << endl;
   cout << " -N                                   : Store Ntuple events (!)" << endl;
   cout << " -R <bitmask>                         : Save events in Root trees, " <<
           "e.g. \"-R 101\"" << endl;
   cout << "     --alpha-sources <count>          : 1 for Am, 2 for Am+Gd" << endl;
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
   cout << "     --no-ssh                         : Disable filling MahineParams from log server" << endl;
   cout << "     --set-calib-alpha                : Provide a root file with alpha calibrations" << endl;
   cout << "     --copy                           : Copy results to server (?)" << endl;
   cout << "     --use-db                         : Run info will be retrieved from and saved into database" << endl;
   cout << "     --update-db                      : Update run info in database" << endl;
   cout << "     --disable-det <bitmask>          : Exclude some detectors from the analysis, e.g. \"000100\" excludes detector 3" << endl;
   cout << "     --ac-dipole-period               : Unsigned integer representing dipole period in ticks, i.e. large number 16441672" << endl;
   cout << endl;
   cout << "Options marked with (!) are not supported" << endl;
   cout << "Options marked with (?) need more work" << endl;
   cout << endl;
}


/**
 * Some of this method functionality should better be moved to MeasInfo Update(). This class
 * should deal only with input command line options.
 */
void AsymAnaInfo::Update(MseMeasInfoX& run)
{
	// A fix for alpha calib runs - Maybe this should go to the process options
   // method
   if (HasAlphaBit()) {
      fAlphaCalibRun           = "";
      fDlCalibRun              = "";
      run.alpha_calib_run_name = "";
      run.dl_calib_run_name    = "";

      // If user set the alpha option override the measurement type extracted
      // from the data file
      gMeasInfo->SetMeasType(kMEASTYPE_ALPHA);
   }

   if ( !fAlphaCalibRun.empty() )
      run.alpha_calib_run_name = fAlphaCalibRun;
   else if ( !run.alpha_calib_run_name.empty() )
      fAlphaCalibRun = run.alpha_calib_run_name;

   if ( fAlphaCalibRun.empty() ) {
      if (!HasAlphaBit()) {
         Error("Update", "Alpha calibration run must be specified");
         Error("Update", "Specify it on command line or use --use-db option");
         exit(EXIT_FAILURE);
      }
   } else
      Info("Update", "Using alpha calibration run %s", fAlphaCalibRun.c_str());


   // Set DL calib files
   if (HasCalibBit()) {
      fDlCalibRun           = "";
      run.dl_calib_run_name = "";
   }

   if (!fDlCalibRun.empty())
      run.dl_calib_run_name = fDlCalibRun;
   else if (!run.dl_calib_run_name.empty())
      fDlCalibRun = run.dl_calib_run_name;

   if (fDlCalibRun.empty()) {
      if (!HasCalibBit()) {
         Warning("Update", "Calibration run is not specified.\n\tOption --calib should be used");
      }
   } else {
      Info("Update", "Using calibration run %s", run.dl_calib_run_name.c_str());
   }
}
