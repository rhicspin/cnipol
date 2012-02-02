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


double Fitf2Fix(double *x,double *par_fix); 
double Fitf2(double *x,double *par); 
double SumUpFunc2(double *par,double *WAVE_mach,int *MASK_quater,int *wave,double *sqrt_sum,
            double *residual,double *mask_tmp,int *mode,int *range_min,int *range_max);
int get_timing(double *par, double *fbin,double *value,int *mode);

double FIX[2];

int TAsym::FitWaveform(int wv[],double FitResult[],double FitMaskResult[],int geo_ch,int mode,WaveForm2Par *wf2par) 
{
  FILE *input_MASK;
  int k;
  int t1,t2,t3,t4,hist_content,hit,ph_max,k_max;
  int range_min,range_max;
  int hist_range,max,min,bin;
  int hist_content_old;
  double par[4],par_fix[2];
  double par_err[4];
  double chi,chi_err;
  double f_n,f_tau;
  double cndf;
  int ndf;
  int intg1_cont,intg2_cont,intg3_cont;
  double sqrt_sum;
  int limit1;//p1_max
  int limit2;//p2_max
  int wave[90];
  double residual[90],mask_tmp[90],tmp_error[90];

  int p1_max,p2_max;
  int RGB_timing_check=0;//count max timimg==1/4timing
  //int g_ats[7]; //max_ph,  1/4timing,  Integ,  max-timing ,10%-timing ,90%-timing,50%-timing
  double wf_p;//max_ph from Waveform itself
  double wf_t;//1/4max from Waveform itself
  double wf_tmax;//max timing from Waveform itself
  double wf_t10;//10%max from Waveform itself
  double wf_t90;//90%max from Waveform itself
  double wf_t50;//90%max from Waveform itself
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
  int MASK_max_k;
  int intg_middle=0;
  double ratio;
  int idmy;
  int MASK_start,MASK_stop;
  double MASK_error[90],MASK[90];
  double Emax;

  int FITMODE=1; //0:not fix  1;FIX


  for(k=0;k<90;++k){
    wave[k]=0;
    residual[k]=0;
    mask_tmp[k]=0;
    tmp_error[k]=0;
  }

  


  intg1_cont=0;
  intg2_cont=0;
  intg3_cont=0;

  TF1* func = new TF1("func",Fitf2,0,mode,4);
  func->SetParNames("par[0]","par[1]","par[2]","par[3]");
  TF1* func_fix = new TF1("func_fix",Fitf2Fix,0,mode,2);
  func_fix->SetParNames("par_fix[0]","par_fix[1]");
 
  MASK_mach=FitMaskResult[0];
  MASK_intg=FitMaskResult[1];
  MASK_quater[0]=(int)FitMaskResult[2];
  MASK_quater[1]=(int)FitMaskResult[3];
  MASK_start=(int)FitMaskResult[4];
  MASK_stop=(int)FitMaskResult[5];
  MASK_max_k=(int)FitMaskResult[6];
//  FIX[0]=FitMaskResult[7];
//  FIX[1]=FitMaskResult[8];

//Fixed parameters tau&n
  FIX[0]=wf2par->wf_tau[geo_ch];//tau
  FIX[1]=wf2par->wf_n[geo_ch];//n

//  FILE *out_residual;
//  out_residual=fopen("residual_out.txt","w");// OPEN sitara kanarazu CLOSE siro!!

  int residual_sample=0;

  hit=0;
///////////wave[k]=hist 
    ph_max=-1;
    int check=0;
    for(k=0;k<mode;++k){
      hist_content = wv[k]; 
      hist_event->SetBinContent(k,wv[k]);
      if(hist_content>1)check=1;
      if(ph_max<=hist_content) {
	ph_max = hist_content;
	k_max = k;
      }
      wave[k]=hist_content;
    } 
    limit1=ph_max/(int)4;
    if(mode==90)limit2=ph_max/(int)4;//ph_max/(double)10;
    if(mode==42)limit2=ph_max/(int)2;
 // Get Fitting Area(range_min,range_max) & Get ph_max
//////Select for Fit range to get single event--- Sandro's suggest
    p1_max=0;p2_max=0;
    for(k=k_max;k>0;--k){
      hist_content=wave[k];
      if(k==k_max)hist_content_old=hist_content+1;
      if((k_max-k)>3 && (hist_content<=limit1 || (hist_content-hist_content_old)>0) ){
	if((hist_content-hist_content_old)>0){
	  p1_max=hist_content_old;// sorikaeri wo husegu
	}else{
	  p1_max=hist_content;
	}
        if(hist_content==0)p1_max=0;
	break;
      }
      hist_content_old=hist_content;
    }
    for(k=k_max;k<mode;++k){
      hist_content=wave[k];
      if(k==k_max)hist_content_old=hist_content+1;
      if((k-k_max)>3 && (hist_content<=limit2 || (hist_content-hist_content_old)>0) ){
	if((hist_content-hist_content_old)>0){
	  p2_max=hist_content_old;
	}else{
	  p2_max=hist_content;
	}
	break;
      }
      hist_content_old=hist_content;
    }
////////////////////////////////////////
//Fitting area  bin No de FIX
    range_min=k_max-10;
    range_max=k_max+20;
 //   printf("---Fitting-------\n");
 //   printf("k_max=%d range_min=%d range_max=%d\n",k_max,range_min,range_max);
    if(range_min<0)range_min=0;
    if(range_max>mode)range_max=mode-1;
    //printf("MASK_start=%d MASK_max_k=%d MASK_stop=%d\n",MASK_start,MASK_max_k,MASK_stop);
    //printf("range_min=%d range_max=%d k_max=%d\n",range_min,range_max,k_max);
    //getchar(); 
    if((range_max-range_min)!=30){
         printf("k_max=%d range_min=%d range_max=%d\n",k_max,range_min,range_max);
         getchar();
        for(k=0;k<9;++k){
         printf("%d %d %d %d %d %d %d %d %d %d\n",wave[k*10],wave[k*10+1],wave[k*10+2],wave[k*10+3],
           wave[k*10+4],wave[k*10+5],wave[k*10+6],wave[k*10+7],wave[k*10+8],wave[k*10+9]);
        }
        getchar();
    }
/////////////////////////////////////
 if(p1_max<250 && p2_max<250 ){
   int WAVE_sum=0;
   int WAVE_sum_k=0;
   double WAVE_mach;
   for(k=range_min;k<=range_max;++k){
     WAVE_sum=WAVE_sum+wave[k];
     WAVE_sum_k=WAVE_sum_k+wave[k]*k;
   }
   intg_middle=WAVE_sum;
   WAVE_mach=WAVE_sum_k/(double)WAVE_sum;
   if(WAVE_mach-(int)WAVE_mach>=0.5){
     WAVE_mach=(int)WAVE_mach+1;
   }else{
     WAVE_mach=(int)WAVE_mach;
   }
 //MASK kara motometa maximum ph
   int tmp_i=(int)(WAVE_mach+(MASK_max_k-MASK_mach)+1);
   MASK_WAVE_max=wave[tmp_i];
   MASK_WAVE_quater=WAVE_mach-MASK_quater[0]+1;//MASK kara motometa quater;
   MASK_WAVE_tmax=tmp_i;//MASK kara motometa tmax;
   MASK_WAVE_intg=0;
   for(k=(int)WAVE_mach-MASK_quater[0];k<=(int)WAVE_mach+MASK_quater[1];++k){
     MASK_WAVE_intg=MASK_WAVE_intg+wave[k];
   }
   ratio=MASK_WAVE_intg*MASK_peak/(double)(MASK_intg*ph_max);

   int move_k=(int)(MASK_mach-WAVE_mach);
   /*
   printf("WAVE_mach=%d\n",WAVE_mach);
   printf("wave[k_max=%d]=%d wave[%d]=%d \n",k_max,wave[k_max],tmp_i,wave[tmp_i]);
   printf("move_k=%d\n",move_k);
   getchar();
   */
   for(k=0;k<mode;++k){
     int kk=k+move_k-1;
     if(kk<0)kk=0;
     if(kk>=mode)kk=(mode-1);
     //printf("wave[%d]=%d MASK[%d]=%lf\n",k,wave[k],kk,MASK[kk]);
     //if(k>0 && k%10==0)getchar();
       double error;
       if(wave[k]!=0){
          //error=1/(double)(sqrt(wave[k])*5);
          //error=1/(double)(sqrt(wave[k]));
          //error=(double)(sqrt(wave[k]));
          error=2.0;//typ2
          //error=sqrt(pow(1.7,2)+pow(0.05*sqrt(wave[k]),2));
          //error=1.0+0.02*wave[k];//typ1
       }else{
          error=1.3;
       }
       hist_event->SetBinError(k,error);
       //!tmp_error[k]=2;
       //hist_event->SetBinError(k,MASK_error[kk]*ph_max/(double)MASK_peak);
       //tmp_error[k]=MASK_error[kk]*ph_max/(double)MASK_peak;
   }
  // Fitting 
/////////////////////
    double param[4];
    double initial_t0=(double)range_min-10;
    if(initial_t0<0)initial_t0=0;
    //printf("range_min=%d range_max=%d ph_max=%d\n",range_min,range_max,ph_max);    
    if(FITMODE==0){ 
         func->SetParameters((double)ph_max,4.0,initial_t0,(double)2.8); //CR-RCn
         func->SetRange(range_min,range_max);
         hist_event->Fit(func,"RNQ");
         func->GetParameters(par);
         par_err[0]=func->GetParError(0);
         param[0]=par[0];
         param[1]=par[1];
         param[2]=par[2];
         param[3]=par[3];
         ndf = func->GetNDF();
         chi = func->GetChisquare();
         f_tau=par[1];
         f_n=par[3];
    }else{ 
         func_fix->SetParameters((double)ph_max,initial_t0); //CR-RCn
         func_fix->SetRange(range_min,range_max);
         hist_event->Fit(func_fix,"RNQ");
         func_fix->GetParameters(par_fix);
         par_err[0]=func_fix->GetParError(0);
         par_err[1]=func_fix->GetParError(1);
         param[0]=par_fix[0];
         param[1]=FIX[0];
         param[2]=par_fix[1];
         param[3]=FIX[1];
         ndf = func_fix->GetNDF();
         chi = func_fix->GetChisquare();
         f_tau=FIX[0];
         f_n=FIX[1];
    }
    Emax=param[0]*exp(param[3])/(double)pow(param[3],param[3]);
    t0=param[2];
    fbinmax=t0+param[1]*param[3];
    if((fbinmax-(int)fbinmax)>=0.5){
       fbinmax=(int)(fbinmax)+1;
    }else{
       fbinmax=(int)(fbinmax);
    } 
    Pmax=param[0];
    value=param[0]/(double)4;
    get_timing(param, &fbin, &value,&mode);
    Pmax_error=par_err[0];
    cndf=chi/(double)ndf;

///GET FITTING INTEGRAL!!/////////////
//normal 90bin
    intg_fit=0;
    intg_fit=SumUpFunc2(param,&WAVE_mach,MASK_quater,wave,&sqrt_sum,residual,mask_tmp,&mode,&range_min,&range_max); 
/*
    if(chi<2 && Pmax>100){
       int ggg;
       printf("ggg=1  or 0\n");
	scanf("%d",&ggg);
	if(ggg==1){
	  //FILE *out_residual;
	  //out_residual=fopen("residual_out.txt","w");
          for(k=0;k<mode;++k){
	    //fprintf(out_residual,"%d,%d,%lf,%lf,%lf\n",k,wave[k],residual[k],mask_tmp[k],tmp_error[k]);
	    printf("%d,%d,%lf,%lf\n",k,wave[k],residual[k],mask_tmp[k]);
            if(k>0 && k%10==0)getchar();
	  }
	  //fclose(out_residual);
	}
    }
   if(chi<2 &&   range_max<89){
     double num_k[90];
     double resi[90];
    // printf("chi=%lf ndf=%d\n",chi,ndf);
    // getchar();
     for(k=0;k<90;++k){
       num_k[k]=k;
       resi[k]=0;
     }
   }
*/
  ////////////////////////////////////////////////////////////
 }else{//if(p1_max< && p2_max<)END  
   MASK_WAVE_intg=0;
   MASK_WAVE_max=0;
   MASK_WAVE_quater=0;
   MASK_WAVE_tmax=0;
   Pmax=0;
   Pmax_error=0;
   chi=-1;
   fbinmax=0;
   fbin=0;
   fbin10=0;
   fbin50=0;
   fbin90=0;
   intg_fit=0;
   intg_middle=1;
   ratio=0;
   ndf=-1;
   cndf=-1;
 }
 FitResult[0]=Pmax;
 FitResult[1]=fbinmax;
 FitResult[2]=fbin;
 FitResult[3]=intg_middle;
 FitResult[4]=intg_fit;
 FitResult[5]=ndf;
 FitResult[6]=chi;
 FitResult[7]=cndf;
 FitResult[8]=range_min;
 FitResult[9]=range_max;
 FitResult[10]=f_tau;// tau
 FitResult[11]=f_n;// n
// printf("FitResult[5]=%lf FitResult[6]=%lf FitResult[7]=%lf\n",
//    FitResult[5],FitResult[6],FitResult[7]);
// getchar();

   
return(0);
}


