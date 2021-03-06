
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

   static const double ALPHA_TDC_CUT;
   std::set<int> bad_tdc_bins;

   CnipolAlphaHists();
   CnipolAlphaHists(TDirectory *dir);
   virtual ~CnipolAlphaHists();

   void BookHists();
   void FillPassOne(ChannelEvent *ch);
   void PostFill();
   void PostFillPassOne(DrawObjContainer *oc=0);
   void Fill(ChannelEvent *ch);

   ClassDef(CnipolAlphaHists, 1)
};

#endif
