// Header file for AsymErrorDetector
// Author   : Itaru Nakagawa
// Creation : 08/01/2006         
#include "AsymROOT.h"

#ifndef ASYM_ERROR_DETECTOR_H
#define ASYM_ERROR_DETECTOR_H

//=================================================================//
//                     Strip Error Detector                        //
//=================================================================//
typedef struct {
  float allowance;
  float max;
  int st;
} StructInvMass;

typedef struct {
  float p[3][NSTRIP];
  float perr[3][NSTRIP];
} StructEnergyCorr;

struct StructStripCheck {
  float average[1];
  StructInvMass dev, chi2, p1;
  StructEnergyCorr ecorr;
} ;

struct StructBunchCheck {
  float average[1];
  float allowance;
} ;



extern StructBunchCheck bnchchk;
extern StructStripCheck strpchk;



int  StripAnomalyDetector();
int  InvariantMassCorrelation(int st);
int  BananaFit();
int  UnrecognizedAnomaly(int *x, int nx, int *y, int ny, int *z, int &nz);


#endif /* ASYM_ERROR_DETECTOR_H */
