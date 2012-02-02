// DATA check utility program 
//  file name :   asym_main.C
// 
// 
//  Author    :   Osamu Jinnouchi
//  Creation  :   3/27/2004
//                

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <iostream.h>
#include "rhicpol.h"
#include "rpoldata.h"
#include "asymrhic.h"

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

// ========================================
// Global variables 
// ========================================

float pawc_[NWORDS_PAWC];

datprocStruct dproc = {
    400,1000,   // energy
    -30,30,     // banana cut width (Constant mode only)
    0,0,        // MODES
    0,0,
    0,0,0,0,0,0,0,0,0,0
};  // data process modes 

atdata_struct atdata;

int tshift=0;    // timing shift in banana cut (ns)

int spinpat[120]; // spin pattern 120 bunches (ADO info)
int fillpat[120]; // spin pattern 120 bunches (ADO info)
float wcmdist[120];  // wall current monitor 120 bunches (ADO info)

long int Ncounts[6][120]; // counts 6detectors 120 bunches
long int NTcounts[6][120][6];  // counts 6detectors 120 bunches 6 tranges
long int NRcounts[6][120][RAMPTIME]; // counts for 6det 120bunch RAMPTIME sec
long int NStrip[3][72]; // counts 72 strips 3 spin states

char datafile[256];   // data file name 
char t0_conf[256];    // overwrite configuration for T0 info
char conf_file[256];  // overwrite configuration file 

cutparamStruct LookUpTable;  // look up table cut parameters 


long int Nevcut=0;       // number of events after 1st cut (whole data)
long int Nevtot=0;       // number of total events (whole data) 
long int Nread=0;        // real total events (completely everything)
int Nskip=1;             // number of events to be skipped in data process 
long int Ngood[120];     // number of events after carbon cut (each bunch)
long int Ntotal[120];    // number of events before carbon cut (each bunch)
long int Nback[120];     // number of events below the curbon cut (each bunch)


