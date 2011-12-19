
#ifndef AnaFillResult_h
#define AnaFillResult_h

#include <map>

#include "TObject.h"

#include "Asym.h"
#include "AnaResult.h"
#include "EventConfig.h"


class AnaFillResult;
class AnaGlobResult;

typedef std::map<UInt_t, AnaFillResult>                   AnaFillResultMap;
typedef std::map<UInt_t, AnaFillResult>::iterator         AnaFillResultMapIter;
typedef std::map<UInt_t, AnaFillResult>::const_iterator   AnaFillResultMapConstIter;


/** */
class AnaFillResult : public TObject
{
private:

   AnaGlobResult               *fAnaGlobResult;

public:

   AnaRunResultMap              fAnaRunResults;
   PolId2ValErrMap              fPolars;
   PolId2ValErrMap              fProfPolars;
   RingId2ValErrMap             fHjetPolars;

   RingId2ValErrMap             fBeamPolars; //! not used
   RingId2ValErrMap             fBeamCollPolars; //! not used
   
   PolId2ValErrMap              fSystProfPolar;
   PolId2ValErrMap              fSystJvsCPolar;
   RingId2ValErrMap             fSystUvsDPolar;

   String2TgtOrientMap          fRunTgtOrients; // a stupid temporary fix
   String2RingIdMap             fRunRingIds;    // a stupid temporary fix
   RingId2TgtOrient2ValErrMap   fPolProfRs;
   RingId2TgtOrient2ValErrMap   fPolProfPMaxs;
   RingId2TgtOrient2ValErrMap   fPolProfPs;

public:

   AnaFillResult();
   ~AnaFillResult();

   void Print(const Option_t* opt="") const;
   //void PrintAsPhp(FILE *f=stdout) const;

   void              AddRunResult(AnaResult &result);
   void              AddRunResult(EventConfig &rc, AnaGlobResult *globRes=0);
   void              Process();
   ValErrPair        GetPolarHJ(EPolarimeterId polId);
   ValErrPair        GetPolarHJ(ERingId ringId);
   ValErrPair        GetPolarPC(EPolarimeterId polId, PolId2ValErrMap *normJC=0);
   //ValErrPair        GetPolarBeam(EBeamId beamId);
   ValErrPair        GetPolarBeam(ERingId ringId);
   ValErrPair        GetSystUvsDPolar(ERingId ringId);
   ValErrPair        GetSystJvsCPolar(EPolarimeterId polId);
   ValErrPair        GetSystProfPolar(EPolarimeterId polId);
   ValErrPair        GetPolProfR(ERingId ringId, ETargetOrient tgtOrient);
   ValErrPair        GetPolProfPMax(ERingId ringId, ETargetOrient tgtOrient);
   ValErrPair        GetPolProfP(ERingId ringId, ETargetOrient tgtOrient);
   //ValErrPair   GetProfPolarSyst(EPolarimeterId polId);
   void              CalcBeamPolar(PolId2ValErrMap &normJC);
   RingId2ValErrMap  CalcSystUvsDPolar(PolId2ValErrMap &normJC);
   PolId2ValErrMap   CalcSystJvsCPolar(PolId2ValErrMap &normJC);
   PolId2ValErrMap   CalcSystProfPolar(PolId2ValErrMap &normPP);
   ValErrPair        CalcAvrgPolar(EPolarimeterId polId);
   ValErrPair        CalcAvrgPolProfPolar(EPolarimeterId polId);
   ValErrPair        CalcAvrgPolProfR(ERingId ringId, ETargetOrient tgtOrient);
   ValErrPair        CalcAvrgPolProfPMax(ERingId ringId, ETargetOrient tgtOrient);
   ValErrPair        CalcPolProfP(ValErrPair R, ValErrPair Pmax);
   void              AddHjetPolar(ERingId ringId, ValErrPair ve);

   ClassDef(AnaFillResult, 1)
};

#endif
