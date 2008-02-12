//  Asymmetry Analysis of RHIC pC Polarimeter
//  file name :   AsymMain.cc
// 
// 
//  Author    :   Itaru Nakagawa
//  Creation  :   11/17/2005
//                

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>
#include <getopt.h>
#include "rhicpol.h"
#include "rpoldata.h"
#include "Asym.h"
#include "AsymMain.h"
#include "AsymProcess.h"
#include "AsymROOT.h"


// ================= 
// beginning of main 
// ================= 
int main (int argc, char *argv[]){

    // for get option
    int c;
    extern char *optarg;
    extern int optind;
    extern StructRunDB rundb;

    // Initialize Variables
    Initialization();


    // prefix directories
    char * datadir = getenv("DATADIR");
    if ( datadir == NULL ){
      cerr << "environment DATADIR is not defined" << endl;
      cerr << "e.g. export DATADIR=$HOME/2005/data" << endl;
      exit(-1);
    }

    // config directories
    confdir = getenv("CONFDIR");
    if ( confdir == NULL ){
      cerr << "environment CONFDIR is not defined" << endl;
      cerr << "e.g. export CONFDIR=/usr/local/cnipol/config" << endl;
      exit(-1);
    }

    // files
    char ifile[32], cfile[32], hbk_outfile[256];
    char ramptiming[256];
    int hbk_read = 0;  // hbk file read from argument:1 default:0
    int ramp_read = 0;  // ramp timing file read from argument:1 default:0
    
    
    // misc
    int i;
    char threshold[20],bunchid[20],enerange[20],cwidth[20],*ptr;
    int lth, uth;
    char suffix[] = ".data"; // suffix of data file

    int opt, option_index = 0;
    static struct option long_options[] = {
      {"raw", 0, 0, 'r'},
      {"feedback", 0, 0, 'b'},
      {"no-error-detector", 0, 0, 'a'},
      {0, 0, 0, 0}
    };

    while ((c = getopt_long(argc, argv, "?f:n:ho:rt:m:e:d:baCDTABZF:MNW:UGRS", long_options, &option_index))!=-1) {
        switch (c) {
        case 'h':
        case '?':
            cout << "Usage of " << argv[0] <<endl;
            cout << " -h(or?)                : print this help" <<endl;
            cout << " -f <filename>          : input data file name " <<endl;
            cout << " -n <number>            : evnt skip (n=1 noskip)" <<endl;
            cout << " -o <filename>          : Output hbk file" <<endl;
	    //            cout << " -r <filename>        : ramp timing file" <<endl;
            cout << " -t <time shift>        : TOF timing shift in [ns]" <<endl;
            cout << "                        : addition to TSHIFT defined in run.db " 
		 <<endl;
            cout << " -e <lower:upper>       : kinetic energy range" <<endl;
            cout << "                        : default (400:900) keV" <<endl;
	    cout << " <MODE> ---------------(default on)---------" <<endl;
	    //            cout << " -B                   : create banana curve on" <<endl;
	    //            cout << " -G                   : mass mode on " <<endl;
	    cout << " -a --no-error-detector : anomaly check off " << endl;
            cout << " <MODE> ---------------(default off)--------" <<endl;
	    cout << " -r --raw               : raw histograms on " << endl;
	    cout << " -b                     : feedback mode on " << endl;
            cout << " -C                     : Calibration mode on " <<endl;
            cout << " -D                     : Dead layer  mode on " <<endl;
	    cout << " -d  <dlayer>           : Additional deadlayer thickness [ug/cm2]" << endl;
	    //            cout << " -T                   : T0 study    mode on " <<endl;
	    //            cout << " -A                   : A0,A1 study mode on " <<endl;
	    //            cout << " -Z                   : without T0 subtraction" <<endl;
            cout << " -F <file>              : overwrite conf file defined in run.db" <<endl;
            cout << " -W <lower:upper>       : const width banana cut" <<endl;
	    cout << " -m <sigma>             : banana cut by <sigma> from 12C mass [def]:3 sigma" 
		 << endl; 
            cout << " -U                     : update histogram" <<endl;
            cout << " -N                     : store Ntuple events" <<endl;
            exit(0);
        case 'f':
            sprintf(ifile, optarg);
	    // if ifile lack of suffix ".data", attach ".data"
	    if (strstr(ifile,suffix)==NULL) strcat(ifile,suffix); 
	    strcat(datafile, datadir);
	    strcat(datafile,     "/");
	    strcat(datafile,   ifile);
            fprintf(stdout,"Input data file : %s\n",datafile);
            break;
        case 'n':
            Nskip = atol(optarg);
            fprintf(stdout,"Events skiped by : %d\n",Nskip);
            break;
        case 'o': // determine output hbk file
            sprintf(hbk_outfile, optarg);
            fprintf(stdout,"Output hbk file: %s \n",hbk_outfile); 
            hbk_read = 1;
            break;
        case 't': // set timing shift in banana cut
            dproc.tshift = atoi(optarg);
	    extinput.TSHIFT = 1;
            break;
        case 'd': // set timing shift in banana cut
            dproc.dx_offset = atoi(optarg);
            break;
        case 'e': // set energy range
	    strcpy(enerange, optarg);
            if (ptr = strrchr(enerange,':')){
                ptr++;
                dproc.eneu = atoi(ptr);
                strtok(enerange,":");
                dproc.enel = atoi(enerange);
                if ((dproc.enel==NULL)||(dproc.enel<0)) { dproc.enel=0;}
                if ((dproc.eneu==NULL)||(dproc.eneu>12000)) { dproc.eneu=2000;}
                fprintf(stdout,"ENERGY RANGE LOWER:UPPER = %d:%d\n",
                        dproc.enel,dproc.eneu);
            } else {
                cout << "Wrong specification for energy threshold" <<endl;
                exit(0);
            }
            break;
	case 'a':
	  Flag.EXE_ANOMALY_CHECK=0;
	  break;
        case 'F':
	  sprintf(cfile, optarg);
          if (!strstr(cfile,"/")) {
              strcat(reConfFile,confdir);
              strcat(reConfFile,    "/");
          }
          strcat(reConfFile,  cfile);
	  fprintf(stdout,"overwrite conf file : %s \n",reConfFile); 
	  extinput.CONFIG = 1;
	  break;
	case 'b':
            dproc.FEEDBACKMODE = Flag.feedback = 1;
            break;
	case 'r':
            dproc.RAWHISTOGRAM = 1;
            break;
        case 'C':
            dproc.CMODE = 1;
	    dproc.RECONFMODE = 0;
            break;
        case 'D':
            dproc.DMODE = 1;
            break;
        case 'T':
            dproc.TMODE = 1;
            fprintf(stdout,"*****TMODE*****\n");
            break;
        case 'A':
            dproc.AMODE = 1;
            break;
        case 'B':
            dproc.BMODE = 1;
            break;
        case 'Z':
            dproc.ZMODE = 1;
            break;
        case 'U':
            dproc.UPDATE = 1;
            break;
        case 'G':
            dproc.MMODE = 1;
            break;
        case 'N':
            dproc.NTMODE = 1;
            break;
        case 'W': // constant width banana cut
            dproc.CBANANA = 1;
            strcpy(cwidth, optarg);
            if (ptr = strrchr(cwidth,':')){
                ptr++;
                dproc.widthu = atoi(ptr);
                strtok(cwidth,":");
                dproc.widthl = atoi(cwidth);
                fprintf(stdout,"CONSTANT BANANA CUT LOWER:UPPER = %d:%d\n",
                        dproc.widthl,dproc.widthu);
		if (dproc.widthu == dproc.widthl) 
		  fprintf(stdout,"WARNING: Banana Lower = Upper Cut\a\n"); 
            } else {
                fprintf(stdout,"Wrong specification constant banana cut\n");
                exit(0);
            }
            fprintf(stdout,"BANANA Cut : %d <==> %d \n",
                    dproc.widthl,dproc.widthu);
            break;
	case 'm':
	    dproc.CBANANA = 2;
	    dproc.MassSigma = atof(optarg);
	    extinput.MASSCUT = 1;
	    break;
        default:
            fprintf(stdout,"Invalid Option \n");
            exit(0);
        }
    }

    // RunID 
    int chrlen = strlen(ifile)-strlen(suffix) ; // f.e. 7999.001.data - .data = 7999.001 
    char RunID[chrlen];
    strncpy(RunID,ifile,chrlen); RunID[chrlen]='\0'; // Without RunID[chrlen]='\0', RunID screwed up.
    runinfo.RUNID = strtod(RunID,NULL); // return 0 when "RunID" contains alphabetical char.


    // For normal runs, RUNID != 0. Then read run conditions from run.db.
    // Otherwise, data filename with characters skip readdb and reconfig routines
    // assuming these are energy calibration or test runs.
    if (runinfo.RUNID) { 
        readdb(runinfo.RUNID);
    } else {     
        dproc.RECONFMODE = 0;
    }

    // if output hbk file is not specified
    if (hbk_read == 0 ) {
        sprintf(hbk_outfile, "outsampleex.hbook");
        fprintf(stdout,"Hbook DEFAULT file: %s \n",hbk_outfile); 
    }        


    // ---------------------------------------------------- // 
    //            Hbook Histogram Booking                   //
    // ---------------------------------------------------- // 
    fprintf(stdout,"Booking ... histgram file\n");
    if (hist_book(hbk_outfile) != 0) {
        perror("Error: hist_book");
        exit(-1);
    }

    // ---------------------------------------------------- // 
    //                 Root Histogram Booking               //
    // ---------------------------------------------------- // 
    char filename[50];
    Root rt;
    sprintf(filename,"%.3f.root",runinfo.RUNID);
    fprintf(stdout,"Booking ROOT histgrams ...\n");
    if (rt.RootFile(filename) != 0) {
        perror("Error: RootFile()");
        exit(-1);
    }
    
    rt.RootHistBook(runinfo);

    // ---------------------------------------------------- // 
    // Quick Scan and Fit for tshift and mass sigma fit     //
    // ---------------------------------------------------- // 
    if (dproc.FEEDBACKMODE){
      printf("Feedback Sparcification Factor = 1/%d \n",dproc.thinout);
      if (readloop() != 0) {
        perror("Error: readloop");
        exit(-1);
      }
      Flag.feedback=0;
    }


    // ---------------------------------------------------- // 
    //                       Main Loop                      //
    // ---------------------------------------------------- // 
    if (readloop() != 0) {
        perror("Error: readloop");
        exit(-1);
    }
    

    // ---------------------------------------------------- // 
    //        Delete Unnecessary ROOT Histograms            //
    // ---------------------------------------------------- // 
    if (rt.DeleteHistogram() !=0) {
        perror("Error: DeleteHistogram()");
        exit(-1);
    }


    // ---------------------------------------------------- // 
    //                  Closing Histogram File              //
    // ---------------------------------------------------- // 
    if (hist_close(hbk_outfile) !=0) {
        perror("Error: hist_close");
        exit(-1);
    }

    // ---------------------------------------------------- // 
    //                     Closing ROOT File                //
    // ---------------------------------------------------- // 
    if (rt.CloseROOTFile() !=0) {
        perror("Error: CloseROOTFile()");
        exit(-1);
    }
    


} // end of main
 


