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

gROOT->Reset();

// Global Variables
const int NDisableList  = 5;
const int NDisableStrip = 5;
int DisableStr[NDisableList][NDisableStrip] =
  {
    { 14, 22, -1, -1, -1}, // 0 Blue
    { 51, 55, -1, -1, -1}, // 1 Yellow FLattop Disable List
    { -1, -1, -1, -1, -1}, // 2
    { -1, -1, -1, -1, -1}, // 3
    { -1, -1, -1, -1, -1}, // 4
  } ;



// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//   FUNCTION to convert from (Edep, Dwidth) to Ekin
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Double_t KinFunc(Double_t *x, Double_t *par){
    
    const Double_t cp0[4] = {-0.5174,0.4172,0.3610E-02,-0.1286E-05}; 
    const Double_t cp1[4] = {1.0000,0.8703E-02,0.1252E-04,0.6948E-07};
    const Double_t cp2[4] = {0.2990E-05,-0.7937E-05,-0.2219E-07,-0.2877E-09};
    const Double_t cp3[4] = {-0.8258E-08,0.4031E-08,0.9673E-12,0.3661E-12};
    const Double_t cp4[4] = {0.3652E-11,-0.8652E-12,0.4059E-14,-0.1294E-15};
    Double_t pp[5];
    
    Double_t par0;
    Double_t x0 = x[0];
    if (par[0]>=0.) {
        par0 = fabs(par[0]);
    } else {
        par0 = 0.;
    }

    pp[0] = cp0[0] + cp0[1]*par0 + cp0[2]*pow(par0,2) + cp0[3]*pow(par0,3);
    pp[1] = cp1[0] + cp1[1]*par0 + cp1[2]*pow(par0,2) + cp1[3]*pow(par0,3);
    pp[2] = cp2[0] + cp2[1]*par0 + cp2[2]*pow(par0,2) + cp2[3]*pow(par0,3);
    pp[3] = cp3[0] + cp3[1]*par0 + cp3[2]*pow(par0,2) + cp3[3]*pow(par0,3);
    pp[4] = cp4[0] + cp4[1]*par0 + cp4[2]*pow(par0,2) + cp4[3]*pow(par0,3);
    
    Double_t Ekin = pp[0] + pp[1]*x0 + pp[2]*pow(x0,2) 
        + pp[3]*pow(x0,3) + pp[4]*pow(x0,4);
    
    if (Ekin != 0.0) {
//----------------------------------------
// t = 22.7*sqrt(Mass-Carbon(AMU)=12)*L
// For 2004 L=15.03 cm => t = 1181.6
// For 2005 L=18.50 cm => t = 1454.75
//----------------------------------------
        Double_t tof = 1454.75/sqrt(Ekin) + par[1];
    } else {
        Double_t tof =0.0;
    }

    return tof;

};

