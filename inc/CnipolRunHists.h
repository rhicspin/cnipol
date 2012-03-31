/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolRunHists_h
#define CnipolRunHists_h

#include <string>

#include "TDirectoryFile.h"
#include "TH2F.h"

#include "AsymHeader.h"

#include "ChannelEvent.h"
#include "DrawObjContainer.h"


/**
 *
 */
class CnipolRunHists : public DrawObjContainer
{
public:


public:

   CnipolRunHists();
   CnipolRunHists(TDirectory *dir);
   ~CnipolRunHists();

   void  BookHists();
   void  PostFill();
   void  Fill(ChannelEvent *ch);
   void  Fill(MeasInfo &ri);

   ClassDef(CnipolRunHists, 1)
};

#endif
