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
//#include "TMath.h"
//#include "TMinuit.h"
//#include "TString.h"

#include "rhicpol.h"
#include "rpoldata.h"

#include "AsymGlobals.h"
//#include "AsymCalc.h"

void DetectorAnomaly();

// Strip Error Detector
void  InvariantMassCorrelation(int st);
void  BananaFit();
void  StripAnomalyDetector();

// Bunch Error Detector
float BunchAsymmetryGaussianFit(TH1F * h1, TH2F * h2, float A[], float dA[], int err_code);
void  BunchAsymmetryAnomaly();
void  BunchAnomalyDetector();
void  HotBunchFinder(int err_code); 

// Misc
void RegisterAnomaly(float x[], int nx, int y[], int ny, int z[], int &nz);
void RegisterAnomaly(int x[], int nx, int y[], int ny, int z[], int &nz);
void UnrecognizedAnomaly(int x[], int nx, int y[], int ny, int z[], int &nz);
//void checkForBadBunches();

#endif
