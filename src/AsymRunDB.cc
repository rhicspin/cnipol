//  Asymmetry Analysis of RHIC pC Polarimeter
//  file name:    AsymRunDB.cc
//
//  Authors:      Itaru Nakagawa
//                Dmitri Smirnov
//
//  Creation:     11/18/2005
//

#include <sstream>

#include "AsymRunDB.h"

using namespace std;

int ProcessStrip[NSTRIP];
int ProcessBunch[NBUNCH];


/**
 * Update common fields in  fCommonRunDb entries using the values from dbrun.
 * dbrun is expected to have the POLARIMETER_ID set
 */
void AsymRunDB::UpdateCommonFields(TStructRunDB &dbrun)
{
   // First check the polarimeter id
   if (dbrun.fFields["POLARIMETER_ID"] != "none") {
      dbrun.fPolId = atoi(dbrun.fFields["POLARIMETER_ID"].c_str());
   } else {
      TObjArray *subStrL = TPRegexp("\\d+\\.(\\d)\\d{2}").MatchS(dbrun.fRunName);

      if (subStrL->GetEntriesFast() < 1) {
         printf("Fatal error: no pol id\n");
         exit(-1);
      }

      TString spolid = ((TObjString *) subStrL->At(1))->GetString();
      delete subStrL;

      dbrun.fPolId = spolid.Atoi();
      dbrun.fFields["POLARIMETER_ID"] = spolid;
   }

   if (dbrun.fFields["START_TIME"] != "none") {
      dbrun.timeStamp = atoi(dbrun.fFields["START_TIME"].c_str());
   }

   //if (dbrun.fPolId == UCHAR_MAX)

   //printf("polId: %d\n", dbrun.fPolId);

   if (!fCommonRunDB[dbrun.fPolId]) {
      TStructRunDB *cr = new TStructRunDB();

      cr->fFields["POLARIMETER_ID"] = " ";
      sprintf(&cr->fFields["POLARIMETER_ID"][0], "%1d", dbrun.fPolId);

      cr->fPolId = dbrun.fPolId;
      fCommonRunDB[dbrun.fPolId] = cr;
   }

   fCommonRunDB[dbrun.fPolId]->UpdateFields(dbrun);
}


/** */
void AsymRunDB::DropCommonFields(TStructRunDB *dbrun)
{
   TStructRunDB *cr = fCommonRunDB[dbrun->fPolId];

   if (!cr) { return; }

   DbFieldMap::iterator ifld;
   DbFieldMap::iterator bfld = dbrun->fFields.begin();
   DbFieldMap::iterator efld = dbrun->fFields.end();

   for (ifld=bfld; ifld!=efld; ifld++) {

      string &field_name  = (string&) ifld->first;
      string &field_value = (string&) ifld->second;
      string &common_field_value = cr->fFields[field_name];

      bool isPrivate = dbrun->fFieldFlags[field_name];
      //bool flag_to   = fFieldFlags[field_name];

      if (!isPrivate && field_value != "none" &&
           field_value == common_field_value)
      {
         field_value = "none";
      }
   }
}


/** */
void AsymRunDB::PrintCommon()
{
   printf("\nCommon DB runs:\n");

   DbCommonRunMap::iterator icr;
   DbCommonRunMap::iterator bcr = fCommonRunDB.begin();
   DbCommonRunMap::iterator ecr = fCommonRunDB.end();

   for (icr=bcr; icr!=ecr; icr++) {
      //fprintf(f, "%s = |%s|\n", ifld->first.c_str(), ifld->second.c_str());
      if (icr->second) icr->second->Print();
   }
}


const UShort_t AsymRunDB::sNFields = 31;

const char* AsymRunDB::sFieldNames[] = {
	"RESET_ALL", "POLARIMETER_ID", "POLARIZATION",
   "MEASUREMENT_TYPE", "MASSCUT", "TSHIFT",
   "INJ_TSHIFT", "ENERGY_CALIB", "DL_CALIB_RUN_NAME",
   "ALPHA_CALIB_RUN_NAME", "CONFIG", "DEFINE_SPIN_PATTERN",
   "DEFINE_FILL_PATTERN", "REFERENCE_RATE", "TARGET_COUNT_MM", "COMMENT",
   "DISABLED_BUNCHES", "EnableBunch", "DISABLED_CHANNELS", "EnableStrip", "DisabledBunches", "DisabledStrips",
   "RUN_STATUS", "START_TIME", "STOP_TIME", "NEVENTS_TOTAL",
   "NEVENTS_PROCESSED", "BEAM_ENERGY", "TARGET_ID", "ASYM_VERSION", "PROFILE_RATIO"};


