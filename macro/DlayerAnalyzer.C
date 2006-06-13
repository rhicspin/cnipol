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
Int_t RUN=5;

void GetScale(Float_t *x, Int_t N, Float_t margin, Float_t & min, Float_t & max);

class DlayerAnalyzer
{

private:
  Float_t RunID[N],Dl[N],DlE[N],ReadRate[N],WCM[N],SpeLumi[N],NBunch[N]; 
  Float_t AveT0[N],DeltaT0[N],Bunch[N];
  Float_t Dl_det1[N], t0Strip1[N],t0EStrip1[N],DlStrip1[N],DlEStrip1[N];
  Float_t t0_2par_Strip1[N],t0E_2par_Strip1[N];
  Float_t dx[N],dy[N];
  TH2D* frame ;
  TNtuple * ntp ;

  typedef struct {
    Float_t t0[N];
    Float_t t0E[N];
    Float_t Dl[N];
    Float_t DlE[N];
  } StructFitMode;

  struct SingleStruct {
    StructFitMode Par2, Par1;
  } single;


    
public:
  Int_t Plot(Int_t, Int_t, Int_t, Char_t *, Int_t, TLegend *aLegend);
  Int_t DlayerPlot(Char_t*, Int_t);
  Int_t BlueAndYellowBeams(Int_t Mode, TCanvas *CurC, TPostScript *ps);
  Int_t DrawFrame(Int_t Mode,Int_t ndata, Char_t*);
  Int_t DlayerAnalyzer();
  Int_t GetData(Char_t * DATAFILE);


}; // end-class DlayerAnalyzer


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
// Class name  : DlayerAnalyzer
// Method name : GetData(Char_t *DATAFILE)
//
// Description : Get Data from DATAFILE
// Input       : Char_t *filename
// Return      : Number of lines read
//
Int_t
DlayerAnalyzer::GetData(Char_t * DATAFILE){
                 
    ifstream fin;
    fin.open(DATAFILE,ios::in);
    if (fin.fail()){
        cerr << "ERROR: " << DATAFILE << " doesn't exist. Force exit." << endl;
        exit(-1);
    }


    // New Ntuples declaration
    ntp = new TNtuple("ntp","Deadlayer Analysis","RunID:Dl:DlE:ReadRate:WCM:SpeLumi:NBunch:AveT0:DeltaT0:Bunch:dx,dy");

    Float_t dum[10];
    Int_t i=0;
    Int_t ch=0;
    while (!fin.eof()) {

        fin >> RunID[i] >> Dl[i] >> DlE[i] >> ReadRate[i] >> WCM[i] >> SpeLumi[i] >> NBunch[i]
            >> AveT0[i] >> DeltaT0[i] >> Bunch[i] 
	    >> single.Par1.Dl[i] >> single.Par1.t0[i] >> single.Par1.t0E[i] 
	    >> single.Par2.Dl[i] >> single.Par2.DlE[i] >> single.Par2.t0[i] >> single.Par2.t0E[i]; 

	Dl[i] -= (3.66*ReadRate[i]*ReadRate[i]-0.02*ReadRate[i]);
	//Dl[i] -= (7.13*ReadRate[i]*ReadRate[i]*ReadRate[i]-9.44*ReadRate[i]*ReadRate[i]+5.78*ReadRate[i]);
	//	Dl[i] -= (7.86*ReadRate[i]*ReadRate[i]*ReadRate[i]-10.71*ReadRate[i]*ReadRate[i]+6.29*ReadRate[i]);
	dx[i]=dy[i]=0;
	ntp->Fill(RunID[i],Dl[i],DlE[i],ReadRate[i],WCM[i],SpeLumi[i],NBunch[i],AveT0[i],DeltaT0[i],Bunch[i],dx[i],dy[i]);
	
	single.Par1.Dl[i] -= 20;
	single.Par2.Dl[i] -= 15;
	single.Par1.t0[i] -=  5;
	single.Par2.t0[i] -= 10;

	++i; 
	if (i>N-1){
          cerr << "WARNING : input data exceed the size of array " << N << endl;
          cerr << "          Ignore beyond line " << N << endl;
          break;
	} // if-(i>N)

    }// end-of-while(!fin.eof())

    // Run-5, Run-6, Run-7....
    if (RunID[0]>7400) RUN=6;

    fin.close();
    return i-1;

}

