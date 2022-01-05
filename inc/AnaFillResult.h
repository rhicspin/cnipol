
#ifndef AnaFillResult_h
#define AnaFillResult_h

#include <map>
#include <time.h>
#include <fstream>

#include "TObject.h"

#include "AsymCommon.h"
#include "AnaFillExternResult.h"
#include "AnaMeasResult.h"
#include "EventConfig.h"
#include "DrawObjContainer.h"
#include "TargetUId.h"
#include "MeasInfo.h"


class AnaFillResult;
class AnaGlobResult;

typedef std::map<UInt_t, AnaFillResult>                   AnaFillResultMap;
typedef std::map<UInt_t, AnaFillResult>::iterator         AnaFillResultMapIter;
typedef std::map<UInt_t, AnaFillResult>::const_iterator   AnaFillResultMapConstIter;


/** */
class AnaFillResult : public TObject
{
protected:

   UInt_t                     fFillId;
   AnaGlobResult             *fAnaGlobResult;
   time_t                     fStartTime;
   time_t                     fEndTime;                // the time of the last measurement in the fill
   AnaFillExternResult        fAnaFillExternResult;

   PolId2TGraphMap            fPCPolarGraphs;          // polarization at flattop
   PolId2TGraphMap            fPCPolarInjGraphs;       // polarization measurements at 24 GeV by polarimeter
   PolId2TgtOrient2TGraphMap  fPCProfRGraphs;
   PolId2TgtOrient2TGraphMap  fPCProfRInjGraphs;       //!
   PolId2TGraphMap            fPCPolarPhaseGraphs;          // spin tilt at flattop
   PolId2TGraphMap            fPCPolarPhaseInjGraphs;       // spin tilt at 24 GeV by polarimeter
   PolId2TargetUIdMap         fPCTargets;

   TFitResultPtr              fPCPolarFitRes;
   TFitResultPtr              fPolProfRFitRes;
   TFitResultPtr              fAgsPolFitRes;

   Double_t                   fFlattopEnergy;
   EFillType                  fFillType;

public:

   AnaMeasResultMap              fAnaMeasResults;
   MeasInfoMap                   fMeasInfos;
   PolId2MeasInfoPtrSetMap       fMeasInfosByPolId;

   PolId2ValErrMap               fPCPolars;          // (Nominal) intensity weighted polarization measurement results
   PolId2ValErrMap               fPCPolarUnWs;       // The same as fPCPolars but not weighted with intensity
   PolId2ValErrMap               fPCPolarPhases;          // average spin tilt angle
   PolId2TgtOrient2ValErrMap     fPCProfRs;          //!
   //PolId2TgtOrient2ValErrMap     fPCProfRInjs;       //!
   TargetUId2ValErrMap           fPCPolarsByTargets;
   PolId2ValErrMap               fPCProfPolars;      // Polarization as measured from the sweep measurements P = P_0/sqrt(1 + R)
   RingId2ValErrMap              fHJPolars;
   RingId2ValErrMap              fHJAsyms;

   RingId2ValErrMap              fBeamPolars;        //!
   RingId2ValErrMap              fBeamPolarP0s;      //!
   RingId2ValErrMap              fBeamPolarSlopes;   //!
   RingId2ValErrMap              fBeamCollPolars;    //! not used
   
   PolId2ValErrMap               fSystProfPolar;
   PolId2ValErrMap               fSystJvsCPolar;
   RingId2ValErrMap              fPolarRatioU2D;

   PolId2ValErrMap               fRotatorPCPolarRatio; // before/after rotator ratio at flattop
   PolId2ValErrMap               fRampPCPolarRatio;    //! before/after ramp to flattop ratio
   PolId2ValErrMap               fRhicAgsPolarRatio;

   String2TgtOrientMap           fMeasTgtOrients;      // a stupid temporary fix
   String2TargetIdMap            fMeasTgtIds;          // a stupid temporary fix
   String2RingIdMap              fMeasRingIds;         // a stupid temporary fix

