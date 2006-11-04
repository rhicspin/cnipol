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

  struct StructStrip {
    Float_t NBad[N];
    Float_t MassMaxChi2[N];
  } ;

  struct StructData {
    Float_t RunID[N];
    StructBunch bunch;
    StructStrip strip;
  } data;
      
  Float_t dx[N],dy[N];
  TCanvas * CurC;
  TH2D * frame;
  TLegend * legend;
  TGraph *tg;
  TPostScript * ps;
  TH1D * BadBunchRate;
  TH1D * BunchMaxDev;
  TH1I * BunchErrCode;
  

public:
  Int_t Initiarize();
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
  Char_t histname[100];

  sprintf(histname,"BadBunchRate%d",j);
  BadBunchRate = new TH1D(histname, "Bad Bunch Rate", 25, 0, 100);
  sprintf(histname,"BunchMaxDev%d",j);
  BunchMaxDev = new TH1D(histname, "Specific Luminosity Bunch Max Dev.", 20, 0, 20);
  sprintf(histname,"BunchErrCode%d",j);
  BunchErrCode = new TH1I(histname, "Bunch Error Code", 4, -0.5, 3.5);

  return 0;

};

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

    Char_t buffer[300], line[300];
    Int_t i=0;
    Int_t ch=0;
    while ( ( ch = fin.peek()) != EOF ) {

      fin.getline(buffer, sizeof(buffer), '\n');
      sprintf(line,buffer);
      // skip header if DATAFILE starts from "==========="
      if (strstr(buffer,"=====")) {
        for (int j=0; j<4; j++) fin.getline(buffer, sizeof(buffer), '\n');
      }

      data.RunID[i] = atof(strtok(buffer," " ));
      data.bunch.NBunch[i] = atof(strtok(NULL," " ));
      data.bunch.NBad[i] = atof(strtok(NULL," " ));
      data.bunch.BadRate[i] = atof(strtok(NULL," " ));
      data.bunch.ErrCode = strtok(NULL," " );
      data.bunch.MaxDev[i] = atof(strtok(NULL," " )) ;
      data.strip.NBad[i] = atof(strtok(NULL," " ));
      data.strip.MassMaxChi2[i] = atof(strtok(NULL," " ));
      dx[i]=dy[i]=0;

      // Overflows
      if (data.bunch.MaxDev[i]>20) data.bunch.MaxDev[i]=20;

      // Error Code Decorder
      Int_t EArray[4];
      if (!ErrCodeDecorder(data.bunch.ErrCode, MB, EArray)) { 
	for (Int_t k=0; k<MB; k++)  BunchErrCode->Fill(EArray[k]);
      }

      // Fill 1-dim histograms
      BadBunchRate->Fill(data.bunch.BadRate[i]);
      BunchMaxDev->Fill(data.bunch.MaxDev[i]);

      ++i; 
      if (i>N-1){
          cerr << "WARNING : input data exceed the size of array " << N << endl;
          cerr << "          Ignore beyond line " << N << endl;
          break;
      } // if-(i>N)

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
  }

  Int_t Mtyp = Mode&1 ? 24 : 20;
  tg -> SetMarkerStyle(Mtyp);
  tg -> SetMarkerColor(Color);
  tg -> SetLineColor(Color);
  tg -> SetLineWidth(Color);
  tg -> Draw("P");
    

  //Legend
  if (text){
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

  Char_t DATAFILE[100];
  sprintf(DATAFILE,"summary/ErrorDet_%s_BadBunch.dat",Beam);
  cout << "DATAFILE=" << DATAFILE << endl;

  Int_t ndata = GetData(DATAFILE);
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
      BunchErrCode->Draw();
      break;
    case 135:
      BunchMaxDev->SetXTitle("Maximum Deviation from Average [sigma]");
      BunchMaxDev->SetFillColor(Color);
      BunchMaxDev->Draw();
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

  MakePlots(100, CurC, ps); // Total Number of Bunches
  MakePlots(110, CurC, ps); // Bad Bunch Rate
  MakePlots(115, CurC, ps); // Bad Bunch Rate (1-dim hist)
  MakePlots(125, CurC, ps); // Bunch Error Code (1-dim hist)
  MakePlots(135, CurC, ps); // Max Specific Luminosity Deviation (1-dim hist)

  cout << "ps file : " << psfile << endl;
  ps->Close();
    
  gSystem->Exec("gv -landscape ps/ErrorDetector.ps");
 
  return 0;

}



