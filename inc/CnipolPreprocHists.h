/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolPreprocHists_h
#define CnipolPreprocHists_h

#include "AsymHeader.h"

#include "ChannelEvent.h"
#include "CnipolRawHists.h"
#include "DrawObjContainer.h"


/**
 *
 */
class CnipolPreprocHists : public DrawObjContainer
{
private:

   TH1 *fhTimeVsEnergyA_ch[N_SILICON_CHANNELS];
   TH1 *fhFitMeanTimeVsEnergyA_ch[N_SILICON_CHANNELS];
   TH1 *fhFitChi2NdfVsEnergyA_ch[N_SILICON_CHANNELS];
   TH1 *fhFitChi2NdfLogVsEnergyA_ch[N_SILICON_CHANNELS];
   TH1 *fhTimeVsEnergyACumul_ch[N_SILICON_CHANNELS];

   void FillDerivedPassOne_SubtractEmptyBunch(CnipolPreprocHists *preprocHists=0);
   void FillDerivedPassOne_FillFromRawHists(CnipolRawHists *rawHists=0);

public:

   CnipolPreprocHists();
   CnipolPreprocHists(TDirectory *dir);
   ~CnipolPreprocHists();

   void BookHists();
   void FillPassOne(ChannelEvent *ch);
   void FillDerivedPassOne(DrawObjContainer &oc);
   void PostFillPassOne(DrawObjContainer *oc=0);
   void SaveAllAs(TCanvas &c, std::string pattern="^.*$", std::string path="./", Bool_t thumbs=kFALSE);

   ClassDef(CnipolPreprocHists, 1)
};

#endif
