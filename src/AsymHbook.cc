//  Asymmetry Analysis of RHIC pC Polarimeter
//  file name :   AsymHbookcc
//
//  Author    :   I. Nakagawa
//                Dmitri Smirnov
//
//  Creation  :   10/21/2005
//

#include "AsymHbook.h"

using namespace std;

void HHBOOK1(int hid, char* hname, int xnbin, float xmin, float xmax)
{
    hhbook1d_(&hid, hname, &xnbin, &xmin, &xmax, strlen(hname));
    return;
}

void HHBOOK2(int hid, char* hname, int xnbin, float xmin, float xmax,
             int ynbin, float ymin, float ymax)
{
    hhbook2d_(&hid, hname, &xnbin, &xmin, &xmax, &ynbin, &ymin, &ymax,
              strlen(hname));
    return;
};

void HHF1(int hid, float xval, float weight) {
    hhf1_(&hid, &xval, &weight);
    return;
};

void HHF2(int hid, float xval, float yval, float weight) {
    hhf2_(&hid, &xval, &yval, &weight);
    return;
};

void HHPAK(int hid, float *xval) {hhpak_(&hid, xval);  return;};

void HHPAKE(int hid, float *errors) {
    hhpake_(&hid, errors);
    return;
};

float HHMAX(int hid){return hhmax_(&hid);};

float HHSTATI(int hid, int icase, char * choice, int num) {
  return  hhstati_(&hid, &icase, choice, &num, strlen(choice));
};


void
HHFITHN(int hid, char*chfun, char*chopt, int np, float*par,
        float*step, float*pmin, float*pmax, float*sigpar, float&chi2){
  hfithn_(&hid, chfun, chopt, &np, par, step, pmin, pmax, sigpar, &chi2,
          strlen(chfun),strlen(chopt) );
  return;
}

/*
void HHKIND(int hid, int*kind, char*chopt) {
  hhkind_(&hid, kind, chopt, strlen(chopt));
    return;
};
*/

// ========================================
// Global variables
// ========================================

float pawc_[NWORDS_PAWC];

