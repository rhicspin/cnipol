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

const Int_t N=1000;
void GetScale(Float_t *x, Int_t N, Float_t margin, Float_t & min, Float_t & max);

class DlayerMonitor
{

private:
  Float_t RunID[N],Dl[N],DlE[N],ReadRate[N],WCM[N],SpeLumi[N],NBunch[N]; 
  Float_t AveT0[N],DeltaT0[N];
  Float_t dx[N],dy[N];
  TH2D* frame ;

public:
  Int_t Plot(Int_t, Int_t, Int_t, Char_t *, Int_t, TLegend *aLegend);
  Int_t DlayerPlot(Char_t*, Int_t);
  Int_t BlueAndYellowBeams(Int_t Mode, TCanvas *CurC, TPostScript *ps);
  Int_t DrawFrame(Int_t Mode,Int_t ndata, Char_t*);
  Int_t DlayerMonitor();
  Int_t GetData(Char_t * DATAFILE);


}; // end-class DlayerMonitor


//
// Class name  : 
// Method name : GetScale(Float_t *x, Int_t N, Float_t margin, Float& min, Float& max);
//
// Description : Calculate min and max from array x(N)
// Input       : 
// Return      : Float_t min, Float_t max
//
void
GetScale(Float_t *x, Int_t N, Float_t margin, Float_t & min, Float_t & max){

  min = max = x[0];
  for (Int_t i=0; i<N; i++) {
    if (x[i]) {
      if (x[i]<min) min=x[i];
      if (x[i]>max) max=x[i];
    }
  }

  Float_t interval = max - min;
  min -= interval*margin;
  max += interval*margin;

  return ;

}





