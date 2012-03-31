/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolKinematHists_h
#define CnipolKinematHists_h

#include "TDirectoryFile.h"

#include "AsymHeader.h"

#include "ChannelEvent.h"
#include "DrawObjContainer.h"


/**
 *
 */
class CnipolKinematHists : public DrawObjContainer
{
private:

   TH1* fhPseudoMass_ch[N_SILICON_CHANNELS];


public:

   CnipolKinematHists();
   CnipolKinematHists(TDirectory *dir);
   ~CnipolKinematHists();

   void BookHists();
   void Fill(ChannelEvent *ch);
   void FillDerived();
   void PostFill();

   ClassDef(CnipolKinematHists, 1)
};

#endif
