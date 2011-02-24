#ifndef AsymHbook_h
#define AsymHbook_h

#include <math.h>
#include <iostream>

#include "rhicpol.h"
#include "rpoldata.h"

#include "AnaInfo.h"
#include "AsymGlobals.h"
#include "TargetInfo.h"

void tgtHistBook();
void hist_book(char *filename);
void hist_close(char *filename);

#endif
