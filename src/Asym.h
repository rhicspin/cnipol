// Header file for Asym
// Author   : Itaru Nakagawa
// Creation : 11/18/2005         

/**
 *
 * 15 Oct, 2010 - Dmitri Smirnov
 *    - Modified readloop to take an object of Root class as an argument 
 *
 */

#ifndef Asym_h
#define Asym_h

#include <map>
#include <set>
#include <string>
#include <iostream>
#include <iterator>
#include <sstream>

#include "TH1.h"
#include "TGraphErrors.h"

#include "rpoldata.h"

#include "AsymHeader.h"


typedef std::map<std::string, std::string> Str2StrMap;

// whole info for one event
struct processEvent {
   int  stN;   // strip number
   int  amp;   // amplitude of signal
   int  tdc;   // time of flight
   int  tdcmax;  // tdc max amplitude
   int  intg;  // integral of signal
   int  bid;   // bunch crossing number
   long delim;
   int  rev0;
   int  rev;
};

struct asymStruct {
    float phys;         // physics asymmetry
    float physE;        // physics asymmetry error
    float acpt;         // acceptance asymmetry
    float acptE;        // acceptance asymmetry error
    float lumi;         // luminosity asymmetry
    float lumiE;        // luminosity asymmetry error
};

struct ErrorDetector {
  float MASS_WIDTH_DEV_ALLOWANCE;
  float MASS_POSITION_ALLOWANCE;
  float MASS_CHI2_ALLOWANCE;
  float MASS_ENERGY_CORR_ALLOWANCE;
  float GOOD_CARBON_EVENTS_ALLOWANCE;
  float BUNCH_RATE_SIGMA_ALLOWANCE;
  float BUNCH_ASYM_SIGMA_ALLOWANCE;
  int   NBUNCH_REQUIREMENT;
};

struct atdata_struct {
   long ia;
   long it;
   long is;
   long ib;
   long id;
   int strip;
   float e;
   float tof;
   int spin;
};

class RunConst
{
public:
   float L;     // Distance from target to detector
   float Ct;    // TDC Count unit (ns/channel)
   float E2T;   // kinetic energy -> tof
   float M2T;   // Mass->tof     [ns/keV^(1/2)]
   float T2M;   // ToF -> Mass

   RunConst(float lL=CARBON_PATH_DISTANCE, float lCt=WFD_TIME_UNIT_HALF);
   void Update(UShort_t ch=0);
   void Print(const Option_t* opt="") const;
   static Bool_t IsSiliconChannel(UShort_t chId);
};


struct StructMask {
   int detector;
};

struct StructAnomaly {
   int nstrip;
   int st[N_CHANNELS];
   float bad_st_rate;
   int strip_err_code;
   int nbunch;
   int bunch[N_BUNCHES];
   float bad_bunch_rate;
   int bunch_err_code;
};

struct StructUnrecognized {
   StructAnomaly anomaly;
};

struct StructSinPhi {
   float P[2];
   float dPhi[2];
   float chi2;

   StructSinPhi() { P[0] = 0; P[1] = 0; dPhi[0] = 0; dPhi[1] = 0; chi2 = 0; }

   void  Streamer(TBuffer &buf) {
      if (buf.IsReading()) {
         buf >> P[0] >> P[1];
      } else {
         buf << P[0] << P[1];
      }
   }
};

struct StructExtInput {
  int CONFIG;
  int MASSCUT;
  int TSHIFT;
};

struct StructAverage {
  float total;
  float average;
  int counter;
};

struct StructHistStat {
  float mean;
  float RMS;
  float RMSnorm;
  float maxbin;
};

struct StructFeedBack {
   float RMS[N_CHANNELS];
   float mdev[N_CHANNELS];
   float tedev[N_CHANNELS];
   float err[N_CHANNELS];
   float chi2[N_CHANNELS];
   float strip[N_CHANNELS];

   StructFeedBack();
};

