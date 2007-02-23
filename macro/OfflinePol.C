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

#ifndef __CINT__

#include <getopt.h>
#include "TFile.h"
#include "TH1F.h" 
#include "TH2F.h"
#include "TH2D.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include "TText.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TPostScript.h"
#include "TSystem.h"
#include "TLegend.h"

#endif


#define Debug 0

const Int_t N=2000;
const Int_t MAX_NFILL=500;
const Int_t MAX_JET_RUNTIME_DATA=500;
const Int_t MAX_NMEAS_PER_FILL=99;
const Int_t MAX_NMEAS_PER_PERIOD=1000;
const Int_t MAX_TARGET_PERIOD=10;

Int_t RUN=5;

// Initialization of analysis options
Int_t FILL=0;
Int_t FILL_BY_FILL_ANALYSIS=1;
Int_t OFFLINE_POL=1;

// Bad data point criterias for Fitting
Float_t RATE_FIT_STANDARD_DEVIATION      = 0.2;   // [MHz]
Float_t RATE_FIT_STANDARD_DEVIATION_DATA = 1.5;   // [sigma]
Float_t RATE_DROP_ALLOWANCE              = 0.6;   // Maximum rate drop factor
Float_t REFERENCE_RATE_DROP_ALLOWANCE    = 2;     // 3 [sigmas] from reference rate
Float_t POLARIZATION_FIT_CHI2            = 5;   
Float_t POLARIZATION_FIT_SIGMA_DATA      = 3;     // [sigma]

Float_t CorrelatedError(Float_t a, Float_t da, Float_t b, Float_t db);


// Jet Period Setting for Run05
const Int_t nJetRun5=19;
Int_t JetRun5[nJetRun5+1]     = {6945, 6973, 6975, 6980, 6988, 6988, 
				 7007, 7046, 7088, 7114, 7120, 7139, 
				 7172, 7224, 7263, 7300, 7303, 7317, 
				 7319, 7328};
const Int_t JetRun5nType=8;
Int_t JetRun5Type[nJetRun5]   = {   0,    2,    0,    2,    0,    2,
				    3,    3,    4,    5,    3,    5,
				    7,    5,    3,    5,    5,    6,
				    3};

// Target period
Float_t BlueTargetRun5[MAX_TARGET_PERIOD]  = {6789.001,
					      7028.001,  // Vertical Background
					      7255.004,  // Vertical Target-3
					      7328.001}; // Vertical Target-1
Float_t YellowTargetRun5[MAX_TARGET_PERIOD]= {6789.101,	
					      6976.101,  // Vertical Target-2
					      6997.106,  // Vertical Background
					      6997.110,  // Vertical Target-2
					      7051.101,  // Vertical Target-3
					      7052.101,  // Vertical Target-2
					      7328.101}; // Vertical Target-1



//
// Class OfflinePol
//
class OfflinePol
{

private:
  ofstream fout;
  struct StructFillByFill {
    Int_t FillID;
    Int_t nRun;
    Float_t Clock0; // The time stamp of the first measurement in store
    Float_t Clock[MAX_NMEAS_PER_FILL];  // time duration from the first measurement in [h]
    Float_t ClockM[MAX_NMEAS_PER_FILL]; // time stamp for horizontal error bar in [h] 
    Float_t Time[MAX_NMEAS_PER_FILL];   // time stamp of the measurement 
    Float_t dT[MAX_NMEAS_PER_FILL];     // time interval between k+1 and k data point 
    Float_t dt[MAX_NMEAS_PER_FILL];     // 1/2 time interval between k+1 and k-1 data point for "k"th data point. 
    Float_t dt_err[MAX_NMEAS_PER_FILL]; // divided dt[k] by half for horizontal error bar plotting purpose
    Float_t RunID[MAX_NMEAS_PER_FILL];
    Float_t Index[MAX_NMEAS_PER_FILL];
    Float_t P_online[MAX_NMEAS_PER_FILL];
    Float_t dP_online[MAX_NMEAS_PER_FILL];
    Float_t P_offline[MAX_NMEAS_PER_FILL];
    Float_t dP_offline[MAX_NMEAS_PER_FILL];
    Float_t Weight[MAX_NMEAS_PER_FILL];  // weight factor for fill by fill average
    Float_t Rate[MAX_NMEAS_PER_FILL];
    Float_t sRate[MAX_NMEAS_PER_FILL]; // scaled rate for superpose plot on polarization
    Float_t RateRest[MAX_NMEAS_PER_FILL];
    Float_t WCM[MAX_NMEAS_PER_FILL];
    Float_t sWCM[MAX_NMEAS_PER_FILL]; // scaled Wall current monitor for superpose plot
    Float_t dum[MAX_NMEAS_PER_FILL];
    Float_t Chi2[2];
    Int_t DoF;
    Float_t FitAve[MAX_NMEAS_PER_FILL];
    Float_t wAve[2];  // [0] : weighted average [1]: error
    struct StructBad {
      Float_t Clock[MAX_NMEAS_PER_FILL];
      Float_t P_offline[MAX_NMEAS_PER_FILL];
      Float_t Rate[MAX_NMEAS_PER_FILL];
      struct StructCounter {
	Int_t data;
	Int_t fill;
      } rate, P;
    } bad, good;
    TGraphErrors *meas_vs_P[2];
    TGraphErrors *meas_vs_Rate;
  } fill[N], Fill ;