// Extracting the Coefficients
Double_t ExCoef(Double_t x, Int_t Index){
    // x : dlayer [ug/cm2]
    // Index : the Index-th order of coefficient
    
    const Double_t cp0[4] = {-0.5174,0.4172,0.3610E-02,-0.1286E-05}; 
    const Double_t cp1[4] = {1.0000,0.8703E-02,0.1252E-04,0.6948E-07};
    const Double_t cp2[4] = {0.2990E-05,-0.7937E-05,-0.2219E-07,-0.2877E-09};
    const Double_t cp3[4] = {-0.8258E-08,0.4031E-08,0.9673E-12,0.3661E-12};
    const Double_t cp4[4] = {0.3652E-11,-0.8652E-12,0.4059E-14,-0.1294E-15};

    Double_t pp[5];
    
    pp[0] = cp0[0] + cp0[1]*x + cp0[2]*pow(x,2) + cp0[3]*pow(x,3);
    pp[1] = cp1[0] + cp1[1]*x + cp1[2]*pow(x,2) + cp1[3]*pow(x,3);
    pp[2] = cp2[0] + cp2[1]*x + cp2[2]*pow(x,2) + cp2[3]*pow(x,3);
    pp[3] = cp3[0] + cp3[1]*x + cp3[2]*pow(x,2) + cp3[3]*pow(x,3);
    pp[4] = cp4[0] + cp4[1]*x + cp4[2]*pow(x,2) + cp4[3]*pow(x,3);
    
    return pp[Index];

};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  MAIN
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class KinFit {
public:
  KinFit(Char_t *, Float_t, Int_t, Int_t);
  ~KinFit();
  
  void Fit(Int_t);
  void FitOne(Int_t, Int_t);

  void CoefsGet();   // read coeficients from root file 
  void PlotResult();

  Int_t DisableList(Int_t RHIC_Beam, Int_t St);
  Float_t WeightedMean(Float_t A[72], Float_t dA[72], Int_t NDAT);



  ofstream fout;
  ofstream ferrout;
    
  Char_t runid[10];
  Float_t bene;
  Int_t RHIC_Beam;
  Int_t HID=15000;   // const. t cut by default

  Float_t dlsum[6];   // average of each detector
  Float_t SiDev[6], StDev[72];
  Float_t totSiDev=0;


  Float_t dl[72], dlE[72];
  Float_t t0[72], t0E[72];
  Float_t t0s[72], t0sE[72]; // T0 result from single parameter fit
  Float_t dlValid[72], dlEValid[72];
  Float_t dlSi[12], dlESi[12];
  static Float_t Chi2[72]; // Fitting Chi2 for each strip
  static Float_t devpst=0; // Average deviation/strip

  Float_t Const[72],Slope[72];   // Final Results for DAQ 

  Float_t dlave=0;

};



// Initialization of the class
KinFit::KinFit(Char_t *runidinput, Float_t beneinput, Int_t RHICBeam, Int_t hid){
    sprintf(runid,"%s",runidinput);
    RHIC_Beam = RHICBeam;
    bene = beneinput;
    HID = hid;

    if (HID==15000) printf("Fit on constant time cut histogram, HID:%5d\n", HID);
    if (HID==15100) printf("Fit on  banana-mass  cut histogram, HID:%5d\n", HID);

    memset(dl,0, sizeof(dl));
    memset(dlE,0, sizeof(dlE));
    memset(t0,0, sizeof(t0));
    memset(t0E,0, sizeof(t0E));
};
KinFit::~KinFit(){};

// -------------------------------------------------------------------
//  fit the E-T band with non-linear dlayer corrected function
//  plot and extract fit results
//  mode : 0 ... two free parameters
//         1 ... fix dlayer width t0 free parameter
// -------------------------------------------------------------------