// ===================================
// Booking hbk files (starts from 10000)
// ===================================
// hid        : comments
//
//  10000     : bunch distribution (default)
//  10010     : bunch distribution carbon (time + -t cut)(default)
//  10020     : bunch distribution carbon (time cut)(default)
//  10030     : wall current monitor distribution (default)
//  10033     : specific luminosity
//  10034     : normalized specific luminosity
//  10035     : specific luminosity distribution (histogram)
//  10040     : energy distribution for A_N calculation (default)
//  10050     : energy distribution for A_N calculation -banana cut- (default)
//  10100+Si  : bunch distribution carbon for each Si (default)
//  10200     : delimiter distribution (default)
//  10300+Si  : strip distribution (default)
//  10310+Si  : strip distribution time cut (default)
//  10320+Si  : strip distribution time + -t cut (default)
//  10400+Si  : energy distribution (defualt)
//  10410+Si  : energy distribution carbon (defualt)
//  10450+Si  : -t cross section (amplitude) (defualt)
//  10460+Si  : -t cross section (integral)  (defualt)
//  10500+St  : energy dists for plus  bunch (each strip,default)
//  10600+St  : energy dists for minus bunch (each strip,default)
//  10700+i   : energy dists in given -t bin
//  11000+bid : bunch by bunch timing infor (default)
//  11200+St  : tdc distribution without cut
//-CMODE
//  12000+St  : energy calibration
//-TMODE
//  12100+St  : T0 determination
//-AMODE
//  12200+St  : pile up rejection parameters
//  12300+St  : Tof vs. Integral
//-BMODE
//  13000+St  : banana curve (E-T)
//  13100+St  : banana curve (ADC-TDC)
//  13200+St  : banana curve (ADC-TDCMAX)
//  13500+St  : lower curbon cut boundary
//  13600+St  : upper curbon cut boundary
//  14000+St  : bunch crossing
//-DMODE
//  15000+St  : Dead layer + T0 estimation
//  15100+St  : Dead layer + T0 estimation (banana with Mass cut)
//-MMODE
//  16000+St  : mass plot(1d)
//  16100+St  : mass plot vs. energy (2d)
//  16200+St  : mass plot(1d) excluding background (for feedback)
//  17200+St  : mass plot(1d) excluding background (for quality check)
//-FEEDBACKMODE
//  16300     : deviation from 12C mass peak [GeV] vs. strip
//-RAMPMODE
//  20000+i   : carbon timing at (600-650keV) along ramp (i=0,300)
//  21000+Si  : delimiter dist for UpSpin (Carbon cut)
//  21100+Si  : delimiter dist for DwSpin (Carbon cut)
//-TARGET ASSOCIATED
//  25050     : CDEV target position vs. time
//  25060     : target position (using stepping moter) vs. time
//-ASYMMETRY RESULTS
//  30000     : X90 phys(0), acpt(10), lumi(20)
//  30100     : X90 phys(0), acpt(10), lumi(20)
//  30200     : X90 phys(0), acpt(10), lumi(20)
//  30300     : X90 phys(0), acpt(10), lumi(20)
//  31000     : spinpattern
//  31010-60  : Si-1...Si-6 Nevents
//  31100     : RL90 (with errors)
//  31110     : RL45 (with errors)
//  31120     : BT45 (with errors)
//  32000+i   : -t bined X90 (i=1,6)
//  32100+i   : -t bined X45
//  32200+i   : -t bined Y45
//  32300+i   : -t bined C45
//  33000     : Good Events Hist
//  33010     : BackGround Events Hist
//  33020     : Total Events Hist
//  34000     : A_N (1bin)
//  35000+St  : fine -t dependece UpSpin for each strip
//  35100+St  : fine -t dependece DwSpin for each strip
//  36000     : strip dists for Up spin
//  36100     : strip dists for Down spin
//  36210     : raw asymmetry vs. strip#
//  36220     : raw asymmetry vs. phi
//  36240     : polarization vs. strip#
//  36250     : polarization vs. phi
//  37000+TgtIndex  : X90 phys(0), acpt(10), lumi(20)
//  37500+TgtIndex  : X45 phys(0), acpt(10), lumi(20)
//  38010     : Intensity Profile (RU(90))
//  38020     : Intensity Profile (RD(90))
//  38030     : Intensity Profile (LU(90))
//  38040     : Intensity Profile (LD(90))
//  38050     : Total Intensity Profile vs. Target Position
//  38060     : Total Intensity Profile vs. Time [s]
//-STUDY MODE
//  40000+Si  : 120 bunch dists for spin tune measurement


