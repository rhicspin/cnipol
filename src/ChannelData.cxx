#include "ChannelData.h"

ClassImp(ChannelData)


/** Default constructor. */
ChannelData::ChannelData() : TObject(), fAmpltd(UCHAR_MAX), fIntgrl(UCHAR_MAX),
   fTdc(UCHAR_MAX), fTdcAMax(UCHAR_MAX)
{
}


/** Default destructor. */
ChannelData::~ChannelData()
{
}


/** */
void ChannelData::Print(const Option_t* opt) const
{
   printf("%12d %12d %12d %12d", fAmpltd, fTdc, fIntgrl, fTdcAMax);
}