void KinFit::Fit(Int_t mode) 
{    

    if (!mode&1) {
        cout << "TRY TWO FREE PARAMETER FIT" <<endl;
    } elseif (mode&1) {
        cout << "TRY T0 PARAMETER FIT (fixed Dlayer)" <<endl;
    }        

    if (mode&1) {
        fout.open("testfit.dat");
    
        gStyle->SetGridColor(1);
        gStyle->SetGridStyle(2);
        //        gStyle->SetPalette(1,0);
    
        TCanvas *CurC = new TCanvas("CurC","",1);
        TPostScript ps("testfit.ps",112);
    
        CurC -> Divide(4,3);
        ps.NewPage();
    }

    Int_t NValidSt=0;
    for (Int_t Si=0;Si<6;Si++) {
      cout << Si << endl;

        Int_t Padn=0;
        Float_t dl_accum = 0.;
        Int_t valstn = 0;

	j=0;
        for (Int_t St=Si*12; St<Si*12+12; St++) {
	  dlValid[St] = dlEValid[St] = dlSi[j] = dlESi[j] = 0;
	  
            Char_t hName[100];
            sprintf(hName,"h%d",HID+St+1);  
            TH2D* htemp = (TH2D*) gDirectory->Get(hName);

            Padn++;   // change pad even if the histograms is empty
            if (mode&1) CurC->cd(Padn);
            
            if (htemp->GetEntries() > 20000) {	//20000) {
                
	      FitOne(St, mode);
                
	      // fill arrays only if strip is valid
	      if (!mode&1) { 
		  if (!DisableList(RHIC_Beam,St)){
		    dl_accum += dl[St];
		    dlValid[St] = dl[St];
		    dlEValid[St] = dlE[St];
		    dlSi[j] = dl[St];
		    dlESi[j] = dlE[St];
		    valstn++;
		  }; // End-of-Loop : Valid Strip 
                }
            } 

	    ++j;
        } // End-of-Loop for Strip 

        if (mode&1) {
            CurC->Update();
            ps.NewPage();
        }

        if (!mode&1) {
	  //dlsum[Si] = dl_accum/valstn;
	  dlsum[Si] = WeightedMean(dlSi,dlESi,12);
	  SiDev[Si]=0;

	  for (Int_t St=Si*12; St<Si*12+12; St++) {
	    if (!DisableList(RHIC_Beam,St)){
	      StDev[St] = fabs(dlsum[Si]- dlValid[St]);
	      SiDev[Si]+= fabs(dlsum[Si]- dl[St]);
	    }
	  }

	  totSiDev+=SiDev[Si];
	  NValidSt+=valstn;
	  dlave+=dl_accum;
	}
    } // end-of-St loop

    if (!mode&1){
      dlave = (Float_t)WeightedMean(dlValid,dlEValid,72);
      if (NValidSt) {
	//dlave/=float(NValidSt);
	devpst=WeightedMean(StDev,dlEValid,NValidSt);
      }
    } else if (mode&1) {
      printf("\n\n");
      printf("-----------------------------------------------------------\n");
      printf(" dlave = %7.2f\n",dlave);
      printf(" Chi2/Det=");
      for (Int_t Si=0;Si<6;Si++) printf(" %7.1f",SiDev[Si]);
      printf("\n Total Deviation=%7.1f\n",totSiDev);
      printf(" Deviation/strip=%7.1f\n",devpst);
      printf("-----------------------------------------------------------\n");
      printf("\n\n");
      fout.close();
      ps.Close();
    }

};


