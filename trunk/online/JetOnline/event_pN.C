//////////////////////////
// ==================== 
// Processing one event Ntuple,Scatter_plot  okada 
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

extern void HHF1(int, float ,float);
extern void HHFN(int, float event_ntuple[]);

void itoa(int val,char *s);
char *strrev(char *s);

int TAsym::EventProcessPawNtuple(processWEvent *wevent, recordConfigRhicStruct *cfginfo,int hist_num,
         int mode,int all_count_AT){   
    int i,st;     // st : c like 0-23
    int si,amph,timedc,inte;
    float event_ntuple[7];
    float ADC_DEFF,TDC_DEFF;
    char iname[32],num[16],comment[2];
    //----------------------------------------------------

    st = wevent->strip_no;
    si = (int)(st/12)+1;   // si=1...6
    amph=wevent->amp;
    timedc=wevent->t_time;
    inte=wevent->square;

    itoa(st,num);
    strcat(iname,num);
    
    iname[0]='N';
    itoa(hist_num,num);
    strcat(iname,num);

    for(int j=0;j<120;++j){
       HHF1(10000,(float)j,(float)j*2); 
    }
    event_ntuple[0]=timedc;
    event_ntuple[1]=amph;
    event_ntuple[2]=inte;
    event_ntuple[3]=all_count_AT;
    event_ntuple[4]=st;
    event_ntuple[5]=timedc;
    event_ntuple[6]=amph;
    HHFN(30,event_ntuple); 
    
if(all_count_AT%10000==0 && all_count_AT>0){
      printf("EventProcessNtuple---now is %d--st=%d si=%d amph=%d timedc=%d inte=%d\n",all_count_AT,st,si,amph,timedc,inte);    
    //getchar();
}
    return(0);
}




