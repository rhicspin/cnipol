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
const Int_t NSTRIP=72;
const Int_t NSTRIP_PER_DETECTOR=12;
const Int_t NDETECTOR=6;

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
    Float_t StripID[N][NSTRIP];
    Float_t dummy[N];
    Float_t Slope[NSTRIP][N];
    Float_t SlopeE[NSTRIP][N];
    Float_t sinphi_P;
    Float_t sinphi_dPhi;
    Float_t sinphi_sigma[NSTRIP][N];
    Float_t sinphi_sigmaE[NSTRIP][N];
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
  TH2D * BadStripIndex;
  TH2F * strip_slope_history[NSTRIP];
  TH2F * sinphi_fit_sigma_st[NSTRIP];

public:
  Int_t Initiarize();
  Int_t OverflowControl(Int_t index);
  Int_t GetData(Char_t * DATAFILE);
  void  DrawFrame(Int_t Mode, Int_t ndata);
  Int_t MakePlots(Int_t Mode, TCanvas * CurC, TPostScript * ps);
  Int_t MakePlots(Char_t *Beam, TCanvas * CurC, TPostScript * ps);
  Int_t Plot(Int_t Mode, Int_t ndata, Int_t Color);
  Int_t GetDataAndPlot(Int_t Mode, Char_t * Beam, Int_t Color);
  Int_t RootFileOperation(Int_t ndata, Char_t * Beam);
  Int_t RootFileOperation2(Int_t ndata, Char_t * Beam);
  Int_t IndividualStripHistory(Int_t ndata, Char_t *Beam, Int_t Color);
  Int_t IndividualStripHistorySinPhi(Int_t ndata, Char_t *Beam, Int_t Color);

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
  overflow.MassDev=3.0;
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
// Class name  : 
// Method name : RootFileOperation(Int_t ndata, Char_t * Beam)
//             :
// Description : Extract informations from histograms in root file
// Input       : Int_t ndata, Char_t * Beam
// Return      : 
//
Int_t
ErrorDetector::RootFileOperation(Int_t ndata, Char_t * Beam){

  Char_t rootfile[100], text[100], histname[100], htitle[100];

  printf("\n");

  // loop over runs
  for (Int_t i=0; i<ndata; i++) {
    
    sprintf(rootfile,"%8.3f.root",data.RunID[i]);
    printf("%s : %d %8.3f \r",Beam, i, data.RunID[i]);
    printf("%8.3f",data.RunID[i]);

    // create symboric link
    sprintf(text,"ln -s root/%s %s",rootfile,rootfile);
    gSystem->Exec(text);

    TFile * rfile = TFile::Open(rootfile);

    // Get sin(phi) fitting function from asym_sinphi_fit histogram
    Asymmetry->cd();
    data.strip.sinphi_P = data.strip.sinphi_dPhi = 0;
    if (gDirectory->Get("asym_sinphi_fit")) {
      /*
      if (asym_sinphi_fit) cout << "--" << endl;
      if (asym_sinphi_fit->GetFunction("tg")){
	TGraphErrors *tg = (TGraphErrors*)asym_sinphi_fit->GetFunction("tg");
	if (tg){
	  if ((tg->GetX())&&(tg->GetY())&&(tg->GetEY())) {
	    Double_t  *x = tg->GetX();
	    Double_t  *y = tg->GetY();
	    Double_t *ey = tg->GetEY();
	    if (tg->GetFunction("sin_phi")) {
	      TF1 * sin_phi = (TF1*)tg->GetFunction("sin_phi");
	      if (sin_phi){ 
		data.strip.sinphi_P    = sin_phi->GetParameter("P");
		data.strip.sinphi_dPhi = sin_phi->GetParameter("dPhi");
	      }
	    }
	  }
	}
      }
    */
    }
    cout << "..done" << endl;

    // loop over strips
    for (Int_t st=0; st<NSTRIP; st++) {
      data.strip.Slope[st][i]=data.strip.SlopeE[st][i]=0;

      // calculate diviation (in the unit of sigma) of each strips from fitted sin(phi) curve.
      /*
      if (ey[st]){
	  data.strip.sinphi_sigma[st][i]=(y[st]-data.strip.sinphi_P*sin(-x[st]+data.strip.sinphi_dPhi))/ey[st] ;
	  data.strip.sinphi_sigmaE[st][i]=ey[st];
	  }*/

      // extract slope from mass_vs_energy_corr_st histogram
      sprintf(histname,"mass_vs_energy_corr_st%d",st+1);
      rfile->cd(); rfile->("Kinema"); 
      if (gDirectory->Get(histname) ) {
	TH1D * hist = (TH1D*)gDirectory->Get(histname);
	if ( hist->GetFunction("f1") ){
	  TF1 * f1 = (TF1*)hist->GetFunction("f1");
	  data.strip.Slope[st][i] = f1->GetParameter(1);
	  data.strip.SlopeE[st][i]= f1->GetParError(1);

	  //	   if (st==51) printf("%8.3f %10.5f  %10.5f\n",data.RunID[i],data.strip.Slope[st][i],data.strip.SlopeE[st][i]);

	}
      }

    } // end-of-st-loop

    // remove symboric link
    sprintf(text,"rm -f %s",rootfile);
    gSystem->Exec(text);
    rfile->Close();


  }; 

  return;

}


