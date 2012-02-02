//========================
//  Fitting with averaged waveform
//========================
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>

#include <iostream.h>
#include "TROOT.h"
#include "TFile.h"
#include "TRandom.h"
#include "TTree.h"
#include "TBranch.h"
#include "TClonesArray.h"
#include "TStopwatch.h"
#include "TH1.h"
#include "TH2.h"
#include <TF1.h>
#include "TMath.h"
#include "TStyle.h"

#include "rhicpol.h"
#include "rpoldata.h"
#include "rhic_offline.h"
#include "rhic_class.h"
#include "TCanvas.h"

double TAsym::GetInE(double Energy,int ch){
   int i;
   int BNL=0;
   double IncidentEnergy=Energy;
   double par[5];

   if((ch >=17 && ch<=32) || (ch>=65 && ch<=80))BNL=1;
   if(BNL==1 ){
      //par[0]=76.04;par[1]=-54.83;par[2]=19.31;par[3]=-3.595;par[4]=0.3423;par[5]=-0.01308;//old
      par[0]=78.59;par[1]=-57.08;par[2]=20.16;par[3]=-3.757;par[4]=0.3579;par[5]=-0.01368;//REVISED
      if(Energy > 7.25){
            //IncidentEnergy=Energy;
            IncidentEnergy=-1;
      }else{
            IncidentEnergy=par[0]+par[1]*Energy+par[2]*pow(Energy,2)+par[3]*pow(Energy,3)
                               +par[4]*pow(Energy,4)+par[5]*pow(Energy,5); 
      }
   }else{
      //par[0]=74.91;par[1]=-57.69;par[2]=21.63;par[3]=-4.28;par[4]=0.4323;par[5]=-0.0175;//old
      par[0]=77.49;par[1]=-59.73;par[2]=22.32;par[3]=-4.397;par[4]=0.4419;par[5]=-0.01779;//REVISED
      if(Energy > 7.0){
            //IncidentEnergy=Energy;
            IncidentEnergy=-1;
      }else{
            IncidentEnergy=par[0]+par[1]*Energy+par[2]*pow(Energy,2)+par[3]*pow(Energy,3)
                               +par[4]*pow(Energy,4)+par[5]*pow(Energy,5); 
      }
   }
   return IncidentEnergy;
}

