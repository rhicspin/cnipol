#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <iostream.h>

#include "TROOT.h"
#include "TFile.h"
#include "TRandom.h"
#include "TTree.h"
#include "TBranch.h"
#include "TClonesArray.h"
#include "TStopwatch.h"
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>

#include "rhicpol.h"
#include "rpoldata.h"
#include "rhic_offline.h"
#include "rhic_class.h"


double TAsym::get_Mp(double Ekin,double tof,double geta,int frag){
  double ans; 
  double D=0.8;
  double x0=0.013;// from center
  double xx=0.02; //HAMAMATSU 8cm
  double L;
  double mP=0.938;
  double Ekin0=0;
  double beta_C,beta_P;
  double light_C=3E+8;
  double v_P,v_C;
  double y;
  int id;
  double Mp;


//  printf("Ekin=%lf tof=%lf\n",Ekin,tof);

  D=0.7915;//0.8-0.0085
  if(frag==1)D=0.7915-0.03;//0.8-0.0085
  xx=0.02;
  y=0.057;
  L=sqrt(pow(D,2)+pow(xx+x0,2)+pow(y,2));

  Ekin=Ekin0+Ekin;//MeV
  Ekin=Ekin*1E-3;//GeV
  tof=(tof-geta)*1E-9;//sec

  Mp=pow(light_C*tof/(double)L,2)*2*Ekin;//GeV
 //printf("L=%lf Ekin=%lf  %lf \n",L,Ekin,light_C*tof);
 //printf("Mp=%lf\n",Mp);

  ans =Mp; 


  return ans;
}

double TAsym::get_ToF(double Ekin,int chan){
  double ans; 
  double D=0.8;
  double x0=0.013;// from center
  double xx=0.02; //HAMAMATSU 8cm
  double mP=0.938;
  double beta_C,beta_P;
  double light_C=3E+8;
  double v_P,v_C;
  int id;
  double tof;
  double y,L;


//  printf("Ekin=%lf tof=%lf\n",Ekin,tof);
  if(chan%16==0)id=chan/(int)16;
  if(chan%16!=0)id=(int)(chan/16)+1;
  if(id==1 || id==6){
     D=0.7725;//0.8-0.0275
     xx=0.02;
     y=0.057;
  }
  if(id==2 || id==5){
     D=0.7815;//0.8-0.0185
     xx=0.0175;
     y=0;
  }
  if(id==3 || id==4){
     D=0.7915;//0.8-0.0085
     xx=0.02;
     y=0.057;
  }
  L=sqrt(pow(D,2)+pow(xx+x0,2)+pow(y,2));

  Ekin=Ekin*1E-3;//GeV
  v_P=sqrt(2*Ekin/(double)mP);
  tof=L/(double)v_P*10/(double)3; //ns

  ans =tof; 


  return ans;
}