void hist_book(char *filename)
{
    //cout << " In function hist_hbook " <<endl;
    int st,si,i,bid;
    char hcomment[256];

    // PAW STUFF
    int  lun =1;
    int  reclen = RECLEN;
    int  icycle = 0;
    int  nwpaw = NWORDS_PAWC;
    char filecomment[100];
    char filestatus[5];

    hhlimit_(&nwpaw);

    sprintf(filecomment, "T-E INFO");
    sprintf(filestatus, "N");
    hhropen_(&lun, filecomment, filename, filestatus, &reclen, &icycle,
             strlen(filecomment), strlen(filename), strlen(filestatus));

    fprintf (stdout, "new hbook file : %s\n", filename);

    // WCM
    sprintf(hcomment,"WCM Distribution");
    HHBOOK1(10030,hcomment,120,-.5,119.5);
    sprintf(hcomment,"Specific Luminosity");
    HHBOOK1(10033,hcomment,120,-.5,119.5);
    sprintf(hcomment,"Normarized Specific Luminosity");
    HHBOOK1(10034,hcomment,120,-.5,119.5);

    //------------------------------------------------------------------//
    //                          DeadLayer Mode                          //
    //------------------------------------------------------------------//
    if (dproc.DMODE == 1) {
        for (st=1;st<=72;st++){
            sprintf(hcomment, "Tof vs Edep St - %d ", st);
            HHBOOK2(15000+st, hcomment, 100, 100., 1100.,  100, 0., 100.);
            HHBOOK2(15100+st, hcomment, 100, 100., 1100.,  100, 0., 100.);
        }
    }

    //------------------------------------------------------------------//
    //                           Nomal Process Mode                     //
    //------------------------------------------------------------------//

    // Bunch Distribution
    sprintf(hcomment,"Bunch Distribution");
    HHBOOK1(10000,hcomment,120,-.5,119.5);
    sprintf(hcomment,"Bunch Distribution (time + -t cut)");
    HHBOOK1(10010,hcomment,120,-.5,119.5);
    sprintf(hcomment,"Bunch Distribution (-t cut)");
    HHBOOK1(10020,hcomment,120,-.5,119.5);

    // Bunch Distribution for Each Si
    HHBOOK1(10110,"bunch distribution Si-1",120,-.5,119.5);
    HHBOOK1(10120,"bunch distribution Si-2",120,-.5,119.5);
    HHBOOK1(10130,"bunch distribution Si-3",120,-.5,119.5);
    HHBOOK1(10140,"bunch distribution Si-4",120,-.5,119.5);
    HHBOOK1(10150,"bunch distribution Si-5",120,-.5,119.5);
    HHBOOK1(10160,"bunch distribution Si-6",120,-.5,119.5);

    // delimeter distribution
    sprintf(hcomment,"Delimiter distribution");
    HHBOOK1(10200,hcomment,24000,0.,24000.);

    // Strip Distribution
    for (si=1;si<=6;si++){
        sprintf(hcomment,"strip dist Si - %d ",si);
        HHBOOK1(10300+si,hcomment, 12,0.5,12.5);
        sprintf(hcomment,"strip dist time cut Si - %d ",si);
        HHBOOK1(10310+si,hcomment, 12,0.5,12.5);
        sprintf(hcomment,"strip dist time + -t cut Si - %d ",si);
        HHBOOK1(10320+si,hcomment, 12,0.5,12.5);
    }

    // Energy Distribution

    HHBOOK1(10040,"For A_N Calc", HENEBIN, 0., HMAXENE);
    HHBOOK1(10050,"For A_N Calc (banana cut)", HENEBIN, 0., HMAXENE);

    for (si=1;si<=6;si++){
        sprintf(hcomment,"energy dist Si - %d ",si);
        HHBOOK1(10400+si,hcomment, HENEBIN,0.,HMAXENE);
        sprintf(hcomment,"energy dist carbon Si - %d ",si);
        HHBOOK1(10410+si,hcomment, HENEBIN,0.,HMAXENE);
        sprintf(hcomment,"energy dist carbon Si - %d ",si);
        HHBOOK1(10470+si,hcomment, HENEBIN,0.,HMAXENE*2.234e-5);
        sprintf(hcomment,"energy dist (integral) carbon Si - %d ",si);
        HHBOOK1(10420+si,hcomment, 50,0.,1500.);
        sprintf(hcomment," -t slope si - %d (amp)",si);
        HHBOOK1(10450+si,hcomment,200,0.,0.04);
        sprintf(hcomment," -t slope si - %d (int)",si);
        HHBOOK1(10460+si,hcomment,200,0.,0.04);
    }


    for (st=1;st<=NSTRIP;st++){
        sprintf(hcomment,"energy dist Strip %d (plus)",st);
        HHBOOK1(10500+st,hcomment,50,0.,1300.);
        sprintf(hcomment,"energy dist Strip %d (minus)",st);
        HHBOOK1(10600+st,hcomment,50,0.,1300.);
    }

    // timing shift in bunch by bunch
    for (bid=0;bid<NBUNCH;bid++){
        sprintf(hcomment,"banana 600-650keV bunch-%d",bid);
        HHBOOK1(11000+bid,hcomment, 40, 0., 80.);
    }

    // TDC DISTS without any cut
    for (st=1;st<=NSTRIP;st++){
        sprintf(hcomment,"TDC dist st-%d",st);
        HHBOOK1(11200+st,hcomment, 256, 0., 256.);
    }

    // Energy distribution in given -t bin
    for (int ibin=0; ibin<10; ibin++) {
        float Elow = 200. + ibin*100.;
        float Eupp = 300. + ibin*100.;

        sprintf(hcomment,"Energy dist %d-bin",ibin);
        HHBOOK1(10700+ibin+1,"",10,Elow,Eupp);
    }

    // Calibration hists
    if (dproc.CMODE == 1) {
        for (st=1;st<=72;st++){
            sprintf(hcomment,"241 Am (5.486MeV) - strip %d ",st);
            HHBOOK1(12000+st,hcomment,256,0.,256.);
        }
    }

    // for T0 (due to cable length) determination
    if (dproc.TMODE == 1) {
        if (dproc.ZMODE == 0) {
            for (st=1;st<=72;st++){
                sprintf(hcomment,"ST-%d abs t wo T0 subt vs. 1./sqrt(e)",st);
                HHBOOK2(12100+st,hcomment,40,0.,0.1,40,60.,140.);
            }
        } else {
            for (st=1;st<=72;st++){
                sprintf(hcomment,"ST-%d abs t wo T0 subt vs. 1./sqrt(e)",st);
                HHBOOK2(12100+st,hcomment,40,0.,0.1,40,-10.,70.);
            }
        }
    }

    // signal integral vs. amplitude A0,A1, and iasigma determination
    if (dproc.AMODE == 1){
        for (st=1;st<=72;st++){
            sprintf(hcomment,"strip %d -- Int vs. Amp ",st);
            HHBOOK2(12200+st,hcomment,100,0.,200., 256,0.,4000.);

            sprintf(hcomment,"strip %d -- Tof vs. Int",st);
            HHBOOK2(12300+st,hcomment,128,0.,4000.,80,20.,100.);
        }
    }

    // banana plots (E-T)
    if (dproc.BMODE == 1){
        for (st=1;st<=72;st++){
            sprintf(hcomment,"st %d - tof vs ene ",st);
            HHBOOK2(13000+st,hcomment, HENEBIN, 0., HMAXENE, 160, -0.,200.);

            sprintf(hcomment,"st %d - tof vs ene (alpha)",st);
            HHBOOK2(13700+st,hcomment, HENEBIN, 0., HMAXENE, 160, -0.,200.);
            sprintf(hcomment,"st %d - tof vs ene (alpha)",st);
            HHBOOK2(13800+st,hcomment, HENEBIN, 0., HMAXENE, 160, -0.,200.);

            sprintf(hcomment,"st %d - TDC vs ADC",st);
            HHBOOK2(13100+st,hcomment, 256, -.5, 255.5, 256, -.5, 255.5);
            sprintf(hcomment,"st %d - TDCMAX vs ADC",st);
            HHBOOK2(13200+st,hcomment, 256, -.5, 255.5, 128, -.5, 255.5);

            sprintf(hcomment,"st %d - lower cut",st);
            HHBOOK1(13500+st,hcomment, HENEBIN, 0., HMAXENE);
            sprintf(hcomment,"st %d - upper cut",st);
            HHBOOK1(13600+st,hcomment, HENEBIN, 0., HMAXENE);

            sprintf(hcomment,"st %d - bunch",st);
            HHBOOK1(14000+st,hcomment, 120, -.5, 119.5);

        }
    }

    // Mass plots

    // Feedback Mass Peak Results
    sprintf(hcomment,"Peak deviation from 12C Mass");
    HHBOOK1(16300,hcomment,72,-0.5, 71.5);

    if (dproc.MMODE == 1) {
        for (st=1;st<=72;st++){
            sprintf(hcomment,"Mass st %d ",st);
            HHBOOK1(16000+st,hcomment,90,0.,30.);
            HHBOOK1(16200+st,hcomment,90,0.,30.);   // for feedback
            HHBOOK1(17200+st,hcomment,90,0.,30.);   // for quality check

        }
    }

    // Target Histograms
    sprintf(hcomment,"Target channel entries");
    HHBOOK1(25000,hcomment,4,71.5,75.5);
    sprintf(hcomment,"Target position vs. Revolution # ");
    HHBOOK2(25010,hcomment, 100, 0., 5.e6, 1000, -5000, 5000);
    sprintf(hcomment,"Ch(73,75) entry vs. Revolution # ");
    HHBOOK2(25020,hcomment, 100, 0., 5.e6, 100, 0, 1e4);
    sprintf(hcomment,"Target position vs. time [s] ");
    HHBOOK2(25030,hcomment, 100, 0., 100., 1000, -5000, 5000);


    // For asymmetry results

    HHBOOK1(30000, "X90 PHYS", 120, -0.5, 119.5);
    HHBOOK1(30010, "X90 ACPT", 120, -0.5, 119.5);
    HHBOOK1(30020, "X90 LUMI", 120, -0.5, 119.5);

    HHBOOK1(30100, "X45 PHYS", 120, -0.5, 119.5);
    HHBOOK1(30110, "X45 ACPT", 120, -0.5, 119.5);
    HHBOOK1(30120, "X45 LUMI", 120, -0.5, 119.5);

    HHBOOK1(30200, "Y45 PHYS", 120, -0.5, 119.5);
    HHBOOK1(30210, "Y45 ACPT", 120, -0.5, 119.5);
    HHBOOK1(30220, "Y45 LUMI", 120, -0.5, 119.5);

    HHBOOK1(30300, "C45 PHYS", 120, -0.5, 119.5);
    HHBOOK1(30310, "C45 ACPT", 120, -0.5, 119.5);
    HHBOOK1(30320, "C45 LUMI", 120, -0.5, 119.5);

    HHBOOK1(31000, "spin pattern", 120, -0.5, 119.5);

    HHBOOK1(31010, "Si-1 Bunch Dist", 120, -0.5, 119.5);
    HHBOOK1(31020, "Si-2 Bunch Dist", 120, -0.5, 119.5);
    HHBOOK1(31030, "Si-3 Bunch Dist", 120, -0.5, 119.5);
    HHBOOK1(31040, "Si-4 Bunch Dist", 120, -0.5, 119.5);
    HHBOOK1(31050, "Si-5 Bunch Dist", 120, -0.5, 119.5);
    HHBOOK1(31060, "Si-6 Bunch Dist", 120, -0.5, 119.5);

    HHBOOK1(31100, "RL90", 120, -0.5, 119.5);
    HHBOOK1(31110, "RL45", 120, -0.5, 119.5);
    HHBOOK1(31120, "BT45", 120, -0.5, 119.5);

    for (int tbin=0; tbin<NTBIN; tbin++) { // runs for number of bunches
        HHBOOK1(32000+tbin+1, "-t binned X90", 120, -0.5, 119.5);
        HHBOOK1(32100+tbin+1, "-t binned X45", 120, -0.5, 119.5);
        HHBOOK1(32200+tbin+1, "-t binned Y45", 120, -0.5, 119.5);
        HHBOOK1(32300+tbin+1, "-t binned C45", 120, -0.5, 119.5);
    }

    HHBOOK1(33000, "Bid dists for Good Events", 120, -0.5, 119.5);
    HHBOOK1(33010, "Bid dists for Background Events", 120, -0.5, 119.5);
    HHBOOK1(33020, "Bid dists for Total Events", 120, -0.5, 119.5);
    HHBOOK1(34000, "A_N", 1, 0., 1.);

    // -t dependence
    for (int st=0;st<72;st++){
        sprintf(hcomment,"-t dist UpSpin for st-%d",st);
        HHBOOK1(35000+st,hcomment,11,-0.5,10.5);
        sprintf(hcomment,"-t dist DwSpin for st-%d",st);
        HHBOOK1(35100+st,hcomment,11,-0.5,10.5);
        sprintf(hcomment,"-t dist 0 Spin for st-%d",st);
        HHBOOK1(35200+st,hcomment,11,-0.5,10.5);
    }

    HHBOOK1(36000, "Strip Dist Up  ", 72, -0.5, 71.5);
    HHBOOK1(36010, "Luminosity Up", 72, -0.5, 71.5);
    HHBOOK1(36100, "Strip Dist Down", 72, -0.5, 71.5);
    HHBOOK1(36110, "Luminosity Down", 72, -0.5, 71.5);
    HHBOOK1(36200, "Luminosity Ratio", 72, -0.5, 71.5);

    // strip-by-strp based asymmetries and polarizations
    HHBOOK1(36210, "Raw Asymmetry vs. Strip#  ", 72, -0.5, 71.5);
    HHBOOK1(36220, "Raw Asymmetry/A_N vs. Strip phi  ", 3600, 0, 360);
    HHBOOK1(36240, "Polarization vs. Strip# ", 72, -0.5, 71.5);
    HHBOOK1(36250, "Strip phi vs. Strip# ", 72, -0.5, 71.5);


    // Target Position Dependent Polarization
    for (int i=0; i<MAXDELIM; i++){
      HHBOOK1(37000+i, "X90 PHYS", 120, -0.5, 119.5);
      HHBOOK1(37500+i, "X45 PHYS", 120, -0.5, 119.5);
    }

    int MAXTGT = 30;
    HHBOOK1(38010, "RU(90)", MAXTGT, -0.5, MAXTGT);
    HHBOOK1(38020, "RD(90)", MAXTGT, -0.5, MAXTGT);
    HHBOOK1(38030, "LU(90)", MAXTGT, -0.5, MAXTGT);
    HHBOOK1(38040, "RD(90)", MAXTGT, -0.5, MAXTGT);
    HHBOOK1(38050, "Intensity Profile (target)", MAXTGT, -0.5, MAXTGT);
    HHBOOK1(38060, "Intensity Profile (time)",   40, -0.5, 40);


    // Ntuple booking
    //    HBNT(1,"AT mode"," ");
    //HBNAME(1,"ATDATA",atdata.ia,
    //       "ADC:U:8,TDC:U:8,S:I,BID:U:7,DLIMIT:U,ST:U:7,ENE:R,TOF:R,POL:I");

    // =======================================
    // histogram during RAMP
    // =======================================
    if (dproc.RAMPMODE==1) {

        // Timing spectrum
        for (i=0;i<300;i++){
            sprintf(hcomment,"carbon %d-%d sec\n",i,i+1);
            HHBOOK1(20000+i,hcomment, 25, 40.,90.);
        }

        // Number of Carbons in Si-s
        for (int Si=0; Si<6; Si++) {
            sprintf(hcomment,"UpSpin Si-%d",Si+1);
            HHBOOK1(21000+Si, hcomment, RAMPTIME, 0., (float)RAMPTIME);
            sprintf(hcomment,"DwSpin Si-%d",Si+1);
            HHBOOK1(21100+Si, hcomment, RAMPTIME, 0., (float)RAMPTIME);
        }
    }

    // Study mode
    if (dproc.STUDYMODE == 1) {
        for (int Si=0; Si<6; Si++){
            sprintf(hcomment,"Spin Tune UpSpin Si-%d",Si+1);
            HHBOOK1(40000+Si, hcomment, 120, 0., 120.);
        }
    }
}