double Fitf2Fix(double *x,double *par_fix){
  Double_t ans;
  double E;
  double xx;

  if(par_fix[0]>200)return(0);

  if(x[0]>par_fix[1]){
    xx=x[0]-par_fix[1];
    if(sqrt(pow(FIX[0],2))>1E-5){
      E=par_fix[0]*exp(FIX[1])/(double)pow(FIX[1],FIX[1]);
      ans=E*pow(xx/FIX[0],FIX[1])*exp(-xx/FIX[0]);
      //if(ans<20)ans=0;
    }else{
      ans=0;
    }
  }else{
    ans=0;
  }
  return(ans);
}

double SumUpFunc2(double *par,double *WAVE_mach,int *MASK_quater,int *wave,double *sqrt_sum,double *residual,double *mask_tmp,
              int *mode,int *range_min,int *range_max){ 
  int k;
  double intg_fit=0;
  double fit_value;
  double k_double;
  double E;
  double check_peaktime,check_peakpulse;

  *sqrt_sum=0;
  check_peakpulse=0;
  E=par[0]*exp(par[3])/(double)pow(par[3],par[3]);

  for(k=0;k<(*mode);++k){
    k_double=k-par[2]-0.5;
    if((k-0.5)>par[2]){
      fit_value=E*pow(k_double/par[1],par[3])*exp(-k_double/par[1]);
      if(fit_value<0)fit_value=0;
      if(check_peakpulse<fit_value){
	check_peakpulse=fit_value;
	check_peaktime=k_double;
      }
    //    printf("check_peakpulse=%lf fit_value=%lf  wv[%d]=%d\n",check_peakpulse,fit_value,k,wave[k]);
    }else{
      fit_value=0;
    }
    if((k-1)>=*range_min && (k-1)<=*range_max){ 
       intg_fit=intg_fit+fit_value;
       *sqrt_sum=(*sqrt_sum)+(pow(fit_value-wave[k],2));
    }
    residual[k]=fit_value-wave[k];
    mask_tmp[k]=fit_value;
  }
  return intg_fit;
}