// ================= 
// beginning of main 
// ================= 
//void main (int argc, char *argv[]){
int main (int argc, char *argv[]){

    // for get option
    int c;
    extern char *optarg;
    extern int optind;
    
    // files
    char hbk_outfile[256];
    char lut_infile[256];
    int lut_read = 0;  // lut file read from argument:1 default:0
    int hbk_read = 0;  // hbk file read from argument:1 default:0
    
    
    // misc
    int i;
    char threshold[20],bunchid[20],enerange[20],cwidth[20],*ptr;
    int lth, uth;

    while ((c = getopt(argc, argv, "?f:n:ho:r:t:e:c:CDTABZF:MNW:UGR"))!=-1) {
        switch (c) {
        case 'h':
        case '?':
            cout << "Usage of " << argv[0] <<endl;
            cout << " -h(or?)              : print this help" <<endl;
            cout << " -f <filename>        : input data file name " <<endl;
            cout << " -n <number>          : evnt skip (n=1 noskip)" <<endl;
            cout << " -o <filename>        : Output hbk file" <<endl;
            cout << "                      : default out_sample.hbk" <<endl;
            cout << " -r <filename>        : LUT parameter file" <<endl;
            cout << "                      : default lut_sample.dat" <<endl;
            cout << " -t <time shift>      : banana cut timing " <<endl;
            cout << "                      : default (0:23) " <<endl;
            cout << " -e <lower:upper>     : kinetic energy range" <<endl;
            cout << "                      : default (350:900) keV" <<endl;
            cout << "                                       " <<endl;
            cout << " <MODE> ---------------(default all off)----" <<endl;
            cout << " -C                   : Calibration mode on " <<endl;
            cout << " -D                   : Dead layer  mode on " <<endl;
            cout << " -T                   : T0 study    mode on " <<endl;
            cout << " -A                   : A0,A1 study mode on " <<endl;
            cout << " -B                   : create banana curve on" <<endl;
            cout << " -Z                   : without T0 subtraction" <<endl;
            cout << " -F <file>            : read T0 info file" <<endl;
            cout << " -c <file>            : read cfg info file" <<endl;
            cout << "                      : fixed file cfginfo.dat" <<endl;
            cout << " -M                   : exit after run message" <<endl;
            cout << " -W <lower:upper>     : const width banana cut" <<endl;
            cout << " -U                   : update histogram" <<endl;
            cout << " -G                   : mass mode on " <<endl;
            cout << " -N                   : store Ntuple events" <<endl;
            cout << " -R                   : ramp measurement 1sec bin" <<endl;
            exit(0);
        case 'f':
            sprintf(datafile, optarg);
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
        case 'r': // determine parameter file to be read
            strcpy(lut_infile, optarg);
            fprintf(stdout,"LookUpTable parameter file: %s \n",lut_infile); 
            lut_read = 1;
            break;
        case 't': // set timing shift in banana cut
            tshift = atoi(optarg);
            fprintf(stdout,"Timing has been shifted by %d ns\n",tshift);
            break;
        case 'e': // set energy range
            strcpy(enerange, optarg);
            if (ptr = strrchr(enerange,':')){
                ptr++;
                dproc.eneu = atoi(ptr);
                strtok(enerange,":");
                dproc.enel = atoi(enerange);
                if ((dproc.enel==NULL)||(dproc.enel<0)) { dproc.enel=0;}
                if ((dproc.eneu==NULL)||(dproc.eneu>2000)) { dproc.eneu=2000;}
                fprintf(stdout,"ENERGY RANGE LOWER:UPPER = %d:%d\n",
                        dproc.enel,dproc.eneu);
            } else {
                cout << "Wrong specification for energy threshold" <<endl;
                exit(0);
            }
            break;
        case 'F':
            sprintf(t0_conf, optarg);
            fprintf(stdout,"read T0 conf: %s \n",t0_conf); 
            dproc.READT0MODE = 1;
            break;
        case 'c':
            sprintf(conf_file, optarg);
            fprintf(stdout,"read conf file: %s \n",conf_file); 
            dproc.CFGMODE = 1;
            break;
        case 'C':
            dproc.CMODE = 1;
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
        case 'M':
            dproc.MESSAGE = 1;
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
        case 'R':
            dproc.RAMPMODE=1;
            fprintf(stdout,"RAMP MEASUREMENT ON\n"); 
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
            } else {
                fprintf(stdout,"Wrong specification constant banana cut\n");
                exit(0);
            }
            fprintf(stdout,"BANANA Cut : %d <==> %d \n",
                    dproc.widthl,dproc.widthu);
            break;
        default:
            fprintf(stdout,"Invalid Option \n");
            exit(0);
        }
    }
    

    // if the parameter file is not specified
    if (lut_read == 0 ) {
        sprintf(lut_infile, "lut_sample.dat");
        fprintf(stdout,"LookUpTable DEFAULT file: %s \n",lut_infile); 
    }        
    
    // if output hbk file is not specified
    if (hbk_read == 0 ) {
        sprintf(hbk_outfile, "outsampleex.hbook");
        fprintf(stdout,"Hbook DEFAULT file: %s \n",hbk_outfile); 
    }        
    
    fprintf(stdout,"Reading ... cut parameter file\n");
    if (read_param(lut_infile) != 0) {
        perror("Error: read parameter");
        exit(1);
    }

    fprintf(stdout,"Booking ... histgram file\n");
    if (hist_book(hbk_outfile) != 0) {
        perror("Error: hist_book");
        exit(1);
    }

    fprintf(stdout,"Processing ... each events\n");
    fprintf(stdout,"If you terminate this process, the temporal output will be made in tmp_out.hbk\n");

    //
    // LOOP THROUGH THE DATA
    //
    if (readloop() != 0) {
        perror("Error: readloop");
        exit(1);
    }
    
    // HIST FILE CLOSE

    if (hist_close(hbk_outfile) !=0) {
        perror("Error: hist_close");
        exit(1);
    }
    
} // end of main



