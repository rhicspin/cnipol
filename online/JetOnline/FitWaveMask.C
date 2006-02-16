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


double FitMap(double *x,double *par);
double SumUpMASK(double *par,int *wave,double *sqrt_sum,double *residual,double *mask_tmp,
       int *mode,int *range_min,int *range_max,double *outMAP);
double MASK[900],bin[900];
double MASK_max,MASK_tmax;

int TAsym::readMap(WaveFormMap *wfm){
  FILE *infile;
  FILE *outfile;
  int i,k,j;
  int idmy;
  double ldmy;
  char iname[256],sdmy[256];
  double tmp;
  int ch;

  for(i=0;i<96;++i){
    wfm->MAP_max_Am[i]=0;
    wfm->MAP_max_Gd[i]=0;
    wfm->MAP_tmax_Am[i]=0;
    wfm->MAP_tmax_Gd[i]=0;
    wfm->ratio_Gd[i]=0;
    wfm->ratio_Am[i]=0;
    wfm->ratio_All[i]=0;
  }
/////////////////Am
  for(i=0;i<6;++i){
     for(k=0;k<256;++k)iname[k]=0;
     sprintf(iname,"/home/okadah/Waveform_test/Jet-offl_test/Waveform_check/MakeMask/result/Am/x0205.201_Si%d_all_mean.txt",i+1);
    infile=fopen(iname,"r");
    fscanf(infile,"%s,",&sdmy);
    //for(k=0;k<900;++k){
    for(k=0;k<1;++k){
      fscanf(infile,"%d,",&idmy);
      for(j=0;j<15;++j){
        ch=j+i*16;
        fscanf(infile,"%lf,",&ldmy);
        if(ldmy>=0)wfm->MAP_Am[ch][k]=ldmy;
        if(k<650 && wfm->MAP_max_Am[ch] < wfm->MAP_Am[ch][k]){
           wfm->MAP_max_Am[ch] = wfm->MAP_Am[ch][k];
           wfm->MAP_tmax_Am[ch] = k;
        }
      }
      ch=15+i*16;
      fscanf(infile,"%lf",&ldmy);
      if(ldmy>=0)wfm->MAP_Am[ch][k]=ldmy;
      if(k<650 && wfm->MAP_max_Am[ch] < wfm->MAP_Am[ch][k]){
          wfm->MAP_max_Am[ch] = wfm->MAP_Am[ch][k];
          wfm->MAP_tmax_Am[ch] = k;
      }
    }
  fclose(infile);
  }
////////////////Gd
  for(i=0;i<6;++i){
     for(k=0;k<256;++k)iname[k]=0;
     sprintf(iname,"/home/okadah/Waveform_test/Jet-offl_test/Waveform_check/MakeMask/result/Gd/slice_out_Si%d_all_mean.txt",i+1);
    infile=fopen(iname,"r");
    fscanf(infile,"%s,",&sdmy);
    for(k=0;k<900;++k){
      fscanf(infile,"%d,",&idmy);
      for(j=0;j<15;++j){
        ch=j+i*16;
        fscanf(infile,"%lf,",&ldmy);
        if(ldmy>=0)wfm->MAP_Gd[ch][k]=ldmy;
        if(k<650 && wfm->MAP_max_Gd[ch] < wfm->MAP_Gd[ch][k]){
           wfm->MAP_max_Gd[ch] = wfm->MAP_Gd[ch][k];
           wfm->MAP_tmax_Gd[ch] = k;
        }
      }
      ch=15+i*16;
      fscanf(infile,"%lf",&ldmy);
      if(ldmy>=0)wfm->MAP_Gd[ch][k]=ldmy;
      if(k<650 && wfm->MAP_max_Gd[ch] < wfm->MAP_Gd[ch][k]){
          wfm->MAP_max_Gd[ch] = wfm->MAP_Gd[ch][k];
          wfm->MAP_tmax_Gd[ch] = k;
      }
    }
  fclose(infile);
  }
//ratio///////////////////////////////
  sprintf(iname,"/home/okadah/Waveform_test/Jet-offl_test/Evt_setect/slope_mod.txt");
  infile=fopen(iname,"r");
  fscanf(infile,"%s,",&sdmy);
  for(k=0;k<96;++k){
     fscanf(infile,"%d,%lf",&idmy,&wfm->ratio_All[k]);
  }
  wfm->seppen=30;
  fclose(infile);

/*
int ratio_start;
int ratio_stop;
outfile=fopen("slope.txt","w");
  for(k=0;k<96;++k){
    ratio_start=(int)(wfm->MAP_tmax_Gd[k]-100);
    ratio_stop=(int)(wfm->MAP_tmax_Gd[k]+200);
    tmp=0;
    for(i=ratio_start;i<=ratio_stop;++i){
     tmp=tmp+wfm->MAP_Gd[k][i]*0.1;
    }
    tmp=tmp/(double)4;
    wfm->ratio_Gd[k]=tmp/(double)(wfm->MAP_max_Gd[k]);

    ratio_start=(int)(wfm->MAP_tmax_Gd[k]-100);
    ratio_stop=(int)(wfm->MAP_tmax_Gd[k]+200);
    tmp=0;
    for(i=ratio_start;i<=ratio_stop;++i){
     tmp=tmp+wfm->MAP_Am[k][i]*0.1;
    }
    tmp=tmp/(double)4;
    wfm->ratio_Am[k]=tmp/(double)(wfm->MAP_max_Am[k]);
    fprintf(outfile,"%d,%lf,%lf\n",k,wfm->ratio_Gd[k],wfm->ratio_Am[k]);
//    printf(" ch=%d ratio=%lf %lf\n",k,wfm->ratio_Gd[k],wfm->ratio_Am[k]);
//    if(k%16==1 && k>1)getchar();
  }
  fclose(outfile);
*/

  return(0);
}



