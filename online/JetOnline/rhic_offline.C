// Event mode data analysis tool
//  file name :   rhic_offline.C
// 
// 
//  Author    :   Osamu Jinnouchi
//  Creation  :   12/9/2003
//  Comment   :   optimized for the detailed analysis for the event mode data
//                
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>

#include <iostream.h>
#include "TROOT.h"
#include "TFile.h"
#include "TRandom.h"
#include "TTree.h"
#include "TBranch.h"
#include "TClonesArray.h"
#include "TStopwatch.h"
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>

#include "rhicpol.h"
#include "rpoldata.h"
#include "rhic_offline.h"
#include "rhic_class.h"

void HHBOOKN(int hid, char* hname) {
    hhbookn_(&hid, hname);
    return;
};
void HHFN(int hid, float event[]) {
    hhfn_(&hid,event);
    return;
};

void HHBOOK1(int hid, char* hname, int xnbin, float xmin, float xmax) {
    hhbook1d_(&hid, hname, &xnbin, &xmin, &xmax, strlen(hname));
    return;
};
void HHBOOK2(int hid, char* hname, int xnbin, float xmin, float xmax,
             int ynbin, float ymin, float ymax) {
    hhbook2d_(&hid, hname, &xnbin, &xmin, &xmax, &ynbin, &ymin, &ymax, 
              strlen(hname));
    return;
};

void HHF1(int hid, float xval, float weight) {
    hhf1_(&hid, &xval, &weight);
    return;
};
void HHF2(int hid, float xval, float yval, float weight) {
    hhf2_(&hid, &xval, &yval, &weight);
    return;
};

void HHPAK(int hid, float *xval) {
    hhpak_(&hid, xval);
    return;
};

void HHPAKE(int hid, float *errors) {
    hhpake_(&hid, errors);
    return;
};


// Global variables 

//long int Nevcut=0;   // number of events after 1st cut (whole data)
long int Nevtot=0;   // number of total events (whole data) 
//long int Nread=0;    // real total events (completely everything)
long int NEVENTS=100000000;     // maxmum number of events to proceed

//long int Ngood[120];   // number of events after carbon cut (each bunch)
//long int Ntotal[120];   // number of events before carbon cut (each bunch)
//long int Nback[120];   // number of events below the curbon cut (each bunch)


// Initialization
TAsym::TAsym()
{
    Nevcut = 0;
    Nevtot = 0;
    Nread = 0;
    Nskip = 1;
    tshift = 0;
    Debug = 0;
    nring = 0;

    dproc.enel = 600;   
    dproc.eneu = 900;
    dproc.widthl = -30;
    dproc.widthu =  30;
    dproc.CMODE = 0; 
    dproc.DMODE = 0; 
    dproc.TMODE = 0; 
    dproc.AMODE = 0; 
    dproc.BUNCHMODE = 0; 
    dproc.BMODE = 0; 
    dproc.ZMODE = 0; 
    dproc.CFGMODE =0; 
    dproc.MESSAGE =0; 
    dproc.CBANANA =0; 
    dproc.UPDATE =0; 
    dproc.MMODE  =0;   
    dproc.NTMODE =0;  
    dproc.READT0MODE =0;
    dproc.RAMPMODE =0;  
    dproc.LMODE =0;   
    dproc.WAVE_OUT_MODE=0;
    dproc.JET_EVENT_MODE=0;
    dproc.RAW_EVENT_MODE=0;
    dproc.WAVE_FIT_MODE=0;
    dproc.SELECT_CHAN=0;
    dproc.BIN_SELECT=2;
    dproc.ToF_W=1;
    
    strcpy(gasymmetry_file,"gasymmetry.dat");
    strcpy(asymmetry_file,"asymmetry.dat");

    memset(Ngood, 0, sizeof(Ngood));
    memset(Nback, 0, sizeof(Nback));
    memset(Ntotal, 0, sizeof(Ntotal));
    memset(Ncounts, 0, sizeof(Ncounts));
    memset(NTcounts, 0, sizeof(NTcounts));
    memset(NRcounts, 0, sizeof(NRcounts));
    memset(Tcounts, 0, sizeof(Tcounts));
    memset(NSTcounts, 0, sizeof(NSTcounts));
};
struct {
    int idiv;		// divider
    float nsperchan;	// ns, WFD time unit, twice LSB of time measurement
    float emin;		// keV
    int ifine;
    float ecoef[96];	// keV/channel
    float edead[96];	// kev
    float tmin[96]; 	// ns
    int   mark[96];
} sipar_;

struct {
    int intp;
    int iraw;
} runpars_;


//	Common /atdata/ (for n-tuple)
struct {
    float a;	// keV
    float t;	// ns
    float tmax; // ns
    float s;	// keV*ns
    long b;	// bunch number: 0-59
    long d; 	// delimiter value
    long rev;	// revolution number
    long jet;	// jet polarization (0-3)
} atdata_;

//	Common /atraw/ (for n-tuple)
struct {
    long ia;
    long it;
    long itmax;
    long is;
    long ib;	// bunch number: 0-59
    long id; 	// delimiter value
    long irev;	// revolution number
    long ijet;	// jet polarization (0-3)
} atraw_;

struct {
    int iquest[100];
} quest_;
int aaa[10];

//	Common /rhic/
struct {
    int fillpat[120];
    int polpat[120];
} rhic_;
//	Commons /poldat/ /beamdat/ /tgtdat1/ /tgtdat2/
polDataStruct poldat_;
beamDataStruct beamdat_;
targetDataStruct tgtdat1_;
targetDataStruct tgtdat2_;
long long scalers_[6]; // LecRoy 48-bit scalers

int sscal_[288];


// ================= 
// beginning of main 
// ================= 
int main (int argc, char *argv[]){
    
    TAsym CNIasym;
    
    // files
    char root_outfile[256] = "";
    char root_outfile2[256] = "";
    char root_outfile3[256] = "";
    char root_outfile4[256] = "";
    char num1[5] ="";		// 4 characters + \0
    char num2[4] ="";		// 3 characters + \0
    char oname_count[256] = "";
    char hbk_outfile[256] = "";
    char kin_infile[256] = "";
    char mass_infile[256] = "";
    char mass_relmov_infile[256] = "";

    int c;
    extern char *optarg;
    extern int optind;
    int mode=96;
    int chan_no=96;
    int moji;
    while ((c = getopt(argc, argv, "?f:n:g:ho:t:e:c:k:m:r:d:CDTABZF:MNW:OUGRLKXwabs:u:p:"))
           !=-1) {
        switch (c) {
        case 'h':
        case '?':
            fprintf(stdout,"Usage of %s\n",argv[0]);
            fprintf(stdout," -h(or?)              : print this help\n");
            fprintf(stdout," -r <number>          : ring 0;blue 1:yellow \n");
            fprintf(stdout," -f <filename>        : input data file name \n");
            fprintf(stdout," -n <number>          : events to proceed\n");
            fprintf(stdout," -o <filename>        : Output root file\n");
            fprintf(stdout,"                      : default sample.root\n");
            fprintf(stdout," -t <time shift>      : banana cut timing \n");
            fprintf(stdout,"                      : default (0:23) \n");
            fprintf(stdout," -e <lower:upper>     : kinetic energy range\n");
            fprintf(stdout,"                      : default (350:900) keV\n");
            fprintf(stdout," -k <file name>       : kinematics variables\n"); 
            fprintf(stdout," -m <file name>       : mass width variables\n"); 
            fprintf(stdout," -d <file name>       : mass bunch relation\n");
            fprintf(stdout,"                                       \n");
            fprintf(stdout," <MODE> ---------------(default all off)----\n");
            fprintf(stdout," -C                   : Calibration mode on \n");
            fprintf(stdout," -D                   : Dead layer  mode on \n");
            fprintf(stdout," -T                   : T0 study    mode on \n");
            fprintf(stdout," -A                   : A0,A1 study mode on \n");
            fprintf(stdout," -K                   : create banana curve on\n");
            fprintf(stdout," -Z                   : without T0 subtraction\n");
            fprintf(stdout," -F <file>            : read T0 info file\n");
            fprintf(stdout," -c <file>            : read cfg info file\n");
            fprintf(stdout,"                      : fixed file cfginfo.dat\n");
            fprintf(stdout," -M                   : exit after run message\n");
            fprintf(stdout," -W <lower:upper>     : const width banana cut\n");
            fprintf(stdout," -O                   : use mass cut(not w/ W)\n");
            fprintf(stdout," -U                   : update histogram\n");
            fprintf(stdout," -G                   : mass mode on \n");
            fprintf(stdout," -N                   : store Ntuple events\n");
            fprintf(stdout," -R                   : ramp measure 1sec bin\n");
            fprintf(stdout," -L                   : Look Up Table for cut\n");
            fprintf(stdout," -X                   : WAVE_OUT_MODE WAVEFORM_FIT & WAVE.root \n");
            exit(0);
        case 'r':
            char ringname[2][10];
            sprintf (ringname[0],"blue");
            sprintf (ringname[1],"yellow");
            CNIasym.nring = atoi(optarg);
            fprintf(stdout,"The ring is : %s\n",ringname[CNIasym.nring]);
            break;
        case 'f':
            sprintf(CNIasym.datafile, optarg);
            fprintf(stdout,"Input data file : %s\n",CNIasym.datafile);
            break;
        case 'n':
            CNIasym.Nskip = atol(optarg);
            fprintf(stdout,"Event proceed per %d skip\n",CNIasym.Nskip);
            break;
        case 'o': // determine output root file
            sprintf(root_outfile, optarg);
            fprintf(stdout,"Output hbk file: %s \n",root_outfile); 
       
            for(moji=0;moji<256;++moji){
                 if(root_outfile[moji]=='.' && root_outfile[moji+1]=='r' && root_outfile[moji+2]=='o'){
                     int ii=0;
                     int iii=0;
                     //printf("moji=%d\n",moji);
                     for(int kk=0;kk<moji;++kk){
                        root_outfile2[kk]=root_outfile[kk];
                        root_outfile3[kk]=root_outfile[kk];
                        root_outfile4[kk]=root_outfile[kk];
                        if(kk>(moji-4)){
                         num2[ii]=root_outfile[kk];
                         ii++;
                        }
                        if(kk>(moji-9) && kk<(moji-4)){
                         num1[iii]=root_outfile[kk];
                         //printf("k=%d %c %c\n",kk,root_outfile[kk],num1[iii]);
                         iii++;
                        }
                     }
                     //printf("ii=%d iii=%d\n",ii,iii);
                     //printf("num1=%s num2=%s\n",num1,num2);
                 }
            }
        
        
            strcat(root_outfile2,"RAW.root");
            strcat(root_outfile3,"EVENT.root");
            strcat(root_outfile4,"WAVE.root");           
            sprintf(oname_count, optarg);
            strcat(oname_count,"_.txt2");

            printf("%s\n",root_outfile2);
            printf("%s\n",root_outfile3);
            printf("%s\n",root_outfile4);
            printf("%s :: %s\n",num1,num2);
            //getchar(); 
            break;
        case 'k': // determine kinematics variables to be read
            strcpy(kin_infile, optarg);
            fprintf(stdout,"Kinetic variables file: %s \n",kin_infile); 
            break;
        case 'm': // determine kinematics variables to be read
            strcpy(mass_infile, optarg);
            fprintf(stdout,"Variable Mass Width file: %s \n",mass_infile); 
            break;
        case 'd': // determine kinematics variables to be read
            strcpy(mass_relmov_infile, optarg);
            fprintf(stdout,"Relative mass shift in bunch file: %s \n",
                    mass_relmov_infile); 
            break;
        case 't': // set timing shift in banana cut
            CNIasym.tshift = atoi(optarg);
            fprintf(stdout,"Timing has been shifted by %d ns\n",
                    CNIasym.tshift);
            break;
        case 'e': // set energy range
            char enerange[20], *ptre;
            strcpy(enerange, optarg);
            if (ptre = strrchr(enerange,':')){
                ptre++;
                CNIasym.dproc.eneu = atoi(ptre);
                strtok(enerange,":");
                CNIasym.dproc.enel = atoi(enerange);
                if ((CNIasym.dproc.enel==NULL)||(CNIasym.dproc.enel<0)) 
                    { CNIasym.dproc.enel=0;}
                if ((CNIasym.dproc.eneu==NULL)||(CNIasym.dproc.eneu>2000)) 
                    { CNIasym.dproc.eneu=2000;}
                fprintf(stdout,"ENERGY RANGE LOWER:UPPER = %d:%d\n",
                        CNIasym.dproc.enel,CNIasym.dproc.eneu);
            } else {
                fprintf(stdout,"Wrong specification for energy threshold\n");
                exit(0);
            }
            break;
        case 'F':
            sprintf(CNIasym.t0_conf, optarg);
            fprintf(stdout,"read T0 conf: %s \n",CNIasym.t0_conf); 
            CNIasym.dproc.READT0MODE = 1;
            break;
        case 'c':
            sprintf(CNIasym.conf_file, optarg);
            fprintf(stdout,"read conf file: %s \n",CNIasym.conf_file); 
            CNIasym.dproc.CFGMODE = 1;
            break;
        case 'C':
            CNIasym.dproc.CMODE = 1;
            cout << " NTUPLE mode " <<endl;
            break;
        case 'D':
            CNIasym.dproc.DMODE = 1;
            break;
        case 'T':
            CNIasym.dproc.TMODE = 1;
            fprintf(stdout,"*****TMODE*****\n");
            break;
        case 'A':
            CNIasym.dproc.AMODE = 1;
            break;
        case 'B':
            CNIasym.dproc.BUNCHMODE = 1;
            break;
        case 'K':
            CNIasym.dproc.BMODE = 1;
            break;
        case 'Z':
            CNIasym.dproc.ZMODE = 1;
            break;
        case 'M':
            CNIasym.dproc.MESSAGE = 1;
            break;
        case 'U':
            CNIasym.dproc.UPDATE = 1;
            break;
        case 'G':
            CNIasym.dproc.MMODE = 1;
            break;
        case 'N':
            CNIasym.dproc.NTMODE = 1;
            break;
        case 'R':
            CNIasym.dproc.RAMPMODE=1;
            fprintf(stdout,"RAMP MEASUREMENT ON\n"); 
            break;
        case 'L':
            CNIasym.dproc.LMODE = 1;
            fprintf(stdout,"Use Look Up Table for calcuration\n"); 
            break;
        case 'W': // constant width banana cut
            CNIasym.dproc.CBANANA = 1;
            char cwidth[20], *ptrc;
            strcpy(cwidth, optarg);
            if (ptrc = strrchr(cwidth,':')){
                ptrc++;
                CNIasym.dproc.widthu = atoi(ptrc);
                strtok(cwidth,":");
                CNIasym.dproc.widthl = atoi(cwidth);
                fprintf(stdout,"CONSTANT BANANA CUT LOWER:UPPER = %d:%d\n",
                        CNIasym.dproc.widthl,CNIasym.dproc.widthu);
            } else {
                fprintf(stdout,"Wrong specification constant banana cut\n");
                exit(0);
            }
            fprintf(stdout,"BANANA Cut : %d <==> %d \n",
                    CNIasym.dproc.widthl,CNIasym.dproc.widthu);
            break;
        case 'O': // use mass info for cut 
            cout << "Using Mass Info for asymmetry calc"<<endl;
            CNIasym.dproc.CBANANA = 2;
            break;
        case 'X': // Do not do WAVEFORM FIT and OUTPUT wave.root 
            printf("NO OUTPUT WAVEFORM HISTGRAM ,NO FITTING!!\n");
        //    getchar();
            CNIasym.dproc.WAVE_FIT_MODE = 1;
            break;
        case 'a':
            CNIasym.dproc.JET_EVENT_MODE =1;
            break;
        case 'b':
            CNIasym.dproc.RAW_EVENT_MODE = 1;
            break;
        case 'w':
            CNIasym.dproc.WAVE_OUT_MODE =1;
            break;
        case 's':
            CNIasym.dproc.SELECT_CHAN = atoi(optarg);
            break;
        case 'u'://Energy BIN SELECT
            CNIasym.dproc.BIN_SELECT = atoi(optarg);
            break;
        case 'p'://Beam selection
            CNIasym.dproc.beam = atoi(optarg);
            //printf("beam=%d \n",CNIasym.dproc.beam);
            //getchar();
            //CNIasym.dproc.ToF_W = atoi(optarg);
            break;
        default:
            fprintf(stdout,"Invalid Option \n");
            exit(0);
        }
    }
    
    // if output root file is not specified
    if (strcmp(root_outfile,"")==0) {
        sprintf(root_outfile, "sample.root");
        sprintf(root_outfile2, "RAW_sample.root");
        sprintf(root_outfile3, "EVENT_sample.root");
        sprintf(root_outfile4, "WAVE_sample.root");
        sprintf(oname_count, "output.txt");
        cout << "Root Default file: "<< root_outfile<<endl;
    }        
/*
    // if output hbk file is not specified
    int hbk_read=0;
    if (hbk_read==0) {
        printf("hbook OPEN\n");
        sprintf(hbk_outfile, "outsample.hbk");
        cout << "Root Default file: "<< hbk_outfile<<endl;
    }        

    printf("hbook initialize\n");
    if(PawInit(hbk_outfile) !=0){
       printf("ERROR HBOOK\n");
       exit(0);
    }
    printf("hbook initialize END\n");
*/    
    // if kinetic varialbe file is not specified
    if (strcmp(kin_infile,"")==0) {
        cout << "Kinematic file Not Specified : "<<endl;
        cout << "Apply 0 to all variables "<<endl;
        
        for (Int_t i=0;i<chan_no;i++) {
            CNIasym.dthick[i] = 0.;  
            CNIasym.kint0[i] = 0.;
        }
    } else {       
        
        cout << "Loading ... Kinetic paraemeter file : "<<kin_infile<<endl;
        if (CNIasym.ReadKin(kin_infile) != 0) {
            perror("Error: kin read ");
            exit(1);
        }
    }
    
    // if Mass variable file is not specified 
    if (strcmp(mass_infile,"")==0) {
        cout << "Mass Width file Not Specified : "<<endl;
        cout << "Apply 11.17 to center 1.5 to all width "<<endl;
        
        for (Int_t st=0;st<chan_no;st++) {
            for (Int_t i=0; i<10; i++) {
            
                CNIasym.mcenter[st][i] = 11.17;  
                CNIasym.msigma[st][i] = 1.5;
            }
        }

    } else {       
        
        cout << "Loading ... Mass Width file: "<<mass_infile<<endl;
        if (CNIasym.ReadMass(mass_infile) != 0) {
            perror("Error: mass read ");
            exit(1);
        }
    }

    
    // if relative mass shift file is not specified 
        if (strcmp(mass_relmov_infile,"")==0) {
        cout << "Relative mass shift file Not Specified : "<<endl;
        cout << "Apply 0.0 GeV for each bunch "<<endl;
        
        for (Int_t st=0;st<chan_no;st++) {
            for (Int_t bid=0;bid<55;bid++) {
                CNIasym.mrelcent[st][bid] = 0.0;  
            }
        }
        
    } else {       
        
        cout << "Loading ... relative mass shift file: "
             << mass_relmov_infile << endl;
        
        if (CNIasym.ReadMassRelShift(mass_relmov_infile) != 0) {
            perror("Error: mass relmov read ");
            exit(1);
        }
        }
    
    cout <<  "Initializing ... ROOT file : "<<root_outfile<<endl;
    //printf("1-->%s\n",root_outfile);
    //printf("2-->%s\n",root_outfile2);
    //printf("3-->%s\n",root_outfile3);
    //printf("4-->%s\n",root_outfile4);
    //printf("------------------------\n");
    if (CNIasym.RootInit(root_outfile,mode,root_outfile2,root_outfile3,root_outfile4) != 0) {
        perror("Error: ROOT INIT");
        exit(1);
    }
    
    cout << "Loading the parameter file for IA calibration: "<<endl;
    if (CNIasym.ReadIAParam("iaparam.dat") != 0) {
        cout << "Use temporary numbers for IA"<<endl;
        for (int i=0;i<chan_no;i++) {
            CNIasym.iapar0[i] = 1.;
            CNIasym.iapar1[i] = 1.;
        }
    }

    cout << "Start Processing Data Stream"<<endl;

    if (CNIasym.ReadLoop(oname_count,num1,num2) != 0) {
        perror("Error: ReadLoop");
        exit(1);
    }
    
    cout << "Finished Processing Data Stream"<<endl;

    if (CNIasym.RootClose(root_outfile) !=0) {
        perror("Error: RootClose");
        exit(1);
    }
    // HIST FILE CLOSE

    if (hist_close(hbk_outfile) !=0) {
        perror("Error: hist_close");
        exit(1);
    }

    return(0);
} // end of main


