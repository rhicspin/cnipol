/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolRawExtendedHists_h
#define CnipolRawExtendedHists_h

#include "TDirectoryFile.h"

#include "AsymHeader.h"

#include "ChannelEvent.h"
#include "CnipolRawHists.h"


/**
 *
 */
class CnipolRawExtendedHists : public CnipolRawHists
{
protected:

   TH1 *fhTvsA_ch_b[N_SILICON_CHANNELS][N_BUNCHES];
   TH1 *fhTvsACumul_ch_b[N_SILICON_CHANNELS][N_BUNCHES];


public:

   CnipolRawExtendedHists();
   CnipolRawExtendedHists(TDirectory *dir);
   ~CnipolRawExtendedHists();

   void  BookHists();
   void  FillPassOne(ChannelEvent *ch);
   void  FillDerivedPassOne();

   ClassDef(CnipolRawExtendedHists, 1)
};

#endif
