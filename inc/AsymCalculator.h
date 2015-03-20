
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


class MseMeasInfoX;

void CompleteHistogram();


class AsymCalculator
{
private:

   static DetLRPair arrX90Dets[1]; //!
   static DetLRPair arrX45Dets[2]; //!
   static DetLRPair arrX45TDets[1]; //!
   static DetLRPair arrX45BDets[1]; //!
   static DetLRPair arrY45Dets[2]; //!

public:

   static DetLRSet X90Dets; //!
   static DetLRSet X45Dets; //!
   static DetLRSet X45TDets; //!
   static DetLRSet X45BDets; //!
   static DetLRSet Y45Dets; //!

   void          CalcBunchAsym(DrawObjContainer *oc);
   void          CalcBunchAsymSqrtFormula(DrawObjContainer *oc);
   void          CalcDelimAsym(DrawObjContainer *oc);
   void          CalcDelimAsymSqrtFormula(DrawObjContainer *oc);
   void          CalcOscillPhaseAsymSqrtFormula(TH2 &h2DetCounts_up, TH2 &h2DetCounts_down, TH1 &hAsym, DetLRSet detSet);

   // first==="left" and second==="right" detectors
   TGraphErrors* CalcBunchAsymDet(TH2 &hDetVsBunchId_ss, TH2 &hDetVsBunchId, DetLRSet detSet, TGraphErrors *gr=0);
   TGraphErrors* CalcBunchAsymX90(TH2 &hDetVsBunchId_ss, TH2 &hDetVsBunchId, TGraphErrors *gr=0);
   TGraphErrors* CalcBunchAsymX45(TH2 &hDetVsBunchId_ss, TH2 &hDetVsBunchId, TGraphErrors *gr=0);
   TGraphErrors* CalcBunchAsymY45(TH2 &hDetVsBunchId_ss, TH2 &hDetVsBunchId, TGraphErrors *gr=0);

   ValErrMap     CalcDetAsymSqrtFormula    (TH1 &hUp, TH1 &hDown, DetLRSet detSet);
   ValErrMap     CalcDetAsymX90SqrtFormula (TH1 &hUp, TH1 &hDown);
   ValErrMap     CalcDetAsymX45SqrtFormula (TH1 &hUp, TH1 &hDown);
   ValErrMap     CalcDetAsymX45TSqrtFormula(TH1 &hUp, TH1 &hDown);
   ValErrMap     CalcDetAsymX45BSqrtFormula(TH1 &hUp, TH1 &hDown);
   ValErrMap     CalcDetAsymY45SqrtFormula (TH1 &hUp, TH1 &hDown);

   void          CumulativeAsymmetry();
   // Strip by Strip
   void  CalcStripAsymmetry(DrawObjContainer *oc);
   void  CalcStripAsymmetryByProfile(DrawObjContainer *oc);
   void  CalcKinEnergyAChAsym(DrawObjContainer *oc);
   void  CalcLongiChAsym(DrawObjContainer *oc);
   void  CalcStripAsymmetry(int Mode);
   TH1D* CalcChannelAsym(TH1I &hUp, TH1I &hDown, TH1D* hChAsym=0);

   TFitResultPtr FitChAsymConst(TH1D &hChAsym, TGraphErrors *gr=0);
   TFitResultPtr FitChAsymSine(TH1D &hChAsym, TGraphErrors *gr=0);

	void SinPhiFit(Float_t p0, Float_t *RawP, Float_t *dRawP,
 	   Float_t *P, Float_t *phase, Float_t &chi2dof);
   void ScanSinPhiFit(Float_t p0, Float_t *RawP, Float_t *dRawP,
      Float_t *P, Float_t *phase, Float_t &chi2dof);
 
   static Float_t    WeightAnalyzingPower(int hid);
   static Bool_t     IsExclusionCandidate(int i, int j);
   static ValErrPair CalcAsym(Double_t A, Double_t B, Double_t totalA, Double_t totalB);
   static ValErrPair CalcAsymSqrtFormula(Double_t A, Double_t B, Double_t C, Double_t D);
   static void       sqass(Double_t A, Double_t B, Double_t C, Double_t D, Double_t &asym, Double_t &asymErr);
   static void       CalcStatistics();
   static void       PrintWarning();
   static void       PrintRunResults();
   static void       DrawPlotvsTar();
};


void FillAsymmetryHistgram(std::string mode, int sign, float *A, float *dA);

// bunch asymmetry average routines
void  calcBunchAsymmetryAverage();
void  calcLRAsymmetry(float X90[2], float X45[2], float &A, float &dA);

// Profile Error
float ProfileError(float x);
void  SpecificLuminosity(float&, float&, float&);

#endif
