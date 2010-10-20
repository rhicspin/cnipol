/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef ChannelData_h
#define ChannelData_h

#include <map>
#include <vector>

#include "TObject.h"

class ChannelData;

typedef std::vector<ChannelData> ChannelDataVec;
typedef std::map<UShort_t, ChannelData> ChannelDataMap;

/**
 *
 */
class ChannelData : public TObject
{
public:

   UChar_t  fAmpltd;
   UChar_t  fTdc;
   UChar_t  fIntgrl;

public:

   ChannelData();
   ~ChannelData();

   virtual void Print(const Option_t* opt="") const;

   ClassDef(ChannelData, 1)
};

#endif
