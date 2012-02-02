/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolAlphaHists_h
#define CnipolAlphaHists_h

#include "TClass.h"
#include "TDirectoryFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TSystem.h"

#include "AsymHeader.h"

#include "DrawObjContainer.h"
#include "ChannelEvent.h"


/** */
class CnipolAlphaHists : public DrawObjContainer
{
public:


public:

   CnipolAlphaHists();
   CnipolAlphaHists(TDirectory *dir);
   virtual ~CnipolAlphaHists();

   void BookHists(std::string cutid="");
   void Fill(ChannelEvent *ch, std::string cutid="");
   void FillPreProcess(ChannelEvent *ch);
   void PostFill();
   //void SaveAllAs(TCanvas &c, std::string pattern="*", std::string path="./");

   ClassDef(CnipolAlphaHists, 1)
};

#endif
