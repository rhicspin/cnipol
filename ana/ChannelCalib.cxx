/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "ChannelCalib.h"

ClassImp(CalibCoefSet)
ClassImp(ChannelCalib)

using namespace std;

/** Default constructor. */
ChannelCalib::ChannelCalib() : TObject(),
   // DL calibration related params
   fDLWidth(0), fDLWidthErr(0), fT0Coef(0), fT0CoefErr(0), fAvrgEMiss(0), fAvrgEMissErr(0), fEMeasDLCorr(0), fBananaChi2Ndf(0),
   fFitStatus(kUNKNOWN)
{
}


/** Default destructor. */
ChannelCalib::~ChannelCalib()
{
}


/** */
EFitStatus ChannelCalib::GetFitStatus() const
{
   return (EFitStatus) fFitStatus;
}


/** */
void ChannelCalib::CopyAlphaCoefs(const ChannelCalib &chCalib)
{
   fAmAmp = chCalib.fAmAmp;
   fAmInt = chCalib.fAmInt;
   fGdAmp = chCalib.fGdAmp;
   fGdInt = chCalib.fGdInt;
}


/**
 *
 */
void ChannelCalib::Print(const Option_t* opt) const
{
   opt = "";

   //printf("ChannelCalib:\n");
   //printf("\n");
   //printf("fACoef, fACoefErr, fAChi2Ndf, fICoef, fICoefErr, fIChi2Ndf, fDLWidth, " \
   //     "fDLWidthErr, fT0Coef, fT0CoefErr, fAvrgEMiss, fAvrgEMissErr, " \
   //     "fBananaChi2Ndf " \
   //     "fFitStatus\n");
   printf("%8.3f  %8.3f  %8.3f  %8.3f  %8.3f  %8.3f  %8.3f  %8.3f  " \
          "%8.3f  %8.3f  %8.3f  %8.3f  %8.3f  %8d\n", fAmAmp.fCoef,
          fAmAmp.fCoefErr, fAmAmp.fChi2Ndf, fAmInt.fCoef, fAmInt.fCoefErr, fAmInt.fChi2Ndf, fDLWidth,
          fDLWidthErr, fT0Coef, fT0CoefErr, fAvrgEMiss, fAvrgEMissErr,
          fBananaChi2Ndf, fFitStatus);

}


/** */
void ChannelCalib::PrintAsPhp(FILE *f) const
{
   fprintf(f, "array(");
   fprintf(f, "'Americium' => array(");
   fprintf(f, "'ACoef' => %7.4f, ",         fAmAmp.fCoef);
   fprintf(f, "'ACoefErr' => %7.4f, ",      fAmAmp.fCoefErr);
   fprintf(f, "'ICoef' => %7.4f, ",         fAmInt.fCoef);
   fprintf(f, "'ICoefErr' => %7.4f, ",      fAmInt.fCoefErr);
   fprintf(f, "),");
   fprintf(f, "'Gadolinium' => array(");
   fprintf(f, "'ACoef' => %7.4f, ",         fGdAmp.fCoef);
   fprintf(f, "'ACoefErr' => %7.4f, ",      fGdAmp.fCoefErr);
   fprintf(f, "'ICoef' => %7.4f, ",         fGdInt.fCoef);
   fprintf(f, "'ICoefErr' => %7.4f, ",      fGdInt.fCoefErr);
   fprintf(f, "),");
   fprintf(f, "'DLWidth' => %7.4f, ",       fDLWidth);
   fprintf(f, "'DLWidthErr' => %7.4f, ",    fDLWidthErr);
   fprintf(f, "'T0Coef' => %7.4f, ",        fT0Coef);
   fprintf(f, "'T0CoefErr' => %7.4f, ",     fT0CoefErr);
   fprintf(f, "'fAvrgEMiss' => %7.4f, ",    fAvrgEMiss);
   fprintf(f, "'fAvrgEMissErr' => %7.4f, ", fAvrgEMissErr);
   fprintf(f, "'fEMeasDLCorr' => %7.4f, ",  fEMeasDLCorr);
   fprintf(f, "'fBananaChi2Ndf' => %7.4f, ",  fBananaChi2Ndf);
   fprintf(f, "'fFitStatus' => %7d",        fFitStatus);
   fprintf(f, ")");
}


/** */
void ChannelCalib::ResetToZero()
{
   fAmAmp.fCoef         = 0;
   fAmAmp.fCoefErr      = 0;
   fAmAmp.fChi2Ndf      = 0;
   fAmInt.fCoef         = 0;
   fAmInt.fCoefErr      = 0;
   fAmInt.fChi2Ndf      = 0;
   fGdAmp.fCoef         = 0;
   fGdAmp.fCoefErr      = 0;
   fGdAmp.fChi2Ndf      = 0;
   fGdInt.fCoef         = 0;
   fGdInt.fCoefErr      = 0;
   fGdInt.fChi2Ndf      = 0;
   fDLWidth       = 0;
   fDLWidthErr    = 0;
   fT0Coef        = 0;
   fT0CoefErr     = 0;
   fAvrgEMiss     = 0;
   fAvrgEMissErr  = 0;
   fEMeasDLCorr   = 0;
   fBananaChi2Ndf = 0;
}
