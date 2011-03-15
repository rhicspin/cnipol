// Header file for AsymCalc
// Author   : Itaru Nakagawa
// Creation : 02/25/2006         

#ifndef ASYM_CALC_H
#define ASYM_CALC_H

#include <math.h>
#include <string>

#include "TF1.h"
#include "TGraphErrors.h"
#include "TLatex.h"

#include "rhicpol.h"
#include "rpoldata.h"

#include "AnaInfo.h"
#include "TargetInfo.h"
#include "AsymGlobals.h"
#include "AsymRootGlobals.h"
//#include "WeightedMean.h"
#include "AsymErrorDetector.h"

struct StructHistStat;

//const int N=NSTRIP;

//===========================================================================
//                      Main End Process Routine
//===========================================================================
void end_process();
void CompleteHistogram();
void TgtHist();

//===========================================================================
//                       Anaolyzing power
//===========================================================================
float WeightAnalyzingPower(int hid);
int   ExclusionList(int i, int j, int polBeam=0);

//===========================================================================
//                              Strip by Strip
//===========================================================================
void StripAsymmetry();
void CalcStripAsymmetry(float aveA_N, int Mode, long int nstrip[][NSTRIP]);
void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);


class AsymFit
{
 private:

 public:
  void SinPhiFit(Float_t p0, Float_t *RawP, Float_t *dRawP, Float_t *phi, 
		 Float_t *P, Float_t *dphi, Float_t &chi2dof);
  void ScanSinPhiFit(Float_t p0, Float_t *RawP, Float_t *dRawP, Float_t *phi, 
  		 Float_t *P, Float_t *dphi, Float_t &chi2dof);

  // following 3 subroutines are unsuccessful MINUIT sin(phi) fit routines.
  /*
  void SinPhiFit(Float_t p0, Float_t *P, Float_t *phi, Float_t &chi2);
  Float_t sinx(Float_t, Double_t *);
  Double_t GetFittingErrors(TMinuit *gMinuit, Int_t NUM);
  */
};

//                          Bunch by Bunch
struct BunchAsym { // array[0]:asymmetry, array[1]:asymmetry error
   float Ax90[2][NBUNCH];
   float Ax45[2][NBUNCH];
   float Ay45[2][NBUNCH];
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
   float Cnts[NBUNCH];
   float dCnts[NBUNCH];
   float ave;
   float max;
   float min;
};

void sqass(float A, float B, float C, float D, float *asym, float *easym);
void CumulativeAsymmetry();
void CalcBunchAsymmetry();
void FillAsymmetryHistgram(std::string mode, int sign, int N, float A[], float dA[], float bunch[]);
TGraphErrors* AsymmetryGraph(int Mode, int N, float x[], float y[], float ex[], float ey[]);
void BunchAsymmetry(int, float A[], float dA[]);

void  calcWeightedMean(float A[], float dA[], int NDAT, float &Ave, float &dAve);
float WeightedMean(float A[], float dA[], int NDAT);
float WeightedMeanError(float A[], float dA[], float Ave, int NDAT);

// bunch asymmetry average routines
void  calcBunchAsymmetryAverage();
void  calcLRAsymmetry(float X90[2], float X45[2], float &A, float &dA);
float calcDivisionError(float x, float y, float dx, float dy);
void  calcAsymmetry(int a, int b, int atot, int btot, float &Asym, float &dAsym);
float TshiftFinder(int, int);

// Calculate Statistics adn  Result Printing
void CalcStatistics();
void binary_zero(int n, int mb);
void PrintWarning();
void PrintRunResults(StructHistStat hstat);
void DrawPlotvsTar();

//   Profile Error
float ProfileError(float x);

void  SpecificLuminosity(float&, float&, float&);

// HBOOK stuff
extern void HHBOOK1(int hid, char* hname, int xnbin, float xmin, float xmax) ;
extern void HHPAK(int, float*);
extern void HHPAKE(int, float*);
extern void HHF1(int, float, float);
//extern void HHKIND(int, int*, char*);
extern float HHMAX(int);
extern float HHSTATI(int hid, int icase, char * choice, int num);
extern void HHFITHN(int hid, char*chfun, char*chopt, int np, float*par, 
	float*step, float*pmin, float*pmax, float*sigpar, float&chi2);
extern void HHFITH(int hid, char*fun, char*chopt, int np, float*par, 
	float*step, float*pmin, float*pmax, float*sigpar, float&chi2);

#endif /* ASYM_CALC_H */
