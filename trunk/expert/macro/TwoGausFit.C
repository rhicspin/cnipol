
#include <iostream>
#include <fstream>

#include "TF1.h"
#include "TH1.h"

using namespace std;

void TwoGausFit()
{
   TCanvas* c1 = new TCanvas("c1");
   h16001->Draw();

   TF1* myFit = new TF1("myFit", gFitFunction,0.,30.,6);

   double pars[] = {2000., 5., 3., 18000., 11., 3.};
   myFit->SetParameters(pars);
   myFit->SetNpx(1000.);

   //myFit->Draw("same");

   h16001->Fit(myFit,"R");

   c1->SaveAs("gausFit.eps");

   double amp_1 = myFit->GetParameter(0);
   double mu_1  = myFit->GetParameter(1);
   double sig_1 = sqrt(myFit->GetParameter(2));
   double amp_2 = myFit->GetParameter(3);
   double mu_2  = myFit->GetParameter(4);
   double sig_2 = sqrt(myFit->GetParameter(5));

   //  cout<<"carbon amp is "<<amp_2<<endl;
   //  cout<<"carbon peak location is "<<mu_2<<endl;
   //  cout<<"carbon sigma is "<<sig_2<<endl;

   double lowerbd = mu_2-3*sig_2;
   cout<<"lower bound is "<<lowerbd<<endl;

   double upperbd = mu_2+3*sig_2;
   cout<<"upper bound is "<<upperbd<<endl;

   TF1* carbonPeak = new TF1("carbonPeak","gaus(0)",0,30);
   carbonPeak->SetParameter(0,amp_2);
   carbonPeak->SetParameter(1,mu_2);
   carbonPeak->SetParameter(2,sig_2);

   double carbonint = carbonPeak->Integral(lowerbd, upperbd);
   cout<<"Carbon peak integral = "<<carbonint<<endl;

   TF1* alphaPeak = new TF1("alphaPeak","gaus(0)",0,30);
   alphaPeak->SetParameter(0,amp_1);
   alphaPeak->SetParameter(1,mu_1);
   alphaPeak->SetParameter(2,sig_1);

   double alphaint = alphaPeak->Integral(lowerbd, 20);
   cout<<"Alpha peak integral = "<<alphaint<<endl;

   double ratio = alphaint/carbonint;
   cout<<"Ratio of Alpha integral to Carbon Int = "<<ratio<<endl;

   ofstream myfile;
   myfile.open("parameters.txt");
   myfile<<"amp-1:\t\t"<<myFit->GetParameter(0)<<endl;
   myfile<<"mu-1:\t\t"<<myFit->GetParameter(1)<<endl;
   myfile<<"sig-1:\t\t"<<myFit->GetParameter(2)<<endl;
   myfile<<"amp-2:\t\t"<<myFit->GetParameter(3)<<endl;
   myfile<<"mu-2:\t\t"<<myFit->GetParameter(4)<<endl;
   myfile<<"sig-2:\t\t"<<myFit->GetParameter(5)<<endl;
   myfile<<"alpha-int:\t"<<alphaint<<endl;
   myfile<<"carbon-int:\t"<<carbonint<<endl;
   myfile<<"ratio:\t\t"<<ratio<<endl;
   myfile.close();
}

double gFitFunction(double* xs, double* pars) {

  double x    = xs[0];

  double n1   = pars[0];
  double mu1  = pars[1];
  double sig1 = pars[2];
  double n2   = pars[3];
  double mu2  = pars[4];
  double sig2 = pars[5];

  double g1 = n1*exp(-0.5*(x-mu1)*(x-mu1)/sig1);
  double g2 = n2*exp(-0.5*(x-mu2)*(x-mu2)/sig2);

  //cout<<"returning:\t"<<g1<<"+\t"<<g2<<endl;
  return g1+g2;
}
