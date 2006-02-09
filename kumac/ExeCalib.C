void ExeCalib(Char_t *run)
{
  char MACRO[100];
  sprintf(MACRO,"%s/CalibFit.C",gSystem->Getenv("MACRODIR"));
  gROOT->LoadMacro(MACRO);

    // select root file 
    Char_t filename[100];
    sprintf (filename, "./douts/%s.calib.root",run);
    TFile *f = TFile::Open(filename);
    
    cout << "RUN: "<<run<< " file: "<<filename<<endl; 

    CalibFit *cfit = new CalibFit(run);
    cfit->Fit();    
    cfit->PlotResult();

    delete cfit;
}

