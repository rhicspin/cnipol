#include "Asym.h"

using namespace std;

// Default Values for Run Condition
TDatprocStruct dproc = {
   400, 900,   // energy
   -30, 30,    // banana cut width (Constant mode only)
   0,          // FEEDBACKMODE
   0,          // RAWHISTOGRAM
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
   0,          // SAVETREES
   3,          // MassSigma banana curve cut within <MassSigma> away from the 12C mass
   2,          // MassSigma banana curve alternative cut within <MassSigmaAlt> away from the 12C mass
   1.5e6,      // 1-sigma of 12C mass peak => 1.5e6 [keV]
   0,          // tshift: Time shift in [ns]
   0,          // Time shift in [ns] for injection w.r.t. flattop
   0,          // additional deadlayer offset [ug/cm2]
   999.05,     // Wall Current Monitor process Fill range +/-5[%]
   8,          // Lower Mass limit for peak position adjustment fit default :8 GeV
   0,          // nEventsProcessed
   0,          // nEventsTotal
   1,          // <thinout> event rate to be feed into feedback routine
   1,          // Expected universal rate for given target
   0.11,       // Target count/mm conversion
   0,          // procDateTime date and time
   0,          // procTimeReal
   0,          // procTimeCpu
   ""          // userCalibFile
};

StructMask mask = {
  0x3F          // detector mask 0x3F={11 1111} All detector active
};

ErrorDetector errdet = {
  0.10,         // MASS_WIDTH_DEV_ALLOWANCE [GeV]
  0.50,         // MASS_POSITION_ALLOWANCE [GeV]
  100,          // MASS_CHI2_ALLOWANCE [GeV]
  0.001,        // MASS_ENERGY_CORR_ALLOWANCE; [GeV/keV]
  0.2,          // GOOD_CARBON_EVENTS_ALLOWANCE fabs(events[st]-average)/average<GOOD_CARBON_EVENTS_ALLOWANCE
  5.,           // BUNCH_RATE_SIGMA_ALLOWANCE;
  5.,           // BUNCH_ASYM_SIGMA_ALLOWANCE;
  20            // NBUNCH_REQUIREMENT;
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
  0,       // VERBOSE mode
  0,       // feedback mode
  -1,      // spin_pattern
  -1,      // fill_pattern
  0,       // mask_bunch
  1,       // EXE_ANOMALY_CHECK;
};

StructCounter cntr = {
  0,    // good_event;
  0,    // revolution number
  0     // taret motion entries
};

StructCounterTgt cntr_tgt = {
  0,    // good_event;
  0,    // revolution number
  0     // taret motion entries
};


TStructRunInfo::TStructRunInfo() : Run(0), RUNID(0.0), runName(100, ' ')
{
   //Run                   = 6;  // Run05, Run06,..
   //RUNID                 = 7279.005; // RUNID
   //runName;
   StartTime             = 0; // StartTime;
   StopTime              = 0; // StopTime;
   RunTime               = 0; // RunTime;
   GoodEventRate         = 0; // GoodEventRate;
   EvntRate              = 0; // EvntRate;
   ReadRate              = 0; // ReadRate;
   WcmAve                = 0; // WcmAve;
   WcmSum                = 0; // WcmSum;
   BeamEnergy            = 0; // BeamEnergy;
   RHICBeam              = 0; // RHICBeam;
   PolarimetryID         = 1; // PolarimetryID; Polarimetry-1 or Polarimetry-2
   MaxRevolution         = 0; // MaxRevolution;
   target                = 'V'; // target
   targetID              = '-'; // targetID
   //TgtOperation;              // TgtOperation (Initialization is done in Initialization() )
   for (int i=0; i<NDETECTOR; i++) ActiveDetector[i] = 0xFFF;
   //ActiveDetector        = { 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF };// ActiveDetector[NDETECTOR]
   for (int i=0; i<NSTRIP; i++) { ActiveStrip[i] = 1; DisableStrip[i] = 0; }
   //ActiveStrip           = { 1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,
   //                          1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1, 
   //                          1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1 }; // ActiveStrip[NSTRIP]
   NActiveStrip          = NSTRIP; // NAactiveStrip;
   NDisableStrip         = 0; // NDisableStrip
   //DisableStrip          = { 0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,
   //                          0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0, 
   //                          0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0 }; // DisableStrip[NSTRIP]
   NFilledBunch          = 0; // NFilledBunch;
   NActiveBunch          = 0; // NActiveBunch;
   NDisableBunch         = 0; // NDisableBunch,
   for (int i=0; i<NBUNCH; i++) { DisableBunch[i] = 0; }
   //DisableBunch[NBUNCH]
}

