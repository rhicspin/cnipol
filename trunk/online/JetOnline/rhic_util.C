// Event mode data analysis tool
//  file name :   rhic_util.C
// 
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

// ================== 
//   histgram close   
// ================== 
// Writing on disk is needed only at the last moment

int TAsym::RootClose(char *filename) {

    cout << " Closing histgram file : "<<filename<<endl;

    tfile->cd();
    tfile -> Write();
    tfile -> Close();
if(dproc.WAVE_OUT_MODE==1){
    tfileTestWave->cd();
    tfileTestWave->Write();
    tfileTestWave->Close();
}
if(dproc.RAW_EVENT_MODE==1){
    tfileTestRaw->cd();
    tfileTestRaw->Write();
    tfileTestRaw->Close();
}
if(dproc.JET_EVENT_MODE==1){
    tfileTestEvent->cd();
    tfileTestEvent->Write();
    tfileTestEvent->Close();
}

    return (0);
}


//===================================
// Kinetic variables raeding 
//===================================
int TAsym::ReadKin(char *filename){

    FILE *fp_kin;
    int i, strip;
    float temp1,temp2,temp3,temp4;
    char temp5[20];
    
    fprintf(stdout,"\nReading ... Kinetic parameter file : %s \n", filename);
    
    fp_kin = fopen(filename,"r");
    if (fp_kin == NULL) {
        perror ("failed to open kinetic variable file");
        exit(1);
    }
    for (i=0;i<72;i++) {
        dthick[i] = 0.;  kint0[i] = 0.;
    }
    
    for (i=0;i<48;i++) {
        fscanf(fp_kin, "%d %f %f %f %f %s", &strip, &temp1, &temp2, 
               &temp3, &temp4, &temp5);

        kint0[strip] = temp1;
        
        fprintf(stdout," Kinetic : %d T0= %f \n", strip,kint0[strip]);
    }
    fclose(fp_kin);


    // ===============================
    // Determine Dead Layer
    // define ring

    cout << "Determine Dead Layer according to ring :" << nring <<endl;
    if (nring ==0) {
        dthick[0] = 59.6;
        dthick[1] = 57.9;
        dthick[2] = 76.6;
        dthick[3] = 49.6;
        dthick[4] = 44.5;
        dthick[5] = 55.6;
    } else {
        dthick[0] = 71.5;
        dthick[1] = 56.4;
        dthick[2] = 57.2;
        dthick[3] = 55.5;
        dthick[4] = 55.5;
        dthick[5] = 58.9;
    }



    return(0);
}


//===================================
// Mass Width variables raeding 
//===================================
int TAsym::ReadMass(char *filename){

    FILE *fp_mass;
    float temp1,temp2;
    
    fprintf(stdout,"\nReading ... mass parameter file : %s \n", filename);
    
    fp_mass = fopen(filename,"r");
    if (fp_mass == NULL) {
        perror ("failed to open masss variable file");
        exit(1);
    }
    
    for (Int_t st=0;st<72;st++) {
        for (Int_t i=0;i<10;i++) {
            
            mcenter[st][i] = 0.;  
            msigma[st][i] = 0.;
        }
    }

    cout << "Mass Parameters "<<endl;
    for (Int_t st=0;st<72;st++) {
        //cerr << "St: "<<st;
        for (Int_t i=0;i<10;i++) {
            fscanf(fp_mass, "%f %f ", &temp1, &temp2) ;
            
            mcenter[st][i] = temp1;
            msigma[st][i]  = temp2;
            
            //fprintf(stderr,"%4.2f -- %4.2f",mcenter[st][i], msigma[st][i]);
            //if (i==4) {cerr << endl; }
            //if (i==9) {cerr << endl;}
        }
    }
    fclose(fp_mass);
    return(0);
}