  struct PeroidByPeriod {
    Int_t PeriodID;
    Int_t nPeriod;
    Int_t nRun;
    Int_t Begin_FillID;
    Int_t End_FillID;
    Int_t Type;
    Int_t nType;
    Int_t flag;
    Float_t Time[MAX_NMEAS_PER_PERIOD]; // time stamp of the measurement in [s]
    Float_t Clock0; // The time stamp of the first measurement in store
    Float_t Clock[MAX_NMEAS_PER_PERIOD]; // time duration from the first measurement in [h]
    Float_t ClockM[MAX_NMEAS_PER_PERIOD]; // time duration from the first measurement in [h]
    Float_t RunID[MAX_NMEAS_PER_PERIOD];
    Float_t P_online[MAX_NMEAS_PER_PERIOD];
    Float_t dP_online[MAX_NMEAS_PER_PERIOD];
    Float_t P_offline[MAX_NMEAS_PER_PERIOD];
    Float_t dP_offline[MAX_NMEAS_PER_PERIOD];
    Float_t Weight[MAX_NMEAS_PER_PERIOD];
    Float_t dt[MAX_NMEAS_PER_PERIOD];   // interval time between consequtive measurements in [sec]
    Float_t dt_err[MAX_NMEAS_PER_PERIOD];   // dt divided by 2 for horizontal error bar plotting purpose
    Float_t WCM[MAX_NMEAS_PER_PERIOD];
    Float_t sWCM[MAX_NMEAS_PER_PERIOD];
    Float_t dum[MAX_NMEAS_PER_PERIOD];
    Float_t wAve[2];
    TGraphErrors *meas_vs_P[2];
    TGraphErrors *meas_vs_WCM;
    Float_t pC_Ave[2][3][2];
    Int_t fill_id[MAX_NFILL];
    Float_t fill_ave[MAX_NFILL];
    Float_t fill_dave[MAX_NFILL];
    Float_t fill_ave_t[MAX_NFILL];
    Int_t nfill;
    Float_t t_jet[MAX_NFILL];
  } Period, period[MAX_NMEAS_PER_PERIOD] ;


  struct StructBeam {
    struct TargetByTarget {
      Int_t nPeriod;
      Float_t Begin_RunID;
      Float_t End_RunID;
      Float_t Type;
      Float_t Mean;
      Float_t Sigma;
      Float_t Threshold;
      TH1F * UniversalRate;
    } Target, target[MAX_TARGET_PERIOD];
  } blue, yellow;


  struct StructFlag {
    Int_t UniversalRate;
  } flag;

  struct StructTime {
    Char_t *Week;
    Char_t *Month_c;
    Int_t Day;
    Char_t *Time;
    Int_t Month;
    Int_t Hour;
    Int_t Minute;
    Int_t Sec;
  }time;

  // Structure for jet runtime data
  struct Jet {
    Int_t ndata;
    Int_t FillID[MAX_JET_RUNTIME_DATA];
    Int_t start[MAX_JET_RUNTIME_DATA];
    Int_t stop[MAX_JET_RUNTIME_DATA];
    Int_t dt[MAX_JET_RUNTIME_DATA];
    Float_t start_time[MAX_JET_RUNTIME_DATA];
    Float_t RunID[MAX_JET_RUNTIME_DATA];
  } jet;

  // scale rate for fill by fill analysis plot
  struct Rate {
    Float_t ymin;
    Float_t ymax;
    Int_t Color;
  } r;

