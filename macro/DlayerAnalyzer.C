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
Float_t SquareRootSum(Float_t A, Float_t B) { return sqrt(A*A+B*B); }


class DlayerAnalyzer
{

private:
  Float_t RunID[N],Dl[N],DlE[N],ReadRate[N],WCM[N],SpeLumi[N],NBunch[N]; 
  Float_t AveT0[N],DeltaT0[N],Emin[N],Emax[N],Bunch[N];
  Float_t Dl_det1[N], t0Strip1[N],t0EStrip1[N],DlStrip1[N],DlEStrip1[N];
  Float_t t0_2par_Strip1[N],t0E_2par_Strip1[N];
  Float_t dx[N],dy[N];
  Float_t Dl_proj[N];
  
  TH2D* frame ;
  TH1D* ProjDlayerRaw ;
  TH1D* ProjDlayerRateCorr ;
  TH1D* ProjDlayerRadCorr ;
  TH1D* DlAveDeviation ;
  TNtuple * ntp ;

  struct PlotOption {
    bool Injection;
    bool SingleStrip;
    bool Ntuple;
    bool WCM;
    bool ScaleToF;
    bool RateCorrection;
    bool RadDamageFit;
    bool ConfigFile;
  } opt;

  typedef struct {
    Float_t t0[N];
    Float_t t0E[N];
    Float_t Dl[N];
    Float_t DlE[N];
  } StructFitMode;

  struct SingleStruct {
    StructFitMode Par2, Par1;
  } single;

  struct StructDlHistoryFitPar {
    Float_t par[2];
    Float_t intersection;
  } blue[2], yellow[2];

  typedef struct {
    Float_t sigma;
    Float_t ave;
    Float_t strip; // mean average deviation of strips from the detector average
    Float_t run;   // run-by-run stability of all-strip-averaged dl 
  } StructError;

  struct Correction {
    StructError no, rate, rad;
    Float_t AsignError;
  } corr[2];

  struct Systematic {
    StructError err;
  } sys[2];

  typedef struct {
    Float_t RunID[N];
    Float_t cRunID[N];
    Float_t dx[N];
    Float_t Dl[N];
    Float_t DlE[N];
    Float_t ReadRate[N];
  } StructConfigFileDB;


  // Analysis summary data file
  ofstream fout;

public:
  void OptionHandler();
  Float_t RateCorrection(Float_t x);
  Int_t InitScope();
  Int_t Plot(Int_t, Int_t, Int_t, Char_t *, Int_t, TLegend *aLegend);
  Int_t PlotSingleStrip(Int_t Mode, Int_t ndata, Int_t Mtyp, Char_t*text);
  Int_t PlotWCM(Int_t Mode, Int_t ndata, Int_t Color, Int_t Mtyp, Char_t*text);
  Int_t DlayerPlot(Char_t*, Int_t);
  Int_t BlueAndYellowBeams(Int_t Mode, TCanvas *CurC, TPostScript *ps);
  Int_t DrawFrame(Int_t Mode,Int_t ndata, Char_t*);
  Int_t DlayerAnalyzer();
  Int_t GetData(Char_t * DATAFILE);
  Int_t RadiationDamageFitter(TGraphErrors* tgae);
  Int_t RadiationDamageDrawer(TGraphErrors* tgae, Int_t Color);
  Int_t RadiationDamageProjection(Float_t runid, Int_t Fill, Float_t dl);
  Int_t CalcSystematicError();
  Int_t PrintOutput();

  TGraphErrors * PlotConfigFile(Int_t Mode, Int_t Color, Char_t * text);

}; // end-class DlayerAnalyzer


