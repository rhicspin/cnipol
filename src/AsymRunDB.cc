//  Asymmetry Analysis of RHIC pC Polarimeter
//  file name:    AsymRunDB.cc
//
//  Authors:      Itaru Nakagawa
//                Dmitri Smirnov
//
//  Creation:     11/18/2005
//

#include "AsymRunDB.h"

using namespace std;

int ProcessStrip[NSTRIP];
int ProcessBunch[NBUNCH];

DbRunSet gDBRuns; // container for run info read from run.db


/** */
TStructRunDB::TStructRunDB() : polarimeterId(UCHAR_MAX), fFields(), fFieldFlags()
{
   for (UShort_t i=0; i<AsymRunDB::sNFields; i++) {
      //printf("%s\n", AsymRunDB::sFieldNames[i]);
      fFields[AsymRunDB::sFieldNames[i]] = "";
      fFieldFlags[AsymRunDB::sFieldNames[i]] = 0;
   }
   //fFieldFlags["hhh"] = 0;
   //printf("%s\n", AsymRunDB::sFieldNames[0].c_str());
   //cout << AsymRunDB::sFieldNames[0]<< endl;
}


/** */
TStructRunDB::~TStructRunDB()
{
   //printf("dest \n");
   fFields.clear();
   fFieldFlags.clear();
}


/** */
void TStructRunDB::PrintAsDbEntry(FILE *f) const
{
   printf("\n[%s]\n", fRunName.c_str());

   DbFieldMap::const_iterator ifld;
   DbFieldMap::const_iterator bfld = fFields.begin();
   DbFieldMap::const_iterator efld = fFields.end();

   for (ifld=bfld; ifld!=efld; ifld++) {
      fprintf(f, "\t%s = %s;\n", ifld->first.c_str(), ifld->second.c_str());
      //ir->Print();
   }
}


/** */
void AsymRunDB::UpdateCommonFields(TStructRunDB *dbrun)
{
   // First check the polarimeter id
   if (dbrun->fFields["POLARIMETER_ID"].size() > 0) {
      dbrun->polarimeterId = atoi(dbrun->fFields["POLARIMETER_ID"].c_str());
   } else {
      TObjArray *subStrL = TPRegexp("\\d+\\.(\\d)\\d{2}").MatchS(dbrun->fRunName);

      if (subStrL->GetEntriesFast() < 1) {
         printf("Fatal error: no pol id\n");
         exit(-1);
      }

      TString spolid = ((TObjString *) subStrL->At(1))->GetString();
      delete subStrL;

      dbrun->polarimeterId = spolid.Atoi();
   }

   //if (dbrun->polarimeterId == UCHAR_MAX)

   printf("polId: %d\n", dbrun->polarimeterId);

   if (!commonRunDB[dbrun->polarimeterId]) {
      TStructRunDB *cr = new TStructRunDB();

      cr->fFields["POLARIMETER_ID"] = " ";
      sprintf(&cr->fFields["POLARIMETER_ID"][0], "%1d", dbrun->polarimeterId);

      cr->polarimeterId = dbrun->polarimeterId;
      commonRunDB[dbrun->polarimeterId] = cr;
   }

   commonRunDB[dbrun->polarimeterId]->UpdateFields(dbrun);
}


/** */
void AsymRunDB::PrintCommon()
{
   printf("\nCommon DB runs:\n");

   map<UShort_t, TStructRunDB*>::iterator icr;
   map<UShort_t, TStructRunDB*>::iterator bcr = commonRunDB.begin();
   map<UShort_t, TStructRunDB*>::iterator ecr = commonRunDB.end();

   for (icr=bcr; icr!=ecr; icr++) {
      //fprintf(f, "%s = |%s|\n", ifld->first.c_str(), ifld->second.c_str());
      if (icr->second) icr->second->Print();
   }
}


const UShort_t AsymRunDB::sNFields = 22;

const char* AsymRunDB::sFieldNames[] = {
   "RESET_ALL", "POLARIMETER_ID", "MEASUREMENT_TYPE", "MASSCUT", "TSHIFT",
   "INJ_TSHIFT", "ENERGY_CALIB", "ENERGY_CALIB_ROOT_FILE",
   "ALPHA_CALIB_RUN_NAME", "CONFIG", "DEFINE_SPIN_PATTERN",
   "DEFINE_FILL_PATTERN", "REFERENCE_RATE", "TARGET_COUNT_MM", "COMMENT",
   "DisableBunch", "EnableBunch", "DisableStrip", "EnableStrip", "DisabledBunches", "DisabledStrips",
   "RUN_STATUS"};


