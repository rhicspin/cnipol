#ifndef ASYM_MAIN_H
#define ASYM_MAIN_H

// Default Values for Run Condition
datprocStruct dproc = { 
    400,900,   // energy
    -30,30,     // banana cut width (Constant mode only)
    0, // FEEDBACKMODE
    0, // CMODE
    0, // DMODE
    0, // TMODE
    0, // AMODE
    1, // BMODE
    0, // ZMODE
    0, // MESSAGE
    2, // CBANANA
    0, // UPDATE
    1, // MMODE
    0, // NTMODE
    1, // RECONFMODE
    0, // RAMPMODE
    0, // STUDYMODE
    3, // MassSigma banana curve cut within <MassSigma> away from the 12C mass
    1.5e6,   // 1-sigma of 12C mass peak => 1.5e6 [keV] 
    0,       // Time shift in [ns]
    999.05,  // Wall Current Monitor process Fill range +/-5[%] 
    8,       // Lower Mass limit for peak position adjustment fit default :8 GeV
    100,     // <thinout> event rate to be feed into feedback routine
};  // data process modes 

StructExtInput extinput = {
  0, // CONFIG
  0, // MASSCUT
  0. // TSHIFT
} ;

StructReadFlag ReadFlag = {
  0, // RECBEGIN
  0, // PCTARGET 
  0, // WCMADO
  0, // READADO
  0  // RHICCONF
};

StructFlag Flag = {
  0       // feedback mode
};

// phi angle of each strips for Run05 (l=18.5cm)
float phiRun5[72]={
0.72601,0.73679,0.74758,0.75838,0.76918,0.77999,0.79080,0.80161,0.81242,0.82322,0.83401,0.84479,
1.51141,1.52219,1.53298,1.54378,1.55458,1.56539,1.57620,1.58701,1.59782,1.60862,1.61941,1.63019,
2.29680,2.30758,2.31837,2.32917,2.33998,2.35079,2.36160,2.37241,2.38321,2.39401,2.40480,2.41558,
3.86760,3.87838,3.88917,3.89997,3.91078,3.92159,3.93240,3.94321,3.95401,3.96481,3.97560,3.98638,
4.65300,4.66378,4.67457,4.68537,4.69617,4.70698,4.71779,4.72860,4.73941,4.75021,4.76100,4.77178,
5.43840,5.44918,5.45997,5.47077,5.48157,5.49238,5.50319,5.51400,5.52481,5.53561,5.54640,5.55718
} ;

// phi angle of each strips in trancated angles {45,90,135,225,270,315} deg.
float phiRun5t[72]={
0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,
1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,
2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,
3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,
4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,
5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779
} ;


atdata_struct atdata;
StructRunInfo runinfo;
StructAverage average;
StructFeedBack feedback;
StructRunConst runconst;
StructAnalysis analysis;

int spinpat[120]; // spin pattern 120 bunches (ADO info)
int fillpat[120]; // spin pattern 120 bunches (ADO info)
int wcmfillpat[120]; // spin pattern 120 bunches (ADO info)
float wcmdist[120];  // wall current monitor 120 bunches (ADO info)

long int Ncounts[6][120]; // counts 6detectors 120 bunches
long int NTcounts[6][120][NTBIN];  // counts 6detectors 120 bunches 6 tranges
long int NRcounts[6][120][RAMPTIME]; // counts for 6det 120bunch RAMPTIME sec
long int NDcounts[6][120][MAXDELIM]; // counts for 6 det 120 bunch per DELIMiter  
long int NStrip[3][72]; // counts 72 strips 3 spin states

char * confdir;
char * calibdir;
char datafile[256];   // data file name 
char reConfFile[256];    // overwrite configuration for T0 info
char conf_file[256];  // overwrite configuration file 
char CalibFile[256];  // energy calibration file

float ramptshift[500];  // ramp timing shift 

long int Nevcut=0;       // number of events after 1st cut (whole data)
long int Nevtot=0;       // number of total events (whole data) 
long int Nread=0;        // real total events (completely everything)
long int Nwcmtot=0;      // number of total wall current monitor data.
int Nskip=1;             // number of events to be skipped in data process 
long int Ngood[120];     // number of events after carbon cut (each bunch)
long int Ntotal[120];    // number of events before carbon cut (each bunch)
long int Nback[120];     // number of events below the curbon cut (each bunch)
int NFilledBunch=0;      // number of Filled Bunch

long int * pointer ;
int ndelim ;
long int VtgtLinear[MAXDELIM]; 
long int VtgtRotary[MAXDELIM];
long int HtgtLinear[MAXDELIM];
long int HtgtRotary[MAXDELIM];
int TgtIndex[MAXDELIM];
int nTgtIndex = 0 ;


#endif /* ASYM_MAIN_H */
