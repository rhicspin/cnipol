#ifndef __RHIC_CLASS_H
#define __RHIC_CLASS_H

class TAsym {
    

public:

    // ============================================================
    //  Functions delcaration
    // ============================================================

    TAsym();   // constructor

    //int ReadLoop(void);
    int ReadLoop(char *,char *, char *);
    int RootInit(char *,int,char *,char *,char *);
    int RootClose(char *);
    int ReadKin(char *);
    int ReadMass(char *);
    int ReadReltime(char *);
    int ReadMassRelShift(char *);
  
    int EventProcess(processEvent *, recordConfigRhicStruct *);
    int EndProcess(recordConfigRhicStruct *);

    int PrintConfig(recordConfigRhicStruct *);
    void ReadConfig(recordConfigRhicStruct *);
    void ReadT0Config(recordConfigRhicStruct *);
    int ReadIAParam(char *);
    
    int sqass(float, float, float, float, float *, float *);
    float ekin(float, float);
    
//okadah///////////////
    int EventProcessWaveform(processWEvent *,recordConfigRhicStruct *,int wv[],int,int,int);
    int EventProcessPawNtuple(processWEvent *,recordConfigRhicStruct *,int,int,int);
    int EventProcessRootNtuple(processWEvent *,recordConfigRhicStruct *,int,int,int);
    int GetAts(int *,double *,int);
    int FitWaveform(int wv[],double FitResult[],double FitMaskResult[],int,int,WaveForm2Par *);
    int FitMASK(double FitMaskResult[]);
    int InputVector(processWEvent *,int *,int); 
    double GetMx(double,double);
    double get_Theta(double);
    double get_Energy(double);
    double ch2theta(int);
    double get_Mp(double,double,double,int);
    double get_ToF(double,int);
    int JetEventProcess(ThetaOffset *,WaveFormMap *,EnergyBin *,MxChan *,processWEvent *,recordConfigRhicStruct *,double,double FitResult[]);
    double banana(double,int);
    int Get8Elements(EnergyBin *,MxChan *,processWEvent *,process8Elements *);
    int GetBunchPat(MxChan *,processWEvent *);
    int reset8Elements(process8Elements *);
    int reset_each8Elements(process8Elements *);
    int sum_each8Elements(process8Elements *);
    int Elements2Asym(process8Elements *,processAsym *);
    int resetAsym(processAsym *);
    int readMap(WaveFormMap *);
    int readMxChan(MxChan *);
    int FitWaveMask900(WaveFormMap *,int wv[],double FitResult[],int,int);
    int setEbin(EnergyBin *);
    int CutBadWf(double,int,int,double,double );
    int BinSelect(EnergyBin *,MxChan *,processWEvent * );
    double GetInE(double,int);

///////////////////////////okadahEND

    // ===============================================================
    // Global variables 
    // ===============================================================
    
    int spinpat[120];                  // spin pattern 120 bunches
    int fillpat[120];                  // fill pattern 120 bunches
    float wcmdist[120];                // wall current monitor 120 bunches
    int nring;
    
    long int Ncounts[6][120];          // counts 6detectors 120 bunches
    long int NTcounts[6][120][6];      // counts in 6 -t range 
    long int NRcounts[6][120][RAMPTIME]; // counts for RAMPTIME sec
    long int Tcounts[96][3][10];  // counts 72 strips 3spin 10 -t bin
    int NSTcounts[96][3];         // counts for strips 3spin
    
    long int FUcounts[2];   // Up spin counts at flattop 0:R 1:L
    long int FDcounts[2];   // Dn spin counts at flattop 0:R 1:L
    
    long int Rlenedep[20];  // -t dependence study
    long int Llenedep[20];
    
    long int RUenedep[20];
    long int LUenedep[20];
    long int RDenedep[20];
    long int LDenedep[20];
    
    char datafile[256];            // data file name 
    char gasymmetry_file[256];     // asymmetry output file for labview
    char asymmetry_file[256];      // asymmetry output file for labview
    char t0_conf[256];             // update configuration file for T0 info
    char conf_file[256];           // update configuration file for T0 info
    
    int st_off[96];          // strip de-activation 1=de-activate
    long int totalEvent;     // total event without any cut
    int Cut;                 // 1: cut 0: without event cut  

    int rampbin;
    float iapar0[96];        // parameters from Integral-Amplitude fit 
    float iapar1[96];
    
    float dthick[6];       // dead layer thickness [ug/cm2]
    float kint0[96];        // t0 deviated from the online parameters

    float mcenter[96][10];
    float msigma[96][10];
    