//
// Class name  : 
// Method name : RootFileOperation2(Int_t ndata, Char_t * Beam)
//             :
// Description : Extract informations from histograms in root file
//             : This routine is incomplete Dec.18,2006
// Input       : Int_t ndata, Char_t * Beam
// Return      : 
//
Int_t
ErrorDetector::RootFileOperation2(Int_t ndata, Char_t * Beam){

  Char_t rootfile[100], text[100], histname[100], htitle[100];

  printf("\n");

  // loop over runs
  for (Int_t i=0; i<ndata; i++) {
    
    printf(" --> %d ", i);
    printf(" %8.3f ", data.RunID[i]);
    sprintf(rootfile,"%8.3f.root",data.RunID[i]);
    cout << " xxx " ;
    //    printf("%s : %d %8.3f \r",Beam, i, data.RunID[i]);
    printf("%s : %d %8.3f ",Beam, i, data.RunID[i]);

    // create symboric link
    sprintf(text,"ln -s root/%s %s",rootfile,rootfile);
    gSystem->Exec(text);

    cout << " file open " << rootfile ;
    TFile * rfile = TFile::Open(rootfile);
    cout << " file opened " << rootfile ;

    // Get sin(phi) fitting function from asym_sinphi_fit histogram
    Asymmetry->cd();
    data.strip.sinphi_P = data.strip.sinphi_dPhi = 0;

    /*
    if (gDirectory->Get("asym_sinphi_fit")) {

      if (asym_sinphi_fit->GetFunction("tg")){
	TGraphErrors *tg = (TGraphErrors*)asym_sinphi_fit->GetFunction("tg");
	if ((tg->GetX())&&(tg->GetY())&&(tg->GetEY())) {
	  Double_t *x = tg->GetX();
	  Double_t *y = tg->GetY();
	  Double_t *ey = tg->GetEY();

	  if (tg->GetFunction("sin_phi")) {
	    TF1 * sin_phi = (TF1*)tg->GetFunction("sin_phi");
	    data.strip.sinphi_P    = sin_phi->GetParameter("P");
	    data.strip.sinphi_dPhi = sin_phi->GetParameter("dPhi");

	    // strip loop
	    for (Int_t st=0; st<NSTRIP; st++) {
	      data.strip.sinphi_sigma[st][i]=data.strip.sinphi_sigmaE[st][i]=0;
	      
	      // calculate diviation (in the unit of sigma) of each strips from fitted sin(phi) curve.
	      if (ey[st]){
		data.strip.sinphi_sigma[st][i]=(y[st]-data.strip.sinphi_P*sin(-x[st]+data.strip.sinphi_dPhi))/ey[st] ;
		data.strip.sinphi_sigmaE[st][i]=ey[st];
	      }

	    } // end-of-st-loop


	  } // if (tg->GetFunction("sin_phi"))

	} // if ((tg->GetX())&&(tg->GetY())&&(tg->GetEY()))

      } // if (asym_sinphi_fit->GetFunction("tg"))

    } // if (gDirectory->Get("asym_sinphi_fit");
    */

    // remove symboric link
    sprintf(text,"rm -f %s",rootfile);
    gSystem->Exec(text);
    rfile->Close();

    cout << "next loop" << endl;

  };  // end-if-ndata-loop


  return;

}

