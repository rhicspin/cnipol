// Event mode data analysis tool
//  file name :   rhic_process.C
// 
//  Author    :   Osamu Jinnouchi
//  Creation  :   12/9/2003
//  Comment   :   optimized for the detailed analysis for the event mode data
//                

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

// ======================================================== 
// Processing one event 
// ======================================================== 
int TAsym::JetEventProcess(ThetaOffset *TOff, WaveFormMap *wfm,EnergyBin *bin,MxChan *chan,processWEvent *wevent, recordConfigRhicStruct *cfginfo,double geta,double FitResult[]) {
     int ch=wevent->geo-1;
     int si;
     si=(int)(ch/16);//si=0-5
     double dt=1.19;//tdc->ns
     double dt_range=20;//12.5;//ns
     double dt_rangeUP;//12.5;//ns
     double dt_rangeDOWN=35;//12.5;//ns
     double dt_rangeBIG=20;//ns
     double E_CutCH_L[32][2]; //MeV
     double E_CutCH_R[32][2]; //MeV
     int  E_Cut_num=0;
     double Theta;
     for (int i=0;i<32;++i){
       E_CutCH_R[i][0]=0;
       E_CutCH_R[i][1]=10;
       E_CutCH_L[i][0]=0;
       E_CutCH_L[i][1]=10;
     }
     E_Cut_num=(wevent->geo-1)%16;
     if(wevent->geo >=17 && wevent->geo<=32){
         E_Cut_num=E_Cut_num+16;
     }
     if(wevent->geo >=65 && wevent->geo<=80){
         E_Cut_num=E_Cut_num+16;
     }
//Left Si1,3 HAMAMATSU
     E_CutCH_L[0][0]=0.7;E_CutCH_L[0][1]=1.8;
     E_CutCH_L[1][0]=0.7;E_CutCH_L[1][1]=2.3;
     E_CutCH_L[2][0]=0.8;E_CutCH_L[2][1]=3.1;
     E_CutCH_L[3][0]=1.0;E_CutCH_L[3][1]=4.0;
     E_CutCH_L[4][0]=1.5;E_CutCH_L[4][1]=4.7;
     E_CutCH_L[5][0]=2.3;E_CutCH_L[5][1]=5.4;
     E_CutCH_L[6][0]=3.1;E_CutCH_L[6][1]=7.2;
     E_CutCH_L[7][0]=4.0;E_CutCH_L[7][1]=7.3;

     E_CutCH_L[8][0]=3.4;E_CutCH_L[8][1]=7.3;
     E_CutCH_L[9][0]=3.0;E_CutCH_L[9][1]=5.5;
     E_CutCH_L[10][0]=2.6;E_CutCH_L[10][1]=4.8;
     E_CutCH_L[11][0]=2.3;E_CutCH_L[11][1]=4.0;
     E_CutCH_L[12][0]=2.0;E_CutCH_L[12][1]=3.5;
     E_CutCH_L[13][0]=1.8;E_CutCH_L[13][1]=3.0;
     E_CutCH_L[14][0]=1.7;E_CutCH_L[14][1]=2.5;
     E_CutCH_L[15][0]=1.6;E_CutCH_L[15][1]=2.5;

//Si2 BNL
     E_CutCH_L[16][0]=0.7;E_CutCH_L[16][1]=1.8;
     E_CutCH_L[17][0]=0.7;E_CutCH_L[17][1]=2.3;
     E_CutCH_L[18][0]=0.8;E_CutCH_L[18][1]=3.1;
     E_CutCH_L[19][0]=1.0;E_CutCH_L[19][1]=4.0;
     E_CutCH_L[20][0]=1.5;E_CutCH_L[20][1]=4.7;
     E_CutCH_L[21][0]=2.3;E_CutCH_L[21][1]=5.4;
     E_CutCH_L[22][0]=3.1;E_CutCH_L[22][1]=7.2;
     E_CutCH_L[23][0]=4.0;E_CutCH_L[23][1]=7.6;

     E_CutCH_L[24][0]=4.0;E_CutCH_L[24][1]=7.7;
     E_CutCH_L[25][0]=3.5;E_CutCH_L[25][1]=7.7;
     E_CutCH_L[26][0]=3.0;E_CutCH_L[26][1]=5.0;
     E_CutCH_L[27][0]=2.7;E_CutCH_L[27][1]=4.6;
     E_CutCH_L[28][0]=2.3;E_CutCH_L[28][1]=3.9;
     E_CutCH_L[29][0]=2.3;E_CutCH_L[29][1]=3.6;
     E_CutCH_L[30][0]=1.9;E_CutCH_L[30][1]=3.1;
     E_CutCH_L[31][0]=1.8;E_CutCH_L[31][1]=2.8;

//Right Si4,6 HAMAMATSU
     E_CutCH_R[0][0]=0;E_CutCH_R[0][1]=0;
     E_CutCH_R[1][0]=0;E_CutCH_R[1][1]=0;
     E_CutCH_R[2][0]=0.7;E_CutCH_R[2][1]=1.5;
     E_CutCH_R[3][0]=0.7;E_CutCH_R[3][1]=2.2;
     E_CutCH_R[4][0]=0.7;E_CutCH_R[4][1]=2.5;
     E_CutCH_R[5][0]=1.2;E_CutCH_R[5][1]=4.0;
     E_CutCH_R[6][0]=1.8;E_CutCH_R[6][1]=4.8;
     E_CutCH_R[7][0]=3.1;E_CutCH_R[7][1]=6.4;

     E_CutCH_R[8][0]=3.9;E_CutCH_R[8][1]=7.2;
     E_CutCH_R[9][0]=3.9;E_CutCH_R[9][1]=7.2;
     E_CutCH_R[10][0]=3.4;E_CutCH_R[10][1]=7.2;
     E_CutCH_R[11][0]=3.1;E_CutCH_R[11][1]=4.9;
     E_CutCH_R[12][0]=2.5;E_CutCH_R[12][1]=4.6;
     E_CutCH_R[13][0]=2.3;E_CutCH_R[13][1]=3.8;
     E_CutCH_R[14][0]=2.0;E_CutCH_R[14][1]=3.3;
     E_CutCH_R[15][0]=1.8;E_CutCH_R[15][1]=2.6;

//Si5 BNL
     E_CutCH_R[16][0]=0;E_CutCH_R[16][1]=0;
     E_CutCH_R[17][0]=0;E_CutCH_R[17][1]=0;
     E_CutCH_R[18][0]=0.7;E_CutCH_R[18][1]=1.3;
     E_CutCH_R[19][0]=0.7;E_CutCH_R[19][1]=2.0;
     E_CutCH_R[20][0]=0.7;E_CutCH_R[20][1]=2.8;
     E_CutCH_R[21][0]=1.1;E_CutCH_R[21][1]=3.7;
     E_CutCH_R[22][0]=1.8;E_CutCH_R[22][1]=4.8;
     E_CutCH_R[23][0]=2.5;E_CutCH_R[23][1]=5.9;

     E_CutCH_R[24][0]=3.8;E_CutCH_R[24][1]=7.0;
     E_CutCH_R[25][0]=4.3;E_CutCH_R[25][1]=7.7;
     E_CutCH_R[26][0]=4.3;E_CutCH_R[26][1]=7.7;
     E_CutCH_R[27][0]=3.4;E_CutCH_R[27][1]=6.6;
     E_CutCH_R[28][0]=3.4;E_CutCH_R[28][1]=5.3;
     E_CutCH_R[29][0]=2.6;E_CutCH_R[29][1]=4.5;
     E_CutCH_R[30][0]=2.4;E_CutCH_R[30][1]=3.8;
     E_CutCH_R[31][0]=2.1;E_CutCH_R[31][1]=3.1;


     if(dproc.ToF_W==1)dt_rangeUP=8.0;
     if(dproc.ToF_W==2)dt_rangeUP=6.0;
     if(dproc.ToF_W==3)dt_rangeUP=9.0;
     dt_rangeUP=8.0;
     int BinTyp=-1;
     BinTyp = BinSelect(bin, chan,wevent);
     int ch_minY = chan->select_minY[BinTyp][si];
     int ch_maxY = chan->select_maxY[BinTyp][si];
     int ch_minB = chan->select_minB[BinTyp][si];
     int ch_maxB= chan->select_maxB[BinTyp][si];
/*
     if(si==0){
         ch_min=1;ch_max=7;
         //ch_min=1;ch_max=ch_min+d_si;
     }else if(si==1){
          ch_min=17;ch_max=23;
          //ch_min=17;ch_max=ch_min+d_si;
     }else if(si==2){
        ch_min=33;ch_max=39;
          //ch_min=33;ch_max=ch_min+d_si;
     }else if(si==3){
          ch_min=51;ch_max=57;
          //ch_min=50;ch_max=ch_min+d_si;
     }else if(si==4){
          ch_min=67;ch_max=73;
          //ch_min=66;ch_max=ch_min+d_si;
     }else if(si==5){
          ch_min=83;ch_max=89;
          //ch_min=82;ch_max=ch_min+d_si;
     }
*/
//Bad waveform Cut study/////////////////////////////////////////////
     int energy_cut=0;

     if( si==0 || si==2 ){//HAMAMATSU
      if(  wevent->Energy < 4.7 || wevent->Energy > 5.6)energy_cut=1;
     }
     if(  si==3 || si==5){//HAMAMATSU
      if(wevent->Energy < 2.5 || (wevent->Energy > 3 && wevent->Energy < 4.7) || wevent->Energy > 5.7)energy_cut=1;
     }
     if( si==1 ){//BNL
      if(  wevent->Energy < 5.2 || wevent->Energy > 5.6)energy_cut=1;
     }
     if(  si==4 ){//BNL
      if( wevent->Energy < 2.8 || (wevent->Energy > 3.2 && wevent->Energy < 5.2) || wevent->Energy > 5.7)energy_cut=1;
     }
//temp
     int prompt_id=0;
     if( wevent->InE < 2 && (wevent->ToF-geta*dt) < 30  )prompt_id=1;
  
    


     int cut_frag=0;
     double cut_seppen;
     double cut_slope;
     cut_slope = wfm->ratio_All[wevent->geo-1];
     cut_seppen = wfm->seppen;
     if(cut_slope !=-1){
       cut_frag=CutBadWf(cut_slope,wevent->geo, wevent->amp2,wevent->square3,cut_seppen);
     }else{
       cut_frag=1;
     }


  if(dproc.SELECT_CHAN==-1  ){ 
    hcalib[ch]->Fill(wevent->amp);//ONLINE
    hcalib_intg[ch]->Fill(wevent->square3);
    tdc_ch[ch] -> Fill(wevent->t_time);//ONLINE

    ADC_INTG3_bad[ch]->Fill(wevent->amp2,wevent->square3);
/*

    if(  dproc.WAVE_FIT_MODE==1 && wevent->f_chi>0  && wevent->f_cndf<100){//Output Fitting

          hist_n_ch[ch]    -> Fill(wevent->f_n);
          hist_nh_ch[ch]   -> Fill(wevent->f_n,wevent->amp2);
          hist_tau_ch[ch]  -> Fill(wevent->f_tau);
          hist_tauh_ch[ch] -> Fill(wevent->f_tau,wevent->amp2);
          hist_cndf_ch[ch] -> Fill(wevent->f_cndf);
          hist_chi_ch[ch]  -> Fill(wevent->f_chi);
          hist_cndfh_ch[ch]-> Fill(wevent->f_cndf,wevent->amp2);

           Pmax_Area[ch]->Fill(wevent->f_amp,wevent->f_square/(double)4);
     //      printf("cndf=%lf ** %lf %d || %lf %lf\n",wevent->f_cndf,wevent->f_amp,
     //           wevent->amp2,wevent->f_square/(double)4,wevent->square3);
    }
*/
  } 
  if(dproc.SELECT_CHAN>=0){
//     if(dproc.CMODE ==1){
        if(wevent->region==31 /*&& wevent->t_time2>40 && wevent->t_time2< 90*/){
        //if(wevent->region>0 /*&& wevent->t_time2>40 && wevent->t_time2< 90*/){
           if(dproc.SELECT_CHAN == wevent->geo){
           //  region_INTG3->Fill(wevent->region);
          //   INTG3->Fill(wevent->square3);
             TDC_G->Fill(2*wevent->timeG);
          //   hist_TMAX->Fill(wevent->tmax2);
           }
        }
//     }
     if(dproc.SELECT_CHAN!=0 && dproc.SELECT_CHAN==wevent->geo && wevent->region==31 && wevent->f_chi>0 ){//Output Fitting
          hist_n   -> Fill(wevent->f_n);
          hist_nh   -> Fill(wevent->f_n,wevent->amp2);
          hist_tau -> Fill(wevent->f_tau);
          hist_tauh -> Fill(wevent->f_tau,wevent->amp2);
          hist_Pmax-> Fill(wevent->f_amp);
          hist_ADC-> Fill(wevent->amp2);
          if(wevent->f_cndf<4){
           hist_ft_TDC-> Fill(2*wevent->f_t_time,wevent->t_time2);
           hist_ftmax_TMAX->Fill(2*wevent->f_tmax,wevent->tmax2);
           hist_Pmaxh-> Fill(wevent->f_amp,wevent->amp2);
//           C_INTG_on->Fill(wevent->f_square/(double)4,wevent->square3);
          }
          if(wevent->f_cndf>=4)C_INTG_on->Fill(wevent->amp2,wevent->square3);
          hist_TMAX->Fill(wevent->tmax2);
          hist_TDC->Fill(wevent->t_time2);
          hist_ft_time-> Fill(2*wevent->f_t_time);
          hist_ftmax->Fill(2*wevent->f_tmax);
          hist_FitBanana-> Fill(wevent->f_amp,2*wevent->f_t_time);
          hist_ndf -> Fill(wevent->f_ndf);
          hist_ndf_ats -> Fill(wevent->region,wevent->f_ndf);
          hist_cndf-> Fill(wevent->f_cndf);
          hist_chi-> Fill(wevent->f_chi);
          hist_cndfh-> Fill(wevent->f_cndf,wevent->amp2);
          hist_dh-> Fill(wevent->f_amp-wevent->amp2);
          hist_dArea-> Fill((wevent->f_square)/(double)4-wevent->square2);
          hist_Areah-> Fill((wevent->f_square)/(double)4,wevent->square2);
          double ttt=(wevent->f_square/(double)4);
          hist_Area-> Fill(ttt);
          region_INTG3->Fill(wevent->tmax3);
          INTG3->Fill(wevent->square3);
          hist_dd-> Fill((wevent->f_square)/(double)4-wevent->square2,wevent->f_amp-wevent->amp2);
     }
  }//END if(dproc.SELECT_CHAN>=0)          

  int select_frag=0;
  if(dproc.SELECT_CHAN==-1){

     double checkE_tof=banana(wevent->Energy,wevent->geo);
     double checkE_tdc=checkE_tof/(double)dt+geta;
     //printf("E=%lf tof=%lf Ctof=%lf\n",wevent->Energy,wevent->ToF,checkE_tof);
     //getchar();
     double E,TOF;
     E=wevent->Energy;//MeV
     TOF=wevent->ToF-geta*dt;//ns
     int iner_chan;
     iner_chan=wevent->geo%16;
     if(iner_chan==0)iner_chan=16;

     if(wevent->geo>=ch_minY && wevent->geo<=ch_maxY)select_frag=1;
     if(wevent->geo>=ch_minB && wevent->geo<=ch_maxB)select_frag=1;

     int punch_frag=0;
     if(si<3){
        if(wevent->geo >= (9+si*16) )punch_frag=1;
     }else{
        if(wevent->geo >= (11+si*16) )punch_frag=1;
     }

     int ECutFrag=0;
     if(wevent->geo<49){
       if( wevent->Energy > E_CutCH_L[E_Cut_num][0] && wevent->Energy < E_CutCH_L[E_Cut_num][1]){
         ECutFrag=1;
       }
     }else{
       if( wevent->Energy > E_CutCH_R[E_Cut_num][0] && wevent->Energy < E_CutCH_R[E_Cut_num][1]){
         ECutFrag=1;
       }
     }

     double dT_check,dT_check2,dT_check3,AveToF0;
     //banana_ch[ch] -> Fill(wevent->InE,TOF);
     //banana_ch[ch] -> Fill(wevent->Energy,TOF);
     //banana_ch[ch] -> Fill(wevent->amp,wevent->t_time);//ONLINE
     banana_ch[ch] -> Fill(wevent->amp2,wevent->t_time2);//WAVEFORM_ONLINE_way

     dT_check=sqrt(pow(wevent->ToF0-TOF,2));//from InE                            
     double depToF0=get_ToF(wevent->Energy,wevent->geo);
     dT_check2=sqrt(pow(depToF0-TOF,2));//form deposit Energy                            
     dT_check3=depToF0-TOF;//form deposit Energy                            

     int frag=0;
     if(si==0 && wevent->geo >=12 && wevent->geo <=14) frag=1;
     if(si==1 && wevent->geo >=28 && wevent->geo <=30) frag=1;
     if(si==2 && wevent->geo >=44 && wevent->geo <=46) frag=1;
     if(si==3 && wevent->geo >=59 && wevent->geo <=61) frag=1;
     if(si==4 && wevent->geo >=75 && wevent->geo <=77) frag=1;
     if(si==5 && wevent->geo >=92 && wevent->geo <=94) frag=1;
     if(si==0 && wevent->geo >=4 && wevent->geo <=6) frag=2;
     if(si==1 && wevent->geo >=20 && wevent->geo <=22) frag=2;
     if(si==2 && wevent->geo >=35 && wevent->geo <=37) frag=2;
     if(si==3 && wevent->geo >=51 && wevent->geo <=53) frag=2;
     if(si==4 && wevent->geo >=67 && wevent->geo <=69) frag=2;
     if(si==5 && wevent->geo >=83 && wevent->geo <=85) frag=2;
     if(  dT_check2 < 8 && wevent->Energy > 1 && wevent->Energy <4){
        if(wevent->geo%16 <=8 && wevent->geo%16!=0)hbunchY_all -> Fill(wevent->bunch);//WAVEFORM_ONLINE_way
        //if(wevent->geo%16 <=8 && wevent->geo%16!=0)frag=2;
        if(wevent->geo%16 >8 || wevent->geo%16==0)hbunchB_all -> Fill(wevent->bunch);//WAVEFORM_ONLINE_way
        //if(wevent->geo%16 >8 || wevent->geo%16==0)frag=1;
        if(frag==2 ){//YELLOW
           //wevent->our_bunchY=wevent->bunch;
           hbunchY[si] -> Fill(wevent->bunch);//WAVEFORM_ONLINE_way
           hbunchY2[si] -> Fill(wevent->our_bunchY);//WAVEFORM_ONLINE_way
        }
        if(frag==1 ){//BLUE
           //wevent->our_bunchB=wevent->bunch;
           hbunchB[si] -> Fill(wevent->bunch);//WAVEFORM_ONLINE_way
           hbunchB2[si] -> Fill(wevent->our_bunchB);//WAVEFORM_ONLINE_way
        }
     }

     banana2_ch[ch] -> Fill(wevent->Energy,TOF);
     //if( E >= 0.4)banana_si[si] -> Fill(wevent->InE,TOF);
     banana_si[si] -> Fill(wevent->amp,wevent->t_time);//ONLINE
     if(dT_check2<12) Ex_si[si] ->Fill(iner_chan,wevent->Energy);

    if(dT_check2 < 8 || dT_check3 < 0 ){
      ADC_INTG3[ch]->Fill(wevent->amp2,wevent->square3);
    }
    ADC_INTG3_bad[ch]->Fill(wevent->amp2,wevent->square3);
     
     if(E>=0.4 /*energy_cut ==1 && select_frag==1*/ ){
         AveToF0=get_ToF(wevent->AveE,wevent->geo);
         //banana2_si[si] -> Fill(wevent->AveE,AveToF0);
         if(dT_check < 8 ){
          // banana3_si[si] -> Fill(wevent->InE,TOF);
         }
     }
     banana_INTG_si[si] -> Fill(wevent->square3,wevent->t_time2);
     if(dproc.WAVE_FIT_MODE==1 && FitResult[7]<10){
        bananaFIT_si[si] -> Fill(wevent->amp,wevent->t_time);
     }

  
  //if( dT_check < 10 ){
  if( dT_check < 10 && wevent->region == 31){
        int beam_pol; //+/-/0
        if(wevent->our_bunchB<112){
            int tmp;
            tmp=wevent->our_bunchB%4;
            if(tmp==0)beam_pol=0;//+beam up
            if(tmp==2)beam_pol=1;//-beam down
            if(tmp==1 || tmp==3)beam_pol=2;
          }else{
            beam_pol=2;
        }
      if(iner_chan<9 && wevent->Energy > 0.5){
//Willy
           //if(beam_pol==0 && wevent->target_inf==1)tubuE_ch[ch] -> Fill(E);
           //if(beam_pol==0 && wevent->target_inf==2)tubdE_ch[ch] -> Fill(E);
           //if(beam_pol==1 && wevent->target_inf==1)tdbuE_ch[ch] -> Fill(E);
           //if(beam_pol==1 && wevent->target_inf==2)tdbdE_ch[ch] -> Fill(E);
      }
      if(iner_chan>=9 && wevent->Energy > 1.0){
//Willy
           //if(beam_pol==0 && wevent->target_inf==1)tubuE_ch[ch] -> Fill(E);
           //if(beam_pol==0 && wevent->target_inf==2)tubdE_ch[ch] -> Fill(E);
           //if(beam_pol==1 && wevent->target_inf==1)tdbuE_ch[ch] -> Fill(E);
           //if(beam_pol==1 && wevent->target_inf==2)tdbdE_ch[ch] -> Fill(E);
      }
   }
//Sandro
   if(wevent->Energy >=1.25 && wevent->Energy <=1.75){
       ToF_si[si] -> Fill(TOF);
       if((wevent->geo >= ch_minY && wevent->geo <= ch_maxY)||(wevent->geo >= ch_minB && wevent->geo <= ch_maxB))SeToF_si[si] -> Fill(TOF);
   }
  //if(ECutFrag ==1 && energy_cut==1 && wevent->region==31 &&  (( wevent->InE >7  && wevent->Energy > 1.5)  || wevent->InE <=7  ) ){
  if( energy_cut==1 && wevent->region==31 &&  wevent->Energy > 0.6 && (( wevent->InE >7  && wevent->Energy > 2.0)  || wevent->InE <=7  ) ){
       ToF_ch[ch] -> Fill(TOF);
       //ToF_ch[ch] -> Fill(wevent->t_time2);
        ToF0_ch[ch] -> Fill((wevent->ToF-geta*dt)-(wevent->ToF0));
       if(dT_check < 8){
         //Mp_ch[ch] -> Fill(wevent->Mp);
         banana3_si[si] -> Fill(wevent->InE,TOF);
       }
  }
  //banana2_si[si] -> Fill(wevent->InE,TOF);
  banana2_si[si] -> Fill(wevent->Energy,TOF);

  if(/*ECutFrag ==1 &&*/ wevent->region==31 &&  dT_check < 8 &&    
              (wevent->InE < 7 || ( wevent->InE>=7 && wevent->Energy>1.5 )) ){
//ECutFrag = ch by channel ECut, energy_cut = alpha cut
       Mx_ch[ch] -> Fill(wevent->Mx);

       if(energy_cut==1 && dT_check < 15){
         Theta0_ch[ch] -> Fill(wevent->Theta0);
         Theta0_si[si] -> Fill(wevent->Theta0,wevent->InE);
       }
       d_Theta0_ch[ch] -> Fill(wevent->Theta0-wevent->Theta);
  }
  bunch_si[si] -> Fill(wevent->bunch);

  if(dT_check < dt_rangeUP ){
    Check_Nonselect->Fill(wevent->geo,wevent->Energy);
    if( select_frag==1){
         if(wevent->Energy < 5.2)Check_select->Fill(wevent->geo,wevent->Energy);
    }
  }
  

//ECutFrag = ch by channel ECut, energy_cut = alpha cut
  //if(  TOF > 30 && TOF < 50 && wevent->Energy > 1 && wevent->Energy <2)chan_E[15] -> Fill(wevent->geo);
  if(dT_check < 12 && wevent->Energy > 1 && wevent->Energy <2)chan_E[15] -> Fill(wevent->geo);
  if(dT_check<6 && wevent->Energy > 1 && wevent->Energy <2 )chan_Etof6[15]->Fill(wevent->geo);
  if(dT_check<8 && wevent->Energy > 1 && wevent->Energy <2 )chan_Etof8[15]->Fill(wevent->geo);
  if(dT_check<10 && wevent->Energy > 1 && wevent->Energy <2 )chan_Etof10[15]->Fill(wevent->geo);
  if(wevent->region == 31  ){
       if(BinTyp >=0 && BinTyp<=14){
           if(dT_check < 8){//dt_rangeUP
              chan_E[BinTyp] -> Fill(wevent->geo);
              if((wevent->geo>=ch_minY && wevent->geo <=ch_maxY) ||  ( wevent->geo>=ch_minB && wevent->geo <=ch_maxB)){
                 chan_selectE[BinTyp] -> Fill(wevent->geo);
              }
           }
       }
////Deposit energy
int DepBin=-1;
//BIN0
       if(0.8 <= wevent->Energy && 1.0 > wevent->Energy)DepBin=0;
//BIN1
       if(1.0 <= wevent->Energy && 1.4 > wevent->Energy)DepBin=1;
//BIN2
       if(1.4 <= wevent->Energy && 1.8 > wevent->Energy)DepBin=2;
//BIN3
       if(1.8 <= wevent->Energy && 2.2 > wevent->Energy)DepBin=3;
//BIN 4-->2.2-2.8 MeV
       if(si==1 || si==4){
            if(2.2 <= wevent->Energy && 2.8 > wevent->Energy)DepBin=4;
       }else{
            if(2.2 <= wevent->Energy && 2.5 > wevent->Energy)DepBin=4;
       }
//BIN 7--> 3.0-3.5 MeV
        if(si==0 || si==2 || si==3 || si==5){
             if(3.0 <= wevent->Energy && 3.5 > wevent->Energy)DepBin=7;
        }else{
             if(3.2 <= wevent->Energy && 3.5 > wevent->Energy)DepBin=7;
        }
//BIN 8--> 3.5-4.2 MeV
        if(3.5 <= wevent->Energy && 4.2 > wevent->Energy)DepBin=8;
//BIN 9--> 4.2-5.2 MeV
        if(si==0 || si==2 || si==3 || si==5){
             if(4.2 <= wevent->Energy && 4.7 > wevent->Energy)DepBin=9;
        }else{
            if(4.2 <= wevent->Energy && 5.2 > wevent->Energy)DepBin=9;
        }

        if(DepBin >=0 && DepBin<10){
          chan_UraE[DepBin] -> Fill(wevent->geo);
          if(dT_check2 < 8 && wevent->f_cndf < 5) chan_UraEtofchi[DepBin] -> Fill(wevent->geo);
          //if(dT_check2 <10)chan_Etof10[DepBin]->Fill(wevent->geo);
          //if(dT_check2 < 8)chan_Etof8[DepBin]->Fill(wevent->geo);
          //if(dT_check2 < 6)chan_Etof6[DepBin]->Fill(wevent->geo);
        }

DepBin=-1;
//Alpha source remve
        int alpha_frag=0;
        if(si==0 || si==2 || si==3 || si==5){
             if(wevent->Energy >=4.7 && wevent->Energy < 5.7)alpha_frag=1;
             if(wevent->Energy >=2.5 && wevent->Energy < 3.0)alpha_frag=1;
        }else{
             if(wevent->Energy >=5.2 && wevent->Energy < 5.7)alpha_frag=1;
             if(wevent->Energy >=2.8 && wevent->Energy < 3.2)alpha_frag=1;
        } 
        if(si==1 || si==4){//BNL using PUNCH_MAP_B_REVISED.csv
//BIN 10--> 5.7-7.3 MeV
           if(alpha_frag==0 && 5.7 <= wevent->Energy && 7.3 > wevent->Energy)DepBin=10;
//BIN 11--> 8-9.0 MeV
           if(alpha_frag==0 && 4.5 <= wevent->Energy && 5.4 > wevent->Energy)DepBin=11;
//BIN 12--> 9-10.5 MeV
           if(alpha_frag==0 && 3.7 <= wevent->Energy && 4.5 > wevent->Energy)DepBin=12;
//BIN 13--> 10.5-12 MeV
           if(alpha_frag==0 && 3.2 <= wevent->Energy && 3.7 > wevent->Energy)DepBin=13;
//BIN 14--> 14.5-16.5 MeV
           if(alpha_frag==0 && 2.4 <= wevent->Energy && 2.7 > wevent->Energy)DepBin=14;
//BIN 15--> 16.5-20.0 MeV
           if(alpha_frag==0 && 2.0 <= wevent->Energy && 2.4 > wevent->Energy)DepBin=15;
        }else{//HAMAMATSU using PUNCH_MAP_H_REVISED.csv
//BIN 10--> 5.7-7.0 MeV
           if(alpha_frag==0 && 5.7 <= wevent->Energy && 7.0 > wevent->Energy)DepBin=10;
//BIN 11--> 8-9.0 MeV
           if(alpha_frag==0 && 4.1 <= wevent->Energy && 4.8 > wevent->Energy)DepBin=11;
//BIN 12--> 9-10.5 MeV
           if(alpha_frag==0 && 3.4 <= wevent->Energy && 4.1 > wevent->Energy)DepBin=12;
//BIN 13--> 10.5-12.0 MeV
           if(alpha_frag==0 && 3.0 <= wevent->Energy && 3.4 > wevent->Energy)DepBin=13;
//BIN 14--> 14.5-16.5 MeV
           if(alpha_frag==0 && 2.2 <= wevent->Energy && 2.5 > wevent->Energy)DepBin=14;
//BIN 15--> 16.5-20 MeV
           if(alpha_frag==0 && 1.9 <= wevent->Energy && 2.2 > wevent->Energy)DepBin=15;
        }
///
        if(DepBin >=10 && DepBin<16){
           chan_UraE[DepBin] -> Fill(wevent->geo);
           if(dT_check < 8 && wevent->f_cndf < 5) chan_UraEtofchi[DepBin] -> Fill(wevent->geo);
          // if(dT_check <10)chan_Etof10[DepBin]->Fill(wevent->geo);
          // if(dT_check < 8)chan_Etof8[DepBin]->Fill(wevent->geo);
          // if(dT_check < 6)chan_Etof6[DepBin]->Fill(wevent->geo);
        }

   }
//Energy spectrum
   //if( wevent->region==31 && (wevent->InE<7 || (wevent->InE>=7 && wevent->Energy >1.0))   ){
   if( wevent->region==31   ){
       if(dT_check < 8){//TOF_CUT
           E_ch[ch] -> Fill(E);
           if(wevent->InE<7 || (wevent->InE>=7 && wevent->Energy >1.0)   ){
              InE_ch[ch] -> Fill(wevent->InE);
              InEE_ch[ch]->Fill(wevent->InE,wevent->Energy);
           }
       }
/*
//RUN2005
        if((punch_frag==0 && wevent->Energy > 0.7) || (punch_frag==1 && wevent->Energy >1.0)){
           Ex_si[si] ->Fill(iner_chan,wevent->Energy);
           InEx_si[si] ->Fill(iner_chan,wevent->InE);
        }
*/
        AveEx_si[si] ->Fill(iner_chan,wevent->AveE);
        Mx_si[si] -> Fill(wevent->Mx);
        if((dT_check2 < 8 || dT_check3 <0) && wevent->Energy > 1 && wevent->Energy <5)  Mp_si[si] -> Fill(wevent->Mp);
        if((dT_check2 < 8 || dT_check3 <0) && wevent->Energy > 1 && wevent->Energy <5)  Mp_ch[ch] -> Fill(wevent->Mp);
        TOFx_si[si] ->Fill(iner_chan,TOF);

//Mp_E,Mx_E
       if(wevent->InE>=0.7 && wevent->InE<1.0 ){ 
         Mp_si_0[si] -> Fill(wevent->Mp);
         Mx_si_0[si] -> Fill(wevent->Mx);
       }
       if(wevent->InE>=1.0 && wevent->InE<1.4 ){ 
         Mp_si_1[si] -> Fill(wevent->Mp);
         Mx_si_1[si] -> Fill(wevent->Mx);
       }
       if(wevent->InE>=1.4 && wevent->InE<1.8 ){ 
         Mp_si_2[si] -> Fill(wevent->Mp);
         Mx_si_2[si] -> Fill(wevent->Mx);
       }
       if(wevent->InE>=1.8 && wevent->InE<2.2 ){ 
         Mp_si_3[si] -> Fill(wevent->Mp);
         Mx_si_3[si] -> Fill(wevent->Mx);
       }
       if(wevent->InE>=2.2 && wevent->InE<2.8 ){ 
         Mp_si_4[si] -> Fill(wevent->Mp);
         Mx_si_4[si] -> Fill(wevent->Mx);
       }
       if(wevent->InE>=3.0 && wevent->InE<3.5  ){ 
         Mp_si_5[si] -> Fill(wevent->Mp);
         Mx_si_5[si] -> Fill(wevent->Mx);
       }
       if(wevent->InE>=3.5 && wevent->InE<4.2  ){ 
         Mp_si_6[si] -> Fill(wevent->Mp);
         Mx_si_6[si] -> Fill(wevent->Mx);
       }
       if(wevent->InE>=4.2 && wevent->InE<5.2  ){ 
         Mp_si_7[si] -> Fill(wevent->Mp);
         Mx_si_7[si] -> Fill(wevent->Mx);
       }
       if((wevent->InE>=5.7 && wevent->InE<7.0) || (wevent->InE>=8.5 && wevent->InE<10.0) ){ 
         Mp_si_8[si] -> Fill(wevent->Mp);
         Mx_si_8[si] -> Fill(wevent->Mx);
       }
       if((wevent->InE>=10 && wevent->InE<12) || (wevent->InE>=14 && wevent->InE<16.0)  ){ 
         Mp_si_9[si] -> Fill(wevent->Mp);
         Mx_si_9[si] -> Fill(wevent->Mx);
       }
       if(wevent->InE>=16 && wevent->InE<20.0  ){ 
         Mp_si_10[si] -> Fill(wevent->Mp);
         Mx_si_10[si] -> Fill(wevent->Mx);
       }
       if((wevent->InE>=5.7 && wevent->InE<7.0) || (wevent->InE>=8.5 && wevent->InE<10.0) || 
                (wevent->InE>=10 && wevent->InE<12) || (wevent->InE>=14 && wevent->InE<16.0)  ){ 
         Mp_si_11[si] -> Fill(wevent->Mp);
         Mx_si_11[si] -> Fill(wevent->Mx);
       }
  }//if(wevent->region == 31 &&  (wevent->InE<7 || (wevent->InE>=7 && wevent->Energy >1.5)) ){

}//END if(dproc.SELECT_CHAN==-1)          

    return(0);
}







