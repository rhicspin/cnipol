/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolHists_h
#define CnipolHists_h

#include "TDirectoryFile.h"
#include "TH2F.h"

#include "AsymHeader.h"

#include "ChannelEvent.h"
#include "DrawObjContainer.h"


/**
 *
 */
class CnipolHists : public DrawObjContainer
{
public:


public:

   CnipolHists();
   CnipolHists(TDirectory *dir);
   ~CnipolHists();

   //void  CnipolHistsBook();
   void  CnipolHistsBook(std::string cutid="");
   void  CnipolHistsBookOld();
   //virtual void Print(const Option_t* opt="") const;
   void  Print(const Option_t* opt="") const;
   Int_t Write(const char* name = 0, Int_t option = 0, Int_t bufsize = 0);
   void  Fill(ChannelEvent *ch, std::string cutid="");
   void  PostFill();
   virtual void  SaveAllAs(TCanvas &c, std::string path="./");

   ClassDef(CnipolHists, 1)
};

#endif
