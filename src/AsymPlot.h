/**
 *
 * 18 Oct, 2010 - Dmitri Smirnov
 *    - Created file
 *
 */

#ifndef ASYMPLOT_H
#define ASYMPLOT_H

#include <math.h>
#include <iostream>
#include <fstream>
#include <string>

#include "TFile.h"
#include "TCanvas.h"
#include "TColor.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TPostScript.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TSystem.h"

#include "rhicpol.h"
#include "rpoldata.h"

#include "Asym.h"

static Int_t GHOSTVIEW      = 0;
static Int_t FEEDBACK       = 0;
static Int_t PLOT_RAW       = 0;
static Int_t PLOT_BANANA    = 1;
static Int_t ERROR_DETECTOR = 1;
static Int_t SUMMARY        = 1;
static Int_t stID           = 0;
static std::string RUNID;
static Char_t lnkfile[50]="AsymPlot.root";

extern  ofstream logfile;

void FindRootFile();

class AsymPlot
{
private:
  
public:
  Int_t GetHistograms(TFile * rootfile);
  Int_t PlotFeedback(TFile * rootfile, TCanvas *CurC, TPostScript * ps);
  Int_t PlotRaw(TFile * rootfile, TCanvas *CurC, TPostScript * ps);
  Int_t PlotErrorDetector(TFile * rootfile, TCanvas *CurC, TPostScript * ps);
  Int_t PlotErrorDetectorSummary(TFile * rootfile, TCanvas *CurC, TPostScript * ps);
  // Directory "FeedBack"
  TH2F * mdev_feedback;  
  // Raw Directory
  TH1F * bunch_dist_raw;              // counts per bunch (raw)
  TH1F * strip_dist_raw;              // counts per strip (raw)
  TH1F * tdc_raw;                     // tdc (raw)
  TH1F * adc_raw;                     // adc (raw)
  TH2F * tdc_vs_adc_raw;              // tdc vs. adc (raw)
  // Directory "ErrDet"
  TH2F * mass_e_correlation_strip;
  TH2F * mass_chi2_vs_strip;
  TH1I * good_carbon_events_strip;
  TH2F * mass_pos_dev_vs_strip;
  TH2F * spelumi_vs_bunch;
  TH1F * bunch_spelumi;
  TH1F * asym_bunch_x90;
  TH1F * asym_bunch_x45;
  TH1F * asym_bunch_y45;
  // Directory "Asymmetry"
  TH2F * asym_vs_bunch_x90;
  TH2F * asym_vs_bunch_x45;
  TH2F * asym_vs_bunch_y45;
  TH2F * asym_sinphi_fit;
  // Directory "Bunch"
  TH1F * bunch_dist;
  TH1F * wall_current_monitor;
  TH1F * specific_luminosity;
  // Directory "Kinema"
  TH1F * energy_spectrum_all;

};

#endif
