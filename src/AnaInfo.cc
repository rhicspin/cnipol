
#include "TSystem.h"

#include "AnaInfo.h"

using namespace std;


// Default Values for Run Condition
TDatprocStruct::TDatprocStruct() :
   enel             (400),
   eneu             (900),   
   widthl           (-30),
   widthu           (3),    
   fModes           (MODE_NORMAL),
   FEEDBACKMODE     (0),     
   RAWHISTOGRAM     (0),     
   CMODE            (0),     
   DMODE            (0),     
   TMODE            (0),     
   AMODE            (0),     
   BMODE            (1),     
   ZMODE            (0),     
   MESSAGE          (0),     
   CBANANA          (2),     
   UPDATE           (0),
   UPDATE_DB        (1),
   QUICK_MODE       (0),
   MMODE            (1),     
   NTMODE           (0),     
   RECONFMODE       (1),     
   RAMPMODE         (0),     
   STUDYMODE        (0),     
   SAVETREES        (0),     
   MassSigma        (3),     
   MassSigmaAlt     (2),     
   OneSigma         (1.5e6), 
   tshift           (0),     
   inj_tshift       (0),     
   dx_offset        (0),     
   WCMRANGE         (999.05),
   MassLimit        (8),     
   nEventsProcessed (0),     
   nEventsTotal     (0),     
   thinout          (1),     
   fFastCalibThinout (0.10),     
   reference_rate   (1),     
   target_count_mm  (0.11),  
   procDateTime     (0),     
   procTimeReal     (0),     
   procTimeCpu      (0),     
   userCalibFile    (""), fAlphaCalibRun(""), fDlCalibRun(""), fAsymEnv()
{ 
   const char* tmpEnv = getenv("DATADIR");

   if (tmpEnv) fAsymEnv["DATADIR"] = tmpEnv;
   else        fAsymEnv["DATADIR"] = ".";

   tmpEnv = getenv("CNIPOL_RESULTS_DIR");

   if (tmpEnv) fAsymEnv["CNIPOL_RESULTS_DIR"] = tmpEnv;
   else        fAsymEnv["CNIPOL_RESULTS_DIR"] = ".";
}


/** */
TDatprocStruct::~TDatprocStruct() { }


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
      gSystem->Error("TDatprocStruct::TDatprocStruct", "Directory %s already exists", GetOutDir().c_str());
   else
      gSystem->Info("TDatprocStruct::TDatprocStruct", "Created directory %s", GetOutDir().c_str());
}


/** */
string TDatprocStruct::GetOutDir()
{
   string path = fAsymEnv["CNIPOL_RESULTS_DIR"];
   path += "/" + gRunInfo.runName + "/";
   return path;
}


/** */
string TDatprocStruct::GetAlphaCalibFile()
{
   if (fAlphaCalibRun.empty()) {
      cout << "Alpha calibration run not defined" << endl;
      return "";
   }

   string path = fAsymEnv["CNIPOL_RESULTS_DIR"];
   path += "/" + fAlphaCalibRun + "/" + fAlphaCalibRun + ".root";
   return path;
}


/** */
string TDatprocStruct::GetDlCalibFile()
{
   if (fDlCalibRun.empty()) {
      cout << "Dead layer calibration run not defined" << endl;
      return "";
   }

   string path = fAsymEnv["CNIPOL_RESULTS_DIR"];
   path += "/" + fDlCalibRun + "/" + fDlCalibRun + ".root";
   return path;
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
void TDatprocStruct::Streamer(TBuffer &buf)
{
   TString tstr;

   if (buf.IsReading()) {
      //printf("reading TDatprocStruct::Streamer(TBuffer &buf) \n");
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
