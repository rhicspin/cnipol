/**
 *
 * 15 Oct, 2010 - Dmitri Smirnov
 *    - Modified readloop to take an object of Root class as an argument 
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
#include "AsymProcess.h"
#include "AsymRecover.h"
#include "AsymROOT.h"

int readloop(Root *root=0);

#endif