  // scale wcm for fill by fill analysis plot
  struct WCM {
    Float_t ymin;
    Float_t ymax;
    Int_t Color;
  } wcm;

  Int_t nRun[N], Time[N];
  Float_t Energy[N];
  Float_t RunID[N],P_online[N],dP_online[N],P_offline[N],dP_offline[N];
  Float_t phi[N], dphi[N],chi2[N],A_N[N],Rate[N],WCM[N],SpeLumi[N],DiffP[N];
  Float_t P_alt[N],dP_alt[N],R_Preg_Palt[N],dR_Preg_Palt[N];
  Float_t index[N],dx[N],dy[N];
  TH2D* frame ;
  TH1D* Pdiff ;
  TH1D* phiDist;
  TH1D* sfitchi2;

public:
  void Initiarization(Int_t);
  Int_t Plot(Int_t, Int_t, Int_t, Char_t *, Int_t, TLegend *aLegend);
  Int_t PlotControlCenter(Char_t*, Int_t, TCanvas *CurC,  TPostScript *ps);
  Int_t RunBothBeam(Int_t Mode, TCanvas *CurC,  TPostScript *ps);
  Int_t DrawFrame(Int_t Mode,Int_t ndata, Char_t*);
  Int_t GetData(Char_t * DATAFILE);
  Int_t OfflinePol();
  // following functions are defined in FillByFill.C
  Int_t FillByFill(Int_t Mode, Int_t RUN, Int_t ndata, Int_t Color, TCanvas *CurC, TPostScript *ps);
  Int_t SingleFillPlot(Int_t Mode, Int_t RUN, Int_t ndata, Int_t FillID, Int_t Color);
  Int_t FillByFillAnalysis(Int_t RUN, Int_t ndata);
  Int_t PrintFillByFillArray(Int_t i, Int_t j, Int_t array_index);
  Int_t MakeFillByFillPlot(Int_t nFill, Int_t Mode, Int_t ndata, Int_t Color, TPostScript *ps);
  Int_t FillByFillPlot(Int_t Mode, Int_t k, Int_t Color);
  Int_t RateFilter(Int_t k, Int_t Mode, Float_t xmin, Float_t xmax, Color);
  Float_t GetDropRate(Int_t k, Int_t i, Int_t &iMax);
  Float_t GetDropRate(Int_t k, Int_t i);
  Float_t GetDropRate(Int_t k, Int_t i, Int_t Color, Float_t &RefRate);
  Int_t TimeDecoder();
  Int_t TargetByTarget(Int_t k, Int_t Color);
  Int_t TargetByTargetUniversalRate(Int_t nFill, Int_t Color, TPostScript *ps);

  // following functions are defined in PeriodByPeriod.C
  Int_t PeroidByPeriod(Int_t Mode, Int_t RUN, Int_t ndata, Char_t *Beam, Int_t Color, TCanvas *CurC, TPostScript *ps);
  Int_t PeriodByPeriodAnalysis(Int_t RUN, Int_t nFill);
  Int_t PrintPeriodByPeriodArray(Int_t i, Int_t j, Int_t array_index);
  Int_t MakePeriodByPeriodPlot(Int_t nPeriod, Int_t Mode, Int_t Color, TPostScript *ps);
  Int_t PeriodByPeriodFillAverage(Int_t Mode, Int_t k, Int_t Color, Float_t Ave[]);
  Float_t PeriodByPeriodPlot(Int_t Mode, Int_t k, Int_t Color, Float_t Ave[]);
  Int_t GetJetRunTime(Char_t *Beam);
  Int_t JetComparison(Int_t nPeriod, Int_t Mode, Int_t Color, TPostScript *ps);
  Int_t PlotJet(Int_t Beam, Int_t index, Float_t xmin);
  Int_t Normalization(Int_t nPeriod, Int_t Mode, Int_t Color, TPostScript *ps);
  Int_t CalcWeightedMean();

}; // end-class Offline


