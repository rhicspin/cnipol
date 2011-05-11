/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "ChannelEvent.h"

#include "RunInfo.h"
#include "Kinema.h"


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
Float_t ChannelEvent::GetEnergyA() const
{
   UChar_t chId = GetChannelId();
   //return fEventConfig->fConfigInfo->data.chan[chId].acoef * fChannel.fAmpltd;
   return fEventConfig->fCalibrator->fChannelCalibs[chId].fACoef * fChannel.fAmpltd;

   //return (fEventConfig->fConfigInfo->data.WFDTUnit/2.) *
   //       (fChannel.fAmpltd + fEventConfig->fRandom->Rndm() - 0.5);
}


/** */
Float_t ChannelEvent::GetKinEnergyA()
{
   UChar_t chId  = GetChannelId();
   Float_t emeas = GetEnergyA();
   Float_t eloss = fEventConfig->fCalibrator->fChannelCalibs[chId].fAvrgEMiss;
   return  emeas + eloss;
}


/** */
Float_t ChannelEvent::GetKinEnergyAEDepend()
{
   UChar_t chId    = GetChannelId();
   Float_t emeas   = GetEnergyA();
   Float_t dlwidth = fEventConfig->fCalibrator->fChannelCalibs[chId].fDLWidth;
   return  ekin(emeas, dlwidth);
}


/** */
Float_t ChannelEvent::GetKinEnergyAEstimate()
{
   Float_t emeas = GetEnergyA();
   Float_t eloss = fEventConfig->fCalibrator->fChannelCalibs[0].fAvrgEMiss;
   return  emeas + eloss;
}


/** */
Float_t ChannelEvent::GetKinEnergyADLCorrEstimate()
{
   Float_t emeas = GetEnergyA();
   Float_t eloss = fEventConfig->fCalibrator->fChannelCalibs[0].fAvrgEMiss;
   Float_t eMeasDLCorr = fEventConfig->fCalibrator->fChannelCalibs[0].fEMeasDLCorr;
   //printf("emeas, eMeasDLCorr, eloss: %f, %f, %f\n", emeas, eMeasDLCorr, eloss);
   return  emeas*eMeasDLCorr + eloss;
   //return  emeas + eloss;
}


/** */
Float_t ChannelEvent::GetKinEnergyAEstimateEDepend()
{
   Float_t emeas   = GetEnergyA();
   Float_t dlwidth = fEventConfig->fCalibrator->fChannelCalibs[0].fDLWidth;
   return  ekin(emeas, dlwidth);
}


