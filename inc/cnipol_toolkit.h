#ifndef cnipol_toolkit_h
#define cnipol_toolkit_h

#include "rhicpol.h"
#include "rpoldata.h"

#include "AsymGlobals.h"

namespace cnipol_toolkit {

void KinemaReconstruction(int Mode, processEvent *event, recordConfigRhicStruct *cfginfo, int st, float &edepo,
   float &e, float &t, float &delt, float &Mass);
void SpinTuneOutput(int bid, double si);

}

#endif
