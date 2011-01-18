
#include "Asym.h"
#include "AsymCalc.h"
#include "AsymRoot.h"
#include "AsymRunDB.h"


using namespace std;


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
  1,       // VERBOSE mode
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
float phiRun5[NSTRIP] = {
   0.72601,0.73679,0.74758,0.75838,0.76918,0.77999,0.79080,0.80161,0.81242,0.82322,0.83401,0.84479,
   1.51141,1.52219,1.53298,1.54378,1.55458,1.56539,1.57620,1.58701,1.59782,1.60862,1.61941,1.63019,
   2.29680,2.30758,2.31837,2.32917,2.33998,2.35079,2.36160,2.37241,2.38321,2.39401,2.40480,2.41558,
   3.86760,3.87838,3.88917,3.89997,3.91078,3.92159,3.93240,3.94321,3.95401,3.96481,3.97560,3.98638,
   4.65300,4.66378,4.67457,4.68537,4.69617,4.70698,4.71779,4.72860,4.73941,4.75021,4.76100,4.77178,
   5.43840,5.44918,5.45997,5.47077,5.48157,5.49238,5.50319,5.51400,5.52481,5.53561,5.54640,5.55718
};

// phi angle of each strips for Run06 (l=18.0cm)
float phiRun6[NSTRIP] = {
   0.72436,0.73544,0.74653,0.75763,0.76873,0.77984,0.79095,0.80206,0.81317,0.82427,0.83536,0.84643,
   1.50976,1.52084,1.53193,1.54303,1.55413,1.56524,1.57635,1.58746,1.59857,1.60967,1.62075,1.63183,
   2.29516,2.30624,2.31733,2.32842,2.33953,2.35064,2.36175,2.37286,2.38397,2.39506,2.40615,2.41723,
   3.86596,3.87703,3.88812,3.89922,3.91033,3.92144,3.93255,3.94366,3.95476,3.96586,3.97695,3.98803,
   4.65135,4.66243,4.67352,4.68462,4.69572,4.70683,4.71794,4.72905,4.74016,4.75126,4.76235,4.77342,
   5.43675,5.44783,5.45892,5.47002,5.48112,5.49223,5.50334,5.51445,5.52556,5.53666,5.54775,5.55882
} ;

// phi angle of each strips in trancated angles {45,90,135,225,270,315} deg.
float phit[NSTRIP] = {
   0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,
   1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,
   2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,
   3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,
   4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,
   5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779
};

// default phi is trancated angles phi[st]=phit[st]
float phi[NSTRIP] = {
   0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,
   1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,
   2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,
   3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,
   4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,
   5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779
};

AsymRoot                 gAsymRoot;
AsymRunDB                gAsymRunDb;
//TStructRunDB             rundb;
TStructRunDB             gRunDb_tmp;
TStructRunDB            &gRunDb = gRunDb_tmp;
//TStructRunDB             gCurrentRunInfo;
TStructRunInfo           runinfo;
TStructRunInfo          &gRunInfo = runinfo;
TDatprocStruct           dproc;
atdata_struct            atdata;
StructAverage            average;
StructFeedBack           feedback;
//RunConst                 runconst;
map<UShort_t, RunConst>   gRunConsts;
StructAnalysis           anal;
StructBunchPattern       phx, str;
recordConfigRhicStruct  *cfginfo;


/** */
RunConst::RunConst(float lL, float lCt)
{
   L   = lL;
   Ct  = lCt;
   E2T = M_SQRT1_2/C_CMNS * sqrt(MASS_12C) * L;
   M2T = L/2/M_SQRT2/C_CMNS/sqrt(MASS_12C);
   T2M = 2*C_CMNS*C_CMNS/L/L;
}


/** */
void RunConst::Update(recordConfigRhicStruct *cfginfo, UShort_t ch)
{
  if (!cfginfo) return;

   Ct = cfginfo->data.WFDTUnit/2.; // Determine the TDC count unit (ns/channel)

   if (ch >= 1 && ch <= cfginfo->data.NumChannels) {
      L = cfginfo->data.chan[ch-1].TOFLength; // ToF Distance [cm]
   } else {
      L = cfginfo->data.TOFLength; // ToF Distance [cm]
   }

  // C_CMNS - speed of light

  // Re-calculate kinematic constants for the Run
  E2T = M_SQRT1_2/C_CMNS * sqrt(MASS_12C) * L;
  M2T = L/2/M_SQRT2/C_CMNS/sqrt(MASS_12C);
  T2M = 2*C_CMNS*C_CMNS/L/L;
}


/** */
void RunConst::Print(const Option_t* opt) const
{
  //printf("\nKinematic Constants:\n");
  printf("\tL   = %10.3f\n", L);
  printf("\tCt  = %10.3f\n", Ct);
  printf("\tE2T = %10.3f\n", E2T);
  printf("\tM2T = %10.3f\n", M2T);
  printf("\tT2M = %10.3f\n", T2M);
}


map<string, string>      gAsymEnv;

BunchAsym     basym;
StructSpeLumi SpeLumi;

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
std::string gDataFileName;   // data file name
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

const float MSIZE = 1.2; // Marker size

StructHist Eslope;


/** */
TBuffer & operator<<(TBuffer &buf, TRecordConfigRhicStruct *&rec)
{
   if (!rec) return buf;
   //printf("operator<<(TBuffer &buf, TRecordConfigRhicStruct *rec) : \n");
   rec->Streamer(buf);
   return buf;
}


/** */
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
void TRecordConfigRhicStruct::Print(const Option_t* opt) const
{
   long len = header.len;
   int nRecords  = (header.len - sizeof(recordHeaderStruct)) / sizeof(configRhicDataStruct);
   int nChannels = data.NumChannels;

   printf("nChannels: %d %d\n", nChannels, nRecords);

   //for (int i=0; i!=nRecords; i++) {
   for (int i=0; i!=nChannels; i++) {
      printf("%02d, acoef: %f\n", i, data.chan[i].acoef);
   }
}


//std::string strip(std::string const& str, char const* sepSet)
//{
//   std::string::size_type const first = str.find_first_not_of(sepSet);
//   return ( first==std::string::npos ) ? std::string() : str.substr(first, str.find_last_not_of(sepSet)-first+1);
//}
