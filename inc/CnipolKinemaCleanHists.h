
#ifndef CnipolKinemaCleanHists_h
#define CnipolKinemaCleanHists_h

#include "TDirectoryFile.h"

#include "AsymHeader.h"

#include "ChannelEvent.h"
#include "CnipolKinematHists.h"


/**
 *
 */
class CnipolKinemaCleanHists : public CnipolKinematHists
{
private:

   TH1* fhPseudoMass_ch[N_SILICON_CHANNELS];


public:

   CnipolKinemaCleanHists();
   CnipolKinemaCleanHists(TDirectory *dir);
   ~CnipolKinemaCleanHists();

   void BookHists();
   void Fill(ChannelEvent *ch);
   void FillDerived();
   void PostFill();

   ClassDef(CnipolKinemaCleanHists, 1)
};

#endif
