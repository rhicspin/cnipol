/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef MAsymFillHists_h
#define MAsymFillHists_h

#include "TDirectory.h"

#include "AsymGlobals.h"
#include "ChannelEvent.h"
#include "DrawObjContainer.h"
#include "RunConfig.h"


/**
 *
 */
class MAsymFillHists : public DrawObjContainer
{
public:


private:

   std::map<Double_t, UInt_t> fHTargetVsRun;
   std::map<Double_t, UInt_t> fVTargetVsRun;

public:

   MAsymFillHists();
   MAsymFillHists(TDirectory *dir);
   ~MAsymFillHists();

   void BookHists(std::string sid="");
   void BookHistsPolarimeter(EPolarimeterId polId);
   //void BookHistsEnergy(EBeamEnergy beamE);
   void Fill(EventConfig &rc);
   void PostFill();
   void UpdateLimits();

   ClassDef(MAsymFillHists, 1)
};

#endif
