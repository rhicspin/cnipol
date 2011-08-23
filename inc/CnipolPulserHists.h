/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolPulserHists_h
#define CnipolPulserHists_h

#include "TDirectoryFile.h"

#include "AsymHeader.h"

#include "ChannelEvent.h"
#include "DrawObjContainer.h"


/**
 *
 */
class CnipolPulserHists : public DrawObjContainer
{
public:


public:

   CnipolPulserHists();
   CnipolPulserHists(TDirectory *dir);
   ~CnipolPulserHists();

   void BookHists(std::string cutid="");
   void FillPassOne(ChannelEvent *ch);
   void FillDerivedPassOne();
   void Fill(ChannelEvent *ch, std::string cutid="");

   ClassDef(CnipolPulserHists, 1)
};

#endif
