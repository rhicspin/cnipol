#ifndef plotAN_h
#define plotAN_h

void plotAN();
Double_t fitfunc3(Double_t *x, Double_t *par);
//void f_AN(double cal_t[], double cal_AN[], double rho, double B, double sigma, double Imr5, double Rer5, double fac);
void f_AN(double cal_t[], double cal_AN[], double rho, double sigma, double Imr5, double Rer5, double fac);

#endif
