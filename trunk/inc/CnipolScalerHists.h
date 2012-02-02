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

   void  BookHists(std::string sid="");
   //virtual void Print(const Option_t* opt="") const;
   void  Print(const Option_t* opt="") const;
   //void  Fill(ChannelEvent *ch, std::string sid="");
   void  Fill(Long_t *hData, UShort_t chId, std::string sid="");
   //virtual void SaveAllAs(TCanvas &c, std::string pattern="^.*$", std::string path="./");

   ClassDef(CnipolScalerHists, 1)
};

#endif
