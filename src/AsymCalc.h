// Header file for AsymCalc
// Author   : Itaru Nakagawa
// Creation : 02/25/2006         
#include "AsymROOT.h"

#ifndef ASYM_CALC_H
#define ASYM_CALC_H

const int ASYM_DEFAULT=-999;
extern const int ASYM_DEFAULT;

//===========================================================================
//                      Main End Process Routine
//===========================================================================
int end_process(recordConfigRhicStruct *cfginfo);


//===========================================================================
//                       Anaolyzing power
//===========================================================================
float WeightAnalyzingPower(int hid);

//===========================================================================
//                              Strip by Strip
//===========================================================================
void StripAsymmetry();
void CalcStripAsymmetry(float aveA_N, int Mode);
Double_t sin_phi(Double_t *x, Double_t *par);
void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);
class AsymFit
{

 private:

 public:
  void SinPhiFit(Float_t p0, Float_t *RawP, Float_t *dRawP, Float_t *phi, 
		 Float_t *P, Float_t *dphi, Float_t &chi2dof);

  // following 3 subroutines are unsuccessful MINUIT sin(phi) fit routines.
  /*
  void SinPhiFit(Float_t p0, Float_t *P, Float_t *phi, Float_t &chi2);
  Float_t sinx(Float_t, Double_t *);
  Double_t GetFittingErrors(TMinuit *gMinuit, Int_t NUM);
  */

};


//===========================================================================
//                          Bunch by Bunch
//===========================================================================
struct BunchAsym {
  float Ax90[2][NBUNCH];
  float Ax45[2][NBUNCH];
  float Ay45[2][NBUNCH];
} ;
extern BunchAsym basym;
struct StructSpeLumi {
  float Cnts[NBUNCH];
  float dCnts[NBUNCH];
  float ave;
  float max;
  float min;
} ;
extern StructSpeLumi SpeLumi;
int  CumulativeAsymmetry();
int calcBunchAsymmetry();
void FillAsymmetryHistgram(char Mode[], int sign, int N, float A[], float dA[], float bunch[]);
TGraphErrors * AsymmetryGraph(int Mode, int N, float x[], float y[], float ex[], float ey[]);
int BunchAsymmetry(int, float A[], float dA[]);



//===========================================================================
//                        Result Printing
//===========================================================================
void binary_zero(int n, int mb);
void PrintWarning();
void PrintRunResults(StructHistStat hstat);



//===========================================================================
//           Some utility routines to determin histogram range
//===========================================================================
float GetMax(int N, float A[]);
float GetMin(int N, float A[]);
void GetMinMax(int N, float A[], float margin, float &min, float &max);
void GetMinMaxOption(float prefix, int N, float A[], float margin, float &min, float &max);
float QuadErrorDiv(float x, float y, float dx, float dy);


//===========================================================================
//                                  HBOOK stuff
//===========================================================================
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
