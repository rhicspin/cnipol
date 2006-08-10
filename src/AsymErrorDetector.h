// Header file for AsymErrorDetector
// Author   : Itaru Nakagawa
// Creation : 08/01/2006         
#include "AsymROOT.h"

#ifndef ASYM_ERROR_DETECTOR_H
#define ASYM_ERROR_DETECTOR_H

//=================================================================//
//                     Strip Error Detector                        //
//=================================================================//
int InvariantMassCorrelation(int st);
int BananaFit();
int StripAnomalyDetector();

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
extern StructStripCheck strpchk;


//=================================================================//
//                     Strip Error Detector                        //
//=================================================================//
int BunchAnomalyDetector();
int BunchAsymmetryGaussianFit(TH1F * h1, TH2F * h2, float A[]);
int HotBunchFinder(); 


struct StructBunch {
  float average[1];
  float allowance;
};

struct StructBunchCheck {
  StructBunch rate, asym[2];
} ;
extern StructBunchCheck bnchchk;



//=================================================================//
//                              Misc                               //
//=================================================================//
int UnrecognizedAnomaly(int *x, int nx, int *y, int ny, int *z, int &nz);
void DrawLine(TH1F * h, float x, float y1, int color);
void DrawLine(TH2F * h, float x0, float x1, float y, int color);


#endif /* ASYM_ERROR_DETECTOR_H */
