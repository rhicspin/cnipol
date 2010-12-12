
#ifndef analyzeCalib_h
#define analyzeCalib_h

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
#include "EventConfig.h"
#include "CnipolCalibHists.h"

TFile        *gInFile;
TFile        *gOutFile;
TChain       *gChain;
TH1F         *hAmpltd, *hIntgrl, *hTdc;
TH1F         *hEventSeq;
TH2F         *hTvsE, *hTvsQ, *hEvsQ;
TRandom      *gRandom;
TCanvas      *c1;
//TStyle      *gRandom;
std::string   gRunName;
std::string   gOutDir;

AnaEvent         *gAnaEvent;
CnipolCalibHists *gH;
EventConfig      *ec;

void analyzeCalib(std::string runName, Long64_t nEvents=-1);
void analyzeCalib_initialize(std::string runName);
//void analyzeCalib_book_histograms();
void analyzeCalib_fill_histograms(Long64_t nEvents=-1);
void analyzeCalib_fit_histograms();
TFitResultPtr analyzeCalib_fit_histograms(TH1 *h, TF1 *f);
void analyzeCalib_restore_channels();
//void analyzeCalib_fill_histograms_derivative();
void analyzeCalib_finalize();

#endif