//
// Target Histograms
//
void tgtHistBook()
{
    float XMIN, XMAX;
    XMIN = XMAX = tgt.all.x[0];

    for (int i=0; i<ndelim; i++) {
        XMAX = tgt.all.x[i]>XMAX ? tgt.all.x[i] : XMAX ;
        XMIN = tgt.all.x[i]<XMIN ? tgt.all.x[i] : XMIN ;
    }

    char hcomment[256];
    float dX = XMAX - XMIN ? (XMAX - XMIN)*0.1 : 1;
    int XBIN = (int) (fabs(XMAX - XMIN + 2*dX) / dproc.target_count_mm);

    sprintf(hcomment,"Target position vs. time ");
    HHBOOK2(25050, hcomment, XBIN, -0.5, ndelim+0.5, ndelim+1, XMIN-dX, XMAX+dX);
  //HHBOOK2(25050, hcomment, 1000, -0.5, ndelim+0.5, 500,XMIN-dX, XMAX+dX);

    sprintf(hcomment,"Target position vs. time (tgt event)");
    HHBOOK2(25060, hcomment, 100, -0.5, ndelim+0.5, 100, XMIN-dX, XMAX+dX);

    // Fill target histo with x[mm] vs.time [s]
    for (int i=0; i<ndelim; i++) {
        HHF2(25050, (float)i, tgt.all.x[i], 1);
    }
}


// ==================
//   histgram close
// ==================
// Writing on disk is needed only at the last moment
void hist_close(char *filename)
{ //{{{
   // PAW STUFF
   //int lun =1;
   //int reclen = RECLEN;
   int  icycle = 0;
   char filecomment[100];
   char filestatus[5];
   char chopt[5];
   int  idout = 0;

   //fprintf(stdout,"\nClosing histgram file : %s \n", filename);
   //fprintf(stdout,"Writing Hbook file on Disk \n");

   sprintf(filecomment, "T-E INFO");
   sprintf(filestatus, "N");

   //hhropen_(&lun, filecomment, filename, filestatus, &reclen, &icycle,
   //         strlen(filecomment),strlen(filename),strlen(filestatus));

   sprintf(chopt, "T");

   hhrout_(&idout, &icycle, chopt, strlen(chopt));
   hhrend_(filecomment, strlen(filecomment));
} //}}}