// ===================================
// for Bunch by Bunch base analysis 
// ===================================
int BunchSelect(int bid){

  int go = 0;
  //  int BunchList[11]={4,13,24,33,44,53,64,73,84,93,104};
  int BunchList[26]={3,6,13,16,23,26,33,36,43,46,53,56,63,66,
		     73,76,83,86,93,96,103,106};


  for (int i=0; i<14; i++) {
    //    BunchList[i]++;
    if (bid == BunchList[i]) {
      go=1;
      break;
    }
  }

  return go;

}
 




// =========================
// Read the parameter file
// =========================

// Ramp timing file 
int read_ramptiming(char *filename){
    int i, strip;

    fprintf(stdout,"\nReading ... cut parameter file : %s \n", filename);

    ifstream rtiming;
    rtiming.open(filename);

    if (!rtiming) {
        cerr << "failed to open ramp timing file" <<endl;
        exit(1);
    }
    memset(ramptshift, 0, sizeof(ramptshift));
    
    float runt;
    
    int index = 0;
    while (!rtiming.eof()) {
        rtiming >> runt >> ramptshift[index] ;
        index ++;
    }
    
    rtiming.close();
    return(0);
}

// Calibration parameter
void reConfig(recordConfigRhicStruct *cfginfo){

    int st,strip;
    float t0,acoef,edead,ecoef,A0,A1,iasigma;

    fprintf(stdout,"**********************************\n");
    fprintf(stdout,"** Configuration is overwritten **\n");
    fprintf(stdout,"**********************************\n");

    ifstream configFile;

    configFile.open(reConfFile);

    if (!configFile) {
        cerr << "failed to open Config File : " << reConfFile << endl;
        exit(1);
    }

    cout << "Reading configuration info from : " << reConfFile <<endl;

    
    char temp[13][20];
    char *tempchar, *stripchar, *T0char;
    
    char buffer[300];
    int stripn;
    float t0n, ecn, edeadn, a0n, a1n, ealphn, dwidthn, peden;
    float c0n, c1n, c2n, c3n, c4n;


    int linen=0;
    while (!configFile.eof()) {
        
        configFile.getline(buffer, sizeof(buffer), '\n'); 
        if (strstr(buffer,"Channel")!=0) { 

            tempchar = strtok(buffer,"l");
            stripn = atoi(strtok(NULL, "="));
            t0n = atof(strtok(NULL," "));
            ecn = atof(strtok(NULL," "));
            edeadn = atof(strtok(NULL," "));
            a0n = atof(strtok(NULL," "));
            a1n = atof(strtok(NULL," "));
            ealphn = atof(strtok(NULL," "));
            dwidthn = atof(strtok(NULL," ")) + dproc.dx_offset; // extra thickness 
            peden = atof(strtok(NULL," "));
            c0n = atof(strtok(NULL," "));
            c1n = atof(strtok(NULL," "));
            c2n = atof(strtok(NULL," "));
            c3n = atof(strtok(NULL," "));
            c4n = atof(strtok(NULL," "));

            cfginfo->data.chan[stripn-1].edead = edeadn;
            cfginfo->data.chan[stripn-1].ecoef = ecn;
            cfginfo->data.chan[stripn-1].t0 = t0n;
            cfginfo->data.chan[stripn-1].A0 = a0n;
            cfginfo->data.chan[stripn-1].A1 = a1n;
            cfginfo->data.chan[stripn-1].acoef = ealphn;
            cfginfo->data.chan[stripn-1].dwidth = dwidthn;
            cfginfo->data.chan[stripn-1].pede = peden;
            cfginfo->data.chan[stripn-1].C[0] = c0n;
            cfginfo->data.chan[stripn-1].C[1] = c1n;
            cfginfo->data.chan[stripn-1].C[2] = c2n;
            cfginfo->data.chan[stripn-1].C[3] = c3n;
            cfginfo->data.chan[stripn-1].C[4] = c4n;

            cout << " Strip " << stripn;
            cout << " Ecoef " << ecn;
            cout << " T0 " << t0n;
            cout << " A0 " << a0n;
            cout << " A1 " << a1n;
            cout << " Acoef " << ealphn;
            cout << " Dwidth " << dwidthn;
            cout << " Pedestal " << peden << endl;
        }
        
        linen ++;
    }


    configFile.close();
}

