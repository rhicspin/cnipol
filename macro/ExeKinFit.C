
void
ExeKinFit(Char_t *run,  Float_t bene, Int_t RHICBeam, Float_t E2T, Float_t EMIN, 
	  Float_t EMAX, Int_t HID, Char_t *cfile, Char_t *online_cfile, Char_t *outputdir) {

  char MACRO[100];
  sprintf(MACRO,"%s/KinFit.C",gSystem->Getenv("MACRODIR"));
  gROOT->LoadMacro(MACRO);


  // open root file 
  Char_t filename[100];
  sprintf (filename, "douts/%s.root",run);
  TFile *f = TFile::Open(filename);
  if (!f) { 
      cerr << "ERROR:" << filename << " not found" << endl;
      exit(-1);
  }

  KinFit *kfit = new KinFit(run, bene, RHICBeam, E2T, EMIN, EMAX, HID, cfile, online_cfile, outputdir);
  kfit->Fit(0);    // arg = 0 ... new fit with two free paramters 
  kfit->Fit(1);    // fix dlayer values with average

  kfit->PlotResult();

    
  delete kfit;

  return ;

}