/** */
AsymRunDB::AsymRunDB() : TObject()
{
   fDbFileName = "run1.db";

   fDbFile = fopen(fDbFileName.c_str(), "r");

   if (!fDbFile) {
      Error("AsymRunDB", "%s file not found. Force exit", fDbFileName.c_str());
      exit(-1);
   }

   for (UShort_t i=0; i<NPOLARIMETERS; i++) {
      commonRunDB[i] = 0;
   }
}


/** */
TStructRunDB* AsymRunDB::SelectRun(string runName)
{
   char   *line = NULL;
   size_t  len  = 0;
   ssize_t nBytes = 0;
   TStructRunDB *currRun = 0;

   while (true) {

      ssize_t nb = getline(&line, &len, fDbFile);
      //if (nb < 0) break;

      nBytes += nb;

      string sline(line);
      //cout << "line: " << sline;

      if (TPRegexp("\\[([\\w,\\W]+)\\]").MatchB(sline) || nb < 0) {

         if (currRun) {
            UpdateCommonFields(currRun);

            //PrintCommon();
            printf("--- %s, %s\n", runName.c_str(), currRun->fRunName.c_str());

            if (currRun->fRunName == runName) {
               printf("XXX matched %s\n", runName.c_str());
               currRun->UpdateFields(commonRunDB[currRun->polarimeterId]);
               return currRun;
            }

            delete currRun;

            if (nb < 0) {
               //currRun->UpdateFields(commonRunDB[currRun->polarimeterId]);
               return 0;
            }
         }

         TObjArray *subStrL = TPRegexp("\\[([\\w,\\W]+)\\]").MatchS(sline);
         TString subStr = ((TObjString *) subStrL->At(1))->GetString();
         delete subStrL;

         printf("XXX found %s\n", subStr.Data());

         currRun = new TStructRunDB();

         //gDBRuns.insert(*currRun);

         //currRun->RunID = subStr.Atof();
         currRun->fRunName = subStr;
         //currRun->RunID = 0;

      } else if (currRun) {

         DbFieldMap::iterator ifld;
         DbFieldMap::iterator bfld = currRun->fFields.begin();
         DbFieldMap::iterator efld = currRun->fFields.end();

         for (ifld=bfld; ifld!=efld; ifld++) {
            //printf("%s=%s\n", ifld->first.c_str(), ifld->second.c_str());
            TObjArray *subStrL = TPRegexp("\\s*"+ifld->first+"\\s*(\\*?)=\\s*(.*)$").MatchS(sline);

            if (subStrL && subStrL->GetEntriesFast() >= 3) {

               // first pattern
               TString subStr = ((TObjString *) subStrL->At(1))->GetString();

               if (subStr == "*") currRun->fFieldFlags[ifld->first] = true;

               // second pattern
               subStr = ((TObjString *) subStrL->At(2))->GetString();
               delete subStrL;

               // remove comments
               Ssiz_t npos = subStr.Index("//");
               if (npos > 0) subStr.Remove(npos);

               //printf("XXX --- %d\n", npos);
               //subStrL = TPRegexp("^(.*)(//)?(.*)$").MatchS(subStr);
               //subStr = ((TObjString *) subStrL->At(1))->GetString();
               //delete subStrL;

               subStr.Remove(TString::kBoth, ' ');
               subStr.Remove(TString::kBoth, '@');
               subStr.Remove(TString::kBoth, ';');
               subStr.Remove(TString::kBoth, ' ');
               subStr.Remove(TString::kBoth, '\"');
               subStr.Remove(TString::kBoth, ' ');

               if (ifld->first == "DisableBunch" ||
                   ifld->first == "EnableBunch"  ||
                   ifld->first == "DisableStrip" ||
                   ifld->first == "EnableStrip")
               {
                  ifld->second += " ";
                  ifld->second += subStr;
               } else
                  ifld->second = subStr;

               printf("XXX found field: %s = |%s| %d\n", ifld->first.c_str(), ifld->second.c_str(), currRun->fFieldFlags[ifld->first]);
               //printf("XXX found field: %s = |%s|\n", ifld->first.c_str(), ifld->second.c_str());

            }
         }
      }
   }

   // Insert the last run in container
   //gDBRuns.insert(*currRun);
   //delete currRun;

   return 0;
}


