#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TMinuit.h>
#include <iostream.h>
#include <fstream>
#include <sstream>
#include <string.h>
#include <math.h>
#include <iomanip>

#define Debug 0

const Int_t N=2000;
const Int_t MB=4; // Most Significant Bit for Bunch Error Code

class ErrorDetector
{

private:
  struct StructBunch {
    Float_t NBunch[N];
    Float_t NBad[N];
    Float_t BadRate[N];
    Char_t  *ErrCode;
    Float_t MaxDev[N];
  } ;
  struct StructDetector {
    Float_t EnergySlope[N];
  };
  struct StructMaxValue {
    Float_t MassDev[N];
    Float_t M_E_Corr[N];
  };
  struct StructStrip {
    StructMaxValue max;
    Float_t InvMassSigma[N];
    Char_t * ErrCode;
    Float_t NBad[N];
    Float_t StripID[N][72];
    Float_t dummy[N];
  } ;

  struct StructData {
    Float_t RunID[N];
    StructBunch bunch;
    StructDetector detector;
    StructStrip strip;
  } data;
      
  struct StructOverFlow {
    Float_t EnergySlope;
    Float_t M_E_Corr;
    Float_t MassDev;
    Float_t InvMassSigma;
  } overflow;

			 
  Float_t dx[N],dy[N];
  TCanvas * CurC;
  TH2D * frame;
  TLegend * legend;
  TGraph *tg;
  TPostScript * ps;
  TH1D * BadBunchRate;
  TH1D * BunchMaxDev;
  TH1I * BunchErrCode;
  TH1D * EnergySlope;
  TH1D * MaxMassPosDev;
  TH1D * MaxM_E_Corr;
  TH1I * StripErrCode;
  TH1I * BadStripStatistics;
  TH2D * BadStripHistory;

public:
  Int_t Initiarize();
  Int_t OverflowControl(Int_t index);
  Int_t GetData(Char_t * DATAFILE);
  void  DrawFrame(Int_t Mode, Int_t ndata);
  Int_t MakePlots(Int_t Mode, TCanvas * CurC, TPostScript * ps);
  Int_t MakePlots(Char_t *Beam, TCanvas * CurC, TPostScript * ps);
  Int_t Plot(Int_t Mode, Int_t ndata, Int_t Color);
  Int_t GetDataAndPlot(Int_t Mode, Char_t * Beam, Int_t Color);
}; // end-class ErrorDetector


//
// Class name  : ErrorDetector
// Method name : Initiarize()
//
// Description : Initiarize variables and book histograms
// Input       : 
// Return      : 
//
Int_t
ErrorDetector::Initiarize(){

  static Int_t j; j++;
  Char_t histname[100],htitle[100];

  // -------- Bunch Error Histograms ------------- // 
  sprintf(histname,"BadBunchRate%d",j);
  BadBunchRate = new TH1D(histname, "Bad Bunch Rate", 25, 0, 100);
  sprintf(histname,"BunchMaxDev%d",j);
  BunchMaxDev = new TH1D(histname, "Specific Luminosity Bunch Max Dev.", 20, 0, 20);
  sprintf(histname,"BunchErrCode%d",j);
  BunchErrCode = new TH1I(histname, "Bunch Error Code", 4, -0.5, 3.5);

  // -------- Detector Error Histograms ------------- // 
  overflow.EnergySlope=-100;
  sprintf(histname,"EnergySlope%d",j);
  EnergySlope = new TH1D(histname, "Energy Slope", 25, overflow.EnergySlope, 0);

  // -------- Strip Error Histograms ------------- // 
  overflow.MassDev=0.5;
  sprintf(histname,"MaxMassPosDev%d",j);
  sprintf(htitle,"Maximum Mass Position Deviation [GeV]");
  MaxMassPosDev = new TH1D(histname, htitle, 10, -1*overflow.MassDev, overflow.MassDev);
  overflow.M_E_Corr=0.012;
  sprintf(histname,"MaxM_E_Corr%d",j);
  sprintf(htitle,"Maximum Mass-Energy Correlation Slope");
  MaxM_E_Corr = new TH1D(histname, htitle, 21, -1*overflow.M_E_Corr, overflow.M_E_Corr);

  overflow.InvMassSigma=3;

  sprintf(histname,"StripErrCode%d",j);
  StripErrCode = new TH1I(histname, "Strip Error Code", 4, -0.5, 3.5);

  sprintf(histname,"BadStripStatistics%d",j);
  BadStripStatistics = new TH1I(histname, "Bad Strip Statistics", 72, 0.5, 72.5);
  

  return 0;

};

