/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "ChannelCalib.h"

ClassImp(ChannelCalib)

using namespace std;

/** Default constructor. */
ChannelCalib::ChannelCalib() : TObject(), fACoef(0), fACoefErr(0), fICoef(0),
   fICoefErr(0), fDLCoef(0), fDLCoefErr(0), fT0Coef(0), fT0CoefErr(0),
   fAvrgEMiss(0), fAvrgEMissErr(0), fBananaChi2Ndf(0)
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
   printf("fACoef, fACoefErr, fICoef, fICoefErr, fDLCoef, fDLCoefErr, fT0Coef, \
          fT0CoefErr, fAvrgEMiss, fAvrgEMissErr, fBananaChi2Ndf\n");
   printf("%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n", fACoef, fACoefErr, fICoef,
          fICoefErr, fDLCoef, fDLCoefErr, fT0Coef, fT0CoefErr, fAvrgEMiss,
          fAvrgEMissErr, fBananaChi2Ndf);

}