TStructRunInfo::~TStructRunInfo()
{
}

TStructRunInfo runinfo;

/*
TStructRunInfo runinfo = {
    6, // Run05, Run06,..
    7279.005, // RUNID
    //"                            ",//(string(" ", 100)),
    0, // StartTime;
    0, // StopTime;
    0, // RunTime;
    0, // GoodEventRate;
    0, // EvntRate;
    0, // ReadRate;
    0, // WcmAve;
    0, // WcmSum;
    0, // BeamEnergy;
    0, // RHICBeam;
    1, // PolarimetryID; Polarimetry-1 or Polarimetry-2
    0, // MaxRevolution;
  'V', // target
  '-',// targetID
    "", // TgtOperation (Initialization is done in Initialization() )
    {  // ActiveDetector[NDETECTOR]
      0xFFF,0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF
    },
    {  // ActiveStrip[NSTRIP]
      1,1,1,1,1,1,1,1,1,1,1,1,
      1,1,1,1,1,1,1,1,1,1,1,1,
      1,1,1,1,1,1,1,1,1,1,1,1,
      1,1,1,1,1,1,1,1,1,1,1,1,
      1,1,1,1,1,1,1,1,1,1,1,1,
      1,1,1,1,1,1,1,1,1,1,1,1
    },
    NSTRIP, // NAactiveStrip;
    0, // NDisableStrip
    {  // DisableStrip[NSTRIP]
      0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0
    },
    0, // NFilledBunch;
    0, // NActiveBunch;
    0  // NDisableBunch,
};
*/

StructTarget tgt = {
    0,  // target position x
    -1, // target motion sign
    0,  // eventID
    0   // Vertical:[0], Horizontal:[1]
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

atdata_struct            atdata;
StructAverage            average;
StructFeedBack           feedback;
StructRunConst           runconst;
StructAnalysis           anal;
StructBunchPattern       phx, str;
recordConfigRhicStruct  *cfginfo;

map<string, string> gAsymEnv;

int spinpat[120]; // spin pattern 120 bunches (ADO info)
int fillpat[120]; // spin pattern 120 bunches (ADO info)
int ActiveBunch[120]; // spin pattern 120 bunches (ADO info)
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

long int Nevcut         = 0; // number of events after 1st cut (whole data)
UInt_t   Nevtot         = 0; // number of total events (whole data)
UInt_t   Nread          = 0; // real total events (completely everything)
UInt_t   gMaxEventsUser = 0; // number of events to process
//UInt_t   Nskip          = 1; // number of events to be skipped in data process
long int Ngood[120];     // number of events after carbon cut (each bunch)
long int Ntotal[120];    // number of events before carbon cut (each bunch)
long int Nback[120];     // number of events below the curbon cut (each bunch)
int NFilledBunch   = 0;  // number of Filled Bunch

long int * pointer ;
int ndelim ;
//long int VtgtLinear[MAXDELIM];
//long int VtgtRotary[MAXDELIM];
//long int HtgtLinear[MAXDELIM];
//long int HtgtRotary[MAXDELIM];
int TgtIndex[MAXDELIM];
int nTgtIndex = 0 ;


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
   free(rec);
   rec = (TDatprocStruct *) realloc(rec, sizeof(TDatprocStruct));
   rec->Streamer(buf);
   return buf;
}


