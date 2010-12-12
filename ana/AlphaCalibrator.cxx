/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "AlphaCalibrator.h"

ClassImp(AlphaCalibrator)

using namespace std;

/** Default constructor. */
AlphaCalibrator::AlphaCalibrator() : Calibrator()
{
}


/** Default destructor. */
AlphaCalibrator::~AlphaCalibrator()
{
}


/**
 *
 */
void AlphaCalibrator::Print(const Option_t* opt) const
{
   opt = "";

   printf("AlphaCalibrator:\n");
}
