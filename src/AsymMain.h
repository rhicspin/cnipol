#ifndef ASYM_MAIN_H
#define ASYM_MAIN_H

// Default Values for Run Condition
datprocStruct dproc = { 
    400,900,    // energy
    -30,30,     // banana cut width (Constant mode only)
    0,          // FEEDBACKMODE
    0,          // CMODE
    0,          // DMODE
    0,          // TMODE
    0,          // AMODE
    1,          // BMODE
    0,          // ZMODE
    0,          // MESSAGE
    2,          // CBANANA
    0,          // UPDATE
    1,          // MMODE
    0,          // NTMODE
    1,          // RECONFMODE
    0,          // RAMPMODE
    0,          // STUDYMODE
    3,          // MassSigma banana curve cut within <MassSigma> away from the 12C mass
    1.5e6,      // 1-sigma of 12C mass peak => 1.5e6 [keV] 
    0,          // Time shift in [ns]
    0,          // Time shift in [ns] for injection w.r.t. flattop
    999.05,     // Wall Current Monitor process Fill range +/-5[%] 
    8,          // Lower Mass limit for peak position adjustment fit default :8 GeV
    100,        // <thinout> event rate to be feed into feedback routine
};  // data process modes 


ErrorDetector errdet = {
  0.10,         // MASS_DEV_ALLOWANCE [GeV]
  100,          // MASS_CHI2_ALLOWANCE [GeV]
  0.001,        // MASS_ENERGY_CORR_ALLOWANCE; [GeV/keV]
  5.,           // BUNCH_RATE_SIGMA_ALLOWANCE;
  1.            // BUNCH_ASYM_SIGMA_ALLOWANCE;
};



StructExtInput extinput = {
  0, // CONFIG
  0, // MASSCUT
  0  // TSHIFT
} ;

StructReadFlag ReadFlag = {
  0, // RECBEGIN
  0, // PCTARGET 
  0, // WCMADO
  0, // READADO
  0  // RHICCONF
};

StructFlag Flag = {
  1,       // VERBOSE mode
  0,       // feedback mode
  -1,      // spin_pattern
  -1,      // fill_pattern
  0        // mask_bunch
};

StructCounter cntr = {
    0,    // revolution number
    0     // taret motion entries
};
	 

StructRunInfo runinfo = {
    6, // Run05, Run06,..
    7279.005, // RUNID
    0, // StartTime;
    0, // StopTime;
    0, // RunTime;
    0, // EvntRate;
    0, // ReadRate;
    0, // WcmAve;
    0, // BeamEnergy;
    0, // RHICBeam;
    0, // MaxRevolution;
  'V', // target
    0, // TgtOperation (Initialization is done in Initialization() )
    0, // NDisableStrip
    {  // DisableStrip[NSTRIP]
      0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0
    }
}; 

StructTarget tgt = {
    0,  // target position x
    -1, // target motion sign
    0   // eventID
};




// Following arrays correspond to phi agnle of each strips. These phi angles are
// subject to change depends on TOFLength from target to detectors.
// To get phi values for given TOFLength, use calcPhi.cc program. Copy and paste outputs.
// phi angle of each strips for Run05 (l=18.5cm) 
float phiRun5[NSTRIP]={
0.72601,0.73679,0.74758,0.75838,0.76918,0.77999,0.79080,0.80161,0.81242,0.82322,0.83401,0.84479,
1.51141,1.52219,1.53298,1.54378,1.55458,1.56539,1.57620,1.58701,1.59782,1.60862,1.61941,1.63019,
2.29680,2.30758,2.31837,2.32917,2.33998,2.35079,2.36160,2.37241,2.38321,2.39401,2.40480,2.41558,
3.86760,3.87838,3.88917,3.89997,3.91078,3.92159,3.93240,3.94321,3.95401,3.96481,3.97560,3.98638,
4.65300,4.66378,4.67457,4.68537,4.69617,4.70698,4.71779,4.72860,4.73941,4.75021,4.76100,4.77178,
5.43840,5.44918,5.45997,5.47077,5.48157,5.49238,5.50319,5.51400,5.52481,5.53561,5.54640,5.55718
} ;

// phi angle of each strips for Run06 (l=18.0cm) 
float phiRun6[NSTRIP]={
0.72436,0.73544,0.74653,0.75763,0.76873,0.77984,0.79095,0.80206,0.81317,0.82427,0.83536,0.84643,
1.50976,1.52084,1.53193,1.54303,1.55413,1.56524,1.57635,1.58746,1.59857,1.60967,1.62075,1.63183,
2.29516,2.30624,2.31733,2.32842,2.33953,2.35064,2.36175,2.37286,2.38397,2.39506,2.40615,2.41723,
3.86596,3.87703,3.88812,3.89922,3.91033,3.92144,3.93255,3.94366,3.95476,3.96586,3.97695,3.98803,
4.65135,4.66243,4.67352,4.68462,4.69572,4.70683,4.71794,4.72905,4.74016,4.75126,4.76235,4.77342,
5.43675,5.44783,5.45892,5.47002,5.48112,5.49223,5.50334,5.51445,5.52556,5.53666,5.54775,5.55882
} ;

// phi angle of each strips in trancated angles {45,90,135,225,270,315} deg.
float phit[NSTRIP]={
0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,
1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,
2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,
3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,
4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,
5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779
} ;

// default phi is trancated angles phi[st]=phit[st]
float phi[NSTRIP]={
0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,
1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,
2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,
3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,
4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,
5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779
} ;





atdata_struct atdata;
StructAverage average;
StructFeedBack feedback;
StructRunConst runconst;
StructAnalysis anal;


int spinpat[120]; // spin pattern 120 bunches (ADO info)
int fillpat[120]; // spin pattern 120 bunches (ADO info)
int wcmfillpat[120]; // spin pattern 120 bunches (ADO info)
float wcmdist[120];  // wall current monitor 120 bunches (ADO info)

long int Ncounts[6][120]; // counts 6detectors 120 bunches
long int NTcounts[6][120][NTBIN];  // counts 6detectors 120 bunches 6 tranges
long int NRcounts[6][120][RAMPTIME]; // counts for 6det 120bunch RAMPTIME sec
long int NDcounts[6][120][MAXDELIM]; // counts for 6 det 120 bunch per DELIMiter  
long int NStrip[3][NSTRIP]; // counts 72 strips 3 spin states

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
//long int VtgtLinear[MAXDELIM]; 
//long int VtgtRotary[MAXDELIM];
//long int HtgtLinear[MAXDELIM];
//long int HtgtRotary[MAXDELIM];
int TgtIndex[MAXDELIM];
int nTgtIndex = 0 ;


#endif /* ASYM_MAIN_H */
