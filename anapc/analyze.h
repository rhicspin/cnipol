
#include "TChain.h"
#include "TFile.h"
#include "TH2F.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TStyle.h"

#include "AnaEvent.h"
#include "EventConfig.h"

TFile        *gOutFile;
TH2F         *hTvsE, *hTvsQ;
TRandom      *gRandom;
//TStyle      *gRandom;

AnaEvent     *gAnaEvent;

void analyze(Long64_t nEvents=-1);
//void analyze_initialize();
//void analyze_book_histograms();
void analyze_fill_histograms(Long64_t nEvents=-1);
//void analyze_fill_histograms_derivative();
//void analyze_finalize();
