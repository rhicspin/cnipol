/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolProfileHists_h
#define CnipolProfileHists_h

#include "TDirectoryFile.h"
#include "TF1.h"
#include "TH2F.h"
#include "TMath.h"
#include "TPaveStats.h"

#include "AsymHeader.h"
#include "TargetInfo.h"
#include "RunInfo.h"
#include "RunConfig.h"

#include "ChannelEvent.h"
#include "DrawObjContainer.h"


/**
 *
 */
class CnipolProfileHists : public DrawObjContainer
{
public:


public:

   CnipolProfileHists();
   CnipolProfileHists(TDirectory *dir);
   ~CnipolProfileHists();

   void      BookHists(std::string sid="");
   void      PreFill(std::string sid);
   void      Fill(ChannelEvent *ch, std::string cutid="");
   void      Fill(UInt_t n, Long_t* hData);
   void      PostFill();
   EMeasType GuessMeasurementType();

   static Double_t ProfileFitFunc(Double_t *x, Double_t *par);

   ClassDef(CnipolProfileHists, 1)
};

#endif
