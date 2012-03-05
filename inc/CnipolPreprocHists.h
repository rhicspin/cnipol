/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolPreprocHists_h
#define CnipolPreprocHists_h

#include "AsymHeader.h"

#include "ChannelEvent.h"
#include "DrawObjContainer.h"


/**
 *
 */
class CnipolPreprocHists : public DrawObjContainer
{
private:

   TH1 *fhTimeVsEnergyA_ch[N_SILICON_CHANNELS];

public:

   CnipolPreprocHists();
   CnipolPreprocHists(TDirectory *dir);
   ~CnipolPreprocHists();

   void BookHists();
   void FillPassOne(ChannelEvent *ch);
   void FillDerivedPassOne();
   void PostFillPassOne(DrawObjContainer *oc=0);
   void SaveAllAs(TCanvas &c, std::string pattern="^.*$", std::string path="./", Bool_t thumbs=kFALSE);
   //void ConvertRawToKin(TH2* hRaw, TH2* hKin, UShort_t chId);

   ClassDef(CnipolPreprocHists, 1)
};

#endif
