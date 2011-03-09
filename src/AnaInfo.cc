
#include "TSystem.h"

#include "AnaInfo.h"

using namespace std;


/** */
TDatprocStruct::TDatprocStruct() :
   fRunId            (""),
   enel              (400),
   eneu              (900),   
   widthl            (-30),
   widthu            (3),    
   fModes            (MODE_NORMAL),
   FEEDBACKMODE      (0),     
   RAWHISTOGRAM      (0),     
   CMODE             (0),     
   DMODE             (0),     
   TMODE             (0),     
   AMODE             (0),     
   BMODE             (1),     
   ZMODE             (0),     
   MESSAGE           (0),     
   CBANANA           (2),     
   UPDATE            (0),
   UPDATE_DB         (1),
   QUICK_MODE        (0),
   MMODE             (1),     
   NTMODE            (0),     
   RECONFMODE        (1),     
   RAMPMODE          (0),     
   STUDYMODE         (0),     
   SAVETREES         (0),     
   MassSigma         (3),     
   MassSigmaAlt      (2),     
   OneSigma          (1.5e6), 
   tshift            (0),     
   inj_tshift        (0),     
   dx_offset         (0),     
   WCMRANGE          (999.05),
   MassLimit         (8),     
   nEventsProcessed  (0),     
   nEventsTotal      (0),     
   thinout           (1),     
   fFastCalibThinout (0.10),     
   reference_rate    (1),     
   //target_count_mm   (0.11),  
   target_count_mm   (1),   // Need to get the real value
   procDateTime      (0),     
   procTimeReal      (0),     
   procTimeCpu       (0),     
   userCalibFile     (""), fAlphaCalibRun(""), fDlCalibRun(""), fAsymEnv(),
   fFileRunInfo(0), fFileRunConf(0), fFileStdLog(0), fFileStdLogName("stdoe.log")
{
   Init();
}


/**
 * Default Values for Run Condition
 */
TDatprocStruct::TDatprocStruct(string runId) :
   fRunId            (runId),
   enel              (400),
   eneu              (900),   
   widthl            (-30),
   widthu            (3),    
   fModes            (MODE_NORMAL),
   FEEDBACKMODE      (0),     
   RAWHISTOGRAM      (0),     
   CMODE             (0),     
   DMODE             (0),     
   TMODE             (0),     
   AMODE             (0),     
   BMODE             (1),     
   ZMODE             (0),     
   MESSAGE           (0),     
   CBANANA           (2),     
   UPDATE            (0),
   UPDATE_DB         (1),
   QUICK_MODE        (0),
   MMODE             (1),     
   NTMODE            (0),     
   RECONFMODE        (1),     
   RAMPMODE          (0),     
   STUDYMODE         (0),     
   SAVETREES         (0),     
   MassSigma         (3),     
   MassSigmaAlt      (2),     
   OneSigma          (1.5e6), 
   tshift            (0),     
   inj_tshift        (0),     
   dx_offset         (0),     
   WCMRANGE          (999.05),
   MassLimit         (8),     
   nEventsProcessed  (0),     
   nEventsTotal      (0),     
   thinout           (1),     
   fFastCalibThinout (0.10),     
   reference_rate    (1),     
   //target_count_mm   (0.11),  
   target_count_mm   (1),   // Need to get the real value
   procDateTime      (0),     
   procTimeReal      (0),     
   procTimeCpu       (0),     
   userCalibFile     (""), fAlphaCalibRun(""), fDlCalibRun(""), fAsymEnv(),
   fFileRunInfo(0), fFileRunConf(0), fFileStdLog(0), fFileStdLogName("stdoe.log")
{
   Init();
}


/** */
TDatprocStruct::~TDatprocStruct()
{
   if (fFileRunInfo) fclose(fFileRunInfo);
   if (fFileRunConf) fclose(fFileRunConf);
   if (fFileStdLog)  fclose(fFileStdLog);
}


/** */
void TDatprocStruct::Init()
{
   const char* tmpEnv;

   tmpEnv = getenv("CNIPOL_DIR");

   if (tmpEnv) fAsymEnv["CNIPOL_DIR"] = tmpEnv;
   else        fAsymEnv["CNIPOL_DIR"] = ".";

   tmpEnv = getenv("DATADIR");

   if (tmpEnv) fAsymEnv["DATADIR"] = tmpEnv;
   else        fAsymEnv["DATADIR"] = ".";

   tmpEnv = getenv("CNIPOL_RESULTS_DIR");

   if (tmpEnv) fAsymEnv["CNIPOL_RESULTS_DIR"] = tmpEnv;
   else        fAsymEnv["CNIPOL_RESULTS_DIR"] = ".";
}


