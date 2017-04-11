#include "AsymCommon.h"

#include <map>
#include <string>
#include <fstream>

#include "TMath.h"
#include "TLatex.h"

#include "AsymAnaInfo.h"
#include "MAsymAnaInfo.h"
#include "AsymCalculator.h"
#include "AsymRoot.h"
#include "RunConfig.h"
#include "TargetInfo.h"

using namespace std;

const int   ASYM_DEFAULT = -999;
const float MSIZE = 1.2;            // Marker size

int     wcmfillpat[120];            // spin pattern 120 bunches (ADO info)

long    Ncounts[6][120];            // counts 6detectors 120 bunches
long    NTcounts[6][120][NTBIN];    // counts 6detectors 120 bunches 6 tranges
long    NRcounts[6][120][RAMPTIME]; // counts for 6det 120bunch RAMPTIME sec
long    NDcounts[6][120][MAXDELIM]; // counts for 6 det 120 bunch per DELIMiter
long    NStrip[3][N_SILICON_CHANNELS];          // counts 72 strips 3 spin states

char   *calibdir;
char    conf_file[256];             // overwrite configuration file
char    CalibFile[256];             // energy calibration file

float   ramptshift[500];            // ramp timing shift

long    Ngood[120];     // number of events after carbon cut (each bunch)
long    Ntotal[120];    // number of events before carbon cut (each bunch)
long    Nback[120];     // number of events below the curbon cut (each bunch)

long   *pointer;
int     gNDelimeters;
int     TgtIndex[MAXDELIM];
int     nTgtIndex = 0;


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

StructFlag Flag = {
   0,       // VERBOSE mode
   0,       // feedback mode
   -1,      // spin_pattern
   -1,      // fill_pattern
   0,       // mask_bunch
   0,       // EXE_ANOMALY_CHECK;
};

StructCounter    cntr;
StructCounterTgt cntr_tgt;

/**
 * Following arrays correspond to phi angle of each strips. These phi angles are
 * subject to change depends on TOFLength from target to detectors.
 * To get phi values for given TOFLength, use calcPhi.cc program. Copy and paste outputs.
 * phi angle of each strips for Run05 (l=18.5cm)
 */
float phiRun5[N_SILICON_CHANNELS] = {
   0.72601,0.73679,0.74758,0.75838,0.76918,0.77999,0.79080,0.80161,0.81242,0.82322,0.83401,0.84479,
   1.51141,1.52219,1.53298,1.54378,1.55458,1.56539,1.57620,1.58701,1.59782,1.60862,1.61941,1.63019,
   2.29680,2.30758,2.31837,2.32917,2.33998,2.35079,2.36160,2.37241,2.38321,2.39401,2.40480,2.41558,
   3.86760,3.87838,3.88917,3.89997,3.91078,3.92159,3.93240,3.94321,3.95401,3.96481,3.97560,3.98638,
   4.65300,4.66378,4.67457,4.68537,4.69617,4.70698,4.71779,4.72860,4.73941,4.75021,4.76100,4.77178,
   5.43840,5.44918,5.45997,5.47077,5.48157,5.49238,5.50319,5.51400,5.52481,5.53561,5.54640,5.55718
};

// phi angle of each strips for Run06 (l=18.0cm)
float phiRun6[N_SILICON_CHANNELS] = {
   0.72436,0.73544,0.74653,0.75763,0.76873,0.77984,0.79095,0.80206,0.81317,0.82427,0.83536,0.84643,
   1.50976,1.52084,1.53193,1.54303,1.55413,1.56524,1.57635,1.58746,1.59857,1.60967,1.62075,1.63183,
   2.29516,2.30624,2.31733,2.32842,2.33953,2.35064,2.36175,2.37286,2.38397,2.39506,2.40615,2.41723,
   3.86596,3.87703,3.88812,3.89922,3.91033,3.92144,3.93255,3.94366,3.95476,3.96586,3.97695,3.98803,
   4.65135,4.66243,4.67352,4.68462,4.69572,4.70683,4.71794,4.72905,4.74016,4.75126,4.76235,4.77342,
   5.43675,5.44783,5.45892,5.47002,5.48112,5.49223,5.50334,5.51445,5.52556,5.53666,5.54775,5.55882
} ;

