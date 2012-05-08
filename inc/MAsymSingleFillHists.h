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
private:

   UInt_t fFillId;

public:

   MAsymSingleFillHists();
   MAsymSingleFillHists(TDirectory *dir);
   ~MAsymSingleFillHists();

   void BookHists();
   void BookHistsPolarimeter(EPolarimeterId polId);
   void SetSignature(const std::string signature);
   void Fill(EventConfig &rc);
   void PostFill();
   void PostFill(AnaFillResult &afr);
   //void UpdateLimits();

   ClassDef(MAsymSingleFillHists, 1)
};

#endif
