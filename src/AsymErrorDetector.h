// Header file for AsymErrorDetector
// Author   : Itaru Nakagawa
// Creation : 08/01/2006         

/**
 * 24 Dec, 2010 - Dmitri Smirnov
 *    - Heavily modified and cleaned up...
 */

#ifndef ASYM_ERROR_DETECTOR_H
#define ASYM_ERROR_DETECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <iostream>

#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TMinuit.h"
#include "TString.h"
#include "TText.h"

#include "rhicpol.h"
#include "rpoldata.h"

#include "Asym.h"
//#include "WeightedMean.h"
#include "AsymCalc.h"


//=================================================================//
//                     Strip Error Detector                        //
//=================================================================//
int InvariantMassCorrelation(int st);
int BananaFit();
void StripAnomalyDetector();

typedef struct {
  float average[1];
  float allowance;
  float max;
  int st;
} StructStrip;

typedef struct {
  float p[3][NSTRIP];
  float perr[3][NSTRIP];
} StructEnergyCorr;

struct StructStripCheck {
  StructStrip dev, chi2, p1, width, evnt;
  StructEnergyCorr ecorr;
};

extern StructStripCheck strpchk;


//=================================================================//
//                     Bunch Error Detector                        //
//=================================================================//
float BunchAsymmetryGaussianFit(TH1F * h1, TH2F * h2, float A[], float dA[], int err_code);
void  BunchAsymmetryAnomaly();
void  BunchAnomalyDetector();

int HotBunchFinder(int err_code); 

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

extern StructBunchCheck bnchchk;


//=================================================================//
//                     Detector Anomaly                            //
//=================================================================//
extern int DetectorAnomaly();


//=================================================================//
//                              Misc                               //
//=================================================================//
int   RegisterAnomaly(float x[], int nx, int y[], int ny, int z[], int &nz);
int   RegisterAnomaly(int x[], int nx, int y[], int ny, int z[], int &nz);
int   UnrecognizedAnomaly(int x[], int nx, int y[], int ny, int z[], int &nz);
void  DrawText(TH1I * h, float x, float y, int color, char * text);
void  DrawText(TH2F * h, float x, float y, int color, char * text);
void  DrawLine(TH1F * h, float x, float y1, int color, int lwidth);
void  DrawLine(TH2F * h, float x0, float x1, float y, int color, int lstyle, int lwidth);
void  DrawLine(TH1I * h, float x0, float x1, float y, int color, int lstyle, int lwidth);
float QuadErrorDiv(float x, float y, float dx, float dy);
float QuadErrorSum(float dx, float dy);
void  checkForBadBunches();

#endif