    // relative mass shift each 
    //   bunch with respect to 11.17 GeV[GeV]
    float mrelcent[96][55];   


    long int Nevcut;        // number of events after 1st cut (whole data)
    long int Nevtot;        // number of total events (whole data) 
    long int Nread;         // real total events (completely everything)
    int Nskip;              // maxmum number of events to proceed
    int tshift;
    
    long int Ngood[120];   // number of events af carbon cut (each bunch)
    long int Ntotal[120];  // number of events bf carbon cut (each bunch)
    long int Nback[120];   // number of events below curbon cut (each bunch)
    
    int Debug;
    
    datprocStruct dproc;

    struct {
        int amp;
        int amp0;
        int itg;
        int geo;
        int tdc;
        int tmax;
        int bid;
        int dlm;
        int stp;
        float ene;
        float tof;
        int spn;
        int rev;
        int rev0;
        double Mx;
        double Mp;
    } atdata;
    
    // variables for ROOT TTree Fill
    struct {
        int spin;
        long counts[6];
        float x90p,x90pe,x90a,x90ae,x90l,x90le;
        float x45p,x45pe,x45a,x45ae,x45l,x45le;
        float y45p,y45pe,y45a,y45ae,y45l,y45le;
        float c45p,c45pe,c45a,c45ae,c45l,c45le;
        float rl90,rl90e,rl45,rl45e,bt45,bt45e;
        long Ngood,Nback,Ntotal;
        float tx90p[6],tx90pe[6];
        float tx45p[6],tx45pe[6];
        float ty45p[6],ty45pe[6];
        long rampup[6],rampdw[6];
        long tstripup[96], tstripdw[96];
        float an,enel,eneu;
        float acoef,ecoef,edead,t0,dwidth,kint0;
    } TreeFill;

    // ROOT
    TFile *tfile;
    TTree *Event;
    TTree *Counts;
    TTree *Asym;
    TTree *Tbasym;
    TTree *Tstrip;
    TTree *Tramp;
    TTree *A_N;
    TTree *Coefs;

    TH1F *hbunchB[6];
    TH1F *hbunchB2[6];
    TH1F *hbunchY[6];
    TH1F *hbunchY2[6];
    TH1F *hbunchY_all;
    TH1F *hbunchB_all;
    TH1F *hcalib[96];
    TH1F *E_ch[96];
    TH1F *InE_ch[96];
    TH2F *InEE_ch[96];
    //TH1F *tubuE_ch[96];
    //TH1F *tubdE_ch[96];
    //TH1F *tdbuE_ch[96];
    //TH1F *tdbdE_ch[96];
    TH1F *ToF_si[6];
    TH1F *SeToF_si[6];
    TH1F *ToF_ch[96];
    TH1F *ToF0_ch[96];
    TH2F *Theta0_si[6];
    TH1F *Theta0_ch[96];
    TH1F *d_Theta0_ch[96];
    TH1F *hwcm, *hdelim;
    TH1F *henetotal;
/*
    TH1F *hstall[6], *hstcbn[6];
    TH1F *heneall[6], *henecbn[6];
    TH1F *heneallitg[6],*henecbnitg[6];
    TH1F *henestpl[96], *henestng[96];
    TH2F *hdlayer[96], *ht0[96];
    TH2F *hia[96], *hti[96];
    TH2F *htofene[96],*htofdep[96];
    TH2F *htdcadcpos[96], *htdcadcneg[96];
    TH2F *htdcintpos[96], *htdcintneg[96];

    TH1F *hbunch_10s[120];  
    TH1F *hbunch_m20s[120];  
    TH1F *hbunch_m30s[120];  
    TH1F *hbunch_p20s[120];  
    TH1F *hbunch_p30s[120];  
    TH1F *hbunch_noc[120];  

    TH1F *hmass_bunch[96][55];

    TH1F *hlowedg[96], *huppedg[96];
    TH1F *hmassall[96], *hmasscbn[96];
    TH2F *hmassenea[96], *hmassenei[96];
    TH2F *hramp[40];
    TH1F *htshift[60];
*/
//okadah/////////////////////////////
    TH1F *hist_tau_ch[96];
    TH2F *hist_tauh_ch[96];
    TH1F *hist_n_ch[96];
    TH2F *hist_nh_ch[96];
    TH1F *hist_chi_ch[96];
    TH1F *hist_cndf_ch[96];
    TH2F *hist_cndfh_ch[96];
    

