/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef MAsymSingleFillHists_h
#define MAsymSingleFillHists_h

#include "TDirectory.h"

#include "AsymGlobals.h"
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

   void BookHists(std::string sid="");
   void BookHistsPolarimeter(EPolarimeterId polId);
   void Fill(EventConfig &rc);
   void PostFill();
   void UpdateLimits();

   ClassDef(MAsymSingleFillHists, 1)
};

#endif
