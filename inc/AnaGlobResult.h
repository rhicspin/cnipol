
#ifndef AnaGlobResult_h
#define AnaGlobResult_h

#include "TObject.h"

#include "AnaMeasResult.h"
#include "AnaFillResult.h"
#include "MAsymAnaInfo.h"
#include "EventConfig.h"
#include "DrawObjContainer.h"

class AsymDbSql;


/** */
class AnaGlobResult : public TObject
{
protected:

   std::string       fPathExternResults;
   UInt_t            fMinFill;
   UInt_t            fMaxFill;
   time_t            fMinTime;
   time_t            fMaxTime;
   Bool_t            fDoCalcPolarNorm;

   BeamEnergySet     fBeamEnergies;

   void       CalcPolarNorm();
   void       CalcAvrgPolProfR();
   void       CalcDependencies();

public:

   AnaFillResultMap           fAnaFillResults;
   PolId2ValErrMap            fNormJetCarbon;  // Ratio of the averages
   PolId2ValErrMap            fNormJetCarbon2; // Average of the fill ratios
   TargetUId2ValErrMap        fNormJetCarbonByTarget2;
   PolId2ValErrMap            fNormProfPolar;
   PolId2ValErrMap            fNormProfPolar2;
   PolId2TgtOrient2ValErrMap  fAvrgPCProfRUnWs;
   RingId2ValErrMap           fSystUvsDPolar;
   PolId2ValErrMap            fSystJvsCPolar;
   PolId2ValErrMap            fSystProfPolar;

public:

   AnaGlobResult();
   ~AnaGlobResult();

   void Print(const Option_t* opt="") const;
   void PrintAsPhp(FILE *f=stdout) const;

   void                       Configure(MAsymAnaInfo &mainfo);
	void                       UpdMinMaxFill(UInt_t fillId);
	void                       UpdMinMaxTime(time_t time);
   void                       AdjustMinMaxFill();
   Int_t                       GetTargetStatus(Double_t runId, ETargetOrient tgetOrient, UShort_t targetId);
   UInt_t                     GetMinFill() const { return fMinFill; }
   UInt_t                     GetMaxFill() const { return fMaxFill; }
   time_t                     GetMinTime() const { return fMinTime; }
   time_t                     GetMaxTime() const { return fMaxTime; }
   BeamEnergySet              GetBeamEnergies() const { return fBeamEnergies; }
   void                       AddMeasResult(AnaMeasResult &result);
   void                       AddMeasResult(EventConfig &mm, DrawObjContainer *ocIn=0);
   void                       AddHJMeasResult();
   void                       Process(DrawObjContainer *ocOut=0);
   ValErrPair                 GetBeamPolar(ERingId ringId, UInt_t fillId, Bool_t norm=kTRUE);
   PolId2TgtOrient2ValErrMap  GetAvrgPCProfRUnWs() const { return fAvrgPCProfRUnWs; }
   ValErrPair                 GetAvrgPCProfRUnW(EPolarimeterId polId, ETargetOrient tgtOrient) const { return fAvrgPCProfRUnWs.find(polId)->second.find(tgtOrient)->second; }
   AnaFillResult*             GetAnaFillResult(UInt_t fillId);
   //ValErrPair                 GetBeamPolarExp(UInt_t fillId, ERingId ringId);
   void                       UpdateInsertDb(AsymDbSql *asymDbSql);
   ValErrPair                 GetNormJetCarbon(EPolarimeterId polId);
   ValErrPair                 GetNormProfPolar(EPolarimeterId polId);

   ClassDef(AnaGlobResult, 1)
};

#endif