/** */
AsymRunDB::AsymRunDB() : TObject(), fDbFileName("run.db")
{
   if ( !dproc.fAsymEnv["CNIPOL_DIR"].empty() )
      fDbFileName = dproc.fAsymEnv["CNIPOL_DIR"] + "/" + fDbFileName;

   //fDbFile = fopen(fDbFileName.c_str(), "r");

   //if (!fDbFile) {
   //   Error("AsymRunDB", "%s file not found. Force exit", fDbFileName.c_str());
   //   exit(-1);
   //}

   for (UShort_t i=0; i<NPOLARIMETERS; i++) {
      fCommonRunDB[i] = 0;
   }
}


/** */
AsymRunDB::~AsymRunDB()
{
   //if (fDbFile) fclose(fDbFile);
   Clear();
}


/** */
void AsymRunDB::Clear()
{
   //if (fDbFile) fclose(fDbFile);

   // There must be a more elegant way for erasing pointers
   DbCommonRunMap::iterator icr;
   DbCommonRunMap::iterator bcr = fCommonRunDB.begin();
   DbCommonRunMap::iterator ecr = fCommonRunDB.end();

   for (icr=bcr; icr!=ecr; icr++) {
      delete icr->second;
      icr->second = 0;
   }

   fCommonRunDB.clear();
   fDBRuns.clear();
}


/** */
TStructRunDB* AsymRunDB::Select(std::string runName)
{
   fDbFile = fopen(fDbFileName.c_str(), "r");

   if (!fDbFile) {
      Error("AsymRunDB", "%s file not found. Force exit", fDbFileName.c_str());
      exit(-1);
   }

   char    *line   = NULL;
   //UInt_t   iLine  = 1;
   size_t   len    = 0;
   ssize_t  nBytes = 0;
   TStructRunDB *currentRun = 0;

   while (true) {

      ssize_t nb = getline(&line, &len, fDbFile);
      nBytes += nb;

      // if reached the end of file
      //if (nb < 0) { if (!currentRun) return 0; }

      string sline(line);
      //cout << setw(8) << iLine++ << " (" << setw(8) << nb << "): " << sline;

      // Skip blank and comment lines
      if ( TPRegexp("^(\\s*)//(.*)$").MatchB(sline) )// ||
           //TPRegexp("^(\\s*)$").MatchB(sline) )
      { continue; }

      if (TPRegexp("\\[([\\w,\\W]+)\\]").MatchB(sline) || nb < 0) {

         if (currentRun) {
            //currentRun->UpdateValues();
            UpdateCommonFields(*currentRun);
            //PrintCommon();
            //printf("--- %s, %s\n", runName.c_str(), currentRun->fRunName.c_str());

            if (currentRun->fRunName == runName) {
               printf("Found run %s\n", runName.c_str());
               currentRun->UpdateFields(*fCommonRunDB[currentRun->fPolId]);

               fclose(fDbFile);
               return currentRun;

            } else if (runName == "") {

               fDBRuns.insert(*currentRun);
               delete currentRun;

               if (nb < 0) { fclose(fDbFile); return 0; }

            } else if (nb < 0) {
               delete currentRun;
               fclose(fDbFile);
               return 0;
            } else {
               delete currentRun;
            }

         } else if (nb < 0) {
            delete currentRun;
            fclose(fDbFile);
            return 0;
         }

         TObjArray *subStrL = TPRegexp("\\[([\\w,\\W]+)\\]").MatchS(sline);
         TString subStr = ((TObjString *) subStrL->At(1))->GetString();
         delete subStrL;

         //printf("XXX found %s\n", subStr.Data());

         currentRun = new TStructRunDB();
         currentRun->fRunName = subStr;

      } else if (currentRun) {
         currentRun->ProcessLine(sline);
      }
   }

   // if ever get here return 0
   return 0;
}


