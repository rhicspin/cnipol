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



class Root
{
 private:
  
 public:
  int RootFile(char*filename);
  int RootHistBook();
  int DeleteHistogram();
  int CloseROOTFile();

  TFile * rootfile;
  


};



// ROOT Histograms
extern  TH2F * t_vs_e[TOT_WFD_CH];
extern  TH2F * mass_vs_e_ecut[TOT_WFD_CH];  // Mass vs. 12C Kinetic Energy 
extern  TH2F * mass_vs_t_ecut[TOT_WFD_CH];  // Mass vs. ToF (w/ Energy Cut)
extern  TH2F * mass_vs_t[TOT_WFD_CH];       // Mass vs. ToF (w/o Energy Cut)



#endif /* ASYM_ROOT_H */