int TAsym::FitWaveMask900(WaveFormMap *wfm,int wv[],double FitResult[],int geo_chan,int mode){
        char iname[256],num[10];
        int k;
        double param[2];
        double residual[90],mask_tmp[90],tmp_error[90];
        double sqrt_sum,intg_fit;
        double FixMASK[90],Fixbin[90];
        double wave_max;
        int wave_tmax;
        int range_min,range_max;
        double outMAP[4];//peak,tdc,tmax,intg
        double chi,cndf;
        int ndf;
        double new_p1;

        wave_max=0;
        for(k=0;k<90;++k){
          hist_event->SetBinContent(k,wv[k]);
          if(wave_max<wv[k]){
               wave_max=wv[k];
               wave_tmax=k;
          }
          residual[k]=0;
          mask_tmp[k]=0;
          tmp_error[k]=0;
        } 
        for(k=0;k<900;++k){

           if(wave_max<100){
             MASK[k]=(wfm->MAP_Gd[geo_chan-1][k]);
           }else{
             MASK[k]=(wfm->MAP_Am[geo_chan-1][k]);
           }


//           if(geo_chan==84){
//             printf("MASK[%d]=%lf\n",k,MASK[k]);
//             if(k%50==0 && k>0)getchar();
//           }

        }
        if(wave_max<100){
          MASK_max=wfm->MAP_max_Gd[geo_chan-1];
          MASK_tmax=wfm->MAP_tmax_Gd[geo_chan-1];
        }else{
          MASK_max=wfm->MAP_max_Am[geo_chan-1];
          MASK_tmax=wfm->MAP_tmax_Am[geo_chan-1];
        }

//        if(geo_chan==84){
//         printf("MASK_max=%lf wave_max=%d\n",MASK_max,wave_max);
//         getchar();
//        } 

  
    range_min=wave_tmax-10;
    range_max=wave_tmax+20;
    if(range_min<0)range_min=0;
    if(range_max>90)range_min=89;
    if(wave_max>0){
 
  	TF1* func = new TF1("func",FitMap,0,90,2);
        func->SetParNames("param[0]","param[1]");

 
        gStyle->SetOptFit(1111);

        func->SetRange(range_min,range_max);
        func->SetParameters(wave_max/(double)MASK_max,wave_tmax*10-5-MASK_tmax);
        for(k=0;k<90;++k){
          hist_event->SetBinError(k,2);
          //if(k>50 && k<57)hist->SetBinError(k,0.5);
        }
        //printf("iname=%s\n",iname);
        hist_event->Fit(func,"RNQE");
        //hist_event->Fit(func,"RNE");


//FitFunc CHECK
        func->GetParameters(param);
        ndf = func->GetNDF();
        chi = func->GetChisquare();
        cndf=chi/(double)ndf;
        if(cndf>10){
          new_p1=param[1]-(wave_tmax*10-5-MASK_tmax-param[1]);
          func->SetParameters(wave_max/(double)MASK_max,new_p1);
          hist_event->Fit(func,"RNEQ");
          func->GetParameters(param);
          ndf = func->GetNDF();
          chi = func->GetChisquare();
          cndf=chi/(double)ndf;
          if(cndf>10){
             printf("MASK_max=%lf MASK_tmax=%lf wave_max=%lf wave_tmax=%d\n",
             MASK_max,MASK_tmax,wave_max,wave_tmax);
             printf("p0=%lf p1=%lf\n",wave_max/(double)MASK_max,wave_tmax*10-MASK_tmax);
             printf("p0=%lf new p1=%lf\n",wave_max/(double)MASK_max,new_p1);
             printf("---------------------------\n");
          }
        }

        intg_fit=SumUpMASK(param,wv,&sqrt_sum,residual,mask_tmp,&mode,&range_min,&range_max,outMAP);


        double ratio[90],bin[90];
//        FILE *out_residual;
//        out_residual=fopen("residual_out.txt","w");
        for(k=0;k<mode;++k){
//          fprintf(out_residual,"%d,%d,%lf,%lf,%lf\n",k,wave[k],residual[k],mask_tmp[k],tmp_error[k]);
          if(mask_tmp[k]!=0){
            ratio[k]=wv[k]/(double)mask_tmp[k];
          }else{
            ratio[k]=0;
          }
//          printf("%d,%d,%lf,%lf,%lf\n",k,wave[k],residual[k],mask_tmp[k],ratio[k]);
//          if(k>0 && k%10==0)getchar();
        }
//        fclose(out_residual);
     FitResult[0]=outMAP[0];
     FitResult[1]=outMAP[2];
     FitResult[2]=outMAP[1];
     FitResult[3]=0;
     FitResult[4]=intg_fit;//or outMAP[3]
     //FitResult[4]=outMAP[3]/(double)10;
     FitResult[5]=ndf;
     FitResult[6]=chi;
     FitResult[7]=cndf;
     FitResult[8]=range_min;
     FitResult[9]=range_max;
     FitResult[10]=-1;//tau
     FitResult[11]=-1;// n

   }else{
     FitResult[0]=0;
     FitResult[1]=0;
     FitResult[2]=0;
     FitResult[3]=0;
     FitResult[4]=0;
     FitResult[5]=-1;
     FitResult[6]=-1;
     FitResult[7]=-1;
     FitResult[8]=range_min;
     FitResult[9]=range_max;
     FitResult[10]=-1;//tau
     FitResult[11]=-1;// n

   }

	return(0);
}

