// ================
// get_ats  Okada
// ================
// g_ats[0]=amp
// g_ats[1]=t_quater
// g_ats[2]=square
// g_ats[3]=tmax
// g_ats[4]=limited are intg
// g_ats[5]=limited area count
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

int TAsym::GetAts(int *wv,double *g_ats,int mode){
	int i,k,l;
	float max=-1000;
	int get_i;
	float float_i,float_s,float_s2;

	get_i=-1;

	g_ats[0]=0;g_ats[1]=0;g_ats[2]=0;g_ats[3]=0;g_ats[4]=0;g_ats[5]=0;
	for(i=0;i<mode;++i){
		if(max<=wv[i]){
			max=(float)wv[i];
			get_i=i;
		}
		g_ats[2]=g_ats[2]+wv[i];
	}
        int imin,imax;
        imin=get_i-10;
        imax=get_i+20;
//        printf("---------------Get_Ats--------------\n");
//        printf("get_i=%d imin=%d imax=%d\n",get_i,imin,imax);
        if(imin<0)imin=0;
        if(imax>mode)imax=mode-1;
        for(i=imin;i<=imax;++i){
          g_ats[4]=g_ats[4]+wv[i];
          g_ats[5]++;
        }
	float_s=g_ats[2]/(float)4;//??
	g_ats[2]=(int)float_s;//kirisute
	float_s2=g_ats[4]/(float)4;//??
	g_ats[4]=float_s2;
	//g_ats[4]=(int)float_s2;//kirisute

	g_ats[0]=wv[get_i];
	g_ats[3]=get_i*2;
	for(i=get_i;i>=0;--i){
		if(max/4 > wv[i]){
			float_i=((max/(int)4-(float)wv[i])/(float)(wv[i+1]-wv[i]));
			float_i=(float_i+i)*2;
			if( (float_i-(int)float_i)>0.0 ){ //kiriage?
				g_ats[1]=(int)(float_i)+1;
			}else{
				g_ats[1]=(int)(float_i);
			}
//			printf("find!! max/4=%f wv[%d]=%d wv[%d]=%d\n",max/4,i,wv[i],i+1,wv[i+1]);
		//	printf("float_i=%f g_ats[1]=%d\n",float_i,g_ats[1]);
			break;
		}
	}
//        printf("g_ats[2]=%d g_ats[4]=%d float_s=%f float_s2=%f\n",g_ats[2],g_ats[4],float_s,float_s2);
//        printf("imin=%d imax=%d g_ats[5]=%d\n",imin,imax,g_ats[5]);
//        getchar();
//	printf("float_s=%f\n",float_s);
//	printf("*get_a=%d get_t=%d get_s=%d get_tmax=%d\n",g_ats[0],g_ats[1],g_ats[2],g_ats[3]);
//	printf("float_i=%f\n",float_i);

	if(get_i<0 || max<150){
		return(-1);
	}else{
		return(0);
	}

}


