/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "Calibrator.h"
#include "ChannelCalib.h"

ClassImp(Calibrator)

using namespace std;

/** Default constructor. */
Calibrator::Calibrator() : TObject(), fChannelCalibs()
{
}


/** Default destructor. */
Calibrator::~Calibrator()
{
}


/** */
void Calibrator::Calibrate(DrawObjContainer *c)
{
	Info("Calibrate", "Executing Calibrate()");
}


/** */
void Calibrator::CalibrateFast(DrawObjContainer *c)
{
	Info("CalibrateFast", "Executing CalibrateFast()");
}


/** */
TFitResultPtr Calibrator::Calibrate(TH1 *h, TH1D *hMeanTime, Bool_t wideLimits)
{
   return 0;
}


/** */
void Calibrator::Print(const Option_t* opt) const
{ //{{{
   opt = "";

   printf("Calibrator:\n");

   ChannelCalibMap::const_iterator mi;
   ChannelCalibMap::const_iterator mb = fChannelCalibs.begin();
   ChannelCalibMap::const_iterator me = fChannelCalibs.end();

   for (mi=mb; mi!=me; mi++) {
		mi->second.Print();
      printf("\n");
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
