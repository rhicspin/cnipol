#ifndef ASYM_ROOT_H
#define ASYM_ROOT_H
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include "TText.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TPostScript.h"
#include "TSystem.h"

// defaults
const float MSIZE=1.2; // Marker size

struct StructHist {
  int nxbin;
  float xmin;
  float xmax;
  int nybin;
  float ymin;
  float ymax;
} ;


class Root
{
 private:
  
 public:
  int RootFile(char*filename);
  int RootHistBook(StructRunInfo runinfo);
  int RootHistBook2(datprocStruct dproc, StructRunConst runconst, StructFeedBack feedback);
  int DeleteHistogram();
  int CloseROOTFile();

  TFile * rootfile;
  


};

// global constants
extern const float MSIZE;


// ROOT Histograms
extern TDirectory * Kinema;
extern TDirectory * Bunch;
extern TDirectory * ErrDet;
extern TDirectory * Asymmetry;

// Kinema Direcotry
extern TH2F * t_vs_e[TOT_WFD_CH];
extern TH2F * t_vs_e_yescut[TOT_WFD_CH];
extern TH2F * mass_vs_e_ecut[TOT_WFD_CH];  // Mass vs. 12C Kinetic Energy 
extern TF1  * banana_cut_l[NSTRIP][2];     // banana cut low 
extern TF1  * banana_cut_h[NSTRIP][2];     // banana cut high
extern TLine  * energy_cut_l[NSTRIP];      // energy cut low 
extern TLine  * energy_cut_h[NSTRIP];      // energy cut high
extern TH1F  * energy_spectrum[NDETECTOR]; // energy spectrum per detector
extern TH1F  * energy_spectrum_all;        // energy spectrum for all detector sum
extern TH1F  * mass_nocut[TOT_WFD_CH];     // invariant mass without banana cut
extern TH1F  * mass_yescut[TOT_WFD_CH];    // invariant mass with banana cut

// Bunch Distribution
extern TH1F * bunch_dist;                  // counts per bunch
extern TH1F * wall_current_monitor;        // wall current monitor
extern TH1F * specific_luminosity;         // specific luminosity

// ErrDet Direcotry
extern TH2F * mass_chi2_vs_strip;          // Chi2 of Gaussian Fit on Mass peak
extern TH2F * mass_sigma_vs_strip;         // Mass sigma width vs. strip 
extern TH2F * mass_e_correlation_strip;    // Mass-energy correlation vs. strip
extern TH2F * mass_pos_dev_vs_strip;       // Mass position deviation vs. strip
extern TH1I * good_carbon_events_strip;    // number of good carbon events per strip
extern TH2F * spelumi_vs_bunch;            // Counting rate vs. bunch
extern TH1F * bunch_spelumi;               // Counting rate per bunch hisogram
extern TH1F * asym_bunch_x45;              // Bunch asymmetry histogram for x45 
extern TH1F * asym_bunch_x90;              // Bunch asymmetry histogram for x90 
extern TH1F * asym_bunch_y45;              // Bunch asymmetry histogram for y45 

// Asymmetry Directory
extern TH2F * asym_vs_bunch_x45;           // Asymmetry vs. bunch (x45)
extern TH2F * asym_vs_bunch_x90;           // Asymmetry vs. bunch (x90)
extern TH2F * asym_vs_bunch_y45;           // Asymmetry vs. bunch (y45)
extern TH2F * asym_sinphi_fit;             // strip asymmetry and sin(phi) fit  






#endif /* ASYM_ROOT_H */