/** */
void AsymRunDB::DeleteRun(std::string runName)
{
   char   *line = NULL;
   size_t  len  = 0;
   ssize_t nBytes = 0;
   bool    skipLine = false;
   ofstream newRunDbFile;
   newRunDbFile.open("run33.db");

   while (true) {

      ssize_t nb = getline(&line, &len, fDbFile);
      if (nb < 0) break;

      nBytes += nb;

      string sline(line);
      //cout << "line: " << sline;

      TObjArray *subStrL = TPRegexp("\\[([\\w,\\W]+)\\]").MatchS(sline);

      if (subStrL->GetEntriesFast() < 2) {
         delete subStrL;
         if (!skipLine) newRunDbFile << sline;
         continue;
      }

      TString subStr = ((TObjString *) subStrL->At(1))->GetString();
      delete subStrL;

      if (subStr == runName) {
         //printf("XXX found %s\n", runName.c_str());
         skipLine = true;
      } else
         skipLine = false;

      if (!skipLine) newRunDbFile << sline;
   }

   newRunDbFile.close();
}


/** */
void AsymRunDB::WriteRun(TStructRunDB *dbrun)
{
}


/** */
AsymRunDB::~AsymRunDB()
{
   fclose(fDbFile);
}


//
// Class name  :
// Method name : readdb(double RUNID)
//
// Description : Read conditions from run.db
// Input       : double RUNID
// Return      : int 1
//
int readdb(double RUNID)
{
   // read DB file
   char *dbfile = "run.db";
   FILE *in_file;

   if ((in_file = fopen(dbfile, "r")) == NULL) {
       printf("ERROR: %s file not found. Force exit.\n", dbfile);;
       exit(-1);
   }

   char   *line = NULL;
   size_t  len  = 0;
   int match    = 0;
   string s;
   ssize_t read;

   while ((read = getline(&line, &len, in_file)) != -1) {

      string str(line);
      //cout << str;

      if (str[0] == '[') { // Get Run Number

        if (str[9] == ']') {
          s = str.substr(1,8) ;
        } else if (str[10] == ']') {
          s = str.substr(1,9) ;
        } else {
          printf("AsymRunDB:ERROR invalid [RunID] statement in run.db. Ignored.");
        }

        rundb.RunID = strtod(s.c_str(),NULL);
        //      printf("AsymRunDB: %.3f\n",rundb.RunID);
        match = MatchPolarimetry(RUNID, rundb.RunID);

        if (match){
           //currRun.RunID = rundb.RunID;

           if (RUNID<rundb.RunID) break;
        }

      } else {
         if (match) {
            // a "*" after the flag name means only apply the flag to this run
            if(str.find("*=") == string::npos || RUNID == rundb.RunID)
            {
               if (str.find("RESET_ALL=Default")   == 1) SetDefault();
               if (str.find("CONFIG")              == 1) rundb.config_file_s         = GetVariables(str);
               if (str.find("MASSCUT")             == 1) rundb.masscut_s             = GetVariables(str);
               if (str.find("TSHIFT")              == 1) rundb.tshift_s              = GetVariables(str);
               if (str.find("ALPHA_CALIB_RUN_NAME=") == 1) rundb.alpha_calib_run_name = GetVariables(str);
               if (str.find("ENERGY_CALIB=")       == 1) rundb.calib_file_s          = GetVariables(str);
               if (str.find("INJ_TSHIFT")          == 1) rundb.inj_tshift_s          = GetVariables(str);
               if (str.find("RUN_STATUS")          == 1) rundb.run_status_s          = GetVariables(str);
               if (str.find("MEASUREMENT_TYPE")    == 1) rundb.measurement_type_s    = GetVariables(str);
               if (str.find("DEFINE_SPIN_PATTERN") == 1) rundb.define_spin_pattern_s = GetVariables(str);
               if (str.find("DEFINE_FILL_PATTERN") == 1) rundb.define_fill_pattern_s = GetVariables(str);
               if (str.find("REFERENCE_RATE")      == 1) rundb.reference_rate_s      = GetVariables(str);
               if (str.find("TARGET_COUNT_MM")     == 1) rundb.target_count_mm_s     = GetVariables(str);
               if (str.find("COMMENT")             == 1) rundb.comment_s             = GetVariables(str);
               if (str.find("DisableBunch")        == 1) {
                  rundb.disable_bunch_s     = GetVariables(str);
                  BunchHandler(atoi(rundb.disable_bunch_s.c_str()), 1);
               }
               if (str.find("EnableBunch")         ==1) {
                  rundb.enable_bunch_s      = GetVariables(str);
                  BunchHandler(atoi(rundb.enable_bunch_s.c_str()),-1);
               }
               if (str.find("DisableStrip")        ==1) {
                  rundb.disable_strip_s     = GetVariables(str);
                  StripHandler(atoi(rundb.disable_strip_s.c_str()), 1);
               }
               if (str.find("EnableStrip")         ==1) {
                  rundb.enable_strip_s      = GetVariables(str);
                  StripHandler(atoi(rundb.enable_strip_s.c_str()),-1);
               }
            }
         }
      }
   }

   // Find Disable Strip List
   runinfo.NDisableStrip = FindDisableStrip();

   // Find Disable Bunch List
   runinfo.NDisableBunch = FindDisableBunch();
   if (runinfo.NDisableBunch) Flag.mask_bunch = 1;

   // processing conditions
   if (!extinput.CONFIG){
     strcat(reConfFile, confdir);
     strcat(reConfFile,     "/");
     strcat(reConfFile, rundb.config_file_s.c_str());
   }

   // calib directories
   calibdir = getenv("CALIBDIR");

   if ( calibdir == NULL ){
      cerr << "environment CALIBDIR is not defined" << endl;
      cerr << "e.g. export CALIBDIR=/usr/local/cnipol/calib" << endl;
      //exit(-1);
      strcpy(calibdir, ".");
   }

   strcat(CalibFile, calibdir);
   strcat(CalibFile, "/");
   strcat(CalibFile, rundb.calib_file_s.c_str());

   // Mass Cut sigma
   if (!extinput.MASSCUT)
     dproc.MassSigma = strtof(rundb.masscut_s.c_str(),NULL);

   // TSHIFT will be cumulated TSHIFT from run.db and -t option
   dproc.tshift  += strtof(rundb.tshift_s.c_str(),NULL);

   // TSHIFT for injection with respect to flattop timing
   dproc.inj_tshift = strtof(rundb.inj_tshift_s.c_str(),NULL);

   // Expected universal rate for given target
   dproc.reference_rate = strtof(rundb.reference_rate_s.c_str(),NULL);

   // Target count/mm conversion
   dproc.target_count_mm = strtof(rundb.target_count_mm_s.c_str(),NULL);

   // Optimize setting for Run
   if ((RUNID>=6500)&&(RUNID<7400)) { // Run05
      runinfo.Run=5;
      for (int i=0; i<NSTRIP; i++) phi[i] = phiRun5[i];
   } else if (RUNID>=7400) { // Run06
      runinfo.Run=6;
      for (int i=0; i<NSTRIP; i++) phi[i] = phiRun6[i];
   } else if (RUNID>=10018) { // Run09
      runinfo.Run=9;
   }

   // Spin Pattern Recoverly
   Flag.spin_pattern = atoi(rundb.define_spin_pattern_s.c_str());

   // Fill Pattern Recoverly
   Flag.fill_pattern = atoi(rundb.define_fill_pattern_s.c_str());

   // VERBOSE mode
   if (Flag.VERBOSE) PrintRunDB();

   return 1;
}


