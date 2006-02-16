// ================
// get_8elements  Okada
// ================
// target[Energy_bin][Si#][+/-/0]
// beam[Energy_bin][Si#][+/-/0]
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


int TAsym::reset8Elements(process8Elements *elements){
 int i,j,k,l;

 elements->plus_c=0;
 elements->minus_c=0;
 elements->zero_c=0;
 elements->plus=0;
 elements->minus=0;
 elements->zero=0;
 elements->valance_check=0;
 elements->count=0;
 for(i=0;i<32;++i){  //Energy Bin
  for(j=0;j<6;++j){  //Si#
   for(k=0;k<3;++k){ //+/-/0 beam
     for(l=0;l<3;++l){ //+/-/0 target
       elements->rawY[i][j][k][l]=0;
       elements->BGrawY[i][j][k][l]=0;
       elements->MMrawY[i][j][k][l]=0;
       elements->eachY[i][j][k][l]=0;
       elements->BGeachY[i][j][k][l]=0;
       elements->MMeachY[i][j][k][l]=0;
       elements->valanceY[i][j][k][l]=0;
       elements->BGvalanceY[i][j][k][l]=0;
       elements->MMvalanceY[i][j][k][l]=0;
       elements->rawB[i][j][k][l]=0;
       elements->BGrawB[i][j][k][l]=0;
       elements->MMrawB[i][j][k][l]=0;
       elements->eachB[i][j][k][l]=0;
       elements->BGeachB[i][j][k][l]=0;
       elements->MMeachB[i][j][k][l]=0;
       elements->valanceB[i][j][k][l]=0;
       elements->BGvalanceB[i][j][k][l]=0;
       elements->MMvalanceB[i][j][k][l]=0;
     }
   }
  }
 }
 return(0);
}

int TAsym::reset_each8Elements(process8Elements *elements){
 int i,j,k,l;
 elements->count=0;
 for(i=0;i<32;++i){  //Energy Bin
  for(j=0;j<6;++j){  //Si#
   for(k=0;k<3;++k){ //+/-/0 beam
     for(l=0;l<3;++l){ //+/-/0 target
       elements->eachY[i][j][k][l]=0;
       elements->BGeachY[i][j][k][l]=0;
       elements->MMeachY[i][j][k][l]=0;
       elements->eachB[i][j][k][l]=0;
       elements->BGeachB[i][j][k][l]=0;
       elements->MMeachB[i][j][k][l]=0;
     }
   }
  }
 }
 return(0);
}

int TAsym::sum_each8Elements(process8Elements *elements){
 int i,j,k,l;
 for(i=0;i<32;++i){  //Energy Bin
  for(j=0;j<6;++j){  //Si#
   for(k=0;k<3;++k){ //+/-/0 beam
     for(l=0;l<3;++l){ //+/-/0 target
       elements->valanceY[i][j][k][l]=elements->valanceY[i][j][k][l]+elements->eachY[i][j][k][l];
       elements->BGvalanceY[i][j][k][l]=elements->BGvalanceY[i][j][k][l]+elements->BGeachY[i][j][k][l];
       elements->MMvalanceY[i][j][k][l]=elements->MMvalanceY[i][j][k][l]+elements->MMeachY[i][j][k][l];
       elements->valanceB[i][j][k][l]=elements->valanceB[i][j][k][l]+elements->eachB[i][j][k][l];
       elements->BGvalanceB[i][j][k][l]=elements->BGvalanceB[i][j][k][l]+elements->BGeachB[i][j][k][l];
       elements->MMvalanceB[i][j][k][l]=elements->MMvalanceB[i][j][k][l]+elements->MMeachB[i][j][k][l];
     }
   }
  }
 }
 return(0);
}

int TAsym::setEbin(EnergyBin *bin){
     bin->Ebin[0] = 0.70;
     bin->Ebin[1] = 1.0;
     bin->Ebin[2] = 1.4;
     bin->Ebin[3] = 1.8;
     bin->Ebin[4] = 2.2;
     bin->Ebin[5] = 2.6;
     bin->Ebin[6] = 3.0;
     bin->Ebin[7] = 3.5;
     bin->Ebin[8] = 4.0;
     bin->Ebin[9] = 4.5;
     bin->Ebin[10] = 5.0;
     bin->Ebin[11] = 6.0;

      return(0);
}