// -------------------------------------------------------------------
//     Fit the given Strip
// -------------------------------------------------------------------
void KinFit::FitOne(Int_t St, Int_t mode)
{

    Int_t Si = (Int_t)St/12;

    cout << " ===================================="<<endl;
    cout << " === Now starting - "<<St<< " ( mode: "<<mode<<" )"<<endl;
    cout << " ===================================="<<endl;

    Char_t hName[100];
    sprintf(hName,"h%d",HID+St+1);  
    
    TH2D* h2d = (TH2D*) gDirectory->Get(hName);
    h2d -> SetStats(0);
    sprintf(hName,"%s ST-%d",runid, St);
    h2d -> SetTitle(hName);
    h2d -> SetName("h2d");
    h2d->GetXaxis()->SetTitle("Measured Energy (keV)");
    h2d->GetYaxis()->SetTitle("Tof");
    
    h2d->GetXaxis()->SetRangeUser(200., 1000.);
    h2d->GetYaxis()->SetRangeUser(10., 100.);
    
    TH2D* h2dcarbon = (TH2D*) h2d->Clone();
    h2dcarbon -> SetName("h2dcarbon");
    
    // Extracting information of the 2D histogram
    Int_t xnbins = h2dcarbon ->GetXaxis()->GetNbins();
    Float_t xmin = h2dcarbon ->GetXaxis()->GetXmin();
    Float_t xmax = h2dcarbon ->GetXaxis()->GetXmax();
    Int_t ynbins = h2dcarbon ->GetYaxis()->GetNbins();
    Float_t ymin = h2dcarbon ->GetYaxis()->GetXmin();
    Float_t ymax = h2dcarbon ->GetYaxis()->GetXmax();
    
    // ======================================================
    // ====   The fit                                  ==
    // ======================================================

    // Fit slices for the 1st fit (auto fit routine with gaussian)
    
    TF1* myf = new TF1("myf","gaus",0.0,150.0);

    // This is not necessary for Mass Cut event selection.
    // if (bene > 50.) 
    // myf -> SetRange(10.0, 50.);

    h2d->FitSlicesY(myf);

    TH1F* sfit = (TH1F*) gDirectory->Get("h2d_1");

    // Define banana function
    TF1 *kinf = new TF1("kinf",KinFunc, 200.0, 1000.0, 2);

    // Set the default values
    // Dlayer Fix/ Or not
    
    if (!mode&1) {
        kinf->SetParameters(50.0, 0.0);
        kinf->SetParLimits(0, 0., 200.);
    } elseif (mode&1) {
        kinf->SetParameters(dlsum[Si], 0.0);
        kinf->SetParLimits(0, dlsum[Si], dlsum[Si]);
    }        
    kinf->SetParLimits(1, -50., 50.);

    if (mode&1) h2d  -> Draw("color");

    // %%%%%%%%%%%%%%
    Float_t FitRangeLow = 400.;
    Float_t FitRangeUpp = 900.;

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    sfit -> Fit(kinf,"E0","", FitRangeLow, FitRangeUpp);
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // putting in vectors

    Float_t ChiDOF=0;
    if (!mode&1) {

        // two parameter fit

	Chi2[St] = kinf->GetChisquare()/kinf->GetNDF();
        dl[St]   = fabs(kinf->GetParameter(0));
        dlE[St]  = Chi2[St] * kinf->GetParError(0);
        t0[St]   = kinf->GetParameter(1);
        t0E[St]  = Chi2[St] * kinf->GetParError(1);

    } else if (mode&1) {

        // one parameter fit
        ChiDOF   = kinf->GetChisquare()/kinf->GetNDF();
        t0s[St]  = kinf->GetParameter(1);
        t0sE[St] = ChiDOF *  kinf->GetParError(1);

    }        

    if (mode&1) {

        // Plot data points (Magenta)

        TH1D* Dpoints = (TH1D*) sfit->Clone();
        Dpoints -> SetName("Dpoints");
        Dpoints -> SetLineColor(6);
        Dpoints -> SetLineWidth(1.2);
        Dpoints -> GetXaxis()->SetRangeUser(300., 1000.);
        Dpoints -> Draw("same");
        
        // Plot fit function (Green)

        TF1* fitfun = (TF1*) kinf->Clone();
        fitfun -> SetName("fitfun");
        fitfun -> SetLineColor(3);
        fitfun -> SetLineWidth(2.0);
        fitfun -> Draw("same");

        // TYPE the result on the Plots
    
        TText t; Char_t rtext[50];
        sprintf(rtext, "Dl = %6.1f ug/cm2", dl[St]); //kinf->GetParameter(0));
        t.DrawTextNDC(0.5, 0.8, rtext);
        sprintf(rtext, "T0 = %6.2f nsec",   kinf->GetParameter(1));
        t.DrawTextNDC(0.5, 0.72, rtext);
        sprintf(rtext, "Chi2/d.o.f = %6.2f", Chi2[St]);
	if (Chi2[St]>40) t.SetTextColor(2);
        t.DrawTextNDC(0.5, 0.64, rtext);
	t.SetTextColor(1);

        // Calculate the Linear Function Coefficients
        // Using the reference of 400-900keV fit
        //             const0   const1  const2     slope0  slope1    
        // 300-900keV  -0.480095 1.81638 0.00702198 1.00975 0.00213976
        // 400-900keV  -0.545298 2.05241 0.00689393 1.00961 0.00182463
        // 500-900keV  -0.71873 2.27486 0.00654193 1.00909 0.0015731
        
        Const[St] = -0.545 + 2.052 * dlsum[(Int_t)St/12] 
            + 0.0069 * dlsum[(Int_t)St/12]*dlsum[(Int_t)St/12];
        
        Slope[St] = 1.0096 + 0.0018 * dlsum[(Int_t)St/12]; 

        // Write Out the Result on File
    
        fout << St<< " ";
        fout << setprecision(4) << dlsum[(Int_t)St/12]  <<" ";
        fout << setprecision(4) << t0s[St]  <<" ";
        fout << setprecision(4) << t0sE[St] <<" ";
        fout << setprecision(4) << dl[St]  <<" ";
        fout << setprecision(4) << dlE[St] <<" ";
        fout << setprecision(4) << t0[St]  <<" ";
        fout << setprecision(4) << t0E[St] <<" ";
        fout << setprecision(3) << kinf->GetChisquare()/kinf->GetNDF() <<" ";
        fout << kinf->GetNDF()<<" ";
        fout << gMinuit->fCstatu.Data() <<" "; 
        fout << setprecision(4) << Const[St] << " ";
        fout << setprecision(4) << Slope[St] << " ";
        fout << setprecision(5) << ExCoef(dlsum[(Int_t)St/12], 0) <<" ";
        fout << setprecision(5) << ExCoef(dlsum[(Int_t)St/12], 1) <<" ";
        fout << setprecision(5) << ExCoef(dlsum[(Int_t)St/12], 2) <<" ";
        fout << setprecision(5) << ExCoef(dlsum[(Int_t)St/12], 3) <<" ";
        fout << setprecision(5) << ExCoef(dlsum[(Int_t)St/12], 4)
        << endl;

    }

    h2d_0    -> Delete();    // just to reduce the error message
    h2d_1    -> Delete();
    h2d_2    -> Delete();
    h2d_chi2 -> Delete();

}