    TH1F *hcalib_intg[96];
    TH1F *tdc_ch[96];
    TH2F *ADC_INTG3[96];
    TH2F *ADC_INTG3_bad[96];
    TH2F *ADC_INTG3_bad2[96];
    TH2F *ADC_INTG3_ratio[96];
    TH1F *ratio_hist[96];
    TH2F *ADC_INTG3_seppen[96];
    TH2F *Pmax_Area[96];
    TH1F *INTG3;
    TH1F *region_INTG3;
    TH1F *hist_n;
    TH2F *hist_nh;
    TH1F *hist_tau;
    TH2F *hist_tauh;
    TH1F *hist_Pmax;
    TH1F *hist_ADC;
    TH1F *hist_ft_time;
    TH2F *hist_ft_TDC;
    TH1F *hist_ftmax;
    TH1F *hist_TMAX;
    TH1F *hist_TDC;
    TH2F *hist_ftmax_TMAX;
    TH1F *TDC_G;
    TH2F *hist_FitBanana;
    TH2F *hist_Pmaxh;
    TH1F *hist_ndf;
    TH2F *hist_ndf_ats;
    TH1F *hist_cndf;
    TH1F *hist_chi;
    TH2F *hist_cndfh;
    TH1F *hist_dArea;
    TH2F *hist_Areah;
    TH1F *hist_Area;
    TH1F *hist_dh;
    TH2F *hist_dd;
    TH2F *C_INTG_on;
    TH2F *banana_ch[96];
    TH2F *banana_si[6];
    TH2F *banana_INTG_si[6];
    TH2F *bananaFIT_si[6];
    TH2F *banana2_si[6];
    TH2F *banana3_si[6];
    TH2F *banana2_ch[96];
    TH2F *Ex_si[6];
    TH2F *InEx_si[6];
    TH2F *AveEx_si[6];
    TH2F *TOFx_si[6];
    TH1F *bunch_si[6];
    TH1F *Mp_ch[96];
    TH1F *Mx_ch[96];
    TH1F *Mp_si[6];
    TH1F *Mx_si[6];
    TH1F *Mp_si_0[6];
    TH1F *Mp_si_1[6];
    TH1F *Mp_si_2[6];
    TH1F *Mp_si_3[6];
    TH1F *Mp_si_4[6];
    TH1F *Mp_si_5[6];
    TH1F *Mp_si_6[6];
    TH1F *Mp_si_7[6];
    TH1F *Mp_si_8[6];
    TH1F *Mp_si_9[6];
    TH1F *Mp_si_10[6];
    TH1F *Mp_si_11[6];
    TH1F *Mx_si_0[6];
    TH1F *Mx_si_1[6];
    TH1F *Mx_si_2[6];
    TH1F *Mx_si_3[6];
    TH1F *Mx_si_4[6];
    TH1F *Mx_si_5[6];
    TH1F *Mx_si_6[6];
    TH1F *Mx_si_7[6];
    TH1F *Mx_si_8[6];
    TH1F *Mx_si_9[6];
    TH1F *Mx_si_10[6];
    TH1F *Mx_si_11[6];
    TH1F *chan_E[16];
    TH1F *chan_Etofchi[16];
    TH1F *chan_UraE[16];
    TH1F *chan_UraEtofchi[16];
    TH1F *chan_selectE[32];
    TH1F *chan_Etof6[32];
    TH1F *chan_Etof8[32];
    TH1F *chan_Etof10[32];
    TH2F *Check_select;
    TH2F *Check_Nonselect;

    TFile *tfileTestRaw;
    TFile *tfileTestWave;
    TFile *tfileTestEvent;
    TH1D *hist_event;//FitWaveform
    TH1D *hist_waveform;//event_pW
    TH2D *hist_residual;
    TF1 *func;
    TF1 *func_fix;
    TTree *JetEvent;
    TTree *JetEventRaw;

    struct {
       int dummy1;
       int dummy2;
       int square;
       int event_num;
       int revolution;
       int bunch;
       int hit_num;
       int array_num;
       int hjet_count;
       int target_inf;
       double HitData[96*17];
    } TreeJet;

    struct {
       int amp;
       int tdc;
       int timemax;
       int itg;
       int geo;
       int event_num;
       int revolution;
       int rev1;
       int rev0;
       int delim;
       int bid;
//HitData[17*96] no nakami
       int stp;
       int amp2;
       int time2;
       int timemax2;
       int square2;
//Energy,tof
       double amp3;
       double time3;
       double timemax3;
       double square3;
       double f_amp;
       double f_time;
       double f_timemax;
       double f_square;
       int f_ndf;
       double f_chi;
       double f_cndf;
       double Energy;
       double ToF;
//target info
       int hjet_count;
       int target_inf;
       double Mx;
       double Mp;
    } TreeJetRaw;

 
};

#endif // __RHIC_CLASS_H
