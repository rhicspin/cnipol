// ================
// readMxChan  Okada
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

int TAsym::readMxChan(MxChan *chan){
 FILE *chan_infile;
 int i,j,k,l;
 char sdmy[256];
 char iname[256];
 int idmy;
 int ch_min,ch_max;
 int d_si=15;
 int si_num;
 int start_ch[6];
 int energy_bin = 11;


 start_ch[0]=1;
 start_ch[1]=17;
 start_ch[2]=33;
 start_ch[3]=49;
 start_ch[4]=65;
 start_ch[5]=81;

 

//Comment//////////////

if(chan->term==0) sprintf(iname,"/home/okadah/Jet2005/JetOnline/chanSelect_2104.txt");//3ch  mode
if(chan->term>0) sprintf(iname,"/home/okadah/Jet2005/JetOnline/chanSelect.txt");//3ch  mode
//printf("term=%d\n",chan->term);getchar();
 if((chan_infile=fopen(iname,"r"))==NULL){
  printf("ERROR\n");
  printf("%s\n",iname);
  getchar();
 }else{
   //fscanf(chan_infile,"%s",&sdmy);
   //printf("%s\n",sdmy);
   for(j=0;j<6;++j){//Si
     for(i=0;i<energy_bin;++i){//Energy_BIN
        fscanf(chan_infile,"%d,%d,%d,%d,%d,%d",&idmy,&si_num,&chan->select_minY[i][j],
               &chan->select_maxY[i][j],&chan->select_minB[i][j],&chan->select_maxB[i][j]);
//        printf("bin=%d si=%d %d %d\n",i,j,chan->select_minY[i][j],chan->select_maxY[i][j],
//                              chan->select_minB[i][j],chan->select_maxB[i][j]);
        si_num=j;
        chan->select_minY2[i][j]=chan->select_minY[i][j];
        chan->select_maxY2[i][j]=chan->select_maxY[i][j];
        chan->select_minB2[i][j]=chan->select_minB[i][j];
        chan->select_maxB2[i][j]=chan->select_maxB[i][j];

        if(chan->select_minB2[i][j]>start_ch[j]+8){
          chan->select_minB2[i][j]=chan->select_minB2[i][j]-1;
        }else{
          chan->select_maxB2[i][j]=chan->select_maxB2[i][j]+1;
        }
        if(chan->select_maxB2[i][j]<start_ch[j]+15){
          chan->select_maxB2[i][j]=chan->select_maxB2[i][j]+1;
        }else{
          chan->select_minB2[i][j]=chan->select_minB2[i][j]-1;
        }

        if(chan->select_minY2[i][j]>start_ch[j]){
          chan->select_minY2[i][j]=chan->select_minY2[i][j]-1;
        }else{
          chan->select_maxY2[i][j]=chan->select_maxY2[i][j]+1;
        }
        if(chan->select_maxY2[i][j]<start_ch[j]+7){
          chan->select_maxY2[i][j]=chan->select_maxY2[i][j]+1;
        }else{
          chan->select_minY2[i][j]=chan->select_minY2[i][j]-1;
        }

//        printf("*************Energy-bin=%d si=%d %d|| minY2=%d maxY2=%d |%d|| minB2=%d maxB2=%d|%d\n",
//        i,j,start_ch[j],chan->select_minY2[i][j],chan->select_maxY2[i][j],start_ch[j]+8,chan->select_minB2[i][j],chan->select_maxB2[i][j],start_ch[j]+15);
     }
//      getchar();
   }
   fclose(chan_infile);
/*
//kakunin
   for(i=0;i<energy_bin;++i){
       for(j=0;j<6;++j){//Si
          printf("BIN=%d Si=%d min=%d,max=%d ",i,j,chan->select_minY[i][j],chan->select_maxY[i][j]);
          printf(" min=%d,max=%d\n",chan->select_minB[i][j],chan->select_maxB[i][j]);
       }
       getchar();
   }
*/
   if(energy_bin+1<32){
     for(i=(energy_bin+1);i<32;++i){
       for(j=0;j<6;++j){//Si
        chan->select_minY[i][j]=start_ch[j];
        chan->select_maxY[i][j]=start_ch[j]+7;
        chan->select_minY[i][j]=start_ch[j]+8;
        chan->select_maxY[i][j]=start_ch[j]+15;
       }
     }
   }
 }
 return(0);
}


