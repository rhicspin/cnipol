/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef MAsymRateHists_h
#define MAsymRateHists_h

#include "TDirectory.h"

#include "AsymGlobals.h"
#include "ChannelEvent.h"
#include "DrawObjContainer.h"
#include "RunConfig.h"


/**
 *
 */
class MAsymRateHists : public DrawObjContainer
{
public:


private:

   std::map<Double_t, UInt_t> fHTargetVsRun;
   std::map<Double_t, UInt_t> fVTargetVsRun;

public:

   MAsymRateHists();
   MAsymRateHists(TDirectory *dir);
   ~MAsymRateHists();

   void BookHists(std::string sid="");
   void BookHistsPolarimeter(EPolarimeterId polId, EBeamEnergy beamE);
   //void BookHistsEnergy(EBeamEnergy beamE);
   void Fill(ChannelEvent *ch, std::string cutid="");
   //void Fill(Int_t n, Double_t* hData);
   void Fill(EventConfig &rc);
   void PostFill();
   void Print(const Option_t* opt="") const;

   ClassDef(MAsymRateHists, 1)
};

#endif
