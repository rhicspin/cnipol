#include <iostream>
#include <fstream>
using namespace std;

#include "TF1.h"
#include "TH1.h"

gROOT->Reset();

//number of histograms to loop over
const int dup = 5;

//parameter seeds for two peaks
double pars[] = {2000., 4.5, 2., 18000., 11., 3.};

//root file name
TFile* f = new TFile("hbook/7804.002.root");

//global variables
char name[100];
char diagram[100];
char param[100];
TH1* h[dup+1];
TCanvas* c[dup+1];
double amp_1[dup+1];
double mu_1[dup+1];
double sig_1[dup+1];
double amp_2[dup+1];
double mu_2[dup+1];
double sig_2[dup+1];

//start main program
void RatioCalc() {

  for (int j=1;j<dup+1;j++) {
    cout<<"j is now "<<j<<endl;
    TwoGausFit(j);
  }
}


void TwoGausFit(int i) {

  cout<<"i is now "<<i<<endl;

  c[i] = new TCanvas();
  sprintf(name,"h%i",i+16000);
  cout<<"name is now "<<name<<endl;
  h[i]=(TH1*)f->Get(name);
  h[i]->Draw();

  TF1* myFit = new TF1("myFit", gFitFunction,0.,30.,6);

  myFit->SetParameters(pars);
  myFit->SetNpx(1000.);

  h[i]->Fit(myFit,"R");

  sprintf(diagram,"./gausouts/diagrams/gausfit_strip%i.eps",i);
  c[i]->SaveAs(diagram);

  amp_1[i] = myFit->GetParameter(0);
  mu_1[i] = myFit->GetParameter(1);
  sig_1[i] = sqrt(myFit->GetParameter(2));
  amp_2[i] = myFit->GetParameter(3);
  mu_2[i] = myFit->GetParameter(4);
  sig_2[i] = sqrt(myFit->GetParameter(5));

  cout<<"amp_1 is "<<amp_1[i]<<endl;

  double lowerbd = mu_2[i]-3*sig_2[i];
  cout<<"lower bound is "<<lowerbd<<endl;

  double upperbd = mu_2[i]+3*sig_2[i];
  cout<<"upper bound is "<<upperbd<<endl;

  TF1* FirstPeak = new TF1("FirstPeak","gaus(0)",0,30);
  FirstPeak->SetParameter(0,amp_1[i]);
  FirstPeak->SetParameter(1,mu_1[i]);
  FirstPeak->SetParameter(2,sig_1[i]);

  double Firstint = FirstPeak->Integral(lowerbd, 20);
  cout<<"Alpha peak integral = "<<Firstint<<endl;

  TF1* SecondPeak = new TF1("SecondPeak","gaus(0)",0,30);
  SecondPeak->SetParameter(0,amp_2[i]);
  SecondPeak->SetParameter(1,mu_2[i]);
  SecondPeak->SetParameter(2,sig_2[i]);

  double Secondint = SecondPeak->Integral(lowerbd, upperbd);
  cout<<"Carbon peak integral = "<<Secondint<<endl;

  double ratio = Firstint/Secondint;
  cout<<"Ratio of Alpha integral to Carbon Int = "<<ratio<<endl;
  
  sprintf(param,"./gausouts/params/strip%i.txt",i);
  ofstream myfile;
  myfile.open(param);
  myfile<<"parameters for strip "<<i<<endl;
  myfile<<"amp-1:\t\t"<<myFit->GetParameter(0)<<endl;
  myfile<<"mu-1:\t\t"<<myFit->GetParameter(1)<<endl;
  myfile<<"sig-1:\t\t"<<myFit->GetParameter(2)<<endl;
  myfile<<"amp-2:\t\t"<<myFit->GetParameter(3)<<endl;
  myfile<<"mu-2:\t\t"<<myFit->GetParameter(4)<<endl;
  myfile<<"sig-2:\t\t"<<myFit->GetParameter(5)<<endl;
  myfile<<"alpha-int:\t"<<Firstint<<endl;
  myfile<<"carbon-int:\t"<<Secondint<<endl;
  myfile<<"ratio:\t\t"<<ratio<<endl;
  myfile.close();

}

double gFitFunction(double* xs, double* pars) {

  double x = xs[0];
  
  double n1 = pars[0];
  double mu1 = pars[1];
  double sig1 = pars[2];
  double n2 = pars[3];
  double mu2 = pars[4];
  double sig2 = pars[5];




  double g1 = n1*exp(-0.5*(x-mu1)*(x-mu1)/sig1);
  double g2 = n2*exp(-0.5*(x-mu2)*(x-mu2)/sig2);

  return g1+g2;
}