int TAsym::GetBunchPat(MxChan *chan,processWEvent *wevent){
        int beam_frag=0;//blue
        int BunchY_0;
        int BunchB_0;
        BunchY_0=0;
        BunchB_0=117;
        if(wevent->WCMcount==54){
           BunchY_0=0;
           BunchB_0=0;
        }
        if(wevent->WCMcount==56){
           BunchY_0=0;
           BunchB_0=116;
        }
        if(wevent->WCMcount>=60){
           BunchY_0=0;
           BunchB_0=117;
        }
        if(wevent->geo%16 < 9 && wevent->geo%16>0){
          beam_frag=1;//yellow
          wevent->our_bunchY=(wevent->bunch-BunchY_0);
          if(wevent->our_bunchY<0)wevent->our_bunchY=wevent->our_bunchY+120;
          wevent->our_bunchY=wevent->our_bunchY%120;
        }else{
          //wevent->our_bunchB=(wevent->bunch+80)%120;
          wevent->our_bunchB=(wevent->bunch-BunchB_0);
          if(wevent->our_bunchB<0)wevent->our_bunchB=wevent->our_bunchB+120;
          wevent->our_bunchB=wevent->our_bunchB%120;
        }
       // wevent->our_bunchB=wevent->bunch;
       // wevent->our_bunchY=wevent->bunch;
}

int TAsym::Get8Elements(EnergyBin *bin,MxChan *chan,processWEvent *wevent,process8Elements *elements){
	int i,k,l;
        int si,ch_min,ch_max;
        double Ebin[32];
        int beam_pol; //+/-/0
        int beam_frag=0;//blue
/*
        if(wevent->geo%16 < 9 && wevent->geo%16>0){
          beam_frag=1;//yellow
          wevent->our_bunchY=(wevent->bunch)%120;
        }else{
          wevent->our_bunchB=(wevent->bunch+80)%120;
        }
        wevent->our_bunchB=wevent->bunch;
        wevent->our_bunchY=wevent->bunch;
*/
        si=(int)(wevent->geo/16);
        if(wevent->geo%16 ==0)si=si-1;

        int E_id = BinSelect(bin, chan,wevent);
        int chanY=0;
        int chanB=0;
        int sideY=0;
        int sideB=0;

        if(E_id >= 0 && E_id<12){
          if(wevent->geo >= chan->select_minY[E_id][si] && wevent->geo <= chan->select_maxY[E_id][si])chanY=1;
          if(chanY!=1 && wevent->geo >= chan->select_minY2[E_id][si] && wevent->geo <= chan->select_maxY2[E_id][si])chanY=2;
          if(wevent->geo%16 <=8 && wevent->geo%16>0 )sideY=1;
          if(wevent->geo >= chan->select_minB[E_id][si] && wevent->geo <= chan->select_maxB[E_id][si])chanB=1;
          if(chanB!=1 && wevent->geo >= chan->select_minB2[E_id][si] && wevent->geo <= chan->select_maxB2[E_id][si])chanB=2;
          if(wevent->geo%16 >8 || wevent->geo%16==0 )sideB=1;
        }
        if((chanY==1 || chanB==1 || sideY==1 || sideB==1 ) && wevent->Energy <= bin->Ebin[11] && wevent->Energy >= bin->Ebin[0]){

          //if(wevent->our_bunchY<112 || wevent->our_bunchB < 112){
          if(wevent->our_bunchY<121 || wevent->our_bunchB < 121){
            int tmp;
            if(beam_frag==0){//blue
              if(elements->PolPatB[wevent->our_bunchB] > 0) beam_pol=0;
              if(elements->PolPatB[wevent->our_bunchB] < 0) beam_pol=1;
              if(elements->PolPatB[wevent->our_bunchB] == 0) beam_pol=2;
/*
              tmp=wevent->our_bunchB%4;
              if(tmp==0)beam_pol=0;
              if(tmp==2)beam_pol=1;
              if(tmp==1 || tmp==3)beam_pol=2;
*/
            }else{//yellow
              if(elements->PolPatY[wevent->our_bunchY] > 0) beam_pol=0;
              if(elements->PolPatY[wevent->our_bunchY] < 0) beam_pol=1;
              if(elements->PolPatY[wevent->our_bunchY] == 0) beam_pol=2;
/*
              tmp=wevent->our_bunchY%8;
              if(tmp==0 || tmp==2)beam_pol=0;
              if(tmp==4 || tmp==6)beam_pol=1;
              if(tmp==1 || tmp==3 || tmp==5 || tmp==7)beam_pol=2;
*/
            }
          }else{
            beam_pol=2;
          }

//1='+',2='-',3='0'
/*
//target-shaffle
          if(wevent->target_inf <3){ //if target is + or -
            elements->count++;
            if( (elements->count)%2==0){
              wevent->target_inf=1;// target is +
            }else{
              wevent->target_inf=2;//target is -
            }
           }
*/         
/*
//target-shaffle
          if(wevent->target_inf <3){ //if target is + or -
             int m=rand();
             if(m%2==0){
              wevent->target_inf=1;// target is +
             }else{
              wevent->target_inf=2;//target is -
             }
          }
*/

//0='+',1='-',2='0'
          if(chanY==1){
            elements->rawY[E_id][si][beam_pol][wevent->target_inf-1]++;
            elements->eachY[E_id][si][beam_pol][wevent->target_inf-1]++;
          }
          if(chanY>=1){
            elements->MMrawY[E_id][si][beam_pol][wevent->target_inf-1]++;
            elements->MMeachY[E_id][si][beam_pol][wevent->target_inf-1]++;
          }
          if(sideY==1){
            elements->BGrawY[E_id][si][beam_pol][wevent->target_inf-1]++;
            elements->BGeachY[E_id][si][beam_pol][wevent->target_inf-1]++;
          }
          if(chanB==1){
            elements->rawB[E_id][si][beam_pol][wevent->target_inf-1]++;
            elements->eachB[E_id][si][beam_pol][wevent->target_inf-1]++;
          }
          if(chanB>=1){
            elements->MMrawB[E_id][si][beam_pol][wevent->target_inf-1]++;
            elements->MMeachB[E_id][si][beam_pol][wevent->target_inf-1]++;
          }
          if(sideB==1){
            elements->BGrawB[E_id][si][beam_pol][wevent->target_inf-1]++;
            elements->BGeachB[E_id][si][beam_pol][wevent->target_inf-1]++;
          }
        }
        return(0);
}

