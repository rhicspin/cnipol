#include "cnipol_toolkit.h"

#include "AnaInfo.h"
#include "AsymRoot.h"
#include "Kinema.h"
#include "MeasInfo.h"


namespace cnipol_toolkit {


/**
 * Calculates kinematic energy from measured ADC and TDC values.
 *
 * \param[in]  chId   Polarimeter channel id starting with 1
 * \param[out] edepo  Energy deposited in the detector strip
 * \param[out] e      Kinetic energy of a carbon particle
 * \param[out] t      Time of flight in ns
 * \param[out] delt   Difference between the observed and 
 *
 * \author Itaru Nakagawa
 * \author Dmitri Smirnov <d.s@plexoos.com>
 */
void KinemaReconstruction(processEvent *event, recordConfigRhicStruct *gConfigInfo,
                     int chId, float &edepo, float &e, float &t, float &delt, float &Mass)
{
  float rand1 = (float)rand()/(float)RAND_MAX;
  float rand2 = (float)rand()/(float)RAND_MAX;

  // Deposited energy
  edepo = gConfigInfo->data.chan[chId-1].acoef * (event->amp+rand2-0.5);

  // ToF in [ns]
  t = gRunConsts[chId].Ct * (event->tdc + rand1 - 0.5) - gConfigInfo->data.chan[chId-1].t0 - gAsymAnaInfo->tshift;

  // Kinetic energy assuming Carbon
  e = ekin(edepo, gConfigInfo->data.chan[chId-1].dwidth);

  // Decrepancy between observed ToF and calculated t from energy
  delt = t - gRunConsts[chId].E2T/sqrt(e);
}

}
