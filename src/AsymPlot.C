#ifndef __CINT__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>
#include <getopt.h>
#include "TString.h"
#include "TMath.h"
#include "rhicpol.h"
#include "rpoldata.h"
#include "Asym.h"
#include "AsymROOT.h"


#endif

static Int_t GHOSTVIEW=0;
static Int_t FEEDBACK=0;
static Int_t PLOT_RAW=0;
static Int_t PLOT_BANANA=1;
static Int_t ERROR_DETECTOR=1;
static Int_t SUMMARY=1;
static Int_t stID=0;
static char * RUNID;
static Char_t lnkfile[50]="AsymPlot.root";

extern  ofstream logfile;

void FindRootFile();
Int_t FileNotOK(Char_t file[]){
  ifstream f;
  f.open(file);
  return f.fail();
}

class AsymPlot
{
private:
  
public:
  Int_t GetHistograms(TFile * rootfile);
  Int_t PlotFeedback(TFile * rootfile, TCanvas *CurC, TPostScript * ps);
  Int_t PlotRaw(TFile * rootfile, TCanvas *CurC, TPostScript * ps);
  Int_t PlotErrorDetector(TFile * rootfile, TCanvas *CurC, TPostScript * ps);
  Int_t PlotErrorDetectorSummary(TFile * rootfile, TCanvas *CurC, TPostScript * ps);
  // Directory "FeedBack"
  TH2F * mdev_feedback;  
  // Raw Directory
  TH1F * bunch_dist_raw;              // counts per bunch (raw)
  TH1F * strip_dist_raw;              // counts per strip (raw)
  TH1F * tdc_raw;                     // tdc (raw)
  TH1F * adc_raw;                     // adc (raw)
  TH2F * tdc_vs_adc_raw;              // tdc vs. adc (raw)
  // Directory "ErrDet"
  TH2F * mass_e_correlation_strip;
  TH2F * mass_chi2_vs_strip;
  TH1I * good_carbon_events_strip;
  TH2F * mass_pos_dev_vs_strip;
  TH2F * spelumi_vs_bunch;
  TH1F * bunch_spelumi;
  TH1F * asym_bunch_x90;
  TH1F * asym_bunch_x45;
  TH1F * asym_bunch_y45;
  // Directory "Asymmetry"
  TH2F * asym_vs_bunch_x90;
  TH2F * asym_vs_bunch_x45;
  TH2F * asym_vs_bunch_y45;
  TH2F * asym_sinphi_fit;
  // Directory "Bunch"
  TH1F * bunch_dist;
  TH1F * wall_current_monitor;
  TH1F * specific_luminosity;
  // Directory "Kinema"
  TH1F * energy_spectrum_all;

};


Int_t
Usage(char *argv[]){

  cout << "\n Usage:" << argv[0] << "[-hxg][-f <runID>][--banana][--error-detector]" << endl;
  cout << "\n Description: " << endl;
  cout << "\t Make plots for Run <runID>." << endl;
  cout << "\n Options:" << endl;
  cout << "\t -f <runID> " << endl;
  cout << "\t --error-detector  plot error detector histograms" << endl;
  cout << "\t --feedback        plot feedback histograms" << endl;
  cout << "\t --raw             plot raw histograms" << endl;
  cout << "\t --banana          plot kinematics reconstruction for all strips" << endl;
  cout << "\t --summary         plot summary" << endl;
  cout << "\t --strip <stID>    plot kinematics reconstruction for strip <stID>" << endl;
  cout << "\t -g \t launch ghostview" << endl;
  cout << "\t -h \t show this help    " << endl;
  cout << "\t -x \t show example    " << endl;
  cout << endl;
  exit(0);

}

Int_t
Example(char *argv[]){

  cout << "\n Exapmle: " << endl;
  cout << "\t" << argv[0] << " -f 7279.005 -g" << endl;
  cout << 
  cout << endl;
  exit(0);

}




void 
ColorSkime(){

    const Int_t NRGBs = 5;
    const Int_t NCont = 255;

    Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
    //    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);

}



