// ================
// elements2asym  Okada
// ================
// raw[Energy_bin][Si#][beam+/-/0][target+/-/0]
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

int TAsym::resetAsym(processAsym *asym){
  int i,j;
  for(i=0;i<32;++i){
   for(j=0;j<4;++j){
    asym->TargetPhys[i][j]=0;
    asym->BeamPhys[i][j]=0;
    asym->TargetLumi[i][j]=0;
    asym->BeamLumi[i][j]=0;
    asym->TargetAcc[i][j]=0;
    asym->BeamAcc[i][j]=0;
    asym->TargetErr[i][j]=0;
    asym->BeamErr[i][j]=0;

    asym->DoublePhys[i][j]=0;
    asym->DoubleLumi[i][j]=0;
    asym->DoubleAcc[i][j]=0;
   }
  }
  return(0);
}

int TAsym::Elements2Asym(process8Elements *elements,processAsym *asym){
	int i,j,k,l;
        int target[32][8][3];
        int beam[32][8][3];

        double a[4],b[4];

        for(i=0;i<32;++i){
         for(k=0;k<8;++k){
          for(l=0;l<3;++l){
            target[i][k][l]=0;
            beam[i][k][l]=0;
          }
         }
        }
/*
//CHECK
       elements->raw[0][0][0][0]=250;elements->raw[0][0][0][1]=267;elements->raw[0][0][1][0]=285;elements->raw[0][0][1][1]=245;
       elements->raw[0][1][0][0]=385;elements->raw[0][1][0][1]=370;elements->raw[0][1][1][0]=385;elements->raw[0][1][1][1]=381;
       elements->raw[0][2][0][0]=286;elements->raw[0][2][0][1]=272;elements->raw[0][2][1][0]=298;elements->raw[0][2][1][1]=300;
       elements->raw[0][3][0][0]=267;elements->raw[0][3][0][1]=351;elements->raw[0][3][1][0]=258;elements->raw[0][3][1][1]=303;
       elements->raw[0][4][0][0]=357;elements->raw[0][4][0][1]=411;elements->raw[0][4][1][0]=356;elements->raw[0][4][1][1]=381;
       elements->raw[0][5][0][0]=264;elements->raw[0][5][0][1]=316;elements->raw[0][5][1][0]=271;elements->raw[0][5][1][1]=291;
*/
        for(i=0;i<32;++i){
          for(j=0;j<6;++j){ //Si#
            for(k=0;k<2;++k){//beam_pol
              for(l=0;l<2;++l){//target
                 target[i][j][l]=target[i][j][l]+elements->rawY[i][j][k][l]; 
                 beam[i][j][k]=beam[i][j][k]+elements->rawY[i][j][k][l]; 
                 if(j<3){
                   target[i][6][l]=target[i][6][l]+elements->rawY[i][j][k][l]; 
                   beam[i][6][k]=beam[i][6][k]+elements->rawY[i][j][k][l]; 
                 }else{
                   target[i][7][l]=target[i][7][l]+elements->rawY[i][j][k][l]; 
                   beam[i][7][k]=beam[i][7][k]+elements->rawY[i][j][k][l]; 
                 }
              }
            }
          }
        }
//PHYS ASYMMETRY
        for(i=0;i<32;++i){
          for(j=0;j<3;++j){
            asym->TargetPhys[i][j]=(sqrt(target[i][j][0]*target[i][j+3][1])-sqrt(target[i][j][1]*target[i][j+3][0]))
                       /(sqrt(target[i][j][0]*target[i][j+3][1])+sqrt(target[i][j][1]*target[i][j+3][0]));
            asym->BeamPhys[i][j]=(sqrt(beam[i][j][0]*beam[i][j+3][1])-sqrt(beam[i][j][1]*beam[i][j+3][0]))
                       /(sqrt(beam[i][j][0]*beam[i][j+3][1])+sqrt(beam[i][j][1]*beam[i][j+3][0]));
          }
          asym->TargetPhys[i][3]=(sqrt(target[i][6][0]*target[i][7][1])-sqrt(target[i][6][1]*target[i][7][0]))
                     /(sqrt(target[i][6][0]*target[i][7][1])+sqrt(target[i][6][1]*target[i][7][0]));
          asym->BeamPhys[i][3]=(sqrt(beam[i][6][0]*beam[i][7][1])-sqrt(beam[i][6][1]*beam[i][7][0]))
                     /(sqrt(beam[i][6][0]*beam[i][7][1])+sqrt(beam[i][6][1]*beam[i][7][0]));
        }
//LUMINOSITY ASYMMETRY
        for(i=0;i<32;++i){
          for(j=0;j<3;++j){
            asym->TargetLumi[i][j]=(sqrt(target[i][j][0]*target[i][j+3][0])-sqrt(target[i][j][1]*target[i][j+3][1]))
                       /(sqrt(target[i][j][0]*target[i][j+3][0])+sqrt(target[i][j][1]*target[i][j+3][1]));
            asym->BeamLumi[i][j]=(sqrt(beam[i][j][0]*beam[i][j+3][0])-sqrt(beam[i][j][1]*beam[i][j+3][1]))
                       /(sqrt(beam[i][j][0]*beam[i][j+3][0])+sqrt(beam[i][j][1]*beam[i][j+3][1]));
          }
          asym->TargetLumi[i][3]=(sqrt(target[i][6][0]*target[i][7][0])-sqrt(target[i][6][1]*target[i][7][1]))
                    /(sqrt(target[i][6][0]*target[i][7][0])+sqrt(target[i][6][1]*target[i][7][1]));
          asym->BeamLumi[i][3]=(sqrt(beam[i][6][0]*beam[i][7][0])-sqrt(beam[i][6][1]*beam[i][7][1]))
                    /(sqrt(beam[i][6][0]*beam[i][7][0])+sqrt(beam[i][6][1]*beam[i][7][1]));
        }
//ACCEPTANCE ASYMMETRY
        for(i=0;i<32;++i){
          for(j=0;j<3;++j){
            asym->TargetAcc[i][j]=(sqrt(target[i][j][0]*target[i][j][1])-sqrt(target[i][j+3][1]*target[i][j+3][0]))
                       /(sqrt(target[i][j][0]*target[i][j][1])+sqrt(target[i][j+3][1]*target[i][j+3][0]));
            asym->BeamAcc[i][j]=(sqrt(beam[i][j][0]*beam[i][j][1])-sqrt(beam[i][j+3][1]*beam[i][j+3][0]))
                       /(sqrt(beam[i][j][0]*beam[i][j][1])+sqrt(beam[i][j+3][1]*beam[i][j+3][0]));
          }
          asym->TargetAcc[i][3]=(sqrt(target[i][6][0]*target[i][6][1])-sqrt(target[i][7][1]*target[i][7][0]))
                     /(sqrt(target[i][6][0]*target[i][6][1])+sqrt(target[i][7][1]*target[i][7][0]));
          asym->BeamAcc[i][3]=(sqrt(beam[i][6][0]*beam[i][6][1])-sqrt(beam[i][7][1]*beam[i][7][0]))
                     /(sqrt(beam[i][6][0]*beam[i][6][1])+sqrt(beam[i][7][1]*beam[i][7][0]));
        }
//ERROR
        for(i=0;i<32;++i){
          for(j=0;j<3;++j){
            asym->TargetErr[i][j]=1/sqrt(target[i][j][0]+target[i][j+3][1]+target[i][j][1]+target[i][j+3][0]);
            asym->BeamErr[i][j]=1/sqrt(beam[i][j][0]+beam[i][j+3][1]+beam[i][j][1]+beam[i][j+3][0]);
          }
          asym->TargetErr[i][3]=1/sqrt(target[i][6][0]+target[i][7][1]+target[i][6][1]+target[i][7][0]);
          asym->BeamErr[i][3]=1/sqrt(beam[i][6][0]+beam[i][7][1]+beam[i][6][1]+beam[i][7][0]);
        }
/*
//CHECK
        printf("CHECK============\n");
        printf("BEAM---------------\n");
        printf("Phys  Lumi  Acc  Err\n");
        for(int k=0;k<4;++k){
         printf("%lf %lf %lf %lf\n",asym->BeamPhys[0][k],asym->BeamLumi[0][k],
              asym->BeamAcc[0][k],asym->BeamErr[0][k]);
        }
        printf("Target-------------------\n");
        printf("Phys  Lumi  Acc  Err\n");
        for(int k=0;k<4;++k){
         printf("%lf %lf %lf %lf\n",asym->TargetPhys[0][k],asym->TargetLumi[0][k],
              asym->TargetAcc[0][k],asym->TargetErr[0][k]);
        }
*/
        return(0);
}