//===================================
// Relative Mass center shift btw bunches
//===================================
int TAsym::ReadMassRelShift(char *filename){

    FILE *fp_relmass;
    int temp1,temp2;
    float temp3, temp4, temp5, temp6;
    
    fprintf(stdout,"\nReading ... relative mass shift : %s \n", filename);
    
    fp_relmass = fopen(filename,"r");
    if (fp_relmass == NULL) {
        perror ("failed to open relative mass shift file");
        exit(1);
    }

    float relsum[72];
    cout << "Relative Mass shift loading  ... "<<endl;
    for (Int_t st=0; st<72; st++) {

        for (Int_t bid=0; bid<55; bid++) {
            fscanf(fp_relmass, " %d %d %f %f %f %f", &temp1, &temp2, 
                   &temp3, &temp4, &temp5, &temp6);
            
            mrelcent[st][bid]  = temp3;
            relsum[st] += temp3;

            if (temp1 != st) {
                cout << "something wrong : strip" <<endl;
            }

            if (temp2 != bid) {
                cout << "something wrong : bunch" <<endl;
            }



        }

        // subtract the average
        for (Int_t bid=0; bid<55; bid++) {
            mrelcent[st][bid] -= relsum[st]/55.;
        }
    }

    fclose(fp_relmass);
    return(0);
}


//=========================================
// Calibration parameter
//=========================================
void TAsym::ReadT0Config(recordConfigRhicStruct *cfginfo){
    FILE *fp;
    int st;
    int strip;
    float t0,t0e;

    fprintf(stdout,"**********************************\n");
    fprintf(stdout,"******* T0 is overwritten ********\n");
    fprintf(stdout,"**********************************\n");

    fp = fopen(t0_conf,"r");
    if (fp == NULL) {
        perror ("failed to open T0 parameter file");
        exit(1);
    }
    for (st=0;st<48;st++){
        fscanf(fp,"%d %f %f",&strip,&t0,&t0e);
        fprintf(stdout,"strip %d from %f",strip,
                cfginfo->data.chan[strip-1].t0);
        fprintf(stdout," to %f\n",t0);
        cfginfo->data.chan[strip-1].t0=(float)t0;
    }
    fclose(fp);
}

//===========================================
// Integ-Amp correlation fit parameters
//===========================================
int TAsym::ReadIAParam(char *filename){
    FILE *fp;
    int st,strip;
    float param0,param1,param2;

    fp = fopen(filename,"r");
    if (fp == NULL) {
        perror ("failed to open IA parameter file");
        return(1);
    }
    for (st=0;st<72;st++){  // Initialization
        iapar0[st] = 1.;
        iapar1[st] = 1.;
    }
    for (st=0;st<48;st++){
        fscanf(fp,"%d %f %f %f",&strip,&param0,&param1,&param2);
        fprintf(stdout,"strip %d fit parameters %f %f\n",strip,
                param0, param1);
        iapar0[strip-1] = param0;
        iapar1[strip-1] = param1;
    }
    fclose(fp);
    return(0);
}


//=================================================
// Calibration parameter
//=================================================
void TAsym::ReadConfig(recordConfigRhicStruct *cfginfo){
    FILE *fp;
    int st,strip;
    float t0,acoef,edead,ecoef,A0,A1,iasigma;

    fprintf(stdout,"**********************************\n");
    fprintf(stdout,"** Configuration is overwritten **\n");
    fprintf(stdout,"**********************************\n");

    fp = fopen(conf_file,"r");
    fprintf(stdout,"Reading configuration info from : %s\n",conf_file);
    if (fp == NULL) {
        perror ("failed to open cfg parameter file");
        exit(1);
    }

    for (st=0;st<48;st++){
        fscanf(fp,"%d %f %f %f %f %f %f %f",
               &strip,&t0,&ecoef,&edead,&A0,&A1,&acoef,&iasigma);
        cfginfo->data.chan[strip-1].acoef=(float)acoef;
        cfginfo->data.chan[strip-1].edead=(float)edead;
        cfginfo->data.chan[strip-1].ecoef=(float)ecoef;
        cfginfo->data.chan[strip-1].t0=(float)t0;
        cfginfo->data.chan[strip-1].A0=(float)A0;
        cfginfo->data.chan[strip-1].A1=(float)A1;
    }
    fclose(fp);
}

