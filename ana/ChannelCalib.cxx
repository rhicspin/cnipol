/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "ChannelCalib.h"

ClassImp(ChannelCalib)

using namespace std;

/** Default constructor. */
ChannelCalib::ChannelCalib() : TObject(), fACoef(-1), fACoefErr(-1), fAChi2Ndf(0),
   fICoef(-1), fICoefErr(-1), fIChi2Ndf(0), fDLCoef(0), fDLCoefErr(0), fT0Coef(0),
   fT0CoefErr(0), fAvrgEMiss(0), fAvrgEMissErr(0), fBananaChi2Ndf(0)
{
}


/** Default destructor. */
ChannelCalib::~ChannelCalib()
{
}


/**
 *
 */
void ChannelCalib::Print(const Option_t* opt) const
{
   opt = "";

   //printf("ChannelCalib:\n");
   printf("fACoef, fACoefErr, fAChi2Ndf, fICoef, fICoefErr, fIChi2Ndf, fDLCoef, \
          fDLCoefErr, fT0Coef, fT0CoefErr, fAvrgEMiss, fAvrgEMissErr, \
          fBananaChi2Ndf\n");
   printf("%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n", fACoef,
          fACoefErr, fAChi2Ndf, fICoef, fICoefErr, fIChi2Ndf, fDLCoef,
          fDLCoefErr, fT0Coef, fT0CoefErr, fAvrgEMiss, fAvrgEMissErr,
          fBananaChi2Ndf);

}