/** */
void AsymRunDB::Delete(std::string runName)
{
   fDbFile = fopen(fDbFileName.c_str(), "r");

   if (!fDbFile) {
      Error("AsymRunDB", "%s file not found. Force exit", fDbFileName.c_str());
      exit(-1);
   }

   string newDbFileName = "run99.db";
   char   *line = NULL;
   size_t  len  = 0;
   ssize_t nBytes = 0;
   bool    skipLine = false;
   ofstream newDbFile;
   newDbFile.open(newDbFileName.c_str());

   while (true) {

      ssize_t nb = getline(&line, &len, fDbFile);
      if (nb < 0) break;

      nBytes += nb;

      string sline(line);
      //cout << "line: " << sline;

      TObjArray *subStrL = TPRegexp("\\[([\\w,\\W]+)\\]").MatchS(sline);

      if (subStrL->GetEntriesFast() < 2) {
         delete subStrL;
         if (!skipLine) newDbFile << sline;
         continue;
      }

      TString subStr = ((TObjString *) subStrL->At(1))->GetString();
      delete subStrL;

      if (subStr == runName) {
         //printf("XXX found %s\n", runName.c_str());
         skipLine = true;
      } else
         skipLine = false;

      if (!skipLine) newDbFile << sline;
   }

   newDbFile.close();
   fclose(fDbFile);

   //remove(fDbFileName.c_str());

   int errcode = rename(newDbFileName.c_str(), fDbFileName.c_str());
   //printf("outfile %s\n", fDbFileName.c_str());

   if (errcode == 0)
      printf("DB file was updated\n");
   else
      printf("ERROR: DB file was NOT updated: %s\n", strerror(errno));
}


/** */
void AsymRunDB::Dump()
{
   ofstream dbFile;
   dbFile.open(fDbFileName.c_str(), ios_base::out | ios_base::trunc);

   // Remove all common info
   DbCommonRunMap::iterator icr;
   DbCommonRunMap::iterator bcr = fCommonRunDB.begin();
   DbCommonRunMap::iterator ecr = fCommonRunDB.end();

   for (icr=bcr; icr!=ecr; icr++) {
      delete icr->second;
      icr->second = 0;
   }

   DbRunSet::iterator ir;
   DbRunSet::iterator br = fDBRuns.begin();
   DbRunSet::iterator er = fDBRuns.end();

   for (ir=br; ir!=er; ir++) {
      if (fCommonRunDB[ir->fPolId]) {
         DropCommonFields((TStructRunDB*) &*ir);
         //printf("Dropping common fields from %s, %d\n", ir->fRunName.c_str(), ir->fPolId);
      }

      ir->PrintAsDbEntry(dbFile);
      //fCommonRunDB->UpdateFields(ir);
      UpdateCommonFields((TStructRunDB&) *ir);
   }

   dbFile.close();
}


/** */
void AsymRunDB::Append(TStructRunDB *dbrun)
{
   TStructRunDB *comRun = fCommonRunDB[dbrun->fPolId];

   // First empty fields if in common block
   DbFieldMap::iterator ifld;
   DbFieldMap::iterator bfld = dbrun->fFields.begin();
   DbFieldMap::iterator efld = dbrun->fFields.end();

   DbFieldMap::const_iterator ifldCom = comRun->fFields.begin();
   DbFieldSaveFlagMap::const_iterator ifldFlag = dbrun->fFieldFlags.begin();

   dbrun->PrintAsDbEntry(cout);

   for (ifld=bfld; ifld!=efld; ifld++, ifldCom++, ifldFlag++) {

      if (ifld->second == ifldCom->second && !ifldFlag->second) // no star and not in common block
         ifld->second = "none";
   }

   ofstream dbFile;
   dbFile.open(fDbFileName.c_str(), ios_base::in|ios_base::ate);

   dbrun->PrintAsDbEntry(dbFile);

   dbFile.close();
}


/** */
void AsymRunDB::Insert(TStructRunDB *dbrun)
{
   DbRunSet::iterator irun = fDBRuns.find(*dbrun);

   if (irun != fDBRuns.end()) { // element exists
      //printf("Updating run %s in database\n", dbrun->fRunName.c_str());
      fDBRuns.erase(irun);
   }
   //else { // element does not exist
   //   printf("XXX dbrun %s not found\n", dbrun->fRunName.c_str());
   //}

   printf("Inserting run \"%s\" in database\n", dbrun->fRunName.c_str());
   fDBRuns.insert(*dbrun);
}


