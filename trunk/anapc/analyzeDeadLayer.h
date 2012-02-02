
#ifndef analyzeDeadLayer_h
#define analyzeDeadLayer_h

#include <iostream>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TF1.h"
#include "TH2F.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TString.h"

#include "AsymHeader.h"

#include "AnaEvent.h"
#include "ChannelCalib.h"
#include "CnipolHists.h"
#include "CnipolRunHists.h"
#include "CnipolProfileHists.h"
#include "DrawObjContainer.h"
#include "EventConfig.h"
#include "DeadLayerCalibratorEDepend.h"

namespace {

TFile            *gInFile;
TFile            *gOutFile;
TChain           *gChain;
TH1F             *hAmpltd, *hIntgrl, *hTdc;
TH1F             *hEventSeq;
TH2F             *hTvsE, *hTvsQ, *hEvsQ;
TRandom          *gRandom;
TCanvas          *c1;
//TStyle          *gRandom;
std::string       gRunName;
std::string       gOutDir;

AnaEvent         *gAnaEvent;
DrawObjContainer *gH;
EventConfig      *ec;

}

void analyzeDeadLayer(std::string runName, std::string pattern="^.*$", Long64_t nEvents=-1);
void analyzeDeadLayer_initialize(std::string runName);
//void analyzeDeadLayer_book_histograms();
//void analyzeDeadLayer_fill_histograms(Long64_t nEvents=-1);
void analyzeDeadLayer_fit_histograms();
//TFitResultPtr analyzeDeadLayer_fit_histograms(TH1 *h, TF1 *f);
//void analyzeDeadLayer_fill_histograms_derivative();
void analyzeDeadLayer_finalize(std::string pattern="^.*$");

#endif