/** */
void PrintDB()
{
   printf("Run DB: %d\n", gDBRuns.size());

   DbRunSet::iterator ir;
   DbRunSet::iterator br = gDBRuns.begin();
   DbRunSet::iterator er = gDBRuns.end();

   for (ir=br; ir!=er; ir++) {
      printf("\n\n");
      ir->Print();
   }
}


//
// Class name  :
// Method name : StripHandler(int st, int flag)
//
// Description : handle enable/disable strips
//             : Any strips ended up with ProcessStrip[st]>0 are disabled.
// Input       : int st, int enable/disable flag
// Return      :
//
int StripHandler(int st, int flag)
{
  static int Initiarize = 1;
  if (Initiarize) for (int i=0; i<NSTRIP; i++) ProcessStrip[i]=0;

  ProcessStrip[st-1] += flag;

  Initiarize=0;

  return 0;
}


//
// Class name  :
// Method name : SetDefault()
//
// Description : Reset Active strip and bunch to be dafault.
//             : The default is currently all active
// Input       :
// Return      :
//
int SetDefault()
{
  cout << "______________ Reset Active Strip and Bunch to default _______________" << endl;

  // initialize strip arrays
  for (int i=0; i<NSTRIP; i++) {
    ProcessStrip[i]         = 0;
    runinfo.ActiveStrip[i]  = 1;
    runinfo.DisableStrip[i] = 0;
    runinfo.NActiveStrip    = NSTRIP;
    runinfo.NDisableStrip   = 0;
  }

  // initialize bunch arrays
  for (int i=0; i<NBUNCH; i++) {
    ProcessBunch[i]         = 0;
    runinfo.DisableBunch[i] = 0;
    runinfo.NActiveBunch    = 0;
    runinfo.NDisableBunch   = 0;
  }

  return 0;
}