//
// Class name  :
// Method name : readdb(double RUNID)
//
// Description : Read conditions from run.db
// Input       : double RUNID
// Return      : int 1
//
void readdb(double RUNID)
{
   // read DB file
   string dbfile = "run.db";
   FILE *in_file;

   if ((in_file = fopen(dbfile.c_str(), "r")) == NULL) {
       gSystem->Error("   AsymRunDB::readdb", "%s file not found. Force exit", dbfile.c_str());
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

        gRunDb.RunID = strtod(s.c_str(),NULL);
        //      printf("AsymRunDB: %.3f\n",gRunDb.RunID);
        match = MatchPolarimetry(RUNID, gRunDb.RunID);

        if (match){
           //currRun.RunID = gRunDb.RunID;

           if (RUNID<gRunDb.RunID) break;
        }

      } else {
         if (match) {
            // a "*" after the flag name means only apply the flag to this run
            if(str.find("*=") == string::npos || RUNID == gRunDb.RunID)
            {
               if (str.find("RESET_ALL=Default")     == 1) SetDefault();
               if (str.find("CONFIG")                == 1) gRunDb.config_file_s         = GetVariables(str);
               if (str.find("MASSCUT")               == 1) gRunDb.masscut_s             = GetVariables(str);
               if (str.find("TSHIFT")                == 1) gRunDb.tshift_s              = GetVariables(str);
               if (str.find("DL_CALIB_RUN_NAME=")    == 1) gRunDb.dl_calib_run_name     = GetVariables(str);
               if (str.find("ALPHA_CALIB_RUN_NAME=") == 1) gRunDb.alpha_calib_run_name  = GetVariables(str);
               if (str.find("ENERGY_CALIB=")         == 1) gRunDb.calib_file_s          = GetVariables(str);
               if (str.find("INJ_TSHIFT")            == 1) gRunDb.inj_tshift_s          = GetVariables(str);
               if (str.find("RUN_STATUS")            == 1) gRunDb.run_status_s          = GetVariables(str);
               if (str.find("MEASUREMENT_TYPE")      == 1) gRunDb.measurement_type_s    = GetVariables(str);
               if (str.find("DEFINE_SPIN_PATTERN")   == 1) gRunDb.define_spin_pattern_s = GetVariables(str);
               if (str.find("DEFINE_FILL_PATTERN")   == 1) gRunDb.define_fill_pattern_s = GetVariables(str);
               if (str.find("REFERENCE_RATE")        == 1) gRunDb.reference_rate_s      = GetVariables(str);
               if (str.find("TARGET_COUNT_MM")       == 1) gRunDb.target_count_mm_s     = GetVariables(str);
               if (str.find("COMMENT")               == 1) gRunDb.comment_s             = GetVariables(str);
               if (str.find("DISABLED_BUNCHES")          == 1) {
                  gRunDb.disable_bunch_s     = GetVariables(str);
                  BunchHandler(atoi(gRunDb.disable_bunch_s.c_str()), 1);
               }
               if (str.find("EnableBunch")         ==1) {
                  gRunDb.enable_bunch_s      = GetVariables(str);
                  BunchHandler(atoi(gRunDb.enable_bunch_s.c_str()),-1);
               }
               if (str.find("DISABLED_CHANNELS")        ==1) {
                  gRunDb.disable_strip_s     = GetVariables(str);
                  StripHandler(atoi(gRunDb.disable_strip_s.c_str()), 1);
               }
               if (str.find("EnableStrip")         ==1) {
                  gRunDb.enable_strip_s      = GetVariables(str);
                  StripHandler(atoi(gRunDb.enable_strip_s.c_str()),-1);
               }
            }
         }
      }
   }

   // Find Disable Strip List
   gRunInfo.NDisableStrip = FindDisableStrip();

   // Find Disable Bunch List
   gRunInfo.NDisableBunch = FindDisableBunch();
   if (gRunInfo.NDisableBunch) Flag.mask_bunch = 1;

   // processing conditions
   if (!extinput.CONFIG){
     strcat(reConfFile, confdir);
     strcat(reConfFile,     "/");
     strcat(reConfFile, gRunDb.config_file_s.c_str());
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
   strcat(CalibFile, gRunDb.calib_file_s.c_str());

   // Mass Cut sigma
   if (!extinput.MASSCUT)
     dproc.MassSigma = strtof(gRunDb.masscut_s.c_str(),NULL);

   // TSHIFT will be cumulated TSHIFT from run.db and -t option
   dproc.tshift  += strtof(gRunDb.tshift_s.c_str(),NULL);

   // TSHIFT for injection with respect to flattop timing
   dproc.inj_tshift = strtof(gRunDb.inj_tshift_s.c_str(),NULL);

   // Expected universal rate for given target
   dproc.reference_rate = strtof(gRunDb.reference_rate_s.c_str(),NULL);

   // Target count/mm conversion
   dproc.target_count_mm = strtof(gRunDb.target_count_mm_s.c_str(),NULL);

   // Optimize setting for Run
   if ((RUNID>=6500)&&(RUNID<7400)) { // Run05
      gRunInfo.Run=5;
      for (int i=0; i<NSTRIP; i++) gPhi[i] = phiRun5[i];
   } else if (RUNID>=7400) { // Run06
      gRunInfo.Run=6;
      for (int i=0; i<NSTRIP; i++) gPhi[i] = phiRun6[i];
   } else if (RUNID>=10018) { // Run09
      gRunInfo.Run=9;
   }

   // Spin Pattern Recoverly
   Flag.spin_pattern = atoi(gRunDb.define_spin_pattern_s.c_str());

   // Fill Pattern Recoverly
   Flag.fill_pattern = atoi(gRunDb.define_fill_pattern_s.c_str());

   // VERBOSE mode
   if (Flag.VERBOSE) PrintRunDB();
}


