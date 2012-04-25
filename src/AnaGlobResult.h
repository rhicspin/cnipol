
#ifndef AnaGlobResult_h
#define AnaGlobResult_h

#include "TObject.h"

#include "AnaMeasResult.h"
#include "AnaFillResult.h"
#include "MAsymAnaInfo.h"
#include "EventConfig.h"



/** */
class AnaGlobResult : public TObject
{
private:

   std::string       fPathExternResults;
   std::string       fFileNameYelHjet;
   std::string       fFileNameBluHjet;
   UInt_t            fMinFill;
   UInt_t            fMaxFill;

   FILE             *fFilePhp;      //!
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

   void       Configure(MAsymAnaInfo &mainfo);
   void       AdjustMinMaxFill();
   void       AddMeasResult(AnaMeasResult &result);
   void       AddMeasResult(EventConfig &mm);
   void       Process();
   ValErrPair GetPolarBeam(ERingId ringId, UInt_t fillId, Bool_t norm=kTRUE);
   //ValErrPair GetBeamPolarExp(UInt_t fillId, ERingId ringId);
   void       UpdateInsertDb();
   ValErrPair GetNormJetCarbon(EPolarimeterId polId);
   ValErrPair GetNormProfPolar(EPolarimeterId polId);
   void       SetFilePhp(FILE *f=stdout);

protected:

   void ReadHJInfo(std::ifstream &file);

   ClassDef(AnaGlobResult, 1)
};

#endif