// ================= 
// read loop routine 
// ================= 
int TAsym::ReadLoop(char *oname, char *num1, char *num2) {
    long int Nevent, i;
//////okada_START
    FILE *tmp_outfile; //okada
    FILE *out_count;//okada
    FILE *out_Tasym;//okada
    FILE *out_Basym;//okada
    FILE *outfile_test;//okada
//    FILE *Cinfile;
    int kkk=0;
    int wave[90]; //WAVEFORM_MODE
    int Ntuple_data[8];
    double FitResult[12];
    double FitMaskResult[9];
    int frag_tmp=0;
    int raw_n=0;
    int nk;
    int nnkk=-1;
    int hist_num=1;
    int amp,t_time,dummy1,square,bunch,dummy2;
    int last;
    int tmp_i;
    int read_typ=1;
    int strip_no;  /// hontou ni srip# ?
    int get_frag;
    double g_ats[6];
    int bunch0=0;
    int error=0;
    int all_event=0;
    int all_event_old=-1;
    int fit_event=0;
    int fit_err=0;
    int all_event_AT=0; //total_event reading ATMODE
    int select_eventY[6]={0,0,0,0,0,0};
    int select_eventB[6]={0,0,0,0,0,0};
    int amp_max,amp_min,tdc_max,tdc_min;
    int RAW_event=0;
    int swap=0;
    int swap_bunch0=0;
    int swap_bunch116=0;
    int notswap=0;
    int notswap_bunch0=0;
    int notswap_bunch116=0;
    int all_bunch0=0;
    int dmy_cont=0;
    int bunch116=0;
    int all_bunch116=0;
    int above200=0;
    int bun[120];
    int bun_odd,bun_even;
    int test_strip[80][6];
    int iii=0;
    int ADC_DIF[200];
    int ADC_DIF_ref[200];
    int typ1=0;//ABS(TDC_DIFF)>1
    int typ2=0;//ABS(ADC_DIFF)>1
    int typ3=0;//ABS(TDC_DIFF)>1 && ABS(ADC_DIFF)>1
    int igor_count=0;//even,no_abord
    int mode=90; //WAVEFORM_MODE
    int rev_num[96];
    int old_delim=0;
    int old_bunch=0;
    int old_rev=0;
    int total_event=-1;
    int chan_no=96;
    int count_at=0;
    double Am_E[2];//MeV
    int hjet_frag=0;//if REC_HJETSWITCH frag=1;
    int hjet_count=0;//if REC_HJETSWITCH jet_count++;
    int first_rev=0; 
    int last_rev=0; 
    int first_delimt=0;
    int last_delimt=0;
    long int pol_rev[3];

    int raw_i,raw_j,raw_k;

    int amp_mismatch=0;
    int tdc_mismatch=0;
    int tmax_mismatch=0;

    int wave_cont=0;//fast+large amp waveform check
    int ch_select=0;//ch_select for Fit or wave_cont

    int target_start_frag=0;
    

    int tmp_i1=0;
    int tmp_i2=0;

    int start,stop;
    double AreaG,AreaS;

    int YR[3][3][4];//beam,target,id  
    int YL[3][3][4];//beam,target,id
    int YLTp[4],YRTp[4],YLTn[4],YRTn[4];//target yield 
    int YLBp[4],YRBp[4],YLBn[4],YRBn[4];//beam yield 
    int intg_check=0;


    int evt_chan[96];
    for(int k=0;k<96;++k){
      evt_chan[k]=0;
    }

    for(int k=0;k<3;++k)pol_rev[k]=0;
    for(int k=0;k<4;++k){
     YRTp[k]=0;YLTp[k]=0;YRTn[k]=0;YLTn[k]=0;
     YRBp[k]=0;YLBp[k]=0;YRBn[k]=0;YLBn[k]=0;
     for(int l=0;l<3;++l){
      for(int m=0;m<3;++m){
        YR[l][m][k]=0;
        YL[l][m][k]=0;
      }
     }
    }

    double Mx,sin_thetaR;
    double Mp,ToF0;
    //double tdc_geta=42.0;//42 for SPIN2004;
    double tdc_geta;//set later
//Run2005    double geta_ch[97];
    FILE *t0_in;
    int idmy;
    //t0_in = fopen("NtupleH/t0_corr.txt","r"); //time0 correction
/*
//Run2005
    t0_in = fopen("/home/okadah/Waveform_test/Jet-offl_highT/NtupleH/t0_corr_new.csv","r"); //time0 correction
    for(int k=0;k<97;++k){
      geta_ch[k]=0;
      if(k>0)fscanf(t0_in,"%d,%lf",&idmy,&geta_ch[k]);
    }
    fclose(t0_in);
*/

    int geo_ch[97];//geo_ch[wfd.ch]
    geo_ch[0]=0;
//    geo_ch 76 <-> 78
    geo_ch[24]=73;geo_ch[23]=74;geo_ch[22]=75;geo_ch[21]=78;geo_ch[20]=77;geo_ch[19]=76;geo_ch[18]=79;geo_ch[17]=80;
    geo_ch[16]=81;geo_ch[15]=82;geo_ch[14]=83;geo_ch[13]=84;geo_ch[12]=85;geo_ch[11]=86;geo_ch[10]=87;geo_ch[9]=88;
    geo_ch[8]=89;geo_ch[7]=90;geo_ch[6]=91;geo_ch[5]=92;geo_ch[4]=93;geo_ch[3]=94;geo_ch[2]=95;geo_ch[1]=96;

    geo_ch[48]=49;geo_ch[47]=50;geo_ch[46]=51;geo_ch[45]=52;geo_ch[44]=53;geo_ch[43]=54;geo_ch[42]=55;geo_ch[41]=56;
    geo_ch[40]=57;geo_ch[39]=58;geo_ch[38]=59;geo_ch[37]=60;geo_ch[36]=61;geo_ch[35]=62;geo_ch[34]=63;geo_ch[33]=64;
    geo_ch[32]=65;geo_ch[31]=66;geo_ch[30]=67;geo_ch[29]=68;geo_ch[28]=69;geo_ch[27]=70;geo_ch[26]=71;geo_ch[25]=72;

    geo_ch[72]=25;geo_ch[71]=26;geo_ch[70]=27;geo_ch[69]=28;geo_ch[68]=29;geo_ch[67]=30;geo_ch[66]=31;geo_ch[65]=32;
    geo_ch[64]=33;geo_ch[63]=34;geo_ch[62]=35;geo_ch[61]=36;geo_ch[60]=37;geo_ch[59]=38;geo_ch[58]=39;geo_ch[57]=40;
    geo_ch[56]=41;geo_ch[55]=42;geo_ch[54]=43;geo_ch[53]=44;geo_ch[52]=45;geo_ch[51]=46;geo_ch[50]=47;geo_ch[49]=48;

    geo_ch[96]=1;geo_ch[95]=2;geo_ch[94]=3;geo_ch[93]=4;geo_ch[92]=5;geo_ch[91]=6;geo_ch[90]=7;geo_ch[89]=8;
    geo_ch[88]=9;geo_ch[87]=10;geo_ch[86]=11;geo_ch[85]=12;geo_ch[84]=13;geo_ch[83]=14;geo_ch[82]=15;geo_ch[81]=16;
    geo_ch[80]=17;geo_ch[79]=18;geo_ch[78]=19;geo_ch[77]=20;geo_ch[76]=21;geo_ch[75]=22;geo_ch[74]=23;geo_ch[73]=24;
/*
    geo_ch[0]= 0;
    for(int i=1; i<97;++i){
      geo_ch[i]= i;
    }
*/
    double ADC_cal[97];
//    double INTG_cal[97];
    FILE *tmp_calib;
//    FILE *inputPar;
//    FILE *INTG_calib;
    int calib_count=0;
    double ldmy;
    char sdmy[256];
    double ADC_mean=0;
    char snum[7]="";
    long int i_num;
    int calib_typ=0;
    char month[2]="";
    int i_month;
//    double MAP_E0[512],MAP_E[512];
//    double MAP_E_d1[512],MAP_E_d2[512];
//    int MAP_num;

//2005 
/*
 Cinfile=fopen("MAP_proton.txt","r");
 int k=0;
 fscanf(Cinfile,"%s",&sdmy);
 //printf("%s\n",sdmy);
 while( fscanf(Cinfile,"%lf,%lf,%lf,%lf",&MAP_E0[k],&MAP_E_d1[k],&MAP_E_d2[k],&ldmy)==4){
 //  printf("%lf %lf\n",MAP_E0[k],MAP_E_d1[k]);
   MAP_E[k]=(MAP_E_d1[k]+MAP_E_d2[k])/2;
   k++;
 }
 MAP_num=k;
 //printf("MAP_num=%d\n",MAP_num);
 fclose(Cinfile);
*/
 

    Am_E[0]=5.486;
    Am_E[1]=5.244;
    
    month[0]=num1[2];
    month[1]=num1[3];
    i_month=atoi(month);

    strcat(snum,num1);
    strcat(snum,num2);
    i_num=atoi(snum);


    printf("i_month=%d i_num=%ld\n",i_month,i_num);
/////////////////ADC//////////////////////////////////////////
//    inputPar=fopen("/home/okadah/Waveform_test/Jet-offl_test/Waveform_check/param_check/x0305.211_ntau.data","r"); 
//    if(i_month==4){  
//      if(i_num>=2604201 && i_num<2904222 ){
        tmp_calib=fopen("/data/2006/rootjet/jet_calib.data","r");
        calib_typ=1;
//      }
/*
      if(i_num>=2904222 && i_num<3104300){
        tmp_calib=fopen("/data/2004/root/x2904.222_calib.data","r");
        calib_typ=2;
      }
    }else{
      if(i_num>=105201 && i_num<705209){
        tmp_calib=fopen("/data/2004/root/x2904.222_calib.data","r");
        calib_typ=3;
      }
      if(i_num>=705209){
        tmp_calib=fopen("/data/2004/root/x0705.209_calib.data","r");
        calib_typ=4;
      }
//tdc_geta/////////
      if(i_num>=705208 && i_num<805208){//24GeV
        tdc_geta=55;
      }
 
    }
    calib_typ=0;
    if(calib_typ==0){
      printf("i_num=%d\n",i_num);
////////tmp
      calib_typ=1;
      //tmp_calib=fopen("/data/2005/root/x1004.301_calib.data","r");
      tmp_calib=fopen("/data/2005/root/x1204.301_calib.data","r");
////////tmpEND
      //exit(0);
    }
*/
    printf("calib_typ=%d\n",calib_typ);
    //fscanf(tmp_calib,"%s",&sdmy);   
    for(int tt=1;tt<97;++tt){
       //fscanf(tmp_calib,"%d	%lf	%lf	%lf	%lf	%lf	%lf",&idmy,&ADC_cal[tt],&ldmy,&ldmy,&ldmy,&ldmy,&ldmy);   
       fscanf(tmp_calib,"%d	%lf",&idmy,&ADC_cal[tt]);   
//       printf("%d %lf\n",idmy,ADC_cal[tt]);
//      if(tt%10==0)getchar();
    //   if(idmy!=29 && idmy!=46)calib_count++;
       ADC_mean=ADC_mean+ADC_cal[tt];
    }
    fclose(tmp_calib);
    ADC_mean=150;
///////////////////////INTG//////////////////////////////////
/*
    calib_typ=-1;
if(i_month==4){
      if(i_num>=2604201 && i_num<2704210 ){
        INTG_calib=fopen("/home/okadah/Waveform_test/Jet-offl_test/dead_layer/Calib1/itg_posi_calib1.txt","r");
        calib_typ=1;
      }
      if(i_num>=2704210 && i_num<2804213){
        INTG_calib=fopen("/home/okadah/Waveform_test/Jet-offl_test/dead_layer/Calib2/itg_posi_calib2.txt","r");
        calib_typ=2;
      }
      if(i_num>=2804213 && i_num<3004210){
        INTG_calib=fopen("/home/okadah/Waveform_test/Jet-offl_test/dead_layer/Calib3/itg_posi_calib3.txt","r");
        calib_typ=3;
      }
      if(i_num>=3004210 && i_num<=3004215){
        INTG_calib=fopen("/home/okadah/Waveform_test/Jet-offl_test/dead_layer/Calib4/itg_posi_calib4.txt","r");
        calib_typ=4;
      }
}else{
      if(i_num>=105201 && i_num<105216){
        INTG_calib=fopen("/home/okadah/Waveform_test/Jet-offl_test/dead_layer/Calib5/itg_posi_calib5.txt","r");
        calib_typ=5;
      }
      if(i_num>=105216 && i_num<305201){
        INTG_calib=fopen("/home/okadah/Waveform_test/Jet-offl_test/dead_layer/Calib6/itg_posi_calib6.txt","r");
        calib_typ=6;
      }
      if(i_num>=305201 && i_num<705209){
        INTG_calib=fopen("/home/okadah/Waveform_test/Jet-offl_test/dead_layer/Calib7/itg_posi_calib7.txt","r");
        calib_typ=7;
      }
      if(i_num>=705209 && i_num<1005203){
        INTG_calib=fopen("/home/okadah/Waveform_test/Jet-offl_test/dead_layer/Calib8/itg_posi_calib8.txt","r");
        calib_typ=8;
      }
      if(i_num>=1005203 && i_num<1105209){
        INTG_calib=fopen("/home/okadah/Waveform_test/Jet-offl_test/dead_layer/Calib9/itg_posi_calib9.txt","r");
        calib_typ=9;
      }
      if(i_num>=1105209 && i_num<1205208){
        INTG_calib=fopen("/home/okadah/Waveform_test/Jet-offl_test/dead_layer/Calib10/itg_posi_calib10.txt","r");
        calib_typ=10;
      }
      if(i_num>=1205208 && i_num<1305207){
        INTG_calib=fopen("/home/okadah/Waveform_test/Jet-offl_test/dead_layer/Calib11/itg_posi_calib11.txt","r");
        calib_typ=11;
      }
      if(i_num>=1305207 ){
        INTG_calib=fopen("/home/okadah/Waveform_test/Jet-offl_test/dead_layer/Calib12/itg_posi_calib12.txt","r");
        calib_typ=12;
      }
    }
    if(calib_typ==-1){
      printf("i_num=%d\n",i_num);
      //exit(0);
    }
//temp
        INTG_calib=fopen("/home/okadah/Waveform_test/Jet-offl_test/dead_layer/Calib12/itg_posi_calib12.txt","r");
        calib_typ=12;
//tmpEND
    printf("INTG calib_typ=%d\n",calib_typ);
    fscanf(INTG_calib,"%s",&sdmy);
    INTG_cal[0]=0;
 for(int tt=1;tt<97;++tt){
       fscanf(INTG_calib,"%d,%lf",&idmy,&INTG_cal[tt]);
       //printf("%d %lf\n",idmy,INTG_cal[tt]);
      //if(tt%10==0)getchar();
    }
    fclose(INTG_calib);
//////////////////////////////INTG END///////////////////////////////
*/
//check delim & rev & rev0 & bunch
    //tmp_outfile=fopen("check_rev.txt","w");

//2005    FitMASK(FitMaskResult);
//    printf("after FitMaskREsult!!\n");
   //getchar();
    for(int k=0;k<96;++k)rev_num[k]=0;
    //FILE *vec_test1;
    //vec_test1=fopen("vec_test1.txt","w");
////////okada_END
   printf("pppppp\n"); 
    // Common structure for the data format
    static union {
        recordHeaderStruct     header;
        recordBeginStruct      begin;
        recordPolAdoStruct     polado;
        recordTagAdoStruct     tag;
        //recordBeamAdoStruct    beamado;
        recordBeamAdoStruct    beam;
        recordConfigRhicStruct cfg;
        recordReadWaveStruct   all;
        recordReadWave120Struct all120;
        recordReadATStruct      at;
        recordWFDV8ArrayStruct  wfd;
        recordEndStruct        end;
        recordScalersStruct    scal;
	recordHJetSwitchStruct jet;
        recordWcmAdoStruct     wcmado;
        char               buffer[BSIZE*sizeof(int)]; // 4MB
        recordDataStruct       data;
    } rec;

//	data structure pointers
    recordReadATStruct * ATPtr;
    recordReadWaveStruct * wavePtr;
    recordReadWave120Struct * wave120Ptr;
    recordALLModeStruct * ALLPtr;
    recordALLMode120Struct * ALL120Ptr;
    
    recordConfigRhicStruct  *cfginfo;
    beamDataStruct beamdat;
    wcmDataStruct wcmdat;
    processEvent event;
    processWEvent wevent;
    process8Elements elements;
    processAsym asym;
    WaveFormMap wfm; 
    WaveForm2Par wf2par; 
    MxChan chan;
    EnergyBin bin;
    ThetaOffset TOff;


    char waveform[100];
    int cnt=0;
    int cnt1=0;
    
    memset(&poldat_, 0, sizeof(poldat_));    
    memset(&rhic_, 0, sizeof(rhic_));    
    memset(&sscal_, 0, sizeof(sscal_));
    memset(&sipar_, 0, sizeof(sipar_));
    memset(&scalers_, 0, sizeof(scalers_));

    chan.term =1;
    if(i_month <=4 || atoi(num1) <405){
       chan.term =0;
    //   printf("num1=%s --> %d\n",num1,chan.term);
    //   getchar();
    }

    printf("dproc.CMODE=%d\n",dproc.CMODE);
    printf("dproc.WAVE_OUT_MODE=%d\n",dproc.WAVE_OUT_MODE);
    // reading the data till its end ...
    FILE *fp = fopen(datafile,"r");


    reset8Elements(&elements);
    resetAsym(&asym);
   //Run2005 readMap(&wfm);
    readMxChan(&chan);
    setEbin(&bin);


    int flag = 0; // exit from while when flag==1
    int last_blue=-1;
    int last_yellow=-1;
    int yel_frag=0;
    while (flag==0) {


        if (fread(&rec.header, sizeof(recordHeaderStruct), 1, fp)!=1){
            break;
        }

        if (feof(fp)) {
            fprintf(stdout,"Expected end of file\n");
            break;
        }
        
        if (rec.header.len > sizeof(rec)){
            fprintf(stdout,"Not enough buffer d: %d byte b: %d byte\n",
                    rec.header.len, sizeof(rec));
            break;
        }

        // Read rest of the structure
        int rval = fread(&rec.begin.version, rec.header.len - 
                     sizeof(recordHeaderStruct),1,fp);
        if (feof(fp)) {
            perror("Unexpected end of file");
            exit(1);
            break;
        }
        
        if (rval != 1) {
            fprintf(stdout,"IO error in stream (header) %s\n",strerror(errno));
            continue;  
        }

        
        switch (rec.header.type & REC_TYPEMASK) {
        case REC_BEGIN:
//okadah//////////////////////
            printf("REC_BEGIN\n");
//	    //getchar();
////////////////////////////////
            cout << "Begin of data stream Version: "<<rec.begin.version<<endl;
            cout << "Comment: "<< rec.begin.comment<<endl;
            cout << "Time: "<< ctime(&rec.begin.header.timestamp.time) <<endl;
            cout << "End of data stream begining"<<endl;
            break;
        case REC_POLADO:
            printf("REC_POLADO\n");
            break; // later
        case REC_TAGADO:
            printf("REC_TAGADO\n");
            memcpy(&tgtdat1_, &rec.tag.data[0], sizeof(tgtdat1_));	    
	    memcpy(&tgtdat2_, &rec.tag.data[1], sizeof(tgtdat2_));	    
	    break;         
        case REC_WCMADO:
//okadah//////////////////////
            printf("REC_WCMADO\n");
//	    //getchar();
////////////////////////////////
            fprintf(stdout,"Reading WCM information\n");
            memcpy(&wcmdat, &rec.wcmado.data,sizeof(wcmdat));
            wevent.WCMcount=0;
            for (int bid=0;bid<120;bid++){
                wcmdist[bid] = wcmdat.fillDataM[bid*3];
                //printf("WCM[%d]=%f\n",bid,wcmdist[bid]);
                if(wcmdist[bid] > 10) wevent.WCMcount++;
            }
            printf("WCMcount = %d\n",wevent.WCMcount);
            //getchar();
            break;
        case REC_BEAMADO:  //new version
            printf("REC_BEAMADO\n");
            memcpy(&beamdat_, &rec.beam.data, sizeof(beamdat_));	    
            printf("fillNumber=%ld\n",beamdat_.fillNumberM);
            printf("BeamEnergy=%lf\n",beamdat_.beamEnergyM);
            if(dproc.beam !=1 && dproc.beam !=2) dproc.beam=1;
	    for (i=0;i<120;i++) {
		rhic_.fillpat[i] = beamdat_.measuredFillPatternM[3*i];
		rhic_.polpat[i] = beamdat_.polarizationFillPatternS[3*i];
                if(dproc.beam==1){//Yellow config
//Yellow from CDEV
                  elements.FillPatY[i]=rhic_.fillpat[i];
                  elements.PolPatY[i]=rhic_.polpat[i];
//Blue
                  elements.FillPatB[i]=rhic_.fillpat[i];
                  if(elements.FillPatB[i]>0){
                    if( last_blue <0){
                      elements.PolPatB[i]=1;
                      last_blue = 1;
                    }else{
                      elements.PolPatB[i]=-1;
                      last_blue = -1;
                    }
                  }
                }else{//Blue config
//Blue from CDEV
                  elements.FillPatB[i]=rhic_.fillpat[i];
                  elements.PolPatB[i]=rhic_.polpat[i];
//Yellow
                  elements.FillPatY[i]=rhic_.fillpat[i];
                  if(elements.FillPatY[i]>0){
                    if( last_yellow <0){
                      elements.PolPatY[i]=1;
                      if(yel_frag==1){
                        last_yellow = 1;
                        yel_frag=0;
                      }else{
                        yel_frag=1;
                      }
                    }else{
                      elements.PolPatY[i]=-1;
                      if(yel_frag==1){
                        last_yellow = -1;
                        yel_frag=0;
                      }else{
                        yel_frag=1;
                      }
                    }
                  }
                }
	    }

            if(dproc.beam==1)printf("Now is YELLOW!\n");
            if(dproc.beam==2)printf("Now is BLUE!\n");
	    printf("Fill pattern: ");
	    for (i=0;i<120;i++) {
		printf("%c",(rhic_.fillpat[i]!=0)?'|':'_');
	    }
	    printf("\n");
	    printf("Pol. pattern: ");
	    for (i=0;i<120;i++) {
		char cc;
		if (rhic_.polpat[i] > 0) cc='+';
		else {
		    if (rhic_.polpat[i] < 0) cc='-';
		    else cc='0';
		}
		printf("%c", cc);
	    }
	    printf("\n");//getchar();

	    printf("==================================================\n");
	    printf("Fill patternY: ");
	    for (i=0;i<120;i++) {
		printf("%c",(elements.FillPatY[i]!=0)?'|':'_');
            }
	    printf("\n");
	    printf("Pol. patternY: ");
	    for (i=0;i<120;i++) {
		char cc;
		if (elements.PolPatY[i] > 0) cc='+';
		else {
		    if (elements.PolPatY[i] < 0) cc='-';
		    else cc='0';
		}
		printf("%c", cc);
	    }
	    printf("\n");
	    printf("---------------------------------------------------\n");

	    printf("Fill patternB: ");
	    for (i=0;i<120;i++) {
		printf("%c",(elements.FillPatB[i]!=0)?'|':'_');
            }
	    printf("\n");
	    printf("Pol. patternB: ");
	    for (i=0;i<120;i++) {
		char cc;
		if (elements.PolPatB[i] > 0) cc='+';
		else {
		    if (elements.PolPatB[i] < 0) cc='-';
		    else cc='0';
		}
		printf("%c", cc);
	    }
	    printf("\n");
            //getchar();
/*
            FILE *BeamFile;
            char sdmy1[256];
            int idmy1,idmy2,idmy3,idmy4,idmy5;
            BeamFile= fopen("BuYe_pat.txt","r");
            fscanf(BeamFile,"%s",&sdmy1);
            for(i=0;i<120;++i){
              fscanf(BeamFile,"%d,%d,%d,%d,%d",&idmy1,&idmy2,&idmy3,&idmy4,&idmy5);
              printf("%d,%d,%d,-->%d,%d\n",idmy1,idmy4,idmy5,elements.FillPatY[i],elements.PolPatY[i]);
              if(i%10==0 && i > 0)getchar();
            }
            getchar();
 */           
	    break;
        case REC_END:
//okadah//////////////////////
            printf("REC_END\n");
	    //getchar();
////////////////////////////////
            fprintf(stdout,"End of data stream \n");
            fprintf(stdout,"End Time: %s\n", 
                    ctime(&rec.end.header.timestamp.time));
            fprintf(stdout,"%ld records found\n",Nread);
            fprintf(stdout,"Total number of counts \n");
            fprintf(stdout,"%ld Carbons are found in\n",Nevcut);
            fprintf(stdout,"%ld Total events of intended fill pattern\n",
                    Nevtot);
            fprintf(stdout,"Data Comment: %s\n", rec.end.comment);
/*
            if (EndProcess(cfginfo)!=0) {
                fprintf(stdout, "Error at end process \n");
            }
*/
            flag = 1;
            break;
        case REC_READRAW:
            printf("REC_READRAW\n");
        case REC_READSUB:
//okadah//////////////////////
            printf("REC_READSUB\n");
	    atraw_.id = rec.header.timestamp.delim;
	    atdata_.d = atraw_.id;
            wevent.delim = atraw_.id;
            if(old_delim!=wevent.delim){
                for(int k=0;k<96;++k){
            //           printf("rev_num[%d]=%d\n",k,rev_num[k]);
                       rev_num[k]=0;
             //          if(k%16==0)getchar();
                    }
                    old_delim=wevent.delim;
                    old_bunch=0;
                    old_rev=0;
                    total_event=-1;
              } 
	    if (rec.header.type & REC_120BUNCH) {
/*
		for (i = 0; i < rec.header.len - sizeof(recordHeaderStruct);) {
		    wave120Ptr = (recordReadWave120Struct *) (&rec.data.rec[i]);
		    k = wave120Ptr->subhead.siNum;
		    l = wave120Ptr->subhead.Events + 1;
		    i += sizeof(subheadStruct) + l*sizeof(waveform120Struct);
		    if (i > rec.header.len - sizeof(recordHeaderStruct)) {
			printf("Broken record %d (%d bytes). Last subhead: siNum=%d  Events=%d\n", 
			    rec.header.num, rec.header.len, k+1, l);
			break;
		    }
		    for (j = 0; j < l; j++) {
			memcpy(waveform, wave120Ptr->data[j].d, sizeof(wave120Ptr->data[j].d));
			atraw_.ia = 0;
			atdata_.a = 0;
			atraw_.it = 0;
			atdata_.t = 0;
			atraw_.itmax = 0;
			atdata_.tmax = 0;
			atraw_.is = 0;
			atdata_.s = 0;
			atraw_.ib = wave120Ptr->data[j].b;
			atdata_.b = atraw_.ib;
			atraw_.irev = *((long *)(wave120Ptr->data[j].rev));
			atraw_.irev = 2*atraw_.irev  + wave120Ptr->data[j].rev0;
			atdata_.rev = atraw_.irev;
			m = sizeof(wave120Ptr->data[j].d); 
			n = rec.header.type & REC_TYPEMASK;
			wfana_(waveform, &m, &n);
			if (runpars_.intp) HFNT(k+1);
			if (cnt1 > 500000) runpars_.intp = 0;
			cnt1++;
		    }
		}
*/
	    } else {
                int tmp_frag=0;
                int process_frag=1;
                int i,j,k,l,m,n;
		for (i = 0; i < rec.header.len - sizeof(recordHeaderStruct);) {
		    wavePtr = (recordReadWaveStruct *) (&rec.data.rec[i]);
		    k = wavePtr->subhead.siNum;
                    wevent.strip_no=wavePtr->subhead.siNum;
		    l = wavePtr->subhead.Events + 1;
		    i += sizeof(subheadStruct) + l*sizeof(waveformStruct);
		    if (i > rec.header.len - sizeof(recordHeaderStruct)) {
			printf("Broken record %d (%d bytes). Last subhead: siNum=%d  Events=%d\n", 
			    rec.header.num, rec.header.len, k+1, l);
			break;
		    }
		    for (j = 0; j < l; j++) {
			memcpy(waveform, wavePtr->data[j].d, sizeof(wavePtr->data[j].d));
			atraw_.ia = 0;
			atdata_.a = 0;
			atraw_.it = 0;
			atdata_.t = 0;
			atraw_.itmax = 0;
			atdata_.tmax = 0;
			atraw_.is = 0;
			atdata_.s = 0;
			atraw_.ib = wavePtr->data[j].b;
			atdata_.b = atraw_.ib;
			atraw_.irev = *((long *)(wavePtr->data[j].rev));
			atraw_.irev = 512*wevent.delim + 2*atraw_.irev  + wavePtr->data[j].rev0;
			atdata_.rev = atraw_.irev;
			m = sizeof(wavePtr->data[j].d); 
			n = rec.header.type & REC_TYPEMASK;
                       // printf("delim=%d strip=%d atraw_.ia=%ld it=%ld itmax=%ld ib=%ld irev=%ld is=%lf\n",
                       //         wevent.delim,wevent.strip_no,atraw_.ia,atraw_.it,atraw_.itmax,atraw_.ib,atraw_.irev,atraw_.is);
//read Waveform
                        for(kkk=0;kkk<m;++kkk){
                               //wave[kkk]=ALLPtr->data[j].d[kkk];
                               wave[kkk]=waveform[kkk];
                              if(wave[kkk]<180){
                                tmp_frag=1;
                              }
			}
                        for(kkk=0;kkk<m;++kkk){
                              if(wave[kkk]<180){
                                tmp_frag=1;
                                break;
                              }
			}
                        int ssss;
                        ssss=(int)(wevent.strip_no+1); 
                        wevent.amp=(int)atraw_.ia;
                        wevent.amp=(int)(wevent.amp*ADC_mean/ADC_cal[wevent.geo]);
                        wevent.t_time=(int)atraw_.it ;
                        wevent.dummy1=0;
                        wevent.bunch=(int)atraw_.ib;
                        wevent.dummy2=0;
                        wevent.tmax=(int)atraw_.itmax;
                        wevent.intg=(int)atraw_.is;
                        wevent.geo=geo_ch[ssss];
                        wevent.revolution=(int)atraw_.irev;
			if(wevent.dummy1 != 0 || wevent.dummy2 != 0){
				printf("Error dummy1=%d dummy2=%d\n",wevent.dummy1,wevent.dummy2);
				dmy_cont++;
			//	getchar();
			}	
                        printf("now is REC_READRAW of REC_READSUB\n");
                        printf("delim=%d strip=%d atraw_.ia=%ld it=%ld itmax=%ld ib=%ld irev=%ld is=%lf\n",
                                wevent.delim,wevent.strip_no,atraw_.ia,atraw_.it,atraw_.itmax,atraw_.ib,atraw_.irev,atraw_.is);
                       
//TEMP TEMP
                       for(int k=0;k<12;++k){
                           FitResult[k]=0;
                       }
                        wevent.amp2=(int)g_ats[0];
                        wevent.t_time2=(int)g_ats[1];
                        wevent.square2=(int)g_ats[2];
                        wevent.tmax2=(int)g_ats[3];
                        wevent.amp3=(int)wevent.amp;
                        wevent.t_time3=(int)wevent.t_time;
                        wevent.square3=g_ats[4];
                        wevent.tmax3=(int)g_ats[5];
                        wevent.Energy=wevent.amp3;
                        wevent.ToF=g_ats[1];
                        wevent.f_amp=(double)FitResult[0];
                        wevent.f_t_time=(double)FitResult[2];
                        wevent.f_square=(double)FitResult[4];
                        wevent.f_tmax=(double)FitResult[1];
                        wevent.f_ndf=(int)FitResult[5];
                        wevent.f_chi=(double)FitResult[6];
                        wevent.f_cndf=(double)FitResult[7];
                        wevent.f_start=(int)FitResult[8];
                        wevent.f_stop=(int)FitResult[9];
// Event Processing
		        if((tmp_frag==1 && dproc.CMODE==1) || process_frag>0)EventProcessWaveform(&wevent,cfginfo,wave, hist_num,mode,all_event);
		        if((tmp_frag==1 && dproc.CMODE==1) || process_frag>0)EventProcessRootNtuple(&wevent,cfginfo, hist_num,mode,all_event);
			cnt1++;
		    }
		}
	    }
	    break;
//	    getchar();
////////////////////////////////
            break; // later
        case REC_READALL:
//okadah//////////////////////
            nnkk++;
            //printf("now is REC_READALL  nnkk=%d \n",nnkk);getchar();
            if(nnkk%1000000==0 )printf("now is REC_READALL  nnkk=%d \n",nnkk);
            if(wave_cont>300)break;

	    for(kkk=0;kkk<mode;++kkk)wave[kkk]=0;
            if (Nevtot<NEVENTS) {
                int process_frag=1;
                wevent.delim = rec.header.timestamp.delim;
                if(hjet_frag==1){
                    hjet_frag=0;
                    total_event=-1;
                } 
	      if (rec.header.type & REC_120BUNCH) {
                if(all_event==0)printf("120bunch mode\n");
                int i,j,k,l,m,n;
		for (i = 0; i < rec.header.len - sizeof(recordHeaderStruct);) {
		    ALL120Ptr = (recordALLMode120Struct *) (&rec.data.rec[i]);
		    k = ALL120Ptr->subhead.siNum;
                    wevent.strip_no=ALL120Ptr->subhead.siNum;
		    l = ALL120Ptr->subhead.Events + 1;
		    i += sizeof(subheadStruct) + l*sizeof(ALLMode120Struct);
		    if (i > rec.header.len - sizeof(recordHeaderStruct)) {
			printf("Broken record %d (%d bytes). Last subhead: siNum=%d  Events=%d\n", 
			    rec.header.num, rec.header.len, k+1, l);
			break;
		    }
		    for (j = 0; j < l; j++) {
			memcpy(waveform, ALL120Ptr->data[j].d, sizeof(ALL120Ptr->data[j].d));
			atraw_.ia = ALL120Ptr->data[j].a;
			atdata_.a = atraw_.ia * sipar_.ecoef[k] + sipar_.edead[k];
			atraw_.it = ALL120Ptr->data[j].t;
			atdata_.t = atraw_.it * 0.5 * sipar_.nsperchan;
			atraw_.itmax = ALL120Ptr->data[j].tmax;
			atdata_.tmax = atraw_.it * 0.5 * sipar_.nsperchan;
			atraw_.is = ALL120Ptr->data[j].s << (2+sipar_.idiv);
			atdata_.s = (atraw_.is * sipar_.ecoef[k] + sipar_.edead[k]) * sipar_.nsperchan;
			atraw_.ib = ALL120Ptr->data[j].b;
			atdata_.b = atraw_.ib;
			atraw_.irev = 512*wevent.delim + 2*ALL120Ptr->data[j].rev +  ALL120Ptr->data[j].rev0;
			atdata_.rev = atraw_.irev;
			m = sizeof(ALL120Ptr->data[j].d); 
			n = REC_READSUB;
///////////////////////////////////////120bunch mdoe
                        int ssss;
                        int si_typ=0;
                        ssss=(int)(wevent.strip_no+1); 
                        wevent.amp=(int)atraw_.ia;
                        event.amp0=(int)atraw_.ia;//for histgram calib_0-96
                        wevent.t_time=(int)atraw_.it ;
                        wevent.dummy1=0;
                        wevent.square=(int)atraw_.is;
                        wevent.bunch=(int)atraw_.ib;
                        wevent.dummy2=0;
                        wevent.tmax=(int)atraw_.itmax;
                        wevent.intg=(int)atraw_.is;
                        wevent.geo=geo_ch[ssss];//REC_READALL
                        double ttt;
                        if(dproc.beam==1)ttt=2;//yellow BZdelay =6
                        if(dproc.beam==2)ttt=-5;//blue BZdelay =7
                        if(wevent.geo == 17 || wevent.geo == 41 || wevent.geo ==45 || wevent.geo ==48){
                           if(wevent.geo==17)tdc_geta = ttt+5;//tdc_check
                           if(wevent.geo==41)tdc_geta = ttt+10;//tdc_check
                           if(wevent.geo==45)tdc_geta = ttt+10;//tdc_check
                           if(wevent.geo==48)tdc_geta = ttt+5.5;//tdc_check
                        }else{
                           tdc_geta = ttt;//tdc_check
                        }
                        //printf("dproc.beam=%d ttt=%lf\n",dproc.beam,ttt);getchar();
                        wevent.revolution=(int)atraw_.irev;
			if(wevent.dummy1 != 0 || wevent.dummy2 != 0){
				printf("Error dummy1=%d dummy2=%d\n",wevent.dummy1,wevent.dummy2);
				dmy_cont++;
			//	getchar();
			}	
                        si_typ=0;
                        if((wevent.geo>=1 && wevent.geo<=16) || (wevent.geo>=33 && wevent.geo<=64) ||
                            (wevent.geo>=81 && wevent.geo<=96)){
                           si_typ=1; //HAMAMATSU
                        }else{
                           si_typ=0;//BNL
                        }

//read Waveform//120bunch mode
                        for(kkk=0;kkk<m;++kkk){
                               wave[kkk]=ALL120Ptr->data[j].d[kkk];
                           //    printf("%d %d\n",kkk,wave[kkk]);
                               //wave[kkk]=waveform[kkk];
			}
                        //getchar();
                        get_frag=GetAts(wave,g_ats,mode);
//check//////////////////////

                       // if(g_ats[0]>20){
                          //if(pow(g_ats[0]-atraw_.ia,2)>=4 || pow(g_ats[1]-atraw_.it,2)>4 || pow(g_ats[3]-atraw_.itmax,2)>4){
                          if(sqrt(pow(g_ats[2]-atraw_.is,2))>10 ){
/*
			    for(kkk=0;kkk<30;++kkk){
				printf("kkk=%d %d||kkk=%d %d || kkk=%d %d\n",kkk,wave[kkk],
                                  kkk+30,wave[kkk+30],kkk+60,wave[kkk+60]);
		   	    }
*/
                        //    printf("atraw_.ia=%ld atraw_.it=%ld itmax=%ld is=%ld\n",atraw_.ia,atraw_.it,atraw_.itmax,atraw_.is);
         //                 printf("atdata_.a=%f %f %f\n",atdata_.a,atdata_.t,atdata_.tmax);
                            intg_check++;
                        //    getchar();
                           }
                       // }

//////////////////////////////////120bunch mode
                       evt_chan[wevent.geo-1]++;
                       ch_select=0;
                       if(dproc.SELECT_CHAN>0){
                          //printf("select_chan=%d\n",dproc.SELECT_CHAN);
                          if(wevent.geo==dproc.SELECT_CHAN ){
                            ch_select=1;
                          }
                       }else{
                         if(dproc.SELECT_CHAN==-1){
                            ch_select=1;//Fitting n&tau check 
                         }
                       }
//////////////////////////////////120bunch mode
                       wevent.amp2=(int)g_ats[0];
                       wevent.t_time2=(int)g_ats[1];
                       wevent.square2=(int)g_ats[2];
                       wevent.tmax2=(int)g_ats[3];
                       wevent.t_time3=wevent.t_time*1.2;//ns
                       wevent.square3=31;//g_ats[4];//region
                       wevent.tmax3=wevent.tmax*1.2;//ns

                       double random=(rand()%10)/(double)10;
                      // printf("random=%lf\n",random);getchar();
                       //wevent.ToF=(random+wevent.t_time2)*1.19;//nsec
                       wevent.ToF=wevent.t_time2*1.19;//nsec
                       //wevent.ToF=wevent.ToF-geta_ch[wevent.geo];

//AT-mode
                       wevent.Energy=wevent.amp*Am_E[0]/(double)ADC_cal[wevent.geo];//MeV,AT-mode
                       wevent.ToF=wevent.t_time*1.19;//nsec,AT-mode
                       
                       wevent.region=(int)g_ats[5];
                       wevent.hjet_count=hjet_count;
                       wevent.target_inf=atraw_.ijet;
                       wevent.timeG = AreaG;

///////////////////////////////////120bunch mode
// Event Processing
                       double dt=1.184;
                       double checkE_tof=banana(wevent.Energy,wevent.geo);
                       double checkE_tdc=checkE_tof/(double)dt+tdc_geta;

////////////Banana cut////////////////////120bunch mode
                       double dT_check;
                       double dt_rangeUP;//ns
                       double dt_rangeBIG=10;//ns
                       if(dproc.ToF_W==1) dt_rangeUP=8.0;
 

//Energy Cut Am & Gd/////////////////////////////////////////////120bunch mode
     int energy_cut=0;
     int si= wevent.geo/(int)16;
     if(wevent.geo%16==0)si=si-1;
     if(si==0 || si==2 || si==3 || si==5){//HAMAMATSU
      //if(wevent.Energy < 2.5 || (wevent.Energy >= 3.0 && wevent.Energy < 4.8) )energy_cut=1;
      if(wevent.Energy < 2.5 || wevent.Energy >= 3.0  )energy_cut=1;
     }else{
      //if(wevent.Energy < 2.8 || (wevent.Energy >= 3.0 && wevent.Energy < 5.3))energy_cut=1;
      if(wevent.Energy < 2.8 || wevent.Energy >= 3.0 )energy_cut=1;
     }

///////////////////////////////////120bunch mode

                       if( ch_select==1 && dproc.WAVE_FIT_MODE==1 && (int)g_ats[5]==31  ){  
//before FITTING
//FitResult[0]=Pmax//FitResult[1]=fbinmax;//FitResult[2]=fbin//FitResult[3]=intg_middle
//FitResult[4]=intg_fit//FitResult[5]=ndf//FitResult[6]=chi//FitResult[7]=cndf
//FitResult[8]=range_min//FitResult[9]=range_max;
//FitResult[10]=f_tau//FitResult[11]=f_n;
                         // FitWaveform(wave,FitResult,FitMaskResult,wevent.geo,mode,&wf2par);
                          FitWaveMask900(&wfm,wave,FitResult,wevent.geo,mode);
                          if(fit_event%10000==0 ){
                             printf("fit_event=%d ch=%d chi=%lf ndf=%lf cndf=%lf \n",fit_event,wevent.geo,FitResult[6],FitResult[5],FitResult[7]);
                          }
                          AreaG=0;
                          AreaS=0;
                          int start=(int)FitResult[8]; 
                          int stop=(int)FitResult[9]; 
                          for(k=start;k<=stop;++k){
                              AreaG=AreaG+wave[k]*k;
                              AreaS=AreaS+wave[k];
                          }
                          AreaG=AreaG/(double)AreaS;

                          //printf("FitResult[3]=%lf FitResult[4]=%lf g_ats[4]=%lf\n",FitResult[3],FitResult[4],g_ats[4]);
                          //getchar();
                          //for(int k=0;k<12;++k){
                          //  printf("FitResult[%d]=%lf\n",k,FitResult[k]);
                          // }
                          // getchar();
                          fit_event++;
                          if(FitResult[6]<0)fit_err++;
                       }else{
                             for(int k=0;k<12;++k){
                               FitResult[k]=0;
                             }
                             FitResult[6]=-1;//chi
                             FitResult[7]=-1;//cndf
                             //if((int)g_ats[5]!=31)FitResult[5]=(int)g_ats[5];
                             if((int)g_ats[5]==31)FitResult[5]=28;
                       }
                       wevent.f_amp=(double)FitResult[0];
                       wevent.f_t_time=(double)FitResult[2];
                       wevent.f_square=(double)FitResult[4];
                       wevent.f_tmax=(double)FitResult[1];
                       wevent.f_ndf=(int)FitResult[5];
                       wevent.f_chi=(double)FitResult[6];
                       wevent.f_cndf=(double)FitResult[7];
                       wevent.f_start=(int)FitResult[8];
                       wevent.f_stop=(int)FitResult[9];
                       wevent.f_tau=(double)FitResult[10];
                       wevent.f_n=(double)FitResult[11];

		       if(hist_num>0  ){
//Missing Mass and Recoiled Mass///////////////////120bunch mode
                          int si;
                          si=(int)((wevent.geo-1)/16);//si=0-5

                          double Theta0,Theta;
                          Theta=ch2theta(wevent.geo);
                          if(wevent.geo==43){
                            double miss_theta=ch2theta(44);
                            Theta=(Theta+miss_theta)/(double)2; 
                          }

                          wevent.Theta=asin(Theta)*1000;//mrad
                          Theta=wevent.Theta+TOff.offset[si];
                          if(si==3){//lower -t no angle is difficult.....
                            if(wevent.geo==52)Theta=wevent.Theta-16;
                            if(wevent.geo==53)Theta=wevent.Theta-12.6;
                            if(wevent.geo==52)Theta=wevent.Theta-12.6;
                            if(wevent.geo==55)Theta=wevent.Theta-11.5;
                            if(wevent.geo==56)Theta=wevent.Theta-10.3;
                            if(wevent.geo==57)Theta=wevent.Theta-10.15;
                            if(wevent.geo==58)Theta=wevent.Theta-10;
                            if(wevent.geo==59)Theta=wevent.Theta-9.85;
                            if(wevent.geo==60)Theta=wevent.Theta-9.7;
                            if(wevent.geo==61)Theta=wevent.Theta-9.2;
                            if(wevent.geo==62)Theta=wevent.Theta-8.5;
                          }
                          if(si==4){
                            if(wevent.geo==68)Theta=wevent.Theta-17.5;
                            if(wevent.geo==69)Theta=wevent.Theta-13.9;
                            if(wevent.geo==70)Theta=wevent.Theta-12.1;
                          }
                          if(si==5){
                            if(wevent.geo==84)Theta=wevent.Theta-19.1;
                            if(wevent.geo==85)Theta=wevent.Theta-16.4;
                            if(wevent.geo==86)Theta=wevent.Theta-14.2;
                            if(wevent.geo==87)Theta=wevent.Theta-14.2;
                          }
                          wevent.Theta=Theta;//check

                            if(dproc.JET_EVENT_MODE==1 &&  total_event==-1){
                                int test1=0;
                                printf("new_data_set start!! all_event=%d\n",all_event);
                                getchar();
                            }
                            if(all_event==all_event_old)all_event++;
                            all_event_old=all_event;

// waveform check////////////120bunch mode
                            if(dproc.WAVE_OUT_MODE==1){
                              double ToF0,dT_check;
                              ToF0=get_ToF(wevent.Energy,wevent.geo);
                              dT_check=sqrt(pow(wevent.ToF0-(wevent.ToF-tdc_geta*dt),2));
                              if(  ch_select==1  &&  wevent.Energy >0.5 && dT_check < 8){
//TDC=25 check
                              //if(  ch_select==1 &&  wevent.t_time == 25){     
                              //if(  ch_select==1 &&  (wevent.t_time-tdc_geta)*dt > 55 && (wevent.t_time-tdc_geta)*dt < 65  
                              //    && wevent.Energy > 2.8 && wevent.Energy < 3.2){     
                              //if(  ch_select==1 &&   (wevent.square3 > 900 && wevent.amp2 > 200)){//Too big event check  
//Fit check
                              //if(  ch_select==1 && g_ats[5] == 31 && wevent.tmax2<(68*2) && wevent.tmax2 > (9*2) &&     
                              //   (dproc.WAVE_FIT_MODE==0 || (dproc.WAVE_FIT_MODE==1 && FitResult[7]>=0 && FitResult[7]<1)) ){ //Fit check
                               // (dproc.WAVE_FIT_MODE==0 || (dproc.WAVE_FIT_MODE==1 &&  FitResult[7]>=10 )) ){ //Fit check

//test pluse check
                                //if(  ch_select==1 && wevent.t_time2<120 && wevent.t_time2>20 &&  
                                // (dproc.WAVE_FIT_MODE==0 || (dproc.WAVE_FIT_MODE==1 && FitResult[7]>0 && FitResult[7]<1)) ){ 

                                 if(wave_cont>300){
                                  //  printf("Waveform count=50\n");
                                    break;
                                  }
                                  wave_cont++;
                                  if(wave_cont%100==0 && wave_cont>0)printf("wave_cont=%d\n",wave_cont);

                                  int cut_frag=0;
                                  double cut_seppen;
                                  double cut_slope;
                                  cut_slope = wfm.ratio_All[wevent.geo-1];
                                  cut_seppen=wfm.seppen;
                                  if(cut_slope != -1){
                                    cut_frag=CutBadWf(cut_slope,wevent.geo, wevent.amp2,wevent.square3,cut_seppen);
                                  }else{
                                    cut_frag =1;
                                  }
                              //    if( cut_frag==1  ){
                                     tfileTestWave->cd();
                                     EventProcessWaveform(&wevent,cfginfo,wave, hist_num,mode,wave_cont);
                              //    }
                                }
                            }//if(dprooc.WAVE_OUT_MODE==1)END

                            if(wevent.geo==dproc.SELECT_CHAN){
                              if(wevent.region==31)tmp_i1++;
                              if(wevent.region!=31)tmp_i2++;
                            }


                           if(dproc.JET_EVENT_MODE==1){
                               if(process_frag>0){
                                  //printf("Output Event\n");
                                  tfileTestEvent->cd();
                                  InputVector(&wevent,&total_event,all_event);
                               }
                           }

                          int HighTofCut=0;
                          int iner;
                          double AveToF0;

                          iner=wevent.geo%16;
                          if(iner==0)iner=16;

                          wevent.InE=wevent.Energy;

                          if(si_typ==1 && wevent.Energy > 7.0)wevent.InE=-1;//HAMAMATSU
                          if(si_typ==0 && wevent.Energy > 7.3)wevent.InE=-1;//BNL
                          wevent.AveE=get_Energy(Theta);//mrad input ; MeV output
                          if( (wevent.ToF-tdc_geta*dt)>0 && (wevent.ToF-tdc_geta*dt)<40 )HighTofCut=1;      
                          if(iner > 6 && iner <= 16 && wevent.AveE>6.5){
                             AveToF0=get_ToF(wevent.AveE,wevent.geo);
                          }

                          if(wevent.geo>=17 && wevent.geo<=32 ){ //BNL
                              if( wevent.geo >=25){
                                   if(HighTofCut==1 ){                            
                                       wevent.InE=GetInE(wevent.Energy,wevent.geo);
                                    }
                              }
                          }
                          if(wevent.geo>=65 && wevent.geo<=80){ //BNL
                              if( wevent.geo >= 75){
                                   if(HighTofCut==1 ){                            
                                       wevent.InE=GetInE(wevent.Energy,wevent.geo);
                                    }
                              }
                          }
                          if(si_typ==1 &&  wevent.geo < 49 ){//HAMAMATSU
                              if( (wevent.geo >=9 && wevent.geo <=16) || wevent.geo >=41){
                                   if(HighTofCut==1 ){                            
                                       wevent.InE=GetInE(wevent.Energy,wevent.geo);
                                   }
                              }
                          }
                          if(si_typ==1 && wevent.geo > 48){//HAMAMATSU
                              if( (wevent.geo>=59 && wevent.geo<=64) || wevent.geo >= 91){
                                   if(HighTofCut==1 ){                            
                                       wevent.InE=GetInE(wevent.Energy,wevent.geo);
                                   }
                              }
                          }
                          wevent.InE=wevent.Energy;
                         

//Mx,Mp,ToF0,Theta0/////////////////////////////////////////120bunch mode
                          ToF0=get_ToF(wevent.InE,wevent.geo);
                          double DepE = wevent.Energy; 
                          int ff=0;
                          if( ToF0 - (wevent.ToF-tdc_geta*dt) < -12 ){
                              ff=1;
                              wevent.InE = 0.0006*pow(DepE,5)-0.0087*pow(DepE,4)+0.0431*pow(DepE,3)-0.0915*pow(DepE,2)+1.0523*DepE+0.1397;
     //                         printf("ToF0=%lf %lf DepE=%lf-->%lf\n",ToF0,wevent.ToF-tdc_geta*dt,DepE,wevent.InE);getchar();
                          }
                          Mx=GetMx(wevent.InE,Theta);
                          Mp=get_Mp(wevent.InE,wevent.ToF,tdc_geta*dt,ff);
                          Theta0=get_Theta(wevent.InE);
                          if(Mp>0 ){
                              //printf("Mp=%lf Mx=%lf amp=%d tdc=%d\n",Mp,Mx,wevent.amp,wevent.t_time);
                              //getchar();
                          }else{
                               Mx=-5;
                               Mp=-5;
                          }
                          wevent.Mp=Mp;
                          wevent.Mx=Mx;
                          wevent.ToF0=ToF0;
                          //wevent.ToF0=AveToF0;
                          wevent.Theta0=asin(Theta0)*1000;//mrad
                          dT_check=sqrt(pow(wevent.ToF0-(wevent.ToF-tdc_geta*dt),2));

//Target profile check //120bunch mode
                       //if((wevent.t_time-tdc_geta)*1.19 >30 && (wevent.t_time-tdc_geta)*1.19 <50 
                       if(dT_check <8 
                            && wevent.Energy >1 && wevent.Energy <2){
                         int si=(wevent.geo-1)/(int)16;
                         int frag=0;

                         if(si==0 && wevent.geo>=13 && wevent.geo<=15 )frag=1;
                         if(si==1 && wevent.geo>=29 && wevent.geo<=31 )frag=1;
                         if(si==2 && wevent.geo>=45 && wevent.geo<=47 )frag=1;
                         if(si==3 && wevent.geo>=58 && wevent.geo<=60 )frag=1;
                         if(si==4 && wevent.geo>=74 && wevent.geo<=76 )frag=1;
                         if(si==5 && wevent.geo>=90 && wevent.geo<=92 )frag=1;

                         if(si==0 && wevent.geo>=5 && wevent.geo<=7 )frag=2;
                         if(si==1 && wevent.geo>=21 && wevent.geo<=23 )frag=2;
                         if(si==2 && wevent.geo>=37 && wevent.geo<=39 )frag=2;
                         if(si==3 && wevent.geo>=50 && wevent.geo<=52 )frag=2;
                         if(si==4 && wevent.geo>=66 && wevent.geo<=68 )frag=2; // was -- wevent.geo<=67
                         if(si==5 && wevent.geo>=83 && wevent.geo<=85 )frag=2;

                         if(frag==2)select_eventY[si]++;  // frag was switched 2 = Y, NOT 1
                         if(frag==1)select_eventB[si]++;  // 1 = B, NOT 2
                       }
//Ntuple//////////////////////////////////////////////////120bunch mode
                           if(dproc.RAW_EVENT_MODE==1){
                             if(  wevent.Energy > 0.5 ){
                                   tfileTestRaw->cd();
                                   EventProcessRootNtuple(&wevent,cfginfo, hist_num,mode,all_event);
                                   //printf("wevent.square3=%lf wevent.square2=%d\n",wevent.square3,wevent.square2);
                                   //getchar();
                             }
                           }
//8 elements//////////////////120bunch mode
                           GetBunchPat(&chan,&wevent);
                           if( dT_check < 8 ){
                           //if( dT_check > 12 ){//Am check
                           //if( (wevent.ToF-dt*tdc_geta)< 15 ){//prompt
                              int cut_frag=0;
                              double cut_seppen;
                              double cut_slope;
                              cut_slope = wfm.ratio_All[wevent.geo-1];
                              cut_seppen= wfm.seppen;
                              if(cut_slope !=-1){
                                cut_frag=CutBadWf(cut_slope,wevent.geo, wevent.amp2,wevent.square3,cut_seppen);
                              }else{
                                cut_frag=1;
                              }
                              //printf("ch=%d,E=%lf InE=%lf TOF=%lf\n",wevent.geo,wevent.Energy,wevent.InE,(wevent.ToF-dt*tdc_geta));
                              Get8Elements(&bin,&chan,&wevent,&elements);
                           } 
//CHECK HIST//////////////////////////////////////////////////120bunch mode
                           //printf("%lf %lf %d\n",wevent.Energy,wevent.ToF,wevent.region);getchar();
                           //if(wevent.region ==31 ){
                             JetEventProcess(&TOff,&wfm,&bin,&chan,&wevent,cfginfo,tdc_geta,FitResult);//ONLINE HIST
                           //}
//CHECK MISMATCH/////////////////////////////120bunch mode
                           if(pow(wevent.amp-wevent.amp2,2)>4){
                              amp_mismatch++;
                           }
                           if(pow(wevent.t_time-wevent.t_time2,2)>4){
                              tdc_mismatch++;
                           }
                           if(pow(wevent.tmax-wevent.tmax2,2)>4){
                              tmax_mismatch++;
                           }
/////////////////////////////////////////////////////120bunch mode
/*
//OUTPUT HBOOK
		            EventProcessPawNtuple(&wevent,cfginfo, hist_num,mode,all_event);
*/
// Event Processing Calibration
                            event.stN=wevent.strip_no;
                            event.amp = wevent.amp;
                            event.tdc = wevent.t_time;
                            event.intg = wevent.square;
                            event.geo = wevent.geo;//geo.ch
                            event.bid = wevent.bunch;       
                            event.rev = wevent.revolution;       
		            event.tmax = wevent.tmax;
                            event.Mp=wevent.Mp;
                            event.Mx=wevent.Mx;

                    
                            tfile->cd();
                            if (EventProcess(&event,cfginfo)!=0) {
                               cout << "Error event process Ev: "<<Nread<<endl;  
                            }
			}//if(hist_num>0  )END
//////////////////////////////////////120bunch mode
			cnt1++;
		    }
		}

	    } else {//Not 120 bunch mode
                if(all_event==0)printf("Not 120bunch mode\n");
                int i,j,k,l,m,n;
                long int start_rev;
                int revUp=0;
		for (i = 0; i < rec.header.len - sizeof(recordHeaderStruct);) {
		    ALLPtr = (recordALLModeStruct *) (&rec.data.rec[i]);
		    k = ALLPtr->subhead.siNum;
                    wevent.strip_no=ALLPtr->subhead.siNum;
		    l = ALLPtr->subhead.Events + 1;
		    i += sizeof(subheadStruct) + l*sizeof(ALLModeStruct);
		    if (i > rec.header.len - sizeof(recordHeaderStruct)) {
			printf("Broken record %d (%d bytes). Last subhead: siNum=%d  Events=%d\n", 
			    rec.header.num, rec.header.len, k+1, l);
			break;
		    }
		    for (j = 0; j < l; j++) {
		        all_event++; //total_event reading
                        process_frag=1;//EventProcessWaveform wo kakuka douka
			memcpy(waveform, ALLPtr->data[j].d, sizeof(ALLPtr->data[j].d));
			atraw_.ia = ALLPtr->data[j].a;
			atraw_.it = ALLPtr->data[j].t;
			atraw_.itmax = ALLPtr->data[j].tmax;
			atraw_.is = ALLPtr->data[j].s << (2+sipar_.idiv);
			//atraw_.is = ALLPtr->data[j].s << (2+Conf.CSR.split.iDiv);//igor's later program
			atraw_.ib = ALLPtr->data[j].b;
			atraw_.irev = 512*wevent.delim + 2*ALLPtr->data[j].rev +  ALLPtr->data[j].rev0;
                        wevent.rev1=ALLPtr->data[j].rev;
                        wevent.rev0=ALLPtr->data[j].rev0;
                        if(i==0 && j==0){
                          start_rev=atraw_.irev;
                          revUp=1;
                        }
                        if(start_rev<atraw_.irev){
                          revUp=1;
                        }
                        if(revUp==1){
                          pol_rev[atraw_.ijet]++; //atraw_.ijet=0-->"+",1-->"-",2-->"0"
                          revUp=0;
                        }
			m = sizeof(ALLPtr->data[j].d); 
			n = REC_READSUB;
                        int ssss;
                        int si_typ=0;
                        ssss=(int)(wevent.strip_no+1); 
                        wevent.amp=(int)atraw_.ia;
                        event.amp0=(int)atraw_.ia;//for histgram calib_0-96
                        wevent.t_time=(int)atraw_.it ;
                        wevent.dummy1=0;
                        wevent.square=(int)atraw_.is;
                        wevent.bunch=(int)atraw_.ib;
                        wevent.dummy2=0;
                        wevent.tmax=(int)atraw_.itmax;
                        wevent.intg=(int)atraw_.is;
                        wevent.geo=geo_ch[ssss];//REC_READALL
                        double ttt;
                        if(dproc.beam==1) ttt=6;//YELLOW
                        if(dproc.beam==2) ttt=-1;//BLUE
                        if(wevent.geo == 17 || wevent.geo == 41 || wevent.geo ==45 || wevent.geo ==48){
                           if(wevent.geo==17)tdc_geta = ttt+5;//tdc_check
                           if(wevent.geo==41)tdc_geta = ttt+10;//tdc_check
                           if(wevent.geo==45)tdc_geta = ttt+10;//tdc_check
                           if(wevent.geo==48)tdc_geta = ttt+5.5;//tdc_check
                        }else{
                           tdc_geta = ttt;//tdc_check
                        }
                        wevent.revolution=(int)atraw_.irev;
			if(wevent.dummy1 != 0 || wevent.dummy2 != 0){
				printf("Error dummy1=%d dummy2=%d\n",wevent.dummy1,wevent.dummy2);
				dmy_cont++;
			//	getchar();
			}	
                        si_typ=0;
                        if((wevent.geo>=1 && wevent.geo<=16) || (wevent.geo>=33 && wevent.geo<=64) ||
                            (wevent.geo>=81 && wevent.geo<=96)){
                           si_typ=1; //HAMAMATSU
                        }else{
                           si_typ=0;//BNL
                        }

//read Waveform
                        for(kkk=0;kkk<m;++kkk){
                               wave[kkk]=ALLPtr->data[j].d[kkk];
                              // printf("%d %d\n",kkk,wave[kkk]);
                               //wave[kkk]=waveform[kkk];
			}
                        //getchar();
                        get_frag=GetAts(wave,g_ats,mode);
//check//////////////////////

                       // if(g_ats[0]>20){
                          //if(pow(g_ats[0]-atraw_.ia,2)>=4 || pow(g_ats[1]-atraw_.it,2)>4 || pow(g_ats[3]-atraw_.itmax,2)>4){
                          if(sqrt(pow(g_ats[2]-atraw_.is,2))>10 ){
/*
			    for(kkk=0;kkk<30;++kkk){
				printf("kkk=%d %d||kkk=%d %d || kkk=%d %d\n",kkk,wave[kkk],
                                  kkk+30,wave[kkk+30],kkk+60,wave[kkk+60]);
		   	    }
*/
                        //    printf("atraw_.ia=%ld atraw_.it=%ld itmax=%ld is=%ld\n",atraw_.ia,atraw_.it,atraw_.itmax,atraw_.is);
         //                 printf("atdata_.a=%f %f %f\n",atdata_.a,atdata_.t,atdata_.tmax);
                            intg_check++;
                        //    getchar();
                           }
                       // }

//////////////////////////////////
                       evt_chan[wevent.geo-1]++;
                       ch_select=0;
                       if(dproc.SELECT_CHAN>0){
                          //printf("select_chan=%d\n",dproc.SELECT_CHAN);
                          if(wevent.geo==dproc.SELECT_CHAN ){
                            ch_select=1;
                          }
                       }else{
                         if(dproc.SELECT_CHAN==-1){
                            ch_select=1;//Fitting n&tau check 
                         }
                       }
//////////////////////////////////
                       wevent.amp2=(int)g_ats[0];
                       wevent.t_time2=(int)g_ats[1];
                       wevent.square2=(int)g_ats[2];
                       wevent.tmax2=(int)g_ats[3];
                       wevent.t_time3=wevent.t_time*1.2;//ns
                       wevent.square3=g_ats[4];//region
                       wevent.tmax3=wevent.tmax*1.2;//ns

                       double random=(rand()%10)/(double)10;
                      // printf("random=%lf\n",random);getchar();
                       //wevent.ToF=(random+wevent.t_time2)*1.19;//nsec
                       wevent.ToF=wevent.t_time2*1.19;//nsec
                       //wevent.ToF=wevent.ToF-geta_ch[wevent.geo];

//AT-mode
                       wevent.Energy=wevent.amp*Am_E[0]/(double)ADC_cal[wevent.geo];//MeV,AT-mode
                       wevent.ToF=wevent.t_time*1.19;//nsec,AT-mode
                       
                       wevent.region=(int)g_ats[5];
                       wevent.hjet_count=hjet_count;
                       wevent.target_inf=atraw_.ijet;
                       wevent.timeG = AreaG;

///////////////////////////////////
// Event Processing
                       double dt=1.184;
                       double checkE_tof=banana(wevent.Energy,wevent.geo);
                       double checkE_tdc=checkE_tof/(double)dt+tdc_geta;

////////////Banana cut////////////////////
                       double dT_check;
                       double dt_rangeUP;//ns
                       double dt_rangeBIG=10;//ns
                       if(dproc.ToF_W==1) dt_rangeUP=8.0;
                       if(dproc.ToF_W==2) dt_rangeUP=6.0;
                       if(dproc.ToF_W==3) dt_rangeUP=9.0;
 

//Energy Cut Am & Gd/////////////////////////////////////////////
     int energy_cut=0;
     int si= wevent.geo/(int)16;
     if(wevent.geo%16==0)si=si-1;
     if(si==0 || si==2 || si==3 || si==5){//HAMAMATSU
      //if(wevent.Energy < 2.5 || (wevent.Energy >= 3.0 && wevent.Energy < 4.8) )energy_cut=1;
      if(wevent.Energy < 2.5 || wevent.Energy >= 3.0  )energy_cut=1;
     }else{
      //if(wevent.Energy < 2.8 || (wevent.Energy >= 3.0 && wevent.Energy < 5.3))energy_cut=1;
      if(wevent.Energy < 2.8 || wevent.Energy >= 3.0 )energy_cut=1;
     }

///////////////////////////////////

                       if( ch_select==1 && dproc.WAVE_FIT_MODE==1 && (int)g_ats[5]==31  ){  
//before FITTING
//FitResult[0]=Pmax//FitResult[1]=fbinmax;//FitResult[2]=fbin//FitResult[3]=intg_middle
//FitResult[4]=intg_fit//FitResult[5]=ndf//FitResult[6]=chi//FitResult[7]=cndf
//FitResult[8]=range_min//FitResult[9]=range_max;
//FitResult[10]=f_tau//FitResult[11]=f_n;
                         // FitWaveform(wave,FitResult,FitMaskResult,wevent.geo,mode,&wf2par);
                          FitWaveMask900(&wfm,wave,FitResult,wevent.geo,mode);
                          if(fit_event%10000==0 ){
                             printf("fit_event=%d ch=%d chi=%lf ndf=%lf cndf=%lf \n",fit_event,wevent.geo,FitResult[6],FitResult[5],FitResult[7]);
                          }
                          AreaG=0;
                          AreaS=0;
                          int start=(int)FitResult[8]; 
                          int stop=(int)FitResult[9]; 
                          for(k=start;k<=stop;++k){
                              AreaG=AreaG+wave[k]*k;
                              AreaS=AreaS+wave[k];
                          }
                          AreaG=AreaG/(double)AreaS;

                          //printf("FitResult[3]=%lf FitResult[4]=%lf g_ats[4]=%lf\n",FitResult[3],FitResult[4],g_ats[4]);
                          //getchar();
                          //for(int k=0;k<12;++k){
                          //  printf("FitResult[%d]=%lf\n",k,FitResult[k]);
                          // }
                          // getchar();
                          fit_event++;
                          if(FitResult[6]<0)fit_err++;
                       }else{
                             for(int k=0;k<12;++k){
                               FitResult[k]=0;
                             }
                             FitResult[6]=-1;//chi
                             FitResult[7]=-1;//cndf
                             //if((int)g_ats[5]!=31)FitResult[5]=(int)g_ats[5];
                             if((int)g_ats[5]==31)FitResult[5]=28;
                       }
                       wevent.f_amp=(double)FitResult[0];
                       wevent.f_t_time=(double)FitResult[2];
                       wevent.f_square=(double)FitResult[4];
                       wevent.f_tmax=(double)FitResult[1];
                       wevent.f_ndf=(int)FitResult[5];
                       wevent.f_chi=(double)FitResult[6];
                       wevent.f_cndf=(double)FitResult[7];
                       wevent.f_start=(int)FitResult[8];
                       wevent.f_stop=(int)FitResult[9];
                       wevent.f_tau=(double)FitResult[10];
                       wevent.f_n=(double)FitResult[11];

		       if(hist_num>0  ){
//Missing Mass and Recoiled Mass///////////////////
                          int si;
                          si=(int)((wevent.geo-1)/16);//si=0-5

                          double Theta0,Theta;
                          Theta=ch2theta(wevent.geo);
                          if(wevent.geo==43){
                            double miss_theta=ch2theta(44);
                            Theta=(Theta+miss_theta)/(double)2; 
                          }

                          wevent.Theta=asin(Theta)*1000;//mrad
                          Theta=wevent.Theta+TOff.offset[si];
                          if(si==3){//lower -t no angle is difficult.....
                            if(wevent.geo==52)Theta=wevent.Theta-16;
                            if(wevent.geo==53)Theta=wevent.Theta-12.6;
                            if(wevent.geo==52)Theta=wevent.Theta-12.6;
                            if(wevent.geo==55)Theta=wevent.Theta-11.5;
                            if(wevent.geo==56)Theta=wevent.Theta-10.3;
                            if(wevent.geo==57)Theta=wevent.Theta-10.15;
                            if(wevent.geo==58)Theta=wevent.Theta-10;
                            if(wevent.geo==59)Theta=wevent.Theta-9.85;
                            if(wevent.geo==60)Theta=wevent.Theta-9.7;
                            if(wevent.geo==61)Theta=wevent.Theta-9.2;
                            if(wevent.geo==62)Theta=wevent.Theta-8.5;
                          }
                          if(si==4){
                            if(wevent.geo==68)Theta=wevent.Theta-17.5;
                            if(wevent.geo==69)Theta=wevent.Theta-13.9;
                            if(wevent.geo==70)Theta=wevent.Theta-12.1;
                          }
                          if(si==5){
                            if(wevent.geo==84)Theta=wevent.Theta-19.1;
                            if(wevent.geo==85)Theta=wevent.Theta-16.4;
                            if(wevent.geo==86)Theta=wevent.Theta-14.2;
                            if(wevent.geo==87)Theta=wevent.Theta-14.2;
                          }
                          wevent.Theta=Theta;//check

                            if(dproc.JET_EVENT_MODE==1 &&  total_event==-1){
                                int test1=0;
                                printf("new_data_set start!! all_event=%d\n",all_event);
                                getchar();
                            }
                            if(all_event==all_event_old)all_event++;
                            all_event_old=all_event;

// waveform check////////////
                            if(dproc.WAVE_OUT_MODE==1){
//TDC=25 check
                              //if(  ch_select==1 &&  wevent.t_time == 25){     
                              //if(  ch_select==1 &&  (wevent.t_time-tdc_geta)*dt > 55 && (wevent.t_time-tdc_geta)*dt < 65  
                              //    && wevent.Energy > 2.8 && wevent.Energy < 3.2){     
                              if(  ch_select==1 &&   (wevent.square3 > 900 && wevent.amp2 > 200)){  
//Fit check
                              //if(  ch_select==1 && g_ats[5] == 31 && wevent.tmax2<(68*2) && wevent.tmax2 > (9*2) &&     
                              //   (dproc.WAVE_FIT_MODE==0 || (dproc.WAVE_FIT_MODE==1 && FitResult[7]>=0 && FitResult[7]<1)) ){ //Fit check
                               // (dproc.WAVE_FIT_MODE==0 || (dproc.WAVE_FIT_MODE==1 &&  FitResult[7]>=10 )) ){ //Fit check

//test pluse check
                                //if(  ch_select==1 && wevent.t_time2<120 && wevent.t_time2>20 &&  
                                // (dproc.WAVE_FIT_MODE==0 || (dproc.WAVE_FIT_MODE==1 && FitResult[7]>0 && FitResult[7]<1)) ){ 

                                 if(wave_cont>300){
                                  //  printf("Waveform count=50\n");
                                    break;
                                  }
                                  wave_cont++;
                                  if(wave_cont%100==0 && wave_cont>0)printf("wave_cont=%d\n",wave_cont);

                                  int cut_frag=0;
                                  double cut_seppen;
                                  double cut_slope;
                                  cut_slope = wfm.ratio_All[wevent.geo-1];
                                  cut_seppen=wfm.seppen;
                                  if(cut_slope != -1){
                                    cut_frag=CutBadWf(cut_slope,wevent.geo, wevent.amp2,wevent.square3,cut_seppen);
                                  }else{
                                    cut_frag =1;
                                  }
                              //    if( cut_frag==1  ){
                                     tfileTestWave->cd();
                                     EventProcessWaveform(&wevent,cfginfo,wave, hist_num,mode,wave_cont);
                              //    }
                                }
                            }//if(dprooc.WAVE_OUT_MODE==1)END

                            if(wevent.geo==dproc.SELECT_CHAN){
                              if(wevent.region==31)tmp_i1++;
                              if(wevent.region!=31)tmp_i2++;
                            }


                           if(dproc.JET_EVENT_MODE==1){
                               if(process_frag>0){
                                  //printf("Output Event\n");
                                  tfileTestEvent->cd();
                                  InputVector(&wevent,&total_event,all_event);
                               }
                           }

                          int HighTofCut=0;
                          int iner;
                          double AveToF0;

                          iner=wevent.geo%16;
                          if(iner==0)iner=16;

                          wevent.InE=wevent.Energy;

                          if(si_typ==1 && wevent.Energy > 7.0)wevent.InE=-1;//HAMAMATSU
                          if(si_typ==0 && wevent.Energy > 7.3)wevent.InE=-1;//BNL
                          wevent.AveE=get_Energy(Theta);//mrad input ; MeV output
                          if( (wevent.ToF-tdc_geta*dt)>0 && (wevent.ToF-tdc_geta*dt)<40 )HighTofCut=1;      
                          if(iner > 6 && iner <= 16 && wevent.AveE>6.5){
                             AveToF0=get_ToF(wevent.AveE,wevent.geo);
                          }

                          if(wevent.geo>=17 && wevent.geo<=32 ){ //BNL
                              if( wevent.geo >=25){
                                   if(HighTofCut==1 ){                            
                                       wevent.InE=GetInE(wevent.Energy,wevent.geo);
                                    }
                              }
                          }
                          if(wevent.geo>=65 && wevent.geo<=80){ //BNL
                              if( wevent.geo >= 75){
                                   if(HighTofCut==1 ){                            
                                       wevent.InE=GetInE(wevent.Energy,wevent.geo);
                                    }
                              }
                          }
                          if(si_typ==1 &&  wevent.geo < 49 ){//HAMAMATSU
                              if( (wevent.geo >=9 && wevent.geo <=16) || wevent.geo >=41){
                                   if(HighTofCut==1 ){                            
                                       wevent.InE=GetInE(wevent.Energy,wevent.geo);
                                   }
                              }
                          }
                          if(si_typ==1 && wevent.geo > 48){//HAMAMATSU
                              if( (wevent.geo>=59 && wevent.geo<=64) || wevent.geo >= 91){
                                   if(HighTofCut==1 ){                            
                                       wevent.InE=GetInE(wevent.Energy,wevent.geo);
                                   }
                              }
                          }
                          wevent.InE=wevent.Energy;
                         

//Mx,Mp,ToF0,Theta0/////////////////////////////////////////
                          ToF0=get_ToF(wevent.InE,wevent.geo);
                          double DepE = wevent.Energy; 
                          int ff=0;
                          if( ToF0 - (wevent.ToF-tdc_geta*dt) < -12 ){
                              ff=1;
                              wevent.InE = 0.0006*pow(DepE,5)-0.0087*pow(DepE,4)+0.0431*pow(DepE,3)-0.0915*pow(DepE,2)+1.0523*DepE+0.1397;
     //                         printf("ToF0=%lf %lf DepE=%lf-->%lf\n",ToF0,wevent.ToF-tdc_geta*dt,DepE,wevent.InE);getchar();
                          }
                          Mx=GetMx(wevent.InE,Theta);
                          Mp=get_Mp(wevent.InE,wevent.ToF,tdc_geta*dt,ff);
                          Theta0=get_Theta(wevent.InE);
                          if(Mp>0 ){
                              //printf("Mp=%lf Mx=%lf amp=%d tdc=%d\n",Mp,Mx,wevent.amp,wevent.t_time);
                              //getchar();
                          }else{
                               Mx=-5;
                               Mp=-5;
                          }
                          wevent.Mp=Mp;
                          wevent.Mx=Mx;
                          wevent.ToF0=ToF0;
                          //wevent.ToF0=AveToF0;
                          wevent.Theta0=asin(Theta0)*1000;//mrad
                          dT_check=sqrt(pow(wevent.ToF0-(wevent.ToF-tdc_geta*dt),2));

//Target profile check
                       //if((wevent.t_time-tdc_geta)*1.19 >30 && (wevent.t_time-tdc_geta)*1.19 <50 
                       if(dT_check <8 
                            && wevent.Energy >1 && wevent.Energy <2){
                         int si=(wevent.geo-1)/(int)16;
                         int frag=0;

                         if(si==0 && wevent.geo>=13 && wevent.geo<=15 )frag=1;
                         if(si==1 && wevent.geo>=29 && wevent.geo<=31 )frag=1;
                         if(si==2 && wevent.geo>=45 && wevent.geo<=47 )frag=1;
                         if(si==3 && wevent.geo>=58 && wevent.geo<=60 )frag=1;
                         if(si==4 && wevent.geo>=74 && wevent.geo<=76 )frag=1;
                         if(si==5 && wevent.geo>=90 && wevent.geo<=92 )frag=1;

                         if(si==0 && wevent.geo>=5 && wevent.geo<=7 )frag=2;
                         if(si==1 && wevent.geo>=21 && wevent.geo<=23 )frag=2;
                         if(si==2 && wevent.geo>=37 && wevent.geo<=39 )frag=2;
                         if(si==3 && wevent.geo>=50 && wevent.geo<=52 )frag=2;
                         if(si==4 && wevent.geo>=66 && wevent.geo<=68 )frag=2; // was -- wevent.geo<=67
                         if(si==5 && wevent.geo>=83 && wevent.geo<=85 )frag=2;

                         if(frag==2)select_eventY[si]++;  // frag was switched 2 = Y, NOT 1
                         if(frag==1)select_eventB[si]++;  // 1 = B, NOT 2
                       }
//Ntuple//////////////////////////////////////////////////
                           if(dproc.RAW_EVENT_MODE==1){
                             if( wevent.region==31  && wevent.Energy > 0.5 ){
                                   tfileTestRaw->cd();
                                   EventProcessRootNtuple(&wevent,cfginfo, hist_num,mode,all_event);
                                   //printf("wevent.square3=%lf wevent.square2=%d\n",wevent.square3,wevent.square2);
                                   //getchar();
                             }
                           }
//8 elements//////////////////
                           GetBunchPat(&chan,&wevent);
                           if( dT_check < 8 ){
                           //if( dT_check > 12 ){//Am check
                           //if( (wevent.ToF-dt*tdc_geta)< 15 ){//prompt
                              int cut_frag=0;
                              double cut_seppen;
                              double cut_slope;
                              cut_slope = wfm.ratio_All[wevent.geo-1];
                              cut_seppen= wfm.seppen;
                              if(cut_slope !=-1){
                                cut_frag=CutBadWf(cut_slope,wevent.geo, wevent.amp2,wevent.square3,cut_seppen);
                              }else{
                                cut_frag=1;
                              }
                              //printf("ch=%d,E=%lf InE=%lf TOF=%lf\n",wevent.geo,wevent.Energy,wevent.InE,(wevent.ToF-dt*tdc_geta));
                              Get8Elements(&bin,&chan,&wevent,&elements);
                           } 
//CHECK HIST//////////////////////////////////////////////////
                           //printf("%lf %lf %d\n",wevent.Energy,wevent.ToF,wevent.region);getchar();
                           if(wevent.region ==31 ){
                             JetEventProcess(&TOff,&wfm,&bin,&chan,&wevent,cfginfo,tdc_geta,FitResult);//ONLINE HIST
                           }
//CHECK MISMATCH/////////////////////////////
                           if(pow(wevent.amp-wevent.amp2,2)>4){
                              amp_mismatch++;
                           }
                           if(pow(wevent.t_time-wevent.t_time2,2)>4){
                              tdc_mismatch++;
                           }
                           if(pow(wevent.tmax-wevent.tmax2,2)>4){
                              tmax_mismatch++;
                           }
/////////////////////////////////////////////////////
/*
//OUTPUT HBOOK
		            EventProcessPawNtuple(&wevent,cfginfo, hist_num,mode,all_event);
*/
// Event Processing Calibration
                            event.stN=wevent.strip_no;
                            event.amp = wevent.amp;
                            event.tdc = wevent.t_time;
                            event.intg = wevent.square;
                            event.geo = wevent.geo;//geo.ch
                            event.bid = wevent.bunch;       
                            event.rev = wevent.revolution;       
		            event.tmax = wevent.tmax;
                            event.Mp=wevent.Mp;
                            event.Mx=wevent.Mx;

                    
                            tfile->cd();
                            if (EventProcess(&event,cfginfo)!=0) {
                               cout << "Error event process Ev: "<<Nread<<endl;  
                            }
			}//if(hist_num>0  )END
			cnt1++;
		    }
		}
	    }
            }

            break; // later
        case REC_READAT:
//okadah//////////////////////
            count_at++;
            //getchar();
            //if(count_at%1000==0)
               printf("REC_READAT count_at=%d\n",count_at);
////////////////////////////////
            event.delim = rec.header.timestamp.delim;
            recordReadATStruct *ATPtr;
            
            for (int i=0; i< rec.header.len - sizeof(recordHeaderStruct);) {

                ATPtr = (recordReadATStruct *) (&rec.data.rec[i]);
                event.stN = ATPtr -> subhead.siNum; // si number
                Nevent = ATPtr -> subhead.Events + 1; 
                i += sizeof(subheadStruct) + Nevent*sizeof(ATStruct);
                if (i > rec.header.len - sizeof(recordHeaderStruct)) {
                    cout << "Broken record "<< rec.header.num << "(" 
                         << rec.header.len << " bytes). Last subhead: siNum= "
                         << event.stN << " Events= " << Nevent << endl; 
                    break;
                }

                // Si Number
                int ssss=(int)(event.stN+1); 

                 //if(ADC_cal[ssss]<5 || ssss==0){
                 //   printf("ADC_cal[%d]=%lf\n",ssss,ADC_cal[ssss]);
                    //getchar();
                 //}
              
                

                for (int j=0; j<Nevent; j++) {
                    Nread++;
		    all_event_AT++; //total_event reading

                    event.amp0  = ATPtr -> data[j].a;
                    event.amp=(int)(event.amp0*ADC_mean/ADC_cal[ssss]);
                    event.tdc  = ATPtr -> data[j].t; 
                    //event.intg = ATPtr -> data[j].s;
                    event.bid  = ATPtr -> data[j].b;

                    event.rev0 = ATPtr -> data[j].rev0; 
                    event.rev = ATPtr -> data[j].rev; 
                    
                    event.intg = geo_ch[ssss];
		    event.tmax = ATPtr->data[j].tmax;
                    
//Missing Mass and Recoiled Mass///////////////////
                    if(event.amp>0 && event.amp<255){
                        Mx=GetMx(event.amp,TOff.offset[6]);
                        Mp=get_Mp(event.amp,event.tdc,tdc_geta,event.intg);
                        if(Mp<0 ){
                           printf("Mp=%lf amp=%d tdc=%d\n",Mp,event.amp,event.tdc);
                           getchar();
                        }
                    }else{
                        Mx=-1;
                        Mp=-1;
                    }
                   event.Mp=Mp;
                   event.Mx=Mx;
                    //if ((fmod(Nread,Nskip)==0)&&
                    //    ((fillpat[event.bid]==1)||(dproc.CMODE==1))) {
                    if (dproc.CMODE==1){
                        //(dproc.CMODE==0)) {
                        // Event Processing
                        if (EventProcess(&event,cfginfo)!=0) {
                            cout << "Error event process Ev: "<<Nread<<endl;  
                        }
                    }

//OUTPUT ONLINE
                    wevent.amp=event.amp;
                    wevent.t_time=event.tdc;
                    wevent.dummy1=0;
                    wevent.square=event.intg;
                    wevent.bunch=event.bid;
                    wevent.dummy2=0;
                    wevent.tmax=event.tdc;//Not yet
                    wevent.intg=event.intg;
                    wevent.geo=event.geo;
                    wevent.revolution=0;
//TEMP TEMP
                    wevent.amp2=0;
                    wevent.t_time2=0;
                    wevent.square2=0;
                    wevent.tmax2=0;
                    wevent.amp3=0;
                    wevent.t_time3=0;
                    wevent.square3=0;
                    wevent.tmax3=0;
                    wevent.f_amp=0;
                    wevent.f_t_time=0;
                    wevent.f_square=0;
                    wevent.f_tmax=0;
                    wevent.f_ndf=0;
                    wevent.f_chi=0;
                    wevent.f_cndf=0;
                    wevent.f_start=0;
                    wevent.f_stop=0;
                    wevent.Mx=event.Mx;
                    wevent.Mp=event.Mp;
                    JetEventProcess(&TOff,&wfm,&bin,&chan,&wevent,cfginfo,tdc_geta,FitResult);//ONLINE HIST
	            //EventProcessPawNtuple(&wevent,cfginfo, hist_num,mode,all_event_AT);

                    
                    if (Nread%1000000==0) {
                        cout << " Ncounts = " << Nread <<endl;
                        fflush(stdout);
                    }
                }
            }
            break;
        case REC_RHIC_CONF:
/*
            sipar_.idiv = rec.cfg.data.CSR.split.iDiv;
	    sipar_.ifine = rec.cfg.data.CSR.split.Mod2D;
	    sipar_.emin = rec.cfg.data.Emin;
	    sipar_.nsperchan = rec.cfg.data.WFDTUnit;
	    k = 0;
	    for (i=0; i<rec.cfg.data.NumChannels; i++) {
		if (rec.cfg.data.chan[i].CamacN == 0) continue;
//printf("found channel %d at location %d %d\n", i+1, rec.cfg.data.chan[i].CamacN,rec.cfg.data.chan[i].VirtexN);
		sipar_.ecoef[i] = rec.cfg.data.chan[i].ecoef;
		sipar_.edead[i] = rec.cfg.data.chan[i].edead;
		sipar_.tmin[i] = 
		    rec.cfg.data.chan[i].Window.split.Beg * sipar_.nsperchan;
		j = i + 1;
		mybook_(&j);
		for (m=0; m<256; m++) {		// carbon lookups
		    data[m] = (rec.cfg.data.chan[i].LookUp[m] & 0xFF) * 0.5 * sipar_.nsperchan;
		    data[m+256] = ((rec.cfg.data.chan[i].LookUp[m] >>8) & 0xFF) * 0.5 * sipar_.nsperchan;
		}
		HPAKAD(800 + j, &data[0]);
		HPAKAD(900 + j, &data[256]);
		for (m=0; m<256; m++) {		// carbon lookups raw
		    data[m] = (rec.cfg.data.chan[i].LookUp[m] & 0xFF);
		    data[m+256] = ((rec.cfg.data.chan[i].LookUp[m] >>8) & 0xFF);
		}
		HPAKAD(1800 + j, &data[0]);
		HPAKAD(1900 + j, &data[256]);
		for (m=0; m<256; m++) {		// integral lookups raw
		    data[m] = (rec.cfg.data.chan[i].LookUp[m+256] & 0xFF) 
			* (1 << (sipar_.idiv+2));
		    data[m+256] = ((rec.cfg.data.chan[i].LookUp[m+256] >>8) & 0xFF)
			* (1 << (sipar_.idiv+2));
		}
		HPAKAD(2000 + j, &data[0]);
		HPAKAD(2100 + j, &data[256]);
		k++;
	    }
	    printf("Configuration OK : %d Si found.\n", k);
	    break;
*/
//okadah//////////////////////
            printf("REC_CONF\n");
	    //getchar();
////////////////////////////////
            fprintf(stdout,"Read configure information\n");
            cfginfo = (recordConfigRhicStruct *)
                malloc(sizeof(recordConfigRhicStruct)+
                       (rec.cfg.data.NumChannels-1)*sizeof(SiChanStruct));

            memcpy(cfginfo, &rec.cfg, sizeof(recordConfigRhicStruct)+
                   (rec.cfg.data.NumChannels-1)*sizeof(SiChanStruct));
            // when we mandatory provide cfg info 
            if (dproc.CFGMODE==1) {
                ReadConfig(cfginfo);
            }
            if (dproc.READT0MODE==1) {
                ReadT0Config(cfginfo);
            }
            // Display configuration
            if (PrintConfig(cfginfo)!=0) {
                perror(" error in printing configuration");
                exit(1);
            }
            if (dproc.MESSAGE == 1) exit(0);
            break;
	case REC_SCALERS:
            printf("REC_SCALERS\n");
	    memcpy(&scalers_, &rec.scal.data[0], sizeof(scalers_));	    
	    break;
	case REC_HJETSWITCH:
            hjet_frag=1;//Vector output frag
            hjet_count++;
            printf("REC_HJETSWITCH\n");
	    atraw_.ijet = rec.jet.data[1]/JET_PLUS;
	    printf("JETSWITCH: to %d at %d events  -----valance=%d  \n", 
               atraw_.ijet, cnt1,elements.valance_check);
            if( atraw_.ijet ==3)elements.zero_c++;
            if( atraw_.ijet ==1)elements.plus_c++;
            if( atraw_.ijet ==2)elements.minus_c++;
            if( atraw_.ijet ==3){
              elements.zero++;
              if(target_start_frag==0)target_start_frag=1;
            }
            if( elements.valance_check==0 && atraw_.ijet < 3){
               if(target_start_frag==1)elements.valance_check=1;
               target_start_frag=1;
            }
            if(elements.valance_check==1){
              if(atraw_.ijet ==1){
                elements.plus++;
                elements.plus_f=1;
              }
              if(atraw_.ijet ==2){
                elements.minus++;
                elements.minus_f=1;
              }
            }
            //if( atraw_.ijet==3 && elements.valance_check==1 && elements.plus_f ==1 && elements.minus_f==1){
            if(  elements.valance_check==1 && elements.plus_f ==1 && elements.minus_f==1){
               printf("copy and reset! plus=%d minus=%d\n",elements.plus,elements.minus);
            //   getchar();
               sum_each8Elements(&elements);
               reset_each8Elements(&elements);
               elements.valance_check=0;
               elements.plus_f=0;
               elements.minus_f=0;
            }
            //getchar();
	    break;
        default:    // unknown record
            //fprintf(stdout,"Encountered Unknown Record 0x%x\n",
            //        rec.header.type & REC_TYPEMASK);
            fprintf(stdout,".");
            break;
        }
    }
    Elements2Asym(&elements,&asym);

    printf("all_event=%d\n",all_event);
    printf("amp_mismatch=%d tdc_mismatch=%d tmax_mismatch=%d\n",amp_mismatch,tdc_mismatch,tmax_mismatch);
    fprintf(stdout,"End of loop\n");
   
//OUTPUT 8 elements
    int pickup=3;//1.8-2.2 MeV
    int total_selectY=0;
    int total_selectB=0;
    int LEFT_B[2][2],RIGHT_B[2][2];
    int BG_LEFT_B[2][2],BG_RIGHT_B[2][2];
    int LEFT_Y[2][2],RIGHT_Y[2][2];
    int BG_LEFT_Y[2][2],BG_RIGHT_Y[2][2];
    for(int i=0;i < 2;++i){
     for(int k=0;k<2;++k){
       LEFT_Y[i][k]=0;
       BG_LEFT_Y[i][k]=0;
       LEFT_B[i][k]=0;
       BG_LEFT_B[i][k]=0;
       RIGHT_Y[i][k]=0;
       BG_RIGHT_Y[i][k]=0;
       RIGHT_B[i][k]=0;
       BG_RIGHT_B[i][k]=0;
     }
   }
/////////////////valance
    for(raw_k=0;raw_k<3;++raw_k){
      for(raw_i=0;raw_i<2;++raw_i){
        for(raw_j=0;raw_j<2;++raw_j){
          for(int e_i=1;e_i < 6;++e_i){
            LEFT_Y[raw_i][raw_j]=LEFT_Y[raw_i][raw_j]+elements.valanceY[e_i][raw_k][raw_i][raw_j];
            BG_LEFT_Y[raw_i][raw_j]=BG_LEFT_Y[raw_i][raw_j]+elements.BGvalanceY[e_i][raw_k][raw_i][raw_j];
            RIGHT_Y[raw_i][raw_j]=RIGHT_Y[raw_i][raw_j]+elements.valanceY[e_i][raw_k+3][raw_i][raw_j];
            BG_RIGHT_Y[raw_i][raw_j]=BG_RIGHT_Y[raw_i][raw_j]+elements.BGvalanceY[e_i][raw_k+3][raw_i][raw_j];
            LEFT_B[raw_i][raw_j]=LEFT_B[raw_i][raw_j]+elements.valanceB[e_i][raw_k][raw_i][raw_j];
            BG_LEFT_B[raw_i][raw_j]=BG_LEFT_B[raw_i][raw_j]+elements.BGvalanceB[e_i][raw_k][raw_i][raw_j];
            RIGHT_B[raw_i][raw_j]=RIGHT_B[raw_i][raw_j]+elements.valanceB[e_i][raw_k+3][raw_i][raw_j];
            BG_RIGHT_B[raw_i][raw_j]=BG_RIGHT_B[raw_i][raw_j]+elements.BGvalanceB[e_i][raw_k+3][raw_i][raw_j];
          }
        }
     }
    }
    double ttt1,ttt2,rawasym,rawasym_err;
    ttt1 = sqrt(LEFT_Y[0][0]+LEFT_Y[1][0])*sqrt(RIGHT_Y[0][1]+RIGHT_Y[1][1]);
    ttt2 = sqrt(LEFT_Y[0][1]+LEFT_Y[1][1])*sqrt(RIGHT_Y[0][0]+RIGHT_Y[1][0]);
    rawasym = (ttt1-ttt2)/(double)(ttt1+ttt2); 
    rawasym_err=1/(double)sqrt(LEFT_Y[0][0]+LEFT_Y[1][0]+RIGHT_Y[0][1]+RIGHT_Y[1][1]);
    printf(" ------------------YELLOW 1-3 MeV  raw-TARGETasymmetry = %lf +/- %lf\n",rawasym,rawasym_err);
    printf("    LEFT(beam,target)  |    RIGHT(beam,target)\n");
    printf("    (+,+)(+,-)(-,+)(-,-)|   (+,+)(+,-)(-,+)(-,-)\n");
    for(raw_i=0;raw_i<2;++raw_i){
        for(raw_j=0;raw_j<2;++raw_j){
          printf("%d    ",LEFT_Y[raw_i][raw_j]);
        }
    }
    printf("| ");
    for(raw_i=0;raw_i<2;++raw_i){
        for(raw_j=0;raw_j<2;++raw_j){
          printf("%d    ",RIGHT_Y[raw_i][raw_j]);
        }
    }
    printf("\n");

    ttt1 = sqrt(BG_LEFT_Y[0][0]+BG_LEFT_Y[1][0])*sqrt(BG_RIGHT_Y[0][1]+BG_RIGHT_Y[1][1]);
    ttt2 = sqrt(BG_LEFT_Y[0][1]+BG_LEFT_Y[1][1])*sqrt(BG_RIGHT_Y[0][0]+BG_RIGHT_Y[1][0]);
    rawasym = (ttt1-ttt2)/(double)(ttt1+ttt2); 
    rawasym_err=1/(double)sqrt(BG_LEFT_Y[0][0]+BG_LEFT_Y[1][0]+BG_RIGHT_Y[0][1]+BG_RIGHT_Y[1][1]);
    printf(" ==================YELLOW WIDE 1-3 MeV raw-TARGETasymmetry = %lf +/- %lf\n",rawasym,rawasym_err);
    printf("    LEFT(beam,target)  |    RIGHT(beam,target)\n");
    printf("    (+,+)(+,-)(-,+)(-,-)|   (+,+)(+,-)(-,+)(-,-)\n");
    for(raw_i=0;raw_i<2;++raw_i){
        for(raw_j=0;raw_j<2;++raw_j){
          printf("%d    ",BG_LEFT_Y[raw_i][raw_j]);
        }
    }
    printf("| ");
    for(raw_i=0;raw_i<2;++raw_i){
        for(raw_j=0;raw_j<2;++raw_j){
          printf("%d    ",BG_RIGHT_Y[raw_i][raw_j]);
        }
    }
    printf("\n");
    ttt1 = sqrt(LEFT_B[0][0]+LEFT_B[1][0])*sqrt(RIGHT_B[0][1]+RIGHT_B[1][1]);
    ttt2 = sqrt(LEFT_B[0][1]+LEFT_B[1][1])*sqrt(RIGHT_B[0][0]+RIGHT_B[1][0]);
    rawasym = (ttt1-ttt2)/(double)(ttt1+ttt2); 
    rawasym_err=1/(double)sqrt(LEFT_B[0][0]+LEFT_B[1][0]+RIGHT_B[0][1]+RIGHT_B[1][1]);
    printf(" ------------------BLUE 1-3 MeV raw-TARGETasymmetry = %lf +/- %lf\n",rawasym,rawasym_err);
    printf("    LEFT(beam,target)  |    RIGHT(beam,target)\n");
    printf("    (+,+)(+,-)(-,+)(-,-)|   (+,+)(+,-)(-,+)(-,-)\n");
    for(raw_i=0;raw_i<2;++raw_i){
        for(raw_j=0;raw_j<2;++raw_j){
          printf("%d    ",LEFT_B[raw_i][raw_j]);
        }
    }
    printf("| ");
    for(raw_i=0;raw_i<2;++raw_i){
        for(raw_j=0;raw_j<2;++raw_j){
          printf("%d    ",RIGHT_B[raw_i][raw_j]);
        }
    }
    printf("\n");
    ttt1 = sqrt(BG_LEFT_B[0][0]+BG_LEFT_B[1][0])*sqrt(BG_RIGHT_B[0][1]+BG_RIGHT_B[1][1]);
    ttt2 = sqrt(BG_LEFT_B[0][1]+BG_LEFT_B[1][1])*sqrt(BG_RIGHT_B[0][0]+BG_RIGHT_B[1][0]);
    rawasym = (ttt1-ttt2)/(double)(ttt1+ttt2); 
    rawasym_err=1/(double)sqrt(BG_LEFT_B[0][0]+BG_LEFT_B[1][0]+BG_RIGHT_B[0][1]+BG_RIGHT_B[1][1]);
    printf(" ==================BLUE WIDE 1-3 MeV raw-TARGETasymmetry = %lf +/- %lf\n",rawasym,rawasym_err);
    printf("    LEFT(beam,target)  |    RIGHT(beam,target)\n");
    printf("    (+,+)(+,-)(-,+)(-,-)|   (+,+)(+,-)(-,+)(-,-)\n");
    for(raw_i=0;raw_i<2;++raw_i){
        for(raw_j=0;raw_j<2;++raw_j){
          printf("%d    ",BG_LEFT_B[raw_i][raw_j]);
        }
    }
    printf("| ");
    for(raw_i=0;raw_i<2;++raw_i){
        for(raw_j=0;raw_j<2;++raw_j){
          printf("%d    ",BG_RIGHT_B[raw_i][raw_j]);
        }
    }
    printf("\n");
///////

  int energy_bin=11;
  if(NULL == (out_count=fopen(oname,"w"))){
     printf("OPEN ERROR\n");
     getchar();
  }
///////3ch selection///////////////////// 
  for(i=0;i<energy_bin;++i){
   for(raw_k=0;raw_k<6;++raw_k){
    for(raw_i=0;raw_i<2;++raw_i){
      for(raw_j=0;raw_j<2;++raw_j){
          //fprintf(out_count,"%d,",elements.rawY[i][raw_k][raw_i][raw_j]);//Empty_target,BG_run
          fprintf(out_count,"%d,",elements.valanceY[i][raw_k][raw_i][raw_j]);
      }
    }
    fprintf(out_count,"\n");
   }
   fprintf(out_count,"\n");
  }
  fprintf(out_count,"---------------------------\n");
  for(i=0;i<energy_bin;++i){
   for(raw_k=0;raw_k<6;++raw_k){
    for(raw_i=0;raw_i<2;++raw_i){
      for(raw_j=0;raw_j<2;++raw_j){
          //fprintf(out_count,"%d,",elements.rawB[i][raw_k][raw_i][raw_j]);//Empty_target,BG_run
          fprintf(out_count,"%d,",elements.valanceB[i][raw_k][raw_i][raw_j]);
      }
    }
    fprintf(out_count,"\n");
   }
   fprintf(out_count,"\n");
  }
  fprintf(out_count,"------3ch_selection_END---------------------\n");
  fprintf(out_count,"------5ch_selection_START---------------------\n");
//5ch selection///////////////////////////////////////////// 
  for(i=0;i<energy_bin;++i){
   for(raw_k=0;raw_k<6;++raw_k){
    for(raw_i=0;raw_i<2;++raw_i){
      for(raw_j=0;raw_j<2;++raw_j){
          //fprintf(out_count,"%d,",elements.MMrawY[i][raw_k][raw_i][raw_j]);//Empty_target,BG_run
          fprintf(out_count,"%d,",elements.MMvalanceY[i][raw_k][raw_i][raw_j]);
      }
    }
    fprintf(out_count,"\n");
   }
   fprintf(out_count,"\n");
  }
  fprintf(out_count,"---------------------------\n");
  for(i=0;i<energy_bin;++i){
   for(raw_k=0;raw_k<6;++raw_k){
    for(raw_i=0;raw_i<2;++raw_i){
      for(raw_j=0;raw_j<2;++raw_j){
          //fprintf(out_count,"%d,",elements.MMrawB[i][raw_k][raw_i][raw_j]);//Empty_target,BG_run
          fprintf(out_count,"%d,",elements.MMvalanceB[i][raw_k][raw_i][raw_j]);
      }
    }
    fprintf(out_count,"\n");
   }
   fprintf(out_count,"\n");
  }
  fprintf(out_count,"----------5ch_selection_END-----------------\n");
  fprintf(out_count,"----------8ch_selection_START-----------------\n");
//8ch selection////////////////////////////////////// 
  for(i=0;i<energy_bin;++i){
   for(raw_k=0;raw_k<6;++raw_k){
    for(raw_i=0;raw_i<2;++raw_i){
      for(raw_j=0;raw_j<2;++raw_j){
          //fprintf(out_count,"%d,",elements.BGrawY[i][raw_k][raw_i][raw_j]);//Empty_target,BG_run
          fprintf(out_count,"%d,",elements.BGvalanceY[i][raw_k][raw_i][raw_j]);
      }
    }
    fprintf(out_count,"\n");
   }
   fprintf(out_count,"\n");
  }
  fprintf(out_count,"---------------------------\n");
  for(i=0;i<energy_bin;++i){
   for(raw_k=0;raw_k<6;++raw_k){
    for(raw_i=0;raw_i<2;++raw_i){
      for(raw_j=0;raw_j<2;++raw_j){
          //fprintf(out_count,"%d,",elements.BGrawB[i][raw_k][raw_i][raw_j]);//Empty_target,BG_run
          fprintf(out_count,"%d,",elements.BGvalanceB[i][raw_k][raw_i][raw_j]);
      }
    }
    fprintf(out_count,"\n");
   }
   fprintf(out_count,"\n");
  }
  fprintf(out_count,"-----TARGET SUMMARY------------------\n");
  fprintf(out_count,"raw_plus,raw_minus,raw_zero,v_plus,v_valance,v_zero\n");
  fprintf(out_count,"%d,%d,%d,%d,%d,%d\n",elements.plus_c,elements.minus_c,elements.zero_c,elements.plus,elements.minus,elements.zero);
  fclose(out_count);

  if(dproc.WAVE_OUT_MODE==1){
       printf("wave_cont total=%d\n",wave_cont);
  }
//  printf("intg_check=%d\n",intg_check);
//  printf("tmp_i1=%d tmp_i2=%d\n",tmp_i1,tmp_i2);
  
 /* 
  printf("=================FIT================\n");
  printf("fit_event=%d fir_err=%d\n",fit_event,fit_err);
  for(int k=0;k<16;++k){
   printf("ch[%d]=%d ch[%d]=%d ch[%d]=%d ch[%d]=%d ch[%d]=%d ch[%d]=%d\n",k+1,evt_chan[k],k+1+16,
     evt_chan[k+16],k+1+32,evt_chan[k+32],k+1+48,evt_chan[k+48],k+1+64,evt_chan[k+64],k+1+80,evt_chan[k+80]);
  } 
*/
  printf("=====TARGET SUMMARY=========\n");
  printf("zero %d plus=%d minus=%d\n",elements.zero,elements.plus,elements.minus);
  printf("hjet_conunt=%d\n",hjet_count);
  printf("%d,%d,%d,%d,%d,%d\n",elements.plus_c,elements.minus_c,elements.zero_c,elements.plus,elements.minus,elements.zero);
/*
  printf("=====SELECT EVENT 1-2MeV && delta_tof <8 ns=========\n");
  printf("  Si1 select_event Yellow %d Blue %d\n",select_eventY[0],select_eventB[0]);
  printf("  Si2 select_event Yellow %d Blue %d\n",select_eventY[1],select_eventB[1]);
  printf("  Si3 select_event Yellow %d Blue %d\n",select_eventY[2],select_eventB[2]);
  printf("  Si4 select_event Yellow %d Blue %d\n",select_eventY[3],select_eventB[3]);
  printf("  Si5 select_event Yellow %d Blue %d\n",select_eventY[4],select_eventB[4]);
  printf("  Si6 select_event Yellow %d Blue %d\n",select_eventY[5],select_eventB[5]);
  printf("  Total select_event Yellow %d\n",select_eventY[0]+select_eventY[1]+select_eventY[2]+select_eventY[3]+select_eventY[4]+select_eventY[5]);
  printf("  Total select_event Blue %d\n",select_eventB[0]+select_eventB[1]+select_eventB[2]+select_eventB[3]+select_eventB[4]+select_eventB[5]);
*/
    return(0);
}