/** */
void TDatprocStruct::MakeOutDir()
{
   if (GetOutDir().size() > 200) {
      printf("ERROR: Output directory name is too long\n");
   }

   //printf("s: %s, %s, %s\n", ifile, RunID, fAsymEnv["CNIPOL_RESULTS_DIR"].c_str());

   //umask(0);
   //if (mkdir(GetOutDir().c_str(), 0777) < 0)
   //   printf("WARNING: Perhaps dir already exists: %s\n", GetOutDir().c_str());

   if (gSystem->mkdir(GetOutDir().c_str()) < 0)
      gSystem->Warning("   TDatprocStruct::TDatprocStruct", "Directory %s already exists", GetOutDir().c_str());
   else {
      gSystem->Info("   TDatprocStruct::TDatprocStruct", "Created directory %s", GetOutDir().c_str());
      gSystem->Chmod(GetOutDir().c_str(), 0775);
   }
}


/** */
string TDatprocStruct::GetOutDir() const
{
   return fAsymEnv.find("CNIPOL_RESULTS_DIR")->second + "/" + fRunId;
}


/** */
string TDatprocStruct::GetAlphaCalibFile() const
{
   if (fAlphaCalibRun.empty()) {
      cout << "Alpha calibration run not defined" << endl;
      return "";
   }

   string path = fAsymEnv.find("CNIPOL_RESULTS_DIR")->second;
   path += "/" + fAlphaCalibRun + "/" + fAlphaCalibRun + ".root";
   return path;
}


/** */
string TDatprocStruct::GetDlCalibFile() const
{
   if (fDlCalibRun.empty()) {
      cout << "Dead layer calibration run not defined" << endl;
      return "";
   }

   string path = fAsymEnv.find("CNIPOL_RESULTS_DIR")->second;
   path += "/" + fDlCalibRun + "/" + fDlCalibRun + ".root";
   return path;
}


/** */
void TDatprocStruct::ProcessOptions()
{
   if (fRunId.empty()) {
      gSystem->Error("   TDatprocStruct::ProcessOptions", "Run name has to be specified");
      PrintUsage();
      exit(0);
   }

   MakeOutDir();

   fFileRunInfo = fopen(GetRunInfoFileName().c_str(), "w");
   gSystem->Chmod(GetRunInfoFileName().c_str(), 0775);

   fFileRunConf = fopen(GetRunConfFileName().c_str(), "w");
   gSystem->Chmod(GetRunConfFileName().c_str(), 0775);

   // Set default standard log output
   if (!fFileStdLogName.empty()) {
      freopen(GetStdLogFileName().c_str(), "w", stdout);
      fclose(stderr);
      stderr = stdout;
      gSystem->Chmod(GetStdLogFileName().c_str(), 0775);
   }

   //freopen(GetStdLogFileName().c_str(), "w", stderr);
   //setbuf(stdout, NULL);
   //fFileStdLogBuf.open(GetStdLogFileName().c_str(), ios::out|ios::ate|ios::app);

   // Various printouts. Should be combined with Print()?
   cout << "Run name:                      " << fRunId << endl;
   cout << "Input data file:               " << gDataFileName << endl;
   cout << "Max events to process:         " << gMaxEventsUser << endl;
   cout << "Events to skip:                " << thinout << endl;
   cout << "User defined calibration file: " << userCalibFile << endl;
   cout << "overwrite conf file:           " << reConfFile << endl;
   cout << "SAVETREES:                     " << SAVETREES << endl;
}


/** */
TBuffer & operator<<(TBuffer &buf, TDatprocStruct *&rec)
{
   if (!rec) return buf;
   //printf("operator<<(TBuffer &buf, TDatprocStruct *rec) : \n");
   rec->Streamer(buf);
   return buf;
}


/** */
TBuffer & operator>>(TBuffer &buf, TDatprocStruct *&rec)
{
   //if (!rec) return buf;
   //printf("operator<<(TBuffer &buf, TDatprocStruct *rec) : \n");
   // if object has been created already delete it
   //free(rec);
   //rec = (TDatprocStruct *) realloc(rec, sizeof(TDatprocStruct));
   rec->Streamer(buf);
   return buf;
}