//
// Class name  :
// Method name : FindDisableStrip()
//
// Description : This subtoutine is under construction (April 4, 06)
// Input       :
// Return      : NDisableStrip
//
int FindDisableStrip()
{
  int NDisableStrip=0;
  for (int i=0;i<NSTRIP; i++) {
    if (ProcessStrip[i]>0) {
      runinfo.DisableStrip[NDisableStrip] = i;
      NDisableStrip++;
    }
  }

  return NDisableStrip;
}


//
// Class name  :
// Method name : BunchHandler(int bunch, int flag)
//
// Description : handle enable/disable bunch
//             : Any bunches ended up with ProcessBunch[bid]>0 are disabled.
// Input       : int bunch, int enable/disable flag
// Return      :
//
int BunchHandler(int bunch, int flag)
{
  static int Initiarize = 1;
  if (Initiarize) for (int i=0; i<NBUNCH; i++) ProcessBunch[i]=0;

  ProcessBunch[bunch] += flag;

  Initiarize=0;

  return 0;
}


//
// Class name  :
// Method name : FindDisableBunch()
//
// Description : dump disabled bunches into runinfo.DisableBunch array
// Input       :
// Return      : NDisableBunch
//
int FindDisableBunch()
{
  int NDisableBunch=0;
  for (int i=0;i<NBUNCH; i++) {
    if (ProcessBunch[i]>0) {
      runinfo.DisableBunch[NDisableBunch] = i;
      NDisableBunch++;
    }
  }

  return NDisableBunch;
}



//
// Class name  :
// Method name : MatchPolarimetry
//
// Description : Match  the Polarimety ID between ThisRunID and RefRunID
// Input       : double ThisRunID, double RefRunID
// Return      : 1 if ThisRunID and RunID match Polarimetry ID
//
int MatchPolarimetry(double ThisRunID, double RefRunID)
{
   struct runid {
      char runid[10];
      char * polarimetry_id;
   } This, Ref;

   sprintf(This.runid, "%.3f", ThisRunID);
   This.polarimetry_id = strrchr(This.runid,'.') + 1;

   sprintf(Ref.runid, "%.3f", RefRunID);
   Ref.polarimetry_id = strrchr(Ref.runid,'.') +1  ;

   int match = (*This.polarimetry_id-*Ref.polarimetry_id) ? 0  : 1 ;

   return match;
}


//
// Class name  :
// Method name : ContinueScan(double ThisRunID, double RunID)
//
// Description : compare ThisRunID and RunID and determine wheather
//             : to be coninued scanning run.db
// Input       : double ThisRunID, double RunID
// Return      : 1 if ThisRunID <= RunID to pickup up-to-date or matching conditions
//             : 0 otherwiser
//
int ContinueScan(double ThisRunID, double RunID)
{
  int Continue = 1;

  struct runid {
    int Fill;
    int Run;
  } ;

  runid THIS;
  THIS.Fill = int(ThisRunID);
  THIS.Run = int((ThisRunID-THIS.Fill)*1e3+0.001);
  //                                       ^^^^^
  // this is so ugry but otherwise THIS.Run will be -1 of what it supposed to be

  runid THAT;
  THAT.Fill = int(RunID);
  THAT.Run = int((RunID-THAT.Fill)*1e3+0.001);

  if (THIS.Fill>=THAT.Fill){
    if (THIS.Run<THAT.Run) Continue = 0 ;
  }

  return Continue;
}