// -------------------------------------------------------------------
//     Plot the result of the Fit 
// -------------------------------------------------------------------
void KinFit::PlotResult()
{

    TCanvas *CurC = new TCanvas("CurC","",1);
    TPostScript ps("testsummary.ps",112);
    
    CurC -> Divide(1,2);

    Float_t strip[72], stripE[72];
    for (Int_t St=0; St<72; St++) {
        strip[St] = (Float_t)St;
        stripE[St] = 0.;
    }

    // Fill Disabled Strip Arrays
    Float_t disable_strip[NDisableStrip];
    Float_t disable_dl[NDisableStrip], disable_dlE[NDisableStrip];
    for (Int_t i=0; i<NDisableStrip; i++){
      disable_dl[i] = disable_dlE[i] = 0; // initiarization
      disable_strip[i]=DisableStr[RHIC_Beam][i];
      if (disable_strip[i]!=-1) {
	disable_dl[i]=dl[int(disable_strip[i])];
        disable_dlE[i]=dlE[int(disable_strip[i])];
      }
    }


    // -------------
    // Plot - 1 (Dead layer distribution)
    // -------------

    CurC -> cd(1);
    
    Char_t title[40];
    sprintf(title, "%s Dead Layer Distribution", runid); 
    TH2D* frame = new TH2D("frame", title, 10, -0.5, 71.5, 10, 0., 80.);
    frame -> SetStats(0);
    frame -> GetXaxis()->SetTitle("Strip Number");
    frame -> GetYaxis()->SetTitle("Dead Layer (\mu g/cm**2)");
    frame -> Draw();

    // draw total average value
    TLine *tave = new TLine(-0.5,dlave,71.5,dlave);
    tave -> SetLineColor(5);
    tave -> SetLineWidth(2);
    tave -> Draw();

    // draw the separaters btw detectors
    for (Int_t isep=0; isep<6 ; isep++) {
        TLine *l = new TLine(12.*isep -0.5, 0., 12.*isep -0.5, 80.);
        l -> Draw();
    }


    TGraphErrors* tgdl = new TGraphErrors(72, strip, dl, stripE, dlE);
    tgdl -> SetMarkerStyle(20);
    tgdl -> SetMarkerSize(1.0);
    tgdl -> SetLineWidth(1.0);
    tgdl -> SetLineColor(4);
    tgdl -> SetMarkerColor(4);
    
    tgdl -> Draw("P");
    
    // Superposition Disabled Strips 
    TGraphErrors* tgdl = new TGraphErrors(NDisableStrip, disable_strip, disable_dl, stripE, disable_dlE);
    tgdl -> SetMarkerStyle(20);
    tgdl -> SetMarkerSize(1.0);
    tgdl -> SetLineWidth(1.0);
    tgdl -> SetLineColor(7);
    tgdl -> SetMarkerColor(7);
    
    tgdl -> Draw("P");
    

    // draw the average values
    for (Int_t isep=0; isep<6 ; isep++) {
        TLine *ave = new TLine(12.*isep -0.5, dlsum[isep], 
                               12.*(isep+1) -0.5, dlsum[isep]);
        ave -> SetLineColor(6);
        ave -> SetLineWidth(2.);
        ave -> Draw();
        
   }
    

    TText t; Char_t rtext[50];
    sprintf(rtext, "Average Deviation = %6.1f ug/cm2", devpst); 
    t.DrawTextNDC(0.5, 0.8, rtext);


    // -------------
    // Plot - 2 (T0 distribution)
    // -------------

    CurC -> cd(2);
    
    Char_t title[40];
    sprintf(title, "%s : T0 Distribution", runid);
    TH2D* frame = new TH2D("frame", title, 10, -0.5, 71.5, 10, -30., 10.);
    frame -> SetStats(0);
    frame -> GetXaxis()->SetTitle("Strip Number");
    frame -> GetYaxis()->SetTitle("T0 values (nsec)");
    frame -> Draw();

    // draw the separaters btw detectors
    for (Int_t isep=0; isep<6 ; isep++) {
        TLine *l = new TLine(12.*isep -0.5, -20., 12.*isep -0.5, 20.);
        l -> Draw();
    }
    
    // result of two paramter fit (blue)
    TGraphErrors* tgt0 = new TGraphErrors(72, strip, t0, stripE, t0E);
    tgt0 -> SetMarkerStyle(20);
    tgt0 -> SetMarkerSize(1.0);
    tgt0 -> SetLineWidth(1.0);
    tgt0-> SetLineColor(4);
    tgt0 -> SetMarkerColor(4);
    
    tgt0 -> Draw("P");
    
    // result of one parameter fit (red)
    TGraphErrors* tgt0s = new TGraphErrors(72, strip, t0s, stripE, t0sE);
    tgt0s -> SetMarkerStyle(21);
    tgt0s -> SetMarkerSize(1.0);
    tgt0s -> SetLineWidth(1.0);
    tgt0s-> SetLineColor(2);
    tgt0s -> SetMarkerColor(2);
    
    tgt0s -> Draw("P");

    ps.Close();


}

//=======================================================================
//          Check Disbled Strip List
//=======================================================================
Int_t 
DisableList(Int_t RHIC_Beam, Int_t St){

  int Match=0;

  for (int j=0;j<NDisableList; j++) {
    Match = St == DisableStr[RHIC_Beam][j]? 1 : 0;
    if (Match) break;
  }

  return Match;
}






//
// Class name  : 
// Method name : WeightedMean(double A[N], double dA[N], int NDAT)
//
// Description : calculate weighted mean
// Input       : double A[N], double dA[N], int NDAT
// Return      : weighted mean
//
Float_t
WeightedMean(Float_t A[72], Float_t dA[72], Int_t NDAT){

  Float_t sum1, sum2, dA2, WM;
  sum1 = sum2 = dA2 = 0;

  for ( int i=0 ; i<NDAT ; i++ ) {
    if (dA[i]){  // skip dA=0 data
      dA2 = dA[i]*dA[i];
      sum1 += A[i]/dA2 ;
      sum2 += 1/dA2 ;
    }
  }

  WM = dA2 == 0 ? 0 : sum1/sum2 ;
  return WM ;

} // end-of-WeightedMean()
