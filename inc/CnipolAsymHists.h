/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolAsymHists_h
#define CnipolAsymHists_h

#include "TDirectoryFile.h"

#include "AsymHeader.h"

#include "ChannelEvent.h"
#include "DrawObjContainer.h"


/**
 *
 */
class CnipolAsymHists : public DrawObjContainer
{
public:


public:

   CnipolAsymHists();
   CnipolAsymHists(TDirectory *dir);
   ~CnipolAsymHists();

   void  BookHists(std::string cutid="");
   void  Fill(ChannelEvent *ch, std::string cutid="");

   ClassDef(CnipolAsymHists, 1)
};

#endif
