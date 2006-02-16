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

double TAsym::GetMx(double TR, double thetaR){
    double mP=0.938;//GeV
    double T1;
    double p_1=100;//GeV
    double x0=0.013;
    double xx;
    double y;
    double L,D;
    int id;
    int iner_chan;
    double Ebeam;
    double Mx,p_R;
    double sin_thetaR;


    sin_thetaR=sin(thetaR*1E-3);

    Ebeam=sqrt(p_1*p_1+mP*mP);
    T1=Ebeam-mP;

    TR=TR*1E-3;//GeV
    p_R=sqrt(pow(TR,2)+2*mP*TR);
//    printf("xx=%lf L=%lf id=%d D=%lf geo=%d\n",xx,L,id,D,geo);
//    printf("TR=%lf Ebeam=%lf sin_thetaR=%lf\n",TR,Ebeam,sin_thetaR);
    Mx=pow(mP,2)-2*TR*(mP+Ebeam)+2*p_1*sqrt(2*mP*TR)*(sin_thetaR);
  //  if(Mx>0){
  //    Mx=sqrt(Mx);
  //  }else{
  //   Mx=3;
  //  }

   return Mx;
}

double TAsym::get_Theta(double TR){
    double mP=0.938;//GeV
    double T1;
    double p_1=100;//GeV
    double x0=0.013;
    double xx;
    double y;
    double L,D;
    int id;
    int iner_chan;
    double Ebeam;
    double sin_thetaR;
    double p_R;

    Ebeam=sqrt(p_1*p_1+mP*mP);
    T1=Ebeam-mP;

    TR=TR*1E-3;//GeV
    p_R=sqrt(pow(TR,2)+2*mP*TR);
//    printf("xx=%lf L=%lf id=%d D=%lf geo=%d\n",xx,L,id,D,geo);
//    printf("TR=%lf Ebeam=%lf sin_thetaR=%lf\n",TR,Ebeam,sin_thetaR);
    //Mx=pow(mP,2)-2*TR*(mP+Ebeam)+2*p_1*sqrt(2*mP*TR)*(sin_thetaR);
    sin_thetaR=(mP+Ebeam)/(double)p_1*sqrt(TR/(double)(2*mP));

   return sin_thetaR;
}

double TAsym::get_Energy(double theta){
    double mP=0.938;//GeV
    double T1;
    double p_1=100;//GeV
    double x0=0.013;
    double xx;
    double y;
    double L,D;
    int id;
    int iner_chan;
    double Ebeam;
    double sin_thetaR;
    double TR;
  

    Ebeam=sqrt(p_1*p_1+mP*mP);
    T1=Ebeam-mP;

   sin_thetaR=sin(theta*1E-3);

   TR=2*mP*pow(sin_thetaR*p_1/(double)(mP+Ebeam),2);
   TR=TR*1E3;//MeV
   return TR;
}

double TAsym::ch2theta(int geo){
    int iner_chan;
    int id;
    double xx,D,L;
    double sin_thetaR;
    double x0=0.008;

    iner_chan=geo%16;
    if(iner_chan==0){
        iner_chan=16;
        id=(int)(geo/16);
    }else{
        id=(int)(geo/16)+1;
    }
        
    xx=iner_chan*0.004;//1ch=4mm 
    if(id==1 || id==6){
     D=0.7725;//0.8-0.0275
     xx=0.08*(iner_chan+0.5)/(double)18;//1ch=80/18 mm 
    }
    if(id==2 || id==5){
     D=0.7815;//0.8-0.0185
     //xx=0.07*(iner_chan-1+0.5)/(double)16;//1ch=70/16 mm 
     xx=0.07*(iner_chan+0.5)/(double)16;//1ch=70/16 mm 
    }
    if(id==3 || id==4){
     D=0.7915;//0.8-0.0085
     xx=0.08*(iner_chan+0.5)/(double)18;//1ch=80/18 mm 
    }
    L=sqrt(pow(D,2)+pow(xx+x0,2));
    sin_thetaR=(xx+x0)/(double)L;
    return sin_thetaR;
}