//
// Class name  : OfflinePol
// Method name : void Initiarization()
//
// Description : Initiarize Array
// Input       : array index i
// Return      : 
//
void 
OfflinePol::Initiarization(Int_t i){
  
  
  RunID[i] = P_online[i] = dP_online[i]= dP_offline[i] = dphi[i] = dx[i] = dy[i] = 0;
  Rate[i] = WCM[i] = 0;
  P_alt[i] = dP_alt[i] = 0;
  P_offline[i] =  phi[i]  = -9999;
  index[i]=0;

  jet.ndata=0;

  // flag
  flag.UniversalRate = 0;

  return;

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
                 
  //define histograms
  Pratio   = new TH1D("Pratio",   "Offline Pol Event Selection Dependence",60,0.5,1.5);
  Pdiff   = new TH1D("Pdiff",   "Online/Offline Polarization Consistency",60,-20,20);
  phiDist = new TH1D("phiDist", "phi angle distribution",60,-35,35);
  sfitchi2= new TH1D("sfitchi2","chi2 distribution of P*sin(phi) fit",30,0,4);

    ifstream fin;
    fin.open(DATAFILE,ios::in);
    printf("datafile=%s\n",DATAFILE);

    if (fin.fail()){
        cerr << "ERROR: " << DATAFILE << " doesn't exist. Force exit." << endl;
        exit(-1);
    }

    Char_t buffer[300], line[300];
    Char_t *RunStatus, *MeasType, *target, *tgtOp, *dum[6];
    Int_t i=0;
    Int_t ch=0;
    while ( ( ch = fin.peek()) != EOF ) {
      
      fin.getline(buffer, sizeof(buffer), '\n'); 
      sprintf(line,buffer);
      // skip header if DATAFILE starts from "==========="
      if (strstr(buffer,"=====")) {
	for (int j=0; j<4; j++) fin.getline(buffer, sizeof(buffer), '\n'); 
      }
      Initiarization(i);

      // main read-in routine
      RunID[i]     = atof(strtok(buffer," "));
      P_online[i]  = atof(strtok(NULL," "));
      dP_online[i] = atof(strtok(NULL," "));
      RunStatus    = strtok(NULL," ");

      // 31 : mask RunStatus == "N/A-","Junk","Bad","BadP","Tune" 
      // 19 : mask RunStatus == "N/A-","Junk","Tune" 
      if (RunStatusFilter(31, RunStatus)){
      //      if (RunStatusFilter(19, RunStatus)){

	// Skip incomplete lines due to half way running Asym. 
	if (strlen(line)>50) { 

	  MeasType      = strtok(NULL," ");
	  // 15 : mask MeasType == "PROF","TUNE","PHYS"
	  if (MeasTypeFilter(15,MeasType)){

	    Energy[i]     = atof(strtok(NULL," "));
	    time.Week     = strtok(NULL," ");
	    time.Month_c  = strtok(NULL," ");
	    time.Day      = atoi(strtok(NULL," "));
	    time.Time     = strtok(NULL," ");

	    P_offline[i]  = atof(strtok(NULL," "));
	    dP_offline[i] = atof(strtok(NULL," "));
	    phi[i]        = atof(strtok(NULL," "));
	    dphi[i]       = atof(strtok(NULL," "));
	    chi2[i]       = atof(strtok(NULL," "));
	    A_N[i]        = atof(strtok(NULL," "));
	    target        = strtok(NULL," ");
	    tgtOp         = strtok(NULL," ");
	    Rate[i]       = atof(strtok(NULL," "));
	    WCM[i]        = atof(strtok(NULL," "));
	    SpeLumi[i]    = atof(strtok(NULL," "));
	    DiffP[i]      = atof(strtok(NULL," "));
	    P_alt[i]      = atof(strtok(NULL," "));
	    dP_alt[i]     = atof(strtok(NULL," "));

	    // Time decorder should be at the end of buffer read loop
	    Time[i] = TimeDecoder();

	    R_Preg_Palt[i] = P_offline[i] ? P_alt[i]/P_offline[i] : 0;
	    dR_Preg_Palt[i]= CorrelatedError(P_alt[i],dP_alt[i],P_offline[i],dP_offline[i]);

	    // overflow
	    if (fabs(DiffP[i])>25) DiffP[i]=25;

	    // fill 1-dim histograms
	    Pratio->Fill(R_Preg_Palt[i]);
	    Pdiff->Fill(DiffP[i]);
	    phiDist->Fill(phi[i]);
	    sfitchi2->Fill(chi2[i]);

	    index[i]=i; ++i; 

	  } // end-of-if(MeasTypeFilter()

	} // end-of-if(strlen(50)

      } // end-of-if(strcmp(RunStatus,"Junk")*strcmp(RunStatus,"N/A-");

      if (i>N-1){
          cerr << "WARNING : input data exceed the size of array " << N << endl;
          cerr << "          Ignore beyond line " << N << endl;
          break;
      } // if-(i>N)

    }// end-of-while(!fin.eof())

    // Run-5, Run-6, Run-7....
    if (RunID[0]>7400) RUN=6;

    fin.close();
    cout << "Total run =" << i-1 << endl;

    return i-1;

}


