/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolPmtHists_h
#define CnipolPmtHists_h

#include "TDirectoryFile.h"

#include "AsymHeader.h"

#include "ChannelEvent.h"
#include "DrawObjContainer.h"


/**
 *
 */
class CnipolPmtHists : public DrawObjContainer
{
public:


public:

   CnipolPmtHists();
   CnipolPmtHists(TDirectory *dir);
   ~CnipolPmtHists();

   void  BookHists(std::string cutid="");
   //void  Fill(ChannelEvent *ch, std::string cutid="");
   void  FillPassOne(ChannelEvent *ch);
   void  PostFillPassOne(DrawObjContainer *oc=0);

   ClassDef(CnipolPmtHists, 1)
};

#endif
