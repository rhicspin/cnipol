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

   void  BookHists(std::string sid="");
   //virtual void Print(const Option_t* opt="") const;
   void  Print(const Option_t* opt="") const;
   void  Fill(ChannelEvent *ch, std::string sid="");

   ClassDef(CnipolRunHists, 1)
};

#endif
