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


void itoa(int val,char *s);
char *strrev(char *s);

int TAsym::EventProcessRootNtuple(processWEvent *wevent, recordConfigRhicStruct *cfginfo,int hist_num,
         int mode,int all_event){   
    int i,st;     // st : c like 0-23
    int si,amph,timedc,inte;
    int bunch,revolution;
    float ADC_DEFF,TDC_DEFF;
    char iname[32],num[16],comment[2];
    //----------------------------------------------------

    itoa(st,num);
    strcat(iname,num);
    
    iname[0]='N';
    itoa(hist_num,num);
    strcat(iname,num);

    
    //tfileTestRaw->cd();
    //tfile->cd("TestNtuple");
    //gDirectory->pwd();
    //printf("now is RAW\n");
//	printf("hjet_count=%d rev=%d bunch=%d all_event=%d ch=%d\n",
//			wevent->hjet_count,wevent->revolution,wevent->bunch,all_event,wevent->geo);
		//getchar();
               
    TreeJetRaw.geo=wevent->geo;       
    TreeJetRaw.itg=wevent->intg;       
    TreeJetRaw.event_num=all_event;
    TreeJetRaw.revolution=wevent->revolution;
    TreeJetRaw.delim=wevent->delim;
    TreeJetRaw.bid=wevent->bunch;
    TreeJetRaw.stp=wevent->strip_no;       
    TreeJetRaw.amp2=wevent->amp2;
    TreeJetRaw.time2=wevent->t_time2;
    TreeJetRaw.timemax2=wevent->tmax2;
    TreeJetRaw.square2=wevent->square2; 
    TreeJetRaw.amp3=wevent->amp3;
    TreeJetRaw.square3=wevent->square3; 
    TreeJetRaw.Energy=wevent->Energy; 
    TreeJetRaw.ToF=wevent->ToF; 
    TreeJetRaw.f_amp=wevent->f_amp;
    TreeJetRaw.f_time=wevent->f_t_time;
    TreeJetRaw.f_timemax=wevent->f_tmax;
    TreeJetRaw.f_square=wevent->f_square; 
    TreeJetRaw.f_ndf=wevent->f_ndf; 
    TreeJetRaw.f_chi=wevent->f_chi; 
    TreeJetRaw.f_cndf=wevent->f_cndf; 
    TreeJetRaw.target_inf=wevent->target_inf;
    TreeJetRaw.hjet_count=wevent->hjet_count;
    TreeJetRaw.Mx=wevent->Mx;
    TreeJetRaw.Mp=wevent->Mp;
    //printf("chi=%lf cndf=%lf\n",wevent->f_chi,wevent->f_cndf);
    //getchar();
    JetEventRaw->Fill();
    return(0);
}




