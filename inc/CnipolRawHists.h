/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolRawHists_h
#define CnipolRawHists_h

#include "TDirectoryFile.h"

#include "AsymHeader.h"

#include "ChannelEvent.h"
#include "DrawObjContainer.h"


/**
 *
 */
class CnipolRawHists : public DrawObjContainer
{
public:


public:

   CnipolRawHists();
   CnipolRawHists(TDirectory *dir);
   ~CnipolRawHists();

   void  BookHists(std::string cutid="");
   //virtual void Print(const Option_t* opt="") const;
   void  Print(const Option_t* opt="") const;
   void  Fill(ChannelEvent *ch, std::string cutid="");

   ClassDef(CnipolRawHists, 1)
};

#endif