int TAsym::BinSelect(EnergyBin *bin,MxChan *chan,processWEvent *wevent){
        int i,k,l;
        int si,ch_min,ch_max;
        double Ebin[32];
        int beam_pol; //+/-/0
        int bunch0,bunchmax;
        int punch_frag=0;
        int si_typ=1;//BNL=0, HAMAMATSU=1


        si=(int)(wevent->geo/16);
        if(wevent->geo%16 ==0)si=si-1;
//Alpha source remve
        int alpha_frag=0;
        if(wevent->Energy >=4.7 && wevent->Energy < 5.7)alpha_frag=1;
        if(wevent->Energy >=2.5 && wevent->Energy < 3.0)alpha_frag=1;

        int E_id=-1;
        if(bin->Ebin[0]> wevent->Energy)E_id=-1;
        if(bin->Ebin[0] <= wevent->InE && bin->Ebin[1] > wevent->InE)E_id=0;//0.7-1.0 MeV
        if(bin->Ebin[1] <= wevent->InE && bin->Ebin[2] > wevent->InE)E_id=1;//1.0-1.4 MeV
        if(bin->Ebin[2] <= wevent->InE && bin->Ebin[3] > wevent->InE)E_id=2;//1.4-1.8 MeV
        if(bin->Ebin[3] <= wevent->InE && bin->Ebin[4] > wevent->InE)E_id=3;//1.8-2.2 MeV
        if(bin->Ebin[4] <= wevent->InE && bin->Ebin[5] > wevent->InE)E_id=4;//2.2-2.6 MeV
        if(bin->Ebin[5] <= wevent->InE && bin->Ebin[6] > wevent->InE)E_id=5;//2.6-3.0 MeV
        if(bin->Ebin[6] <= wevent->InE && bin->Ebin[7] > wevent->InE)E_id=6;//3.0-3.5 MeV
        if(bin->Ebin[7] <= wevent->InE && bin->Ebin[8] > wevent->InE)E_id=7;//3.5-4.0 MeV
        if(bin->Ebin[8] <= wevent->InE && bin->Ebin[9] > wevent->InE)E_id=8;//4.0-4.5 MeV
        if(bin->Ebin[9] <= wevent->InE && bin->Ebin[10] > wevent->InE)E_id=9;//4.5-5.0 MeV
        if(bin->Ebin[10] <= wevent->InE && bin->Ebin[11] > wevent->InE)E_id=10;//5.0-6.0 MeV

        return(E_id);
}

