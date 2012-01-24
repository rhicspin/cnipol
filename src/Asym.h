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
#include <utility> // for std::pair definition

#include "TH1.h"
#include "TGraphErrors.h"
#include "TFitResultPtr.h"

#include "rpoldata.h"

#include "AsymHeader.h"
//#include "TargetUId.h"


enum EPolarimeterId {kB1U = 0, kY1D = 1, kB2D = 2, kY2U = 3, kUNKNOWN_POLID};

enum EBeamId {kBLUE_BEAM = 1, kYELLOW_BEAM = 2, kUNKNOWN_BEAM};

enum ERingId {kBLUE_RING = 1, kYELLOW_RING = 2, kUNKNOWN_RING};

enum EStreamId {kUPSTREAM = 1, kDOWNSTREAM = 2, kUNKNOWN_STREAM};

enum ETargetOrient {kTARGET_H = 0, kTARGET_V = 1, kUNKNOWN_ORIENT};

enum EBeamEnergy {kINJECTION = 24, kBEAM_ENERGY_100 = 100, kFLATTOP = 250};

enum ESpinState {kSPIN_DOWN = -1, kSPIN_NULL = 0, kSPIN_UP = +1};


typedef std::map<std::string, std::string>     Str2StrMap;
typedef std::pair<Double_t, Double_t>          ValErrPair;
typedef std::set<ValErrPair>                   ValErrSet;
typedef ValErrSet::iterator                    ValErrSetIter;
typedef std::map<std::string, ValErrPair>      ValErrMap;
typedef std::map<std::string, TFitResultPtr>   Str2FitResMap;
typedef std::map<ESpinState,  TFitResultPtr>   Spin2FitResMap;
typedef std::pair<UShort_t, UShort_t>          DetLRPair;
typedef std::set<DetLRPair>                    DetLRSet;
//typedef std::map<UShort_t, UShort_t>  DetLRPairs;

typedef std::map<EPolarimeterId, ValErrSet>    PolId2ValErrSet;
typedef PolId2ValErrSet::iterator              PolId2ValErrSetIter;
typedef PolId2ValErrSet::const_iterator        PolId2ValErrSetConstIter;
typedef std::map<EPolarimeterId, ValErrPair>   PolId2ValErrMap;
typedef PolId2ValErrMap::iterator              PolId2ValErrMapIter;
typedef PolId2ValErrMap::const_iterator        PolId2ValErrMapConstIter;

// Beam id
typedef std::map<EBeamId,        ValErrSet>    BeamId2ValErrSet;
typedef BeamId2ValErrSet::iterator             BeamId2ValErrSetIter;
typedef BeamId2ValErrSet::const_iterator       BeamId2ValErrSetConstIter;
typedef std::map<EBeamId,        ValErrPair>   BeamId2ValErrMap;
typedef BeamId2ValErrMap::iterator             BeamId2ValErrMapIter;
typedef BeamId2ValErrMap::const_iterator       BeamId2ValErrMapConstIter;

typedef std::map<ETargetOrient,  ValErrPair>   TgtOrient2ValErrMap;
typedef TgtOrient2ValErrMap::iterator          TgtOrient2ValErrMapIter;
typedef std::map<EBeamId, TgtOrient2ValErrMap> BeamId2TgtOrient2ValErrMap;
typedef BeamId2TgtOrient2ValErrMap::iterator   BeamId2TgtOrient2ValErrMapIter;

typedef std::map<std::string, EBeamId>         String2BeamIdMap;
typedef String2BeamIdMap::iterator             String2BeamIdMapIter;

// Ring id
typedef std::map<ERingId,        ValErrSet>    RingId2ValErrSet;
typedef RingId2ValErrSet::iterator             RingId2ValErrSetIter;
typedef RingId2ValErrSet::const_iterator       RingId2ValErrSetConstIter;
typedef std::map<ERingId,        ValErrPair>   RingId2ValErrMap;
typedef RingId2ValErrMap::iterator             RingId2ValErrMapIter;
typedef RingId2ValErrMap::const_iterator       RingId2ValErrMapConstIter;

typedef std::map<ETargetOrient,  ValErrPair>   TgtOrient2ValErrMap;
typedef TgtOrient2ValErrMap::iterator          TgtOrient2ValErrMapIter;
typedef std::map<ERingId, TgtOrient2ValErrMap> RingId2TgtOrient2ValErrMap;
typedef RingId2TgtOrient2ValErrMap::iterator   RingId2TgtOrient2ValErrMapIter;

typedef std::map<std::string, ERingId>         String2RingIdMap;
typedef String2RingIdMap::iterator             String2RingIdMapIter;

typedef std::map<std::string, ETargetOrient>   String2TgtOrientMap;
typedef String2TgtOrientMap::iterator          String2TgtOrientMapIter;

typedef std::map<std::string, UShort_t>        String2TargetIdMap;
typedef String2TargetIdMap::iterator           String2TargetIdMapIter;



typedef std::set<EBeamId>                  BeamIdSet;
typedef BeamIdSet::iterator                BeamIdSetIter;
typedef BeamIdSet::const_iterator          BeamIdConstIter;

typedef std::set<ERingId>                  RingIdSet;
typedef RingIdSet::iterator                RingIdSetIter;
typedef RingIdSet::const_iterator          RingIdConstIter;

typedef std::set<ETargetOrient>            TargetOrientSet;
typedef TargetOrientSet::iterator          TargetOrientSetIter;

