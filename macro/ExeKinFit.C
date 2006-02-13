
void ExeKinFit(Char_t *run, Float_t bene, Int_t RHICBeam, Int_t HID)
{

  char MACRO[100];
  sprintf(MACRO,"%s/KinFit.C",gSystem->Getenv("MACRODIR"));
  gROOT->LoadMacro(MACRO);

  // open root file 
  Char_t filename[100];
  sprintf (filename, "douts/%s.root",run);
  TFile *f = TFile::Open(filename);
    
  cout << "RUN: "<<run<< " ENE: "<<bene<< " file: "<<filename<<endl; 
  KinFit *kfit = new KinFit(run, bene, RHICBeam, HID);
  kfit->Fit(0);    // arg = 0 ... new fit with two free paramters 
  kfit->Fit(1);    // fix dlayer values with average

  kfit->PlotResult();

    
  delete kfit;

}

