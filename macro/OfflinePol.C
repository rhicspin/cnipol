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
const Int_t MAX_NMEAS_PER_FILL=99;
Int_t RUN=5;


// Bad data point criterias for Fitting
Float_t RATE_FIT_STANDARD_DEVIATION      = 0.2;   // [MHz]
Float_t RATE_FIT_STANDARD_DEVIATION_DATA = 1.5;   // [sigma]
Float_t POLARIZATION_FIT_CHI2            = 5;   
Float_t POLARIZATION_FIT_SIGMA_DATA      = 3;   // [sigma]

Float_t CorrelatedError(Float_t a, Float_t da, Float_t b, Float_t db);

class OfflinePol
{

private:
  ofstream fout;
  struct StructFillByFill {
    Int_t FillID;
    Int_t nRun;
    Float_t Clock0;
    Float_t Clock[MAX_NMEAS_PER_FILL];
    Float_t RunID[MAX_NMEAS_PER_FILL];
    Float_t Index[MAX_NMEAS_PER_FILL];
    Float_t P_online[N];
    Float_t dP_online[N];
    Float_t P_offline[N];
    Float_t dP_offline[N];
    Float_t Rate[N];
    Float_t sRate[N]; // scaled rate for superpose plot on polarization
    Float_t dum[N];
    Float_t Chi2[2];
    Int_t DoF;
    Float_t FitAve[N];
    struct StructBad {
      Float_t Clock[N];
      Float_t P_offline[N];
      Float_t Rate[N];
      struct StructCounter {
	Int_t data;
	Int_t fill;
      } rate, P;
    } bad, good;
    TGraphErrors *meas_vs_P[2];
    TGraphErrors *meas_vs_Rate;
  } fill[N], Fill ;


  struct StructTime {
    Char_t *Week;
    Char_t *Month;
    Int_t Day;
    Char_t *Time;
    Int_t Hour;
    Int_t Minute;
    Int_t Sec;
  }time;

  // scale rate for fill by fill analysis plot
  struct Rate {
    Float_t ymin;
    Float_t ymax;
    Int_t Color;
  } r;

  Int_t Fill[N], nRun[N], Time[N];
  Float_t Energy[N];
  Float_t RunID[N],P_online[N],dP_online[N],P_offline[N],dP_offline[N];
  Float_t phi[N], dphi[N],chi2[N],A_N[N],Rate[N],SpeLumi[N],DiffP[N];
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
  Int_t OfflinePol();
  Int_t GetData(Char_t * DATAFILE);
  // following functions are defined in FillByFill.C
  Int_t FillByFill(Int_t Mode, Int_t RUN, Int_t ndata, Int_t Color, TCanvas *CurC, TPostScript *ps);
  Int_t FillByFillAnalysis(Int_t RUN, Int_t ndata);
  Int_t MakeFillByFillPlot(Int_t nFill, Int_t Mode, Int_t ndata, Int_t Color, TPostScript *ps);
  Int_t FillByFillPlot(Int_t Mode, Int_t k, Int_t Color);
  Int_t TimeDecoder();

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
  P_alt[i]=dP_alt[i]= 0;
  P_offline[i] =  phi[i]  = -9999;
  index[i]=0;

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

      // 31 : process if RunStatus != "N/A-","Junk","Bad","BadP","Tune" 
      if (RunStatusFilter(31, RunStatus)){

	// Skip incomplete lines due to half way running Asym. 
	if (strlen(line)>50) { 

	  MeasType      = strtok(NULL," ");
	  if (MeasTypeFilter(15,MeasType)){

	    Energy[i]     = atof(strtok(NULL," "));
	    time.Week     = strtok(NULL," ");
	    time.Month    = strtok(NULL," ");
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
    cout << ymin << " " << ymax;
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
    //  Mode Selection, see OfflinePol::FillByFillPlot() @ FillByFill.C
    //  Mode += 3  (Offline,Online)
    //  Mode += 7  (Offline,Online,Rate)
    //  Mode += 9  (Offline,fit)
    //  Mode += 13 (Offline,Rate,fit)
    //  Mode += 18 (Rate, Rate_fit)
    FillByFill(Mode+18, RUN, ndata, Color, CurC, ps);
    //    FillByFill(Mode+7, RUN, ndata, Color, CurC, ps);
    //    FillByFill(Mode+9, RUN, ndata, Color, CurC, ps);
    //    FillByFill(Mode+13, RUN, ndata, Color, CurC, ps);
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

Int_t 
OfflinePol::OfflinePol()
{

  // load header macro
  Char_t HEADER[100];
  sprintf(HEADER,"%s/Utility.h",gSystem->Getenv("MACRODIR"));
  gROOT->LoadMacro(HEADER);
  sprintf(HEADER,"%s/SuperposeSummaryPlot.h",gSystem->Getenv("MACRODIR"));
  gROOT->LoadMacro(HEADER);
  sprintf(HEADER,"%s/FillByFill.C",gSystem->Getenv("MACRODIR"));
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
    RunBothBeam(12,  CurC, ps); // onlineP and Linear Fit
    /*
    RunBothBeam(10,  CurC, ps); // onlineP and offlineP vs. RunID
    //    RunBothBeam(15,  CurC, ps); // onlineP and offlineP vs. index
    RunBothBeam(30,  CurC, ps); // 2-sigma/3-sigma ratio vs. RunID
    RunBothBeam(130,  CurC, ps); // 2-sigma/3-sigma ratio Distribution
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
    RunBothBeam(1000,  CurC, ps); // onlineP and offlineP vs. RunID

    // close output file
    cout << "output data file: " << outfile << endl;
    fout.close();

    cout << "ps file : " << psfile << endl;
    ps->Close();


    //    gSystem->Exec("gv ps/OfflinePol.ps");

    return 0;

}


