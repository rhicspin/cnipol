/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolProfileHists_h
#define CnipolProfileHists_h

#include "TDirectoryFile.h"
#include "TH2F.h"

#include "AsymHeader.h"

#include "ChannelEvent.h"
#include "DrawObjContainer.h"


/**
 *
 */
class CnipolProfileHists : public DrawObjContainer
{
public:


public:

   CnipolProfileHists();
   CnipolProfileHists(TDirectory *dir);
   ~CnipolProfileHists();

   void BookHists(std::string sid="");
   virtual void PostFill();

   ClassDef(CnipolProfileHists, 1)
};

#endif