struct StructReadFlag {
  int RECBEGIN;
  int PCTARGET;
  int WCMADO;
  int BEAMADO;
  int RHICCONF;

  StructReadFlag() : RECBEGIN(0), PCTARGET(0), WCMADO(0), BEAMADO(0), RHICCONF(0) {}
};

struct StructFlag {
  int VERBOSE;
  int feedback;
  int spin_pattern;
  int fill_pattern;
  int mask_bunch;
  int EXE_ANOMALY_CHECK;
};

struct StructStripCounter
{
   long int NStrip[NUM_SPIN_STATES][N_CHANNELS];   // strip counters for 3 different spin states

   StructStripCounter();
};

struct StructStripCounterTgt
{
   long int NStrip[MAXDELIM][NUM_SPIN_STATES][N_CHANNELS]; // strip counters for 3 different spin states

   StructStripCounterTgt();
};

struct StructCounter {
   long int good_event;
   long int revolution; // revolution number
   long int tgtMotion;  // target motion entries
   long int good[MAXDELIM];
   StructStripCounter reg, alt, phx, str;

   StructCounter();
};

struct StructCounterTgt {
   long int good_event; 
   long int revolution;
   long int tgtMotion;
   long int good[MAXDELIM];
   StructStripCounterTgt reg;

   StructCounterTgt();
};

//struct StructTarget {
//   float  x;                    // (arbitarary) target postion [mm]
//   int    vector;
//   long   eventID;
//   int    VHtarget;             // Vertical:[0], Horizontal:[1]
//   int    Index[MAXDELIM];
//   int    Linear[MAXDELIM][2];   
//   int    Rotary[MAXDELIM][2];
//   float  X[MAXDELIM];          // target position [mm] array excluding static position 
//   float  Interval[MAXDELIM];   // time interval of given target postiion [sec]
//   float  Time[MAXDELIM];       // duration from measurement start in [sec]
//   struct StructAll {
//      float x[TARGETINDEX];    // target position in [mm] including static position
//   } all;
//};

struct StructBunchPattern {
  int bunchpat[N_BUNCHES];
};

struct StructHist {
  int   nxbin;
  float xmin;
  float xmax;
  int   nybin;
  float ymin;
  float ymax;
};

// Bunch by Bunch
struct BunchAsym { // array[0]:asymmetry, array[1]:asymmetry error
   float Ax90[2][N_BUNCHES];
   float Ax45[2][N_BUNCHES];
   float Ay45[2][N_BUNCHES];
   struct Ave {
      float Ax90[2];  // bunch averaged asymmetry 
      float Ax45[2];
      float Ay45[2];
      float Ax[2];   // left-right average asymmetry 
      float Ay[2];   // top-bottom average asymmetry
      float phase[2];// spin vector angle w.r.t vertical axis [rad]
   } ave;
};

struct StructSpeLumi {
   float Cnts[N_BUNCHES];
   float dCnts[N_BUNCHES];
   float ave;
   float max;
   float min;
};

struct StructStrip {
   float average[1];
   float allowance;
   float max;
   int   st;
};

struct StructEnergyCorr {
   float p[3][N_CHANNELS];
   float perr[3][N_CHANNELS];
};

struct StructStripCheck {
   StructStrip dev, chi2, p1, width, evnt;
   StructEnergyCorr ecorr;
};

struct StructBunch {
   float average[1];
   float sigma_over_mean;
   float sigma;
   float allowance;
   float max_dev;
};

struct StructBunchCheck {
   StructBunch rate, asym[3];// asym[0]:x90, asym[1]:x45, asym[2]:y45
};

