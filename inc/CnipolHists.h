/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolHists_h
#define CnipolHists_h

#include "TDirectoryFile.h"
#include "TF1.h"
#include "TH2F.h"
#include "TPaveStats.h"

#include "AsymHeader.h"

#include "ChannelEvent.h"
#include "DrawObjContainer.h"


/**
 *
 */
class CnipolHists : public DrawObjContainer
{
public:

   CnipolHists();
   CnipolHists(TDirectory *dir);
   ~CnipolHists();

   void  BookHists(std::string cutid="");
   void  BookHistsExtra(std::string cutid="");
   //Int_t Write(const char* name = 0, Int_t option = 0, Int_t bufsize = 0);
   //Int_t Write(const char* name = 0, Int_t option = 0, Int_t bufsize = 0) const;
   void  Fill(ChannelEvent *ch, std::string cutid="");
   void  PreFill(std::string sid="");
   void  PostFill();
   void  SaveAllAs(TCanvas &c, std::string pattern="^.*$", std::string path="./");

   ClassDef(CnipolHists, 1)
};

#endif