/** */
void TDatprocStruct::Print(const Option_t* opt) const
{
   cout
   << "fRunId           = " << fRunId           << endl
   << "enel             = " << enel             << endl
   << "eneu             = " << eneu             << endl
   << "widthl           = " << widthl           << endl
   << "widthu           = " << widthu           << endl
   << "fModes           = " << hex << showbase << fModes << endl << dec << noshowbase
   << "FEEDBACKMODE     = " << FEEDBACKMODE     << endl
   << "RAWHISTOGRAM     = " << RAWHISTOGRAM     << endl
   << "CMODE            = " << CMODE            << endl
   << "DMODE            = " << DMODE            << endl
   << "TMODE            = " << TMODE            << endl
   << "AMODE            = " << AMODE            << endl
   << "BMODE            = " << BMODE            << endl
   << "ZMODE            = " << ZMODE            << endl
   << "MESSAGE          = " << MESSAGE          << endl
   << "CBANANA          = " << CBANANA          << endl
   << "UPDATE           = " << UPDATE           << endl
   << "UPDATE_DB        = " << UPDATE_DB        << endl
   << "QUICK_MODE       = " << QUICK_MODE       << endl
   << "MMODE            = " << MMODE            << endl
   << "NTMODE           = " << NTMODE           << endl
   << "RECONFMODE       = " << RECONFMODE       << endl
   << "RAMPMODE         = " << RAMPMODE         << endl
   << "STUDYMODE        = " << STUDYMODE        << endl
   << "SAVETREES        = " << SAVETREES        << endl
   << "MassSigma        = " << MassSigma        << endl
   << "MassSigmaAlt     = " << MassSigmaAlt     << endl
   << "OneSigma         = " << OneSigma         << endl
   << "tshift           = " << tshift           << endl
   << "inj_tshift       = " << inj_tshift       << endl
   << "dx_offset        = " << dx_offset        << endl
   << "WCMRANGE         = " << WCMRANGE         << endl
   << "MassLimit        = " << MassLimit        << endl
   << "nEventsProcessed = " << nEventsProcessed << endl
   << "nEventsTotal     = " << nEventsTotal     << endl
   << "thinout          = " << thinout          << endl
   << "fFastCalibThinout= " << fFastCalibThinout<< endl
   << "reference_rate   = " << reference_rate   << endl
   << "target_count_mm  = " << target_count_mm  << endl
   << "procDateTime     = " << procDateTime     << endl
   << "procTimeReal     = " << procTimeReal     << endl
   << "procTimeCpu      = " << procTimeCpu      << endl
   << "userCalibFile    = " << userCalibFile    << endl
   << "fAlphaCalibRun   = " << fAlphaCalibRun   << endl
   << "fDlCalibRun      = " << fDlCalibRun      << endl;
}


/** */
void TDatprocStruct::PrintAsPhp(FILE *f) const
{ //{{{
   fprintf(f, "$rc['fRunId']                       = \"%s\";\n", fRunId.c_str());
   fprintf(f, "$rc['enel']                         = %d;\n", enel);
   fprintf(f, "$rc['eneu']                         = %d;\n", eneu);
   fprintf(f, "$rc['widthl']                       = %d;\n", widthl);
   fprintf(f, "$rc['widthu']                       = %d;\n", widthu);
   fprintf(f, "$rc['CMODE']                        = %d;\n", CMODE);
   fprintf(f, "$rc['nEventsProcessed']             = %u;\n", nEventsProcessed);
   fprintf(f, "$rc['nEventsTotal']                 = %u;\n", nEventsTotal);
   fprintf(f, "$rc['thinout']                      = %u;\n", thinout);
   fprintf(f, "$rc['procDateTime']                 = %u;\n", (UInt_t) procDateTime);
   fprintf(f, "$rc['procTimeReal']                 = %f;\n", procTimeReal);
   fprintf(f, "$rc['procTimeCpu']                  = %f;\n", procTimeCpu);
} //}}}