//
// Class name  : ErrorDetector
// Method name : OverflowControl()
//
// Description : Controlls overflow
// Input       : Int_t index
// Return      : 
//
Int_t
ErrorDetector::OverflowControl(Int_t index){

  if (data.bunch.MaxDev[index]>20) data.bunch.MaxDev[index]=20;

  if (fabs(data.strip.max.MassDev[index])>overflow.MassDev) 
    data.strip.max.MassDev[index]=-1*overflow.MassDev;

  if (fabs(data.strip.max.M_E_Corr[index])>overflow.M_E_Corr) 
    data.strip.max.M_E_Corr[index]=-1*overflow.M_E_Corr;

  if (data.strip.InvMassSigma[index]>overflow.InvMassSigma)
    data.strip.InvMassSigma[index]=overflow.InvMassSigma;


  return 0;
}


//
// Class name  : 
// Method name : ErrCodeDecorder(Char_t *ErrCode){
//
// Description : 
// Input       : Char_t *ErrCode
// Return      : 
//
Int_t
ErrCodeDecorder(Char_t *ErrCode, Int_t MB, Int_t EArray[]){

  Char_t j[1];
  for (Int_t i=0; i<MB; i++) EArray[i] = -1;

  for (Int_t i=0;i<MB; i++) {
    j[0] = *ErrCode++ ;  
    if (strcmp(j,"-1")==-1) return -1; // if ErrCode=-1
    if (strcmp(j,"1")==1) {
      EArray[i] = i;
    }
  }

  return 0;
}


