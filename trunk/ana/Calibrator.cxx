/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "Calibrator.h"

#include "Asym.h"
#include "AsymGlobals.h"
#include "MeasInfo.h"


ClassImp(Calibrator)

using namespace std;


/** Default constructor. */
Calibrator::Calibrator() : TObject(), fRandom(new TRandom()), fChannelCalibs()
{
}


/** */
Calibrator::Calibrator(TRandom *random) : TObject(), fRandom(random), fChannelCalibs()
{
}


/** Default destructor. */
Calibrator::~Calibrator()
{
}


/** */
ChannelCalib* Calibrator::GetAverage()
{ //{{{
	//Info("GetAverage", "Executing GetAverage()");

   ChannelCalib *ch = new ChannelCalib();
   ch->ResetToZero();

   ChannelCalibMap::const_iterator mi;
   ChannelCalibMap::const_iterator mb = fChannelCalibs.begin();
   ChannelCalibMap::const_iterator me = fChannelCalibs.end();

   UInt_t nChannels = 0;

   for (mi=mb; mi!=me; ++mi) {
      if (gMeasInfo->IsSiliconChannel(mi->first) && !isnan(mi->second.fT0Coef) && !isinf(mi->second.fT0Coef) &&
          !gMeasInfo->fDisabledChannels[mi->first-1] && mi->second.fBananaChi2Ndf < 1e3)
      {
         ch->fT0Coef        += mi->second.fT0Coef;
         ch->fT0CoefErr     += mi->second.fT0CoefErr;
         ch->fDLWidth       += mi->second.fDLWidth;
         ch->fDLWidthErr    += mi->second.fDLWidthErr;
         ch->fBananaChi2Ndf += mi->second.fBananaChi2Ndf;
         nChannels++;
      }
   }

   if (nChannels) {
      ch->fT0Coef        /= nChannels;
      ch->fT0CoefErr     /= nChannels;
      ch->fDLWidth       /= nChannels;
      ch->fDLWidthErr    /= nChannels;
      ch->fBananaChi2Ndf /= nChannels;
   }

   return ch;
} //}}}


/** */
void Calibrator::CopyAlphaCoefs(Calibrator &calibrator)
{ //{{{
   ChannelCalibMap::const_iterator mi;
   ChannelCalibMap::const_iterator mb = calibrator.fChannelCalibs.begin();
   ChannelCalibMap::const_iterator me = calibrator.fChannelCalibs.end();

   for (mi=mb; mi!=me; ++mi) {

      ChannelCalibMap::iterator iChCalib = fChannelCalibs.find(mi->first);

      if (iChCalib != fChannelCalibs.end()) {
         iChCalib->second.CopyAlphaCoefs(mi->second);
      } else {
         ChannelCalib newChCalib;
         newChCalib.CopyAlphaCoefs(mi->second);
         fChannelCalibs[mi->first] = newChCalib;
      }
   }
} //}}}


/** */
void Calibrator::Calibrate(DrawObjContainer *c)
{ //{{{
	Info("Calibrate", "Executing Calibrate()");
} //}}}


/** */
void Calibrator::CalibrateFast(DrawObjContainer *c)
{ //{{{
	Info("CalibrateFast", "Executing CalibrateFast()");
} //}}}


/** */
Float_t Calibrator::GetDLWidth(UShort_t chId) const
{ //{{{
   return fChannelCalibs.find(chId)->second.fDLWidth;
} //}}}


/** */
Float_t Calibrator::GetDLWidthErr(UShort_t chId) const
{ //{{{
   return fChannelCalibs.find(chId)->second.fDLWidthErr;
} //}}}


/** */
Float_t Calibrator::GetT0Coef(UShort_t chId) const
{ //{{{
   return fChannelCalibs.find(chId)->second.fT0Coef;
} //}}}


/** */
Float_t Calibrator::GetBananaChi2Ndf(UShort_t chId) const
{ //{{{
   return fChannelCalibs.find(chId)->second.fBananaChi2Ndf;
} //}}}


/** */
UInt_t Calibrator::GetFitStatus(UShort_t chId) const
{ //{{{
   ChannelCalibMapConstIter iChCalib = fChannelCalibs.find(chId);

   if (iChCalib != fChannelCalibs.end()) {
      return iChCalib->second.GetFitStatus();
   }

   return kUNKNOWN;
} //}}}


