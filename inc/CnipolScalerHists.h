/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolScalerHists_h
#define CnipolScalerHists_h

#include "TDirectoryFile.h"
#include "TF1.h"
#include "TH2F.h"

#include "AsymHeader.h"

#include "ChannelEvent.h"
#include "DrawObjContainer.h"


/**
 *
 */
class CnipolScalerHists : public DrawObjContainer
{
public:


public:

   CnipolScalerHists();
   CnipolScalerHists(TDirectory *dir);
   ~CnipolScalerHists();

   void  BookHists();
   //void  Fill(ChannelEvent *ch);
   void  Fill(Long_t *hData, UShort_t chId);
   //virtual void SaveAllAs(TCanvas &c, std::string pattern="^.*$", std::string path="./");

   ClassDef(CnipolScalerHists, 1)
};

#endif
