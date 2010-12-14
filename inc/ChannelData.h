/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef ChannelData_h
#define ChannelData_h

#include <map>
#include <vector>

#include "TObject.h"

//#include "EventConfig.h"

class ChannelData;

typedef std::vector<ChannelData> ChannelDataVec;
typedef std::map<UShort_t, ChannelData> ChannelDataMap;
typedef std::pair<const UShort_t, ChannelData> ChannelDataPair;


/**
 *
 */
class ChannelData : public TObject
{
public:

   UChar_t  fAmpltd;
   UChar_t  fIntgrl;
   UChar_t  fTdc;
   UChar_t  fTdcAMax;

public:

   ChannelData();
   ~ChannelData();

   //virtual void Print(const Option_t* opt="") const;
   //float GetTime(EventConfig *ec);
   void Print(const Option_t* opt="") const;

   ClassDef(ChannelData, 1)
};

#endif
