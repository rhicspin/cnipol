
#ifndef AnaFillResult_h
#define AnaFillResult_h

#include <map>

#include "TObject.h"

#include "Asym.h"
#include "AnaMeasResult.h"
#include "EventConfig.h"
#include "TargetUId.h"


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

   AnaMeasResultMap             fAnaMeasResults;
   PolId2ValErrMap              fPolars;
   TargetUId2ValErrMap          fPolarsByTargets;
   PolId2ValErrMap              fProfPolars;     // Polarization as measured from the sweep measurements P = P_0/sqrt(1 + R)
   RingId2ValErrMap             fHjetPolars;

   RingId2ValErrMap             fBeamPolars;     //! not used
   RingId2ValErrMap             fBeamCollPolars; //! not used
   
   PolId2ValErrMap              fSystProfPolar;
   PolId2ValErrMap              fSystJvsCPolar;
   RingId2ValErrMap             fSystUvsDPolar;

   String2TgtOrientMap          fMeasTgtOrients; // a stupid temporary fix
   String2TargetIdMap           fMeasTgtIds;  // a stupid temporary fix
   String2RingIdMap             fMeasRingIds;    // a stupid temporary fix
   RingId2TgtOrient2ValErrMap   fPolProfRs;
   RingId2TgtOrient2ValErrMap   fPolProfPMaxs;
   RingId2TgtOrient2ValErrMap   fPolProfPs;

public:

   AnaFillResult();
   ~AnaFillResult();

   void Print(const Option_t* opt="") const;
   //void PrintAsPhp(FILE *f=stdout) const;

   void              AddRunResult(AnaMeasResult &result);
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
