/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolHists_h
#define CnipolHists_h

#include "TDirectoryFile.h"
#include "TF1.h"
#include "TH2F.h"
#include "TPaveStats.h"

#include "AsymHeader.h"

#include "ChannelEvent.h"
#include "DrawObjContainer.h"


/**
 *
 */
class CnipolHists : public DrawObjContainer
{

private:

   TH1* fhKinEnergyA_o;
   TH2* fhSpinVsChannel;
   TH2* fhSpinVsBunch;
   TH2* fhTimeVsEnergyA_ch[N_SILICON_CHANNELS];
   //TH2* fhTofVsKinEnergyA_ch[N_SILICON_CHANNELS];
   TH2* fhSpinVsDelim_ch[N_SILICON_CHANNELS];
   TH2* fhTofVsKinEnergyA_ch_ss[N_SILICON_CHANNELS][N_SPIN_STATES];
   TH2* fhLongiTimeDiffVsEnergyA_ch_ss[N_SILICON_CHANNELS][N_SPIN_STATES];

protected:

   void  ConvertRawToKin(TH2* hRaw, TH2* hKin, UShort_t chId);

public:

   CnipolHists();
   CnipolHists(TDirectory *dir);
   ~CnipolHists();

   void BookHists();
   void BookHistsExtra();
   void PreFill();
   void Fill(ChannelEvent *ch);
   void FillDerived();
   void PostFill();
   void SaveAllAs(TCanvas &c, std::string pattern="^.*$", std::string path="./", Bool_t thumbs=kFALSE);

   ClassDef(CnipolHists, 1)
};

#endif
