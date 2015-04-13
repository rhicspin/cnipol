
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

   void  BookHists();
   void  PreFill() ;
   void  Fill(ChannelEvent *ch) ;
   void  PostFill() ;

   ClassDef(CnipolPmtHists, 1)
};

#endif
