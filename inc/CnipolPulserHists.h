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
   //void Fill(ChannelEvent *ch, std::string cutid="");
   void FillPreProcess(ChannelEvent *ch);
   void PostFill();

   ClassDef(CnipolPulserHists, 1)
};

#endif
