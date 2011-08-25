/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolPulserHists_h
#define CnipolPulserHists_h

#include "TDirectoryFile.h"

#include "AsymHeader.h"

#include "ChannelEvent.h"
#include "DrawObjContainer.h"


/**
 *
 */
class CnipolPulserHists : public DrawObjContainer
{
private:

   TH1 *fhBunchCounts;
   TH1 *fhStripCounts;

   TH2 *fhTvsA_ch[N_SILICON_CHANNELS];
   TH2 *fhTvsI_ch[N_SILICON_CHANNELS];
   TH2 *fhIvsA_ch[N_SILICON_CHANNELS];
   TH2 *fhTimeVsEnergyA_ch[N_SILICON_CHANNELS];

public:

   CnipolPulserHists();
   CnipolPulserHists(TDirectory *dir);
   ~CnipolPulserHists();

   void BookHists(std::string cutid="");
   void FillPassOne(ChannelEvent *ch);
   void FillDerivedPassOne();
   void Fill(ChannelEvent *ch, std::string cutid="");

   ClassDef(CnipolPulserHists, 1)
};

#endif