/** */
void AsymRunDB::Print(const Option_t* opt) const
{
   printf("DB size: %d\n", fDBRuns.size());

   DbRunSet::iterator ir;
   DbRunSet::iterator br = fDBRuns.begin();
   DbRunSet::iterator er = fDBRuns.end();

   for (ir=br; ir!=er; ir++) {
      ir->Print();
   }
}


// Description : handle enable/disable strips
//             : Any strips ended up with ProcessStrip[st]>0 are disabled.
// Input       : int st, int enable/disable flag
void StripHandler(int st, int flag)
{
   static int Initiarize = 1;
   if (Initiarize) for (int i=0; i<NSTRIP; i++) ProcessStrip[i]=0;
 
   ProcessStrip[st-1] += flag;
   Initiarize = 0;
}


// Description : Reset Active strip and bunch to be dafault.
//             : The default is currently all active
void SetDefault()
{
   cout << "______________ Reset Active Strip and Bunch to default _______________" << endl;
 
   // initialize strip arrays
   for (int i=0; i<NSTRIP; i++) {
      ProcessStrip[i]          = 0;
      gRunInfo.ActiveStrip[i]  = 1;
      gRunInfo.fDisabledChannels[i] = 0;
      gRunInfo.NActiveStrip    = NSTRIP;
      gRunInfo.NDisableStrip   = 0;
   }
 
   // initialize bunch arrays
   for (int i=0; i<NBUNCH; i++) {
      ProcessBunch[i]          = 0;
      gRunInfo.DisableBunch[i] = 0;
      gRunInfo.NActiveBunch    = 0;
      gRunInfo.NDisableBunch   = 0;
   }
}


// Description : This subtoutine is under construction (April 4, 06)
// Input       :
// Return      : NDisableStrip
//
int FindDisableStrip()
{
   int NDisableStrip=0;
   for (int i=0;i<NSTRIP; i++) {
      if (ProcessStrip[i]>0) {
         gRunInfo.fDisabledChannels[NDisableStrip] = i;
         NDisableStrip++;
      }
   }

   return NDisableStrip;
}


// Description : handle enable/disable bunch
//             : Any bunches ended up with ProcessBunch[bid]>0 are disabled.
// Input       : int bunch, int enable/disable flag
void BunchHandler(int bunch, int flag)
{
   static int Initiarize = 1;
   if (Initiarize) for (int i=0; i<NBUNCH; i++) ProcessBunch[i]=0;

   ProcessBunch[bunch] += flag;
   Initiarize = 0;
}


// Description : dump disabled bunches into gRunInfo.DisableBunch array
// Input       :
// Return      : NDisableBunch
int FindDisableBunch()
{
  int NDisableBunch=0;
  for (int i=0;i<NBUNCH; i++) {
    if (ProcessBunch[i]>0) {
      gRunInfo.DisableBunch[NDisableBunch] = i;
      NDisableBunch++;
    }
  }

  return NDisableBunch;
}



// Description : Match  the Polarimety ID between ThisRunID and RefRunID
// Input       : double ThisRunID, double RefRunID
// Return      : 1 if ThisRunID and RunID match Polarimetry ID
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


