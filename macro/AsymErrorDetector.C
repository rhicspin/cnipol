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
#include "ROOT.h"

#endif

#ifndef __CINT__
int main(int argc, char **argv) 
#else
  Int_t AsymErrorDetector()
#endif
{

  Char_t RunID[10]="7327.003";
  Char_t filename[50], text[100];
  sprintf(filename,"%s.root",RunID);
  
  // some tricks
  sprintf(text,"ln -s root/%s %s",filename,filename);
  gSystem->Exec(text);

  // Root file open
  cout << filename << endl;
  TFile * rootfile = TFile::Open(filename);


  // postscript file
  Char_t psfile[100];
  sprintf(psfile,"ps/AsymErrorDetctor_%s.ps",RunID);
  TPostScript *ps = new TPostScript(psfile,112);

  gDirectory->ls();

  // Cambus Setup
  TCanvas *CurC = new TCanvas("CurC","",1);
  CurC -> SetGridx();

  // detailes 
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
  bunch_spelumi    -> Draw(); CurC -> Update(); ps->NewPage();
  bunch_dist     -> Draw(); CurC -> Update(); ps->NewPage();
  wall_current_monitor -> Draw(); CurC -> Update(); CurC->Clear();

  // one page summary plot 
  CurC->Divide(2,2); 
  rootfile->cd(); rootfile->cd("Asymmetry");
  CurC->cd(1) ; asym_vs_bunch_x90 -> Draw(); CurC->Update(); 
  CurC->cd(2) ; asym_vs_bunch_x45 -> Draw(); CurC->Update(); 
  CurC->cd(3) ; asym_vs_bunch_y45 -> Draw(); CurC->Update(); 
  rootfile->cd(); rootfile->cd("ErrDet");
  CurC->cd(4) ; spelumi_vs_bunch -> Draw(); CurC->Update(); 
  

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
  
