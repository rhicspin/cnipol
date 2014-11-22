// Header file for AsymProcess
//
// Author   : Itaru Nakagawa
//            Dmitri Smirnov
//
// Creation : 07/26/2006         
//

#ifndef ASYM_PROCESS_H
#define ASYM_PROCESS_H

#include "rhicpol.h"
#include "rpoldata.h"

#include "AsymGlobals.h"


// Description : calculate kinematics from ADC and TDC
// Input       : int Mode, processEvent *event, recordConfigRhicStruct *cfginfo, int st
// Return      : float &edepo, float &e, float &t, float &delt, float &Mass
void KinemaReconstruction(int Mode, processEvent *event, recordConfigRhicStruct *cfginfo, 
			 int st, float &edepo, float &e, float &t, float &delt, float &Mass);

// Description : output routine for spin tune measurements upon request by Mei Bai
// Input       : int bid, double si
void SpinTuneOutput(int bid, double si);

#endif