//
// Class name  : DlayerAnalyzer
// Method name : Plot(Int_t Mode)
//
// Description : Plot data
// Input       : Int_t Mode, Int_t ndata
// Return      : 
//
Int_t
DlayerAnalyzer::Plot(Int_t Mode, Int_t ndata, Int_t Mtyp, Char_t*text, 
		    Int_t Color, TLegend *aLegend){

  bool Single=true;

  switch (Mode) {
  case 10:
    TGraphErrors* tgae = new TGraphErrors(ndata, RunID, Dl, dx, DlE);
  if (Single){
    TGraphErrors* tgaP1 = new TGraphErrors(ndata, RunID, single.Par1.Dl, dx, dy);
    tgaP1 -> SetMarkerStyle(Mtyp+2);
    tgaP1 -> SetMarkerSize(1.0);
    tgaP1 -> SetLineWidth(2);
    tgaP1 -> SetMarkerColor(20);
    tgaP1 -> Draw("P");
    TGraphErrors* tgaP2 = new TGraphErrors(ndata, RunID, single.Par2.Dl, dx, dy);
    tgaP2 -> SetMarkerStyle(Mtyp+3);
    tgaP2 -> SetMarkerSize(1.0);
    tgaP2 -> SetLineWidth(2);
    tgaP2 -> SetMarkerColor(25);
    tgaP2 -> Draw("P");
  }
    break;
  case 20:
    TGraphErrors* tgae = new TGraphErrors(ndata, ReadRate, Dl, dx, DlE);
    break;
  case 30:
    TGraphErrors* tgae = new TGraphErrors(ndata, RunID, AveT0, dx, dy);
  if (Single){
    TGraphErrors* tgaP1 = new TGraphErrors(ndata, RunID, single.Par1.t0, dx, single.Par1.t0E);
    tgaP1 -> SetMarkerStyle(Mtyp+2);
    tgaP1 -> SetMarkerSize(1.0);
    tgaP1 -> SetLineWidth(2);
    tgaP1 -> SetMarkerColor(20);
    tgaP1 -> Draw("P");
    TGraphErrors* tgaP2 = new TGraphErrors(ndata, RunID, single.Par2.t0, dx, dy);
    tgaP2 -> SetMarkerStyle(Mtyp+3);
    tgaP2 -> SetMarkerSize(1.0);
    tgaP2 -> SetLineWidth(2);
    tgaP2 -> SetMarkerColor(25);
    tgaP2 -> Draw("P");
  }
    break;
  case 40:
    TGraphErrors* tgae = new TGraphErrors(ndata, RunID, DeltaT0, dx, dy);
    break;
  case 50:
    TGraphErrors* tgae = new TGraphErrors(ndata, ReadRate, AveT0, dx, dy);
    break;
  case 60:
    TGraphErrors* tgae = new TGraphErrors(ndata, RunID, ReadRate, dx, dy);
    break;
  case 70:
    TGraphErrors* tgae = new TGraphErrors(ndata, RunID, NBunch, dx, dy);
    break;
  case 100:
    TGraphErrors* tgae = new TGraphErrors(ndata, AveT0, Dl, dx, DlE);
    break;
  case 101:
    TGraphErrors* tgae = new TGraphErrors(ndata, AveT0, Dl, dx, DlE);
    break;
  }



  tgae -> SetMarkerStyle(Mtyp);
  tgae -> SetMarkerSize(1.3);
  tgae -> SetLineWidth(2);
  tgae -> SetMarkerColor(Color);
  tgae -> Draw("P");

  aLegend->AddEntry(tgae,text,"P");
  aLegend->Draw("same");

  OverDrawNtuple(Mode, ndata, Mtyp, text, Color, aLegend);

  return 0;

} // end-of-DlayerPlot()