// ===================================
// ROOT file initializing
// ===================================
// hbunch[si]           1d : bunch dist
// hwcm                 1d : wall current monitor
// hdelim               1d : delimeter dist
// hstall[si]           1d : strip dist all
// hstcbn[si]           1d : strip dist carbon
// heneall[si]          1d : energy spectrum all
// henecbn[si]          1d : energy spectrum carbon
// henestpl[st]         1d : energy spectrum carbon plus spin 
// henestng[st]         1d : energy spectrum carbon minus spin 
// henetotal            1d : cross section (for A_N)
// hcalib[st]           1d : 241 Am calibration
// ht0[st]              2d : for T0 estimation 
// hia[st]              2d : Integral vs Amplitude
// hti[st]              2d : Tof vs. Integral 
// htofenepos[st]       2d : Tof vs. Energy (+)
// htofeneneg[st]       2d : Tof vs. Energy (-)
// htofdep[st]          2d : Tof vs. Deposit Energy
// htdcadc[st]          2d : TDC vs. ADC
// hlowedg[st]          1d : Lower Cut 
// huppedg[st]          1d : Upper Cut 
// hmassall[st]         1d : Mass all (600-1000keV)
// hmasscbn[st]         1d : Mass Carbon (600-1000keV)
// hmassenea[st]        2d : Mass vs. Energy (amp)
// hmassenei[st]        2d : Mass vs. Energy (int)
// hramp[ibin]          2d : 
// htshift[bid]         1d : Banana 600-650 keV
 
