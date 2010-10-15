/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "ChannelData.h"

ClassImp(ChannelData)

using namespace std;

/** Default constructor. */
ChannelData::ChannelData() : TObject(), fAmpltd(-1), fTdc(-1), fIntgrl(-1)
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

   printf("ChannelData:\n");
}
