/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


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

   std::map<Double_t, UInt_t> fHTargetVsRun;
   std::map<Double_t, UInt_t> fVTargetVsRun;

   std::map<EPolarimeterId, TH1*>    hRotatorPCPolarRatiosByPol;
   std::map<EPolarimeterId, TGraph*> grRotatorPCPolarRatiosByPol;
   std::map<ERingId, TH1*>           hRotatorPCPolarRatiosByRing;
   std::map<ERingId, TGraph*>        grRotatorPCPolarRatiosByRing;

public:

   MAsymFillHists();
   MAsymFillHists(TDirectory *dir);
   ~MAsymFillHists();

   void BookHists();
   void BookHistsByPolarimeter(EPolarimeterId polId);
   void BookHistsByRing(ERingId ringId);
   void Fill(const EventConfig &rc);
   void PostFill();
   void PostFill(AnaGlobResult &agr);
   void UpdateLimits();
	DrawObjContainer *GetSingleFillHists(const AnaFillResult &afr);

   ClassDef(MAsymFillHists, 1)
};

#endif
