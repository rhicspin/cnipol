#ifndef AgsCniDefs_H
#define AgsCniDefs_H

#include <stdlib.h>
#include <vector>
#include <string>
#include <math.h>
#include <TH1F.h>
#include <TF1.h>
#include <TLatex.h>
#include "CniDefs.h"
#include "cnidata.h"

using namespace CniDefs;
namespace AgsCni {
  const int nAgsCniDetectors   =  8;   // number of detectors
  const int nAgsCniStripsTotal = nAgsCniDetectors*nCniStrips;

  const int nAgsSections       = 12;
  const int nAgsBunches        =  6;
  const int nAgsCniTOF         =  2;   
  
  enum AgsCycleUnits { cu_momentum=0, cu_time=1 };
  enum AgsStatUnits  { su_minutes=0,  su_Mevents=1, su_spills=2 };
  enum AgsGccCode    {gcc_ramp, gcc_injection, gcc_momentum, gcc_flattop,
		      gcc_time, gcc_sweep, gcc_mixed, gcc_unknown};
  enum AgsUser       {user_unknown, user_flattop, user_injection, user_ggamma};
  const char* user_string(AgsUser);
  enum AgsMeasType   {meas_unknown, meas_fixed, meas_profile, meas_ramp};
  const char* meas_string(AgsMeasType);
  enum AgsSummary    {sum_Scan=1, sum_Prof=2};

#include "AgsCniEmittance.h"
  const int    AgsGccMean      =   2165;
  const int    AgsGccBins      =     20;
  const double AgsMomentMax    =     30.;
  const int    AgsMomentBin    =    150;
  const int    AgsTimeBins     =    250;
  const int    AgsTimeMax      =   2500;
  const int    nAgsProfBins    =     80;
  const int    AgsProfStep     =     16;

  const int    def_SpillEvents =  10000; 
  const double def_SpillRates  =  0.200; 
  const int    def_SpillMin    =      1;
  const int    def_SpillMax    = 100000;
  const int    def_BunchMask   =  0x000;
  const int    def_GccMin      =      1;
  const int    def_GccMax      =   3000;



  int  CodeMask(int code);
  int  DetMaskV();
  int  DetMaskH();
  int  DetMask(int);
  bool canFit (int mask);
  bool canFitH(int mask);
  int  findPair(int);
  int *findQuad(int);

