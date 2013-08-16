/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "ChannelEvent.h"

#include "MeasInfo.h"
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

   return fEventConfig->fCalibrator->fChannelCalibs[chId].fAmAmp.fCoef * fChannel.fAmpltd;
   //return fEventConfig->fCalibrator->fChannelCalibs[chId].fAmAmp.fCoef *
   //       (fChannel.fAmpltd + (fEventConfig->fRandom->Rndm() - 0.5));

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
Float_t ChannelEvent::GetKinEnergyAEDependAverage()
{
   Float_t emeas   = GetEnergyA();
   Float_t dlwidth = fEventConfig->fCalibrator->fChannelCalibs[0].fDLWidth;
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
   float   emeas  = GetEnergyA();
   float   funnyE = ekin(emeas, 60);
   return  funnyE;
}


/** */
Float_t ChannelEvent::GetEnergyI()
{
   UChar_t chId = fEventId.fChannelId + 1;
   // XXX acoef has to be changed to ... icoef
   //return fEventConfig->fConfigInfo->data.chan[chId].acoef * fChannel.fIntgrl;
   return fEventConfig->fCalibrator->fChannelCalibs[chId].fAmInt.fCoef * fChannel.fIntgrl;
}


/** */
Float_t ChannelEvent::GetTime() const
{
   //return (fEventConfig->fConfigInfo->data.WFDTUnit/2.) *
   return WFD_TIME_UNIT_HALF * (fChannel.fTdc + fEventConfig->fRandom->Rndm() - 0.5);
}


/** */
Float_t ChannelEvent::GetTime2() const
{
   return WFD_TIME_UNIT_HALF * fChannel.fTdc;
}


/** */
Float_t ChannelEvent::GetTimeOfFlight()
{
   UChar_t chId   = GetChannelId();
   Float_t t0coef = fEventConfig->fCalibrator->fChannelCalibs[chId].fT0Coef;

   return GetTime() + t0coef;
}


/** */
Float_t ChannelEvent::GetTimeOfFlight2()
{
   UChar_t chId   = GetChannelId();
   Float_t t0coef = fEventConfig->fCalibrator->fChannelCalibs[chId].fT0Coef;

   return GetTime2() + t0coef;
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
   Float_t mass = tof * tof * GetKinEnergyAEDepend() * gRunConsts[chId].T2M * k2G;
   return mass;
}


/** */
Float_t ChannelEvent::GetCarbonMassEstimate()
{
   //UChar_t chId = GetChannelId();
   Float_t tof  = GetTimeOfFlightEstimate();
   Float_t mass = tof * tof * GetKinEnergyAEstimate() * gRunConsts[0].T2M * k2G;
   return mass;
}


/** */
Float_t ChannelEvent::GetMandelstamT()
{
   return -2 * MASS_12C * k2G * GetKinEnergyAEstimate() * k2G; // t = -2ME
}


/** */
Float_t ChannelEvent::GetTdcAdcTimeDiff()
{
   UChar_t chId  = GetChannelId();
   //Float_t delta = GetTimeOfFlight() - gRunConsts[chId].E2T/sqrt(GetKinEnergyAEDepend());
   Float_t delta = GetTimeOfFlight2() - gRunConsts[chId].E2T/sqrt(GetKinEnergyAEDepend());

   return delta;

   //if ( TMath::Abs(delta) < 3) {
   //   return 0;
   //} else if (delta >= 3)
   //   return  +1;
   //} else if (delta <= -3) {
   //   return  -1;
   //} else
   //   return 2;

}