   RingId2TgtOrient2ValErrMap    fPolProfRs;
   RingId2TgtOrient2ValErrMap    fPolProfPMaxs;
   RingId2TgtOrient2ValErrMap    fPolProfPs;

public:

   AnaFillResult() {};
   AnaFillResult(UInt_t fillId);
   ~AnaFillResult();

   time_t GetStartTime() const;
   time_t GetEndTime() const;
   time_t GetLumiOnRelTime() const;
   time_t GetLumiOffRelTime() const;
   time_t GetLumiOnTime() const;
   time_t GetLumiOffTime() const;

   void Print(const Option_t* opt="") const;
   //void PrintAsPhp(FILE *f=stdout) const;

   AnaGlobResult*       GetAnaGlobResult() const { return fAnaGlobResult; }
   void                 SetAnaGlobResult(AnaGlobResult* agr) { fAnaGlobResult = agr; }
   UInt_t               GetFillId() const { return fFillId; }
   Double_t             GetFlattopEnergy() const { return fFlattopEnergy; }
   EFillType            GetFillType() const { return fFillType; }
   TGraphErrors*        GetGrBluIntens() const;
   TGraphErrors*        GetGrYelIntens() const;
   TGraphErrors*        GetIntensGraph(ERingId ringId) const;
   ValErrPair           GetIntensDecay(ERingId ringId) const;
   TGraphErrors*        GetRotCurStarGraph(ERingId ringId) const;
   TGraphErrors*        GetRotCurPhenixGraph(ERingId ringId) const;
   TGraphErrors*        GetSnakeCurGraph(ERingId ringId) const;
   std::vector<TGraphErrors*> GetBCCurGraphs(EPolarimeterId polId) const;
   TGraphErrors*        GetAgsPolFitGraph() const;
   void                 AddMeasResult(AnaMeasResult &result);
   void                 AddMeasResult(EventConfig &mm);
   void                 AddGraphMeasResult(EventConfig &mm, DrawObjContainer &ocIn);
   void                 Process(DrawObjContainer *ocOut=0);
   Bool_t               IsValidFlattopMeas(const MeasInfo &measInfo);
   ValErrPair           GetPCPolar(EPolarimeterId polId, PolId2ValErrMap *normJC=0) const;
   ValErrPair           GetPCPolarUnW(EPolarimeterId polId, PolId2ValErrMap *normJC=0) const;
   ValErrPair           GetPCPolarPhase(EPolarimeterId polId) const;
   TGraphErrors*        GetPCPolarGraph(EPolarimeterId polId) const;
   TGraphErrors*        GetPCPolarInjGraph(EPolarimeterId polId);
   ValErrPair           GetPCPolarP0(EPolarimeterId polId) const;
   ValErrPair           GetPCPolarSlope(EPolarimeterId polId) const;
   TargetUId            GetPCTarget(EPolarimeterId polId) const { return fPCTargets.find(polId)->second; }
   //ValErrPair           GetPCPolarPmax(EPolarimeterId polId, ETargetOrient tgtOrient);
   ValErrPair           GetHJPolar(EPolarimeterId polId);
   ValErrPair           GetHJPolar(ERingId ringId) const;
   ValErrPair           GetHJAsym(EPolarimeterId polId);
   ValErrPair           GetHJAsym(ERingId ringId);
   ValErrPair           GetBeamPolar(ERingId ringId) const;
   ValErrPair           GetBeamPolarP0(ERingId ringId) const;
   ValErrPair           GetBeamPolarDecay(ERingId ringId) const;
   ValErrPair           GetPolarUDRatio(ERingId ringId);
   ValErrPair           GetSystJvsCPolar(EPolarimeterId polId);
   ValErrPair           GetSystProfPolar(EPolarimeterId polId);
   TGraphErrors*        GetPCProfRGraph(EPolarimeterId polId, ETargetOrient tgtOrient) const;
   TGraphErrors*        GetPCProfRInjGraph(EPolarimeterId polId, ETargetOrient tgtOrient) const;
   ValErrPair           GetPCProfR(ERingId ringId, ETargetOrient tgtOrient) const; // deprecated
   ValErrPair           GetPCProfR(EPolarimeterId polId, ETargetOrient tgtOrient) const;
   ValErrPair           GetPCProfRInj(EPolarimeterId polId, ETargetOrient tgtOrient) const;
   ValErrPair           GetPCProfR0(EPolarimeterId polId, ETargetOrient tgtOrient) const;
   ValErrPair           GetPCProfRSlope(EPolarimeterId polId, ETargetOrient tgtOrient) const;
   ValErrPair           GetPCProfPMax(ERingId ringId, ETargetOrient tgtOrient) const;
   ValErrPair           GetPCProfP(ERingId ringId, ETargetOrient tgtOrient) const;
   //ValErrPair           GetProfPolarSyst(EPolarimeterId polId);
   ValErrPair           GetAgsCniPolar() const;
   void                 CalcBeamPolar(Bool_t doNorm=kFALSE);
   RingId2ValErrMap     CalcPolarRatioU2D(PolId2ValErrMap &normJC);
   PolId2ValErrMap      CalcSystJvsCPolar(PolId2ValErrMap &normJC);
   PolId2ValErrMap      CalcSystProfPolar(PolId2ValErrMap &normPP);
   ValErrPair           CalcAvrgPolar(EPolarimeterId polId);
   ValErrPair           CalcAvrgPolarUnweighted(EPolarimeterId polId);
   ValErrPair           CalcAvrgPolarPhase(EPolarimeterId polId);
   ValErrPair           CalcAvrgPolProfPolar(EPolarimeterId polId);
   //ValErrPair           CalcAvrgProfRInj(EPolarimeterId polid, ETargetOrient tgtOrient);
   ValErrPair           CalcAvrgProfR(EPolarimeterId polid, ETargetOrient tgtOrient);
   ValErrPair           CalcAvrgPolProfR(ERingId ringId, ETargetOrient tgtOrient);
   ValErrPair           CalcAvrgPolProfPMax(ERingId ringId, ETargetOrient tgtOrient);
   ValErrPair           CalcPolProfP(ValErrPair R, ValErrPair PMax);
   void                 CalcRotatorPCPolarRatio();
   void                 CalcRampPCPolarRatio();
   void                 CalcRhicAgsPolarRatio();
   void                 CalcAvrgAsymByBunch(const AnaMeasResult &amr, const MeasInfo &mi, DrawObjContainer &ocOut) const;
   void                 UpdateExternGraphRange();
   void                 FitPCPolarGraphs();
   void                 FitPCProfRGraphs();
   void                 FitAgsPol();
   void                 SetHJPolar(ERingId ringId, ValErrPair ve);
   void                 SetHJAsym(ERingId ringId, ValErrPair ve);
   void                 AppendToPCPolarGraph(EPolarimeterId polId, Double_t x, Double_t y, Double_t xe, Double_t ye);
   void                 AppendToPCPolarInjGraph(EPolarimeterId polId, Double_t x, Double_t y, Double_t xe, Double_t ye);
   void                 AppendToPCProfRGraph(EPolarimeterId polId, ETargetOrient tgtOrient, Double_t x, Double_t y, Double_t xe, Double_t ye);
   void                 AppendToPCProfRInjGraph(EPolarimeterId polId, ETargetOrient tgtOrient, Double_t x, Double_t y, Double_t xe, Double_t ye);
   //
   TGraphErrors*        GetPCPolarPhaseGraph(EPolarimeterId polId) const;
   TGraphErrors*        GetPCPolarPhaseInjGraph(EPolarimeterId polId);
   void                 AppendToPCPolarPhaseGraph(EPolarimeterId polId, Double_t x, Double_t y, Double_t xe, Double_t ye);
   void                 AppendToPCPolarPhaseInjGraph(EPolarimeterId polId, Double_t x, Double_t y, Double_t xe, Double_t ye);

   ClassDef(AnaFillResult, 1)
};

#endif
