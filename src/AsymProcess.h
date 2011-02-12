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

#include "TH1F.h"
#include "TH2F.h"

//#include "DrawObjContainer.h"

#include "AnaInfo.h"
#include "AsymGlobals.h"
#include "Kinema.h"
#include "AsymRoot.h"
#include "AsymRootGlobals.h"

/**
 * Main process event routine. This routine is called event by event basis
 * Input       : processEvent *event
 * Return      : 0
 */
void event_process(processEvent *event);

//
// Class name  : 
// Method name : KinemaReconstruction
//
// Description : calculate kinematics from ADC and TDC
// Input       : int Mode, processEvent *event, recordConfigRhicStruct *cfginfo, int st
// Return      : float &edepo, float &e, float &t, float &delt, float &Mass
//
void KinemaReconstruction(int Mode, processEvent *event, recordConfigRhicStruct *cfginfo, 
			 int st, float &edepo, float &e, float &t, float &delt, float &Mass);


// Class name  : 
// Method name : float ekin(float edep, float dthick)
//
// Description : Function to convert energy deposit (edep) to 12C kinetic 
//             : energy for given deadlayer thickness (dthick)
// Input       : float edep, float dthick
// Return      : Ekin = p0 + p1*Edep + p2*Edep^2 + p3*Edep^3 + p4*Edep^4
//
float ekin(float edep, float dtheck);

//
// Class name  : 
// Method name : SpinTuneOutput(int bid, double si)
//
// Description : output routine for spin tune measurements upon request by Mei Bai
// Input       : int bid, double si
// Return      : 
//
void SpinTuneOutput(int bid, double si);

#endif