/** */
Float_t Calibrator::GetT0CoefErr(UShort_t chId) const
{ //{{{
   return fChannelCalibs.find(chId)->second.fT0CoefErr;
} //}}}


/** */
Float_t Calibrator::GetEnergyA(UShort_t adc, UShort_t chId) const
{ //{{{
   return fChannelCalibs.find(chId)->second.fACoef * adc;
} //}}}


/** */
Float_t Calibrator::GetKinEnergyA(UShort_t adc, UShort_t chId) const
{ //{{{
   Float_t emeas = GetEnergyA(adc, chId);
   Float_t eloss = fChannelCalibs.find(chId)->second.fAvrgEMiss;
   return  emeas + eloss;
} //}}}


/** */
Float_t Calibrator::GetTime(UShort_t tdc) const
{ //{{{
   //return WFD_TIME_UNIT_HALF * (tdc + fRandom->Rndm() - 0.5);
   return WFD_TIME_UNIT_HALF * tdc;
} //}}}


/** */
Float_t Calibrator::GetTimeOfFlight(UShort_t tdc, UShort_t chId) const
{ //{{{
   Float_t t0coef = fChannelCalibs.find(chId)->second.fT0Coef;
   return GetTime(tdc) + t0coef;
} //}}}


/** */
TFitResultPtr Calibrator::CalibrateOld(TH1 *h, TH1D *hMeanTime, UShort_t chId, Bool_t wideLimits)
{ //{{{
   return 0;
} //}}}


/** */
void Calibrator::Calibrate(TH1 *h, TH1D *hMeanTime, UShort_t chId, Bool_t wideLimits)
{ //{{{
   return;
} //}}}


/** */
void Calibrator::Print(const Option_t* opt) const
{ //{{{
   Info("Print", "Calibrator members:");

   printf("              fACoef fACoefErr fAChi2Ndf    fICoef fICoefErr fIChi2Ndf fDLWidth " \
          "fDLWidthErr fT0Coef fT0CoefErr fAvrgEMiss fAvrgEMissErr " \
          "fBananaChi2Ndf fFitStatus\n");

   ChannelCalibMap::const_iterator mi;
   ChannelCalibMap::const_iterator mb = fChannelCalibs.begin();
   ChannelCalibMap::const_iterator me = fChannelCalibs.end();

   for (mi=mb; mi!=me; mi++) {
	   printf("Channel %2d: ", mi->first);
		mi->second.Print();
      //printf("\n");
   }
} //}}}


/** */
void Calibrator::PrintAsPhp(FILE *f) const
{ //{{{
   ChannelCalibMap::const_iterator mi;
   ChannelCalibMap::const_iterator mb = fChannelCalibs.begin();
   ChannelCalibMap::const_iterator me = fChannelCalibs.end();

   for (mi=mb; mi!=me; mi++) {
  
      UShort_t chId = mi->first;

      fprintf(f, "$rc['calib'][%d] = ", chId);
      mi->second.PrintAsPhp(f);
      fprintf(f, ";\n");
   }
} //}}}


/** */
void Calibrator::PrintAsConfig(FILE *f) const
{ //{{{
   ChannelCalibMap::const_iterator mi;
   ChannelCalibMap::const_iterator mb = fChannelCalibs.begin();
   ChannelCalibMap::const_iterator me = fChannelCalibs.end();

   UShort_t chId;
   const ChannelCalib *ch;

   for (mi=mb; mi!=me; mi++) {
  
      chId =  mi->first;
      ch   = &mi->second;

      fprintf(f, "Channel%02d=%5.3f %5.3f %7.1f %4.1f %5.2f %5.3f %4.1f %4.1f %4.3G %4.3G %4.3G %4.3G %4.3G\n",
         chId, -1*ch->fT0Coef, ch->fACoef*ch->fEMeasDLCorr, ch->fAvrgEMiss,
         10., 100., ch->fACoef, 0., 0., 0., 0., 0., 0., 0.);
   }

	// XXX need to add 8 more default channels here

} //}}}
