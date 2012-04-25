/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef MAsymSingleFillHists_h
#define MAsymSingleFillHists_h

#include "TDirectory.h"

#include "AsymGlobals.h"
#include "AnaFillResult.h"
#include "ChannelEvent.h"
#include "DrawObjContainer.h"
#include "RunConfig.h"


/**
 *
 */
class MAsymSingleFillHists : public DrawObjContainer
{
public:


public:

   MAsymSingleFillHists();
   MAsymSingleFillHists(TDirectory *dir);
   ~MAsymSingleFillHists();

   void BookHists();
   void BookHistsPolarimeter(EPolarimeterId polId);
   void Fill(EventConfig &rc);
   void PostFill();
   void PostFill(AnaFillResult &afr);
   //void UpdateLimits();

   ClassDef(MAsymSingleFillHists, 1)
};

#endif