/** */
Float_t ChannelEvent::GetFunnyEnergyA()
{
   float   depoE  = GetEnergyA();
   float   funnyE = ekin(depoE, 60);
   return  funnyE;
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
Float_t ChannelEvent::GetTime() const
{
   //return (fEventConfig->fConfigInfo->data.WFDTUnit/2.) *
   return WFD_TIME_UNIT_HALF * (fChannel.fTdc + fEventConfig->fRandom->Rndm() - 0.5);
}


/** */
Float_t ChannelEvent::GetTimeOfFlight()
{
   UChar_t chId   = GetChannelId();
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
   UChar_t chId = GetChannelId();
   Float_t tof  = GetTimeOfFlight();
   Float_t mass = tof * tof * GetKinEnergyA() * gRunConsts[chId].T2M * k2G;
   return mass;
}


/** */
Float_t ChannelEvent::GetCarbonMassEstimate()
{
   UChar_t chId = GetChannelId();
   Float_t tof  = GetTimeOfFlightEstimate();
   Float_t mass = tof * tof * GetKinEnergyAEstimate() * gRunConsts[chId].T2M * k2G;
   return mass;
}


/** */
Float_t ChannelEvent::GetMandelstamT()
{
   return -2 * MASS_12C * k2G * k2G * GetKinEnergyAEstimate();
}


/** */
void ChannelEvent::Print(const Option_t* opt) const
{
   opt = "";

   //printf("ChannelEvent:");
   //printf("\n\t");
   //fEventId.Print();
   //printf("\n\t");
   //fChannel.Print();
   printf("%12.3f %12.3f\n", GetEnergyA(), GetTime());
   //printf("\n");
}


/** */
bool ChannelEvent::operator()(const ChannelEvent &ch1, const ChannelEvent &ch2)
{
   if (ch1.fEventId < ch2.fEventId) return true;

   return false;
}


/** */
Bool_t ChannelEvent::PassCutRawAlpha()
{
   // Do not consider channels other than silicon detectors
   //if (GetChannelId() > NSTRIP) return false;

   if (fChannel.fAmpltd < 50) return false;

   if (fChannel.fTdc < 15 || fChannel.fTdc > 50) return false;

   return true;
}


/** */
Bool_t ChannelEvent::PassCutDetectorChannel()
{
   // Do not consider channels other than silicon detectors
   if (GetChannelId() > NSTRIP) return false;

   ////if (fChannel.fAmpltd < 30) return false; // ADC
   ////if (fChannel.fAmpltd > 215) return false; // ADC

   //if (fChannel.fAmpltd < 15) return false; // ADC
   //if (fChannel.fAmpltd > 215) return false; // ADC

   //if (fChannel.fTdc < 12) return false; // TDC
   ////if (fChannel.fTdc > 65) return false; // TDC

   //if (fChannel.fTdc > 68) return false; // TDC

   return true;
}


/** */
Bool_t ChannelEvent::PassCutDepEnergyTime()
{
   switch (gRunInfo->fPolId) {

   case 0:   // B1U
      return true;
      break;

   case 1:   // Y1D
      if ( GetEnergyA() < 200 || GetEnergyA() > 1000)// || GetTime() < 15 || GetTime() > 75)
          //(GetTime() + gRunInfo->GetExpectedGlobalTimeOffset()) < 15 ||
          //(GetTime() + gRunInfo->GetExpectedGlobalTimeOffset()) > 75)
         return false;
      break;

   case 2:   // B2D
      if ( GetEnergyA() < 200 || GetEnergyA() > 1000)// || GetTime() < 15 || GetTime() > 75)
         return false;
      break;

   case 3:   // Y2U
      return true;
      break;
   }

   return true;
}


/** */
Bool_t ChannelEvent::PassCutKinEnergyADLCorrEstimate()
{
   if (GetKinEnergyADLCorrEstimate() < 400) return false;  // keV
   if (GetKinEnergyADLCorrEstimate() > 900) return false; // keV

   return true;
}


/** */
Bool_t ChannelEvent::PassQACutCarbonMass()
{
   UChar_t chId = GetChannelId();
   //float delta  = GetTimeOfFlightEstimate() - gRunConsts[chId].E2T/sqrt(GetKinEnergyAEstimate());
   float delta  = GetTimeOfFlightEstimate() - gRunConsts[chId].E2T/sqrt(GetKinEnergyAEstimateEDepend());
   //float delta = GetTime() - gRunConsts[].E2T/sqrt(GetEnergyA());
   
   if (fabs(delta) <= 20) return true; // in ns

   //if fabs(delta) < gRunConsts[].M2T * feedback.RMS[st] * gAnaInfo->MassSigma/sqrt(GetEnergyA());
   //if ( fabs(delta) < gRunConsts[].M2T * gAnaInfo->OneSigma * gAnaInfo->MassSigma / sqrt(GetEnergyA()) ) return true;

   return false;
}


/** */
Bool_t ChannelEvent::PassCutPulser()
{
   switch (gRunInfo->fPolId) {

   case 0:   // B1U
      if (fChannel.fAmpltd > 130 && fChannel.fAmpltd < 200 && fChannel.fTdc > 64)
         return false;
      break;

   case 1:   // Y1D
      if (fChannel.fAmpltd > 120 && fChannel.fAmpltd < 170 && fChannel.fTdc > 54)
         return false;
      break;

   case 2:   // B2D
      if (fChannel.fAmpltd > 130 && fChannel.fAmpltd < 210 && fChannel.fTdc > 50)
         return false;

      break;
   case 3:   // Y2U
      //if (fChannel.fAmpltd > 155 && fChannel.fAmpltd < 200 && fChannel.fTdc > 50)
      if (fChannel.fAmpltd > 140 && fChannel.fAmpltd < 200 && fChannel.fTdc > 50) // 15039.302
         return false;

      break;
   }

   return true;
}


/** */
Bool_t ChannelEvent::PassCutNoise()
{
   switch (gRunInfo->fPolId) {

   case 0:   // B1U
      //if ( (fChannel.fAmpltd < 50 && fChannel.fTdc < 35) || fChannel.fAmpltd > 215)
      //if (fChannel.fAmpltd < 35 || fChannel.fAmpltd > 215 || (fChannel.fAmpltd < 50 && fChannel.fTdc < 35))
      if ( fChannel.fAmpltd < 35 || fChannel.fAmpltd > 215 ||
           fabs( GetTdc() - ( fEventConfig->fRunInfo->fProtoCutSlope * GetAmpltd() + fEventConfig->fRunInfo->fProtoCutOffset) ) > 17 )
         return false;
      break;

   case 1:   // Y1D
      //if (fChannel.fAmpltd < 20 || fChannel.fAmpltd > 100 || (fChannel.fAmpltd < 30 && fChannel.fTdc < 40)) // based on 14958.101
      //if (fChannel.fAmpltd < 20 || fChannel.fAmpltd > 215 || (fChannel.fAmpltd < 50 && fChannel.fTdc < 50)) // prelim values for D outside the tunnel
      if ( fChannel.fAmpltd < 20 || fChannel.fAmpltd > 215 ||
           fabs( GetTdc() - ( fEventConfig->fRunInfo->fProtoCutSlope * GetAmpltd() + fEventConfig->fRunInfo->fProtoCutOffset) ) > 17 )
         return false;
      break;

   case 2:   // B2D
      //if (fChannel.fAmpltd < 20 || fChannel.fAmpltd > 220) // based on 14958.201
      //if (fChannel.fAmpltd < 20 || fChannel.fAmpltd > 100 || (fChannel.fAmpltd < 30 && fChannel.fTdc < 40)) // based on 15019.202
      //if (fChannel.fAmpltd < 20 || fChannel.fAmpltd > 130 || (fChannel.fAmpltd < 40 && fChannel.fTdc < 60)) // prelim values for BD outside the tunnel
      //if ( fChannel.fAmpltd < 20 || fChannel.fAmpltd > 130 ||
      //    (fChannel.fTdc < -0.28*fChannel.fAmpltd + 60) ||
      //    (fChannel.fTdc > -0.28*fChannel.fAmpltd + 90 ) )
      if ( GetAmpltd() < 20 || GetAmpltd() > 130 ||
           fabs( GetTdc() - ( fEventConfig->fRunInfo->fProtoCutSlope * GetAmpltd() + fEventConfig->fRunInfo->fProtoCutOffset) ) > 17 )
         return false;
      break;

   case 3:   // Y2U
      //if (fChannel.fAmpltd < 30 || fChannel.fAmpltd > 215 || (fChannel.fAmpltd < 50 && fChannel.fTdc < 44) ) // 15039.302
      if ( GetAmpltd() < 30 || GetAmpltd() > 215 ||
           fabs( GetTdc() - ( fEventConfig->fRunInfo->fProtoCutSlope * GetAmpltd() + fEventConfig->fRunInfo->fProtoCutOffset) ) > 17 )
         return false;
      break;
   }

   return true;
}


/** */
Bool_t ChannelEvent::PassCutEnabledChannel()
{
   UShort_t chId = GetChannelId();

   return !gRunInfo->fDisabledChannels[chId-1];
}


/** */
Bool_t ChannelEvent::PassCutTargetChannel()
{
   UShort_t chId = GetChannelId();

   //if (chId > gConfigInfo->data.NumChannels-4 && chId <= gConfigInfo->data.NumChannels)
   if (chId > NSTRIP && chId <= NSTRIP+4)
      return true;

  return false;
}