int get_timing(double *par, double *fbin, double *value,int *mode){
  int k,get_k;

  double E=par[0]*exp(par[3])/(double)pow(par[3],par[3]);
  double fit_value=0;
  double k_double;
  k=0;
  while(fit_value<*value){
    k_double=k-par[2]-0.5;
    if((k-0.5)>par[2]){
      fit_value=E*pow(k_double/par[1],par[3])*exp(-k_double/par[1]);
      if(fit_value<0)fit_value=0;
    }else{
      fit_value=0;
    }
    k++;
    if(k>(*mode-1))break;
  }
  if(k==(*mode)){
    *fbin=100;
  }else{
    get_k=k-1;
    double k_double1=k_double;
    double k_double0=get_k-1-par[2]-0.5;
    double e0,e1;

    e0=E*pow(k_double0/par[1],par[3])*exp(-k_double0/par[1]);
    e1=E*pow(k_double1/par[1],par[3])*exp(-k_double1/par[1]);
    double a,b;
    a=(e1-e0)/(double)(k_double1-k_double0);
    b=(k_double1*e0-k_double0*e1)/(double)(k_double1-k_double0);
    *fbin=(*value-b)/(double)a;
    *fbin=*fbin+par[2]+0.5;
  //printf("a=%f b=%f\n",a,b);
  //printf("k_double1=%f k_double0=%f\n",k_double1,k_double0);
  //printf("e0=%f value=%f e1=%f fbin=%f\n",e0,*value,e1,*fbin);
  //getchar();
  }

  return(0);


}