/** */
void TDatprocStruct::Streamer(TBuffer &buf)
{
   if (buf.IsReading()) {
      //printf("reading TDatprocStruct::Streamer(TBuffer &buf) \n");
      buf >> enel;
      buf >> eneu;
      buf >> widthl;
      buf >> widthu;
      buf >> CMODE;
      buf >> nEventsProcessed;
      buf >> nEventsTotal;
      buf >> thinout;
      buf >> procDateTime >> procTimeReal >> procTimeCpu;
   } else {
      //printf("writing TDatprocStruct::Streamer(TBuffer &buf) \n");
      buf << enel;
      buf << eneu;
      buf << widthl;
      buf << widthu;
      buf << CMODE;
      buf << nEventsProcessed;
      buf << nEventsTotal;
      buf << thinout;
      buf << procDateTime << procTimeReal << procTimeCpu;
   }
}


/** */
TBuffer & operator<<(TBuffer &buf, TStructRunInfo *&rec)
{
   if (!rec) return buf;
   //printf("operator<<(TBuffer &buf, TStructRunInfo *rec) : \n");
   rec->Streamer(buf);
   return buf;
}


/**
 *
 */
TBuffer & operator>>(TBuffer &buf, TStructRunInfo *&rec)
{
   //printf("operator>>(TBuffer &buf, TStructRunInfo *rec) : \n");

   //if (!rec) {
   //   printf("ERROR in operator>>\n");
   //   exit(-1);
   //}

   // if object has been created already delete it
   //free(rec);

   //rec = (TStructRunInfo *) realloc(rec, sizeof(TStructRunInfo) + 11*sizeof(char));
   //rec = (TStructRunInfo *) realloc(rec, sizeof(TStructRunInfo));
   //rec->TgtOperation = new char[10];

   rec->Streamer(buf);
   return buf;
}


void TStructRunInfo::Streamer(TBuffer &buf)
{
   if (buf.IsReading()) {
      //printf("reading TStructRunInfo::Streamer(TBuffer &buf) \n");
      buf >> Run;
      buf >> RUNID;
      TString tstr;
      buf >> tstr; runName = tstr.Data();
      buf >> StartTime;
      buf >> StopTime;
      buf >> RunTime;
      buf >> GoodEventRate;
      buf >> EvntRate;
      buf >> ReadRate;
      buf >> WcmAve;
      buf >> WcmSum;
      buf >> BeamEnergy;
      buf >> RHICBeam;
      buf >> PolarimetryID;
      buf >> MaxRevolution;
      buf >> target;
      buf >> targetID;
      buf >> TgtOperation;
      //buf.ReadString(TgtOperation, 32);
      buf.ReadFastArray(ActiveDetector, NDETECTOR);
      buf.ReadFastArray(ActiveStrip, NSTRIP);
      buf >> NActiveStrip;
      buf >> NDisableStrip;
      buf.ReadFastArray(DisableStrip, NSTRIP);
      buf >> NFilledBunch;
      buf >> NActiveBunch;
      buf >> NDisableBunch;
      buf.ReadFastArray(DisableBunch, NBUNCH);

   } else {
      //printf("writing TStructRunInfo::Streamer(TBuffer &buf) \n");
      buf << Run;
      buf << RUNID;
      TString tstr = runName;
      buf << tstr;
      buf << StartTime;
      buf << StopTime;
      buf << RunTime;
      buf << GoodEventRate;
      buf << EvntRate;
      buf << ReadRate;
      buf << WcmAve;
      buf << WcmSum;
      buf << BeamEnergy;
      buf << RHICBeam;
      buf << PolarimetryID;
      buf << MaxRevolution;
      buf << target;
      buf << targetID;
      buf << TgtOperation;
      buf.WriteFastArray(ActiveDetector, NDETECTOR);
      buf.WriteFastArray(ActiveStrip, NSTRIP);
      buf << NActiveStrip;
      buf << NDisableStrip;
      buf.WriteFastArray(DisableStrip, NSTRIP);
      buf << NFilledBunch;
      buf << NActiveBunch;
      buf << NDisableBunch;
      buf.WriteFastArray(DisableBunch, NBUNCH);
   }
}


/**
 *
 */
TBuffer & operator<<(TBuffer &buf, TRecordConfigRhicStruct *&rec)
{
   if (!rec) return buf;
   //printf("operator<<(TBuffer &buf, TRecordConfigRhicStruct *rec) : \n");
   rec->Streamer(buf);
   return buf;
}


/**
 *
 */