// Hbook Associated Stuff
extern "C" {

void  hhbook1d_(int*, char*, int*, float*, float*,  int);
void  hhbook2d_(int*, char*, int*, float*, float*, int*, float*, float*,  int);
void  hhf1_(int*, float*, float*);
void  hhf2_(int*, float*, float*, float*);
void  hhlimit_(int*);
void  hhropen_(int*, char*, char*, char*, int*, int*,  int,int,int);
void  hhrend_(char*, int);
void  hhrout_(int*, int*, char*, int);
void  hhrammar_(float*, int*);
void  hhrebin_(int*, float*, float*, float*, float*, int*, int*, int*);
void  hhpak_(int*, float*);
void  hhpake_(int*, float*);
float hhmax_(int*);
float hhstati_(int*, int*, char*, int*, int);
//void  hhkind_(int*, int*, char*, int);
void  hfith_(int*, char*, char*, int*, float*, float*, float*, float*, float*, float*, int, int);
void  hfithn_(int*, char*, char*, int*, float*, float*, float*, float*, float*, float*, int, int);
}

// Some utility routines to determin histogram range
float GetMax(int N, float A[]);
float GetMin(int N, float A[]);
void  GetMinMax(int N, float A[], float margin, float &min, float &max);
void  GetMinMaxOption(float prefix, int N, float A[], float margin, float &min, float &max);
float WeightedMean(float *A, float *dA, int NDAT);
float WeightedMeanError(float *dA, int NDAT);
void  CalcWeightedMean(float *A, float *dA, int NDAT, float &Ave, float &dAve);
float CalcDivisionError(float x, float y, float dx, float dy);

float QuadErrorDiv(float x, float y, float dx, float dy);
float QuadErrorSum(float dx, float dy);
void  DrawText(TH1 *h, float x, float y, int color, char *text);
void  DrawLine(TH1 *h, float x1, float y1, float x2, float y2, int color, int lstyle, int lwidth);
void  DrawHorizLine(TH1 *h, float x1, float x2, float y, int color, int lstyle, int lwidth);
void  DrawVertLine (TH1 *h, float x, float y, int color, int lwidth);

void  binary_zero(int n, int mb);
void  sqass(float A, float B, float C, float D, float *asym, float *easym);

TGraphErrors* AsymmetryGraph(int Mode, int N, float* x, float* y, float* ex, float* ey);
void  ReadRampTiming(char *filename);


/** */
template<class T> void ReadStlContainer(TBuffer &buf, std::set<T> &s)
{
   s.clear();

   int size = 0;
   buf >> size;

   T value;

   for (int i=0; i<size; i++) {
      buf >> value;
      s.insert(value);
   }
}


/** */
template<class T> void WriteStlContainer(TBuffer &buf, std::set<T> &s)
{
   buf << s.size();

   typename std::set<T>::const_iterator is;

   for (is=s.begin(); is!=s.end(); ++is) {
      buf << *is;
   }
}


/** */
template<class T> std::string VecAsPhpArray(const std::vector<T>& v)
{
   std::stringstream ssChs("");

   ssChs << "array(";

   typename std::vector<T>::const_iterator iv;

   for (iv=v.begin(); iv!=v.end(); ++iv) {
      ssChs << *iv;
      ssChs << (++iv == v.end() ? "" : ", ");
      --iv;
   }

   ssChs << ")";

   return ssChs.str();
}


template<class T> std::string SetAsPhpArray(const std::set<T>& s)
{
   std::stringstream ssChs("");

   ssChs << "array(";

   typename std::set<T>::const_iterator is;

   for (is=s.begin(); is!=s.end(); ++is) {
      ssChs << *is;
      ssChs << (++is == s.end() ? "" : ", ");
      --is;
   }

   ssChs << ")";

   return ssChs.str();
}


template<class Key, class T> std::string MapAsPhpArray(const std::map<Key, T>& m)
{
   std::stringstream ssChs("");

   ssChs << "array(";

   typename std::map<Key, T>::const_iterator im;

   for (im=m.begin(); im!=m.end(); ++im) {
      ssChs << "'" << im->first << "' => '" << im->second << "'";
      ssChs << (++im == m.end() ? "" : ", ");
      --im;
   }

   ssChs << ")";

   return ssChs.str();
}

#endif