//
// Class name  : DlayerMonitor
// Method name : GetData(Char_t *DATAFILE)
//
// Description : Get Data from DATAFILE
// Input       : Char_t *filename
// Return      : Number of lines read
//
Int_t
DlayerMonitor::GetData(Char_t * DATAFILE){
                 
    ifstream fin;
    fin.open(DATAFILE,ios::in);
    if (fin.fail()){
        cerr << "ERROR: " << DATAFILE << " doesn't exist. Force exit." << endl;
        exit(-1);
    }

    Float_t dum[10];
    Int_t i=0;
    Int_t ch=0;
    while (!fin.eof()) {

        fin >> RunID[i] >> Dl[i] >> DlE[i] >> ReadRate[i] >> WCM[i] >> SpeLumi[i] >> NBunch[i]
            >> AveT0[i] >> DeltaT0[i] >> dum[2] >> dum[3] >> dum[4] 
            >> dum[5] >> dum[6] >> dum[7] >> dum[8] >> dum[9];

      ++i; dx[i]=dy[i]=0;

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
// Class name  : DlayerMonitor
// Method name : Plot(Int_t Mode)
//
// Description : Plot data
// Input       : Int_t Mode, Int_t ndata
// Return      : 
//
Int_t
DlayerMonitor::Plot(Int_t Mode, Int_t ndata, Int_t Mtyp, Char_t*text, 
		    Int_t Color, TLegend *aLegend){

  switch (Mode) {
  case 10:
    TGraphErrors* tgae = new TGraphErrors(ndata, RunID, Dl, dx, DlE);
    break;
  case 20:
    TGraphErrors* tgae = new TGraphErrors(ndata, ReadRate, Dl, dx, DlE);
    break;
  case 30:
    TGraphErrors* tgae = new TGraphErrors(ndata, RunID, AveT0, dx, dy);
    break;
  case 40:
    TGraphErrors* tgae = new TGraphErrors(ndata, RunID, DeltaT0, dx, dy);
    break;
  }

    tgae -> SetMarkerStyle(Mtyp);
    tgae -> SetMarkerSize(1.5);
    tgae -> SetLineWidth(2);
    tgae -> SetMarkerColor(Color);
    tgae -> Draw("P");

    aLegend->AddEntry(tgae,text,"P");
    aLegend->Draw("same");

    return 0;

} // end-of-DlayerPlot()




//
// Class name  : DlayerMonitor
// Method name : DrawFrame()
//
// Description : 
// Input       : 
// Return      : 
//
Int_t 
DlayerMonitor::DrawFrame(Int_t Mode, Int_t ndata, Char_t *Beam){

  Float_t xmin, xmax, ymin, ymax;
  Float_t margin=0.05;
  Char_t xtitle[100],ytitle[100];
  Char_t title[100],htitle[100];
  sprintf(title,"DeadLayer History (%s)",Beam);

  switch (Mode) {
  case 10:
    GetScale(RunID, ndata, margin, xmin, xmax);
    ymin=20; ymax=80;
    sprintf(xtitle,"Fill Number");
    sprintf(ytitle,"DeadLayer Thickness [ug/cm^2]");
    sprintf(title,"DeadLayer History (%s)",Beam);
    break;
  case 20:
    xmin=0.0 ; xmax=1.5;
    ymin=60  ; ymax=75;
    sprintf(xtitle,"Event Rate [MHz]");
    sprintf(ytitle,"DeadLayer Thickness [ug/cm^2]");
    sprintf(title," DeadLayer Rate Dependence (%s)",Beam);
    break;
  case 30:
    GetScale(RunID, ndata, margin, xmin, xmax);
    ymin=-20  ; ymax=0;
    sprintf(xtitle,"Fill Number");
    sprintf(ytitle,"t0 Average [ns]");
    sprintf(title," t0 Average History (%s)",Beam);
    break;
  case 40:
    GetScale(RunID, ndata, margin, xmin, xmax);
    GetScale(DeltaT0, ndata, margin, ymin, ymax);
    sprintf(xtitle,"Fill Number");
    sprintf(ytitle,"Delta_t0 [ns]");
    sprintf(title," Delta_t0 History (%s)",Beam);
    break;
  }

  sprintf(htitle,"%s%d",Beam,Mode);
  frame = new TH2D(htitle,title, 10, xmin, xmax, 10, ymin, ymax);
  frame -> SetStats(0);
  frame -> GetXaxis()->SetTitle(xtitle);
  frame -> GetYaxis()->SetTitle(ytitle);
  frame -> Draw();

  return 0;

}


//
// Class name  : DlayerMonitor
// Method name : DlayerPlot
//
// Description : 
// Input       : 
// Return      : 
//
Int_t 
DlayerMonitor::DlayerPlot(Char_t *Beam, Int_t Mode){

  Int_t Color = Beam == "Blue" ? 4 : 94 ;

  // Y-coodinates for Legend
  Float_t interval=0.15;
  Float_t ymin=0.15;
  switch (Mode) {
  case 30:
    ymin=0.70;
    break;
  case 40:
    ymin=0.70;
    break;
  }
  Float_t ymax=ymin+interval;

  Char_t DATAFILE[256];
  sprintf(DATAFILE,"summary/dLayer_%s_FTP.dat",Beam);
  Int_t ndata = GetData(DATAFILE);
  DrawFrame(Mode, ndata, Beam);

  // Legend
  TLegend * aLegend = new TLegend(0.7, ymin, 0.85, ymax);

  // Plot flattop
  Plot(Mode, ndata, 20, "Flattop", Color, aLegend);
  sprintf(DATAFILE,"summary/dLayer_%s_INJ.dat",Beam);
  Int_t ndata = GetData(DATAFILE);
  // Plot injection
  Plot(Mode, ndata, 24, "Injection", Color, aLegend);

  return 0;

}

//
// Class name  : DlayerMonitor
// Method name : BlueAndYellowBeams(Int_t Mode, TCanvas *CurC, TPostScript *ps)
//
// Description : Execute Blue & Yellow make plot routines
// Input       : Int_t Mode, TCanvas *CurC, TPostScript *ps
// Return      : 
//
Int_t 
DlayerMonitor::BlueAndYellowBeams(Int_t Mode, TCanvas *CurC, TPostScript *ps){

  DlayerPlot("Blue",Mode);
  CurC -> Update();
  frame->Delete();

  ps->NewPage();
  DlayerPlot("Yellow",Mode);
  CurC -> Update();
  if (Mode!=40) frame->Delete();
  
  return 0;
    
}


//
// Class name  : DlayerMonitor
// Method name : DlayerMonitor
//
// Description : Main program
// Input       : 
// Return      : 
//

Int_t 
DlayerMonitor::DlayerMonitor()
{

    // Cambus Setup
    TCanvas *CurC = new TCanvas("CurC","",1);
    CurC -> SetGridy();

    // postscript file
    Char_t psfile[100];
    sprintf(psfile,"ps/dLmonitor.ps");
    TPostScript *ps = new TPostScript(psfile,112);

    BlueAndYellowBeams(10, CurC, ps);
    BlueAndYellowBeams(20, CurC, ps);
    BlueAndYellowBeams(30, CurC, ps);
    BlueAndYellowBeams(40, CurC, ps);

    cout << "ps file : " << psfile << endl;
    ps->Close();
    
    return 0;

}


