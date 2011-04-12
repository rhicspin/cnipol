/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolTimeMHists_h
#define CnipolTimeMHists_h

#include "TDirectoryFile.h"

#include "AsymHeader.h"

#include "ChannelEvent.h"
#include "DrawObjContainer.h"


/**
 *
 */
class CnipolTimeMHists : public DrawObjContainer
{
public:


public:

   CnipolTimeMHists();
   CnipolTimeMHists(TDirectory *dir);
   ~CnipolTimeMHists();

   void  BookHists(std::string cutid="");
   //virtual void Print(const Option_t* opt="") const;
   void  Print(const Option_t* opt="") const;
   void  Fill(ChannelEvent *ch, std::string cutid="");

   ClassDef(CnipolTimeMHists, 1)
};

#endif