Float_t 
CorrelatedError(Float_t a, Float_t da, Float_t b, Float_t db){
  return b ? a/b*sqrt((da/a)*(da/a)-(db/b)*(db/b)) : 0;
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
  case 22:
    TGraphErrors* tgae = new TGraphErrors(ndata, RunID, P_offline, dx, dP_offline);
    TF1 * f = new TF1("f","pol0");
    f->SetLineColor(2);
    tgae->Fit(f);
    break;
  case 25:
    TGraphErrors* tgae = new TGraphErrors(ndata, index, P_offline, dx, dP_offline);
    break;
  case 30:
    TGraphErrors* tgae = new TGraphErrors(ndata, RunID, R_Preg_Palt, dx, dR_Preg_Palt);
    break;
  case 50:
    TGraphErrors* tgae = new TGraphErrors(ndata, RunID, DiffP, dx, dy);
    break;
  case 60:
    TGraphErrors* tgae = new TGraphErrors(ndata, RunID, phi, dx, dphi);
    break;
  case 65:
    TGraphErrors* tgae = new TGraphErrors(ndata, index, phi, dx, dphi);
    break;
  case 70:
    TGraphErrors* tgae = new TGraphErrors(ndata, phi, P_offline, dphi, dP_offline);
    break;
  case 80:
    TGraphErrors* tgae = new TGraphErrors(ndata, chi2, phi, dy, dphi);
    break;
  case 90:
    TGraphErrors* tgae = new TGraphErrors(ndata, RunID, SpeLumi, dx, dy);
    break;
  case 100:
    TGraphErrors* tgae = new TGraphErrors(ndata, RunID, A_N, dx, dy);
    break;
  }

  tgae -> SetMarkerStyle(Mtyp);
  tgae -> SetMarkerSize(1.5);
  tgae -> SetLineWidth(2);
  tgae -> SetMarkerColor(Color);
  tgae -> Draw("P");
   
  if ( Mode < 50 ) {
      aLegend->AddEntry(tgae,text,"P");
      aLegend->Draw("same");
  }

  return 0;

} // end-of-DlayerPlot()




