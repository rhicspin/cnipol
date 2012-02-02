/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolTargetHists_h
#define CnipolTargetHists_h

#include "TDirectoryFile.h"
#include "TH2F.h"
#include "TGraphErrors.h"

#include "AsymHeader.h"

#include "ChannelEvent.h"
#include "DrawObjContainer.h"


/**
 *
 */
class CnipolTargetHists : public DrawObjContainer
{
public:


public:

   CnipolTargetHists();
   CnipolTargetHists(TDirectory *dir);
   ~CnipolTargetHists();

   void BookHists(std::string sid="");
   void Fill(ChannelEvent *ch, std::string cutid="");
   void Fill(Int_t n, Double_t* hData);
   void PostFill();

   ClassDef(CnipolTargetHists, 1)
};

#endif
