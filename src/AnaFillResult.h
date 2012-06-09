
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

   UInt_t               fFillId;
   AnaGlobResult       *fAnaGlobResult;
   time_t               fStartTime;
   AnaFillExternResult *fAnaFillExternResult;

   PolId2TGraphMap            fPCPolarGraphs;
   PolId2TGraphMap            fPCPolarInjGraphs;
   PolId2TgtOrient2TGraphMap  fPCPolarRGraphs;

   TFitResultPtr        fPCPolarFitRes;
   TFitResultPtr        fPolProfRFitRes;

   //TH1F                *fAsymVsBunchId_X;
   Double_t             fFlattopEnergy;
   EFillType            fFillType;

public:

   AnaMeasResultMap             fAnaMeasResults;
   MeasInfoMap                  fMeasInfos;

   PolId2ValErrMap              fPCPolars;          // (Nominal) intensity weighted polarization measurement results
   PolId2ValErrMap              fPCPolarUnWs;       // 
   TargetUId2ValErrMap          fPCPolarsByTargets;
   PolId2ValErrMap              fPCProfPolars;     // Polarization as measured from the sweep measurements P = P_0/sqrt(1 + R)
   RingId2ValErrMap             fHJPolars;
   RingId2ValErrMap             fHJAsyms;

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
   time_t GetLumiOnTime();
   time_t GetLumiOffTime();

   void Print(const Option_t* opt="") const;
   //void PrintAsPhp(FILE *f=stdout) const;

   Double_t      GetFlattopEnergy() const { return fFlattopEnergy; }
   EFillType     GetFillType() const { return fFillType; }
   TGraphErrors* GetGrBluIntens() const;
   TGraphErrors* GetGrYelIntens() const;
   TGraphErrors* GetIntensGraph(ERingId ringId) const;
   TGraphErrors* GetRotCurStarGraph(ERingId ringId) const;
   TGraphErrors* GetRotCurPhenixGraph(ERingId ringId) const;
   TGraphErrors* GetSnakeCurGraph(ERingId ringId) const;
   AnaFillExternResult* GetAnaFillExternResult() const;

   void              AddMeasResult(AnaMeasResult &result);
   void              AddMeasResult(EventConfig &mm, AnaGlobResult *globRes=0);
   void              AddGraphMeasResult(EventConfig &mm, DrawObjContainer &ocIn);
   void              AddExternInfo(std::ifstream &file);
   void              Process(DrawObjContainer *ocOut=0);
   Bool_t            IsValidFlattopMeas(const MeasInfo &measInfo);
   TGraphErrors*     GetPCPolarGraph(EPolarimeterId polId);
   TGraphErrors*     GetPCPolarInjGraph(EPolarimeterId polId);
   TGraphErrors*     GetPCPolarRGraph(EPolarimeterId polId, ETargetOrient tgtOrient);
   ValErrPair        GetPCPolarDecay(EPolarimeterId polId);
   ValErrPair        GetIntensDecay(ERingId ringId);
   ValErrPair        GetPCPolarRSlope(EPolarimeterId polId, ETargetOrient tgtOrient);
   ValErrPair        GetHJPolar(EPolarimeterId polId);
   ValErrPair        GetHJPolar(ERingId ringId);
   ValErrPair        GetHJAsym(EPolarimeterId polId);
   ValErrPair        GetHJAsym(ERingId ringId);
   ValErrPair        GetPCPolar(EPolarimeterId polId, PolId2ValErrMap *normJC=0);
   ValErrPair        GetPCPolarUnW(EPolarimeterId polId, PolId2ValErrMap *normJC=0);
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
   ValErrPair        CalcAvrgPolarUnweighted(EPolarimeterId polId);
   ValErrPair        CalcAvrgPolProfPolar(EPolarimeterId polId);
   ValErrPair        CalcAvrgPolProfR(ERingId ringId, ETargetOrient tgtOrient);
   ValErrPair        CalcAvrgPolProfPMax(ERingId ringId, ETargetOrient tgtOrient);
   ValErrPair        CalcPolProfP(ValErrPair R, ValErrPair Pmax);
   void              CalcAvrgAsymByBunch(const AnaMeasResult &amr, const MeasInfo &mi, DrawObjContainer &ocOut) const;
   void              UpdateExternGraphRange();
   void              FitExternGraphs();
   void              FitPolarGraphs();
   void              SetHJPolar(ERingId ringId, ValErrPair ve);
   void              SetHJAsym(ERingId ringId, ValErrPair ve);
   void              AppendToPCPolarGraph(EPolarimeterId polId, Double_t x, Double_t y, Double_t xe, Double_t ye);
   void              AppendToPCPolarInjGraph(EPolarimeterId polId, Double_t x, Double_t y, Double_t xe, Double_t ye);
   void              AppendToPCPolarRGraph(EPolarimeterId polId, ETargetOrient tgtOrient, Double_t x, Double_t y, Double_t xe, Double_t ye);

   ClassDef(AnaFillResult, 1)
};

#endif