// ================= 
// read loop routine 
// ================= 
int readloop() {
    
    int rval;
    int nev;
    int j;
    long int Nevent, i;
    int flag;  // exit from while when flag==1

    // Common structure for the data format
    static union {
        recordHeaderStruct     header;
        recordBeginStruct      begin;
        recordPolAdoStruct     polado;
        recordTagAdoStruct     tagado;
        recordBeamAdoStruct    beamado;
        recordConfigRhicStruct cfg;
        recordReadWaveStruct   all;
        recordReadWave120Struct all120;
        recordReadATStruct      at;
        recordWFDV8ArrayStruct  wfd;
        recordEndStruct        end;
        recordScalersStruct    scal;
        recordWcmAdoStruct     wcmado;
        char                   buffer[BSIZE*sizeof(int)];
        recordDataStruct       data;
    } rec;
    
    recordConfigRhicStruct  *cfginfo;
    beamDataStruct beamdat;
    wcmDataStruct wcmdat;
    int bid,pat;
    processEvent event;
    char pattern[1];
    int nreadsi;  // number of Si already read
    int nreadbyte; 
    FILE *fp;

    // -------------------------------------------------
    flag = 0;
    
    // reading the data till its end ...
 
    fp = fopen(datafile,"r");

    while (flag==0) {
        if (fread(&rec.header, sizeof(recordHeaderStruct), 1, fp)!=1){
            break;
        }

        if (feof(fp)) {
            fprintf(stdout,"Expected end of file\n");
            break;
        }
        
        if (rec.header.len > BSIZE*sizeof(int)){
            fprintf(stdout,"Not enough buffer d: %d byte b: %d byte\n",
                    rec.header.len, sizeof(rec));
            break;
        }

        // Read rest of the structure
        rval = fread(&rec.begin.version, rec.header.len - 
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
            cout << "Begin of data stream Ver: " <<rec.begin.version<<endl;
            cout << "Comment: " << rec.begin.comment <<endl;
            cout << "Time: " << ctime(&rec.begin.header.timestamp.time) 
                 <<endl;
            /*
            memset(Ucounts, 0, sizeof(Ucounts));  
            memset(Dcounts, 0, sizeof(Dcounts));  
            memset(lcounts, 0, sizeof(lcounts));  
            */
            break;

        case REC_POLADO:
            break; // later

        case REC_TAGADO:
            break; // later

        case REC_WCMADO:
            fprintf(stdout,"Reading WCM information\n");
            memcpy(&wcmdat, &rec.wcmado.data,sizeof(wcmdat));
            for (bid=0;bid<120;bid++){
                wcmdist[bid] = wcmdat.fillDataM[bid*3];
            }
            break;

        case REC_BEAMADO:
            fprintf(stdout,"Reading Beam Information\n");
            memcpy(&beamdat, &rec.beamado.data, sizeof(beamdat));
            for (bid=0;bid<120;bid++) {
                pat = beamdat.polarizationFillPatternS[bid*3];
                if (pat>0) {
                    spinpat[bid] = 1;
                } else if (pat<0) {
                    spinpat[bid] = -1;
                } else {
                    spinpat[bid] = 0;
                }
                pat = beamdat.measuredFillPatternM[bid*3];
                if (pat>0) {
                    fillpat[bid] = 1;
                } else if (pat<0) {
                    fillpat[bid] = -1;
                } else {
                    fillpat[bid] = 0;
                }
            }

            cout << "Spin Pattern Used : " << endl;
            char pattern[3][5] ;
            sprintf(pattern[0],"-");
            sprintf(pattern[1],".");
            sprintf(pattern[2],"+");

            for (bid=0;bid<120;bid++) {
                if (bid%10 == 0) cout << " ";
                cout << pattern[spinpat[bid]+1];
            }
            cout <<endl;
            cout << "Fill Pattern Used : " << endl;
            for (bid=0;bid<120;bid++) {
                if (bid%10 == 0) cout << " ";
                cout << pattern[fillpat[bid]+1];
            }
            cout<<endl;
            break; //later

        case REC_END:
            fprintf(stdout,"End of data stream \n");
            fprintf(stdout,"End Time: %s\n", 
                    ctime(&rec.end.header.timestamp.time));
            fprintf(stdout,"%ld records found\n",Nread);
            fprintf(stdout,"Total number of counts \n");
            fprintf(stdout,"%ld Carbons are found in\n",Nevcut);
            fprintf(stdout,"%ld Total events of intended fill pattern\n",
                    Nevtot);
            fprintf(stdout,"Data Comment: %s\n", rec.end.comment);
            if (end_process(cfginfo)!=0) {
                fprintf(stdout, "Error at end process \n");
            }
            flag = 1;
            break;

        case REC_READRAW:
            break; // later

        case REC_READSUB:
            break; // later

        case REC_READALL:
            break; // later

        case REC_WFDV8SCAL:
            break; // later

        case REC_SCALERS:
            fprintf(stdout,"Scaler readtime \n");
            fprintf(stdout,"Scaler End Time: %s\n", 
                    ctime(&rec.scal.header.timestamp.time));
            break; // later

        case REC_READAT:
            event.delim = rec.header.timestamp.delim;
            nreadsi = 0;  // number of Si already read
            nreadbyte = 0;  // number of bite already read
            
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

                for (j=0; j<Nevent; j++) {
                    Nread++;
                    event.amp  = ATPtr -> data[j].a;
                    event.tdc  = ATPtr -> data[j].t; 
                    event.intg = ATPtr -> data[j].s;
                    event.bid  = ATPtr -> data[j].b;
                    
                    /*cout << " i " <<i
                        << " Nevent " << Nevent
                        << " St " << event.stN  
                       << " amp " << event.amp 
                       << " tdc " << event.tdc 
                       << " bid " << event.bid <<endl;
                    */

                    if ((fmod(Nread,Nskip)==0)&&
                        ((fillpat[event.bid]==1)||(dproc.CMODE==1))) {
                        
                        // Event Processing
                        if (event_process(&event,cfginfo)!=0) {
                            fprintf(stdout, 
                                    "Error event process Si:%d Ev:%d\n",
                                    nreadsi,j);
                        }
                    }
                    
                    if (Nread%1000000==0) {
                        printf (" Ncounts = %ld \n", Nread);
                        fflush(stdout);
                    }
                }
            }
            break;
            
        case REC_RHIC_CONF:
            fprintf(stdout,"Read configure information\n");
            cfginfo = (recordConfigRhicStruct *)
                malloc(sizeof(recordConfigRhicStruct)+
                       (rec.cfg.data.NumChannels-1)*sizeof(SiChanStruct));

            memcpy(cfginfo, &rec.cfg, sizeof(recordConfigRhicStruct)+
                   (rec.cfg.data.NumChannels-1)*sizeof(SiChanStruct));
            // when we mandatory provide cfg info  
            if (dproc.CFGMODE==1) {
                readConfig(cfginfo);
            }
            if (dproc.READT0MODE==1) {
                readT0Config(cfginfo);
            }
            // Display configuration
            if (printConfig(cfginfo)!=0) {
                perror(" error in printing configuration");
                exit(1);
            }
            if (dproc.MESSAGE == 1) exit(0);
            break;

        default:    // unknown record
            fprintf(stdout,"Encountered Unknown Record 0x%x\n",
                    rec.header.type & REC_TYPEMASK);
            break;
        }
    }
    fprintf(stdout,"End of loop\n");
    return(0);
}

