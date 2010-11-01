/**
 *
 * 15 Oct, 2010 - Dmitri Smirnov
 *    - Modified readloop to take an object of Root class as an argument 
 *
 * 18 Oct, 2010 - Dmitri Smirnov
 *    - Made the Root object global, redefined readloop back
 *
 */

#ifndef ASYM_READ_H
#define ASYM_READ_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <iostream>
#include <fstream>

#include "rhicpol.h"
#include "rpoldata.h"

#include "Asym.h"
#include "AsymProcess.h"
#include "AsymRecover.h"
//#include "AsymROOT.h"
#include "AsymROOTGlobals.h"

int readloop();

#endif