double  FitMap(double *x,double *par){
  Double_t ans;
  double E;
  double Epeak;
  int xx;
  double d_xx;

  if(x[0]*10>par[1]){
        //d_xx=(x[0]-par[1])*10;
        d_xx=x[0]*10-par[1];
        xx=(int)d_xx;
        if(d_xx-xx>=0.5)xx=xx+1;
	if(xx>-1 && xx<900){
            ans=par[0]*MASK[xx];
  //        printf("x[0]=%lf MASK[%d]=%lf\n",x[0],xx,MASK[xx]);
  //        getchar();
	}else{
   	    ans=0;
	}
  }else{
    ans=0;
  }
  return(ans);
}


double SumUpMASK(double *par,int *wave,double *sqrt_sum,double *residual,double *mask_tmp,
              int *mode,int *range_min,int *range_max,double *outMAP){
  int k;
  double intg_fit=0;
  double fit_value;
  int k_int;
  double E;
  double check_peaktime,check_peakpulse;
  double d_min_range,d_max_range;
  int i_min_range,i_max_range;

 d_min_range=*range_min*10-par[1];
 i_min_range=(int)d_min_range;
 if((d_min_range-i_min_range)>=0.5)i_min_range=i_min_range+1;
 d_max_range=*range_max*10-par[1];
 i_max_range=(int)d_max_range;
 if((d_max_range-i_max_range)>=0.5)i_max_range=i_max_range+1;


  *sqrt_sum=0;
  check_peakpulse=0;

  for(k=0;k<(*mode);++k){
    k_int=(int)(k*10-par[1]);
    if(k*10>par[1]){
      fit_value=par[0]*MASK[k_int];
      if(fit_value<0)fit_value=0;
      if(check_peakpulse<fit_value){
        check_peakpulse=fit_value;
        check_peaktime=k_int;
      }
    //    printf("check_peakpulse=%lf fit_value=%lf  wv[%d]=%d\n",check_peakpulse,fit_value,k,wave[k]);
    }else{
      fit_value=0;
    }
    if(k>=*range_min && k<=*range_max){
       intg_fit=intg_fit+fit_value;
       *sqrt_sum=(*sqrt_sum)+(pow(fit_value-wave[k],2));
    }
    residual[k]=fit_value-wave[k];
    mask_tmp[k]=fit_value;
  }
//outMAP[4]
  for(k=0;k<4;++k)outMAP[k]=0;
  for(k=0;k<650;++k){
      if(outMAP[0]< MASK[k] ){
        outMAP[0]=MASK[k];
        outMAP[2]=(int)k;
      //  printf("k=%d outMAP[0]=%lf\n",k,MASK[k]);
      }
      if(i_min_range<=k && i_max_range>=k){
        outMAP[3]=outMAP[3]+par[0]*MASK[k];
      }
  }
  for(k=(int)outMAP[2];k>=0;--k){
    if(MASK[k] < outMAP[0]/(double)4){
       outMAP[1]=k;
       break;
    }
  }  
  //printf("par[0]=%lf\n",par[0]);
  outMAP[0]=outMAP[0]*par[0];
  double tmp1,tmp2;
  tmp1=(outMAP[1]+par[1])/(double)10;
  outMAP[1]=tmp1;
  //outMAP[1]=(int)tmp1;
  //if((tmp1-outMAP[1])>=0.5)outMAP[1]=outMAP[1];
  tmp2=(outMAP[2]+par[1])/(double)10;
  outMAP[2]=(int)tmp2;
  if((tmp2-outMAP[2])>=0.5)outMAP[2]=outMAP[2];
    
  return intg_fit;
}