Int_t GetHistograms(TFile * rootfile);


// =======================================================================================
//                                    Histogram Checker 
// =======================================================================================
Int_t IsOK(Char_t histname[]){
  ofstream logfile;
  Int_t OK = Int_t(gDirectory->Get(histname));
  if (!OK) logfile << "The histogram : " << histname << " doesn't exist in root file. Ignored." << endl;
  return OK;
}


Int_t 
PlotStrip(TFile * rootfile, TCanvas *CurC, TPostScript * ps, Int_t stID){
  cout << "PlotStrip stID=" << stID << endl;

  gStyle->SetOptLogz(0);  gStyle->SetOptStat(11);  gStyle->SetOptFit(0);
  gStyle -> SetPalette (1);

  ps->NewPage();
  rootfile->cd(); rootfile->cd("Kinema");  

  Char_t histname[100];
  // banana
  sprintf(histname,"t_vs_e_st%d",stID);
  if (IsOK(histname)) gDirectory->Get(histname) -> Draw("colz");  CurC->Update();  ps->NewPage();

  // mass_vs_e correlation
  sprintf(histname,"mass_vs_e_ecut_st%d",stID);
  if (IsOK(histname)) gDirectory->Get(histname) -> Draw("contz"); 
  sprintf(histname,"mass_vs_energy_corr_st%d",stID);
  if (IsOK(histname)) gDirectory->Get(histname) -> Draw("same");  CurC->Update();    
  ps->NewPage();

  // invariant mass
  sprintf(histname,"mass_nocut_st%d",stID);
  if (IsOK(histname)) gDirectory->Get(histname) -> Draw("");  
  sprintf(histname,"mass_yescut_st%d",stID);
  if (IsOK(histname)) gDirectory->Get(histname) -> Draw("same");    CurC->Update();    

  return 0;

}


Int_t 
PlotBanana(TFile * rootfile, TCanvas *CurC, TPostScript * ps){
  gStyle->SetOptLogz(0);  gStyle->SetOptStat(11);  gStyle->SetOptFit(0);
  gStyle -> SetPalette (1);

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
      if (IsOK(histname)) gDirectory->Get(histname) -> Draw("colz");  CurC->Update();    

    }// end-of-strip loop

    if (det!=NDETECTOR-1) ps->NewPage();

  }


  return 0;
}

Int_t 
PlotMassEnergyCorrelation(TFile * rootfile, TCanvas *CurC, TPostScript * ps){
  gStyle->SetOptLogz(0);  gStyle->SetOptStat(11);  gStyle->SetOptFit(0);

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
      if (IsOK(histname)) gDirectory->Get(histname) -> Draw("contz");  CurC->Update();    

      sprintf(histname,"mass_vs_energy_corr_st%d",stID);
      if (IsOK(histname)) gDirectory->Get(histname) -> Draw("same");  CurC->Update();    

    }// end-of-strip loop

    if (det!=NDETECTOR-1) ps->NewPage();

  }


  return 0;
}