// ================================================================
// Print Out Configuration information 
// ================================================================
int TAsym::PrintConfig(recordConfigRhicStruct *cfginfo){
    
    int ccutwu;
    int ccutwl;
    int st;

    fprintf(stdout,"========================================\n");
    fprintf(stdout,"===  RHIC Polarimeter Configuration  ===\n");
    fprintf(stdout,"========================================\n");

    if (dproc.CBANANA == 0) {
        ccutwl = (int)cfginfo->data.chan[3].ETCutW;
        ccutwu = (int)cfginfo->data.chan[3].ETCutW;
    } else {
        ccutwl = (int)dproc.widthl;
        ccutwu = (int)dproc.widthu;
    }
    fprintf (stdout,"Carbon cut width : (low) %d (up) %d nsec \n",
             ccutwl,ccutwu);
    
    for (st=0;st<cfginfo->data.NumChannels;st++){
        if (cfginfo->data.chan[st].t0==0.0) {
            fprintf(stdout,"   \n");
        } else {
            fprintf(stdout, 
                    "St%2d T0:%5.2f EA:%5.2f EC:%5.2f ED:%5.2f C-%d V-%d\n",
                    st+1, 
                    cfginfo->data.chan[st].t0,
                    cfginfo->data.chan[st].acoef,
                    cfginfo->data.chan[st].ecoef,
                    cfginfo->data.chan[st].edead,
                    cfginfo->data.chan[st].CamacN,
                    cfginfo->data.chan[st].VirtexN);
        }
    }
    
    cout << "Time Coefficent : " << cfginfo->data.WFDTUnit/2. << "ns/ch"<<endl;

    fprintf(stdout,"================================================\n");
    fprintf(stdout,"===  RHIC Polarimeter Configuration (END)    ===\n");
    fprintf(stdout,"================================================\n");
    
    return(0);
}


// =================== 
// square root formula 
// =================== 
// A-RightUp  B-LeftDown  C-RightDown  D-LeftUp
// elastic Carbons are scattered off more in Right for Up 
int TAsym::sqass(float A, float B, float C, float D, float *asym, float *easym) {

    float den;
    den = sqrt(A*B) + sqrt(C*D);
    if ((A*B==0.)&&(C*D==0.)) {
        *asym  = 0. ; *easym = 0.;
    } else {
        *asym  = (sqrt(A*B) - sqrt(C*D))/den;
        *easym = sqrt(A*B*(C+D) + C*D*(A+B))/den/den;
    }
    return(0);
}


// ==============================
// Dead Layer Correction
// ==============================
// Ekin = p0 + p1*Edep + p2*Edep^2 + p3*Edep^3 + p4*Edep^4
float TAsym::ekin(float edep, float dthick){

    const float cp0[4] = {-0.5174,0.4172,0.3610E-02,-0.1286E-05};
    const float cp1[4] = {1.0000,0.8703E-02,0.1252E-04,0.6948E-07};
    const float cp2[4] = {0.2990E-05,-0.7937E-05,-0.2219E-07,-0.2877E-09};
    const float cp3[4] = {-0.8258E-08,0.4031E-08,0.9673E-12,0.3661E-12};
    const float cp4[4] = {0.3652E-11,-0.8652E-12,0.4059E-14,-0.1294E-15};

    float pp[5];
    int i;

    pp[0] = cp0[0]+cp0[1]*dthick+cp0[2]*pow(dthick,2)+cp0[3]*pow(dthick,3);
    pp[1] = cp1[0]+cp1[1]*dthick+cp1[2]*pow(dthick,2)+cp1[3]*pow(dthick,3);
    pp[2] = cp2[0]+cp2[1]*dthick+cp2[2]*pow(dthick,2)+cp2[3]*pow(dthick,3);
    pp[3] = cp3[0]+cp3[1]*dthick+cp3[2]*pow(dthick,2)+cp3[3]*pow(dthick,3);
    pp[4] = cp4[0]+cp4[1]*dthick+cp4[2]*pow(dthick,2)+cp4[3]*pow(dthick,3);
    
    return(pp[0] + pp[1]*edep + pp[2]*pow(edep,2) 
        + pp[3]*pow(edep,3) + pp[4]*pow(edep,4));
    
}