//
// Class name  :
// Method name : ConfigureActiveStrip(int mask.detector)
//
// Description : Disable detector and configure active strips
//
// Input       : int mask.detector
// Return      : runinfo.ActiveDetector[i] remains masked strip configulation
//
int ConfigureActiveStrip(int mask){

  // Disable Detector First
  for (int i=0; i<NDETECTOR; i++) {
    if ((~mask>>i)&1) {
      runinfo.ActiveDetector[i] = 0x000;
      for (int j=0;j<NSTRIP_PER_DETECTOR; j++) {
	runinfo.NActiveStrip--;
	runinfo.ActiveStrip[i*NSTRIP_PER_DETECTOR+j] = 0;
      }
    }
  }

  // Configure Active Strips
  int det, strip=0;
  for (int i=0; i<runinfo.NDisableStrip; i++) {
    det   = runinfo.DisableStrip[i]/NSTRIP_PER_DETECTOR;

    // skip if the detector is already disabled
    if ((mask>>det)&1) { 
      strip = runinfo.DisableStrip[i] - det*NSTRIP_PER_DETECTOR;
      runinfo.ActiveDetector[det] ^= int(pow(2,double(strip))); // mask strips of detector=det
      runinfo.ActiveStrip[strip+det*NSTRIP_PER_DETECTOR] = 0;
      runinfo.NActiveStrip--;
    }

  } // end-of-for(runinof.NDisableStrip) loop


  return 0;
}