int TAsym::RootInit(char *filename,int mode,char *nameRAW,char *nameEvent,char *nameWave){
    int hid,i,bid;
    char hcomment[256];
    char histname[256];
    int chan_no=96;
    int si,st;

  

    tfile = new TFile(filename, "RECREATE");

    //tfile->mkdir("OUT_HIST");
    //tfile->mkdir("TestNtuple");
    //tfile->cd("OUT_HIST");

    // Bunch Distribution
    sprintf(hcomment,"bunch distribution YELLOW all 8 channels ,all 6 Si");
    sprintf(histname,"hbunchY_all");
    hbunchY_all = new TH1F(histname,hcomment,150, -0.5, 149.5);
    sprintf(hcomment,"bunch distribution BLUE  all 8 channels ,all 6 Sis");
    sprintf(histname,"hbunchB_all");
    hbunchB_all = new TH1F(histname,hcomment,150, -0.5, 149.5);

    for (si=0; si<6;si++ ) {
        sprintf(hcomment,"bunch distribution YELLOW %d ",si+1);
        sprintf(histname,"hbunchY_%d",si);
        hbunchY[si] = new TH1F(histname,hcomment,150, -0.5, 149.5);

        sprintf(hcomment,"bunch distribution BLUE %d ",si+1);
        sprintf(histname,"hbunchB_%d",si);
        hbunchB[si] = new TH1F(histname,hcomment,150, -0.5, 149.5);

        sprintf(hcomment,"our bunch distribution YELLOW %d ",si+1);
        sprintf(histname,"hbunchY2_%d",si);
        hbunchY2[si] = new TH1F(histname,hcomment,150, -0.5, 149.5);

        sprintf(hcomment,"our bunch distribution BLUE %d ",si+1);
        sprintf(histname,"hbunchB2_%d",si);
        hbunchB2[si] = new TH1F(histname,hcomment,150, -0.5, 149.5);
    }

    // WCM
    hwcm = new TH1F("hwcm","Wall Current Monitor", 150, -0.5, 149.5);
    
    // Calibration hists
    if (dproc.SELECT_CHAN==-1){
        for (int st=0;st<chan_no;st++){
            sprintf(hcomment,"ADC spectrum - strip %d ",st+1);
            sprintf(histname,"hcalib_%d",st);
            hcalib[st] = new TH1F(histname, hcomment,256, 0.0, 256.);
        }
        for (int st=0;st<chan_no;st++){
            sprintf(hcomment,"INTG spectrum - strip %d ",st+1);
            sprintf(histname,"hcalib_itg_%d",st);
            hcalib_intg[st] = new TH1F(histname, hcomment,1024, 0.0, 1024.);
        }
        for (int st=0;st<chan_no;st++){
            sprintf(hcomment,"TDC ch=%d",st);
            sprintf(histname,"TDC_ch%d",st);
            tdc_ch[st]=new TH1F(histname,hcomment,256,0,256);
        }

    }

    // Ntuple booking
    Event = new TTree("Event","Event mode data");
    Event -> Branch("amp", &atdata.amp, "amp/I");
    Event -> Branch("itg", &atdata.itg, "itg/I");
    Event -> Branch("tdc", &atdata.tdc, "tdc/I");
    Event -> Branch("tmax", &atdata.tmax, "tmax/I");
    Event -> Branch("bid", &atdata.bid, "bid/I");
    Event -> Branch("dlm", &atdata.dlm, "dlm/I");
    Event -> Branch("stp", &atdata.stp, "stp/I");
    Event -> Branch("ene", &atdata.ene, "ene/F");
    Event -> Branch("tof", &atdata.tof, "tof/F");
    Event -> Branch("spn", &atdata.spn, "spn/I");
    Event -> Branch("rev", &atdata.rev, "rev/I");
    Event -> Branch("rev0", &atdata.rev0, "rev0/I");
    Event -> Branch("Mx", &atdata.Mx, "Mx/D");
    Event -> Branch("Mp", &atdata.Mp, "Mp/D");

//okadah///////////////////////////////////
// Hists
    double M_min=-1;
    double M_max=8;
    double M_min2=0;
    double M_max2=2;
    int dhMp=90;
    int dhMx=100;
if(dproc.SELECT_CHAN>=0){
//T0 check
    sprintf(hcomment,"TDC ");
    sprintf(histname,"hist_TDC");
    hist_TDC=new TH1F(histname,hcomment,256,0,256);

    sprintf(hcomment,"TDC G");
    sprintf(histname,"TDC_G");
    TDC_G=new TH1F(histname,hcomment,256,0,256);
//TMAX
    sprintf(hcomment,"TMAX");
    sprintf(histname,"hist_TMAX");
    hist_TMAX= new TH1F(histname,hcomment,256,0.0,256.);
//ADC for comparing Fit
    sprintf(hcomment,"selected_ADC for Fit result");
    sprintf(histname,"hist_ADC");
    hist_ADC= new TH1F(histname,hcomment,256,0.0,256.);
        
//INTG VS ADC
    sprintf(hcomment,"INTG type2");
    sprintf(histname,"INTG3");
    INTG3= new TH1F(histname,hcomment,1024,0.0,1024.);

    sprintf(hcomment,"INTG type2 region");
    sprintf(histname,"region_INTG3");
    region_INTG3= new TH1F(histname,hcomment,200,0.0,200.);

}

if(dproc.SELECT_CHAN>0){
//OUTPUT Fitting
//Fitting check TDC vs Fit-TDC 
   sprintf(hcomment,"FIT t_time ch=%d\n",dproc.SELECT_CHAN);
   sprintf(histname,"hist_ft_time");
   hist_ft_time=new TH1F(histname,hcomment,256,0,256);
   TAxis *axis_x1=hist_ft_time->GetXaxis();axis_x1->SetTitle("fitting_TDC"); 

   sprintf(hcomment,"FIT t_time  vs TDC ch=%d\n",dproc.SELECT_CHAN);
   sprintf(histname,"hist_ft_TDC");
   hist_ft_TDC=new TH2F(histname,hcomment,256,0.,256.,256,0.,256.); 
   TAxis *hist_ft_TDC_x=hist_ft_TDC->GetXaxis();hist_ft_TDC_x->SetTitle("Fit-TDC"); 
   TAxis *hist_ft_TDC_y=hist_ft_TDC->GetYaxis();hist_ft_TDC_y->SetTitle("TDC"); 

//Fitting check TMAX vs Fit-TMAX
  sprintf(hcomment,"FIT tmax ch=%d\n",dproc.SELECT_CHAN);
  sprintf(histname,"hist_ftmax");
  hist_ftmax=new TH1F(histname,hcomment,256,0,256);

  sprintf(hcomment,"FIT tmax  vs TMAX ch=%d\n",dproc.SELECT_CHAN);
  sprintf(histname,"hist_ftmax_TMAX");
  hist_ftmax_TMAX=new TH2F(histname,hcomment,256,0.,256.,256,0.,256.);
   TAxis *hist_ftmax_TMAX_x=hist_ftmax_TMAX->GetXaxis();hist_ftmax_TMAX_x->SetTitle("Fit-TMAX"); 
   TAxis *hist_ftmax_TMAX_y=hist_ftmax_TMAX->GetYaxis();hist_ftmax_TMAX_y->SetTitle("TMAX"); 

//Fitting check ADC vs Fit-ADC
   sprintf(hcomment,"FIT Pmax ch=%d\n",dproc.SELECT_CHAN);
   sprintf(histname,"hist_Pmax");
   hist_Pmax=new TH1F(histname,hcomment,256,0.,256.);

   sprintf(hcomment,"FIT Pmax vs ADC ch=%d\n",dproc.SELECT_CHAN);
   sprintf(histname,"hist_Pmaxh");
   hist_Pmaxh=new TH2F(histname,hcomment,256,0.,256.,256,0.,256.);
   TAxis *hist_Pmaxh_x=hist_Pmaxh->GetXaxis();hist_Pmaxh_x->SetTitle("Fit-ADC"); 
   TAxis *hist_Pmaxh_y=hist_Pmaxh->GetYaxis();hist_Pmaxh_y->SetTitle("ADC"); 

   sprintf(hcomment,"FIT Pmax-ADC ch=%d\n",dproc.SELECT_CHAN);
   sprintf(histname,"hist_dh");
   hist_dh=new TH1F(histname,hcomment,60,-15.,15.);


//Fitting check INTEG vs Fit-INTG
   sprintf(hcomment,"FIT Area ch=%d\n",dproc.SELECT_CHAN);
   sprintf(histname,"hist_Area");
   hist_Area=new TH1F(histname,hcomment,1024,0.,1024.);

   sprintf(hcomment,"FIT Area vs INTG ch=%d\n",dproc.SELECT_CHAN);
   sprintf(histname,"C_INTG_on");
   C_INTG_on=new TH2F(histname,hcomment,1024,0.,1024.,1024,0.,1024.);
   TAxis *C_INTG_on_x=C_INTG_on->GetXaxis();C_INTG_on_x->SetTitle("Fit-Intg"); 
   TAxis *C_INTG_on_y=C_INTG_on->GetYaxis();C_INTG_on_y->SetTitle("INTG"); 


   sprintf(hcomment,"FIT Pmax-ADC vs Area-INTG ch=%d\n",dproc.SELECT_CHAN);
   sprintf(histname,"hist_dd");
   hist_dd=new TH2F(histname,hcomment,100,-100.,100.,60,-15,15.);

   sprintf(hcomment,"FIT Area-Intg ch=%d\n",dproc.SELECT_CHAN);
   sprintf(histname,"hist_dArea");
   hist_dArea=new TH1F(histname,hcomment,100,-100.,100.);

   sprintf(hcomment,"FIT Area-INTG ch=%d\n",dproc.SELECT_CHAN);
   sprintf(histname,"hist_Areah");
   hist_Areah=new TH2F(histname,hcomment,1024,0.,1024.,1024,0.,1024.);


//Fitting PARAMETER
   sprintf(hcomment,"FIT tau ch=%d\n",dproc.SELECT_CHAN);
   sprintf(histname,"hist_tau");
   hist_tau=new TH1F(histname,hcomment,50,0.,10.);

   sprintf(hcomment,"FIT tau vs ADC ch=%d\n",dproc.SELECT_CHAN);
   sprintf(histname,"hist_tauh");
   hist_tauh=new TH2F(histname,hcomment,50,0.,10.,200,0.,200.);

   sprintf(hcomment,"FIT n ch=%d\n",dproc.SELECT_CHAN);
   sprintf(histname,"hist_n");
   hist_n=new TH1F(histname,hcomment,50,0.,10.);

   sprintf(hcomment,"FIT n vs ADC ch=%d\n",dproc.SELECT_CHAN);
   sprintf(histname,"hist_nh");
   hist_nh=new TH2F(histname,hcomment,50,0.,10.,200,0.,200.);

   sprintf(hcomment,"FIT ndf ch=%d\n",dproc.SELECT_CHAN);
   sprintf(histname,"hist_ndf");
   hist_ndf=new TH1F(histname,hcomment,40,0.,40.);

   sprintf(hcomment,"FIT ndf vs g_ats[5] ch=%d\n",dproc.SELECT_CHAN);
   sprintf(histname,"hist_ndf_ats");
   hist_ndf_ats=new TH2F(histname,hcomment,40,0.,40.,40,0.,40.);

   sprintf(hcomment,"FIT chi ch=%d\n",dproc.SELECT_CHAN);
   sprintf(histname,"hist_chi");
   hist_chi=new TH1F(histname,hcomment,510,-10.,500.);

   sprintf(hcomment,"FIT cndf ch=%d\n",dproc.SELECT_CHAN);
   sprintf(histname,"hist_cndf");
   hist_cndf=new TH1F(histname,hcomment,60,0.,30.);

   sprintf(hcomment,"FIT cndf vs Pmax ch=%d\n",dproc.SELECT_CHAN);
   sprintf(histname,"hist_cndfh");
   hist_cndfh=new TH2F(histname,hcomment,60,0.,30.,1024,0.,1024.);
}

if(dproc.SELECT_CHAN==-1){ 
// check Waveform-INTG3 VS Waveform-ADC
 for (st=0;st<chan_no;st++){
     sprintf(hcomment,"waveform-ADC vs. INTG3  ch=%d",st+1);
     sprintf(histname,"ADC_INTG3_ch%d",st);
     ADC_INTG3[st]=new TH2F(histname,hcomment,256,0,256,1024,0,1024);

     sprintf(hcomment,"cndf>=10 waveform-ADC vs. INTG3  ch=%d",st+1);
     sprintf(histname,"ADC_INTG3_bad_ch%d",st);
     ADC_INTG3_bad[st]=new TH2F(histname,hcomment,256,0,256,1024,0,1024);

     sprintf(hcomment,"cndf>=5  && cndf < 10 waveform-ADC vs. INTG3  ch=%d",st+1);
     sprintf(histname,"ADC_INTG3_bad2_ch%d",st);
     ADC_INTG3_bad2[st]=new TH2F(histname,hcomment,256,0,256,1024,0,1024);


    }

//Fitting check Fit-Area VS Fit-ADC
    for (st=0;st<chan_no;st++){
     sprintf(hcomment,"FIT-Pmax vs. Area  ch=%d",st+1);
     sprintf(histname,"Pmax_Area_ch%d",st);
     Pmax_Area[st]=new TH2F(histname,hcomment,256,0,256,1024,0,1024);
    }

    if(dproc.WAVE_FIT_MODE==1){
//Fitting PARAMETER--ALL 96ch--------------------
      for (st=0;st<chan_no;st++){
        sprintf(hcomment,"FIT tau ch=%d\n",st);
        sprintf(histname,"hist_tau_%d",st);
        hist_tau_ch[st]=new TH1F(histname,hcomment,50,0.,10.);
      }

      for (st=0;st<chan_no;st++){
        sprintf(hcomment,"FIT tau vs ADC ch=%d\n",st);
        sprintf(histname,"hist_tauh_%d",st);
        hist_tauh_ch[st]=new TH2F(histname,hcomment,50,0.,10.,200,0.,200.);
      }

      for (st=0;st<chan_no;st++){
        sprintf(hcomment,"FIT n ch=%d\n",st);
        sprintf(histname,"hist_n_%d",st);
        hist_n_ch[st]=new TH1F(histname,hcomment,50,0.,10.);
      }

      for (st=0;st<chan_no;st++){
        sprintf(hcomment,"FIT n vs ADC ch=%d\n",st);
        sprintf(histname,"hist_nh_%d",st);
        hist_nh_ch[st]=new TH2F(histname,hcomment,50,0.,10.,200,0.,200.);
      }

      for (st=0;st<chan_no;st++){
        sprintf(hcomment,"FIT chi ch=%d\n",st);
        sprintf(histname,"hist_chi_%d",st);
        hist_chi_ch[st]=new TH1F(histname,hcomment,510,-10.,500.);
      }

      for (st=0;st<chan_no;st++){
        sprintf(hcomment,"FIT cndf ch=%d\n",st);
        sprintf(histname,"hist_cndf_%d",st);
        hist_cndf_ch[st]=new TH1F(histname,hcomment,100,0.,50.);
      }

      for (st=0;st<chan_no;st++){
        sprintf(hcomment,"FIT cndf vs Pmax ch=%d\n",st);
        sprintf(histname,"hist_cndfh_%d");
        hist_cndfh_ch[st]=new TH2F(histname,hcomment,100,0.,50.,50,0.,10.);
      }
   }//END if(dproc.WAVE_FIT_MODE==1)

}

if(dproc.SELECT_CHAN>=0){
//Fitting BANANA
   sprintf(hcomment,"FIT banana ch=%d\n",dproc.SELECT_CHAN);
   sprintf(histname,"hist_FitBanana");
   hist_FitBanana=new TH2F(histname,hcomment,256,0,256,256,0,256);

}

if(dproc.SELECT_CHAN==-1){
//BANANA every_ch
    for (st=0;st<chan_no;st++){
       si=(int)(st/16);//0-5
       sprintf(hcomment,"Si=%d ch=%d ",si,st+1);
       sprintf(histname,"banana_ch%d",st);
       //banana_ch[st] = new TH2F(histname, hcomment,100, 0.0, 10.,150,-50.0,100.);
       //banana_ch[st] = new TH2F(histname, hcomment,1024, 0.0, 1024.,256,0.0,256.);//INTG vs. t_time
       banana_ch[st] = new TH2F(histname, hcomment,256, 0.0, 256.,256,0.0,256.);//amp vs.t_time 
    }
//BANANA every_Si
    for (si=0;si<6;si++){
       sprintf(hcomment,"Si=%d all_chan ",si+1);
       sprintf(histname,"bananaSi_%d",si);
       //banana_si[si] = new TH2F(histname, hcomment,200, 0.0, 20.,150,-50,100.);
       banana_si[si] = new TH2F(histname, hcomment,256, 0, 256,256,0,256.);//amp vs. t_time
       if(dproc.WAVE_FIT_MODE==1){
         sprintf(hcomment,"Si=%d all_chan ",si+1);
         sprintf(histname,"bananaFITSi_%d",si);
         bananaFIT_si[si] = new TH2F(histname, hcomment,256, 0.0, 256.,256,0.0,256.);
       }

       sprintf(hcomment,"Si=%d all_chan ",si+1);
       sprintf(histname,"bananaINTG_Si_%d",si);
       banana_INTG_si[si] = new TH2F(histname, hcomment,1024, 0.0, 1024.,256,0.0,256.);
    }

//BANANA E vs TOF every_ch
    for (st=0;st<chan_no;st++){
       si=(int)(st/16);//0-5
       sprintf(hcomment,"Si=%d ch=%d ",si,st+1);
       sprintf(histname,"banana2_ch%d",st);
       banana2_ch[st] = new TH2F(histname, hcomment,100, 0.0, 10.,150,-30.0,120.);
    }
//BANANA E vs TOF every_Si
    for ( si=0;si<6;si++){
       sprintf(hcomment,"Si=%d all_chan ",si+1);
       sprintf(histname,"banana2Si_%d",si);
       banana2_si[si] = new TH2F(histname, hcomment,100, 0.0, 10.,150,-30.0,120.);
       sprintf(histname,"banana3Si_%d",si);
       banana3_si[si] = new TH2F(histname, hcomment,100, 0.0, 10.,150,-30.0,120.);
    }

//E vs position every_Si
   for (si=0;si<6;si++){
       sprintf(hcomment,"ADC vs position Si=%d all_chan",si+1);
       sprintf(histname,"Ex_%d",si);
       Ex_si[si] = new TH2F(histname,hcomment,18,0.,18.,70,0.0,7.);
       //Ex_si[si] = new TH2F(histname,hcomment,18,0.,18.,256,0,256);
   }
//InE vs position every_Si
   for (si=0;si<6;si++){
       sprintf(hcomment,"InE vs position Si=%d all_chan",si+1);
       sprintf(histname,"InEx_%d",si);
       InEx_si[si] = new TH2F(histname,hcomment,18,0.,18.,200,0.0,20.);
   }

//AveE vs position every_Si
   for (si=0;si<6;si++){
       sprintf(hcomment,"AveE vs position Si=%d all_chan",si+1);
       sprintf(histname,"AveEx_%d",si);
       AveEx_si[si] = new TH2F(histname,hcomment,18,0.,18.,200,0.0,20.);
   }

//Check Select
    sprintf(hcomment,"select_chan vs Energy ");
    sprintf(histname,"check_select");
    Check_select = new TH2F(histname, hcomment,97,0.,97.,50,0.,10.);
    sprintf(hcomment,"Non select_chan vs Energy ");
    sprintf(histname,"check_Nonselect");
    Check_Nonselect = new TH2F(histname, hcomment,97,0.,97.,50,0.,10.);


//TOF vs position every_Si
   for (si=0;si<6;si++){
       sprintf(hcomment,"TOF vs position Si=%d all_chan",si+1);
       sprintf(histname,"TOFx_%d",si);
       TOFx_si[si] = new TH2F(histname,hcomment,18,0.,18.,160,40.,200.);
   }
//BUNCH every_Si
    for (si=0;si<6;si++){
       sprintf(hcomment,"Si=%d ",si);
       sprintf(histname,"bunch_si%d",si);
       bunch_si[si] = new TH1F(histname, hcomment,131,-5., 125.);
    }

//Mp every_Si
    for (si=0;si<6;si++){
       sprintf(hcomment,"Si=%d ",si);
       sprintf(histname,"Mp_si%d",si);
       Mp_si[si] = new TH1F(histname, hcomment,(int)dhMp, M_min, M_max);
    }
//Mx every_Si
    for (si=0;si<6;si++){
       sprintf(hcomment,"Si=%d ",si);
       sprintf(histname,"Mx_si%d",si);
       Mx_si[si] = new TH1F(histname, hcomment,(int)dhMx, M_min2, M_max2);
    }

//Mp every_ch
    for (st=0;st<chan_no;st++){
       si=(int)(st/16);
       sprintf(hcomment,"Si=%d ch=%d ",si,st+1);
       sprintf(histname,"Mp_ch%d",st);
       Mp_ch[st] = new TH1F(histname, hcomment,(int)dhMp, M_min, M_max);
    }
//Mx every_ch
    for (st=0;st<chan_no;st++){
       si=(int)(st/16);
       sprintf(hcomment,"Si=%d ch=%d ",si,st+1);
       sprintf(histname,"Mx_ch%d",st);
       Mx_ch[st] = new TH1F(histname, hcomment,(int)dhMx, M_min2, M_max2);
    }
//E in the cut every_ch-->Willy
    for (st=0;st<chan_no;st++){
       si=(int)(st/16);
       sprintf(hcomment,"Si=%d ch=%d ",si,st+1);
       sprintf(histname,"E_ch%d",st);
       E_ch[st] = new TH1F(histname, hcomment,100, 0., 10.);
       //E_ch[st] = new TH1F(histname, hcomment,1024, 0., 1024.);//default 1206

       sprintf(hcomment,"Si=%d ch=%d ",si,st+1);
       sprintf(histname,"InE_ch%d",st);
       InE_ch[st] = new TH1F(histname, hcomment,200, 0., 20.);

       sprintf(hcomment,"Si=%d ch=%d ",si,st+1);
       sprintf(histname,"InEE_ch%d",st);
       InEE_ch[st] = new TH2F(histname, hcomment,100, 0., 20.,100,0.,10.);
/*
       sprintf(hcomment,"tubu Si=%d ch=%d ",si,st+1);
       sprintf(histname,"tubuE_ch%d",st);
       tubuE_ch[st] = new TH1F(histname, hcomment,100, 0., 10.);

       sprintf(hcomment,"tubd Si=%d ch=%d ",si,st+1);
       sprintf(histname,"tubdE_ch%d",st);
       tubdE_ch[st] = new TH1F(histname, hcomment,100, 0., 10.);

       sprintf(hcomment,"tdbu Si=%d ch=%d ",si,st+1);
       sprintf(histname,"tdbuE_ch%d",st);
       tdbuE_ch[st] = new TH1F(histname, hcomment,100, 0., 10.);

       sprintf(hcomment,"tdbd Si=%d ch=%d ",si,st+1);
       sprintf(histname,"tdbdE_ch%d",st);
       tdbdE_ch[st] = new TH1F(histname, hcomment,100, 0., 10.);
*/
    }
//ToF in the E cut every_ch
    for (si=0;si<6;si++){
       sprintf(hcomment,"Si=%d ",si);
       sprintf(histname,"ToF_si%d",si);
       ToF_si[si] = new TH1F(histname, hcomment,150, -10., 140.);
       //ToF_ch[st] = new TH1F(histname, hcomment,256, 0., 256.);

       sprintf(histname,"SeToF_si%d",si);
       SeToF_si[si] = new TH1F(histname, hcomment,150, -10., 140.);
    }
//ToF in the cut every_ch
    for (st=0;st<chan_no;st++){
       si=(int)(st/16);
       sprintf(hcomment,"Si=%d ch=%d ",si,st+1);
       sprintf(histname,"ToF_ch%d",st);
       ToF_ch[st] = new TH1F(histname, hcomment,150, -10., 140.);
       //ToF_ch[st] = new TH1F(histname, hcomment,256, 0., 256.);
    }
    
//ToF0 in cut every_ch
    for (st=0;st<96;st++){
       si=(int)(st/16);
       sprintf(hcomment,"Si=%d ch=%d ",si,st+1);
       sprintf(histname,"ToF0_ch%d",st);
       //ToF0_ch[st] = new TH1F(histname, hcomment,150, -10., 140.);
       ToF0_ch[st] = new TH1F(histname, hcomment,80, -20., 20.);
    }

//Theta0 in cut everySi_
    for (si=0;si<6;si++){
       sprintf(hcomment,"Si=%d ",si);
       sprintf(histname,"Theta0_si%d",si);
       Theta0_si[si] = new TH2F(histname,hcomment,160,-10.,150.,200,0.,20);
    }

//Theta0 in cut everych_
    for (st=0;st<96;st++){
       si=(int)(st/16);
       sprintf(hcomment,"Si=%d ch=%d ",si,st+1);
       sprintf(histname,"Theta0_ch%d",st);
       Theta0_ch[st] = new TH1F(histname,hcomment,160,-10.,150.);
    }
//delta_Theta0 in cut everych_
    for (st=0;st<96;st++){
       si=(int)(st/16);
       sprintf(hcomment,"Si=%d ch=%d ",si,st+1);
       sprintf(histname,"d_Theta0_ch%d",st);
       d_Theta0_ch[st] = new TH1F(histname,hcomment,50,-25.,25.);
    }
//Mp everySi typ1 MeV
    for (si=0;si<6;si++){
       sprintf(hcomment,"Si=%d 0.7-1.0MeV ",si+1);
       sprintf(histname,"MpSi_%d_0",si);
       Mp_si_0[si] = new TH1F(histname, hcomment,(int)dhMp, M_min, M_max);
//Mp everySi typ2 MeV
       sprintf(hcomment,"Si=%d 1.0-1.4MeV ",si+1);
       sprintf(histname,"MpSi_%d_1",si);
       Mp_si_1[si] = new TH1F(histname, hcomment,(int)dhMp, M_min, M_max);
//Mp everySi typ3 MeV
       sprintf(hcomment,"Si=%d 1.4-1.8MeV ",si+1);
       sprintf(histname,"MpSi_%d_2",si);
       Mp_si_2[si] = new TH1F(histname, hcomment,(int)dhMp,M_min, M_max);
//Mp everySi typ4 MeV
       sprintf(hcomment,"Si=%d 1.8-2.2MeV ",si+1);
       sprintf(histname,"MpSi_%d_3",si);
       Mp_si_3[si] = new TH1F(histname, hcomment,(int)dhMp,M_min, M_max);
//Mp everySi typ5 MeV
       sprintf(hcomment,"Si=%d 2.2-3.0MeV ",si+1);
       sprintf(histname,"MpSi_%d_4",si);
       Mp_si_4[si] = new TH1F(histname, hcomment,(int)dhMp, M_min, M_max);
//Mp everySi typ6 MeV
       sprintf(hcomment,"Si=%d 3.0-5MeV ",si+1);
       sprintf(histname,"MpSi_%d_5",si);
       Mp_si_5[si] = new TH1F(histname, hcomment,(int)dhMp,M_min, M_max);
//Mp everySi typ7 MeV
       sprintf(hcomment,"Si=%d 5-7 MeV ",si+1);
       sprintf(histname,"MpSi_%d_6",si);
       Mp_si_6[si] = new TH1F(histname, hcomment,(int)dhMp,M_min, M_max);
//Mp everySi typ4 MeV
       sprintf(hcomment,"Si=%d 7-9MeV ",si+1);
       sprintf(histname,"MpSi_%d_7",si);
       Mp_si_7[si] = new TH1F(histname, hcomment,(int)dhMp,M_min, M_max);
//Mp everySi typ5 MeV
       sprintf(hcomment,"Si=%d 9-11MeV ",si+1);
       sprintf(histname,"MpSi_%d_8",si);
       Mp_si_8[si] = new TH1F(histname, hcomment,(int)dhMp, M_min, M_max);
//Mp everySi typ6 MeV
       sprintf(hcomment,"Si=%d 11-14MeV ",si+1);
       sprintf(histname,"MpSi_%d_9",si);
       Mp_si_9[si] = new TH1F(histname, hcomment,(int)dhMp,M_min, M_max);
//Mp everySi typ7 MeV
       sprintf(hcomment,"Si=%d 14-17MeV ",si+1);
       sprintf(histname,"MpSi_%d_10",si);
       Mp_si_10[si] = new TH1F(histname, hcomment,(int)dhMp,M_min, M_max);
//Mp everySi typ7 MeV
       sprintf(hcomment,"Si=%d 17-20MeV ",si+1);
       sprintf(histname,"MpSi_%d_11",si);
       Mp_si_11[si] = new TH1F(histname, hcomment,(int)dhMp,M_min, M_max);
    }
//Mx everySi 0.7-1MeV
    for (si=0;si<6;si++){
       sprintf(hcomment,"Si=%d 0.7-1.0MeV ",si+1);
       sprintf(histname,"MxSi_%d_0",si);
       Mx_si_0[si] = new TH1F(histname, hcomment,(int)dhMx,M_min2, M_max2);
//Mx everySi 1-1.4MeV
       sprintf(hcomment,"Si=%d 1.0-1.4MeV ",si+1);
       sprintf(histname,"MxSi_%d_1",si);
       Mx_si_1[si] = new TH1F(histname, hcomment,(int)dhMx,M_min2, M_max2);
//Mx everySi 1.4-1.8MeV
       sprintf(hcomment,"Si=%d 1.4-1.8MeV ",si+1);
       sprintf(histname,"MxSi_%d_2",si);
       Mx_si_2[si] = new TH1F(histname, hcomment,(int)dhMx, M_min2, M_max2);
//Mx everySi 1.8-2.2MeV
       sprintf(hcomment,"Si=%d 1.8-2.2MeV ",si+1);
       sprintf(histname,"MxSi_%d_3",si);
       Mx_si_3[si] = new TH1F(histname, hcomment,(int)dhMx,M_min2, M_max2);
//Mx everySi 2.2-2.8MeV
       sprintf(hcomment,"Si=%d 2.2-2.8MeV ",si+1);
       sprintf(histname,"MxSi_%d_4",si);
       Mx_si_4[si] = new TH1F(histname, hcomment,(int)dhMx,M_min2, M_max2);
//Mx everySi 3-3.5MeV
       sprintf(hcomment,"Si=%d 3.0-3.5MeV ",si+1);
       sprintf(histname,"MxSi_%d_5",si);
       Mx_si_5[si] = new TH1F(histname, hcomment,(int)dhMx,M_min2, M_max2);
//Mx everySi 3.5-4.2MeV
       sprintf(hcomment,"Si=%d 3.5-4.2MeV ",si+1);
       sprintf(histname,"MxSi_%d_6",si);
       Mx_si_6[si] = new TH1F(histname, hcomment,(int)dhMx,M_min2, M_max2);
//Mx everySi 4.2-5.2MeV
       sprintf(hcomment,"Si=%d 4.2-5.2MeV ",si+1);
       sprintf(histname,"MxSi_%d_7",si);
       Mx_si_7[si] = new TH1F(histname, hcomment,(int)dhMx,M_min2, M_max2);
//Mx everySi 5.7-10.0MeV
       sprintf(hcomment,"Si=%d 5.7-10.0MeV ",si+1);
       sprintf(histname,"MxSi_%d_8",si);
       Mx_si_8[si] = new TH1F(histname, hcomment,(int)dhMx,M_min2, M_max2);
//Mx everySi 10-16MeV
       sprintf(hcomment,"Si=%d 10-16MeV ",si+1);
       sprintf(histname,"MxSi_%d_9",si);
       Mx_si_9[si] = new TH1F(histname, hcomment,(int)dhMx,M_min2, M_max2);
//Mx everySi 16-20MeV
       sprintf(hcomment,"Si=%d 16-20kMeV ",si+1);
       sprintf(histname,"MxSi_%d_10",si);
       Mx_si_10[si] = new TH1F(histname, hcomment,(int)dhMx,M_min2, M_max2);
//Mx everySi 5.6-16MeV
       sprintf(hcomment,"Si=%d 5.6-16MeV ",si+1);
       sprintf(histname,"MxSi_%d_11",si);
       Mx_si_11[si] = new TH1F(histname, hcomment,(int)dhMx,M_min2, M_max2);
    }

//channel distribution by MeV
    for(int ie=0;ie<16;++ie){
      sprintf(hcomment,"cll chi hannel distribution for typ%d MeV",ie);
      sprintf(histname,"chan_dist_%d",ie);
      chan_E[ie]= new TH1F(histname,hcomment,97,0.,97.);

      sprintf(hcomment,"all chi hannel distribution for typ%d MeV",ie);
      sprintf(histname,"chan_distTofChi_%d",ie);
      chan_Etofchi[ie]= new TH1F(histname,hcomment,97,0.,97.);
    }
    for(int ie=0;ie<16;++ie){
      sprintf(hcomment,"all chi hannel distribution for typ%d MeV",ie);
      sprintf(histname,"chan_distUra_%d",ie);
      chan_UraE[ie]= new TH1F(histname,hcomment,97,0.,97.);

      sprintf(hcomment,"all chi hannel distribution for typ%d MeV",ie);
      sprintf(histname,"chan_distUraTofChi_%d",ie);
      chan_UraEtofchi[ie]= new TH1F(histname,hcomment,97,0.,97.);
    }
//channel distribution by MeV
    for(int ie=0;ie<16;++ie){
      sprintf(hcomment,"cll chi hannel distribution for typ%d MeV",ie);
      sprintf(histname,"chan_distSelect_%d",ie);
      chan_selectE[ie]= new TH1F(histname,hcomment,97,0.,97.);
    }
//channel distribution by MeV
    for(int ie=0;ie<16;++ie){
      sprintf(hcomment,"tof 6 distribution for typ%d MeV",ie);
      sprintf(histname,"chan_dist_tof6_%d",ie);
      chan_Etof6[ie]= new TH1F(histname,hcomment,97,0.,97.);
    }
//channel distribution by MeV
    for(int ie=0;ie<16;++ie){
      sprintf(hcomment,"tof 8 distribution for typ%d MeV",ie);
      sprintf(histname,"chan_dist_tof8_%d",ie);
      chan_Etof8[ie]= new TH1F(histname,hcomment,97,0.,97.);
    }
//channel distribution by MeV
    for(int ie=0;ie<16;++ie){
      sprintf(hcomment,"channel tof 10 distribution for typ%d MeV",ie);
      sprintf(histname,"chan_dist_tof10_%d",ie);
      chan_Etof10[ie]= new TH1F(histname,hcomment,97,0.,97.);
    }



}//END if(dproc.SELECT_CHAN==-1){

//TREE    
    if(dproc.JET_EVENT_MODE==1){
       tfileTestEvent = new TFile(nameEvent, "RECREATE");

       JetEvent = new TTree("JetEvent","JetEvent builder");
       JetEvent->Branch("event_num",&TreeJet.event_num,"event_num/I");
       JetEvent->Branch("revolution",&TreeJet.revolution,"revolution/I");
       JetEvent->Branch("bunch",&TreeJet.bunch,"bunch/I");
       JetEvent->Branch("hit_num",&TreeJet.hit_num,"hit_num/I");
       JetEvent->Branch("target_inf",&TreeJet.target_inf,"target_inf/I");
       JetEvent->Branch("array_num",&TreeJet.array_num,"array_num/I");
       JetEvent->Branch("HitData",TreeJet.HitData,"HitData[array_num]/D");
       JetEvent->Branch("hjet_count",&TreeJet.hjet_count,"hjet_count/I");
    }

    if(dproc.RAW_EVENT_MODE==1){
       tfileTestRaw = new TFile(nameRAW, "RECREATE");

       JetEventRaw = new TTree("JetEventRaw","JetEventRaw builder");
       JetEventRaw->Branch("geo",&TreeJetRaw.geo,"geo/I");
       JetEventRaw->Branch("itg",&TreeJetRaw.itg,"itg/I");
       JetEventRaw->Branch("event_num",&TreeJetRaw.event_num,"event_num/I");
       JetEventRaw->Branch("revolution",&TreeJetRaw.revolution,"revolution/I");
       JetEventRaw->Branch("bid",&TreeJetRaw.bid,"bid/I");
       JetEventRaw->Branch("stp",&TreeJetRaw.stp,"stp/I");
       JetEventRaw->Branch("amp2",&TreeJetRaw.amp2,"amp2/I");
       JetEventRaw->Branch("time2",&TreeJetRaw.time2,"time2/I");
       JetEventRaw->Branch("timemax2",&TreeJetRaw.timemax2,"timemax2/I");
       JetEventRaw->Branch("square2",&TreeJetRaw.square2,"square2/I");
       JetEventRaw->Branch("amp3",&TreeJetRaw.amp3,"amp3/D");
       JetEventRaw->Branch("square3",&TreeJetRaw.square3,"square3/D");
       JetEventRaw->Branch("Energy",&TreeJetRaw.Energy,"Energy/D");
       JetEventRaw->Branch("ToF",&TreeJetRaw.ToF,"ToF/D");
       JetEventRaw->Branch("f_amp",&TreeJetRaw.f_amp,"f_amp/D");
       JetEventRaw->Branch("f_time",&TreeJetRaw.f_time,"f_time/D");
       JetEventRaw->Branch("f_timemax",&TreeJetRaw.f_timemax,"f_timemax/D");
       JetEventRaw->Branch("f_square",&TreeJetRaw.f_square,"f_square/D");
       JetEventRaw->Branch("f_ndf",&TreeJetRaw.f_ndf,"f_ndf/I");
       JetEventRaw->Branch("f_chi",&TreeJetRaw.f_chi,"f_chi/D");
       JetEventRaw->Branch("f_cndf",&TreeJetRaw.f_cndf,"f_cndf/D");
       JetEventRaw->Branch("target_inf",&TreeJetRaw.target_inf,"target_inf/I");
       JetEventRaw->Branch("hjet_count",&TreeJetRaw.hjet_count,"hjet_count/I");
       JetEventRaw->Branch("Mx", &TreeJetRaw.Mx, "Mx/D");
       JetEventRaw->Branch("Mp", &TreeJetRaw.Mp, "Mp/D");
    }

    if(dproc.WAVE_OUT_MODE==1){
       tfileTestWave = new TFile(nameWave, "RECREATE");
    }

    tfile->cd();
    if(dproc.WAVE_FIT_MODE==1){
       hist_event=new TH1D("hist_event","wv",90,0.,90);
       hist_residual  = new TH2D("hist_residual"," residual i & i+1",251,-25,25,251,-25,25);
    }

//////////////okadahEND

    return(0);
}