//
// Class name  : ErrorDetector
// Method name : GetData(Char_t *DATAFILE)
//
// Description : Get Data from DATAFILE
// Input       : Char_t *filename
// Return      : Number of lines read
//
Int_t
ErrorDetector::GetData(Char_t * DATAFILE){
                 
    ifstream fin;
    fin.open(DATAFILE,ios::in);
    if (fin.fail()){
        cerr << "ERROR: " << DATAFILE << " doesn't exist. Force exit." << endl;
        exit(-1);
    }
    
    cout << DATAFILE << " "  ;
    // Initialization of Histograms and variables
    Initiarize();

    Char_t buffer[400], line[400];
    Char_t *RunStatus;
    Int_t i=0;
    Int_t ch=0;
    Float_t nevents;
    while ( ( ch = fin.peek()) != EOF ) {

      fin.getline(buffer, sizeof(buffer), '\n');
      sprintf(line,buffer);
      // skip header if DATAFILE starts from "==========="
      if (strstr(buffer,"=====")) {
        for (int j=0; j<4; j++) fin.getline(buffer, sizeof(buffer), '\n');
      }

      data.RunID[i] = atof(strtok(buffer," " ));
      RunStatus = strtok(NULL," ");
      
      if ( strcmp(RunStatus,"Bad")*strcmp(RunStatus,"N/A")*strcmp(RunStatus,"Junk") ) {

	nevents = atof(strtok(NULL," "));
	data.bunch.NBunch[i] = atof(strtok(NULL," " ));
	data.bunch.NBad[i] = atof(strtok(NULL," " ));
	data.bunch.BadRate[i] = atof(strtok(NULL," " ));
	data.bunch.ErrCode = strtok(NULL," " );
	data.bunch.MaxDev[i] = atof(strtok(NULL," " )) ;

	data.detector.EnergySlope[i] = atof(strtok(NULL," " )) ;

	data.strip.max.MassDev[i] = atof(strtok(NULL," " )) ;
	data.strip.max.M_E_Corr[i] = atof(strtok(NULL," " )) ;
	data.strip.InvMassSigma[i] = atof(strtok(NULL," " )) ;
	data.strip.ErrCode = strtok(NULL," ");
	data.strip.NBad[i] = atof(strtok(NULL," " ));

	Char_t * tok; Int_t k=0;
	while (tok=strtok(NULL," " )) {
	  data.strip.StripID[i][k]=atoi(tok);
	  BadStripStatistics->Fill(data.strip.StripID[i][k]);
	  k++;
	};
	dx[i]=dy[i]=0; data.strip.dummy[i]=-2;

	// Overflows
	OverflowControl(i);

	// Error Code Decorder
	Int_t EArray[4];
	if (!ErrCodeDecorder(data.bunch.ErrCode, MB, EArray)) { 
	  for (Int_t k=0; k<MB; k++) {
	    if (EArray[k] != -1) BunchErrCode->Fill(EArray[k]);
	  }
	}
	for (int j=0;j<4;j++) EArray[j]=0;
	if (!ErrCodeDecorder(data.strip.ErrCode, MB, EArray)) { 
	  for (Int_t k=0; k<MB; k++) {
	    if (EArray[k] != -1) StripErrCode->Fill(EArray[k]);
	  }
	}

	// Fill 1-dim histograms
	BadBunchRate->Fill(data.bunch.BadRate[i]);
	BunchMaxDev->Fill(data.bunch.MaxDev[i]);
	EnergySlope->Fill(data.detector.EnergySlope[i]);
	MaxM_E_Corr->Fill(data.strip.max.M_E_Corr[i]);
	MaxMassPosDev->Fill(data.strip.max.MassDev[i]);

	++i; 
	if (i>N-1){
          cerr << "WARNING : input data exceed the size of array " << N << endl;
          cerr << "          Ignore beyond line " << N << endl;
          break;
	} // if-(i>N)

      } // if (RunStatus)

    }// end-of-while(!fin.eof())

    fin.close();
    return i-1;

}


//
// Class name  : ErrorDetector
// Method name : Plot(Int_t Mode)
//
// Description : Plot data
// Input       : Int_t Mode, Int_t ndata
// Return      : 
//
Int_t
ErrorDetector::Plot(Int_t Mode, Int_t ndata, Int_t Color){

  Char_t text[100];
  switch (Mode) {
  case 100:
    tg = new TGraph(ndata, data.RunID, data.bunch.NBunch);
    sprintf(text,"Total Bunch");
    break;
  case 101:
    tg = new TGraph(ndata, data.RunID, data.bunch.NBad);
    sprintf(text,"Bad Bunch");
    break;
  case 110:
    tg = new TGraph(ndata, data.RunID, data.bunch.BadRate);
    break;
  case 200:
    tg = new TGraph(ndata, data.RunID, data.detector.EnergySlope);
    break;
  case 300:
    tg = new TGraph(ndata, data.RunID, data.strip.max.MassDev);
    break;
  case 310:
    tg = new TGraph(ndata, data.RunID, data.strip.max.M_E_Corr);
    break;
  case 320:
    tg = new TGraph(ndata, data.RunID, data.strip.InvMassSigma);
    break;
  case 340:
    tg = new TGraph(ndata, data.RunID, data.strip.NBad);
    break;
  }

  Int_t Mtyp = Mode&1 ? 24 : 20;
  tg -> SetMarkerStyle(Mtyp);
  tg -> SetMarkerSize(1.2);
  tg -> SetMarkerColor(Color);
  tg -> SetLineColor(Color);
  tg -> SetLineWidth(Color);
  tg -> Draw("P");
    

  //Legend
  if (Mode<110){
    legend->AddEntry(tg,text,"P");
    legend->Draw("same");
  }


  return 0;

} // end-of-Plot()