//
// Class name  : ErrorDetector
// Method name : IndividualStripHistory(Int_t ndata, Char_t *Beam, Int_t Color)
//
// Description : Draw Strip history individually
// Input       : Int_t ndata, Char_t Beam, Int_t Color
// Return      : 
//
Int_t
ErrorDetector::IndividualStripHistory(Int_t ndata, Char_t *Beam, Int_t Color){

  gStyle->SetOptStat(0);
  CurC->Divide(4,3);   ps->NewPage();

  Char_t histname[100], htitle[100];

  Float_t margin=0.05;
  Float_t xmin,xmax,ymin,ymax; 
  GetScale(data.RunID, ndata, margin, xmin, xmax);
  TLine * t0 = new TLine(xmin, 0, xmax, 0);
  TLine * tl = new TLine(xmin,  0.001, xmax,  0.001);
  TLine * th = new TLine(xmin, -0.001, xmax, -0.001);
  tl->SetLineStyle(2);  tl->SetLineColor(2);
  th->SetLineStyle(2);  th->SetLineColor(2);

  for (Int_t det=0; det<NDETECTOR; det++){
    
    for (Int_t i=0; i<NSTRIP_PER_DETECTOR; i++){

      // Draw TH2F frame 
      st=det*NSTRIP_PER_DETECTOR+i;
      sprintf(histname,"strip_slope_history_st%d",st+1);
      sprintf(htitle,"%s : Strip Slope History St %d",Beam, st+1);
      GetScalePrefix(0.002, ndata, data.strip.Slope[st], margin, ymin, ymax);
      strip_slope_history[st] = new TH2F(histname, htitle, 100, xmin, xmax, 100, ymin, ymax);
      TH2F * h = (TH2F*)gDirectory->Get(histname);
      h->GetXaxis() -> SetTitle("Run ID");
      h->GetYaxis() -> SetTitle("slope [GeV/keV]");
      CurC->cd(i+1); //h->Draw(); tl->Draw("same") ; th->Draw("same"); t0->Draw("same");
      h->GetListOfFunctions()->Add(tl); h->GetListOfFunctions()->Add(th); h->GetListOfFunctions()->Add(t0);


      // Superpose TGraphErrors
      TGraphErrors * tge = new TGraphErrors(ndata, data.RunID, data.strip.Slope[st], dx, data.strip.SlopeE[st]);
      tge -> SetMarkerStyle(20);
      tge -> SetMarkerSize(1.0);
      tge -> SetMarkerColor(Color);
      //      tge -> Draw("PL");
      h->GetListOfFunctions()->Add(tge,"PL"); 
      h->Draw();
      CurC->Update();

    }// end-of-STRIP_PER_DETECTOR loop

    if (det!=NDETECTOR-1) ps->NewPage();

  }// end-of-NDETECTOR loop


}


