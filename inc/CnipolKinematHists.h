/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolKinematHists_h
#define CnipolKinematHists_h

#include "TDirectoryFile.h"

#include "AsymHeader.h"

#include "ChannelEvent.h"
#include "DrawObjContainer.h"


/**
 *
 */
class CnipolKinematHists : public DrawObjContainer
{
public:


public:

   CnipolKinematHists();
   CnipolKinematHists(TDirectory *dir);
   ~CnipolKinematHists();

   void  BookHists(std::string cutid="");
   void  Fill(ChannelEvent *ch, std::string cutid="");
   void  PostFill();

   ClassDef(CnipolKinematHists, 1)
};

#endif
