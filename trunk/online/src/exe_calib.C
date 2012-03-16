void exe_calib(Char_t *run)
{
    
    gROOT->LoadMacro("calibfit.C");
    //gROOT->LoadMacro("calibfitwithBG.C");
    
    // select root file 
    Char_t filename[100];
    sprintf (filename, "%s.root",run);
    TFile *f = TFile::Open(filename);
    
    cout << "RUN: "<<run<< " file: "<<filename<<endl; 
    
    calibfit *cfit = new calibfit(run);
    cfit->Fit();    
    cfit->PlotResult();

    delete cfit;
}

