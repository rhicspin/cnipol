
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

#include "TGraphErrors.h"
#include "TH1.h"
#include "TH1D.h"

#include "DrawObjContainer.h"

#include "AsymGlobals.h"
#include "AsymErrorDetector.h"


class MseRunInfoX;

//  Main End Process Routine
void end_process(MseRunInfoX &run);
void CompleteHistogram();


class AsymCalculator
{
private:

public:

   //void          CalcBunchAsym(TH2 &hDetVsBunchId);
   void          CalcBunchAsym(DrawObjContainer *oc);
   void          CalcBunchAsymSqrtFormula(DrawObjContainer *oc);

   // first==="left" and second==="right" detectors
   TGraphErrors* CalcBunchAsymDet(TH2 &hDetVsBunchId_ss, TH2 &hDetVsBunchId, DetLRSet detSet, TGraphErrors *gr=0);
   TGraphErrors* CalcBunchAsymX90(TH2 &hDetVsBunchId_ss, TH2 &hDetVsBunchId, TGraphErrors *gr=0);
   TGraphErrors* CalcBunchAsymX45(TH2 &hDetVsBunchId_ss, TH2 &hDetVsBunchId, TGraphErrors *gr=0);
   TGraphErrors* CalcBunchAsymY45(TH2 &hDetVsBunchId_ss, TH2 &hDetVsBunchId, TGraphErrors *gr=0);

   ValErrMap     CalcBunchAsymDetSqrtFormula(TH1 &hUp, TH1 &hDown, DetLRSet detSet);
   ValErrMap     CalcBunchAsymX90SqrtFormula(TH1 &hUp, TH1 &hDown);
   ValErrMap     CalcBunchAsymX45SqrtFormula(TH1 &hUp, TH1 &hDown);
   ValErrMap     CalcBunchAsymY45SqrtFormula(TH1 &hUp, TH1 &hDown);

   void          CumulativeAsymmetry();
   // Strip by Strip
   void  CalcStripAsymmetry(DrawObjContainer *oc);
   void  CalcStripAsymmetryByProfile(DrawObjContainer *oc);
   void  CalcKinEnergyAChAsym(DrawObjContainer *oc);
   void  CalcStripAsymmetry(int Mode);
   TH1D* CalcChannelAsym(TH1I &hUp, TH1I &hDown, TH1D* hChAsym=0);
   //void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);

   TFitResultPtr FitChAsymConst(TH1D &hChAsym, TGraphErrors *gr=0);
   TFitResultPtr FitChAsymSine(TH1D &hChAsym, TGraphErrors *gr=0);

	void SinPhiFit(Float_t p0, Float_t *RawP, Float_t *dRawP,
 	   Float_t *P, Float_t *phase, Float_t &chi2dof);
   void ScanSinPhiFit(Float_t p0, Float_t *RawP, Float_t *dRawP,
      Float_t *P, Float_t *phase, Float_t &chi2dof);
 
   static Float_t    WeightAnalyzingPower(int hid);
	static Bool_t     ExcludeStrip(int i, int j);
   static ValErrPair CalcAsym(Double_t A, Double_t B, Double_t totalA, Double_t totalB);
   static ValErrPair CalcAsymSqrtFormula(Double_t A, Double_t B, Double_t C, Double_t D);
   static void       sqass(Double_t A, Double_t B, Double_t C, Double_t D, Double_t &asym, Double_t &asymErr);

   // following 3 subroutines are unsuccessful MINUIT sin(phi) fit routines.
   //void     SinPhiFit(Float_t p0, Float_t *P, Float_t *phi, Float_t &chi2);
   //Float_t  sinx(Float_t, Double_t *);
   //Double_t GetFittingErrors(TMinuit *gMinuit, Int_t NUM);
};

void FillAsymmetryHistgram(std::string mode, int sign, float *A, float *dA);

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