/** */
void TDatprocStruct::PrintUsage()
{
   cout << endl;
   cout << "Options:" << endl;
   cout << " -h, -?                          : Print this help" << endl;
   cout << " -r, -f, --run-name <run_name>   : Name of run with raw data in $DATADIR directory" << endl;
   cout << " -n <number>                     : Maximum number of events to process"
        << " (default \"-n 0\" all events)" << endl;
   cout << " -s <number>                     : Only every <number> event will be"
        << " processed (default \"-s 1\" no skip)" << endl;
   cout << " -c <calib_file_name>            : Set root file with calibration info (!)" << endl;
   cout << " -o <filename>                   : Output hbk file (!)" << endl;
   //cout << " -r <filename>                   : ramp timing file" << endl;
   cout << " -l, --log=[filename]            : Optional log file to redirect stdout and stderr" << endl;
   cout << " -t <time shift>                 : TOF timing shift in [ns], addition to TSHIFT defined in run.db (!)" << endl;
   cout << " -e <lower:upper>                : Kinetic energy range (default [400:900] keV) (!)" << endl;
   //cout << " -B                              : create banana curve on" << endl;
   //cout << " -G                              : mass mode on " << endl;
   cout << " -a, --no-error-detector         : Anomaly check off (!)" << endl;
   cout << " -b                              : Feedback mode on (!)" << endl;
   cout << " -D                              : Dead layer mode on (!)" << endl;
   cout << " -d <dlayer>                     : Additional deadlayer thickness [ug/cm2] (!)" << endl;
   //cout << " -T                              : T0 study    mode on " << endl;
   //cout << " -A                              : A0,A1 study mode on " << endl;
   //cout << " -Z                              : without T0 subtraction" << endl;
   cout << " -F <file>                       : Overwrite conf file defined in run.db (!)" << endl;
   cout << " -W <lower:upper>                : Const width banana cut (!)" << endl;
   cout << " -m <sigma>                      : Banana cut by <sigma> from 12C mass [def]:3 sigma (!)" << endl;
   cout << " -U                              : Update histogram" << endl;
   cout << "     --update-db                 : Update run info in database" << endl;
   cout << " -N                              : Store Ntuple events (!)" << endl;
   cout << " -R <bitmask>                    : Save events in Root trees, " <<
           "e.g. \"-R 101\"" << endl;
   cout << " -q, --quick                     : Skips the main loop. Use for a quick check" << endl;
   cout << " -C, --mode-alpha, --alpha       : Use when run over alpha run data" << endl;
   cout << "     --mode-calib, --calib       : Update calibration constants" << endl;
   cout << "     --mode-normal               : Default set of histograms" << endl;
   cout << "     --mode-no-normal            : Turn off the default set of histograms" << endl;
   cout << "     --mode-scaler, --scaler     : Fill and save scaler histograms (from V124 memory)" << endl;
   cout << "     --mode-raw, --raw           : Fill and save raw histograms" << endl;
   cout << "     --mode-run                  : Fill and save bunch, lumi and other run related histograms" << endl;
   cout << "     --mode-target, --target     : Fill and save target histograms" << endl;
   cout << "     --mode-full                 : Fill and save all histograms" << endl;
   cout << " -g, --graph                     : Save histograms as images" << endl;
   cout << endl;
   cout << "Options marked with (!) are not really supported" << endl;
   cout << endl;
}


/** */
void TDatprocStruct::Streamer(TBuffer &buf)
{
   TString tstr;

   if (buf.IsReading()) {
      //printf("reading TDatprocStruct::Streamer(TBuffer &buf) \n");
      buf >> tstr; fRunId = tstr.Data();
      buf >> enel;
      buf >> eneu;
      buf >> widthl;
      buf >> widthu;
      buf >> fModes;
      buf >> CMODE;
      buf >> nEventsProcessed;
      buf >> nEventsTotal;
      buf >> thinout;
      buf >> fFastCalibThinout;
      buf >> procDateTime >> procTimeReal >> procTimeCpu;
      buf >> tstr; fAlphaCalibRun = tstr.Data();
      buf >> tstr; fDlCalibRun = tstr.Data();
   } else {
      //printf("writing TDatprocStruct::Streamer(TBuffer &buf) \n");
      tstr = fRunId; buf << tstr;
      buf << enel;
      buf << eneu;
      buf << widthl;
      buf << widthu;
      buf << fModes;
      buf << CMODE;
      buf << nEventsProcessed;
      buf << nEventsTotal;
      buf << thinout;
      buf << fFastCalibThinout;
      buf << procDateTime << procTimeReal << procTimeCpu;
      tstr = fAlphaCalibRun; buf << tstr;
      tstr = fDlCalibRun;    buf << tstr;
   }
}


/** */
void TDatprocStruct::Update(TStructRunDB &rundb)
{
   // If user didn't specify his/her calibration file use the one from the DB
   if (fAlphaCalibRun.empty())
      fAlphaCalibRun = rundb.fFields["ALPHA_CALIB_RUN_NAME"];

   if (fDlCalibRun.empty())
      fDlCalibRun = rundb.fFields["DL_CALIB_RUN_NAME"];
}
