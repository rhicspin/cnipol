/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolAlphaHists_h
#define CnipolAlphaHists_h

#include "TClass.h"
#include "TDirectoryFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TSystem.h"

#include "AsymHeader.h"

#include "DrawObjContainer.h"
#include "ChannelEvent.h"


/** */
class CnipolAlphaHists : public DrawObjContainer
{
public:


public:

   CnipolAlphaHists();
   CnipolAlphaHists(TDirectory *dir);
   virtual ~CnipolAlphaHists();

   void BookHists();
   void Fill(ChannelEvent *ch);
   void FillPreProcess(ChannelEvent *ch);
   void PostFill();

   ClassDef(CnipolAlphaHists, 1)
};

#endif
