#ifndef MAsymBiasHists_h
#define MAsymBiasHists_h

#include <TDirectory.h>
#include <TGraphErrors.h>
#include <TH2F.h>

#include "AsymGlobals.h"
#include "ChannelEvent.h"
#include "DrawObjContainer.h"
#include "RunConfig.h"


class MAsymBiasHists : public DrawObjContainer
{
private:

   TGraphErrors *grBiasCurrent[N_POLARIMETERS][N_DETECTORS];
   TH2F *hBiasCurrent[N_POLARIMETERS][N_DETECTORS];

public:

   MAsymBiasHists();
   MAsymBiasHists(TDirectory *dir);

   void BookHists(std::string sid="");
   void BookHistsPolarimeter(EPolarimeterId polId);
   void Fill(const EventConfig &rc);
   virtual void SaveAllAs(ImageFormat fmt_ext, TCanvas &default_canvas, std::string pattern, std::string path, Bool_t thumbs);
   void UpdateLimits();

   ClassDef(MAsymBiasHists, 1)
};

#endif
