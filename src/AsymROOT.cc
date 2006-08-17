//  Asymmetry Analysis of RHIC pC Polarimeter
//  file name :   AsymROOT.cc
// 
//  Author    :   I. Nakagawa
//  Creation  :   7/11/2006
//                

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>
#include "rhicpol.h"
#include "rpoldata.h"
#include "Asym.h"
#include "AsymROOT.h"

// Direcotories
TDirectory * Kinema;
TDirectory * Bunch;
TDirectory * ErrDet;
TDirectory * Asymmetry;


//
//  Histogram Definitions 
//
//  Number arrays are TOT_WFD_CH, not NSTRIP, because there are events with strip>72,73,74,75
//  in Run06 which are target events. These histograms are deleted before ROOT file closing 
//  anyway though, need to be declared to aviod crash in histogram filling rouitne in process_event()
//
// Kinema Dir
TH2F * t_vs_e[TOT_WFD_CH];          // t vs. 12C Kinetic Energy (banana with/o cut)
TH2F * mass_vs_e_ecut[TOT_WFD_CH];  // Mass vs. 12C Kinetic Energy 
TH2F * mass_vs_t_ecut[TOT_WFD_CH];  // Mass vs. ToF (w/ Energy Cut)
TH2F * mass_vs_t[TOT_WFD_CH];       // Mass vs. ToF (w/o Energy Cut)
TF1  * banana_cut_l[NSTRIP];        // banana cut low 
TF1  * banana_cut_h[NSTRIP];        // banana cut high
TLine  * energy_cut_l[NSTRIP];      // energy cut low 
TLine  * energy_cut_h[NSTRIP];      // energy cut high

// Bunch Distribution
TH1F * bunch_dist;                  // counts per bunch

// ErrDet dir
TH2F * mass_sigma_vs_strip;         // Mass sigma width vs. strip 
TH2F * mass_chi2_vs_strip;          // Mass Gaussian fit chi2 vs. strip 
TH2F * mass_e_correlation_strip;    // Mass-energy correlation vs. strip
TH2F * rate_vs_bunch;                       // Counting rate vs. bunch
TH1F * bunch_rate;                          // Counting rate per bunch hisogram
TH1F * asym_bunch_x45;                      // Bunch asymmetry histogram for x45 
TH1F * asym_bunch_x90;                      // Bunch asymmetry histogram for x90 
TH1F * asym_bunch_y45;                      // Bunch asymmetry histogram for y45 

// Asymmetry dir
TH2F * asym_vs_bunch_x45;                   // Asymmetry vs. bunch (x45)
TH2F * asym_vs_bunch_x90;                   // Asymmetry vs. bunch (x90)
TH2F * asym_vs_bunch_y45;                   // Asymmetry vs. bunch (y45)
TH2F * asym_sinphi_fit;                     // strip asymmetry and sin(phi) fit 


//
// Class name  : Root
// Method name : RootFile(char * filename)
//
// Description : Open Root File and define directory structure of histograms
//             : 
// Input       : char *filename
// Return      : 
//
int 
Root::RootFile(char *filename){

  rootfile = new TFile(filename,"RECREATE","ROOT Histogram file");

  // directory structure
  Kinema    = rootfile->mkdir("Kinema");
  Bunch     = rootfile->mkdir("Bunch");
  ErrDet    = rootfile->mkdir("ErrDet");
  Asymmetry = rootfile->mkdir("Asymmetry");

  return 0;

}



