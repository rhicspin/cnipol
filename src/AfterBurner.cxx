// fit one set of spin U/D detector hits for aymmetry etc.
//
//  input: nup[6] - # Up hits in [det = 0-5]
//         nup[6] - # Dn hits in [det = 0-5]
//         fixpar - integer, bit(s) 0-2 = 1  => fix paramater(2) 0-2 to value in par[0-2]
//
// output:  par[3] - results [0]=asymmetry magnitude, [1]=spin tilt (rad.), [2]=lumi asym.
//         epar[3] - error
//            chi2 - minimized chi-square
//         
//
#include "AfterBurner.h"

#include "TVirtualFitter.h"
#include "TGraph2D.h"
#include "TMath.h"

using namespace TMath;

// function prototypes:

void afterburner::fit1asym(double nup[] , double ndn[] , double par[], double epar[] , double &chi2, int fixpar)
{

  double phidet[6] = { 45. , 90. , 135. , 225. , 270. , 315. };

  // fitter wants a TObject to fit: TGraph2D X=phi, Y=nhits_up, Z=nhits_dn
  TGraph2D* gdata = new TGraph2D();
  int npfit = 0;
  for (int i=0; i<6; i++){
    if ( nup[i]>0. && ndn[i]>0. ) { // require hits in detector
      gdata->SetPoint(npfit,DegToRad()*phidet[i],nup[i],ndn[i]);
      npfit++;
    }
  }
  
  // set up fitter
  TVirtualFitter::SetDefaultFitter("Minuit");  // default is Minuit
  TVirtualFitter *fitter = TVirtualFitter::Fitter(0, 3); // need to check what this means; 3=nparam
  fitter->SetFCN(myFcn);
  fitter->TVirtualFitter::SetObjectFit(gdata);

  // define pareters with reasonable init. values and ranges, fix if specified:
  if (!(fixpar & 1)) {
    fitter->SetParameter(0, "epsilon", 0.01, 0.001,    0., 0.02);
  }
  else {
    fitter->SetParameter(0, "epsilon", par[0], 0., par[0], par[0]); fitter->FixParameter(0);
  }
  if (!(fixpar & 2)) {
    fitter->SetParameter(1, "phi",       0., 0.001, -Pi(), Pi());
  }
  else {
    fitter->SetParameter(1, "phi",     par[1], 0., par[1], par[1]); fitter->FixParameter(1);
  }
  if (!(fixpar & 4)) {
    fitter->SetParameter(2, "lambda",    0., 0.001,   -1.,   1.);
  }
  else {
    fitter->SetParameter(2, "lambda",  par[2], 0., par[2], par[2]); fitter->FixParameter(2);
  }
  /*
  // fixed parmaeters:
  if (fixpar & 4) {
    fitter->SetParameter(2, "lambda",  par[2], 0., par[2], par[2]);
    fitter->FixParameter(2);
  }
  */
  Double_t arglist[1] = {0};
  fitter->ExecuteCommand("MIGRAD", arglist, 0);

  for (int i=0; i<3; i++) {par[i] = fitter->GetParameter(i); epar[i] = fitter->GetParError(i);}

  Int_t nPar; Double_t *grad = 0x0;
  myFcn(nPar,grad,chi2,par,0);

}

// fit function: (form of user func. from tutorials/fit/fit2dHist.C)
void afterburner::myFcn(int & /*nPar*/, double * /*grad*/ , double &fval, double *par, int /*iflag */  )
{

  TGraph2D * gdata = dynamic_cast<TGraph2D*>( (TVirtualFitter::GetFitter())->GetObjectFit() );

  double epsilon = par[0]; double phi = par[1]; double lambda = par[2];

  fval = 0.;

  for (int i=0; i<gdata->GetN(); i++) {
    double phidet = gdata->GetX()[i];
    double asymfit = epsilon * Sin(phidet - phi);

    double nup = gdata->GetY()[i]; double ndn = gdata->GetZ()[i];
    pair<double,double> asympair = asym_err(lambda,nup,ndn);
    double asym = asympair.first; double easym = asympair.second;

    fval += pow(asym-asymfit,2) / pow(easym,2);
  }

}

// asymmetry & error pair:
pair<double,double> afterburner::asym_err(double lambda, double nup, double ndn)
{
  pair<double,double> result;

  double denom = ( (1.-lambda)*nup + (1.+lambda)*ndn );

  result.first = ( (1.-lambda)*nup - (1.+lambda)*ndn ) / denom; 
  result.second = 2.*(1.-pow(lambda,2))*Sqrt(nup*ndn*(nup+ndn)) / pow(denom,2);

  return result;

}
