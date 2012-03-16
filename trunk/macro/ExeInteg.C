void ExeInteg(Char_t *run)
{

  char MACRO[100];
  sprintf(MACRO,"%s/Integ.C",gSystem->Getenv("MACRODIR"));
  gROOT->LoadMacro(MACRO);
    
  // select root file 
  Char_t filename[100];
  sprintf (filename, "./douts/%s.root",run);
  TFile *f = TFile::Open(filename);
    
  cout << "RUN: "<<run << " file: "<<filename<<endl; 
    
  KinFit *kfit = new KinFit(run);
  kfit->Fit(); 
  kfit->PlotResult();


    
  delete kfit;
}