// phi angle of each strips in trancated angles {45,90,135,225,270,315} deg.
float phit[N_SILICON_CHANNELS] = {
   0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,
   1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,
   2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,
   3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,
   4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,
   5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779
};

// default phi is trancated angles phi[st]=phit[st]
float gPhi[N_SILICON_CHANNELS] = {
   0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,0.78540,
   1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,1.57080,
   2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,2.35619,
   3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,3.92699,
   4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,4.71239,
   5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779,5.49779
};

// chan.->WFD map Runs 11, 12, 17 (<11?)
// WFD#s in crates:
// upper crate 3 (blu) / 5 (yel):  1  2  3  4  5  6  7  8  9 19
// lower crate 4 (blu) / 6 (yel): 10 11 12 13 14 15 16 17 18 20
UShort_t ch2WfdMap_run11[80] = { 1, 10,  2, 11,  3, 12,  4, 13,  5, 14,  6, 15,
				 7, 16,  8, 17,  9, 18,  1, 10,  2, 11,  3, 12, 
				 4, 13,  5, 14,  6, 15,  7, 16,  8, 17,  9, 18,
				 1, 10,  2, 11,  3, 12,  4, 13,  5, 14,  6, 15,
				 7, 16,  8, 17,  9, 18,  1, 10,  2, 11,  3, 12,
				 4, 13,  5, 14,  6, 15,  7, 16,  8, 17,  9, 18,
                                19, 19, 19, 19,
                                20, 20, 20, 20 };

// chan.->WFD map Runs 13, 15
// WFD#s in crates:
// upper crate 3 (blu) / 5 (yel):  1  2  3  4  6  8 10 12 14 19
// lower crate 4 (blu) / 6 (yel):  5  7  9 11 13 15 16 17 18 20
UShort_t ch2WfdMap_run13[80] = { 1,  2,  3,  1,  2,  3,  1,  2,  3,  1,  2,  3,
                                 4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
                                 4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
                                 4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
                                 4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
                                16, 17, 18, 16, 17, 18, 16, 17, 18, 16, 17, 18,
                                19, 19, 19, 19,
                                20, 20, 20, 20 };


AsymAnaInfo              *gAsymAnaInfo;
MAsymAnaInfo             *gMAsymAnaInfo;
MeasInfo                 *gMeasInfo;
AnaMeasResult            *gAnaMeasResult;
AsymRoot                 *gAsymRoot;
UShort_t                 *gCh2WfdMap = 0;
RunConfig                 gRunConfig;
atdata_struct             atdata;
StructAverage             average;
StructFeedBack            feedback;
//RunConst                 runconst;
map<UShort_t, RunConst>   gRunConsts;
TRecordConfigRhicStruct  *gConfigInfo;
TargetInfo                tgt;
BunchAsym                 gBunchAsym;
StructSpeLumi             SpeLumi;
AsymCalculator            gAsymCalculator;
StructBunchCheck          bnchchk;
StructStripCheck          strpchk;
StructHistStat            gHstat;
StructHist                Eslope;


StructFeedBack::StructFeedBack()
{
   for (int i=0; i<N_SILICON_CHANNELS; i++) {
      RMS[i]  = CARBON_MASS_PEAK_SIGMA;
      mdev[i] = tedev[i] = err[i] = chi2[i] = strip[i] = 0;
   }
}


StructStripCounter::StructStripCounter()
{
   for (int i=0; i<N_SPIN_STATES; i++) {
      for (int j=0; j<N_SILICON_CHANNELS; j++) {
         NStrip[i][j] = 0;
      }
   }
}