//
// Class name  : ErrorDetector
// Method name : DrawFrame(Int_t Mode, Int_t ndata)
//
// Description : Draws Frame
// Input       : (Int_t Mode, Int_t ndata)
// Return      : 
//
void
ErrorDetector::DrawFrame(Int_t Mode, Int_t ndata){

  Float_t margin=0.05;
  Float_t xmin=0;
  Float_t xmax=9999;
  Float_t ymin=0;
  Float_t ymax=120;

  Char_t title[100];
  Char_t xtitle[100]="Fill Number";
  Char_t ytitle[100]="Number of Bad Bunches";

  switch (Mode) {
  case 100:
    sprintf(title,"Number of Bunches");
    GetScale(data.RunID, ndata, margin, xmin, xmax);
    break;
  case 110:
    sprintf(title,"Bad Bunch Rate");
    sprintf(ytitle,"(# Bad Bunch/#Total Bunch) [%]");
    GetScale(data.RunID, ndata, margin, xmin, xmax);
    ymin=0, ymax=100;
    break;
  case 200:
    sprintf(title,"Energy Slope");
    sprintf(ytitle,"Energy Slope");
    GetScale(data.RunID, ndata, margin, xmin, xmax);
    ymin=-100, ymax=0;
    break;
  case 300:
    sprintf(title,"Maximum Mass Position Deviation");
    sprintf(ytitle,"12C Invariant Mass Deviatoin from M_12C [GeV]");
    GetScale(data.RunID, ndata, margin, xmin, xmax);
    GetScale(data.strip.max.MassDev, ndata, margin, ymin, ymax);
    break;
  case 310:
    sprintf(title,"Maximum Mass-Energy Correlation");
    sprintf(ytitle,"Mass-Energy Correlation Slope");
    GetScale(data.RunID, ndata, margin, xmin, xmax);
    GetScale(data.strip.max.M_E_Corr, ndata, margin, ymin, ymax);
    break;
  case 320:
    sprintf(title,"Invariant Mass Width Average");
    sprintf(ytitle,"Invariant Mass Witdh Average [GeV]");
    GetScale(data.RunID, ndata, margin, xmin, xmax);
    ymin=-0.1, ymax=3;
    break;
  case 340:
    sprintf(title,"Number of Problematic Strips");
    sprintf(ytitle,"# of Problematic Strips");
    GetScale(data.RunID, ndata, margin, xmin, xmax);
    ymin=-1.5, ymax=72.5;
    break;
  case 350:
    sprintf(title,"Problematic Strip ID History");
    sprintf(ytitle,"Problematic Strips ID");
    GetScale(data.RunID, ndata, margin, xmin, xmax);
    ymin=-1.5, ymax=72.5;
    break;
  }

  frame = new TH2D("frame",title, 10, xmin, xmax, 10, ymin, ymax);
  frame -> SetStats(0);
  frame -> GetXaxis()->SetTitle(xtitle);
  frame -> GetYaxis()->SetTitle(ytitle);
  frame -> Draw();

  return;
}
//
// Class name  : ErrorDetector
// Method name : MakePlots(0
//
// Description : Plotting controll center
// Input       : 
// Return      : 
//
Int_t
ErrorDetector::GetDataAndPlot(Int_t Mode, Char_t * Beam, Int_t Color){

  Char_t DATAFILE[100],text[100], histname[100], htitle[100];
  sprintf(DATAFILE,"summary/ErrorDet_%s_phys.dat",Beam);
  cout << "DATAFILE=" << DATAFILE << endl;

  Int_t ndata = GetData(DATAFILE);

  Int_t i,j; Float_t xmin,xmax,ymin,ymax;  Float_t margin=0.05;
  if (!(Mode&1)) {
    DrawFrame(Mode, ndata);
    Plot(Mode, ndata, Color);
    if (Mode==100) Plot(Mode+1, ndata, Color);
  } else {
    switch (Mode) {
    case 115:
      BadBunchRate->SetXTitle("# of Bad Bunch / Total Bunch [%]");
      BadBunchRate->SetFillColor(Color);
      BadBunchRate->Draw();
      break;
    case 125:
      BunchErrCode->SetXTitle("Bunch Error Code ");
      BunchErrCode->SetFillColor(Color);
      BunchErrCode->GetXaxis()->SetBinLabel(1,"Specific Luminosity");
      BunchErrCode->GetXaxis()->SetBinLabel(2,"X90 Asymmetry");
      BunchErrCode->GetXaxis()->SetBinLabel(3,"X45 Asymmetry");
      BunchErrCode->GetXaxis()->SetBinLabel(4,"Y45 Asymmetry");
      BunchErrCode->Draw();
      break;
    case 135:
      BunchMaxDev->SetXTitle("Maximum Deviation from Average [sigma]");
      BunchMaxDev->SetFillColor(Color);
      BunchMaxDev->Draw();
      break;
    case 205:
      EnergySlope->SetXTitle("Energy Slope [GeV/keV]");
      EnergySlope->SetFillColor(Color);
      EnergySlope->Draw();
      break;
    case 305:
      MaxMassPosDev->SetXTitle("Maximum Mass Position Deviation [GeV]");
      MaxMassPosDev->SetFillColor(Color);
      MaxMassPosDev->Draw();
      break;
    case 315:
      MaxM_E_Corr->SetXTitle("Maximum Mass-Energy Correlation Slope [GeV/keV]");
      MaxM_E_Corr->SetFillColor(Color);
      MaxM_E_Corr->Draw();
      break;
    case 335:
      StripErrCode->SetXTitle("Strip Error Code ");
      StripErrCode->SetFillColor(Color);
      StripErrCode->GetXaxis()->SetBinLabel(1,"Mass-E Correlation");
      StripErrCode->GetXaxis()->SetBinLabel(2,"#Events in Banana");
      StripErrCode->GetXaxis()->SetBinLabel(3,"Mass Position");
      StripErrCode->GetXaxis()->SetBinLabel(4,"Mass Width");
      StripErrCode->Draw();
      break;
    case 353:
      // Superpose Measurements as a Reference
      TGraph * tg = new TGraph(ndata, data.RunID, data.strip.dummy);
      tg->SetMarkerColor(11);
      tg->SetMarkerSize(1.2);
      tg->SetMarkerStyle(20);

      // primary 2D histograms
      sprintf(histname,"BadStripHistory%d",j);
      sprintf(htitle,"Problematic Strip ID History");
      GetScale(data.RunID, ndata, margin, xmin, xmax);
      BadStripHistory = new TH2D(histname, htitle, ndata, xmin, xmax, 750, -2.5, 72.5);
      BadStripHistory -> GetXaxis()->SetTitle("Run ID");
      BadStripHistory -> GetYaxis()->SetTitle("Problematic Strip ID");
      BadStripHistory -> SetMarkerSize(10);
      BadStripHistory -> SetMarkerColor(Color);
      BadStripHistory -> SetFillColor(Color);
      BadStripHistory->GetListOfFunctions() -> Add(tg,"p");
      for (i=0; i<ndata; i++) {
	for (j=0; j<data.strip.NBad[i]; j++) {
	  BadStripHistory->Fill(data.RunID[i], data.strip.StripID[i][j]);
	}
      }
      for (i=0;i<6;i++) { // DrawLines
	TLine * l = new TLine(xmin, 12*i, xmax, 12*i);
	l -> SetLineStyle(2);
	BadStripHistory->GetListOfFunctions() -> Add(l);
      }
      BadStripHistory -> Draw();
      break;
    case 355:
      BadStripStatistics->SetXTitle("Strip ID");
      BadStripStatistics->SetFillColor(Color);
      BadStripStatistics->Draw();
      xmin = xmax = BadStripStatistics->GetXaxis()->GetXmax();
      ymin = BadStripStatistics->GetYaxis()->GetXmin();
      ymax = BadStripStatistics->GetMaximum();
      TGaxis * A1 = new TGaxis(xmin,ymin,xmax,ymax,0,ymax/Float_t(ndata)*100,510,"+L");
      A1 -> SetTitle("Bad Strip Frequencey [%]");
      A1 -> Draw("same");
      sprintf(text,"Total # of Runs %d", ndata);
      TText * t = new TText(3,BadStripStatistics->GetMaximum(),text);
      t->Draw("same");
      break;
    }
  }


  return 0;

}



