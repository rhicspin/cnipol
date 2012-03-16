#ifndef KinFit_h
#define KinFit_h
 
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <iomanip>

#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2D.h>
#include <TLegend.h>
#include <TLine.h>
#include <TMinuit.h>
#include <TPostScript.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>

#include <CnipolHists.h>

#define Debug 0

// Global Variables
//const int NSTRIP=72;
const int NDisableList  = 5;
const int NDisableStrip = 5;
static Float_t KinConst_E2T=1454.75; // constant for Run05 L=18.5cm

int DisableStr[NDisableList][NDisableStrip] =
{
  { 14, 22, -1, -1, -1}, // 0 Blue
  { 51, 55, -1, -1, -1}, // 1 Yellow FLattop Disable List
  { -1, -1, -1, -1, -1}, // 2
  { -1, -1, -1, -1, -1}, // 3
  { -1, -1, -1, -1, -1}, // 4
};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//   FUNCTION to convert from (Edep, Dwidth) to Ekin
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const Double_t cp0[4] = {-0.5174     , 0.4172      , 0.3610E-02  , -0.1286E-05};
const Double_t cp1[4] = {1.0000      , 0.8703E-02  , 0.1252E-04  , 0.6948E-07};
const Double_t cp2[4] = {0.2990E-05  , -0.7937E-05 , -0.2219E-07 , -0.2877E-09};
const Double_t cp3[4] = {-0.8258E-08 , 0.4031E-08  , 0.9673E-12  , 0.3661E-12};
const Double_t cp4[4] = {0.3652E-11  , -0.8652E-12 , 0.4059E-14  , -0.1294E-15};

Double_t    KinFunc(Double_t *x, Double_t *par);
Double_t    ExCoef(Double_t x, Int_t Index);
std::string BaseName(Char_t *filename);

class KinFit
{

public:
  //KinFit(Char_t *, Float_t, Int_t, Float_t, Float_t, Float_t, Int_t, Char_t*, Char_t *);
  KinFit(string runidinput, Float_t beneinput=0, Int_t RHICBeam=0, Float_t E2T=0,
         Float_t EMIN=0, Float_t EMAX=0, Int_t hid=15000, Char_t *cfile="",
         Char_t *online_cfile="", Char_t *outputdir="./");

  ~KinFit();

  std::ofstream fout;
  std::ofstream ferrout;

  void    Fit(Int_t);
  void    FitOne(TH2* h, Int_t, Int_t);
  //void    CoefsGet();   // read coeficients from root file
  Int_t   PlotDlayer(Int_t, TLegend*);
  Int_t   PlotT0(Int_t);
  void    Residual();
  void    PlotResidual(Int_t St);
  void    PlotResult();
  Int_t   ReferenceConfig();
  Int_t   ReferenceDlayer(TLegend*);
  Int_t   GetDlayerFromFile();
  Int_t   GetDlayer(Char_t* fileName);
  Int_t   SuperposeDlayerPlot(Char_t *, TLegend*, Int_t);
  Int_t   SuperposeT0Plot(Int_t);
  Int_t   DisableList(Int_t RHIC_Beam, Int_t St);
  Float_t WeightedMean(Float_t A[72], Float_t dA[72], Int_t NDAT);
  Int_t   GetData(Char_t*);

  CnipolHists *fHists;
  Char_t  runid[10], CONFFILE[256], DLAYERFILE[256], ONLINE_CONFFILE[256], OUTPUTDIR[100];
  Float_t RUNID, bene;

  Int_t   RHIC_Beam;
  Int_t   HID;   // const. t cut by default

  Float_t dlsum[6];   // average of each detector
  Float_t SiDev[6], StDev[72];
  Float_t totSiDev;

  struct StrcutT0 {
      Float_t Delta;
      Float_t ave;
      Float_t Valid[72];
      Float_t ValidE[72];
  } T0;

  struct StructFlag {
      Int_t PLOT_ONLINE_CONFIG;
  } flag;

  // Fitting Energy Range
  Float_t FitRangeLow;// = 400.;
  Float_t FitRangeUpp;// = 900.;

  struct StructResiducals {
      Float_t x[72][1000];
      Float_t y[72][1000];
  } resd;

  struct PlotRange {
    Float_t TMIN;
    Float_t TMAX;
    Float_t DMIN;
    Float_t DMAX;
  } plotrange;

  Float_t strip[72], stripE[72];
  Float_t dl[72], dlE[72], dl_d[72], dlE_d[72];
  Float_t t0[72], t0E[72], t0_d[72], t0E_d[72];
  Float_t t0s[72], t0sE[72], t0s_d[72], t0sE_d[72]; // T0 result from single parameter fit
  Float_t dlValid[72], dlEValid[72];
  Float_t dlSi[12], dlESi[12];
  Float_t Chi2[72]; // Fitting Chi2 for each strip
  Float_t devpst; // Average deviation/strip

  Float_t Const[72],Slope[72];   // Final Results for DAQ
  Float_t dlave;

  // From configulation file
  Float_t dlsum_ref[6],t0_ref[72],t0E_ref[72];
};

#endif