//
// Class name  :
// Method name : DisabledDet
//
// Description : Check Disabled detector
// Input       : int strip nuumber
// Return      : 1 if disabled. otherwise 0
//
/*
int
DisabledDet(int det){

  // det(0,1,2,3,4,5} => {0, 1, 0, 0, 1, 0} => 18
  int DeadDet = tgt.VHtarget ? 18 : 0 ;
  //                            ^   ^
  //                       H-target V-target 

  return DeadDet>>det & 1 ;

}
*/



// =================== 
// square root formula 
// =================== 
// A-RightUp  B-LeftDown  C-RightDown  D-LeftUp
// elastic Carbons are scattered off more in Right for Up 
int sqass(float A, float B, float C, float D, float *asym, float *easym) {

    float den;
    den = sqrt(A*B) + sqrt(C*D);
    if ((A*B==0.)&&(C*D==0.)) {
        *asym  = 0. ; *easym = 0.;
    } else {
        *asym  = (sqrt(A*B) - sqrt(C*D))/den;
        *easym = sqrt(A*B*(C+D) + C*D*(A+B))/den/den;
    }
    return(0);
}






//
// Class name  :
// Method name : Initialization
//
// Description : Initialize variables
//             : 
// Input       : 
// Return      : 
//
int
Initialization(){


  for (int i=0; i<NSTRIP; i++) {
    feedback.mdev[i] = 0.;
    feedback.RMS[i] = dproc.OneSigma ;
  }
  runinfo.TgtOperation = "fixed";

  // Initiarize Strip counters
  for (int i=0; i<NSTRIP; i++) {
    for (int j=0; j<3; j++) cntr.reg.NStrip[j][i] = cntr.alt.NStrip[j][i] = cntr.phx.NStrip[j][i] = cntr.str.NStrip[j][i] = 0;
    for (int j=0; j<3; j++) {
      for(int kk=0;kk<MAXDELIM;kk++) cntr_tgt.reg.NStrip[kk][j][i] = 0;
    }
  }

  return 1;

}






