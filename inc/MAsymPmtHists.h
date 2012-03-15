/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef MAsymPmtHists_h
#define MAsymPmtHists_h

#include "TDirectory.h"

#include "AsymGlobals.h"
#include "ChannelEvent.h"
#include "DrawObjContainer.h"
#include "RunConfig.h"


/**
 *
 */
class MAsymPmtHists : public DrawObjContainer
{
public:


private:

   std::map<Double_t, UInt_t> fHTargetVsRun;
   std::map<Double_t, UInt_t> fVTargetVsRun;

public:

   MAsymPmtHists();
   MAsymPmtHists(TDirectory *dir);
   ~MAsymPmtHists();

   void BookHists(std::string sid="");
   void BookHistsPolarimeter(EPolarimeterId polId, EBeamEnergy beamE);
   void Fill(const EventConfig &rc);
   void PostFill();
   void UpdateLimits();

   ClassDef(MAsymPmtHists, 1)
};

#endif