//
// Class name  : DlayerAnalyzer
// Method name : OverDrawNtuple(Int_t Mode, Int_t ndata, Int_t Mtyp, Char_t*text, 
//			       Int_t Color, TLegend *aLegend)
//
// Description : Overdraw present plot with ntuples 
// Input       : Int_t Mode, Int_t ndata, Mtype, text, Color, TLegend
// Return      : 
//
Int_t
DlayerAnalyzer::OverDrawNtuple(Int_t Mode, Int_t ndata, Int_t Mtyp, Char_t*text, 
			       Int_t Color, TLegend *aLegend){


  bool YesLegend = false;

  ntp->SetMarkerStyle(Mtyp);
  ntp->SetMarkerSize(1.3);
  if (Color==4) {
    ntp->SetMarkerColor(7);
  }else{
    ntp->SetMarkerColor(5);
  }

  Char_t CuT[100];
  sprintf(CuT,"Bunch==60");

  switch (Mode) {
  case 10:
    ntp->Draw("Dl:RunID",CuT,"same");
    YesLegend = true;
    break;
  case 20:
    ntp->Draw("Dl:ReadRate",CuT,"same");
    YesLegend = true;
    break;
  case 30:
    ntp->Draw("AveT0:RunID",CuT,"same");
    YesLegend = true;
    break;
  case 60:
    ntp->Draw("ReadRate:RunID",CuT,"same");
    YesLegend = true;
    break;
  case 70:
    ntp->Draw("NBunch:RunID",CuT,"same");
    YesLegend = true;
    break;
  case 100:
    ntp->Draw("Dl:AveT0",CuT,"same");
    YesLegend = true;
    break;

  };

  if (YesLegend){
    Char_t legend[100];
    sprintf(legend,"%s(60Bunch)",text);
    aLegend->AddEntry(ntp,legend,"P");
    aLegend->Draw("same");
  }


  return 0;

} // end-of-OverDrawNtuple()





