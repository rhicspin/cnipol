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

protected:

   void  ConvertRawToKin(TH2* hRaw, TH2* hKin, UShort_t chId);

public:

   CnipolHists();
   CnipolHists(TDirectory *dir);
   ~CnipolHists();

   void BookHists(std::string cutid="");
   void BookHistsExtra(std::string cutid="");
   void PreFill(std::string cutid="");
   void Fill(ChannelEvent *ch, std::string cutid="");
   void PostFill();
   void SaveAllAs(TCanvas &c, std::string pattern="^.*$", std::string path="./", Bool_t thumbs=kFALSE);

   ClassDef(CnipolHists, 1)
};

#endif
