/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolAsymHists_h
#define CnipolAsymHists_h

#include <string>

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

   CnipolAsymHists();
   CnipolAsymHists(TDirectory *dir);
   ~CnipolAsymHists();

   void BookHists(std::string cutid="");
   void PreFill(std::string cutid="");
   void Fill(ChannelEvent *ch, std::string cutid="");
   void FillDerived();
   void FillDerived(DrawObjContainer &oc);
   void PostFill();

   ClassDef(CnipolAsymHists, 1)
};

#endif