//
// Class name  : DlayerAnalyzer
// Method name : DrawFrame()
//
// Description : 
// Input       : 
// Return      : 
//
Int_t 
DlayerAnalyzer::DrawFrame(Int_t Mode, Int_t ndata, Char_t *Beam){

  Float_t xmin, xmax, ymin, ymax;
  Float_t margin=0.05;
  Char_t xtitle[100],ytitle[100];
  Char_t title[100],htitle[100];
  sprintf(title,"DeadLayer History (%s)",Beam);

  switch (Mode) {
  case 10:
    GetScale(RunID, ndata, margin, xmin, xmax);
    if (RUN==5) {ymin=20  ; ymax=65;}
    if (RUN==6) {ymin=50  ; ymax=75;}
    sprintf(xtitle,"Fill Number");
    sprintf(ytitle,"DeadLayer Thickness [ug/cm^2]");
    sprintf(title,"DeadLayer History (%s)",Beam);
    break;
  case 20:
    xmin=0.0 ; xmax=1.5;
    if (RUN==5) {ymin=20  ; ymax=65;}
    if (RUN==6) {ymin=60  ; ymax=75;}
    sprintf(xtitle,"Event Rate [MHz]");
    sprintf(ytitle,"DeadLayer Thickness [ug/cm^2]");
    sprintf(title," DeadLayer Rate Dependence (%s)",Beam);
    break;
  case 30:
    GetScale(RunID, ndata, margin, xmin, xmax);
    GetScale(AveT0, ndata, margin, ymin, ymax);
    if ((RUN==5)&&(Beam=="Blue")) {ymax=-5;};
    if (RUN==6) {ymin=-20  ; ymax=0;}
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
  case 50:
    xmin=0.0 ; xmax=1.5;
    if (RUN==5) {ymin=-20  ; ymax=20;}
    if (RUN==6) {ymin=-20  ; ymax=0;}
    sprintf(xtitle,"Event Rate [MHz]");
    sprintf(ytitle,"t0 Average [ns]");
    sprintf(title," t0 Average Rate Dependence (%s)",Beam);
    break;
  case 60:
    GetScale(RunID, ndata, margin, xmin, xmax);
    ymin=0.0 ; ymax=1.6;
    sprintf(ytitle,"Event Rate [MHz]");
    sprintf(xtitle,"Fill Number");
    sprintf(title," Event Rate History",Beam);
    break;
  case 70:
    GetScale(RunID, ndata, margin, xmin, xmax);
    ymin=0.0 ; ymax=120;
    sprintf(ytitle,"Number of Filled Bunches");
    sprintf(xtitle,"Fill Number");
    sprintf(title," Filled Bunch History (%s)",Beam);
    break;
  case 100:
    GetScale(AveT0, ndata, margin, xmin, xmax);
    if (RUN==5) {ymin=20  ; ymax=65; xmax= Beam=="Blue" ? -6 : xmax; }
    if (RUN==6) {ymin=60  ; ymax=75;}
    sprintf(xtitle,"t0 Average [ns]");
    sprintf(ytitle,"Deadlayer Thickness [ug/cm^2]");
    sprintf(title," t0-deadlayer Correlation (%s)",Beam);
    break;
  case 101:
    if (RUN==5) {xmin=-5; xmax=8; ymin=20; ymax=65;}
    if (RUN==6) {xmin=-20 ; xmax=-14;  ymin=58; ymax=80;
    if (Beam=="Blue") {xmin=-16; xmax=-10;}
    }
    sprintf(xtitle,"t0 Average [ns]");
    sprintf(ytitle,"Deadlayer Thickness [ug/cm^2]");
    sprintf(title," t0-deadlayer Correlation (%s)",Beam);
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
// Class name  : DlayerAnalyzer
// Method name : DlayerPlot
//
// Description : 
// Input       : Char_t *Beam, Int_t Mode
// Return      : 
//
Int_t 
DlayerAnalyzer::DlayerPlot(Char_t *Beam, Int_t Mode){

  Int_t Color = Beam == "Blue" ? 4 : 94 ;

  // Y-coodinates for Legend
  Float_t interval=0.15;
  Float_t ymin=0.15;
  Float_t xmin=0.7;
  switch (Mode) {
  case 30:
    xmin=0.15;
    ymin=0.15;
    break;
  case 40:
    if (RUN==6) ymin=0.70;
    break;
  case 60:
    xmin=0.15;  ymin=0.70;
    break;
  case 70:
    xmin=0.15; ymin=0.70;
    break;
  }
  Float_t xmax=xmin+interval;
  Float_t ymax=ymin+interval;

  Char_t DATAFILE[256];
  sprintf(DATAFILE,"summary/dLayer_%s_FTP.dat",Beam);
  Int_t ndata = GetData(DATAFILE);
  DrawFrame(Mode, ndata, Beam);

  // Legend
  TLegend * aLegend = new TLegend(xmin, ymin, xmax, ymax);

  // Plot flattop
  if (ndata>0) Plot(Mode, ndata, 20, "Flattop", Color, aLegend);
  sprintf(DATAFILE,"summary/dLayer_%s_INJ.dat",Beam);
  Int_t ndata = GetData(DATAFILE);
  // Plot injection
  if (ndata>0) Plot(Mode, ndata, 24, "Injection", Color, aLegend);

  return 0;

}

//
// Class name  : DlayerAnalyzer
// Method name : BlueAndYellowBeams(Int_t Mode, TCanvas *CurC, TPostScript *ps)
//
// Description : Execute Blue & Yellow make plot routines
// Input       : Int_t Mode, TCanvas *CurC, TPostScript *ps
// Return      : 
//
Int_t 
DlayerAnalyzer::BlueAndYellowBeams(Int_t Mode, TCanvas *CurC, TPostScript *ps){

  DlayerPlot("Blue",Mode);
  CurC -> Update();
  frame->Delete();

  ps->NewPage();

  DlayerPlot("Yellow",Mode);
  CurC -> Update();
  if (Mode!=101) frame->Delete();


  return 0;
    
}


//
// Class name  : DlayerAnalyzer
// Method name : DlayerAnalyzer
//
// Description : Main program
// Input       : 
// Return      : 
//

Int_t 
DlayerAnalyzer::DlayerAnalyzer()
{


    // Cambus Setup
    TCanvas *CurC = new TCanvas("CurC","",1);
    CurC -> SetGridy();

    // postscript file
    Char_t psfile[100];
    sprintf(psfile,"ps/DlayerAnalyzer.ps");
    TPostScript *ps = new TPostScript(psfile,112);


    BlueAndYellowBeams(10, CurC, ps);   // Fill vs. Deadlayer
    BlueAndYellowBeams(20, CurC, ps);   // Rate vs. Deadlayer
    BlueAndYellowBeams(30, CurC, ps);   // Fill vs. Average t0
    BlueAndYellowBeams(40, CurC, ps);   // Fill vs. Delta_t0
    BlueAndYellowBeams(50, CurC, ps);   // Rate vs. Averega t0
    BlueAndYellowBeams(60, CurC, ps);   // Event Rate History
    BlueAndYellowBeams(70, CurC, ps);   // Fill vs. Active Bunches
    BlueAndYellowBeams(100, CurC, ps);  // Average T0 vs. Deadlayer
    BlueAndYellowBeams(101, CurC, ps);  // Average T0 vs. Deadlayer (zoom)

    cout << "ps file : " << psfile << endl;
    ps->Close();

    gSystem->Exec("gv ps/DlayerAnalyzer.ps");

    return 0;

}


