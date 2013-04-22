#ifndef CalibCoefSet_h
#define CalibCoefSet_h

class CalibCoefSet : public TObject
{
public:

   Float_t fPeakPos;
   Float_t fPeakPosErr;
   Float_t fCoef;
   Float_t fCoefErr;
   Float_t fChi2Ndf;

public:

   CalibCoefSet() : fCoef(-1), fCoefErr(-1), fChi2Ndf(0) { };

   ClassDef(CalibCoefSet, 1)
};

#endif
