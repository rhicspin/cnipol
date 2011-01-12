/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "ChannelEvent.h"

ClassImp(ChannelEvent)

using namespace std;

/** Default constructor. */
ChannelEvent::ChannelEvent() : TObject(), fEventConfig(0), fEventId(),
   fChannel()
{
}


/** Default destructor. */
ChannelEvent::~ChannelEvent()
{
}


/** */
Float_t ChannelEvent::GetEnergyA()
{
   UChar_t chId = fEventId.fChannelId + 1;
   //return fEventConfig->fConfigInfo->data.chan[chId].acoef * fChannel.fAmpltd;
   return fEventConfig->fCalibrator->fChannelCalibs[chId].fACoef * fChannel.fAmpltd;

   //return (fEventConfig->fConfigInfo->data.WFDTUnit/2.) *
   //       (fChannel.fAmpltd + fEventConfig->fRandom->Rndm() - 0.5);
}


/** */
Float_t ChannelEvent::GetKinEnergyA()
{
   UChar_t chId  = fEventId.fChannelId + 1;
   Float_t acoef = fEventConfig->fCalibrator->fChannelCalibs[chId].fACoef;
   Float_t eloss = fEventConfig->fCalibrator->fChannelCalibs[chId].fAvrgEMiss;
   return  acoef * fChannel.fAmpltd + eloss;
}


/** */
Float_t ChannelEvent::GetKinEnergyAEstimate()
{
   UChar_t chId  = fEventId.fChannelId + 1;
   Float_t acoef = fEventConfig->fCalibrator->fChannelCalibs[chId].fACoef;
   Float_t eloss = fEventConfig->fCalibrator->fChannelCalibs[0].fAvrgEMiss;
   return  acoef * fChannel.fAmpltd + eloss;
}


/** */
Float_t ChannelEvent::GetFunnyEnergyA()
{
   UChar_t chId = fEventId.fChannelId + 1;
   float depoE  = fEventConfig->fCalibrator->fChannelCalibs[chId].fACoef * fChannel.fAmpltd;
   float funnyE = ekin(depoE, 60);
   return funnyE;
}


/** */
Float_t ChannelEvent::GetEnergyI()
{
   UChar_t chId = fEventId.fChannelId + 1;
   // XXX acoef has to be changed to ... icoef
   //return fEventConfig->fConfigInfo->data.chan[chId].acoef * fChannel.fIntgrl;
   return fEventConfig->fCalibrator->fChannelCalibs[chId].fICoef * fChannel.fIntgrl;
}


/** */
Float_t ChannelEvent::GetTime()
{
   return (fEventConfig->fConfigInfo->data.WFDTUnit/2.) *
          (fChannel.fTdc + fEventConfig->fRandom->Rndm() - 0.5);
          //(fChannel.fTdc);
}


/** */
Float_t ChannelEvent::GetTimeOfFlight()
{
   UChar_t chId   = fEventId.fChannelId + 1;
   Float_t t0coef = fEventConfig->fCalibrator->fChannelCalibs[chId].fT0Coef;

   return GetTime() + t0coef;
}


/** */
Float_t ChannelEvent::GetTimeOfFlightEstimate()
{
   Float_t t0coef = fEventConfig->fCalibrator->fChannelCalibs[0].fT0Coef;

   return GetTime() + t0coef;
}


/** */
Float_t ChannelEvent::GetCarbonMass()
{
   UChar_t chId = fEventId.fChannelId + 1;
   Float_t tof  = GetTimeOfFlight();
   Float_t mass = tof * tof * GetKinEnergyA() * gRunConsts[chId].T2M * k2G;
   return mass;
}


/** */
Float_t ChannelEvent::GetCarbonMassEstimate()
{
   UChar_t chId = fEventId.fChannelId + 1;
   Float_t tof  = GetTimeOfFlightEstimate();
   Float_t mass = tof * tof * GetKinEnergyAEstimate() * gRunConsts[chId].T2M * k2G;
   return mass;
}


/** */
void ChannelEvent::Print(const Option_t* opt) const
{
   opt = "";

   printf("ChannelEvent:");
   printf("\n\t");
   fEventId.Print();
   printf("\n\t");
   fChannel.Print();
   printf("\n");
}


/** */
bool ChannelEvent::operator()(const ChannelEvent &ch1, const ChannelEvent &ch2)
{
   if (ch1.fEventId < ch2.fEventId) return true;

   return false;
}


/** */
Bool_t ChannelEvent::PassQACutRaw()
{
   //if (fChannel.fAmpltd < 30) return false; // ADC
   //if (fChannel.fAmpltd > 215) return false; // ADC

   if (fChannel.fAmpltd < 15) return false; // ADC
   if (fChannel.fAmpltd > 215) return false; // ADC

   if (fChannel.fTdc < 12) return false; // TDC
   //if (fChannel.fTdc > 65) return false; // TDC

   if (fChannel.fTdc > 68) return false; // TDC

   UChar_t chId = fEventId.fChannelId + 1;
   
   if (chId >= NSTRIP) return false;
   if (chId <  0)      return false;

   return true;
}


/** */
Bool_t ChannelEvent::PassQACut1()
{
   //if (GetEnergyA() < 150) return false;  // keV
   if (GetEnergyA() > 1150) return false; // keV

   if (GetTime() < 15) return false; // ns
   if (GetTime() > 75) return false; // ns

   return true;
}


/** */
Bool_t ChannelEvent::PassQACutCarbonMass()
{
   UChar_t chId = fEventId.fChannelId + 1;
   float delta  = GetTimeOfFlightEstimate() - gRunConsts[chId].E2T/sqrt(GetKinEnergyAEstimate());
   //float delta = GetTime() - gRunConsts[].E2T/sqrt(GetEnergyA());
   
   if (fabs(delta) <= 12) return true; // in ns

   //if fabs(delta) < gRunConsts[].M2T * feedback.RMS[st] * dproc.MassSigma/sqrt(GetEnergyA());
   //if ( fabs(delta) < gRunConsts[].M2T * dproc.OneSigma * dproc.MassSigma / sqrt(GetEnergyA()) ) return true;

   return false;
}