//
// Class name  : DlayerAnalyzer
// Method name : InitScope()
//
// Description : Initiarize parameters
// Input       : 
// Return      : 
//
Int_t
DlayerAnalyzer::InitScope(){

  // deadlayer history linear fit parameters
  blue[0].par[0] = -300.47;
  blue[0].par[1] = 0.0496;
  blue[1].par[0] = -21.4;
  blue[1].par[1] = 0.00957;
  blue[0].intersection = (blue[0].par[0]-blue[1].par[0])/(blue[1].par[1]-blue[0].par[1]);

  yellow[0].par[0] = -443.43;
  yellow[0].par[1] = 0.0703;
  yellow[1].par[0] = 16;
  yellow[1].par[1] = 0.00476;
  yellow[0].intersection = (yellow[0].par[0]-yellow[1].par[0])/(yellow[1].par[1]-yellow[0].par[1]);

  return 0;
}

//
// Class name  : DlayerAnalyzer
// Method name : OptionHandler()
//
// Description : Handles analysis/plotting options
// Input       : 
// Return      : 
//
void
DlayerAnalyzer::OptionHandler(){

  // plot injection
  opt.Injection=false;
  // Plot Single strip Behavior 
  opt.SingleStrip=false;
  // Plot ntuples
  opt.Ntuple=true;
  // Plot Wall Current Monitor t0 history
  opt.WCM=false;
  // Shift 120 bunchmode ToF to make sooth continuity with 60 bunches data 
  opt.ScaleToF=false;
  // Apply Rate Correction onto Deadlayer Thickness
  opt.RateCorrection=true;
  // Fit Deadlayer History to vanish radiation damage
  opt.RadDamageFit=true;
  // plot Configulation file history
  opt.ConfigFile=false;

  // Error allocation
  corr[0].AsignError = corr[1].AsignError = 3; // sigma


  return;

}// end-of-OptionHandler()






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
                 
  static Int_t J; ++J; 
  Char_t histname[100];
  Int_t Fill;

    ifstream fin;
    fin.open(DATAFILE,ios::in);
    if (fin.fail()){
        cerr << "ERROR: " << DATAFILE << " doesn't exist. Force exit." << endl;
        exit(-1);
    }

    //Define Histograms
    sprintf(histname,"ProjDlayerRaw%d",J);
    ProjDlayerRaw      = new TH1D(histname, "Dead Layer Stability (No correction)", 50, 20, 80);
    sprintf(histname,"ProjDlayerRateCorr%d",J);
    ProjDlayerRateCorr = new TH1D(histname, "Dead Layer Stability (Rate Correction)", 50, 20, 80);
    sprintf(histname,"ProjDlayerRateRadCorr%d",J);
    ProjDlayerRadCorr  = new TH1D(histname, "Dead Layer Stability (Radiation Damage Correction)", 50, -30, 30);
    sprintf(histname,"DlAveDeviation%d",J);
    DlAveDeviation     = new TH1D(histname, "Dl Average Deviation from Detector Average", 50, 0.5, 10.5);

    // New Ntuples declaration
    ntp  = new TNtuple("ntp","Deadlayer Analysis","RunID:Dl:DlE:ReadRate:WCM:SpeLumi:NBunch:AveT0:DeltaT0:Bunch:dx,dy");
    ntps = new TNtuple("ntps","Single Strip Data","RunID:single.Par1.Dl:single.Par1.t0:single.Par1.t0E:single.Par2.Dl:single.Par2.DlE:single.Par2.t0Lsingle.Par2.t0E");

    Float_t dum[10];
    Int_t i=0;
    while (!fin.eof()) {

        fin >> RunID[i] >> Dl[i] >> DlE[i] >> ReadRate[i] >> WCM[i] >> SpeLumi[i] >> NBunch[i]
            >> AveT0[i] >> DeltaT0[i] >> Emin[i] >> Emax[i] >> Bunch[i] 
	    >> single.Par1.Dl[i] >> single.Par1.t0[i] >> single.Par1.t0E[i] 
	    >> single.Par2.Dl[i] >> single.Par2.DlE[i] >> single.Par2.t0[i] >> single.Par2.t0E[i]; 
	Fill=int(RunID[i]); dx[i]=dy[i]=0;

	// Fill Histograms
	ProjDlayerRaw -> Fill(Dl[i]);
	DlAveDeviation -> Fill(DlE[i]);

	if (opt.RateCorrection)	Dl[i] -= RateCorrection(ReadRate[i]);
	//	if (opt.RateCorrection)	Dl[i] -= (3.66*ReadRate[i]*ReadRate[i]-0.02*ReadRate[i]);
	//Dl[i] -= (7.13*ReadRate[i]*ReadRate[i]*ReadRate[i]-9.44*ReadRate[i]*ReadRate[i]+5.78*ReadRate[i]);
	//Dl[i] -= (7.86*ReadRate[i]*ReadRate[i]*ReadRate[i]-10.71*ReadRate[i]*ReadRate[i]+6.29*ReadRate[i]);

	// Fill Histograms
	ProjDlayerRateCorr -> Fill(Dl[i]);

	// Radiation Damage Correction
	Dl_proj[i] = RadiationDamageProjection(RunID[i], Fill, Dl[i]);
	ProjDlayerRadCorr  -> Fill(Dl_proj[i]);

	// adjust T0 distribution btwn 60 and 120 bunches modes
	if (opt.ScaleToF) {
	  if (Bunch[i]==120) AveT0[i] += (RunID[i]-Fill)*1000-100>0 ? 18 : 14;
	}

	// Single strip intentional shifts to avoid overlaps with average data points
	if (opt.SingleStrip){
	  single.Par1.Dl[i] -= 20; single.Par2.Dl[i] -= 15;
	  single.Par1.t0[i] -=  5; single.Par2.t0[i] -=  8;
	}

	// Ntuples
	if (opt.Ntuple){
	  ntp->Fill(RunID[i],Dl[i],DlE[i],ReadRate[i],WCM[i],SpeLumi[i],NBunch[i],AveT0[i],DeltaT0[i],Bunch[i],dx[i],dy[i]);
	  ntps->Fill(RunID[i],single.Par1.Dl[i],single.Par1.t0[i],single.Par1.t0E[i],single.Par2.Dl[i],single.Par2.DlE[i],
		     single.Par2.t0[i],single.Par2.t0E[i]);
	}

	// ignore Dl=0 entries, coz failur of deadlayer fit evidently 
	if (Dl[i]!=0) ++i; 

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
// Method name : RateCorrection(Float_t x)
//
// Description : Calculate deadlayer thickness currection for rate
// Input       : Float_t x (rate)
// Return      : 
//
Float_t
DlayerAnalyzer::RateCorrection(Float_t x){return 3.66*x*x-0.02*x; }




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

  switch (Mode) {
  case 10:
    TGraphErrors* tgae = new TGraphErrors(ndata, RunID, Dl, dx, DlE);
    if (opt.SingleStrip) PlotSingleStrip(Mode,ndata,Mtyp,text);
    if (opt.ConfigFile) {
      TGraphErrors * tg; 
      tg = PlotConfigFile(Mode,Color,text);
    }
    break;
  case 20:
    TGraphErrors* tgae = new TGraphErrors(ndata, ReadRate, Dl, dx, DlE);
    break;
  case 30:
    TGraphErrors* tgae = new TGraphErrors(ndata, RunID, AveT0, dx, dy);
    if (opt.SingleStrip) PlotSingleStrip(Mode,ndata,Mtyp,text);
    if (opt.WCM) PlotWCM(Mode,ndata,Color,Mtyp,text);
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
  case 80:
    TGraphErrors* tgae = new TGraphErrors(ndata, RunID, DlE, dx, dy);
    break;
  case 90:
    TGraphErrors* tgae = new TGraphErrors(ndata, RunID, Emin, dx, dy);
    TGraphErrors* tga2 = new TGraphErrors(ndata, RunID, Emax, dx, dy);
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

  if (Mode==90) {
    tga2 -> SetMarkerStyle(Mtyp);
    tga2 -> SetMarkerSize(1.3);
    tga2 -> SetLineWidth(2);
    tga2 -> SetMarkerColor(Color+2);
    tga2 -> Draw("P");

    aLegend->AddEntry(tga2,text,"P");
    aLegend->Draw("same");
  }

  // supoerposition ntuples
  if (opt.Ntuple) OverDrawNtuple(Mode, ndata, Mtyp, text, Color, aLegend);

  if (opt.RadDamageFit){
    // Fit Deadlayer History to vanish radiation damage (only flattop)
    //    if ((Mode==10)&&(Mtyp==20)) RadiationDamageFitter(tgae);
    if ((Mode==10)&&(Mtyp==20)) RadiationDamageDrawer(tgae, Color);
  }

  // supserpose configluation file updates by Asym
  if ((opt.ConfigFile)&&(Mode==10)) tg -> Draw("PL");

  return 0;

} // end-of-DlayerPlot()

//
// Class name  : DlayerAnalyzer
// Method name : PlotWCM(Int_t Mode, Int_t ndata, Int_t Color, Int_t Mtype, Char_t*text)
//
// Description : Plot Wall Current Monitor timing history
// Input       : Int_t Mode, Int_t ndata, Int_t Color, Int_t Mtype, Char_t text
// Return      : 
//
Int_t
DlayerAnalyzer::PlotWCM(Int_t Mode, Int_t ndata, Int_t Color, Int_t Mtyp, Char_t*text)
{ 

  if (text=="Injection") return;

  const Int_t K=20;
  Float_t wcmRunID[K];
  Float_t wcmBlueT0[K],wcmBlueT0E[K];
  Float_t wcmYellowT0[K],wcmYellowT0E[K];
  Float_t dummy[K];

  ifstream fin;
  fin.open("/home/itaru/2005/offline/WCM/dat/WCM_timingRun5.dat",ios::in);
  if (fin.fail()){
    cerr << "ERROR: " << DATAFILE << " doesn't exist. Force exit. PlotWCM Ignored." << endl;
    return;
  };

  Int_t i=0;
  while (!fin.eof()) {
    fin >> wcmRunID[i] >> wcmBlueT0[i] >> wcmBlueT0E[i] >> wcmYellowT0[i] >> wcmYellowT0E[i];
    wcmBlueT0[i] *= -1;
    wcmYellowT0[i] = -1*wcmYellowT0[i]+20;
    dummy[i]=0;
    ++i;
  };

  Int_t ndata=i-1;
  if (Color==4)  TGraphErrors* wcmT0 = new TGraphErrors(ndata, wcmRunID, wcmBlueT0, dummy, wcmBlueT0E);
  if (Color==94) TGraphErrors* wcmT0 = new TGraphErrors(ndata, wcmRunID, wcmYellowT0, dummy, wcmYellowT0E);

  Mtyp=21;
  wcmT0 -> SetMarkerStyle(Mtyp);
  wcmT0 -> SetMarkerSize(1.3);
  wcmT0 -> SetLineWidth(2);
  wcmT0 -> SetMarkerColor(40);
  wcmT0 -> Draw("P");

  return 0;
}

//
// Class name  : DlayerAnalyzer
// Method name : PlotSingleStrip(Int_t Mode, Int_t ndata, Int_t Mtype, Char_t*text)
//
// Description : Plot Single Strip data
// Input       : Int_t Mode, Int_t ndata, Int_t Mtype, Char_t text
// Return      : 
//
Int_t
DlayerAnalyzer::PlotSingleStrip(Int_t Mode, Int_t ndata, Int_t Mtyp, Char_t*text)
{ 

  Float_t xmin=0.15;
  Float_t ymin=0.70;
  Float_t interval=0.15;

  switch(Mode) {
  case 10:
    TGraphErrors* tgaP1 = new TGraphErrors(ndata, RunID, single.Par1.Dl, dx, dy);
    TGraphErrors* tgaP2 = new TGraphErrors(ndata, RunID, single.Par2.Dl, dx, dy);
    break;
  case 30:
    TGraphErrors* tgaP1 = new TGraphErrors(ndata, RunID, single.Par1.t0, dx, single.Par1.t0E);
    TGraphErrors* tgaP2 = new TGraphErrors(ndata, RunID, single.Par2.t0, dx, dy);
    xmin=0.6; ymin=0.5;
    break;
  }

    tgaP1 -> SetMarkerStyle(Mtyp+2);
    tgaP1 -> SetMarkerSize(1.0);
    tgaP1 -> SetLineWidth(2);
    tgaP1 -> SetMarkerColor(20);
    tgaP1 -> Draw("P");

    tgaP2 -> SetMarkerStyle(Mtyp+3);
    tgaP2 -> SetMarkerSize(1.0);
    tgaP2 -> SetLineWidth(2);
    tgaP2 -> SetMarkerColor(25);
    tgaP2 -> Draw("P");

  // Legend
  Char_t legend[100];
  Float_t xmax=xmin+interval+0.05;
  Float_t ymax=ymin+interval-0.05;
  TLegend * slegend = new TLegend(xmin, ymin, xmax, ymax);
  if (text=="Flattop"){
    slegend->AddEntry(tgaP1,"1 par fit(scaled)","P");
    slegend->AddEntry(tgaP2,"2 par fit(scaled)","P");
    slegend->Draw("same");
  }


  return 0;

}



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
// Method name : RadiationDamageFitter()
//
// Description : Fit deadlayer history to vanish radiation damage
// Input       : 
// Return      : 
//
Int_t
DlayerAnalyzer::RadiationDamageFitter(TGraphErrors* tgae)
{ 

  TF1 *f1 = new TF1("f1","pol1",6800,7050);
  TF1 *f2 = new TF1("f2","pol1",7000,7400);
  f1->SetLineColor(2);
  f2->SetLineColor(2);
  cout << "****** Range 1 *********" << endl;
  tgae->Fit("f1","R");
  f1->Draw("same");
  cout << "****** Range 2 *********" << endl;
  tgae->Fit("f2","R");
  f2->Draw("same");


  return 0;
};


//
// Class name  : DlayerAnalyzer
// Method name : RadiationDamageFitter()
//
// Description : draw deadlayer history fitting results
// Input       : TGraphErrors* tgae, Int_t Color
// Return      : 0
//
Int_t
DlayerAnalyzer::RadiationDamageDrawer(TGraphErrors* tgae, Int_t Color)
{ 

  Char_t linf[100];
  if (Color==4) { // Blue 
    sprintf(linf,"%f+(%f)*x",blue[0].par[0],blue[0].par[1]);
    TF1 *f1 = new TF1("f1",linf,6600,6970);
    sprintf(linf,"%f+(%f)*x",blue[1].par[0],blue[1].par[1]);
    TF1 *f2 = new TF1("f2",linf,6940,7350);
  }else{ // Yellow
    sprintf(linf,"%f+(%f)*x",yellow[0].par[0],yellow[0].par[1]);
    TF1 *f1 = new TF1("f1",linf,6600,7020);
    sprintf(linf,"%f+(%f)*x",yellow[1].par[0],yellow[1].par[1]);
    TF1 *f2 = new TF1("f2",linf,6940,7350);
  }

  f1->SetLineColor(2);
  f2->SetLineColor(2);
  f1->Draw("same");
  f2->Draw("same");

  return 0;
};


//
// Class name  : DlayerAnalyzer
// Method name : RadiationDamageProjection(Float_t RunID,Int_t Fill, Float_t dl)
//
// Description : calculate deadlayer history fitting projection
// Input       : (Float_t runid, int_t fill, Float_t dl)
// Return      : Float_t proj
//
Float_t 
DlayerAnalyzer::RadiationDamageProjection(Float_t runid, Int_t fill, Float_t dl){

  Float_t proj;
  Int_t range;
  if ((runid-fill)*1000-100 < 0) { // blue
    range = runid < blue[0].intersection  ? 0 : 1;
    proj = dl - (blue[range].par[0]+blue[range].par[1]*runid);
  }else{ // yellow
    range = runid < yellow[0].intersection  ? 0 : 1;
    proj = dl - (yellow[range].par[0]+yellow[range].par[1]*runid);
  }
  return proj;
}


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
    if (RUN==6) {ymin=55  ; ymax=85;}
    sprintf(xtitle,"Fill Number");
    sprintf(ytitle,"DeadLayer Thickness [ug/cm^2]");
    if (opt.RateCorrection) strcat(ytitle,"(Rate Corrected)");
    sprintf(title,"DeadLayer History (%s)",Beam);
    break;
  case 20:
    xmin=0.0 ; xmax=1.5;
    if (RUN==5) {ymin=20  ; ymax=65;}
    if (RUN==6) {ymin=60  ; ymax=75;}
    sprintf(xtitle,"Event Rate [MHz]");
    sprintf(ytitle,"DeadLayer Thickness [ug/cm^2]");
    if (opt.RateCorrection) strcat(ytitle,"(Rate Corrected)");
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
  case 80:
    GetScale(RunID, ndata, margin, xmin, xmax);
    ymin=0.0 ; ymax=5.5;
    sprintf(title,"Average Strip-by-Strip Deviation from Detector Average ");
    sprintf(xtitle,"Fill Number");
    sprintf(ytitle," DlE [ug/cm^2]",Beam);
    break;
  case 90:
    GetScale(RunID, ndata, margin, xmin, xmax);
    ymin=0 ; ymax=1000;
    sprintf(title,"Deadlayer Fitting Range Emin & Emax History");
    sprintf(xtitle,"Fill Number");
    sprintf(ytitle,"Fitting Range Emin & Emax [keV]",Beam);
    break;
  case 100:
    GetScale(AveT0, ndata, margin, xmin, xmax);
    if (RUN==5) {ymin=20  ; ymax=65; xmax= Beam=="Blue" ? -6 : xmax; }
    if (RUN==6) {ymin=60  ; ymax=75;}
    sprintf(title," t0-deadlayer Correlation (%s)",Beam);
    sprintf(xtitle,"t0 Average [ns]");
    sprintf(ytitle,"Deadlayer Thickness [ug/cm^2]");
    if (opt.RateCorrection) strcat(ytitle,"(Rate Corrected)");
    break;
  case 101:
    if (RUN==5) {xmin=-5; xmax=8; ymin=20; ymax=65;}
    if (RUN==6) {xmin=-20 ; xmax=-14;  ymin=58; ymax=80;
    if (Beam=="Blue") {xmin=-16; xmax=-10;}
    }
    sprintf(title," t0-deadlayer Correlation (%s)",Beam);
    sprintf(xtitle,"t0 Average [ns]");
    sprintf(ytitle,"Deadlayer Thickness [ug/cm^2]");
    if (opt.RateCorrection) strcat(ytitle,"(Rate Corrected)");
    break;
  }

  sprintf(htitle,"%s%d",Beam,Mode);
  frame = new TH2D(htitle,title, 10, xmin, xmax, 10, ymin, ymax);
  frame -> SetStats(0);
  frame -> GetXaxis()->SetTitle(xtitle);
  frame -> GetYaxis()->SetTitle(ytitle);
  frame -> Draw();

  // Superpose some beam operational comments on the frame 
  SuperposeComments(Mode, frame);


  return 0;

}