//
// Class name  : Offline
// Method name : DrawFrame()
//
// Description : Draw TH2D frame before plotting
// Input       : Int_t Mode, Int_t ndata, Char_t *Beam, Char_t subtitle[]
// Return      : 0
//
Int_t 
OfflinePol::DrawFrame(Int_t Mode, Int_t ndata, Char_t *Beam, Char_t subtitle[]){

  Float_t xmin, xmax, ymin, ymax;
  Float_t margin=0.05;

  Char_t title[100];
  sprintf(title,"Polarization (%s) %s", Beam, subtitle);

  ymin=-70 ; ymax=-20;
  if (RUN==6) {ymin=20; ymax=80;}

  // determine xmin, xmax, ymin, ymax of frame
  switch (Mode) {
  case 10:
    GetScale(RunID, ndata, margin, xmin, xmax);
    Char_t xtitle[100]="Fill Number";
    Char_t ytitle[100]="Polarization [%]";
    break;
  case 12:
    GetScale(RunID, ndata, margin, xmin, xmax);
    Char_t xtitle[100]="Fill Number";
    Char_t ytitle[100]="Polarization [%]";
    break;
  case 15:
    GetScale(index, ndata, margin, xmin, xmax);
    Char_t xtitle[100]="Index";
    Char_t ytitle[100]="Polarization [%]";
    break;
  case 30:
    GetScale(RunID, ndata, margin, xmin, xmax);
    ymin=0.5;   ymax=+1.5;
    Char_t xtitle[100]="Fill Number";
    Char_t ytitle[100]="2-sigma/3-sigma Ratio";
    sprintf(title,"Offline Pol Event Selection Dependence %s", Beam, subtitle);
    break;
  case 50:
    GetScale(RunID, ndata, margin, xmin, xmax);
    ymin=-10; ymax=10; 
    if (RUN==5) {ymin=-25;ymax=25;};
    Char_t xtitle[100]="Fill Number";
    Char_t ytitle[100]="P_online-P_offline";
    sprintf(title,"Online/Offline Polarization Diff.(%s) %s",Beam, subtitle);
    break;
  case 60:
    GetScale(RunID, ndata, margin, xmin, xmax);
    ymin=-35; ymax=35;
    Char_t xtitle[100]="Fill Number";
    Char_t ytitle[100]="phi angle [deg]";
    sprintf(title,"phi angle (%s) %s", Beam, subtitle);
    break;
  case 65:
    GetScale(index, ndata, margin, xmin, xmax);
    ymin=-35; ymax=35;
    Char_t xtitle[100]="Index";
    Char_t ytitle[100]="phi angle [deg]";
    sprintf(title,"phi angle (%s) %s", Beam, subtitle);
    break;
  case 70:
    xmin=-30; xmax=30;
    Char_t xtitle[100]="phi angle [deg]";
    Char_t ytitle[100]="P_offline [%]";
    sprintf(title,"P vs. phi angle %s",Beam, subtitle);
    break;
  case 80:
    xmin=0; xmax=4;
    ymin=-40; ymax=40;
    Char_t xtitle[100]="chi2 of P*sin(phi) fit";
    Char_t ytitle[100]="phi angle [deg]";
    sprintf(title,"Chi2 vs. phi %s", Beam, subtitle);
    break;
  case 90:
    GetScale(RunID, ndata, margin, xmin, xmax);
    GetScale(SpeLumi, ndata, margin, ymin, ymax);
    Char_t xtitle[100]="Fill Number";
    Char_t ytitle[100]="Specific Luminosity sigma";
    sprintf(title,"Specific Luminosity Sigma vs. Fill Number %s", Beam, subtitle);
    break;
  case 100:
    GetScale(RunID, ndata, margin, xmin, xmax);
    GetScale(A_N, ndata, margin, ymin, ymax);
    Char_t xtitle[100]="Fill Number";
    Char_t ytitle[100]="A_N";
    sprintf(title,"Energy Spectrum Weighted Average A_N %s", Beam, subtitle);
    break;
  }


  // draw frames
  frame = new TH2D(Beam,title, 10, xmin, xmax, 10, ymin, ymax);
  frame -> SetStats(0);
  frame -> GetXaxis()->SetTitle(xtitle);
  frame -> GetYaxis()->SetTitle(ytitle);
  frame -> Draw();

  // Superpose some beam operational comments on the frame 
  SuperposeComments(Mode, frame);

  return 0;

}


