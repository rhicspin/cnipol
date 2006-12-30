#ifndef __CINT__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <iostream.h>
#include "TString.h"
#include "TMath.h"
#include "rhicpol.h"
#include "rpoldata.h"
#include "Asym.h"
#include "AsymROOT.h"


#endif

/*
Int_t 
ColorSkime(TFile * rootfile, TCanvas *CurC, TPostScript * ps){

  const Int_t colNum = 20;
  Int_t palette[colNum];
  double red, green, blue;
  for (Int_t c = 0; c < colNum; c++) {
    red   =  0.9 * c / (colNum * 1.);
    green =  1. - 1.0 * c / (colNum * 1.);
    blue  =  1. - 0.2 * c / (colNum * 1.);
    cout << red << " " << blue << " " << green << " " << red+blue+green << endl;
    if (!gROOT -> GetColor (230 + c))
      TColor *color = new TColor (230 + c, red, green, blue);
    else {
      TColor *color = gROOT -> GetColor (230 + c);
      color -> SetRGB (red, green, blue);
    }
    palette[c] = 230 + c;
    gStyle -> SetPalette (colNum, palette);
  }

}
*/

Int_t 
PlotBanana(TFile * rootfile, TCanvas *CurC, TPostScript * ps){
  gStyle->SetOptLogz(0);  gStyle->SetOptStat(11);  gStyle->SetOptFit(0);

  gStyle -> SetPalette (55);
  ps->NewPage();
  rootfile->cd(); rootfile->cd("Kinema");  
  
  CurC->Divide(4,3);
  
  Char_t histname[100];
  Int_t stID;
  for (Int_t det=0; det<NDETECTOR; det++) {

    for (Int_t st=1; st<=NSTRIP_PER_DETECTOR; st++){
      stID=det*NSTRIP_PER_DETECTOR+st;
      CurC->cd(st);
      sprintf(histname,"t_vs_e_st%d",stID);
      gDirectory->Get(histname) -> Draw("colz");  CurC->Update();    

    }// end-of-strip loop

    if (det!=NDETECTOR-1) ps->NewPage();

  }


  return 0;
}

Int_t 
PlotMassEnergyCorrelation(TFile * rootfile, TCanvas *CurC, TPostScript * ps){
  gStyle->SetOptLogz(0);  gStyle->SetOptStat(11);  gStyle->SetOptFit(0);

  gStyle -> SetPalette (55);
  ps->NewPage();
  rootfile->cd(); rootfile->cd("Kinema");  
  
  CurC->Divide(4,3);
  
  Char_t histname[100];
  Int_t stID;
  for (Int_t det=0; det<NDETECTOR; det++) {

    for (Int_t st=1; st<=NSTRIP_PER_DETECTOR; st++){
      stID=det*NSTRIP_PER_DETECTOR+st;
      CurC->cd(st); 
      sprintf(histname,"mass_vs_e_ecut_st%d",stID);
      gDirectory->Get(histname) -> Draw("contz");  CurC->Update();    

      sprintf(histname,"mass_vs_energy_corr_st%d",stID);
      if (gDirectory->Get(histname)) gDirectory->Get(histname) -> Draw("same");  CurC->Update();    

    }// end-of-strip loop

    if (det!=NDETECTOR-1) ps->NewPage();

  }


  return 0;
}

Int_t 
PlotInvariantMass(TFile * rootfile, TCanvas *CurC, TPostScript * ps){
  gStyle->SetOptLogz(0);  gStyle->SetOptStat(11);  gStyle->SetOptFit(0);

  gStyle -> SetPalette (55);
  ps->NewPage();
  rootfile->cd(); rootfile->cd("Kinema");  
  
  CurC->Divide(4,3);
  
  Char_t histname[100];
  Int_t stID;
  for (Int_t det=0; det<NDETECTOR; det++) {

    for (Int_t st=1; st<=NSTRIP_PER_DETECTOR; st++){
      stID=det*NSTRIP_PER_DETECTOR+st;
      CurC->cd(st); 
      sprintf(histname,"mass_nocut_st%d",stID);
      gDirectory->Get(histname) -> Draw("");  CurC->Update();    

    }// end-of-strip loop

    if (det!=NDETECTOR-1) ps->NewPage();

  }


  return 0;
}


Int_t 
PlotStrip(TFile * rootfile, TCanvas *CurC, TPostScript * ps){

  PlotBanana(rootfile, CurC, ps);   // Plot Individual Strip
  PlotMassEnergyCorrelation(rootfile, CurC, ps);   // Plot Individual Strip
  PlotInvariantMass(rootfile, CurC, ps);   // Plot Individual Strip

  return 0;

}

