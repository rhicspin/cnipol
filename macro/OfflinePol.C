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
void GetScale(Float_t *x, Int_t N, Float_t margin, Float_t & min, Float_t & max);


class OfflinePol
{

private:
  Float_t RunID[N],P_online[N],dP_online[N],P_offline[N],dP_offline[N];
  Float_t index[N],dx[N],dy[N];
  TH2D* frame ;

public:
  Int_t Plot(Int_t, Int_t, Int_t, Char_t *, Int_t, TLegend *aLegend);
  Int_t DlayerPlot(Char_t*, Int_t);
  Int_t RunBothBeam(Int_t Mode, TCanvas *CurC,  TPostScript *ps);
  Int_t DrawFrame(Int_t Mode,Int_t ndata, Char_t*);
  Int_t OfflinePol();
  Int_t GetData(Char_t * DATAFILE);


}; // end-class Offline


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
// Class name  : Offline
// Method name : GetData(Char_t *DATAFILE)
//
// Description : Get Data from DATAFILE
// Input       : Char_t *filename
// Return      : Number of lines read
//
Int_t
OfflinePol::GetData(Char_t * DATAFILE){
                 
    ifstream fin;
    fin.open(DATAFILE,ios::in);
    if (fin.fail()){
        cerr << "ERROR: " << DATAFILE << " doesn't exist. Force exit." << endl;
        exit(-1);
    }

    Float_t Fill,ID;
    Float_t dum[10];
    Char_t buffer[300];
    Char_t *RunStatus;
    Int_t i=0;
    Int_t ch=0;
    while ( ( ch = fin.peek()) != EOF ) {
      
      fin.getline(buffer, sizeof(buffer), '\n'); 
      // skip header if DATAFILE starts from "==========="
      if (strstr(buffer,"=====")) {
	for (int j=0; j<4; j++) fin.getline(buffer, sizeof(buffer), '\n'); 
      }

      // main read-in routine
      RunID[i]     = atof(strtok(buffer," "));
      P_online[i]  = atof(strtok(NULL," "));
      dP_online[i] = atof(strtok(NULL," "));
      RunStatus    = strtok(NULL," ");
      for (int k=0; k<6; k++) strtok(NULL, " ");
      P_offline[i]  = atof(strtok(NULL," "));
      dP_offline[i] = atof(strtok(NULL," "));

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
// Class name  : Offline
// Method name : Plot(Int_t Mode)
//
// Description : Plot data
// Input       : Int_t Mode, Int_t ndata
// Return      : 
//
Int_t
OfflinePol::Plot(Int_t Mode, Int_t ndata, Int_t Mtyp, Char_t*text, 
		    Int_t Color, TLegend *aLegend){

  switch (Mode) {
  case 10:
    TGraphErrors* tgae = new TGraphErrors(ndata, RunID, P_online, dx, dP_online);
    break;
  case 15:
    TGraphErrors* tgae = new TGraphErrors(ndata, index, P_online, dx, dP_online);
    break;
  case 20:
    TGraphErrors* tgae = new TGraphErrors(ndata, RunID, P_offline, dx, dP_offline);
    break;
  case 25:
    TGraphErrors* tgae = new TGraphErrors(ndata, index, P_offline, dx, dP_offline);
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
// Class name  : Offline
// Method name : DrawFrame()
//
// Description : 
// Input       : 
// Return      : 
//
Int_t 
OfflinePol::DrawFrame(Int_t Mode, Int_t ndata, Char_t *Beam){

  Float_t xmin, xmax, ymin, ymax;
  Float_t margin=0.05;

  switch (Mode) {
  case 10:
    GetScale(RunID, ndata, margin, xmin, xmax);
    ymin=20; ymax=80;
    Char_t xtitle[100]="Fill Number";
    Char_t ytitle[100]="Polarization [%]";
    break;
  case 15:
    GetScale(index, ndata, margin, xmin, xmax);
    ymin=20; ymax=80;
    Char_t xtitle[100]="Index";
    Char_t ytitle[100]="Polarization [%]";
    break;
  }

  Char_t title[100];
  sprintf(title,"Polarization (%s)",Beam);
  frame = new TH2D(Beam,title, 10, xmin, xmax, 10, ymin, ymax);
  frame -> SetStats(0);
  frame -> GetXaxis()->SetTitle(xtitle);
  frame -> GetYaxis()->SetTitle(ytitle);
  frame -> Draw();

  return 0;

}


//
// Class name  : Offline
// Method name : DlayerPlot
//
// Description : 
// Input       : 
// Return      : 
//
Int_t 
OfflinePol::DlayerPlot(Char_t *Beam, Int_t Mode){

  Int_t Color = Beam == "Blue" ? 4 : 94 ;

  Char_t DATAFILE[256];
  sprintf(DATAFILE,"summary/OfflinePol_%s.dat",Beam);
  Int_t ndata = GetData(DATAFILE);
  DrawFrame(Mode, ndata, Beam);

  TLegend * aLegend = new TLegend(0.7,0.15,0.85,0.3);
  Plot(Mode, ndata, 24, "Online", Color, aLegend);

  Int_t ndata = GetData(DATAFILE);
  Plot(Mode+10, ndata, 20, "Offline", Color, aLegend);


  return 0;

}

//
// Class name  : Offline
// Method name : RunBeam(Int_t Mode)
//
// Description : Execute Blue & Yellow make plot routines
// Input       : Int_t Mode, TCanvas *CurC, TPostScript *ps
// Return      : 
//
Int_t 
OfflinePol::RunBothBeam(Int_t Mode, TCanvas *CurC, TPostScript *ps){


  DlayerPlot("Blue",10);
  CurC -> Update();
  frame->Delete();

  ps->NewPage();

  DlayerPlot("Yellow",10);
  CurC -> Update();

  return 0;
    
}


//
// Class name  : Offline
// Method name : Offline
//
// Description : Main program
// Input       : 
// Return      : 
//

Int_t 
OfflinePol::OfflinePol()
{


    // Cambus Setup
    TCanvas *CurC = new TCanvas("CurC","",1);
    CurC -> SetGridy();

    // postscript file
    Char_t psfile[100];
    sprintf(psfile,"ps/OfflinePol.ps");
    TPostScript *ps = new TPostScript(psfile,112);

    RunBothBeam(10, CurC, ps);

    cout << "ps file : " << psfile << endl;
    ps->Close();
    
    gSystem->Exec("gv ps/OfflinePol.ps");

    return 0;

}


