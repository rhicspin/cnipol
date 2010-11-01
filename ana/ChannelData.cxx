/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "ChannelData.h"

ClassImp(ChannelData)

using namespace std;

/** Default constructor. */
ChannelData::ChannelData() : TObject(), fAmpltd(UCHAR_MAX), fTdc(UCHAR_MAX), fIntgrl(UCHAR_MAX)
{
};


/** Default destructor. */
ChannelData::~ChannelData()
{
};


/**
 *
 */
void ChannelData::Print(const Option_t* opt) const
{
   opt = "";

   //printf("ChannelData:\n");
   printf("Ampltd, tdc, intgrl: %12d, %12d, %12d", fAmpltd, fTdc, fIntgrl);
}


float ChannelData::GetTime(EventConfig *ec)
{
   return (ec->fConfigInfo->data.WFDTUnit/2.) * (fTdc + ec->fRandom->Rndm() - 0.5);
}