// ===================================
// Booking hbk files (starts from 10000)
// ===================================
// hid        : comments 
//     
//  10000     : bunch distribution (default)
//  10010     : bunch distribution carbon (time + -t cut)(default)
//  10020     : bunch distribution carbon (time cut)(default)
//  10030     : wall current monitor distribution (default)
//  10040     : energy distribution for A_N calculation (default)
//  10100+Si  : bunch distribution carbon for each Si (default)
//  10200     : delimiter distribution (default)
//  10300+Si  : strip distribution (default)
//  10310+Si  : strip distribution time cut (default)
//  10320+Si  : strip distribution time + -t cut (default)
//  10400+Si  : energy distribution (defualt)
//  10410+Si  : energy distribution carbon (defualt)
//  10450+Si  : -t cross section (amplitude) (defualt)
//  10460+Si  : -t cross section (integral)  (defualt)
//  10500+St  : energy dists for plus  bunch (each strip,default)
//  10600+St  : energy dists for minus bunch (each strip,default)
//  10700+i   : energy dists in given -t bin
//  11000+bid : bunch by bunch timing infor (default)
//-CMODE
//  12000+St  : energy calibration
//-TMODE
//  12100+St  : T0 determination
//-AMODE
//  12200+St  : pile up rejection parameters
//  12300+St  : Tof vs. Integral 
//-BMODE
//  13000+St  : banana curve (E-T)
//  13100+St  : banana curve (ADC-TDC)
//  13500+St  : lower curbon cut boundary
//  13600+St  : upper curbon cut boundary
//  14000+St  : bunch crossing
//-DMODE
//  15000+St  : Dead layer + T0 estimation
//-MMODE
//  16000+St  : mass plot(1d)
//  16100+St  : mass plot vs. energy (2d)
//-RAMPMODE
//  20000+i   : carbon plots for ramp 
//  21000+Si  : delimiter dist for UpSpin
//  21100+Si  : delimiter dist for DwSpin
//-ASYMMETRY RESULTS
//  30000     : X90 phys(0), acpt(10), lumi(20)  
//  30100     : X90 phys(0), acpt(10), lumi(20)  
//  30200     : X90 phys(0), acpt(10), lumi(20)  
//  30300     : X90 phys(0), acpt(10), lumi(20)  
//  31000     : spinpattern
//  31010-60  : Si-1...Si-6 Nevents
//  31100     : RL90 (with errors)
//  31110     : RL45 (with errors)
//  31120     : BT45 (with errors)
//  32000+i   : -t bined X90 (i=1,6)
//  32100+i   : -t bined X45
//  32200+i   : -t bined Y45
//  32300+i   : -t bined C45
//  33000     : Good Events Hist
//  33010     : BackGround Events Hist
//  33020     : Total Events Hist
//  34000     : A_N (1bin)
//  35000+St  : fine -t dependece UpSpin for each strip 
//  35100+St  : fine -t dependece DwSpin for each strip 
//  36000     : strip dists for Up spin
//  36100     : strip dists for Down spin

