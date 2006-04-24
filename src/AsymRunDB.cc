//  Asymmetry Analysis of RHIC pC Polarimeter
//  file name :   AsymRunDB.cc
// 
// 
//  Author    :   Itaru Nakagawa
//  Creation  :   11/18/2005
//                

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
using namespace std;
#include <iostream.h>
#include <fstream.h>
#include "rhicpol.h"
#include "rpoldata.h"
#include "Asym.h"
#include "AsymRecover.h"
#include "AsymRunDB.h"



//
// Class name  :
// Method name : readdb(double RUNID)
//
// Description : Read conditions from run.db
// Input       : double RUNID
// Return      : int 1
//
int 
readdb(double RUNID) {

  // run DB file
  char *dbfile="run.db";
  FILE * in_file;
  if ((in_file = fopen(dbfile,"r")) == NULL) {
      printf("ERROR: %s file not found. Force exit.\n",dbfile);;
      exit(-1);
  } 

  string s;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  int match=0;
  while ((read = getline(&line, &len, in_file)) != -1){
    
    string str(line);
    if (str[0] == '[') { // Get Run Number
      s = str.substr(1,8);
      rundb.RunID = strtod(s.c_str(),NULL);
      //printf("%8.3f\n",rundb.RunID);
      match = MatchBeam(RUNID,rundb.RunID);
      if (match){
	if (RUNID<rundb.RunID) break;
      }
    }else{
      if (match){
	if (str.find("CONFIG")              ==1) rundb.config_file_s         = GetVariables(str);
	if (str.find("MASSCUT")             ==1) rundb.masscut_s             = GetVariables(str);
	if (str.find("TSHIFT")              ==1) rundb.tshift_s              = GetVariables(str);
	if (str.find("ENERGY_CALIB")        ==1) rundb.calib_file_s          = GetVariables(str);
	if (str.find("INJ_TSHIFT")          ==1) rundb.inj_tshift_s          = GetVariables(str);
	if (str.find("RUN_STATUS")          ==1) rundb.run_status_s          = GetVariables(str);
	if (str.find("MEASUREMENT_TYPE")    ==1) rundb.measurement_type_s    = GetVariables(str);
	if (str.find("DEFINE_SPIN_PATTERN") ==1) rundb.define_spin_pattern_s = GetVariables(str);
	if (str.find("COMMENT")             ==1) rundb.comment_s             = GetVariables(str);
	if (str.find("DisableStrip")        ==1){
	  rundb.disable_strip_s     = GetVariables(str);
	  StripHandler(atoi(rundb.disable_strip_s.c_str()), 1);}
	if (str.find("EnableStrip")         ==1) {
	  rundb.enable_strip_s      = GetVariables(str);
	  StripHandler(atoi(rundb.enable_strip_s.c_str()),-1);}
      }
    }

  } // end-of-while(getline-loop)

  // Find Disable Strip List
  runinfo.NDisableStrip = FindDisableStrip();


  // processing conditions
  if (!extinput.CONFIG){
    strcat(reConfFile,confdir);
    strcat(reConfFile,    "/");
    strcat(reConfFile,rundb.config_file_s.c_str());
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

  // Optimize setting for Run
  if ((RUNID>=6500)&&(RUNID<7400)) { // Run05
      runinfo.Run=5;
    for (int i=0; i<NSTRIP; i++) phi[i] = phiRun5[i];
  } else if (RUNID>=7400) { // Run06
      runinfo.Run=6;
    for (int i=0; i<NSTRIP; i++) phi[i] = phiRun6[i];
  }

  // Spin Pattern Recoverly
  Flag.spin_pattern = atoi(rundb.define_spin_pattern_s.c_str());


  // VERBOSE mode
  if (Flag.VERBOSE) PrintRunDB();

  return 1;

}


//
// Class name  :
// Method name : StripHandler(int st, int flag)
//
// Description : handle enable/disable strips
// Input       : 
// Return      : 
//
int 
StripHandler(int st, int flag){

  static int Initiarize = 1;
  if (Initiarize) for (int i=0; i<NSTRIP; i++) ProcessStrip[i]=0;

  ProcessStrip[st-1] += flag;

  Initiarize=0;

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
int 
FindDisableStrip(){

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
// Method name : MatchBeam
//
// Description : check yellow or blue beam 
// Input       : double ThisRunID, double RunID
// Return      : 1 if ThisRunID and RunID are both blue or yellow beam
//
int 
MatchBeam(double ThisRunID, double RunID){

  int match=0;

  int ThisRun = int((ThisRunID-int(ThisRunID))*1e3);
  int Run     = int((RunID-int(RunID))*1e3);

  if ((ThisRun>=100)&&(Run>=100)) match=1;
  if ((ThisRun<= 99)&&(Run<=99))  match=1;

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
void
PrintRunDB(){

  printf("Run Status       = %s\n",    rundb.run_status_s.c_str());
  printf("Config File      = %s\n",    rundb.config_file_s.c_str());
  printf("Calib File       = %s\n",    rundb.calib_file_s.c_str());
  printf("Mass Cut         = %5.1f\n", strtof(rundb.masscut_s.c_str(),NULL));
  printf("TSHIFT           = %5.1f\n", strtof(rundb.tshift_s.c_str(),NULL));
  printf("INJ_TSHIFT       = %5.1f\n", strtof(rundb.inj_tshift_s.c_str(),NULL));
  printf("MEASUREMENT_TYPE = %5.1f\n", strtof(rundb.measurement_type_s.c_str(),NULL));
  printf("COMMENT          = %s\n",    rundb.comment_s.c_str());

  return;
}



// =====================================
// Print Out Configuration information 
// =====================================
int 
printConfig(recordConfigRhicStruct *cfginfo){


    int ccutwu;
    int ccutwl;

    fprintf(stdout,"================================================\n");
    fprintf(stdout,"===  RHIC Polarimeter Configuration (BGN)    ===\n");
    fprintf(stdout,"================================================\n");

    // Configulation File
    fprintf(stdout," RUN STATUS   = %s\n",    rundb.run_status_s.c_str());
    fprintf(stdout," MEAS. TYPE   = %s\n",    rundb.measurement_type_s.c_str());
    fprintf(stdout," CONFIG       = %s\n",reConfFile);
    fprintf(stdout," CALIB        = %s\n",CalibFile);

    // banana cut configulation
    if (dproc.CBANANA == 0) {
        ccutwl = (int)cfginfo->data.chan[3].ETCutW;
        ccutwu = (int)cfginfo->data.chan[3].ETCutW;
    } else if (dproc.CBANANA == 2) {
      fprintf(stdout," MASSCUT      = %5.1f\n",dproc.MassSigma);
    } else {
        ccutwl = (int)dproc.widthl;
        ccutwu = (int)dproc.widthu;
    }
    if (dproc.CBANANA!=2) 
      fprintf (stdout,"Carbon cut width : (low) %d (up) %d nsec \n",ccutwl,ccutwu);

    // tshift in [ns]
    fprintf(stdout," TSHIFT       = %5.1f\n",dproc.tshift);

    // Disabled strips
    fprintf(stdout,"#DisableStrip =   %d\n", runinfo.NDisableStrip);
    if (runinfo.NDisableStrip){
      fprintf(stdout," DisableStrip = ");
      for (int i=0;i<runinfo.NDisableStrip;i++) printf("%d ",runinfo.DisableStrip[i]+1);
    }
    printf("\n");


    // print comment
    if (strlen(rundb.comment_s.c_str())>3)
      printf(" COMMENT      = %s\n",    rundb.comment_s.c_str());

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
string
GetVariables(string str){

  string::size_type begin = str.find("=")+ 1;
  string::size_type end = str.find(";");
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
