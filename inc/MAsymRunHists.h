/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef MAsymRunHists_h
#define MAsymRunHists_h

#include "TDirectoryFile.h"
#include "TH2F.h"
#include "TGraphErrors.h"

#include "AsymHeader.h"

#include "ChannelEvent.h"
#include "DrawObjContainer.h"


/**
 *
 */
class MAsymRunHists : public DrawObjContainer
{
public:


public:

   MAsymRunHists();
   MAsymRunHists(TDirectory *dir);
   ~MAsymRunHists();

   void BookHists(std::string sid="");
   void Fill(ChannelEvent *ch, std::string cutid="");
   //void Fill(Int_t n, Double_t* hData);
   //void PostFill();
   void Print(const Option_t* opt="") const;

   ClassDef(MAsymRunHists, 1)
};

#endif
