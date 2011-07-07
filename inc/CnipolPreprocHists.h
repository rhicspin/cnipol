/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolPreprocHists_h
#define CnipolPreprocHists_h

#include "AsymHeader.h"

#include "ChannelEvent.h"
#include "DrawObjContainer.h"


/**
 *
 */
class CnipolPreprocHists : public DrawObjContainer
{
public:

   CnipolPreprocHists();
   CnipolPreprocHists(TDirectory *dir);
   ~CnipolPreprocHists();

   void BookHists(std::string cutid="");
   //void Fill(ChannelEvent *ch, std::string cutid="");
   void FillPreProcess(ChannelEvent *ch);
   void SaveAllAs(TCanvas &c, std::string pattern="^.*$", std::string path="./", Bool_t thumbs=kFALSE);

   ClassDef(CnipolPreprocHists, 1)
};

#endif
