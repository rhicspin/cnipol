
#ifndef MAsymFillHists_h
#define MAsymFillHists_h

#include "TDirectory.h"

#include "AsymGlobals.h"
#include "AnaGlobResult.h"
#include "ChannelEvent.h"
#include "DrawObjContainer.h"
#include "RunConfig.h"


/**
 *
 */
class MAsymFillHists : public DrawObjContainer
{
private:

   TH1          *hAgsPolarVsFillId;
   TGraph       *grAgsPolarVsFillId;

   std::map<Double_t, UInt_t> fHTargetVsRun;
   std::map<Double_t, UInt_t> fVTargetVsRun;

   std::map<EPolarimeterId, TH1*>    hRotatorPCPolarRatiosByPol;
   std::map<EPolarimeterId, TH1*>    hRampPCPolarRatiosByPol;
   std::map<EPolarimeterId, TGraph*> grRotatorPCPolarRatiosByPol;
   std::map<EPolarimeterId, TGraph*> grRampPCPolarRatiosByPol;
   std::map<ERingId, TH1*>           hRotatorPCPolarRatiosByRing;
   std::map<ERingId, TH1*>           hRampPCPolarRatiosByRing;
   std::map<ERingId, TH1*>           hRhicAgsPolarRatiosByRing;
   std::map<ERingId, TGraph*>        grRotatorPCPolarRatiosByRing;
   std::map<ERingId, TGraph*>        grRampPCPolarRatiosByRing;
   std::map<ERingId, TGraph*>        grRhicAgsPolarRatiosByRing;

public:

   MAsymFillHists();
   MAsymFillHists(TDirectory *dir);
   ~MAsymFillHists();

   void BookHists();
   void BookHistsByPolarimeter(EPolarimeterId polId);
   void BookHistsByRing(ERingId ringId);
   void Fill(const EventConfig &rc);
   void PostFill(AnaGlobResult &agr);
   void PostFill();
   void UpdateLimits();
	DrawObjContainer *GetSingleFillHists(const AnaFillResult &afr);

   ClassDef(MAsymFillHists, 1)
};

#endif
