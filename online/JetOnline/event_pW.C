//////////////////////////
// ==================== 
// Processing one event WaveForm  okada 
// ==================== 
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

void itoa(int val,char *s);
char *strrev(char *s);

int TAsym::EventProcessWaveform(processWEvent *wevent, recordConfigRhicStruct *cfginfo,int  wv[],int hist_num,
         int mode,int all_event) {
   // printf("EventProcessWaveform %d  amp=%d\n",all_event,wevent->amp); 
    int i,st;     // st : c like 0-23
    int bu,rev,event_num,delim;
    int si,amph,timedc,inte;
    int bunch_typ=120;
    char iname[256],num[256],comment[256],comment1[256],comment2[256],comment3[256];
    //----------------------------------------------------  dproc.CMODE ninsiki dekiru!
//  printf("event_pW dproc.CMODE=%d\n",dproc.CMODE);
//  getchar();


   for(int k=0;k<256;++k)iname[k]=0;
   for(int k=0;k<256;++k)num[k]=0;
   for(int k=0;k<256;++k){
          comment[k]=0;
          comment1[k]=0;
          comment2[k]=0;
          comment3[k]=0;
   }
   if(dproc.CMODE!=1){
   	comment1[0]='r';
   	comment1[1]='e';
   	comment1[2]='v';
   	comment1[3]='-';
   	comment2[0]=';';
   	comment2[1]='b';
   	comment2[2]='u';
   	comment2[3]='n';
   	comment2[4]='c';
   	comment2[5]='h';
   	comment2[6]='-';
   	comment3[0]=';';
   	comment3[1]='c';
   	comment3[2]='h';
   	comment3[3]='a';
   	comment3[4]='n';
   	comment3[5]='-';
   }
   if(dproc.CMODE==1){
   	comment1[0]='C';
   	comment1[1]='a';
   	comment1[2]='l';
   	comment1[3]='i';
   	comment1[4]='b';
   	comment2[0]=';';
   	comment2[1]='c';
   	comment2[2]='h';
   	comment2[3]='a';
   	comment2[4]='n';
   	comment2[5]='-';
   	comment3[0]=';';
   	comment3[1]='e';
   	comment3[2]='v';
   	comment3[3]='e';
   	comment3[4]='n';
   	comment3[5]='t';
   }
  
    st = wevent->strip_no;
    si = (int)(st/12)+1;   // si=1...6
    amph=wevent->amp;
    timedc=wevent->t_time;
    inte=wevent->square;

   if(dproc.CMODE!=1){ 
    	iname[0]='w';
    //	delim=wevent->delim;
   // 	itoa(delim,num);
   // 	strcat(iname,num); 
    	for(int k=0;k<256;++k)num[k]=0;
   // 	strcat(iname,"_"); 
    	//event_num=wevent->revolution*bunch_typ+wevent->bunch;
    	event_num=all_event;
    	itoa(event_num,num);
    	strcat(iname,num); 
    	for(int k=0;k<256;++k)num[k]=0;
    	strcat(iname,"_"); 
    	//st = wevent->strip_no;
    	st = wevent->geo;
    	itoa(st,num);
    	strcat(iname,num);
    	for(int k=0;k<256;++k)num[k]=0;

    	rev=wevent->revolution;
    	itoa(rev,num);
    	strcat(comment1,num); 
    	for(int k=0;k<256;++k)num[k]=0;
    	bu=wevent->bunch;
    	itoa(bu,num);
    	strcat(comment2,num); 
    	for(int k=0;k<256;++k)num[k]=0;
    	st = wevent->strip_no;
    	itoa(st,num);
    	strcat(comment3,num);
    	for(int k=0;k<256;++k)num[k]=0;
    	strcat(comment,comment1);
    	strcat(comment,comment2);
    	strcat(comment,comment3);
    }else{
        iname[0]='w';
        //st = wevent->strip_no;
        st = wevent->geo;//Geo.ch
    	itoa(st,num);
    	strcat(iname,num);
    	for(int k=0;k<256;++k)num[k]=0;
    	strcat(iname,"_"); 
    	itoa(all_event,num);
    	strcat(iname,num); 
    	for(int k=0;k<256;++k)num[k]=0;


    	itoa(st,num);
    	strcat(comment2,num); 
    	for(int k=0;k<256;++k)num[k]=0;
    	itoa(all_event,num);
    	strcat(comment3,num);
    	for(int k=0;k<256;++k)num[k]=0;
    	strcat(comment,comment1);
    	strcat(comment,comment2);
    	strcat(comment,comment3);
    } 

    //printf("check-->%s %s\n",comment,iname);
    //getchar();

   /* 
    if(all_event==82 || all_event==72 || all_event==100 || all_event==105 || all_event==128 || all_event== 135
        || all_event==581 || all_event==595 || all_event==607 || all_event==661 || all_event==663){
      printf("EventProcessWaveform---st=%d si=%d amph=%d timedc=%d inte-%d\n",st,si,amph,timedc,inte);    
      getchar();
    }
*/
    hist_waveform = new TH1D(iname,comment,mode,0,mode);
    for(int k=0;k<mode;++k)hist_waveform->SetBinContent(k,wv[k]);

    hist_waveform->Write();
    //for(int k=0;k<mode;++k){
    //  printf("hist->%lf\n",hist_waveform->GetBinContent(k));
   // }
   //getchar();


    return(0);
}
//////////////////////////

void itoa(int val,char *s){
  char *t;
  int mod;

  if(val < 0){
  	*s++ = '-';
	val=-val;
  }
  t=s;

  while(val){
    	mod=val%10;
	*t++ = (char)mod + '0';
	val /=10;
  }
  if(s ==t) *t++ = '0';
  *t = '\0';
  strrev(s);
}

char *strrev(char *s){
  char *ret =s;
  char *t =s;
  char c;

  while( *t != '\0') t++;
  t--;

  while(t > s){
    c =*s;
    *s = *t;
    *t = c;
    s++;
    t--;
  }

  return ret;
}