// Description : Printout final varibles read from run.db
//             : for Debugging
void PrintRunDB()
{
   printf("Run Status       = %s\n",    gRunDb.run_status_s.c_str());
   printf("Config File      = %s\n",    gRunDb.config_file_s.c_str());
   printf("Calib File       = %s\n",    gRunDb.calib_file_s.c_str());
   printf("Mass Cut         = %5.1f\n", strtof(gRunDb.masscut_s.c_str(),NULL));
   printf("TSHIFT           = %5.1f\n", strtof(gRunDb.tshift_s.c_str(),NULL));
   printf("INJ_TSHIFT       = %5.1f\n", strtof(gRunDb.inj_tshift_s.c_str(),NULL));
   printf("MEASUREMENT_TYPE = %5.1f\n", strtof(gRunDb.measurement_type_s.c_str(),NULL));
   printf("REFERENCE_RATE   = %7.3f\n", strtof(gRunDb.reference_rate_s.c_str(),NULL));
   printf("TARGET_COUNT_MM  = %8.5f\n", strtof(gRunDb.target_count_mm_s.c_str(),NULL));
   printf("COMMENT          = %s\n",    gRunDb.comment_s.c_str());
}


// Print Out Configuration information
void printConfig(recordConfigRhicStruct *cfginfo)
{
    int ccutwu;
    int ccutwl;

    fprintf(stdout,"================================================\n");
    fprintf(stdout,"===  RHIC Polarimeter Configuration (BGN)    ===\n");
    fprintf(stdout,"================================================\n");

    // Configulation File
    fprintf(stdout,"         RUN STATUS = %s\n", gRunDb.run_status_s.c_str());
    fprintf(stdout,"         MEAS. TYPE = %s\n", gRunDb.measurement_type_s.c_str());
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
    if (gRunInfo.Run==5)   fprintf(stdout,"     REFERENCE_RATE = %.4f\n",dproc.reference_rate);

    // target count/mm
    fprintf(stdout,"    TARGET_COUNT_MM = %.5f\n",dproc.target_count_mm);

    // Disabled bunch
    fprintf(stdout,"      #DISABLED_BUNCHES = %d\n", gRunInfo.NDisableBunch);
    if (gRunInfo.NDisableBunch){
      fprintf(stdout,"       DISABLED_BUNCHES = ");
      for (int i=0;i<gRunInfo.NDisableBunch;i++) printf("%d ",gRunInfo.DisableBunch[i]);
      printf("\n");
    }

    // Disabled strips
    fprintf(stdout,"      #DISABLED_CHANNELS = %d\n", gRunInfo.NDisableStrip);
    if (gRunInfo.NDisableStrip){
      fprintf(stdout,"       DISABLED_CHANNELS = ");
      for (int i=0;i<gRunInfo.NDisableStrip;i++) printf("%d ", gRunInfo.fDisabledChannels[i]+1);
      printf("\n");
    }

    // Active Detector and Strip Configulation
    printf("    Active Detector =");
    for (int i=0; i<NDETECTOR; i++)  printf(" %1d", gRunInfo.ActiveDetector[i] ? 1 : 0 );
    printf("\n");
    //    printf("Active Strip Config =");
    //    for (int i=NDETECTOR-1; i>=0; i--) printf(" %x", gRunInfo.ActiveDetector[i]);
    //    printf("\n");
    printf("Active Strip Config =");
    for (int i=0; i<NSTRIP; i++) {
      if (i%NSTRIP_PER_DETECTOR==0) printf(" ");
      printf("%d", gRunInfo.ActiveStrip[i]);
    }
    printf("\n");

    // print comment
    if (strlen(gRunDb.comment_s.c_str())>3)
      printf("            COMMENT = %s\n",    gRunDb.comment_s.c_str());

    fprintf(stdout,"================================================\n");
    fprintf(stdout,"===  RHIC Polarimeter Configuration (END)    ===\n");
    fprintf(stdout,"================================================\n");
}


// Description : extract variables (between characters "=" and ";") from string.
// Input       : string str
// Return      : string variables
string GetVariables(string str)
{
  string::size_type begin  = str.find("=") + 1;
  string::size_type end    = str.find(";");
  string::size_type length = end - begin ;

  return str.substr(begin,length);
}


/*
float GetVariablesFloat(string str){

  string::size_type begin = str.find("=")+ 1;
  string::size_type end = str.find(";");
  string::size_type length = end - begin ;

  string s = str.substr(begin,length);
  return strtof(s.c_str(),NULL);

}
*/