Int_t 
PlotInvariantMass(TFile * rootfile, TCanvas *CurC, TPostScript * ps){
  gStyle->SetOptLogz(0);  gStyle->SetOptStat(11);  gStyle->SetOptFit(0);

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
      if (IsOK(histname)) gDirectory->Get(histname) -> Draw(""); 
      sprintf(histname,"mass_yescut_st%d",stID);
      if (IsOK(histname)) gDirectory->Get(histname) -> Draw("same");  
      CurC->Update();    

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
PlotFeedbackStrip(TFile * rootfile, TCanvas *CurC, TPostScript * ps){
  gStyle->SetOptLogz(0);  gStyle->SetOptStat(11);  gStyle->SetOptFit(0);

  ps->NewPage();
  rootfile->cd(); rootfile->cd("FeedBack");  
  
  CurC->Divide(4,3);
  
  Char_t histname[100];
  Int_t stID;
  for (Int_t det=0; det<NDETECTOR; det++) {

    for (Int_t st=1; st<=NSTRIP_PER_DETECTOR; st++){
      stID=det*NSTRIP_PER_DETECTOR+st;
      CurC->cd(st); 
      sprintf(histname,"mass_feedback_st%d",stID);
      if (IsOK(histname)) gDirectory->Get(histname) -> Draw(""); 
      CurC->Update();    

    }// end-of-strip loop

    if (det!=NDETECTOR-1) ps->NewPage();

  }


  return 0;
}


Int_t
AsymPlot::PlotFeedback(TFile * rootfile, TCanvas *CurC, TPostScript * ps){

  // following two lines should go to GetHistograms routine once all run05 data are replayed
  rootfile->cd(); rootfile->cd("FeedBack");  
  mdev_feedback              = (TH2F*)gDirectory->Get("mdev_feedback");

  //  ps->NewPage(); CurC->Clear(); CurC->Divide(1,1); 
  rootfile->cd(); rootfile->cd("Kinema");
  mdev_feedback->Draw(); CurC->Update(); ps->NewPage(); CurC->Clear();

  PlotFeedbackStrip(rootfile, CurC, ps);


  // energy slope

  return 0;

}

Int_t
AsymPlot::PlotRaw(TFile * rootfile, TCanvas *CurC, TPostScript * ps){


  rootfile->cd(); rootfile->cd("Raw");  
  bunch_dist_raw              = (TH1F*)gDirectory->Get("bunch_dist_raw");
  strip_dist_raw              = (TH1F*)gDirectory->Get("strip_dist_raw");
  tdc_raw                     = (TH1F*)gDirectory->Get("tdc_raw");
  adc_raw                     = (TH1F*)gDirectory->Get("adc_raw");
  tdc_vs_adc_raw              = (TH2F*)gDirectory->Get("tdc_vs_adc_raw");
  rootfile->cd(); rootfile->cd("Bunch");  
  bunch_dist                 = (TH1F*)gDirectory->Get("bunch_dist");

  rootfile->cd(); rootfile->cd("Kinema");
  gStyle->SetOptLogz(1);
  bunch_dist_raw->Draw(); 
  bunch_dist->Draw("same"); CurC->Update(); ps->NewPage(); 
  gStyle->SetOptLogz(0);
  strip_dist_raw->Draw(); CurC->Update(); ps->NewPage(); 
  tdc_raw->Draw(); CurC->Update(); ps->NewPage(); 
  adc_raw->Draw(); CurC->Update(); ps->NewPage(); 
  tdc_vs_adc_raw->Draw(); CurC->Update(); ps->NewPage(); 

  CurC->Clear(); CurC->Divide(2,2); 
  CurC->cd(1) ; bunch_dist_raw->Draw(); bunch_dist->Draw("same");
  CurC->cd(2) ; strip_dist_raw->Draw();
  CurC->cd(3) ; tdc_raw->Draw();
  CurC->cd(4) ; adc_raw -> Draw(); CurC->Update(); 

  gStyle -> SetPalette (1);  ColorSkime();
  ps->NewPage();
  CurC->cd(1) ; tdc_vs_adc_raw->Draw(); 
  CurC->cd(2) ; tdc_vs_adc_raw->Draw("colz"); 
  CurC->cd(3) ; tdc_vs_adc_raw->Draw("lego"); CurC->Update(); 

  return 0;

}


Int_t 
AsymPlot::PlotErrorDetectorSummary(TFile * rootfile, TCanvas *CurC, TPostScript * ps){

  CurC -> SetGridx();

  ps->NewPage();
  CurC->Clear(); CurC->Divide(2,2); 
  rootfile->cd(); rootfile->cd("ErrDet");  
  CurC->cd(1) ; if (gDirectory->Get("mass_e_correlation_strip")!=0) mass_e_correlation_strip -> Draw(); 
  CurC->cd(2) ; if (gDirectory->Get("mass_sigma_vs_strip")!=0) mass_sigma_vs_strip -> Draw(); 
  CurC->cd(3) ; if (gDirectory->Get("good_carbon_events_strip")!=0) good_carbon_events_strip -> Draw(); 
  CurC->cd(4) ; if (gDirectory->Get("mass_pos_dev_vs_strip")!=0) mass_pos_dev_vs_strip -> Draw(); 
  CurC->Update();   ps->NewPage();
  
  rootfile->cd("Asymmetry");
  CurC->cd(1) ; if (gDirectory->Get("asym_vs_bunch_x90")!=0) asym_vs_bunch_x90 -> Draw(); 
  CurC->cd(2) ; if (gDirectory->Get("asym_vs_bunch_x45")!=0) asym_vs_bunch_x45 -> Draw(); 
  CurC->cd(3) ; if (gDirectory->Get("asym_vs_bunch_y45")!=0) asym_vs_bunch_y45 -> Draw(); 
  rootfile->cd(); rootfile->cd("ErrDet");
  CurC->cd(4) ; if (gDirectory->Get("spelumi_vs_bunch")!=0) spelumi_vs_bunch -> Draw(); 
  CurC->Update(); 

  return 0;


}


Int_t 
AsymPlot::PlotErrorDetector(TFile * rootfile, TCanvas *CurC, TPostScript * ps){

  CurC -> SetGridx();   gStyle -> SetPalette (1);
  ps->NewPage(); CurC->Clear(); CurC->Divide(1,1); 

  // energy slope
  rootfile->cd(); rootfile->cd("Kinema");
  if (gDirectory->Get("energy_spectrum_all")!=0) {energy_spectrum_all->Draw(); CurC->Update();}

  // bunch detailes 
  rootfile->cd("Asymmetry");
  gStyle->SetStatStyle(0);
  if (gDirectory->Get("asym_vs_bunch_x90")!=0) {asym_vs_bunch_x90 -> Draw(); CurC -> Update(); ps->NewPage();}
  if (gDirectory->Get("asym_vs_bunch_x45")!=0) {asym_vs_bunch_x45 -> Draw(); CurC -> Update(); ps->NewPage();}
  if (gDirectory->Get("asym_vs_bunch_y45")!=0) {asym_vs_bunch_y45 -> Draw(); CurC -> Update(); ps->NewPage();}

  rootfile->cd(); rootfile->cd("ErrDet");
  gStyle->SetOptFit(111);
  if (gDirectory->Get("asym_bunch_x90")!=0) {asym_bunch_x90   -> Draw(); CurC -> Update(); ps->NewPage();}
  if (gDirectory->Get("asym_bunch_x45")!=0) {asym_bunch_x45   -> Draw(); CurC -> Update(); ps->NewPage();}
  if (gDirectory->Get("asym_bunch_y45")!=0) {asym_bunch_y45   -> Draw(); CurC -> Update(); ps->NewPage();}
  if (gDirectory->Get("spelumi_vs_bunch")!=0){spelumi_vs_bunch -> Draw(); CurC -> Update(); ps->NewPage();}

  rootfile->cd(); rootfile->cd("Bunch");
  if (IsOK("bunch_spelumi")) bunch_spelumi -> Draw(); CurC -> Update(); ps->NewPage();
  if (IsOK("bunch_dist"))    bunch_dist    -> Draw(); CurC -> Update(); ps->NewPage();
  if (IsOK("wall_current_monitor")) wall_current_monitor -> Draw(); CurC -> Update(); ps->NewPage();

  // strip details
  rootfile->cd(); rootfile->cd("ErrDet");
  if (IsOK("mass_e_correlation_strip")) mass_e_correlation_strip -> Draw(); CurC->Update(); 
  if (IsOK("mass_sigma_vs_strip"))      mass_sigma_vs_strip      -> Draw(); CurC->Update(); 
  if (IsOK("mass_chi2_vs_strip"))       mass_chi2_vs_strip       -> Draw(); CurC->Update(); 
  if (IsOK("good_carbon_events_strip")) good_carbon_events_strip -> Draw(); CurC->Update(); 
  if (IsOK("mass_pos_dev_vs_strip"))    mass_pos_dev_vs_strip    -> Draw(); CurC->Update(); 

  // sin(phi) fit
  rootfile->cd(); rootfile->cd("Asymmetry");
  gStyle->SetOptFit(111); gStyle->SetOptStat(0);
  if (IsOK("asym_sinphi_fit")) asym_sinphi_fit->Draw(); CurC->Update();

  return 0;

}



//
// Class name  : AsymPlot
// Method name : GetHistograms(TFile * rootfile)
//
// Description : Get histogram pointers from rootfile
// Input       : TFile * rootfile
// Return      : 
//
Int_t 
AsymPlot::GetHistograms(TFile * rootfile){

  rootfile->cd(); rootfile->cd("ErrDet");  
  mass_e_correlation_strip   = (TH2F*)gDirectory->Get("mass_e_correlation_strip");
  mass_sigma_vs_strip        = (TH2F*)gDirectory->Get("mass_sigma_vs_strip");
  mass_chi2_vs_strip         = (TH2F*)gDirectory->Get("mass_chi2_vs_strip");
  good_carbon_events_strip   = (TH1I*)gDirectory->Get("good_carbon_events_strip");
  mass_pos_dev_vs_strip      = (TH2F*)gDirectory->Get("mass_pos_dev_vs_strip");
  spelumi_vs_bunch           = (TH2F*)gDirectory->Get("spelumi_vs_bunch");
  bunch_spelumi              = (TH1F*)gDirectory->Get("bunch_spelumi");
  asym_bunch_x90             = (TH1F*)gDirectory->Get("asym_bunch_x90");
  asym_bunch_x45             = (TH1F*)gDirectory->Get("asym_bunch_x45");
  asym_bunch_y45             = (TH1F*)gDirectory->Get("asym_bunch_y45");

  rootfile->cd(); rootfile->cd("Asymmetry");
  asym_vs_bunch_x90          = (TH2F*)gDirectory->Get("asym_vs_bunch_x90");
  asym_vs_bunch_x45          = (TH2F*)gDirectory->Get("asym_vs_bunch_x45");
  asym_vs_bunch_y45          = (TH2F*)gDirectory->Get("asym_vs_bunch_y45");
  asym_sinphi_fit            = (TH2F*)gDirectory->Get("asym_sinphi_fit");

  rootfile->cd(); rootfile->cd("Bunch");
  bunch_dist                 = (TH1F*)gDirectory->Get("bunch_dist");
  wall_current_monitor       = (TH1F*)gDirectory->Get("wall_current_monitor");
  specific_luminosity        = (TH1F*)gDirectory->Get("specific_luminosity");

  // energy slope
  rootfile->cd(); rootfile->cd("Kinema");
  energy_spectrum_all        = (TH1F*)gDirectory->Get("energy_spectrum_all");


  return 0;

}



//
// Class name  : 
// Method name : FindRootFile(TFile * rootfile)
//
// Description : Search for root file in ./root directory. If the root file is not
//             : found in ./root direcotry, then search for current working directory. 
//             : The root file is accessed via symbolic link to the data file. If root
//             : file is found in pwd, then the file is moved to root directry then
//             : symbolic linked.
// Input       : 
// Return      : 
//
void 
FindRootFile(){

  // rootfile operation
  Char_t filename[50], text[100];

  // remove existing ./AsymPlot.root 
  sprintf(text,"rm -f %s", lnkfile); gSystem->Exec(text); 
  if (!RUNID) RUNID="7279.005"; // default

  // First search for ./root directory 
  sprintf(filename,"root/%s.root",RUNID);
  if (FileNotOK(filename)) {
    cout<< filename << " is not found. Search for pwd." << endl;
    sprintf(filename,"%s.root",RUNID); // then look for pwd
    if (FileNotOK(filename)) { 
      cerr << "./" << filename << " is not found neither. Force exit." << endl;
      exit(-1);
    } else {
      sprintf(text,"mv -f %s root/.", filename); gSystem->Exec(text);
      sprintf(filename,"root/%s.root",RUNID);
    }
  }
	
  // make symbolic link from root/RUNID.root to ./AsymPlot.root
  sprintf(text,"ln -s %s %s",filename,lnkfile);
  gSystem->Exec(text);

  return;

 }


#ifndef __CINT__
int main(int argc, char **argv) {

  int opt, option_index = 0;
  static struct option long_options[] = {
    {"strip", 1, 0, 's'},
    {"banana", 0, 0, 'b'},
    {"feedback", 0, 0, 'F'},
    {"raw", 0, 0, 'r'},
    {"error-detector", 0, 0, 'e'},
    {"summary", 0, 0, 'S'},
    {"create", 1, 0, 'c'},
    {"file", 1, 0, 0},
    {0, 0, 0, 0}
  };

  while (EOF != (opt = getopt_long (argc, argv, "gerSFh?xf:s:b:", long_options, &option_index))) {
    switch (opt) {
    case -1:
      break;
    case 'b':
      PLOT_BANANA=1;
      ERROR_DETECTOR=0;
      SUMMARY=0;
      break;
    case 'r':
      PLOT_BANANA=0;
      ERROR_DETECTOR=0;
      SUMMARY=0;
      PLOT_RAW=1;
      break;
    case 'e':
      ERROR_DETECTOR=1;
      PLOT_BANANA=0;
      break;
    case 'g':
      GHOSTVIEW=1;
      break;
    case 'f':
      RUNID=optarg;
      break;
    case 'F':
      FEEDBACK=1;
      SUMMARY=0;
      ERROR_DETECTOR=0;
      PLOT_BANANA=0;
      break;
    case 'S':
      SUMMARY=1;
      ERROR_DETECTOR=0;
      PLOT_BANANA=0;
      break;
    case 's':
      stID=atoi(optarg);
      break;
    case 'h':
    case '?':
    case '*':
      Usage(argv);
      break;
    }
  }



#else
int AsymPlot() {
#endif

  // load header file
  Char_t HEADER[100], text[100], filename[100];
  sprintf(HEADER,"%s/AsymHeader.h",gSystem->Getenv("MACRODIR"));
  gROOT->LoadMacro(HEADER);


  // open rootfile 
  FindRootFile();
  TFile * rootfile = TFile::Open(lnkfile);


  // Log file handling
  ofstream logfile;
  logfile.open(".AsymPlot.log");
  if (logfile.fail()){ cerr << "Warning: Cannot open .AsymPlot.log." << endl;}


  // setup color skime
  //  ColorSkime();

  // Cambus Setup
  TCanvas *CurC = new TCanvas("CurC","",1);

  // postscript file
  Char_t psfile[100];
  sprintf(psfile,"ps/AsymPlot_%s.ps",RUNID);
  TPostScript *ps = new TPostScript(psfile,112);

  AsymPlot asymplot;
  asymplot.GetHistograms(rootfile);

  if (stID) PlotStrip(rootfile, CurC, ps, stID);
  if (FEEDBACK)     asymplot.PlotFeedback(rootfile, CurC, ps);  // Plot Feedback 
  if (PLOT_BANANA)  PlotStrip(rootfile, CurC, ps);   // Plot Individual Strip
  if (SUMMARY) asymplot.PlotErrorDetectorSummary(rootfile, CurC, ps);   // Plot Error Detector Summary
  if (ERROR_DETECTOR)  asymplot.PlotErrorDetector(rootfile, CurC, ps);   // Plot Error Detector
  if (PLOT_RAW) asymplot.PlotRaw(rootfile, CurC, ps);

  // close ps file
  cout << "ps file : " << psfile << endl;
  ps->Close();

  // close logfile
  //  logfile.close();

  // remove symboric link to root file in root directory
  sprintf(text,"rm AsymPlot.root ",filename);    
  gSystem->Exec(text);  

  sprintf(text,"gv -landscape %s",psfile);
  if (GHOSTVIEW) gSystem->Exec(text);

  return 0;

}
  