int hist_book(char *filename){

    cout << " In function hist_hbook " <<endl;
    int st,si,i,bid;
    char hcomment[256];
    
    // PAW STUFF 
    int lun =1;
    int reclen = RECLEN;
    int icycle = 0;
    int nwpaw = NWORDS_PAWC;
    
    int hid, xnbin, ynbin;
    float xmin, xmax, ymin, ymax;
    char hname[100];
    char filecomment[100];
    char filestatus[5];

    hhlimit_(&nwpaw);
    
    sprintf(filecomment, "T-E INFO");
    sprintf(filestatus, "N");
    hhropen_(&lun, filecomment, filename, filestatus, &reclen, &icycle, 
             strlen(filecomment),strlen(filename),strlen(filestatus));

    fprintf (stdout,"new hbook file : %s\n",filename);

    // Bunch Distribution
    sprintf(hcomment,"Bunch Distribution");
    HHBOOK1(10000,hcomment,120,-.5,119.5);
    sprintf(hcomment,"Bunch Distribution (time + -t cut)");
    HHBOOK1(10010,hcomment,120,-.5,119.5);
    sprintf(hcomment,"Bunch Distribution (-t cut)");
    HHBOOK1(10020,hcomment,120,-.5,119.5);

    // WCM
    sprintf(hcomment,"WCM Distribution");
    HHBOOK1(10030,hcomment,120,-.5,119.5);

    // Bunch Distribution for Each Si
    HHBOOK1(10110,"bunch distribution Si-1",120,-.5,119.5);
    HHBOOK1(10120,"bunch distribution Si-2",120,-.5,119.5);
    HHBOOK1(10130,"bunch distribution Si-3",120,-.5,119.5);
    HHBOOK1(10140,"bunch distribution Si-4",120,-.5,119.5);
    HHBOOK1(10150,"bunch distribution Si-5",120,-.5,119.5);
    HHBOOK1(10160,"bunch distribution Si-6",120,-.5,119.5);

    // delimeter distribution
    sprintf(hcomment,"Delimiter distribution");
    HHBOOK1(10200,hcomment,24000,0.,24000.);

    // Strip Distribution
    for (si=1;si<=6;si++){
        sprintf(hcomment,"strip dist Si - %d ",si);
        HHBOOK1(10300+si,hcomment, 12,0.5,12.5);
        sprintf(hcomment,"strip dist time cut Si - %d ",si);
        HHBOOK1(10310+si,hcomment, 12,0.5,12.5);
        sprintf(hcomment,"strip dist time + -t cut Si - %d ",si);
        HHBOOK1(10320+si,hcomment, 12,0.5,12.5);
    }

    // Energy Distribution

    HHBOOK1(10040,"For A_N Calc", 80, 0., 1300. );

    for (si=1;si<=6;si++){
        sprintf(hcomment,"energy dist Si - %d ",si);
        HHBOOK1(10400+si,hcomment, 100,0.,1300.);
        sprintf(hcomment,"energy dist carbon Si - %d ",si);
        HHBOOK1(10410+si,hcomment, 100,0.,1300.);
        sprintf(hcomment,"energy dist (integral) carbon Si - %d ",si);
        HHBOOK1(10420+si,hcomment, 50,0.,1500.);
        sprintf(hcomment," -t slope si - %d (amp)",si);
        HHBOOK1(10450+si,hcomment,200,0.,0.04);
        sprintf(hcomment," -t slope si - %d (int)",si);
        HHBOOK1(10460+si,hcomment,200,0.,0.04);
    }
    for (st=1;st<=72;st++){
        sprintf(hcomment,"energy dist Strip %d (plus)",st);
        HHBOOK1(10500+st,hcomment,50,0.,1300.);
        sprintf(hcomment,"energy dist Strip %d (minus)",st);
        HHBOOK1(10600+st,hcomment,50,0.,1300.);
    }
    
    // timing shift in bunch by bunch
    for (bid=0;bid<60;bid++){
        sprintf(hcomment,"banana 600-650keV bunch-%d",bid);
        HHBOOK1(11000+bid,hcomment, 40, 0., 80.);
    }   

    // Dead layer estimation
    if (dproc.DMODE == 1) {
        for (st=1;st<=72;st++){
            sprintf(hcomment,"Tof vs Edep St - %d ",st);
            HHBOOK2(15000+st,hcomment,
                    100, 200., 1200.,  120, 0., 120.);
        }
    }
    
    // Energy distribution in given -t bin
    for (int ibin=0; ibin<10; ibin++) {
        float Elow = 200. + ibin*100.;
        float Eupp = 300. + ibin*100.;
        
        sprintf(hcomment,"Energy dist %d-bin",ibin);
        HHBOOK1(10700+ibin+1,"",10,Elow,Eupp);
    }

    // Calibration hists
    if (dproc.CMODE == 1) {
        for (st=1;st<=72;st++){
            sprintf(hcomment,"241 Am (5.486MeV) - strip %d ",st);
            HHBOOK1(12000+st,hcomment,256,0.,256.);
        }
    }

    // for T0 (due to cable length) determination 
    if (dproc.TMODE == 1) {
        if (dproc.ZMODE == 0) {
            for (st=1;st<=72;st++){
                sprintf(hcomment,"ST-%d abs t wo T0 subt vs. 1./sqrt(e)",st);
                HHBOOK2(12100+st,hcomment,40,0.,0.1,40,60.,140.);
            }
        } else {
            for (st=1;st<=72;st++){
                sprintf(hcomment,"ST-%d abs t wo T0 subt vs. 1./sqrt(e)",st);
                HHBOOK2(12100+st,hcomment,40,0.,0.1,40,-10.,70.);
            }
        }
    }
    
    // signal integral vs. amplitude A0,A1, and iasigma determination
    if (dproc.AMODE == 1){
        for (st=1;st<=72;st++){
            sprintf(hcomment,"strip %d -- Int vs. Amp ",st);
            HHBOOK2(12200+st,hcomment,100,0.,200., 256,0.,4000.);
            
            sprintf(hcomment,"strip %d -- Tof vs. Int",st);
            HHBOOK2(12300+st,hcomment,128,0.,4000.,80,20.,100.);
        }
    }
    
    // banana plots (E-T)
    if (dproc.BMODE == 1){
        for (st=1;st<=72;st++){
            sprintf(hcomment,"st %d - tof vs ene ",st);
            HHBOOK2(13000+st,hcomment, 160, 0., HMAXENE, 160, -0.,200.);
            sprintf(hcomment,"st %d - TDC vs ADC",st);
            HHBOOK2(13100+st,hcomment, 256, -.5, 255.5, 256, -.5, 255.5);

            sprintf(hcomment,"st %d - lower cut",st);
            HHBOOK1(13500+st,hcomment, 80, 0., HMAXENE);
            sprintf(hcomment,"st %d - upper cut",st);
            HHBOOK1(13600+st,hcomment, 80, 0., HMAXENE);

            sprintf(hcomment,"st %d - bunch",st);
            HHBOOK1(14000+st,hcomment, 120, -.5, 119.5);

        }
    }

    // Mass plots
    if (dproc.MMODE == 1) {
        for (st=1;st<=72;st++){
            sprintf(hcomment,"Mass st %d ",st);
            HHBOOK1(16000+st,hcomment,90,0.,30.);
        }
    }

    // For asymmetry results 
    
    HHBOOK1(30000, "X90 PHYS", 120, -0.5, 119.5);
    HHBOOK1(30010, "X90 ACPT", 120, -0.5, 119.5);
    HHBOOK1(30020, "X90 LUMI", 120, -0.5, 119.5);

    HHBOOK1(30100, "X45 PHYS", 120, -0.5, 119.5);
    HHBOOK1(30110, "X45 ACPT", 120, -0.5, 119.5);
    HHBOOK1(30120, "X45 LUMI", 120, -0.5, 119.5);

    HHBOOK1(30200, "Y45 PHYS", 120, -0.5, 119.5);
    HHBOOK1(30210, "Y45 ACPT", 120, -0.5, 119.5);
    HHBOOK1(30220, "Y45 LUMI", 120, -0.5, 119.5);

    HHBOOK1(30300, "C45 PHYS", 120, -0.5, 119.5);
    HHBOOK1(30310, "C45 ACPT", 120, -0.5, 119.5);
    HHBOOK1(30320, "C45 LUMI", 120, -0.5, 119.5);

    HHBOOK1(31000, "spin pattern", 120, -0.5, 119.5);

    HHBOOK1(31010, "Si-1 Bunch Dist", 120, -0.5, 119.5);
    HHBOOK1(31020, "Si-2 Bunch Dist", 120, -0.5, 119.5);
    HHBOOK1(31030, "Si-3 Bunch Dist", 120, -0.5, 119.5);
    HHBOOK1(31040, "Si-4 Bunch Dist", 120, -0.5, 119.5);
    HHBOOK1(31050, "Si-5 Bunch Dist", 120, -0.5, 119.5);
    HHBOOK1(31060, "Si-6 Bunch Dist", 120, -0.5, 119.5);

    HHBOOK1(31100, "RL90", 120, -0.5, 119.5);
    HHBOOK1(31110, "RL45", 120, -0.5, 119.5);
    HHBOOK1(31120, "BT45", 120, -0.5, 119.5);
    
    for (int tbin=0; tbin<6; tbin++) {
        HHBOOK1(32000+tbin+1, "-t binned X90", 120, -0.5, 119.5);
        HHBOOK1(32100+tbin+1, "-t binned X45", 120, -0.5, 119.5);
        HHBOOK1(32200+tbin+1, "-t binned Y45", 120, -0.5, 119.5);
        HHBOOK1(32300+tbin+1, "-t binned C45", 120, -0.5, 119.5);
    }
    
    HHBOOK1(33000, "Bid dists for Good Events", 120, -0.5, 119.5);
    HHBOOK1(33010, "Bid dists for Background Events", 120, -0.5, 119.5);
    HHBOOK1(33020, "Bid dists for Total Events", 120, -0.5, 119.5);
    HHBOOK1(34000, "A_N", 1, 0., 1.);

    // -t dependence
    for (int st=0;st<72;st++){
        sprintf(hcomment,"-t dist UpSpin for st-%d",st);
        HHBOOK1(35000+st,hcomment,11,-0.5,10.5);
        sprintf(hcomment,"-t dist DwSpin for st-%d",st);
        HHBOOK1(35100+st,hcomment,11,-0.5,10.5);
        sprintf(hcomment,"-t dist 0 Spin for st-%d",st);
        HHBOOK1(35200+st,hcomment,11,-0.5,10.5);
    }

    HHBOOK1(36000, "Strip Dist Up  ", 72, -0.5, 71.5);
    HHBOOK1(36100, "Strip Dist Down", 72, -0.5, 71.5);

    // Ntuple booking
    //    HBNT(1,"AT mode"," ");
    //HBNAME(1,"ATDATA",atdata.ia,
    //       "ADC:U:8,TDC:U:8,S:I,BID:U:7,DLIMIT:U,ST:U:7,ENE:R,TOF:R,POL:I");

    // histogram during RAMP
    if (dproc.RAMPMODE==1) {

        // Timing spectrum 
        for (i=0;i<40;i++){
            sprintf(hcomment,"carbon %d-%d sec\n",i*10,(i+1)*10);
            HHBOOK2(20000+i,hcomment, 40, 0., HMAXENE, 40, -0.,200.);
        }
        
        // Number of Carbons in Si-s
        for (int Si=0; Si<6; Si++) {
            sprintf(hcomment,"UpSpin Si-%d",Si+1);
            HHBOOK1(21000+Si, hcomment, RAMPTIME, 0., (float)RAMPTIME);
            sprintf(hcomment,"DwSpin Si-%d",Si+1);
            HHBOOK1(21100+Si, hcomment, RAMPTIME, 0., (float)RAMPTIME);
        }

    }

    return(0);
}
//
// UTILITY PART
//