//
// Class name  :
// Method name : PrintRunDB()
//
// Description : Printout final varibles read from run.db
//             : for Debugging
// Input       :
// Return      :
//             :
//
void PrintRunDB()
{
  printf("Run Status       = %s\n",    rundb.run_status_s.c_str());
  printf("Config File      = %s\n",    rundb.config_file_s.c_str());
  printf("Calib File       = %s\n",    rundb.calib_file_s.c_str());
  printf("Mass Cut         = %5.1f\n", strtof(rundb.masscut_s.c_str(),NULL));
  printf("TSHIFT           = %5.1f\n", strtof(rundb.tshift_s.c_str(),NULL));
  printf("INJ_TSHIFT       = %5.1f\n", strtof(rundb.inj_tshift_s.c_str(),NULL));
  printf("MEASUREMENT_TYPE = %5.1f\n", strtof(rundb.measurement_type_s.c_str(),NULL));
  printf("REFERENCE_RATE   = %7.3f\n", strtof(rundb.reference_rate_s.c_str(),NULL));
  printf("TARGET_COUNT_MM  = %8.5f\n", strtof(rundb.target_count_mm_s.c_str(),NULL));
  printf("COMMENT          = %s\n",    rundb.comment_s.c_str());

  return;
}


// =====================================
// Print Out Configuration information
// =====================================
int printConfig(recordConfigRhicStruct *cfginfo)
{
    int ccutwu;
    int ccutwl;

    fprintf(stdout,"================================================\n");
    fprintf(stdout,"===  RHIC Polarimeter Configuration (BGN)    ===\n");
    fprintf(stdout,"================================================\n");

    // Configulation File
    fprintf(stdout,"         RUN STATUS = %s\n", rundb.run_status_s.c_str());
    fprintf(stdout,"         MEAS. TYPE = %s\n", rundb.measurement_type_s.c_str());
    fprintf(stdout,"             CONFIG = %s\n", reConfFile);
    fprintf(stdout,"              CALIB = %s\n", CalibFile);

    // banana cut configulation
    if (dproc.CBANANA == 0) {
        ccutwl = (int)cfginfo->data.chan[3].ETCutW;
        ccutwu = (int)cfginfo->data.chan[3].ETCutW;
    } else if (dproc.CBANANA == 2) {
      fprintf(stdout,"            MASSCUT = %.1f\n",dproc.MassSigma);
    } else {
        ccutwl = (int)dproc.widthl;
        ccutwu = (int)dproc.widthu;
    }
    if (dproc.CBANANA!=2)
      fprintf (stdout,"Carbon cut width : (low) %d (up) %d nsec \n",ccutwl,ccutwu);

    // tshift in [ns]
    fprintf(stdout,"             TSHIFT = %.1f\n",dproc.tshift);

    // expected reference rate
    if (runinfo.Run==5)   fprintf(stdout,"     REFERENCE_RATE = %.4f\n",dproc.reference_rate);

    // target count/mm
    fprintf(stdout,"    TARGET_COUNT_MM = %.5f\n",dproc.target_count_mm);

    // Disabled bunch
    fprintf(stdout,"      #DisableBunch = %d\n", runinfo.NDisableBunch);
    if (runinfo.NDisableBunch){
      fprintf(stdout,"       DisableBunch = ");
      for (int i=0;i<runinfo.NDisableBunch;i++) printf("%d ",runinfo.DisableBunch[i]);
      printf("\n");
    }

    // Disabled strips
    fprintf(stdout,"      #DisableStrip = %d\n", runinfo.NDisableStrip);
    if (runinfo.NDisableStrip){
      fprintf(stdout,"       DisableStrip = ");
      for (int i=0;i<runinfo.NDisableStrip;i++) printf("%d ",runinfo.DisableStrip[i]+1);
      printf("\n");
    }

    // Active Detector and Strip Configulation
    printf("    Active Detector =");
    for (int i=0; i<NDETECTOR; i++)  printf(" %1d", runinfo.ActiveDetector[i] ? 1 : 0 );
    printf("\n");
    //    printf("Active Strip Config =");
    //    for (int i=NDETECTOR-1; i>=0; i--) printf(" %x", runinfo.ActiveDetector[i]);
    //    printf("\n");
    printf("Active Strip Config =");
    for (int i=0; i<NSTRIP; i++) {
      if (i%NSTRIP_PER_DETECTOR==0) printf(" ");
      printf("%d", runinfo.ActiveStrip[i]);
    }
    printf("\n");

    // print comment
    if (strlen(rundb.comment_s.c_str())>3)
      printf("            COMMENT = %s\n",    rundb.comment_s.c_str());

    fprintf(stdout,"================================================\n");
    fprintf(stdout,"===  RHIC Polarimeter Configuration (END)    ===\n");
    fprintf(stdout,"================================================\n");

    return(0);
}


