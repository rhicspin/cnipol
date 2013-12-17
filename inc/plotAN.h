#ifndef plotAN_h
#define plotAN_h

#include "TEllipse.h"
#include "TFitResult.h"
#include "TGraph.h"
#include "TGraphErrors.h"

const double MASS_PROTON   = 0.93827205;
const double ALPHA         = 7.2973525698E-3; // 1/137.035999074(44)
const double MAGMOM_PROTON = 2.792847356;

void plotAN();
Double_t modelAN(Double_t *x, Double_t *par);
//void f_AN(double cal_t[], double cal_AN[], double rho, double B, double sigma, double Imr5, double Rer5, double scale);
void f_AN(double cal_t[], double cal_AN[], double rho, double sigma, double Imr5, double Rer5, double scale);
TFitResultPtr FitGraph(TGraph *gr, TF1 *func, TEllipse *ell);

#endif