//
// Class name  : ErrorDetector
// Method name : IndividualStripHistorySinPhi(Int_t ndata, Char_t *Beam, Int_t Color)
//
// Description : Draw Strip history individually for sin(phi) fit
// Input       : Int_t ndata, Char_t Beam, Int_t Color
// Return      : 
//
Int_t
ErrorDetector::IndividualStripHistorySinPhi(Int_t ndata, Char_t *Beam, Int_t Color){

  gStyle->SetOptStat(0);
  CurC->Divide(4,3);   ps->NewPage();

  Char_t histname[100], htitle[100];

  Float_t margin=0.05;
  Float_t xmin,xmax,ymin,ymax; 
  GetScale(data.RunID, ndata, margin, xmin, xmax);
  TLine * t0 = new TLine(xmin, 0, xmax, 0);
  TLine * tl = new TLine(xmin,  0.001, xmax,  0.001);
  TLine * th = new TLine(xmin, -0.001, xmax, -0.001);
  tl->SetLineStyle(2);  tl->SetLineColor(2);
  th->SetLineStyle(2);  th->SetLineColor(2);

  for (Int_t det=0; det<NDETECTOR; det++){
    
    for (Int_t i=0; i<NSTRIP_PER_DETECTOR; i++){

      // Draw TH2F frame 
      st=det*NSTRIP_PER_DETECTOR+i;
      sprintf(histname,"sinphi_fit_sigma_st%d",st+1);
      sprintf(htitle,"%s : Sigma from Sin(phi) Fit St %d",Beam, st+1);
      GetScalePrefix(0.002, ndata, data.strip.sinphi_sigma[st], margin, ymin, ymax);
      sinphi_fit_sigma_st[st] = new TH2F(histname, htitle, 100, xmin, xmax, 100, ymin, ymax);
      TH2F * h = (TH2F*)gDirectory->Get(histname);
      h->GetXaxis() -> SetTitle("Run ID");
      h->GetYaxis() -> SetTitle("sigma form sin(phi) fit");
      CurC->cd(i+1); //h->Draw(); tl->Draw("same") ; th->Draw("same"); t0->Draw("same");
      h->GetListOfFunctions()->Add(tl); h->GetListOfFunctions()->Add(th); h->GetListOfFunctions()->Add(t0);

      // Superpose TGraphErrors
      TGraphErrors * tge = new TGraphErrors(ndata, data.RunID, data.strip.sinphi_sigma[st], dx, data.strip.sinphi_sigmaE[st]);
      tge -> SetMarkerStyle(20);
      tge -> SetMarkerSize(1.0);
      tge -> SetMarkerColor(Color);
      //      tge -> Draw("PL");
      h->GetListOfFunctions()->Add(tge,"PL"); 
      h->Draw();
      CurC->Update();

    }// end-of-STRIP_PER_DETECTOR loop

    if (det!=NDETECTOR-1) ps->NewPage();

  }// end-of-NDETECTOR loop


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
    Char_t *RunStatus, *MeasType;
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
      MeasType  = strtok(NULL," ");
      
      if ( strcmp(RunStatus,"Bad")*strcmp(RunStatus,"N/A")*strcmp(RunStatus,"Junk") ) {
	if ( strcmp(MeasType,"PROF")*strcmp(MeasType,"TUNE")*strcmp(MeasType,"PHYS") ) {

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

	} // if (MeasType)

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
// Method name : GetDataAndPlot(Int_t Mode, Char_t * Beam, Int_t Color)
//
// Description : Plotting controll center
//             : Mode Number : odd  - typical 1D histograms don't requre frame to be drawn
//             :             : even - typical 2D histograms require frame to be drawn and then 
//             :                      superposed by graph *tg object in Plot() routine
// Input       : (Int_t Mode, Char_t * Beam, Int_t Color)
// Return      : 
//
Int_t
ErrorDetector::GetDataAndPlot(Int_t Mode, Char_t * Beam, Int_t Color){

  Char_t DATAFILE[100],text[100], histname[100], htitle[100];
  sprintf(DATAFILE,"summary/ErrorDet_%s_phys.dat",Beam);
  cout << "DATAFILE=" << DATAFILE << endl;

  Int_t ndata = GetData(DATAFILE);
  if (Mode<100){
    RootFileOperation(ndata, Beam);
    IndividualStripHistory(ndata, Beam, Color);
    /* Following routines are incomplete. Dec.18,2006
    RootFileOperation(ndata, Beam);
    IndividualStripHistorySinPhi(ndata, Beam, Color);
    */
    return;
  };

  Int_t i,j; Float_t xmin,xmax,ymin,ymax;  Float_t margin=0.05;
  if (!(Mode&1)) { // Only Even Mode Numbers
    DrawFrame(Mode, ndata);
    Plot(Mode, ndata, Color);
    if (Mode==100) Plot(Mode+1, ndata, Color);
  } else { // Odd Mode Numbers
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
    case 351:
      sprintf(histname,"BadStripIndex%d",j);
      sprintf(htitle,"Problematic Strip ID History in Index");
      BadStripIndex = new TH2D(histname, htitle, 72, 0.5, 72.5, ndata, 0, ndata);
      BadStripIndex -> GetXaxis()->SetTitle("Strip ID");
      BadStripIndex -> GetYaxis()->SetTitle("Index");
      BadStripIndex -> SetMarkerStyle(21);
      //      BadStripIndex -> SetMarkerSize(10);
      BadStripIndex -> SetMarkerColor(Color);
      BadStripIndex -> SetFillColor(Color);
      for (i=0; i<ndata; i++) {
	TLine * l = new TLine(12*i, 0, 12*i, ndata);
	l -> SetLineStyle(2);
	BadStripIndex -> GetListOfFunctions() -> Add(l);
	for (j=0; j<data.strip.NBad[i]; j++) {
	  BadStripIndex->Fill(data.strip.StripID[i][j], i);
	}
      }
      BadStripIndex->Draw("box");
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

  Int_t Mode=0; // 0 - regular , 100 - individual strip history
  Char_t HEADER[100], psfile[100], text[100];

  // load header macro
  sprintf(HEADER,"%s/SuperposeSummaryPlot.h",gSystem->Getenv("MACRODIR"));
  gROOT->LoadMacro(HEADER);

  // some utility macros
  sprintf(HEADER,"%s/include.h",gSystem->Getenv("MACRODIR"));
  gROOT->LoadMacro(HEADER);

  // Cambus Setup
  CurC = new TCanvas("CurC","",1);
  CurC -> SetGridy();

  // Individual Strip History Routines (slow)
  if (Mode==100){
    sprintf(psfile,"ps/IndividualStripHistory.ps");
    ps = new TPostScript(psfile,112);
    gStyle->SetOptStat(1111);
    MakePlots(10, CurC, ps);// Individual Strip History 4x3 canvas times 6 pages times 2
  } else {

    // Regular Routines
    sprintf(psfile,"ps/ErrorDetector.ps");
    ps = new TPostScript(psfile,112);

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
    MakePlots(351, CurC, ps); // Bad Strip Index
    MakePlots(353, CurC, ps); // Bad Strip History
    CurC->SetGridy();
    MakePlots(355, CurC, ps); // Bad Strip Statistics (1-dim)

  }


  cout << "ps file : " << psfile << endl;
  ps->Close();
    
  sprintf(text,"gv -landscape %s", psfile );
  gSystem->Exec(text);
 
  return 0;

}