//okada//////////////////////////
int PawInit(char *outfilename){
    int st,si,i,bid;
    char hcomment[256];
    
    // PAW STUFF 
    int lun =1;
    int reclen = 1024;
    int icycle = 0;
    int nwpaw = 1000;//1000000,NWORDS_PAWC

    int hid, xnbin, ynbin;
    float xmin, xmax, ymin, ymax;
    char hname[100];
    char filecomment[100];
    char filestatus[5];

    hhlimit_(&nwpaw);
    
    sprintf(filecomment, "T-E INFO");
    sprintf(filestatus, "N");
    hhropen_(&lun, filecomment, outfilename, filestatus, &reclen, &icycle, 
             strlen(filecomment),strlen(outfilename),strlen(filestatus));

    fprintf (stdout,"new hbook file : %s\n",outfilename);

    // Bunch Distribution
    sprintf(hcomment,"Bunch Distribution");
    HHBOOK1(10000,hcomment,120,-.5,119.5);
    sprintf(hcomment,"Bunch Distribution (time + -t cut)");
    HHBOOK1(10010,hcomment,120,-.5,119.5);
    sprintf(hcomment,"Bunch Distribution (-t cut)");
    HHBOOK1(10020,hcomment,120,-.5,119.5);

    sprintf(hcomment,"Test Ntuple");
    HHBOOKN(30,hcomment);
    
 return(0);
}



// ================== 
//   histgram close   
// ================== 
// Writing on disk is needed only at the last moment

int hist_close(char *filename) {

    // PAW STUFF 
    int lun =1;
    int reclen = 1024;
    int icycle = 0;
    char filecomment[100];
    char filestatus[5];
    char chopt[5];
    int idout = 0;

    fprintf(stdout,"\nClosing histgram file : %s \n",filename);
    fprintf(stdout,"Writing ROOT file on Disk \n");
    
    sprintf(filecomment, "T-E INFO");
    sprintf(filestatus, "N");

//    hhropen_(&lun, filecomment, filename, filestatus, &reclen, &icycle, 
//             strlen(filecomment),strlen(filename),strlen(filestatus));

    sprintf(chopt, "T");

    hhrout_(&idout, &icycle, chopt, strlen(chopt));
    hhrend_(filecomment, strlen(filecomment));
    return (0);
}


//okada END//////////////