  struct AgsCniSpillStat {
    struct AgsCniSpillPair *Pair;   // reference to the Spill Pair (0 if none)
    int          recNumber;         // spill number in the file
    int          runSpillNumber;    // spill number assigned by the DAQ
    int          agsSpillNumber;    // AGS spill number
    CniBunchFill bf;                // Beam polarity (from CDEV)
    int          CBM;               // Intensity (from CDEV)
    int          nEvents;           // Total Number Events in the Spill
    int          nTotal;            // Number of events in "Good" bunches
    int bunchStat[nAgsSections];    // Statistice per Bunch
    int bunchMask;                  // "Good Bunch" mask, 
                                    // to be set by the setBunchMask()
    int          gccMin;
    int          gccMax;
    int hGCC     [AgsGccBins];      // GCC Histogram
    int hInj     [AgsGccBins];      // GCC Histogram for injection 
    int hTime    [AgsTimeBins];     // GCC as Time
    int    tMin;
    int    tMax;
    double tMean;
    double tSigma;
    double tFrac;
    void add(int gcc, int nev) {
      if (gcc<gccMin) gccMin=gcc;
      if (gcc>gccMax) gccMax=gcc;
      int _gcc = gcc - (AgsGccMean-AgsGccBins/2);
      if (_gcc>=0 && _gcc<AgsGccBins) hGCC [_gcc] += nev;
      if ( gcc>=0 &&  gcc<AgsGccBins) hInj [ gcc] += nev;
      if ( gcc>=0 &&  gcc<AgsTimeMax) hTime[ gcc*AgsTimeBins/AgsTimeMax] += nev;
    }
    void setBunchMask() {           // A function to calculate bunchMask
                                    //        for the Spill
      if (bunchMask) {
	nTotal = 0;
	for (int i=0;i<nAgsSections;i++) {
	  if (bunchMask & (1<<i)) nTotal += bunchStat[i];
	}
	return;
      }
      int nm=0;
      int nmax=0;
      for (int i=0;i<nAgsSections;i++) {
	if (bunchStat[i]>nmax) {
	  nmax = bunchStat[i];
	  nm = i;
	}
      }
      if (nmax < def_SpillEvents) {
	bunchMask=0;
	return;
      }
      bunchStat[nm] = -bunchStat[nm];
      bunchMask = 1<<nm;
      int level = int(0.1*nmax), nBunch=1;
      while(true) {
	nmax=0;
	for (int i=0; i<nAgsSections;i++) {
	  if (bunchStat[i]>nmax) {nmax = bunchStat[i]; nm = i;}
	}
	if (nmax<level ) break;
	if (++nBunch >= 4) {bunchMask = (1<<nAgsSections)-1; break;}
	bunchStat[nm] = -bunchStat[nm];
	if (nm<nAgsSections-1  && bunchStat[nm+1]>=0) bunchMask |= (1<<nm);
	if (nm>0 && bunchStat[nm-1]<0) {
	  if (abs(bunchStat[nm]) > 0.2*abs(bunchStat[nm-1]))
	    bunchMask &= ~(1<<(nm-1));
	}	  	
      }
      nTotal = 0;
      for (int i=0;i<nAgsSections;i++) {
	if (bunchStat[i]<0) bunchStat[i]=-bunchStat[i];
	if (bunchMask & (1<<i)) nTotal += bunchStat[i];
      }
    }

    void scaleTimeHist() {
      tFrac = AgsProfStep/double(gccMax-gccMin);
      tMean  = (gccMax+gccMin+1)/2.;
      tSigma = (gccMax-gccMin)/2.;
      tMin   = int(tMean+0.5) - nAgsProfBins*AgsProfStep/2;
      tMax   = tMin           + nAgsProfBins*AgsProfStep;
    }
    void scaleProfileHist() {
      
      tFrac = (gccMax>gccMin ? AgsProfStep/double(gccMax-gccMin) : 0);
      tMean  = pseudogaus(AgsTimeBins,hTime,tSigma,0,AgsTimeBins);
      tMean  = double(AgsTimeMax)/double(AgsTimeBins)*(tMean+0.5);
      tSigma = double(AgsTimeMax)/double(AgsTimeBins)*tSigma;
      tMin   = int(tMean+0.5) - nAgsProfBins*AgsProfStep/2;
      tMax   = tMin           + nAgsProfBins*AgsProfStep;
    }
  };

  struct AgsCniSpillPair {
    AgsCniSpillStat *pSpill[2];
    CniPolStat       detStat[nAgsCniDetectors];
    inline CniPolStat *getDetStat(unsigned i=0) {
      return (int(i)<nAgsCniDetectors ? detStat+i : 0);};
    inline CniStatStruct *getDetStat(unsigned i, CniBunchFill bf) {
      if ((int)i >= nAgsCniDetectors ) return 0;
      switch (bf) {
      case bf_pos : return &((detStat+i)->statPos);
      case bf_neg : return &((detStat+i)->statNeg);
      default:      return 0;
      }
    };
    inline CniStatStruct *getDetStatPos(unsigned i) {
      return (int(i)<nAgsCniDetectors ? &((detStat+i)->statPos) : 0);};
    inline CniStatStruct *getDetStatNeg(unsigned i) {
      return (int(i)<nAgsCniDetectors ? &((detStat+i)->statPos) : 0);};
    inline AgsCniSpillStat *getSpill(CniBunchFill bf) {
      if (pSpill[0]->bf == bf) return pSpill[0]; 
      if (pSpill[1]->bf == bf) return pSpill[1];
      return 0;
    };
  };
  double AN(double t);
  
};


#endif //AgsCniDefs_H
 