StructStripCounterTgt::StructStripCounterTgt()
{
   for(int i=0; i<MAXDELIM; i++) {
      for (int j=0; j<N_SPIN_STATES; j++) {
         for (int k=0; k<N_SILICON_CHANNELS; k++)
            cntr_tgt.reg.NStrip[i][j][k] = 0;
      }
   }
}


StructCounter::StructCounter() : good_event(0), revolution(0), tgtMotion(0),
   reg(), alt(), phx(), str()
{
   for(int i=0; i<MAXDELIM; i++)
      good[i] = 0;
}


StructCounterTgt::StructCounterTgt() : good_event(0), revolution(0),
   tgtMotion(0), reg()
{
   for(int i=0; i<MAXDELIM; i++)
      good[i] = 0;
}


/** */
RunConst::RunConst(float lL, float lCt)
{
   L   = lL;
   Ct  = lCt;
   E2T = M_SQRT1_2 * sqrt(MASS_12C) * L / C_CMNS;
   M2T = L/2/M_SQRT2/C_CMNS/sqrt(MASS_12C);       // ~ 18 [cm] / 2 / 1.4142 / 29.98 [cm/ns] / 3.3447e3 [keV^(1/2)] = 0.00006347 [ns/keV^(1/2)]
   T2M = 2*C_CMNS*C_CMNS/L/L;
}


