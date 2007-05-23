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


// global declarations
StructHist Eslope;

// Direcotories
TDirectory * Run;
TDirectory * FeedBack;
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
// Run Dir
TH2F * rate_vs_delim;

// FeedBack Dir
TH2F  * mdev_feedback;
TH1F  * mass_feedback[TOT_WFD_CH];   // invariant mass for feedback 

// Kinema Dir
TH2F  * t_vs_e[TOT_WFD_CH];          // t vs. 12C Kinetic Energy (banana with/o cut)
TH2F  * t_vs_e_yescut[TOT_WFD_CH];   // t vs. 12C Kinetic Energy (banana with cut)
TH2F  * mass_vs_e_ecut[TOT_WFD_CH];  // Mass vs. 12C Kinetic Energy 
TF1   * banana_cut_l[NSTRIP][2];     // banana cut low     [0]: regular [1] alternative sigma cut
TF1   * banana_cut_h[NSTRIP][2];     // banana cut high    [0]: regular [1] alternative sigma cut
TLine * energy_cut_l[NSTRIP];        // energy cut low 
TLine * energy_cut_h[NSTRIP];        // energy cut high
TH1F  * energy_spectrum[NDETECTOR];  // energy spectrum per detector
TH1F  * energy_spectrum_all;         // energy spectrum for all detector sum
TH1F  * mass_nocut[TOT_WFD_CH];      // invariant mass without banana cut
TH1F  * mass_yescut[TOT_WFD_CH];     // invariant mass with banana cut

// Bunch Distribution
TH1F * bunch_dist;                  // counts per bunch
TH1F * wall_current_monitor;        // wall current monitor
TH1F * specific_luminosity;         // specific luminosity

