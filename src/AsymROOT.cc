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


// Histogram Definitions
TH2F * t_vs_e[NSTRIP];          // t vs. 12C Kinetic Energy (banana with/o cut)
TH2F * mass_vs_e_ecut[NSTRIP];  // Mass vs. 12C Kinetic Energy 
TH2F * mass_vs_t_ecut[NSTRIP];  // Mass vs. ToF (w/ Energy Cut)
TH2F * mass_vs_t[NSTRIP];       // Mass vs. ToF (w/o Energy Cut)



//
// Class name  : Root
// Method name : RootFile(char * filename)
//
// Description : Open Root File
//             : 
// Input       : char *filename
// Return      : 
//
int 
Root::RootFile(char *filename){

  rootfile = new TFile(filename,"RECREATE","ROOT Histogram file");

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

  Char_t histname[100], histtitle[100];
  for (int i=0; i<NSTRIP; i++) {

    sprintf(histname,"t_vs_e_st%d",i);
    sprintf(histtitle,"%8.3f : t vs. Kin.Energy Str%d ",runinfo.RUNID, i);
    t_vs_e[i] = new TH2F(histname,histtitle, 50, 200, 1500, 100, 20, 90);

    sprintf(histname,"mass_vs_e_ecut_st%d",i);
    sprintf(histtitle,"%8.3f : Mass vs. Kin.Energy (Energy Cut) Str%d ",runinfo.RUNID, i);
    mass_vs_e_ecut[i] = new TH2F(histname,histtitle, 50, 200, 1000, 200, 6, 18);

    sprintf(histname,"mass_vs_t_ecut_st%d",i);
    sprintf(histtitle,"%8.3f : Mass vs. ToF (Energy Cut) Str%d ", runinfo.RUNID, i);
    mass_vs_t_ecut[i] = new TH2F(histname,histtitle, 100, 10, 90, 100, 5, 25);

    sprintf(histname,"mass_vs_t_st%d",i);
    sprintf(histtitle,"%8.3f : Mass vs. ToF Str%d", runinfo.RUNID, i);
    mass_vs_t[i] = new TH2F(histname,histtitle, 100, 10, 90, 100, 5, 25);

  }


  return 0;

}




//
// Class name  : Root
// Method name : RootFile(char * filename)
//
// Description : Close Root File
//             : 
// Input       : 
// Return      : 
//
int 
Root::CloseROOTFile(){

  rootfile->Write();
  rootfile->Close();

  return 0;

}


