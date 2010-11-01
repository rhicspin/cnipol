
#include "TChain.h"
#include "TFile.h"
#include "TH2F.h"
#include "TRandom.h"
#include "TCanvas.h"

#include "AnaEvent.h"
#include "EventConfig.h"

TH2F         *hTvsE, *hTvsQ;
AnaEvent     *gAnaEvent;
TRandom      *gRandom;

void testCalib(Long64_t nEvents=-1);
