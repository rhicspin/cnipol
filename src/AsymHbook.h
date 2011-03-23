#ifndef AsymHbook_h
#define AsymHbook_h

#include <math.h>
#include <iostream>

#include "rhicpol.h"
#include "rpoldata.h"

#include "AsymGlobals.h"

void  HHBOOK1(int hid, char* hname, int xnbin, float xmin, float xmax) ;
void  HHPAK(int, float*);
void  HHPAKE(int, float*);
void  HHF1(int, float, float);
//void  HHKIND(int, int*, char*);
float HHMAX(int);
float HHSTATI(int hid, int icase, char * choice, int num);
void  HHFITHN(int hid, char*chfun, char*chopt, int np, float*par, float*step, float*pmin, float*pmax, float*sigpar, float&chi2);
//void  HHFITH(int hid, char*fun, char*chopt, int np, float*par, float*step, float*pmin, float*pmax, float*sigpar, float&chi2);

void tgtHistBook();
void hist_book(char *filename);
void hist_close(char *filename);

#endif