//
// Class name  : ErrorDetector
// Method name : MakePlots(Int_t Mode)
//
// Description : Plotting controll center
// Input       : 
// Return      : 
//
Int_t
ErrorDetector::MakePlots(Int_t Mode, TCanvas * CurC, TPostScript *ps){


  legend = new TLegend(0.15,0.7,0.35,0.85);
  // plot blue
  GetDataAndPlot(Mode, "Blue", 4);
  CurC -> Update();
  ps->NewPage();

  legend = new TLegend(0.15,0.7,0.35,0.85);
  // plot yellow
  GetDataAndPlot(Mode, "Yellow", 95);
  CurC -> Update();
  ps->NewPage();


  return 0;

} // end-of-Plot()



//
// Class name  : ErrorDetector
// Method name : ErrorDetector
//
// Description : Main program
// Input       : 
// Return      : 
//


Int_t 
ErrorDetector::ErrorDetector()
{


  // load header macro
  Char_t HEADER[100];
  sprintf(HEADER,"%s/SuperposeSummaryPlot.h",gSystem->Getenv("MACRODIR"));
  gROOT->LoadMacro(HEADER);

  // some utility macros
  sprintf(HEADER,"%s/include.h",gSystem->Getenv("MACRODIR"));
  gROOT->LoadMacro(HEADER);

  // postscript file
  Char_t psfile[100];
  sprintf(psfile,"ps/ErrorDetector.ps");
  ps = new TPostScript(psfile,112);

  // Cambus Setup
  CurC = new TCanvas("CurC","",1);
  CurC -> SetGridy();

  gStyle->SetOptStat(1111);
  MakePlots(100, CurC, ps); // Total Number of Bunches
  MakePlots(110, CurC, ps); // Bad Bunch Rate
  MakePlots(115, CurC, ps); // Bad Bunch Rate (1-dim hist)
  MakePlots(125, CurC, ps); // Bunch Error Code (1-dim hist)
  MakePlots(135, CurC, ps); // Max Specific Luminosity Deviation (1-dim hist)
  MakePlots(200, CurC, ps); // Energy Slope
  MakePlots(205, CurC, ps); // Energy Slope (1-dim hist)
  MakePlots(300, CurC, ps); // Maximum Mass Postion Deviation 
  MakePlots(305, CurC, ps); // Maximum Mass Postion Deviation (1-dim hist)
  MakePlots(310, CurC, ps); // Maximum Mass-Energy Correlation
  MakePlots(315, CurC, ps); // Maximum Mass-Energy Correlation (1-dim hist)
  MakePlots(320, CurC, ps); // Invariant Mass Width Average 
  MakePlots(335, CurC, ps); // Strip Error Code
  MakePlots(340, CurC, ps); // Number of Bad Strips
  gStyle->SetOptStat(11);
  CurC->SetGridy(0);
  MakePlots(353, CurC, ps); // Bad Strip History
  CurC->SetGridy();
  MakePlots(355, CurC, ps); // Bad Strip Statistics (1-dim)

  cout << "ps file : " << psfile << endl;
  ps->Close();
    
  gSystem->Exec("gv -landscape ps/ErrorDetector.ps");
 
  return 0;

}