//
// Class name  :
// Method name : GetVariables(string str)
//
// Description : extract variables (between characters "=" and ";") from string.
// Input       : string str
// Return      : string variables
//
string GetVariables(string str)
{
  string::size_type begin  = str.find("=") + 1;
  string::size_type end    = str.find(";");
  string::size_type length = end - begin ;

  string s = str.substr(begin,length);
  return s;
}


/*
float
GetVariablesFloat(string str){

  string::size_type begin = str.find("=")+ 1;
  string::size_type end = str.find(";");
  string::size_type length = end - begin ;

  string s = str.substr(begin,length);
  return strtof(s.c_str(),NULL);

}
*/


/** */
TBuffer & operator<<(TBuffer &buf, TStructRunDB *&rec)
{
   if (!rec) return buf;
   //printf("operator<<(TBuffer &buf, TStructRunDB *rec) : \n");
   rec->Streamer(buf);
   return buf;
}


/** */
TBuffer & operator>>(TBuffer &buf, TStructRunDB *&rec)
{
   //if (!rec) return buf;
   //printf("operator<<(TBuffer &buf, TStructRunDB *rec) : \n");
   // if object has been created already delete it
   //free(rec);
   //rec = (TStructRunDB *) realloc(rec, sizeof(TStructRunDB ));
   rec->Streamer(buf);
   return buf;
}


/** */
void TStructRunDB::Streamer(TBuffer &buf)
{
   if (buf.IsReading()) {
      TString tstr;
      //printf("reading TStructRunDB::Streamer(TBuffer &buf) \n");
      buf >> RunID;
      buf >> isCalibRun;
      buf >> tstr; calib_file_s      = tstr.Data();
      buf >> tstr; alpha_calib_run_name = tstr.Data();
      buf >> tstr; config_file_s     = tstr.Data();
   } else {
      TString tstr;
      //printf("writing TStructRunDB::Streamer(TBuffer &buf) \n");
      buf << RunID;
      buf << isCalibRun;
      tstr = calib_file_s;      buf << tstr;
      tstr = alpha_calib_run_name; buf << tstr;
      tstr = config_file_s;     buf << tstr;
   }
}


bool TStructRunDB::operator()(const TStructRunDB &rec1, const TStructRunDB &rec2) const
{
  return (rec1.RunID < rec2.RunID);
}


/** */
void TStructRunDB::Print(const Option_t* opt) const
{ //{{{
   //printf("RunID:             %f\n", RunID);
   //printf("isCalibRun:        %d\n", isCalibRun);
   //printf("calib_file_s:      %s\n", calib_file_s.c_str());
   //printf("alpha_calib_run_name: %s\n", alpha_calib_run_name.c_str());
   //printf("config_file_s:     %s\n", config_file_s.c_str());
   //printf("masscut_s:         %s\n", masscut_s.c_str());
   //printf("comment_s:         %s\n", comment_s.c_str());
   //cout << "RunID: " <<

   PrintAsDbEntry();

} //}}}


/** */
void TStructRunDB::UpdateFields(TStructRunDB *dbrun)
{
   DbFieldMap::iterator ifld;
   DbFieldMap::iterator bfld = dbrun->fFields.begin();
   DbFieldMap::iterator efld = dbrun->fFields.end();

   for (ifld=bfld; ifld!=efld; ifld++) {
      string field_name  = ifld->first;
      string field_value = ifld->second;

      bool flag_from = dbrun->fFieldFlags[field_name];
      bool flag_to   = fFieldFlags[field_name];

      if (!flag_from && !flag_to && field_value.size() > 0) {
         fFields[field_name] = field_value;
      }
   }
}


/** */
void TStructRunDB::PrintAsPhp(FILE *f) const
{ //{{{
   fprintf(f, "$rc['calib_file_s']                 = \"%s\";\n", calib_file_s.c_str());
   fprintf(f, "$rc['alpha_calib_run_name']         = \"%s\";\n", alpha_calib_run_name.c_str());
   fprintf(f, "$rc['config_file_s']                = \"%s\";\n", config_file_s.c_str());
} //}}}