// ================== 
//   histgram close   
// ================== 
// Writing on disk is needed only at the last moment

int hist_close(char *filename) {

    // PAW STUFF 
    int lun =1;
    int reclen = RECLEN;
    int icycle = 0;
    char filecomment[100];
    char filestatus[5];
    char chopt[5];
    int idout = 0;

    fprintf(stdout,"\nClosing histgram file : %s \n",filename);
    fprintf(stdout,"Writing Hbook file on Disk \n");
    
    sprintf(filecomment, "T-E INFO");
    sprintf(filestatus, "N");

    hhropen_(&lun, filecomment, filename, filestatus, &reclen, &icycle, 
             strlen(filecomment),strlen(filename),strlen(filestatus));

    sprintf(chopt, "T");

    hhrout_(&idout, &icycle, chopt, strlen(chopt));
    hhrend_(filecomment, strlen(filecomment));
    return (0);
}


// =========================
// Read the parameter file
// =========================
// Look Up Table reading 
int read_param(char *filename){
    /*
    FILE *fp_lut;
    int i, strip;

    fprintf(stdout,"\nReading ... cut parameter file : %s \n", filename);

    fp_lut = fopen(filename,"r");
    if (fp_lut == NULL) {
        perror ("failed to open parameter file");
        exit(1);
    }
    for (i=0;i<6;i++) {
        fscanf(fp_lut, "%d %f %f", 
               &strip, &LookUpTable.t0f[i],&LookUpTable.t0w[i]);
        if (i!=(strip-1)) {
            perror ("wrong format in cut parameter file ");
            exit(1);
        }
        fprintf(stdout,"CUT PARAMETER (%d) : T0 %f WIDTH %f \n",
                i,LookUpTable.t0f[i], LookUpTable.t0w[i]);
    }
    fclose(fp_lut);
    */
    return(0);
}
// Calibration parameter
void readT0Config(recordConfigRhicStruct *cfginfo){
    FILE *fp;
    int st;
    int strip;
    float t0,t0e;

    fprintf(stdout,"**********************************\n");
    fprintf(stdout,"******* T0 is overwritten ********\n");
    fprintf(stdout,"**********************************\n");

    fp = fopen(t0_conf,"r");
    if (fp == NULL) {
        perror ("failed to open T0 parameter file");
        exit(1);
    }
    for (st=0;st<48;st++){
        fscanf(fp,"%d %f %f",&strip,&t0,&t0e);
        fprintf(stdout,"strip %d from %f",strip,
                cfginfo->data.chan[strip-1].t0);
        fprintf(stdout," to %f\n",t0);
        cfginfo->data.chan[strip-1].t0=(float)t0;
    }
    fclose(fp);
}