TBuffer & operator>>(TBuffer &buf, TRecordConfigRhicStruct *&rec)
{
   //printf("operator>>(TBuffer &buf, TRecordConfigRhicStruct *rec) : \n");
   //if (!rec) return buf; // of course it is 0!
   int nChannels;
   buf >> nChannels;
   //printf("nChannels: %d\n", nChannels);
   // delete memory allocated by default if any
   //delete rec;
   free(rec);
   rec = (TRecordConfigRhicStruct *) realloc(rec, sizeof(TRecordConfigRhicStruct) +
   //rec = (TRecordConfigRhicStruct *) malloc(sizeof(TRecordConfigRhicStruct) +
         (nChannels)*sizeof(SiChanStruct));
   rec->Streamer(buf);

   return buf;
}


//TBuffer & operator<<(TBuffer &buf, recordHeaderStruct &rec)
//{
//   printf("operator<<(TBuffer &buf, recordHeaderStruct *rec) : \n");
//   buf << rec.len;
//   return buf;
//}
//
//
//TBuffer & operator>>(TBuffer &buf, recordHeaderStruct &rec)
//{
//   printf("operator>>(TBuffer &buf, recordHeaderStruct *rec) : \n");
//   buf >> rec.len;
//   //buf.WriteInt(rec.len);
//   return buf;
//}



//TBuffer & operator<<(TBuffer &buf, SiChanStruct &si)
//{
//   return buf;
//}


//______________________________________________________________________________
void TRecordConfigRhicStruct::Streamer(TBuffer &buf)
{
   if (buf.IsReading()) {
      //printf("reading TRecordConfigRhicStruct::Streamer(TBuffer &buf) \n");
      //if (!this) { printf("reading error: this=0 \n"); exit(-1); }
      buf >> header.len;
      buf >> data.WFDTUnit;
      //Int_t = tmp;
      //buf >> tmp; data.CSR.split = tmp
      buf >> data.CSR.WFDMode;
      //printf("data.CSR: %#x, %#x, %#x, %#x, %#x\n", data.CSR.WFDMode, data.CSR.reg,
      //       data.CSR.split.iDiv, data.CSR.split.Mode, (data.CSR.WFDMode&0x0C));
      buf >> data.NumChannels;
      // Now we know the number of channels saved;
      for (int i=0; i!=data.NumChannels; i++) {
         buf >> data.chan[i].t0;
         buf >> data.chan[i].ecoef;
         buf >> data.chan[i].edead;
         buf >> data.chan[i].A0;
         buf >> data.chan[i].A1;
         buf >> data.chan[i].acoef;
         buf >> data.chan[i].dwidth;
      }
   } else {
      //printf("writing TRecordConfigRhicStruct::Streamer(TBuffer &buf) \n");
      //if (!this) { printf("error this=0 \n"); exit(-1); }
      // this one has to go first so we know how much memmory to allocate when reading
      buf << data.NumChannels;

      buf << header.len ;
      buf << data.WFDTUnit ;
      buf << data.CSR.WFDMode; // split, WFDMode, reg - union
      //printf("data.CSR: %#x, %#x, %#x, %#x, %#x\n", data.CSR.WFDMode, data.CSR.reg,
      //       data.CSR.split.iDiv, data.CSR.split.Mode, (data.CSR.WFDMode&0x0C));
      buf << data.NumChannels;
      for (int i=0; i!=data.NumChannels; i++) {
         buf << data.chan[i].t0;
         buf << data.chan[i].ecoef;
         buf << data.chan[i].edead;
         buf << data.chan[i].A0;
         buf << data.chan[i].A1;
         buf << data.chan[i].acoef;
         buf << data.chan[i].dwidth;
      }
   }
}


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


void TStructRunDB::Print(const Option_t* opt) const
{
   printf("RunID:             %f\n", RunID);
   printf("isCalibRun:        %c\n", isCalibRun);
   printf("calib_file_s:      %s\n", calib_file_s.c_str());
   printf("alpha_calib_run_name: %s\n", alpha_calib_run_name.c_str());
   printf("config_file_s:     %s\n", config_file_s.c_str());
   printf("masscut_s:         %s\n", masscut_s.c_str());
   printf("comment_s:         %s\n", comment_s.c_str());
   //cout << "RunID: " <<
}
