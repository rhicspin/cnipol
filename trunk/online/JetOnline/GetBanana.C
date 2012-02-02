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


double TAsym::banana(double Ekin,int chan){
  double ans; 
  double tof;
  double D=0.8;
  double x0=0.005;// from center
  double xx=0.02; //HAMAMATSU 8cm
  double L;
  double mP=0.938;
  double Ekin0=0;
  double beta_C,beta_P;
  double light_C=3E+8;
  double v_P,v_C;
  double y;
  int id;

  id=(int)(chan/16)+1;
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

  Ekin=Ekin0+Ekin;//MeV
  Ekin=Ekin*1E-3;//GeV
  v_P=sqrt(2*Ekin/(double)mP)*light_C;
  tof=L/v_P; 

  ans = tof*1E+9; //ns


  return ans;
}

