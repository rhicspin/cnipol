// Header file for AsymCalc
// Author   : Itaru Nakagawa
// Creation : 02/25/2006         

#ifndef ASYM_CALC_H
#define ASYM_CALC_H

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


float RawP[72], dRawP[72]; // Raw Polarization (Not corrected for phi)
float FitChi2;
void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);
float WeightAnalyzingPower(int hid);


int BunchAsymmetry(int, float A[], float dA[]);
struct BunchAsym {
  float Ax90[2][NBUNCH];
  float Ax45[2][NBUNCH];
  float Ay45[2][NBUNCH];
} basym;



class AsymFit
{

 private:

 public:
  void SinPhiFit(Float_t p0, Float_t *par, Float_t *phi, Float_t &chi2);
  Float_t sinx(Float_t, Double_t *);

};




#endif /* ASYM_CALC_H */