// ErrDet dir
TH2F * mass_chi2_vs_strip;          // Mass Gaussian fit chi2 vs. strip 
TH2F * mass_sigma_vs_strip;         // Mass sigma width vs. strip 
TH2F * mass_e_correlation_strip;    // Mass-energy correlation vs. strip
TH2F * mass_pos_dev_vs_strip;       // Mass position deviation vs. strip
TH1I * good_carbon_events_strip;    // number of good carbon events per strip
TH2F * spelumi_vs_bunch;                    // Specific Luminosity vs. bunch
TH1F * bunch_spelumi;                       // Specific Luminosity bunch hisogram
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
  Run       = rootfile->mkdir("Run");
  FeedBack  = rootfile->mkdir("FeedBack");
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
Root::RootHistBook(StructRunInfo runinfo){


  Char_t hname[100], htitle[100];

  Kinema->cd();
  // 1-dim Energy Spectrum
  Eslope.nxbin=100; Eslope.xmin=0; Eslope.xmax=0.03;
  for (int i=0; i<NDETECTOR; i++) {
    sprintf(hname,"energy_spectrum_det%d",i+1);
    sprintf(htitle,"%8.3f : Energy Spectrum Detector %d ",runinfo.RUNID, i+1);
    energy_spectrum[i] = new TH1F(hname,htitle, Eslope.nxbin, Eslope.xmin, Eslope.xmax);
    energy_spectrum[i] -> GetXaxis() -> SetTitle("Momentum Transfer [-GeV/c]^2");
  }
  sprintf(htitle,"%8.3f : Energy Spectrum (All Detectors)",runinfo.RUNID);
  energy_spectrum_all = new TH1F("energy_spectrum_all",htitle, Eslope.nxbin, Eslope.xmin, Eslope.xmax);
  energy_spectrum_all -> GetXaxis() -> SetTitle("Momentum Transfer [-GeV/c]^2");


  // Need to book for TOT_WFD_CH instead of NSTRIP to avoid seg. fault by filling histograms by
  // target events strip [73 - 76].
  for (int i=0; i<TOT_WFD_CH; i++) { 

    sprintf(hname,"t_vs_e_st%d",i+1);
    sprintf(htitle,"%8.3f : t vs. Kin.Energy Strip-%d ",runinfo.RUNID, i+1);
    t_vs_e[i] = new TH2F(hname,htitle, 50, 200, 1500, 100, 20, 90);
    t_vs_e[i] -> GetXaxis() -> SetTitle("Kinetic Energy [keV]");
    t_vs_e[i] -> GetYaxis() -> SetTitle("Time of Flight [ns]");

    sprintf(hname,"t_vs_e_yescut_st%d",i+1);
    sprintf(htitle,"%8.3f : t vs. Kin.Energy (with cut) Strip-%d ",runinfo.RUNID, i+1);
    t_vs_e_yescut[i] = new TH2F(hname,htitle, 50, 200, 1500, 100, 20, 90);
    t_vs_e_yescut[i] -> GetXaxis() -> SetTitle("Kinetic Energy [keV]");
    t_vs_e_yescut[i] -> GetYaxis() -> SetTitle("Time of Flight [ns]");

    sprintf(hname,"mass_vs_e_ecut_st%d",i+1);
    sprintf(htitle,"%8.3f : Mass vs. Kin.Energy (Energy Cut) Strip-%d ",runinfo.RUNID, i+1);
    mass_vs_e_ecut[i] = new TH2F(hname,htitle, 50, 200, 1000, 200, 6, 18);
    mass_vs_e_ecut[i] -> GetXaxis() -> SetTitle("Kinetic Energy [keV]");
    mass_vs_e_ecut[i] -> GetYaxis() -> SetTitle("Invariant Mass [GeV]");

    sprintf(hname,"mass_nocut_st%d",i+1);
    sprintf(htitle,"%8.3f : Invariant Mass (nocut) for Strip-%d ",runinfo.RUNID, i+1);
    mass_nocut[i] = new TH1F(hname, htitle, 100, 0, 20);     
    mass_nocut[i] -> GetXaxis() -> SetTitle("Mass [GeV/c^2]");

    sprintf(hname,"mass_yescut_st%d",i+1);
    sprintf(htitle,"%8.3f : Invariant Mass (w/cut) for Strip-%d ",runinfo.RUNID, i+1);
    mass_yescut[i] = new TH1F(hname, htitle, 100, 0, 20);     
    mass_yescut[i] -> GetXaxis() -> SetTitle("Mass [GeV/c^2]");
    mass_yescut[i] -> SetLineColor(2);



  }


  // FeedBack Directory
  FeedBack->cd();
  for (int i=0; i<TOT_WFD_CH; i++) { 

    sprintf(hname,"mass_feedback_st%d",i+1);
    sprintf(htitle,"%8.3f : Invariant Mass (feedback) for Strip-%d ",runinfo.RUNID, i+1);
    mass_feedback[i] = new TH1F(hname, htitle, 100, 0, 20);     
    mass_feedback[i] -> GetXaxis() -> SetTitle("Mass [GeV/c^2]");
    mass_feedback[i] -> SetLineColor(2);

  }


  // Bunch Directory
  Bunch->cd();
  sprintf(htitle,"%8.3f : Counts per Bunch ", runinfo.RUNID);
  bunch_dist = new TH1F("bunch_dist", htitle, NBUNCH, -0.5, NBUNCH-0.5);
  bunch_dist -> GetXaxis() -> SetTitle("Bunch ID");
  bunch_dist -> GetYaxis() -> SetTitle("Counts");
  bunch_dist -> SetFillColor(13);

  sprintf(htitle,"%8.3f : Wall Current Monitor", runinfo.RUNID);
  wall_current_monitor = new TH1F("wall_current_monitor", htitle, NBUNCH, -0.5, NBUNCH-0.5);
  wall_current_monitor -> GetXaxis() -> SetTitle("Bunch ID");
  wall_current_monitor -> GetYaxis() -> SetTitle("x10^9 protons");
  wall_current_monitor -> SetFillColor(13);

  sprintf(htitle,"%8.3f : Specific Luminosity", runinfo.RUNID);
  specific_luminosity = new TH1F("specific_luminosity", htitle, NBUNCH, -0.5, NBUNCH-0.5);
  specific_luminosity -> GetXaxis() -> SetTitle("Bunch ID");
  specific_luminosity -> GetYaxis() -> SetTitle("x10^9 protons");
  specific_luminosity -> SetFillColor(13);

  // Error detectors
  ErrDet->cd();
  sprintf(htitle,"%8.3f : Bunch Asymmetry X90", runinfo.RUNID);
  asym_bunch_x90 = new TH1F("asym_bunch_x90", htitle, 100, -0.1, 0.1);
  sprintf(htitle,"%8.3f : Bunch Asymmetry X45", runinfo.RUNID);
  asym_bunch_x45 = new TH1F("asym_bunch_x45", htitle, 100, -0.1, 0.1);
  sprintf(htitle,"%8.3f : Bunch Asymmetry Y45", runinfo.RUNID);
  asym_bunch_y45 = new TH1F("asym_bunch_y45", htitle, 100, -0.1, 0.1);
  sprintf(htitle,"%8.3f : # of Events in Banana Cut per strip", runinfo.RUNID);
  good_carbon_events_strip = new TH1I("good_carbon_events_strip", htitle, NSTRIP, 0.5, NSTRIP+0.5);
  good_carbon_events_strip->SetFillColor(17);

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
Root::RootHistBook2(datprocStruct dproc, StructRunConst runconst, StructFeedBack feedback){

  Kinema->cd();
  char formula[100],fname[100];
  float low, high, sqrte, sigma;
  int Color=2;
  int Width=2;

  for (int i=0; i<NSTRIP; i++) {

    for (int j=0; j<2; j++) {
      
      sigma = j ? runconst.M2T*feedback.RMS[i]*dproc.MassSigmaAlt : runconst.M2T*feedback.RMS[i]*dproc.MassSigma;
      int Style = j + 1 ; 

      // lower limit 
      sprintf(formula,"%f/sqrt(x)+(%f)/sqrt(x)", runconst.E2T, sigma);
      sprintf(fname,"banana_cut_l_st%d_mode%d",i,j);
      banana_cut_l[i][j] = new TF1(fname, formula, dproc.enel, dproc.eneu);
      banana_cut_l[i][j] -> SetLineColor(Color); 
      banana_cut_l[i][j] -> SetLineWidth(Width); 
      banana_cut_l[i][j] -> SetLineStyle(Style); 

      // upper limit 
      sprintf(formula,"%f/sqrt(x)-(%f)/sqrt(x)", runconst.E2T, sigma);
      sprintf(fname,"banana_cut_h_st%d",i);
      banana_cut_h[i][j] = new TF1(fname, formula, dproc.enel, dproc.eneu);
      banana_cut_h[i][j] -> SetLineColor(Color); 
      banana_cut_h[i][j] -> SetLineWidth(Width); 
      banana_cut_h[i][j] -> SetLineStyle(Style); 

    }

    // energy cut low
    low  = runconst.E2T/sqrt(double(dproc.enel))-runconst.M2T*feedback.RMS[i]*dproc.MassSigma/sqrt(double(dproc.enel));
    high = runconst.E2T/sqrt(double(dproc.enel))+runconst.M2T*feedback.RMS[i]*dproc.MassSigma/sqrt(double(dproc.enel));
    energy_cut_l[i] = new TLine(dproc.enel, low, dproc.enel, high);
    energy_cut_l[i] ->SetLineColor(Color);
    energy_cut_l[i] ->SetLineWidth(Width);

    // energy cut high
    low  = runconst.E2T/sqrt(double(dproc.eneu))-runconst.M2T*feedback.RMS[i]*dproc.MassSigma/sqrt(double(dproc.eneu));
    high = runconst.E2T/sqrt(double(dproc.eneu))+runconst.M2T*feedback.RMS[i]*dproc.MassSigma/sqrt(double(dproc.eneu));
    energy_cut_h[i] = new TLine(dproc.eneu, low, dproc.eneu, high);
    energy_cut_h[i] ->SetLineColor(Color);
    energy_cut_h[i] ->SetLineWidth(Width);

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
    t_vs_e_yescut[i] -> Delete();
    mass_vs_e_ecut[i] -> Delete();  // Mass vs. 12C Kinetic Energy 
    mass_nocut[i] -> Delete();
    //    mass_yescut[i] -> Delete();

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
      for (int j=0; j<2; j++){
	//	if (banana_cut_l[i]) t_vs_e[i] -> GetListOfFunctions() -> Add(banana_cut_l[i][j]);
	//	if (banana_cut_h[i]) t_vs_e[i] -> GetListOfFunctions() -> Add(banana_cut_h[i][j]);
      }
      if (energy_cut_l[i]) t_vs_e[i] -> GetListOfFunctions() -> Add(energy_cut_l[i]);
      if (energy_cut_h[i]) t_vs_e[i] -> GetListOfFunctions() -> Add(energy_cut_h[i]);
    }
  }

  // Write out memory before closing
  /*
  ErrDet->cd();
  if (mass_sigma_vs_strip)    mass_sigma_vs_strip      -> Write();
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