/** */
void RunConst::Update(UShort_t ch)
{
  if (!gConfigInfo) return;

   Ct = gConfigInfo->data.WFDTUnit/2.; // Determine the TDC count unit (ns/channel)

   if (ch >= 1 && ch <= gConfigInfo->data.NumChannels) {
      L = gConfigInfo->data.chan[ch-1].TOFLength; // ToF Distance [cm]
   } else {
      L = gConfigInfo->data.TOFLength; // ToF Distance [cm]
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
  printf("\tM2T = %10.3g\n", M2T);
  printf("\tT2M = %10.3f\n", T2M);
}


/** */
void RunConst::PrintAll()
{
   printf("Print gRunConsts:\n");

   map<UShort_t, RunConst>::iterator irc = gRunConsts.begin();

	for (; irc!=gRunConsts.end(); ++irc) {
      printf("Channel %-2d consts: \n", irc->first);
	   irc->second.Print();
   }

   printf("\n");
}


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


//std::string strip(std::string const& str, char const* sepSet)
//{
//   std::string::size_type const first = str.find_first_not_of(sepSet);
//   return ( first==std::string::npos ) ? std::string() : str.substr(first, str.find_last_not_of(sepSet)-first+1);
//}


// Return Maximum from array A[N]. Ignores ASYM_DEFAULT as an exception
float GetMax(int N, float A[])
{
   float max = A[0];// != ASYM_DEFAULT ? A[0] : A[1];
   for (int i=1; i<N; i++) max = (A[i] && max < A[i] && A[i] != ASYM_DEFAULT) ? A[i] : max;
   return max;
}


// Return Miminum from array A[N]. Ignores ASYM_DEFAULT as an exception
float GetMin(int N, float A[])
{
  float min = A[0];// != ASYM_DEFAULT ? A[0] : A[1];
  for (int i=1; i<N; i++) min = (A[i]) && (min>A[i]) && (A[i] != ASYM_DEFAULT) ? A[i] : min;
  return min;
}


/**
 * Return Minimum and Maximum from array A[N]
 *
 * If margin is not 0 the maximum is increased by max*margin and minimum is
 * decreased by min*margin
 */
void GetMinMax(int N, float A[], float margin, float &min, float &max)
{
   min  = GetMin(N, A);
   max  = GetMax(N, A);
   min -= fabs(min) * margin;
   max += fabs(max) * margin;
}


// Return Minimum and Maximum from array A[N]. Same as GetMinMax() function. But
// GetMinMaxOption takes prefix value which forces min, max to be prefix when the
// absolute min,max are smaller than prefix.
void GetMinMaxOption(float prefix, int N, float A[], float margin, float &min, float &max)
{
   GetMinMax(N, A, margin, min, max);
   if ( fabs(min) < prefix ) min = -prefix;
   if ( fabs(max) < prefix ) max =  prefix;
}


// Description : draw text on histogram. Text alignment is (center,top) by default
void DrawText(TH1 *h, float x, float y, int color, char *text)
{
   TLatex *t = new TLatex(x, y, text);
   t->SetTextColor(color);
   t->SetTextAlign(21);
   h->GetListOfFunctions()->Add(t);
}


// Description : DrawLines in TH2F histogram
//             : Assumes  (x1,x2) y=y0=y1
// Input       : TH2F * h, float x0, float x1, float y, int color, int lstyle
void DrawLine(TH1 *h, float x1, float y1, float x2, float y2, int color, int lstyle, int lwidth)
{
   TLine *l = new TLine(x1, y1, x2, y2);
   l->SetLineStyle(lstyle);
   l->SetLineColor(color);
   l->SetLineWidth(lwidth);
   h->GetListOfFunctions()->Add(l);
}


/** */
void DrawHorizLine(TH1 *h, float x1, float x2, float y, int color, int lstyle, int lwidth)
{
   DrawLine(h, x1, y, x2, y, color, lstyle, lwidth);
}


/** */
void DrawVertLine(TH1 *h, float x, float y, int color, int lwidth)
{
   DrawLine(h, x, 0, x, y, color, 2, lwidth);
}


// Description : print integer in binary with zero filled from the most significant bit
//             : to zero bit, f.i. 0101 for n=5, mb=4
// Input       : int n, int mb(the most significant bit)
// Return      : writes out n in binary
void binary_zero(int n, int mb)
{
   int X = int(pow(double(2), double(mb-1)));
 
   for (int i=0; i<mb; i++) {
     int j = n << i & X ? 1 : 0;
     cout << j ;
   }
}


// Description : Define net TGraphErrors object asymgraph for vectors x,y,ex,ey
//             : specifies marker color based on mode
//             : positive spin : blue
//             : negative spin : red
// Input       : int Mode, int N, float x[], float y[], float ex[], float ey[]
//
TGraphErrors* AsymmetryGraph(int Mode, int N, float* x, float* y, float* ex, float* ey)
{
  int Color = Mode == 1 ? kBlue : kRed;

  TGraphErrors *asymgraph = new TGraphErrors(N, x, y, ex, ey);

  asymgraph->SetMarkerStyle(kFullCircle);
  asymgraph->SetMarkerSize(MSIZE);
  asymgraph->SetMarkerColor(Color);

  return asymgraph;
}


// Read the parameter file
// Ramp timing file
void ReadRampTiming(char *filename)
{
   printf("\nReading ramp timing file : %s \n", filename);

   ifstream rtiming;
   rtiming.open(filename);

   if (!rtiming) {
      cerr << "failed to open ramp timing file" <<endl;
      exit(1);
   }

   memset(ramptshift, 0, sizeof(ramptshift));

   float runt;
   int   index = 0;

   while (!rtiming.eof()) {
      rtiming >> runt >> ramptshift[index] ;
      index++;
   }

   rtiming.close();
}


/** */
ostream& operator<<(ostream &os, const TgtOrient2ValErrMap &vep)
{
   //os << "array( " << vep.first << ", " << vep.second << " )";
   os << MapAsPhpArray<ETargetOrient, ValErrPair>(vep);

   return os;
}


/** */
string PairAsPhpArray(const ValErrPair &p)
{
   std::stringstream sstr("");

   sstr << p;
   //sstr << "array (" << p.first << ", " << p.second << ")";

   return sstr.str();
}
