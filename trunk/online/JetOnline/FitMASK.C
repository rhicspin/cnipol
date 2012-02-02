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
#include "TMath.h"
#include "TStyle.h"
#include "TCanvas.h"



double Fitf2(double *x,double *par); 


int TAsym::FitMASK(double FitMaskResult[]) 
{
  FILE *input_MASK;
  int t1,t2,t3,t4,hist_content,hit,ph_max,k_max;
  int range_min,range_max;
  int hist_range,max,min,bin;
  int hist_content_old;
  double par[4],par_fix[2];
  double par_err[4];
  double chi,chi_err;
  int ndf;
  int intg1_cont,intg2_cont,intg3_cont;
  double sqrt_sum;
  int limit1;//p1_max
  int limit2;//p2_max
//for AGS   change , search AGS
  int wave[90];
  double residual[90],mask_tmp[90],tmp_error[90];
//for RHIC change ,search RHIC 
//  int wave[42];
//  double residual[42];

  int p1_max,p2_max;
  int RGB_timing_check=0;//count max timimg==1/4timing
  //int g_ats[7]; //max_ph,  1/4timing,  Integ,  max-timing ,10%-timing ,90%-timing,50%-timing
  double wf_p;//max_ph from Waveform itself
  double wf_t;//1/4max from Waveform itself
  double wf_tmax;//max timing from Waveform itself
  double wf_t10;//10%max from Waveform itself
  double wf_t90;//90%max from Waveform itself
  double wf_t50;//90%max from Waveform itself
  int intg_middle;
  double Pmax,Pmax_error;
  double t0,t0_error;
  int MASK_quater[2];
  double fbin,value;
  double fbinmax;
  double fbin10;
  double fbin90;
  double fbin50;
  double intg_fit;
  double MASK_WAVE_max,MASK_WAVE_quater,MASK_WAVE_tmax,MASK_WAVE_intg,MASK_intg;
  double MASK_mach,sum_MASK,sum_MASK_k,MASK_peak;
  int MASK_max_k,MASK_start,MASK_stop;
  double ratio;
  int idmy;
  double MASK_error[90],MASK[90];
  double MASK_error30[90];
  double MASK_error40[90];
  double MASK_error50[90];
  double MASK_error60[90];
  double MASK_error70[90];
  double MASK_error80[90];
  double MASK_error90[90];
  double Emax;
  int mode=90;

 //printf("mode=%d\n",mode);
 // for(int k=0;k<42;++k){
 //    printf("wv[%d]=%d\n",k,wv[k]);
 // }
 //getchar();

  TH1D* hist_MASK = new TH1D("hist_MASK"," MASK",90,0.,89.);
  
   TF1* func = new TF1("func",Fitf2,20,mode,4);
   func->SetParNames("par[0]","par[1]","par[2]","par[3]");

 //INPUT MASK file
  gStyle->SetOptFit(1111);
  TCanvas* canvas_MASK = new TCanvas("canvas_MASK","MASK",200,10,600,500);
  input_MASK=fopen("MASK.txt","r");
  //input_MASK=fopen("sim_mask.txt","r");
  MASK_peak=0;
  sum_MASK=0;sum_MASK_k=0;
  for(int k=0;k<90;++k){
    //fscanf(input_MASK,"%lf,%lf",&MASK[k],&MASK_error[k]);
    fscanf(input_MASK,"%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",&idmy,&MASK[k],&MASK_error[k],&MASK_error30[k],
	  &MASK_error40[k],&MASK_error50[k],&MASK_error60[k],&MASK_error70[k],&MASK_error80[k],&MASK_error90[k]);
    if(MASK[k]<0)MASK[k]=0;
    hist_MASK->SetBinContent(k,MASK[k]);
    hist_MASK->SetBinError(k,1);
    if(MASK_peak<MASK[k]){
      MASK_peak=MASK[k];
      MASK_max_k=k;
    }
 //   printf("MASK_error[%d]=%lf MASK_error90[%d]=%lf\n",k,MASK_error[k],k,MASK_error90[k]);
 //   if(k%10==0 && k>0)getchar();
  }
  printf("MASK_peak=%lf MASK_max_k=%d\n",MASK_peak,MASK_max_k);
  MASK_start=mode-1;
  MASK_stop=mode-1;
  for(int k=0;k<90;++k){
    if(MASK_peak/(int)5 < MASK[k]){
      if(MASK_start>k)MASK_start=k;
      MASK_stop=k;
      sum_MASK=sum_MASK+MASK[k];
      sum_MASK_k=sum_MASK_k+MASK[k]*k;
    }
  }
  MASK_mach=sum_MASK_k/(double)sum_MASK;
  if(MASK_mach-(int)MASK_mach>=0.5){
    MASK_mach=(int)MASK_mach+1;
  }else{
    MASK_mach=(int)MASK_mach;
  }
  MASK_quater[0]=(int)(MASK_mach-MASK_start);
  MASK_quater[1]=(int)(MASK_stop-MASK_mach);
  MASK_intg=0;
  for(int k=MASK_start;k<=MASK_stop;++k){
    MASK_intg=MASK_intg+MASK[k];
  }
  
  printf("sum_MASK=%lf sum_MASK_k=%lf MASK_mach=%lf\n",
      sum_MASK,sum_MASK_k,MASK_mach);
  //getchar();
  range_min=MASK_start;
  range_max=MASK_stop;

  func->SetParameters(MASK_peak,2.5,(double)range_min-10,(double)3); //CR-RCn
  func->SetRange(range_min,range_max);
  hist_MASK->Fit(func,"R");
  //hist_MASK->Fit(func,"RN");
  canvas_MASK->Update();
  func->GetParameters(par);
    ndf = func->GetNDF();
    chi = func->GetChisquare();
    printf("chi=%lf ndf=%d  --> %lf\n",chi,ndf,chi/(double)ndf);
  printf("par[1]=%lf par[3]=%lf par[0]=%lf par[2]=%lf\n",
      par[1],par[3],par[0],par[2]);
  printf("MASK_intg=%lf\n",MASK_intg);
  printf("MASK_mach=%lf MASK_quater[0]=%d MASK_quater[1]=%d\n",MASK_mach,MASK_quater[0],MASK_quater[1]);
  printf("MASK_start=%d MASK_stop=%d range=%d\n",MASK_start,MASK_stop,MASK_stop-MASK_start+1);
  //getchar();
  par_err[2]=func->GetParError(2);
  par_err[3]=func->GetParError(3);


  FitMaskResult[0]=(double)MASK_mach;
  FitMaskResult[1]=MASK_intg;
  FitMaskResult[2]=(double)MASK_quater[0];
  FitMaskResult[3]=(double)MASK_quater[1];
  FitMaskResult[4]=(double)MASK_start;
  FitMaskResult[5]=(double)MASK_stop;
  FitMaskResult[6]=(double)MASK_max_k;
  FitMaskResult[7]=par[1];
  FitMaskResult[8]=par[3];

  return 0;
}

double Fitf2(double *x,double *par){
  Double_t ans;
  double E;
  double xx;

  if(par[0]>200)return(0);

  if(x[0]>par[2]){
    xx=x[0]-par[2];
    if(sqrt(pow(par[1],2))>1E-5){
      E=par[0]*exp(par[3])/(double)pow(par[3],par[3]);
      ans=E*pow(xx/par[1],par[3])*exp(-xx/par[1]);
      //if(ans<20)ans=0;
    }else{
      ans=0;
    }
  }else{
    ans=0;
  }
  return(ans);
}