//
// Class name  : Offline
// Method name : PlotControlCenter
//
// Description : Control Center of all plots. 
// Input       : Char_t *Beam, Int_t Mode, TCanvas *CurC, TPostScript *ps
// Return      : 0
//
Int_t 
OfflinePol::PlotControlCenter(Char_t *Beam, Int_t Mode, TCanvas *CurC, TPostScript *ps){

  Int_t Color = Beam == "Blue" ? 4 : 94 ;

  Char_t DATAFILE[256];
  sprintf(DATAFILE,"summary/OfflinePol_%s.dat",Beam);
  Int_t ndata = GetData(DATAFILE);

  Int_t FILL[2];
  FILL[0] = int(RunID[0]);
  FILL[1] = int(RunID[ndata]);
  Char_t subtitle[50]; 
  sprintf(subtitle,"[Fill#%d - %d]",FILL[0],FILL[1]);

  if (Mode<100) DrawFrame(Mode, ndata, Beam, subtitle);


  Char_t ytitle[100];
  sprintf(ytitle,"# of Runs %s",subtitle);
  TLegend * aLegend = new TLegend(0.7,0.15,0.85,0.3);

  switch (Mode) {
  case 10:
      Plot(Mode,    ndata, 24, "Online",  Color, aLegend);
      Plot(Mode+10, ndata, 20, "Offline", Color, aLegend);
      break;
  case 12:
    gStyle->SetOptFit(111);
      Plot(Mode+10, ndata, 20, "Offline", Color, aLegend);
      break;
  case 15:
      Plot(Mode,    ndata, 24, "Online",  Color, aLegend);
      Plot(Mode+10, ndata, 20, "Offline", Color, aLegend);
     break;
  case 30:
      Plot(Mode,    ndata, 20, " ",  Color, aLegend);
      break;
  case 130:
      Pratio->SetXTitle("P(2sigma/3sigma)");
      Pratio->SetYTitle(ytitle);
      Pratio->SetFillColor(Color);
      Pratio->Draw();
      break;
  case 50:
      Plot(Mode,    ndata, 20, " ",  Color, aLegend);
      break;
  case 150:
      Pdiff->SetXTitle("Poffline - Ponline [%]");
      Pdiff->SetYTitle(ytitle);
      Pdiff->SetFillColor(Color);
      Pdiff->Draw();
      break;
  case 60:
      Plot(Mode,    ndata, 20, " ",  Color, aLegend);
      break;
  case 65:
      Plot(Mode,    ndata, 20, " ",  Color, aLegend);
      break;
  case 160:
      phiDist->SetXTitle("phi angle [deg.]");
      phiDist->SetYTitle(ytitle);
      phiDist->SetFillColor(Color);
      phiDist->Draw();
      break;
  case 70:
      Plot(Mode,    ndata, 20, " ",  Color, aLegend);
      break;
  case 80:
      Plot(Mode,    ndata, 20, " ",  Color, aLegend);
      break;
  case 90:
      Plot(Mode,    ndata, 20, " ",  Color, aLegend);
      break;
  case 100:
      Plot(Mode,    ndata, 20, " ",  Color, aLegend);
      break;
  case 180:
      sfitchi2->SetXTitle("sin(phi) fit chi2");
      sfitchi2->SetYTitle(ytitle);
      sfitchi2->SetFillColor(Color);
      sfitchi2->Draw();
      break;
  case 1000:
    // Target by target period allocation
    if (RUN==5){
      for (Int_t i=0; i<MAX_TARGET_PERIOD; i++){
	if (BlueTargetRun5[i]==0) {blue.Target.nPeriod=i-1; break;}
	blue.target[i].Begin_RunID = BlueTargetRun5[i];
	blue.target[i].End_RunID   = BlueTargetRun5[i+1];
	blue.target[i].Type        = i;
      }
      for (Int_t i=0; i<MAX_TARGET_PERIOD; i++){
	if (YellowTargetRun5[i]==0) {yellow.Target.nPeriod=i-1; break;}
	yellow.target[i].Begin_RunID = YellowTargetRun5[i];
	yellow.target[i].End_RunID   = YellowTargetRun5[i+1];
	yellow.target[i].Type        = i;
      }
    } else if (RUN==6) {
      cerr << "Targett info for Run06 is not impremented yet" << endl;
    };
    //  Mode Selection, see OfflinePol::FillByFillPlot() @ FillByFill.C
    //  Mode += 3  (Offline,Online)
    //  Mode += 7  (Offline,Online,Rate)
    //  Mode += 9  (Offline,fit)
    //  Mode += 13 (Offline,fit,Rate)
    //  Mode += 18 (Rate, Rate_filter)
    //  Mode += 32 (Rate fileter with universal rate target by target) 
    //  Mode += 64 (Fill by Fill average)
    //    FillByFill(Mode+18, RUN, ndata, Color, CurC, ps);
    //    FillByFill(Mode+7, RUN, ndata, Color, CurC, ps);
    //    FillByFill(Mode+9, RUN, ndata, Color, CurC, ps);
    //    FillByFill(Mode+13, RUN, ndata, Color, CurC, ps);
    //    FillByFill(Mode+32, RUN, ndata, Color, CurC, ps);
    FillByFill(Mode+64+1, RUN, ndata, Color, CurC, ps);
    break;
  case 1100:
    SingleFillPlot(Mode+9, RUN, ndata, 7272, Color);
    break;
  case 2000:
    if (RUN==5){
      Period.nPeriod  = nJetRun5;
      Period.nType    = JetRun5nType;
      for (Int_t i=0; i<Period.nPeriod; i++){
	period[i].Begin_FillID = JetRun5[i];
	period[i].End_FillID   = JetRun5[i+1];
	period[i].Type         = JetRun5Type[i];
      }
    } else {
      cerr << "Jet info for Run06 is not impremented yet" << endl;
    }

    PeroidByPeriod(Mode, RUN, ndata, Beam, Color, CurC, ps);
    //    PeroidByPeriod(Mode+128, RUN, ndata, Beam, Color, CurC, ps); // period by period (Diagnose purpose)
    break;
  }

  return 0;

} // End-of-PlotControlCenter()



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


  PlotControlCenter("Blue",Mode, CurC, ps);
  CurC -> Update();
  if (frame) frame->Delete();

  ps->NewPage();


  PlotControlCenter("Yellow",Mode, CurC, ps);
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


