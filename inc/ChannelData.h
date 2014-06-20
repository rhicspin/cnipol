#ifndef ChannelData_h
#define ChannelData_h

#include <map>
#include <vector>
#include <limits.h>

#include "TObject.h"

class ChannelData;

typedef std::vector<ChannelData>               ChannelDataVec;
typedef std::map<UShort_t, ChannelData>        ChannelDataMap;
typedef std::pair<const UShort_t, ChannelData> ChannelDataPair;


/**
 * A simple data container to hold the principle observables such as signal
 * amplitude, integral, time of arrival, etc. measured by a single detector
 * channel. This class inherits from the ROOT's TObject so, the objects of this
 * class can be naturally saved in a ROOT file.
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
   virtual ~ChannelData() {};

   virtual void Print(const Option_t* opt="") const;

   ClassDef(ChannelData, 1)
};

#endif
