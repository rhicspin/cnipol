/**
 *
 * Nov 2, 2010 - Dmitri Smirnov
 *    - 
 *
 */

//#include "TFile.h"
//#include "TSystem.h"

//#include "KinFit.C"

//using namespace std;


void ExeKinFit(Char_t *run,  Float_t bene=0, Int_t RHICBeam=1, Float_t E2T=0,
   Float_t EMIN=400, Float_t EMAX=900, Int_t HID=15000, Char_t *cfile="",
   Char_t *online_cfile="", Char_t *outputdir="./", Int_t mode=1)
{
   char MACRO[100];
   sprintf(MACRO,"%s/KinFit.C", gSystem->Getenv("MACRODIR"));
   gROOT->LoadMacro(MACRO);
 
   // open root file 
   Char_t filename[100];
   sprintf (filename, "douts/%s.root", run);
   TFile *f = TFile::Open(filename);
 
   if (!f) { 
       cerr << "ERROR:" << filename << " not found" << endl;
       exit(-1);
   }
 
   KinFit *kfit = new KinFit(run, bene, RHICBeam, E2T, EMIN, EMAX, HID, cfile,
                             online_cfile, outputdir);
   kfit->Fit(0);    // arg = 0 ... new fit with two free paramters 
   kfit->Fit(mode);    
 
   kfit->PlotResult();
     
   delete kfit;
 
   return;
}