//
// Class name  : DlayerAnalyzer
// Method name : DlayerPlot
//
// Description : Draw Frame for 2D-histograms and Call Injection & Flattop plots
//             : Determin geometry for Legends.
//             : Note 1-D histograms are plotted for flattop only!
// Input       : Char_t *Beam, Int_t Mode
// Return      : 
//
Int_t 
DlayerAnalyzer::DlayerPlot(Char_t *Beam, Int_t Mode){

  Int_t Color = Beam == "Blue" ? 4 : 94 ;
  Int_t beam  = Beam == "Blue" ? 0 : 1  ;

  // Get Data from File and draw frame
  Char_t DATAFILE[256];
  sprintf(DATAFILE,"summary/dLayer_%s_FTP.dat",Beam);
  Int_t ndata = GetData(DATAFILE);
  if (!(Mode&1)) DrawFrame(Mode, ndata, Beam);

  TF1 * func = new TF1("gaussian", "gaus");

  // Default XY-coodinates for Legend
  Float_t interval=0.15;
  Float_t ymin=0.15;
  Float_t xmin=0.7;
  switch (Mode) {
  case 15:
    ProjDlayerRaw->SetXTitle("Dead Layer [ug/cm^2]");
    ProjDlayerRaw->SetFillColor(Color);
    ProjDlayerRaw->Fit("gaussian","Q");
    corr[beam].no.sigma = func->GetParameter(2); // get sigma
    break;
  case 17:
    ProjDlayerRateCorr->SetXTitle("Dead Layer [ug/cm^2]");
    ProjDlayerRateCorr->SetFillColor(Color);
    ProjDlayerRateCorr->Fit("gaussian","Q");
    corr[beam].rate.sigma = func->GetParameter(2); // get sigma
    break;
  case 19:
    ProjDlayerRadCorr->SetXTitle("Dead Layer [ug/cm^2]");
    ProjDlayerRadCorr->SetFillColor(Color);
    ProjDlayerRadCorr->Fit("gaussian","Q");
    corr[beam].rad.sigma = func->GetParameter(2);  // get sigma
    break;
  case 30:
    xmin=0.15; ymin=0.15;
    if (RUN==6) xmin=0.70; ymin=0.60;
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
  case 81:
    DlAveDeviation->SetXTitle("Average Deviation from Detector Average [ug/cm^2]");
    DlAveDeviation->SetFillColor(Color);
    DlAveDeviation->Fit("gaussian","Q");
    sys[beam].err.strip = func->GetParameter(1); // get mean
    break;
  }

  Float_t xmax=xmin+interval;
  Float_t ymax=ymin+interval;

  // Legend
  TLegend * aLegend = new TLegend(xmin, ymin, xmax, ymax);

  if (!(Mode&1)) {

    // Plot flattop
    if (ndata>0) Plot(Mode, ndata, 20, "Flattop", Color, aLegend);

    // Plot injection
    if (opt.Injection){     
      sprintf(DATAFILE,"summary/dLayer_%s_INJ.dat",Beam);
      Int_t ndata = GetData(DATAFILE);
      if (ndata>0) Plot(Mode, ndata, 24, "Injection", Color, aLegend);

    }

  } // end-of-if(!(mode&1))


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
  if (!(Mode&1)) frame->Delete();

  ps->NewPage();

  DlayerPlot("Yellow",Mode);
  CurC -> Update();
  if (!(Mode&1)) frame->Delete();

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
  gStyle->SetOptFit(111);

  // load header macro
  Char_t HEADER[100];
  sprintf(HEADER,"%s/SuperposeSummaryPlot.h",gSystem->Getenv("MACRODIR"));
  gROOT->LoadMacro(HEADER);
  sprintf(HEADER,"%s/SuperposeDlayerPlot.h",gSystem->Getenv("MACRODIR"));
  gROOT->LoadMacro(HEADER);

  // Cambus Setup
  TCanvas *CurC = new TCanvas("CurC","",1);
  CurC -> SetGridy();
  
  // Handle Plotting Options
  OptionHandler();

  // Initiarizer
  InitScope();

  // postscript file
  Char_t psfile[100];
  sprintf(psfile,"ps/DlayerAnalyzer.ps");
  TPostScript *ps = new TPostScript(psfile,112);

  // Analysis summary data file
  Char_t outfile[100];
  sprintf(outfile,"summary/DlayerAnalyzer.dat");
  fout.open(outfile,ios::out);

  //---------------------------------------------------------------------------//
  //                       main plotting routines                              //
  //         (Odd ID numbers are 1D histograms - flattop only)                 //
  //---------------------------------------------------------------------------//
  BlueAndYellowBeams(10, CurC, ps);   // Fill vs. Deadlayer
  BlueAndYellowBeams(15, CurC, ps);   // Deadlayer Raw (flattop only)
  BlueAndYellowBeams(17, CurC, ps);   // Deadlayer Rate Correction (flattop only)
  BlueAndYellowBeams(19, CurC, ps);   // Deadlayer Raw (flattop only)
  BlueAndYellowBeams(20, CurC, ps);   // Rate vs. Deadlayer
  BlueAndYellowBeams(30, CurC, ps);   // Fill vs. Average t0
  BlueAndYellowBeams(40, CurC, ps);   // Fill vs. Delta_t0
  BlueAndYellowBeams(50, CurC, ps);   // Rate vs. Averega t0
  BlueAndYellowBeams(60, CurC, ps);   // Event Rate History
  BlueAndYellowBeams(70, CurC, ps);   // Fill vs. Active Bunches
  BlueAndYellowBeams(80, CurC, ps);   // Fill vs. Strip DlE
  BlueAndYellowBeams(81, CurC, ps);   // Strip Deadlayer Average Deviation from Detector Average(DlE)
  BlueAndYellowBeams(90, CurC, ps);   // Fitting Energy Range History
  BlueAndYellowBeams(100, CurC, ps);  // Average T0 vs. Deadlayer
  BlueAndYellowBeams(101, CurC, ps);  // Average T0 vs. Deadlayer (zoom)

  // Calculate Systematic Errors
  CalcSystematicError();

  // Output analysis results
  PrintOutput();
  cout << "---" << endl << endl << "analysis summary file : " << outfile << endl;
  fout.close();

  // close ps file
  cout << "ps file               : " << psfile << endl;
  ps->Close();

  // launch ghostview
  gSystem->Exec("gv ps/DlayerAnalyzer.ps");

  return 0;

}


