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

TStructRunDB rundb;
TStructRunDB gCurrentRunInfo;
DBRunSet     gDBRuns; // container for run info read from run.db

int ProcessStrip[NSTRIP];
int ProcessBunch[NBUNCH];

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

   if ((in_file = fopen(dbfile,"r")) == NULL) {
       printf("ERROR: %s file not found. Force exit.\n",dbfile);;
       exit(-1);
   }
 
   char   *line = NULL;
   size_t  len  = 0;
   int match    = 0;
   string s;
   ssize_t read;
   TStructRunDB *currRun;
   currRun = new TStructRunDB();
   bool isFirstRunInFile = true;
 
   while ((read = getline(&line, &len, in_file)) != -1) {
 
      string str(line);
      //cout << str;

      if (TPRegexp("\\[([\\w,\\W]+)\\]").MatchB(str)) {
      
         //cout << "XXX matched" << endl;

         if (!isFirstRunInFile) {
            //printf("\nsaved prev run\n");
            //currRun->Print();
            //printf("\n^^^^^saved prev run\n");
            gDBRuns.insert(*currRun);
            delete currRun;
            currRun = new TStructRunDB();
         } else {
            isFirstRunInFile = false;
         }

         TObjArray *subStrL = TPRegexp("\\[([\\w,\\W]+)\\]").MatchS(str);
         TString subStr = ((TObjString *) subStrL->At(1))->GetString();

         //printf("XXX found %s\n", subStr.Data());

         currRun->RunID = subStr.Atof();
         //currRun->RunID = 0;
      } else {

         if (str.find("CONFIG") == 1)                  
            currRun->config_file_s     = GetVariables(str);
         if (str.find("ENERGY_CALIB_ROOT_FILE=") == 1) 
            currRun->alpha_calib_run_name = GetVariables(str);
         if (str.find("ENERGY_CALIB=")       == 1)     
            currRun->calib_file_s      = GetVariables(str);
         if (str.find("COMMENT")             == 1)
            currRun->comment_s      = GetVariables(str);
      }
 
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
            if(str.find("*=") == -1 || RUNID == rundb.RunID)
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

   // Insert the last run in container
   gDBRuns.insert(*currRun);
   delete currRun;
 
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
     exit(-1);
   }

   strcat(CalibFile,calibdir);
   strcat(CalibFile,    "/");
   strcat(CalibFile,rundb.calib_file_s.c_str());
 
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

   DBRunSet::iterator ir;
   DBRunSet::iterator br = gDBRuns.begin();
   DBRunSet::iterator er = gDBRuns.end();

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
int
SetDefault(){

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
int
ContinueScan(double ThisRunID, double RunID){

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

}//end-of-ContinueScan()



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
// Return      : const char* variables
//
/*
const char *
GetVariables(string strdyn){

  string str=strdyn;
  string::size_type begin = str.find("=")+ 1;
  string::size_type end = str.find(";");
  string::size_type length = end - begin ;

  string s = str.substr(begin,length);
  return s.c_str();

  }*/


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
