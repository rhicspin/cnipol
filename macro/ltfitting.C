#include "ltfitting.hpp"
#include "likelihood.C"
//#include "likelihood2.C"

#include "TH2F.h"
#include "TF1.h"
#include "TLatex.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TFile.h"
#include "TMatrixD.h"
 
void ltfitting() { //ltfitting loop

  minvalue[0] = MinuitFit();
  printf("minvalue = %0.5f \n", minvalue[0]);
  
  gMinuit->GetParameter(0,getpar[0],geterr[0]);
  gMinuit->GetParameter(1,getpar[1],geterr[1]);
  gMinuit->GetParameter(2,getpar[2],geterr[2]);

  int nfixpar = gMinuit->GetNumFixedPars();
  int nfrepar = gMinuit->GetNumFreePars();
  int npar    = gMinuit->GetNumPars();

  if (npar > 2 ) gMinuit->FixParameter(2) ; // Need to fix rho to plot the contour of Imr5 and Rer5
  TMatrixD matrix(2,2);
  gMinuit->mnemat(&matrix[0][0],2);
  
  ofstream Outputfile;
  Outputfile.open("allparvalue.txt");
  Outputfile<<"par name"<<"                  "<<"par_value"<<"          "<<"err_value"<<endl;
  Outputfile<<"--------------------------------------------------------- -----------"<<endl;
  Outputfile<<"Rer5                      "<<getpar[0]<<"              "<<geterr[0]<<endl;
  Outputfile<<"Imr5                      "<<getpar[1]<<"              "<<geterr[1]<<endl;
  if (npar > 2 ) 
    Outputfile<<"Rho                       "<<getpar[2]<<"              "<<geterr[2]<<endl;
  Outputfile<<"------------------------------------------------------------------------"<<endl;
  Outputfile<<"         Minvalue             "<<minvalue[0]<<endl;
  Outputfile<<" Total   free  parameters     "<<npar<<endl;
  //  Outputfile<<" total candidates went to likelihood fit:    "<<Nevts<<endl;
  //  Outputfile<<" fit is agginging:: fsig*total_candidate:    "<<Nevts*getpar[0]<<"(+-"<<Nevts*geterr[0]<<")"<<endl;

  Int_t ngraphs = 0 ;
  TGraph *Cgr[3]; 

  cout<<"     I  reached   till   here  ---  1111111    "<<endl;
  cout<<"     I  reached   till   here  ---  1111111    "<<endl;
  gMinuit->SetErrorDef(1.);
  Cgr[0] = (TGraph*)gMinuit->Contour(160,0,1);
  Cgr[0]->SetTitle("1 sigma contour");
  cout<<"     I  reached   till   here  ---  2222222    "<<endl;
  cout<<"     I  reached   till   here  ---  2222222    "<<endl;

  ngraphs++ ;

  //  gMinuit->SetErrorDef(1.21);
  gMinuit->SetErrorDef(4.);
  Cgr[1] = (TGraph*)gMinuit->Contour(320,0,1);
  Cgr[1]->SetTitle("2 sigma contour");
  cout<<"     I  reached   till   here  ---  6666666    "<<endl;
  cout<<"     I  reached   till   here  ---  6666666    "<<endl;
    
  ngraphs++ ;
  //  gMinuit->SetErrorDef(1.44);
  gMinuit->SetErrorDef(9.);
  Cgr[2] = (TGraph*)gMinuit->Contour(480,0,1);
  Cgr[2]->SetTitle("3 sigma contour");
  cout<<"     I  reached   till   here  ---  7777777    "<<endl;
  cout<<"     I  reached   till   here  ---  7777777    "<<endl;
  ngraphs++ ;


  
  TFile *f = new TFile("hist_ltfitting.root","RECREATE");
  char fname1[50]; 
  for(int i=0; i<ngraphs; i++){  
    strcpy(fname1,"");  sprintf(fname1,"Cgr%d",i);   Cgr[i]->Write(fname1);
    cout<<i<<"     writing  graph name     "<<fname1<<endl;
  }  

  cout <<     "Rer5                      "<<getpar[0]<<"              "<<geterr[0]<<endl;
  cout <<     "Imr5                      "<<getpar[1]<<"              "<<geterr[1]<<endl;
  if (npar > 2 ) 
    cout <<     "Rho                       "<<getpar[2]<<"              "<<geterr[2]<<endl;

  cout.precision(10);
  cout << " ========= Error Matrix and its Inverse ========= " << endl ;
  matrix.Print();
  cout << matrix[0][0] << " " << matrix[0][1] << endl ;
  cout << matrix[1][0] << " " << matrix[1][1] << endl ;
  //  matrix.Draw("text");
  cout << "Determinant : " << matrix.Determinant() << endl ;
  matrix.Invert();   matrix.Print();   
  cout << matrix[0][0] << " " << matrix[0][1] << endl ;
  cout << matrix[1][0] << " " << matrix[1][1] << endl ;
  cout << "Determinant : " << matrix.Determinant() << endl ;
  cout << " ================================================= " << endl ;

  /*
  cout << "Combined : " << endl ;
  matrix[0][0] = 4.13439944E-05 ; 
  matrix[0][1] = 3.38166050E-04 ; 
  matrix[1][0] = 3.38166050E-04 ; 
  matrix[1][1] = 0.00330779 ; 
  matrix.Print();
  cout << "Determinant : " << matrix.Determinant() << endl ;

  matrix.Invert();   matrix.Print();   
  cout << matrix[0][0] << " " << matrix[0][1] << endl ;
  cout << matrix[1][0] << " " << matrix[1][1] << endl ;
  cout << "Determinant : " << matrix.Determinant() << endl ;
  */

  // without phi0
  cout << "Combined : " << endl ;
  /*
  matrix[0][0] = 4.07894112E-05 ;
  matrix[0][1] = 3.33673244E-04 ; 
  matrix[1][0] = 3.33673244E-04 ; 
  matrix[1][1] = 0.00325836 ; 
  */

  // Feb. 23, 2012 : After putting 1/sqrt(2) on Leff
  matrix[0][0] = 4.01995010E-05 ;
  matrix[0][1] = 3.29621285E-04 ; 
  matrix[1][0] = 3.29621285E-04 ; 
  matrix[1][1] = 0.00323057 ; 
  matrix.Print();
  cout << "Determinant : " << matrix.Determinant() << endl ;

  matrix.Invert();   matrix.Print();   
  cout << matrix[0][0] << " " << matrix[0][1] << endl ;
  cout << matrix[1][0] << " " << matrix[1][1] << endl ;
  cout << "Determinant : " << matrix.Determinant() << endl ;
  //

  return ;

}