Int_t 
PlotErrorDetector(TFile * rootfile, TCanvas *CurC, TPostScript * ps){

  CurC -> SetGridx();

  // One Page Summary for Strip
  ps->NewPage();
  CurC->Divide(2,2); 
  rootfile->cd(); rootfile->cd("ErrDet");  

  TH2F * mass_e_correlation_strip = (TH2F*)gDirectory->Get("mass_e_correlation_strip");
  TH2F * mass_sigma_vs_strip = (TH2F*)gDirectory->Get("mass_sigma_vs_strip");
  TH1F * good_carbon_events_strip = (TH1F*)gDirectory->Get("good_carbon_events_strip");
  TH2F * mass_pos_dev_vs_strip = (TH2F*)gDirectory->Get("mass_pos_dev_vs_strip");

  CurC->cd(1) ; mass_e_correlation_strip -> Draw(); 
  CurC->cd(2) ; mass_sigma_vs_strip -> Draw(); 
  CurC->cd(3) ; good_carbon_events_strip -> Draw(); 
  CurC->cd(4) ; mass_pos_dev_vs_strip -> Draw(); CurC->Update(); 
  ps->NewPage();

  
  /*
  // one page summary for bunch
  rootfile->cd("Asymmetry");
  CurC->cd(1) ; asym_vs_bunch_x90 -> Draw(); 
  CurC->cd(2) ; asym_vs_bunch_x45 -> Draw(); 
  CurC->cd(3) ; asym_vs_bunch_y45 -> Draw(); 
  rootfile->cd(); rootfile->cd("ErrDet");
  CurC->cd(4) ; spelumi_vs_bunch -> Draw(); CurC->Update(); 
  CurC->Clear();

  // bunch detailes 
  rootfile->cd("Asymmetry");
  gStyle->SetStatStyle(0);
  asym_vs_bunch_x90 -> Draw(); CurC -> Update(); ps->NewPage(); 
  asym_vs_bunch_x45 -> Draw(); CurC -> Update(); ps->NewPage();
  asym_vs_bunch_y45 -> Draw(); CurC -> Update(); ps->NewPage();

  rootfile->cd(); rootfile->cd("ErrDet");
  gStyle->SetOptFit(111);
  asym_bunch_x90   -> Draw(); CurC -> Update(); ps->NewPage();
  asym_bunch_x45   -> Draw(); CurC -> Update(); ps->NewPage();
  asym_bunch_y45   -> Draw(); CurC -> Update(); ps->NewPage();
  spelumi_vs_bunch -> Draw(); CurC -> Update(); ps->NewPage();

  rootfile->cd(); rootfile->cd("Bunch");
  if (gDirectory(bunch_spelumi))       bunch_spelumi  -> Draw(); CurC -> Update(); ps->NewPage();
  if (gDirecotry(bunch_dist))          bunch_dist     -> Draw(); CurC -> Update(); ps->NewPage();
  if (gDirectory(wall_current_monitor))wall_current_monitor -> Draw(); CurC -> Update(); ps->NewPage();

  // strip details
  rootfile->cd(); rootfile->cd("ErrDet");
  mass_e_correlation_strip -> Draw(); CurC->Update(); 
  // mass_sigma_vs_strip -> Draw(); CurC->Update(); 
  mass_chi2_vs_strip -> Draw(); CurC->Update(); 
  good_carbon_events_strip -> Draw(); CurC->Update(); 
  mass_pos_dev_vs_strip -> Draw(); CurC->Update(); 

  // energy slope
  rootfile->cd(); rootfile->cd("Kinema");
  energy_spectrum_all->Draw(); CurC->Update();
  */


  return 0;

}


#ifndef __CINT__
int main(int argc, char **argv) {

  int opt;
  while (EOF != (opt = getopt(argc, argv, "w:e:ishx?"))) {
    switch (opt) {
    case 'h':
    case '?':
    case '*':
      cout << " help" << endl;
      break;
    }
  }

#else
  Int_t AsymErrorDetector() {
#endif

  // load header file
  Char_t HEADER[100];
  sprintf(HEADER,"%s/AsymHeader.h",gSystem->Getenv("MACRODIR"));
  gROOT->LoadMacro(HEADER);

  Char_t RunID[10]="6931.003";
  Char_t filename[50], text[100];
  sprintf(filename,"%s.root",RunID);
  
  // some tricks to open rootfile in a subdirectory
  sprintf(text,"ln -s root/%s %s",filename,filename);
  gSystem->Exec(text);

  // Root file open.
  cout << filename << endl;
  TFile * rootfile = TFile::Open(filename);
  gDirectory->ls();

  // postscript file
  Char_t psfile[100];
  sprintf(psfile,"ps/AsymErrorDetctor_%s.ps",RunID);
  TPostScript *ps = new TPostScript(psfile,112);

  // Cambus Setup
  TCanvas *CurC = new TCanvas("CurC","",1);

  PlotErrorDetector(rootfile, CurC, ps);   // Plot Error Detector
  //  PlotStrip(rootfile, CurC, ps);   // Plot Individual Strip

  // remove link
  sprintf(text,"rm -f %s",filename);
  gSystem->Exec(text);

  cout << "ps file : " << psfile << endl;
  ps->Close();

  sprintf(text,"gv -landscape %s",psfile);
  gSystem->Exec(text);
  //  gSystem->Exec("gv ps/AsymErrorDetctor.ps");


  return 0;

}
  
