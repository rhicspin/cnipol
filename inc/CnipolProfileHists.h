
#ifndef CnipolProfileHists_h
#define CnipolProfileHists_h

#include "TDirectoryFile.h"
#include "TF1.h"
#include "TH2F.h"
#include "TMath.h"
#include "TPaveStats.h"

#include "AsymHeader.h"
#include "TargetInfo.h"
#include "MeasInfo.h"
#include "RunConfig.h"

#include "ChannelEvent.h"
#include "DrawObjContainer.h"


/**
 *
 */
class CnipolProfileHists : public DrawObjContainer
{
private:

   TH1* fhPolarProfile; //!
   TH1* fhIntensProfile;
   TH1* fhIntensProfile_ch[N_SILICON_CHANNELS];
   TH1* fhAsymVsIntensProfile; //!
   TH1* fhIntensProfileFine; //!
   TH1* fhIntensProfileFine_ch[N_SILICON_CHANNELS]; //!

public:

   CnipolProfileHists();
   CnipolProfileHists(TDirectory *dir);
   ~CnipolProfileHists();

   void      BookHists();
   void      PreFill();
   void      Fill(ChannelEvent *ch);
   void      Fill(UInt_t n, int32_t* hData);
   void      PostFill();
   void      FillDerived(DrawObjContainer &oc);
   EMeasType GuessMeasurementType();

   //static Double_t ProfileFitFunc(Double_t *x, Double_t *par);
   Double_t ProfileFitFunc(Double_t *x, Double_t *par);
   Double_t ProfileTailFitFunc(Double_t *x, Double_t *par);

   ClassDef(CnipolProfileHists, 1)
};

#endif
