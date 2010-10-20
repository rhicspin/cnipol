/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "ChannelData.h"

ClassImp(ChannelData)

using namespace std;

/** Default constructor. */
ChannelData::ChannelData() : TObject(), fAmpltd(255), fTdc(255), fIntgrl(255)
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
