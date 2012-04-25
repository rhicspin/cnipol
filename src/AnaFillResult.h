
#ifndef AnaFillResult_h
#define AnaFillResult_h

#include <map>
#include <time.h>
#include <fstream>

#include "TObject.h"

#include "Asym.h"
#include "AnaFillExternResult.h"
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
protected:

   UInt_t                       fFillId;
   AnaGlobResult               *fAnaGlobResult;
   time_t                       fStartTime;
   AnaFillExternResult         *fAnaFillExternResult;

   PolId2TGraphMap              fPCPolarGraphs;
   TFitResultPtr                fPCPolarFitRes;

   PolId2TGraphMap              fPolProfRGraphs;
   TFitResultPtr                fPolProfRFitRes;

public:

   AnaMeasResultMap             fAnaMeasResults;
   MeasInfoMap                  fMeasInfos;

   PolId2ValErrMap              fPCPolars;       // Nominal polarization measurement results
   TargetUId2ValErrMap          fPCPolarsByTargets;
   PolId2ValErrMap              fPCProfPolars;     // Polarization as measured from the sweep measurements P = P_0/sqrt(1 + R)
   RingId2ValErrMap             fHJPolars;

   RingId2ValErrMap             fBeamPolars;     //! not used
   RingId2ValErrMap             fBeamCollPolars; //! not used
   
   PolId2ValErrMap              fSystProfPolar;
   PolId2ValErrMap              fSystJvsCPolar;
   RingId2ValErrMap             fSystUvsDPolar;

   //String2TgtOrientMap          fMeasTgtOrients;   // a stupid temporary fix
   String2TgtOrientMap          fMeasTgtOrients;   // a stupid temporary fix
   String2TargetIdMap           fMeasTgtIds;       // a stupid temporary fix
   String2RingIdMap             fMeasRingIds;      // a stupid temporary fix

   RingId2TgtOrient2ValErrMap   fPolProfRs;
   RingId2TgtOrient2ValErrMap   fPolProfPMaxs;
   RingId2TgtOrient2ValErrMap   fPolProfPs;

public:

   AnaFillResult(UInt_t fillId = 0);
   ~AnaFillResult();

   time_t GetStartTime();

   void Print(const Option_t* opt="") const;
   //void PrintAsPhp(FILE *f=stdout) const;

   TGraphErrors* GetGrBluIntens() const;
   TGraphErrors* GetGrYelIntens() const;
   TGraphErrors* GetIntensGraph(ERingId ringId) const;
   AnaFillExternResult* GetAnaFillExternResult() const;

   void              AddMeasResult(AnaMeasResult &result);
   void              AddMeasResult(EventConfig &mm, AnaGlobResult *globRes=0);
   void              AddExternInfo(std::ifstream &file);
   void              Process();
   TGraphErrors*     GetPCPolarGraph(EPolarimeterId polId);
   ValErrPair        GetPCPolarDecay(EPolarimeterId polId);
   ValErrPair        GetIntensDecay(ERingId ringId);
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
   void              FitGraphs();
   void              AddHjetPolar(ERingId ringId, ValErrPair ve);

   ClassDef(AnaFillResult, 1)
};

#endif
