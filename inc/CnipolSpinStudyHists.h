/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolSpinStudyHists_h
#define CnipolSpinStudyHists_h

#include "TDirectoryFile.h"
#include "TF1.h"
#include "TH2F.h"
#include "TMath.h"

#include "RunConfig.h"

#include "ChannelEvent.h"
#include "DrawObjContainer.h"


/**
 *
 */
class CnipolSpinStudyHists : public DrawObjContainer
{
public:

   CnipolSpinStudyHists();
   CnipolSpinStudyHists(TDirectory *dir);
   ~CnipolSpinStudyHists();

   void      BookHists();
   void      Fill(ChannelEvent *ch);
   void      PostFill();
   void      FillDerived();

   ClassDef(CnipolSpinStudyHists, 1)
};

#endif