// Calibration parameter
void readConfig(recordConfigRhicStruct *cfginfo){
    FILE *fp;
    int st,strip;
    float t0,acoef,edead,ecoef,A0,A1,iasigma;

    fprintf(stdout,"**********************************\n");
    fprintf(stdout,"** Configuration is overwritten **\n");
    fprintf(stdout,"**********************************\n");

    fp = fopen(conf_file,"r");
    fprintf(stdout,"Reading configuration info from : %s\n",conf_file);
    if (fp == NULL) {
        perror ("failed to open cfg parameter file");
        exit(1);
    }

    for (st=0;st<48;st++){
        fscanf(fp,"%d %f %f %f %f %f %f %f",
               &strip,&t0,&ecoef,&edead,&A0,&A1,&acoef,&iasigma);
        cfginfo->data.chan[strip-1].acoef=(float)acoef;
        cfginfo->data.chan[strip-1].edead=(float)edead;
        cfginfo->data.chan[strip-1].ecoef=(float)ecoef;
        cfginfo->data.chan[strip-1].t0=(float)t0;
        cfginfo->data.chan[strip-1].A0=(float)A0;
        cfginfo->data.chan[strip-1].A1=(float)A1;
    }
    fclose(fp);
}

// =====================================
// Print Out Configuration information 
// =====================================
int printConfig(recordConfigRhicStruct *cfginfo){
    
    int ccutwu;
    int ccutwl;

    fprintf(stdout,"================================================\n");
    fprintf(stdout,"===  RHIC Polarimeter Configuration (BGN)    ===\n");
    fprintf(stdout,"================================================\n");

    if (dproc.CBANANA == 0) {
        ccutwl = (int)cfginfo->data.chan[3].ETCutW;
        ccutwu = (int)cfginfo->data.chan[3].ETCutW;
    } else {
        ccutwl = (int)dproc.widthl;
        ccutwu = (int)dproc.widthu;
    }
    fprintf (stdout,"Carbon cut width : (low) %d (up) %d nsec \n",
             ccutwl,ccutwu);
    
    fprintf(stdout,"================================================\n");
    fprintf(stdout,"===  RHIC Polarimeter Configuration (END)    ===\n");
    fprintf(stdout,"================================================\n");

    return(0);
}


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









