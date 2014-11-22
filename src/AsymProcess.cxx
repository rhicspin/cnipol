#include "AsymProcess.h"

#include "TH1F.h"

#include "AnaInfo.h"
#include "AsymRoot.h"
#include "Kinema.h"
#include "MeasInfo.h"

using namespace std;


// Description : calculate kinematics from ADC and TDC
// Input       : int Mode, processEvent *event, recordConfigRhicStruct *gConfigInfo, int st
// Return      : float &edepo, float &e, float &t, float &delt, float &Mass
void KinemaReconstruction(int Mode, processEvent *event, recordConfigRhicStruct *gConfigInfo,
                     int st, float &edepo, float &e, float &t, float &delt, float &Mass)
{
  float rand1 = (float)rand()/(float)RAND_MAX;
  float rand2 = (float)rand()/(float)RAND_MAX;

  // random numbers in order to smear for the integer reading
  //int vlen = 1;
  //hhrammar_(&rand1, &vlen);
  //hhrammar_(&rand2, &vlen);

  // Energy deposit
  edepo = gConfigInfo->data.chan[st].acoef * (event->amp+rand2-0.5);

  // ToF in [ns]
  t = gRunConsts[st+1].Ct * (event->tdc + rand1 - 0.5) - gConfigInfo->data.chan[st].t0 - gAsymAnaInfo->tshift;

  // Kinetic energy assuming Carbon
  e = ekin(edepo, gConfigInfo->data.chan[st].dwidth);

  // Decrepancy between observed ToF and calculated t from energy
  delt = t - gRunConsts[st+1].E2T/sqrt(e);
}


// Description : output routine for Mei Bai's spin tune measurements
//             : The output should be stderr. Following two commands
//             : creat data file without unnecessary Warnings.
//             : 1) Asym -f 9961.002 -b 2> tmp.dat
//             : 2) grep -v "Warning" tmp.dat > SpinTune9961.002.dat
//             :
// Input       : int bid, double si
// Ouput       : revolusion#, bid, si
//             : output Bunch ID starts from 1, not zero
// Return      : 0
//
void SpinTuneOutput(int bid, double si)
{
   fprintf(stderr, "%10ld", cntr.revolution);
   fprintf(stderr,   "%5d", bid+1);
   fprintf(stderr,   "%5d", gMeasInfo->GetBunchSpin(bid + 1) );
   fprintf(stderr,   "%5d", int(si)+1);
 
   /*
   si==0 ? fprintf(stderr,"%5d",1) : fprintf(stderr,"%5d",0);
   si==1 ? fprintf(stderr,"%5d",1) : fprintf(stderr,"%5d",0);
   si==2 ? fprintf(stderr,"%5d",1) : fprintf(stderr,"%5d",0);
   si==3 ? fprintf(stderr,"%5d",1) : fprintf(stderr,"%5d",0);
   si==4 ? fprintf(stderr,"%5d",1) : fprintf(stderr,"%5d",0);
   si==5 ? fprintf(stderr,"%5d",1) : fprintf(stderr,"%5d",0);
   */
 
   fprintf(stderr, "\n");
}
