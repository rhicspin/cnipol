
/**
 * Header file for AsymCalc
 * Author   : Itaru Nakagawa
 *          : Dmitri Smirnov
 *
 * Creation : 02/25/2006         
 */

#ifndef AsymCalculator_h
#define AsymCalculator_h

#include <string>

#include "TF1.h"
#include "TGraphErrors.h"
#include "TLatex.h"

#include "rhicpol.h"
#include "rpoldata.h"

#include "AsymGlobals.h"
#include "AsymErrorDetector.h"

class MseRunInfoX;

//  Main End Process Routine
void end_process(MseRunInfoX &run);
void CompleteHistogram();
void TgtHist();

// Strip by Strip
void StripAsymmetry(MseRunInfoX &run);
void CalcStripAsymmetry(float aveA_N, int Mode, long int nstrip[][NSTRIP]);
//void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);


class AsymCalculator
{
private:

public:

   void CalcBunchAsymmetry();
   void BunchAsymmetry(int, float A[], float dA[]);
	void SinPhiFit(Float_t p0, Float_t *RawP, Float_t *dRawP,
 	   Float_t *P, Float_t *phase, Float_t &chi2dof);
   void ScanSinPhiFit(Float_t p0, Float_t *RawP, Float_t *dRawP,
      Float_t *P, Float_t *phase, Float_t &chi2dof);
 
   static void   CalcAsymmetry(int a, int b, int atot, int btot, float &Asym, float &dAsym);
   static float  WeightAnalyzingPower(int hid);
	static Bool_t ExcludeStrip(int i, int j);

   // following 3 subroutines are unsuccessful MINUIT sin(phi) fit routines.
   //void     SinPhiFit(Float_t p0, Float_t *P, Float_t *phi, Float_t &chi2);
   //Float_t  sinx(Float_t, Double_t *);
   //Double_t GetFittingErrors(TMinuit *gMinuit, Int_t NUM);
};

void CumulativeAsymmetry();
void FillAsymmetryHistgram(std::string mode, int sign, int N, float A[], float dA[], float bunch[]);

// bunch asymmetry average routines
void  calcBunchAsymmetryAverage();
void  calcLRAsymmetry(float X90[2], float X45[2], float &A, float &dA);
float TshiftFinder(int, int);

// Calculate Statistics adn  Result Printing
void  CalcStatistics();
void  PrintWarning();
void  PrintRunResults();
void  DrawPlotvsTar();

// Profile Error
float ProfileError(float x);
void  SpecificLuminosity(float&, float&, float&);

#endif
