/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "ChannelCalib.h"

ClassImp(ChannelCalib)

using namespace std;

/** Default constructor. */
ChannelCalib::ChannelCalib() : TObject(), fACoef(-1), fACoefErr(-1), fAChi2Ndf(0),
   fICoef(-1), fICoefErr(-1), fIChi2Ndf(0), fDLWidth(0), fDLWidthErr(0), fT0Coef(0),
   fT0CoefErr(0), fAvrgEMiss(0), fAvrgEMissErr(0), fEMeasDLCorr(0), fBananaChi2Ndf(0)
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
   printf("fACoef, fACoefErr, fAChi2Ndf, fICoef, fICoefErr, fIChi2Ndf, fDLWidth, " \
          "fDLWidthErr, fT0Coef, fT0CoefErr, fAvrgEMiss, fAvrgEMissErr, " \
          "fBananaChi2Ndf\n");
   printf("%12.3f  %12.3f  %12.3f  %12.3f  %12.3f  %12.3f  %12.3f  %12.3f  %12.3f  %12.3f  %12.3f  %12.3f  %12.3f\n", fACoef,
          fACoefErr, fAChi2Ndf, fICoef, fICoefErr, fIChi2Ndf, fDLWidth,
          fDLWidthErr, fT0Coef, fT0CoefErr, fAvrgEMiss, fAvrgEMissErr,
          fBananaChi2Ndf);

}


/** */
void ChannelCalib::PrintAsPhp(FILE *f) const
{ //{{{
   fprintf(f, "array(");
   fprintf(f, "'ACoef' => %7.4f, ",         fACoef);
   fprintf(f, "'ACoefErr' => %7.4f, ",      fACoefErr);
   fprintf(f, "'ICoef' => %7.4f, ",         fICoef);
   fprintf(f, "'ICoefErr' => %7.4f, ",      fICoefErr);
   fprintf(f, "'DLWidth' => %7.4f, ",       fDLWidth);
   fprintf(f, "'DLWidthErr' => %7.4f, ",    fDLWidthErr);
   fprintf(f, "'T0Coef' => %7.4f, ",        fT0Coef);
   fprintf(f, "'T0CoefErr' => %7.4f, ",     fT0CoefErr);
   fprintf(f, "'fAvrgEMiss' => %7.4f, ",    fAvrgEMiss);
   fprintf(f, "'fAvrgEMissErr' => %7.4f, ", fAvrgEMissErr);
   fprintf(f, "'fEMeasDLCorr' => %7.4f, ",  fEMeasDLCorr);
   fprintf(f, "'fBananaChi2Ndf' => %7.4f",  fBananaChi2Ndf);
   fprintf(f, ")");
} //}}}