//
// Class name  : DlayerAnalyzer
// Method name : CalcSystematicError()
//
// Description : Calculate Systematic Errors
// Input       : 
// Return      : 
//
Int_t 
DlayerAnalyzer::CalcSystematicError(){

  // Loop i=0:Blue i=1:Yellow
  for (Int_t i=0; i<=1; i++) {
    
    switch (RUN) {
    case 5:
      sys[i].err.run = corr[i].AsignError*corr[i].rad.sigma;
      break;
    case 6:
      sys[i].err.run = corr[i].AsignError*corr[i].no.sigma;
      break;
    default:
      sys[i].err.run = corr[i].AsignError*corr[i].no.sigma;
    }

  }// end-of-for(i) loop

  return 0;

}



//
// Class name  : DlayerAnalyzer
// Method name : PrintOutput
//
// Description : Print out analysis results into output file
// Input       : 
// Return      : 
//
Int_t 
DlayerAnalyzer::PrintOutput(){


  fout << endl;
  fout << "Blue \t\t\t\t\t\t 1-sigma  " << setw(1) << corr[0].AsignError << "-sigma " << endl;

  fout.precision(3);
  for (Int_t i=0; i<=1; i++) {
    if (i==1)   fout << "Yellow     \t\t\t\t\t 1-sigma  " << setw(1) << corr[0].AsignError << "-sigma " << endl;
    fout << " Deadlayer Stability (No Correction)             : " 
	 <<  corr[i].no.sigma   << setw(8) << corr[i].AsignError*corr[i].no.sigma << endl;
    fout << " Deadlayer Stability (Rate Correction)           : " 
	 <<  corr[i].rate.sigma << setw(8) << corr[i].AsignError*corr[i].rate.sigma << endl;
    fout << " Deadlayer Stability (RadiationDmage Correction) : " 
	 <<  corr[i].rad.sigma  << setw(8) << corr[i].AsignError*corr[i].rad.sigma << endl;
    fout << "\t\t\t\t\t\t   Mean" << endl;
    fout << " Average Deviation from Detector Average         : " << sys[i].err.strip << endl;
    fout << " Total Error  (Quadratic Sum)                    : " 
	 << SquareRootSum(sys[i].err.run, sys[i].err.strip) << endl; 
  }


  return 0;

}