//
// Class name  : Root
// Method name : RootHistBook()
//
// Description : Book ROOT Histograms
//             : 
// Input       : 
// Return      : 
//
int 
Root::RootHistBook(){

  Char_t hname[100], htitle[100];

  Kinema->cd();
  for (int i=0; i<TOT_WFD_CH; i++) {

    sprintf(hname,"t_vs_e_st%d",i);
    sprintf(htitle,"%8.3f : t vs. Kin.Energy Str%d ",runinfo.RUNID, i);
    t_vs_e[i] = new TH2F(hname,htitle, 50, 200, 1500, 100, 20, 90);
    t_vs_e[i] -> GetXaxis() -> SetTitle("Kinetic Energy [keV]");
    t_vs_e[i] -> GetYaxis() -> SetTitle("Time of Flight [ns]");

    sprintf(hname,"mass_vs_e_ecut_st%d",i);
    sprintf(htitle,"%8.3f : Mass vs. Kin.Energy (Energy Cut) Str%d ",runinfo.RUNID, i);
    mass_vs_e_ecut[i] = new TH2F(hname,htitle, 50, 200, 1000, 200, 6, 18);
    mass_vs_e_ecut[i] -> GetXaxis() -> SetTitle("Kinetic Energy [keV]");
    mass_vs_e_ecut[i] -> GetYaxis() -> SetTitle("Invariant Mass [GeV]");

    sprintf(hname,"mass_vs_t_ecut_st%d",i);
    sprintf(htitle,"%8.3f : Mass vs. ToF (Energy Cut) Str%d ", runinfo.RUNID, i);
    mass_vs_t_ecut[i] = new TH2F(hname,htitle, 100, 10, 90, 100, 5, 25);
    mass_vs_t_ecut[i] -> GetXaxis() -> SetTitle("Kinetic Energy [keV]");
    mass_vs_t_ecut[i] -> GetYaxis() -> SetTitle("Invariant Mass [GeV]");

    sprintf(hname,"mass_vs_t_st%d",i);
    sprintf(htitle,"%8.3f : Mass vs. ToF Str%d", runinfo.RUNID, i);
    mass_vs_t[i] = new TH2F(hname,htitle, 100, 10, 90, 100, 5, 25);
    mass_vs_t[i] -> GetXaxis() -> SetTitle("Time of Flight [ns]");
    mass_vs_t[i] -> GetYaxis() -> SetTitle("Invariant Mass [GeV]");

  }

  Bunch->cd();
  sprintf(htitle,"%8.3f : Counts per Bunch ", runinfo.RUNID);
  bunch_dist = new TH1F("bunch_dist", htitle, NBUNCH, -0.5, NBUNCH-0.5);
  bunch_dist -> GetXaxis() -> SetTitle("Bunch ID");
  bunch_dist -> GetYaxis() -> SetTitle("Counts");

  // Error detectors
  ErrDet->cd();
  sprintf(htitle,"%8.3f : Bunch Asymmetry X90", runinfo.RUNID);
  asym_bunch_x90 = new TH1F("asym_bunch_x90", htitle, 100, -0.1, 0.1);
  sprintf(htitle,"%8.3f : Bunch Asymmetry X45", runinfo.RUNID);
  asym_bunch_x45 = new TH1F("asym_bunch_x45", htitle, 100, -0.1, 0.1);
  sprintf(htitle,"%8.3f : Bunch Asymmetry Y45", runinfo.RUNID);
  asym_bunch_y45 = new TH1F("asym_bunch_y45", htitle, 100, -0.1, 0.1);


  return 0;

}


//
// Class name  : Root
// Method name : RootFuncBook()
//
// Description : Book ROOT Functions and Histograms using Feedback infomations
//             : This routine shuould be called after Feedback operation
// Input       : 
// Return      : 
//
int 
Root::RootHistBook2(){

  Kinema->cd();
  char formula[100],fname[100];
  float low, high, sqrte;
  for (int i=0;i<NSTRIP; i++) {

    // lower limit
    sprintf(formula,"%f/sqrt(x)+(%f)/sqrt(x)", runconst.E2T, runconst.M2T*feedback.RMS[i]*dproc.MassSigma);
    sprintf(fname,"banana_cut_l_st%d",i);
    banana_cut_l[i] = new TF1(fname, formula, dproc.enel, dproc.eneu);
    banana_cut_l[i] -> SetLineColor(2); 
    banana_cut_l[i] -> SetLineWidth(4); 

    // upper limit
    sprintf(formula,"%f/sqrt(x)-(%f)/sqrt(x)", runconst.E2T, runconst.M2T*feedback.RMS[i]*dproc.MassSigma);
    sprintf(fname,"banana_cut_h_st%d",i);
    banana_cut_h[i] = new TF1(fname, formula, dproc.enel, dproc.eneu);
    banana_cut_h[i] -> SetLineColor(2); 
    banana_cut_h[i] -> SetLineWidth(4); 

    // energy cut low
    low  = runconst.E2T/sqrt(dproc.enel)-runconst.M2T*feedback.RMS[i]*dproc.MassSigma/sqrt(dproc.enel);
    high = runconst.E2T/sqrt(dproc.enel)+runconst.M2T*feedback.RMS[i]*dproc.MassSigma/sqrt(dproc.enel);
    energy_cut_l[i] = new TLine(dproc.enel, low, dproc.enel, high);
    energy_cut_l[i] ->SetLineColor(2);
    energy_cut_l[i] ->SetLineWidth(4);

    // energy cut high
    low  = runconst.E2T/sqrt(dproc.eneu)-runconst.M2T*feedback.RMS[i]*dproc.MassSigma/sqrt(dproc.eneu);
    high = runconst.E2T/sqrt(dproc.eneu)+runconst.M2T*feedback.RMS[i]*dproc.MassSigma/sqrt(dproc.eneu);
    energy_cut_h[i] = new TLine(dproc.eneu, low, dproc.eneu, high);
    energy_cut_h[i] ->SetLineColor(2);
    energy_cut_h[i] ->SetLineWidth(4);

  }


  return 0;

}