/** */
void ChannelEvent::Print(const Option_t* opt) const
{
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


/** Returns true for silicon channels. */
Bool_t ChannelEvent::PassCutSiliconChannel()
{
   if ( gMeasInfo->IsSiliconChannel(GetChannelId()) )
      return true;

   return false;
}


/** */
Bool_t ChannelEvent::PassCutDepEnergyTime()
{
   switch (gMeasInfo->fPolId) {

   case 0:   // B1U
      return true;
      break;

   case 1:   // Y1D
      if ( GetEnergyA() < 200 || GetEnergyA() > 1000)// || GetTime() < 15 || GetTime() > 75)
          //(GetTime() + gMeasInfo->GetExpectedGlobalTimeOffset()) < 15 ||
          //(GetTime() + gMeasInfo->GetExpectedGlobalTimeOffset()) > 75)
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
Bool_t ChannelEvent::PassCutKinEnergyAEDepend()
{
   if (GetKinEnergyAEDepend() < 400) return false;  // keV
   if (GetKinEnergyAEDepend() > 900) return false; // keV

   return true;
}


/** */
Bool_t ChannelEvent::PassCutKinEnergyAEDependAverage()
{
   if (GetKinEnergyAEDependAverage() < 400) return false;  // keV
   if (GetKinEnergyAEDependAverage() > 900) return false; // keV

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
Bool_t ChannelEvent::PassCutCarbonMass()
{
   //if (fabs(GetCarbonMass() - MASS_12C * k2G) < 1.0*CARBON_MASS_PEAK_SIGMA*k2G )
   if (fabs(GetCarbonMass() - MASS_12C * k2G) < 2.0*CARBON_MASS_PEAK_SIGMA*k2G )
      return true;

   return false;
}


/** */
Bool_t ChannelEvent::PassCutCarbonMassEstimate()
{
   //UChar_t chId = GetChannelId();
   //float delta  = GetTimeOfFlightEstimate() - gRunConsts[chId].E2T/sqrt(GetKinEnergyAEstimate());
   //float delta  = GetTimeOfFlightEstimate() - gRunConsts[chId].E2T/sqrt(GetKinEnergyAEstimateEDepend());
   //float delta = GetTime() - gRunConsts[].E2T/sqrt(GetEnergyA());
   //if (fabs(delta) <= 20) return true; // in ns
   //if fabs(delta) < gRunConsts[].M2T * feedback.RMS[st] * gAnaInfo->MassSigma/sqrt(GetEnergyA());
   //if ( fabs(delta) < gRunConsts[].M2T * gAnaInfo->OneSigma * gAnaInfo->MassSigma / sqrt(GetEnergyA()) ) return true;

   if (fabs(GetCarbonMassEstimate() - MASS_12C * k2G) < 3.0*CARBON_MASS_PEAK_SIGMA*k2G )
      return true;

   return false;
}


/** */
Bool_t ChannelEvent::PassCutPulser()
{
   if ( GetAmpltd() < gMeasInfo->GetPulserCutAdcMax() &&
        GetAmpltd() > gMeasInfo->GetPulserCutAdcMin() &&
        GetTdc() < gMeasInfo->GetPulserCutTdcMax() &&
        GetTdc() > gMeasInfo->GetPulserCutTdcMin()
      )
      return false;

   return true;

   //switch (gMeasInfo->fPolId) {
   //case 0:   // B1U
   //   if (fChannel.fAmpltd > 130 && fChannel.fAmpltd < 200 && fChannel.fTdc > 64)
   //      return false;
   //   break;
   //case 1:   // Y1D
   //   if (fChannel.fAmpltd > 120 && fChannel.fAmpltd < 170 && fChannel.fTdc > 54)
   //      return false;
   //   break;
   //case 2:   // B2D
   //   if (fChannel.fAmpltd > 130 && fChannel.fAmpltd < 210 && fChannel.fTdc > 50)
   //      return false;
   //   break;
   //case 3:   // Y2U
   //   //if (fChannel.fAmpltd > 155 && fChannel.fAmpltd < 200 && fChannel.fTdc > 50)
   //   if (fChannel.fAmpltd > 140 && fChannel.fAmpltd < 200 && fChannel.fTdc > 50) // 15039.302
   //      return false;
   //   break;
   //}
   //return true;
}


/** */
Bool_t ChannelEvent::PassCutNoise()
{
   Double_t extraOffset = 0;

   if ( gMeasInfo->GetBeamEnergy() != kINJECTION)
      extraOffset = -8; // 8 TDC units ~= ? ns, 6 TDC units ~= 8 ns

   if ( GetAmpltd() < gMeasInfo->GetProtoCutAdcMin() ||
        GetAmpltd() > gMeasInfo->GetProtoCutAdcMax() ||
        GetTdc() < gMeasInfo->GetProtoCutTdcMin() ||
        GetTdc() > gMeasInfo->GetProtoCutTdcMax() ||
        fabs( GetTdc() - ( gMeasInfo->GetProtoCutSlope() * GetAmpltd() + gMeasInfo->GetProtoCutOffset() + extraOffset) ) >
           gMeasInfo->GetProtoCutWidth()
      )
      return false;

   return true;
}


/** */
Bool_t ChannelEvent::PassCutEnabledChannel()
{
   return !gMeasInfo->IsDisabledChannel(GetChannelId());
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


/** */
Bool_t ChannelEvent::PassCutPmtChannel() const
{
   UShort_t chId = GetChannelId();

   if ( gMeasInfo->IsPmtChannel(chId) )
      return true;

  return false;
}


/** */
Bool_t ChannelEvent::IsSpinFlipperMarkerChannel() const
{
   UShort_t chId = GetChannelId();

   if ( fEventConfig->GetMeasInfo()->IsSpinFlipperMarkerChannel(chId) )
      return true;

  return false;
}


/** */
Bool_t ChannelEvent::PassCutHamaChannel()
{
   UShort_t chId = GetChannelId();

   if ( gMeasInfo->IsHamaChannel(chId) )
      return true;

  return false;
}


/** */
Bool_t ChannelEvent::PassCutPmtNoise()
{
   if ( GetAmpltd() < 50 || GetAmpltd() > 150)
      return false;

   return true;
}


/** */
Bool_t ChannelEvent::PassCutEmptyBunch() const
{
   Int_t bId = GetBunchId() + 1;

   return gMeasInfo->IsEmptyBunch(bId) ? true : false;
}
