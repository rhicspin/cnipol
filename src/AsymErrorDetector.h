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
//                     Detector Anomaly                            //
//=================================================================//
int DetectorAnomaly();


//=================================================================//
//                              Misc                               //
//=================================================================//
int RegisterAnomaly(float x[], int nx, int y[], int ny, int z[], int &nz);
int RegisterAnomaly(int x[], int nx, int y[], int ny, int z[], int &nz);
int UnrecognizedAnomaly(int x[], int nx, int y[], int ny, int z[], int &nz);
void DrawLine(TH1F * h, float x, float y1, int color, int lwidth);
void DrawLine(TH2F * h, float x0, float x1, float y, int color, int lstyle, int lwidth);
float QuadErrorDiv(float x, float y, float dx, float dy);
float QuadErrorSum(float dx, float dy);



#endif /* ASYM_ERROR_DETECTOR_H */