typedef std::set<EBeamEnergy>::iterator    IterBeamEnergy;
typedef std::set<EPolarimeterId>::iterator IterPolarimeterId;
typedef std::set<ESpinState>::iterator     IterSpinState;

typedef std::set<EPolarimeterId>           PolarimeterIdSet;

typedef std::set<EBeamEnergy>::iterator    BeamEnergyIter;
typedef PolarimeterIdSet::iterator         PolarimeterIdIter;
typedef PolarimeterIdSet::const_iterator   PolarimeterIdConstIter;
typedef std::set<ESpinState>::iterator     SpinStateIter;


typedef std::set<UShort_t>    ChannelSet;
typedef ChannelSet::iterator  ChannelSetIter;


std::ostream& operator<<(std::ostream &os, const ESpinState &ss);
std::ostream& operator<<(std::ostream &os, const ValErrPair &vep);
std::ostream& operator<<(std::ostream &os, const TgtOrient2ValErrMap &vep);
TBuffer&      operator<<(TBuffer &buf, const ValErrPair &vep);
TBuffer&      operator>>(TBuffer &buf, ValErrPair &vep);


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

   static void PrintAll();
};


struct StructMask {
   int detector;
};

struct StructAnomaly {
   int nstrip;
   int st[N_SILICON_CHANNELS];
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
   float RMS[N_SILICON_CHANNELS];
   float mdev[N_SILICON_CHANNELS];
   float tedev[N_SILICON_CHANNELS];
   float err[N_SILICON_CHANNELS];
   float chi2[N_SILICON_CHANNELS];
   float strip[N_SILICON_CHANNELS];

   StructFeedBack();
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
   long int NStrip[N_SPIN_STATES][N_SILICON_CHANNELS];   // strip counters for 3 different spin states

   StructStripCounter();
};

struct StructStripCounterTgt
{
   long int NStrip[MAXDELIM][N_SPIN_STATES][N_SILICON_CHANNELS]; // strip counters for 3 different spin states

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
   float p[3][N_SILICON_CHANNELS];
   float perr[3][N_SILICON_CHANNELS];
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
float       GetMax(int N, float A[]);
float       GetMin(int N, float A[]);
void        GetMinMax(int N, float A[], float margin, float &min, float &max);
void        GetMinMaxOption(float prefix, int N, float A[], float margin, float &min, float &max);
float       WeightedMean(float *A, float *dA, int NDAT);
float       WeightedMeanError(float *dA, int NDAT);
void        CalcWeightedMean(float *A, float *dA, int NDAT, float &Ave, float &dAve);
ValErrPair  CalcWeightedAvrgErr(const ValErrSet &valerrs);
ValErrPair  CalcWeightedAvrgErr(const ValErrPair ve1, const ValErrPair ve2);
float       CalcDivisionError(float x, float y, float dx, float dy);
ValErrPair  CalcDivision(ValErrPair ve1, ValErrPair ve2, Double_t r12=0);

float QuadErrorDiv(float x, float y, float dx, float dy);
float QuadErrorSum(float dx, float dy);
void  DrawText(TH1 *h, float x, float y, int color, char *text);
void  DrawLine(TH1 *h, float x1, float y1, float x2, float y2, int color, int lstyle, int lwidth);
void  DrawHorizLine(TH1 *h, float x1, float x2, float y, int color, int lstyle, int lwidth);
void  DrawVertLine (TH1 *h, float x, float y, int color, int lwidth);

void  binary_zero(int n, int mb);

TGraphErrors* AsymmetryGraph(int Mode, int N, float* x, float* y, float* ex, float* ey);
void  ReadRampTiming(char *filename);


/** */
template<class T> void ReadStlContainer(TBuffer &buf, std::set<T> &s)
{ //{{{
   s.clear();

   int size = 0;
   buf >> size;

   T value;

   for (int i=0; i<size; i++) {
      buf >> value;
      s.insert(value);
   }
} //}}}


/** */
template<class T> void WriteStlContainer(TBuffer &buf, std::set<T> &s)
{ //{{{
   buf << s.size();

   typename std::set<T>::const_iterator is;

   for (is=s.begin(); is!=s.end(); ++is) {
      buf << *is;
   }
} //}}}


/** */
template<class T> std::string VecAsPhpArray(const std::vector<T>& v)
{ //{{{
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
} //}}}


/** */
template<class T> std::string SetAsPhpArray(const std::set<T>& s)
{ //{{{
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
} //}}}


/** */
template<class Key, class T> std::string MapAsPhpArray(const std::map<Key, T>& m)
{ //{{{
   std::stringstream ssChs("");

   ssChs << "array(";

   typename std::map<Key, T>::const_iterator im;

   for (im=m.begin(); im!=m.end(); ++im) {
      ssChs << std::endl << "\t\t" << "'" << im->first << "' => " << im->second;
      //PairAsPhpArray< std::pair<Key, T> >(*im);
      ssChs << (++im == m.end() ? "" : ", ");
      --im;
   }

   ssChs << ")";

   return ssChs.str();
} //}}}


/** */
//template<class P1, class P2> std::string PairAsPhpArray(const std::pair<P1, P2> &p)
//{ //{{{
//   std::stringstream ssChs("");
//
//   ssChs << "'" << p.first << "' => " << p.second;
//
//   return ssChs.str();
//} //}}}

std::string PairAsPhpArray(const ValErrPair &p);
std::string FitResultAsPhpArray(const TFitResultPtr &fitres);

//template<class > std::string

#endif