//
// Class name  : Root
// Method name : DeleteHistogram
//
// Description : Delete Unnecessary Histograms
//             : 
// Input       : 
// Return      : 
//
int 
Root::DeleteHistogram(){

  
  // Delete histograms declared for WFD channel 72 - 75 to avoid crash. These channcles 
  // are for target channels and thus thes histograms wouldn't make any sense.
  for (int i=NSTRIP; i<TOT_WFD_CH; i++ ) {

    t_vs_e[i] -> Delete();
    mass_vs_e_ecut[i] -> Delete();  // Mass vs. 12C Kinetic Energy 
    mass_vs_t_ecut[i] -> Delete();  // Mass vs. ToF (w/ Energy Cut)
    mass_vs_t[i] -> Delete();       // Mass vs. ToF (w/o Energy Cut)

  }

  
  return 0;

}




//
// Class name  : Root
// Method name : RootFile(char * filename)
//
// Description : Write out objects in memory and dump in rootfile before closing it
//             : 
// Input       : 
// Return      : 
//
int 
Root::CloseROOTFile(){
  

  Kinema->cd();
  TLine * l;
  for (int i=0;i<NSTRIP; i++){
    if (t_vs_e[i]) {
      if (banana_cut_l[i]) t_vs_e[i] -> GetListOfFunctions() -> Add(banana_cut_l[i]);
      if (banana_cut_h[i]) t_vs_e[i] -> GetListOfFunctions() -> Add(banana_cut_h[i]);
      if (energy_cut_l[i]) t_vs_e[i] -> GetListOfFunctions() -> Add(energy_cut_l[i]);
      if (energy_cut_h[i]) t_vs_e[i] -> GetListOfFunctions() -> Add(energy_cut_h[i]);
    }
  }

  // Write out memory before closing
  /*
  ErrDet->cd();
  if (mass_sigma_vs_strip)      mass_sigma_vs_strip      -> Write();
  if (mass_chi2_vs_strip)       mass_chi2_vs_strip       -> Write();
  if (mass_e_correlation_strip) mass_e_correlation_strip -> Write();
  if (bunch_rate)               bunch_rate    -> Write();
  if (rate_vs_bunch)            rate_vs_bunch -> Write();
  if (asym_bunch_x90)           asym_bunch_x90-> Write();
  if (asym_bunch_x45)           asym_bunch_x45-> Write();
  if (asym_bunch_y45)           asym_bunch_y45-> Write();


  Asymmetry->cd();
  if (asym_sinphi_fit)   asym_sinphi_fit   -> Write();
  if (asym_vs_bunch_x45) asym_vs_bunch_x45 -> Write();
  if (asym_vs_bunch_x90) asym_vs_bunch_x90 -> Write();
  if (asym_vs_bunch_y45) asym_vs_bunch_y45 -> Write();

  */

  rootfile->Write();


  // close rootfile
  rootfile->Close();

  return 0;

}


