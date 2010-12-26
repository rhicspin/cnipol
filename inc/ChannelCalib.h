/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef ChannelCalib_h
#define ChannelCalib_h

#include <map>
#include <vector>

#include "TObject.h"

class ChannelCalib;

typedef std::vector<ChannelCalib> ChannelCalibVec;
typedef std::map<UShort_t, ChannelCalib> ChannelCalibMap;
typedef std::pair<const UShort_t, ChannelCalib> ChannelCalibPair;


/** */
class ChannelCalib : public TObject
{
public:

   Float_t  fACoef;
   Float_t  fACoefErr;
   Float_t  fAChi2Ndf;
   Float_t  fICoef;
   Float_t  fICoefErr;
   Float_t  fIChi2Ndf;
   Float_t  fDLCoef;    // Dead layer
   Float_t  fDLCoefErr;
   Float_t  fT0Coef;    // T0
   Float_t  fT0CoefErr;
   Float_t  fAvrgEMiss;
   Float_t  fAvrgEMissErr;
   Float_t  fBananaChi2Ndf;

public:

   ChannelCalib();
   ~ChannelCalib();

   void Print(const Option_t* opt="") const;

   ClassDef(ChannelCalib, 1)
};

#endif