#ifndef __CINT__
Int_t main(int argc, char **argv) {

  int opt, option_index = 0;
  static struct option long_options[] = {
    {"fill-by-fill", 1, 0, 'f'},
    {"fill", 0, 0, 'F'},
  };

  while (EOF != (opt = getopt_long (argc, argv, "fh?xFg", long_options, &option_index))) {
    switch (opt) {
    case -1:
      break;
    case 'f':
      FILL_BY_FILL_ANALYSIS=1;
      OFFLINE_POL=0;
      break;
    case 'F':
      FILL=atoi(optarg);
      OFFLINE_POL=0;
      break;
    case 'g':
      GHOSTVIEW=1;
      break;
    case 'h':
    case '?':
    case '*':
      Usage(argv);
      break;
    }
  }

#else
Int_t OfflinePol::OfflinePol() {
#endif


  // load header macro
  Char_t HEADER[100];
  sprintf(HEADER,"%s/Utility.h",gSystem->Getenv("MACRODIR"));
  gROOT->LoadMacro(HEADER);
  sprintf(HEADER,"%s/SuperposeSummaryPlot.h",gSystem->Getenv("MACRODIR"));
  gROOT->LoadMacro(HEADER);
  sprintf(HEADER,"%s/FillByFill.C",gSystem->Getenv("MACRODIR"));
  gROOT->LoadMacro(HEADER);
  sprintf(HEADER,"%s/PeriodByPeriod.C",gSystem->Getenv("MACRODIR"));
  gROOT->LoadMacro(HEADER);

    // Cambus Setup
    TCanvas *CurC = new TCanvas("CurC","",1);
    CurC -> SetGridy();

    // postscript file
    Char_t psfile[100];
    sprintf(psfile,"ps/OfflinePol.ps");
    TPostScript *ps = new TPostScript(psfile,112);

    // ==============================================================
    //                   Run By Run Analysis
    // ==============================================================
    /*
    RunBothBeam(12,  CurC, ps); // onlineP and Linear Fit
    RunBothBeam(10,  CurC, ps); // onlineP and offlineP vs. RunID
    //    RunBothBeam(15,  CurC, ps); // onlineP and offlineP vs. index
    RunBothBeam(30,  CurC, ps); // reg-sigma/alternative-sigma ratio vs. RunID
    RunBothBeam(130,  CurC, ps); // reg-sigma/alternative-sigma ratio Distribution
    RunBothBeam(50,  CurC, ps); // onlineP-offlineP vs. RunID
    RunBothBeam(150, CurC, ps); // onlineP-offlineP Distribution
    RunBothBeam(60,  CurC, ps); // phi-angle vs. RunID
    RunBothBeam(160, CurC, ps); // phi-angle Distribution
    //    RunBothBeam(65,  CurC, ps); // phi-angle vs. index
    RunBothBeam(70,  CurC, ps); // offline P vs. phi-angle 
    RunBothBeam(80,  CurC, ps); // phi-angle vs. chi2
    RunBothBeam(180, CurC, ps); // sin(phi) fit chi2 Distribution
    RunBothBeam(90,  CurC, ps); // Specific Luminosity  vs. RunID
    //    RunBothBeam(100, CurC, ps); //A_N vs. RunID
*/
    cout << "ps file : " << psfile << endl;
    ps->Close();
    CurC->Clear();


    // ==============================================================
    //                   Fill by Fill Analysis
    // ==============================================================
    sprintf(psfile,"ps/FillByFill.ps");
    TPostScript *ps = new TPostScript(psfile,112);

    Char_t outfile[100]; 
    sprintf(outfile,"summary/FillByFill.dat");
    fout.open(outfile,ios::out);
    RunBothBeam(1000,  CurC, ps); // Fill By Fill Analysis
    //    RunBothBeam(1100,  CurC, ps); // Single Fill Plot

    // close output file
    cout << "output data file: " << outfile << endl;
    fout.close();
    cout << "ps file : " << psfile << endl;
    ps->Close();

    // ==============================================================
    //                   Period by Period Analysis
    // ==============================================================
    sprintf(psfile,"ps/PeriodByPeriod.ps");
    TPostScript *ps = new TPostScript(psfile,112);

    RunBothBeam(2000, CurC, ps); // period by period (Jet Run Type combined)
   
    Char_t outfile[100]; 
    sprintf(outfile,"summary/PeriodByPeriod.dat");
    fout.open(outfile,ios::out);
    cout << "output data file: " << outfile << endl;
    fout.close();
    cout << "ps file : " << psfile << endl;
    ps->Close();


    //    gSystem->Exec("gv ps/OfflinePol.ps");

    return 0;

}


