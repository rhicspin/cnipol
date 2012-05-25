
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
   std::string       fFileNameYelHjet;
   std::string       fFileNameBluHjet;
   UInt_t            fMinFill;
   UInt_t            fMaxFill;
   Bool_t            fDoCalcPolarNorm;

   BeamEnergySet     fBeamEnergies;

   void       CalcPolarNorm();
   void       CalcAvrgPolProfR();
   void       CalcPolarDependants();

public:

   AnaFillResultMap             fAnaFillResults;
   PolId2ValErrMap              fNormJetCarbon;  // Ratio of the averages
   PolId2ValErrMap              fNormJetCarbon2; // Average of the fill ratios
   TargetUId2ValErrMap          fNormJetCarbonByTarget2;
   PolId2ValErrMap              fNormProfPolar;
   PolId2ValErrMap              fNormProfPolar2;
   RingId2TgtOrient2ValErrMap   fAvrgPolProfRs;
   RingId2ValErrMap             fSystUvsDPolar;
   PolId2ValErrMap              fSystJvsCPolar;
   PolId2ValErrMap              fSystProfPolar;

public:

   AnaGlobResult();
   ~AnaGlobResult();

   void Print(const Option_t* opt="") const;
   void PrintAsPhp(FILE *f=stdout) const;

   void           Configure(MAsymAnaInfo &mainfo);
   void           AdjustMinMaxFill();
   UInt_t         GetMinFill() const { return fMinFill; }
   UInt_t         GetMaxFill() const { return fMaxFill; }
   void           AddMeasResult(AnaMeasResult &result);
   void           AddMeasResult(EventConfig &mm, DrawObjContainer *ocIn=0);
   void           AddHJMeasResult();
   void           Process(DrawObjContainer *ocOut=0);
   ValErrPair     GetPolarBeam(ERingId ringId, UInt_t fillId, Bool_t norm=kTRUE);
   AnaFillResult* GetAnaFillResult(UInt_t fillId);
   //ValErrPair     GetBeamPolarExp(UInt_t fillId, ERingId ringId);
   void           UpdateInsertDb(AsymDbSql *asymDbSql);
   ValErrPair     GetNormJetCarbon(EPolarimeterId polId);
   ValErrPair     GetNormProfPolar(EPolarimeterId polId);

   ClassDef(AnaGlobResult, 1)
};

#endif
