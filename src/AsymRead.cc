//  Asymmetry Analysis of RHIC pC Polarimeter
//  file name :   AsymRead.cc
// 
// 
//  Author    :   Itaru Nakagawa
//  Creation  :   10/17/2005
//                

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>
#include "rhicpol.h"
#include "rpoldata.h"
#include "Asym.h"

#define REC_BEAMMASK 0x00030000


// ================= 
// read loop routine 
// ================= 
int readloop() {
  
  
  long int * pointer ;
    int rval;
    int nev;
    int j;
    long int Nevent, i;
    int flag;     // exit from while when flag==1
    int THINOUT = Flag.feedback ? dproc.thinout : Nskip;

    static int READ_FLAG=0;

    // Common structure for the data format
    static union {
        recordHeaderStruct     header;
        recordBeginStruct      begin;
        recordPolAdoStruct     polado;
        recordTagAdoStruct     tagado;
        recordBeamAdoStruct    beamado;
        recordConfigRhicStruct cfg;
        recordReadWaveStruct   all;
        recordReadWave120Struct all120;
        recordReadATStruct      at;
        recordWFDV8ArrayStruct  wfd;
        recordEndStruct        end;
        recordScalersStruct    scal;
        recordWcmAdoStruct     wcmado;
        char                   buffer[BSIZE*sizeof(int)];
        recordDataStruct       data;
    } rec;
    
    recordConfigRhicStruct  *cfginfo;
    beamDataStruct beamdat;
    targetDataStruct tgtdat1;
    targetDataStruct tgtdat2;
    wcmDataStruct wcmdat;
    pCTargetStruct pctdat;
    int bid,pat;
    processEvent event;
    char pattern[1];
    int nreadsi;  // number of Si already read
    int nreadbyte; 

    FILE *fp;

    // -------------------------------------------------
    flag = 0;
    
    // reading the data till its end ...
    if ((fp = fopen(datafile,"r")) == NULL) {
      printf("ERROR: %s file not found. Fource exit.\n",datafile);;
      exit(-1);
    } 


    while (flag==0) {
        if (fread(&rec.header, sizeof(recordHeaderStruct), 1, fp)!=1){
            break;
        }

        if (feof(fp)) {
            fprintf(stdout,"Expected end of file\n");
            break;
        }
        
        if (rec.header.len > BSIZE*sizeof(int)){
            fprintf(stdout,"Not enough buffer d: %d byte b: %d byte\n",
                    rec.header.len, sizeof(rec));
            break;
        }

        // Read rest of the structure
        rval = fread(&rec.begin.version, rec.header.len - 
                     sizeof(recordHeaderStruct),1,fp);
        if (feof(fp)) {
            perror("Unexpected end of file");
            exit(1);
            break;
        }
        
        if (rval != 1) {
            fprintf(stdout,"IO error in stream (header) %s\n",strerror(errno));
            continue;  
        }

	// Distinguish between blue & yellow beam
        switch (rec.header.type & REC_BEAMMASK) {
	case REC_YELLOW:
	  runinfo.RHICBeam=1;
	  break;
	case REC_BLUE:
	  runinfo.RHICBeam=0;
	  break;
	}


	//=============================================================
	//                          Main Switch
	//=============================================================

        switch (rec.header.type & REC_TYPEMASK) {
        case REC_BEGIN:
	  if (!ReadFlag.RECBEGIN){
            cout << "Begin of data stream Ver: " <<rec.begin.version<<endl;
            cout << "Comment: " << rec.begin.comment <<endl;
            cout << "Time: " << ctime(&rec.begin.header.timestamp.time) 
                 <<endl;
	    runinfo.StartTime=rec.begin.header.timestamp.time ;
	    
	    ReadFlag.RECBEGIN = 1;
	  }
	  break;

        case REC_POLADO:
            break; // later

        case REC_TAGADO:
	    memcpy(&tgtdat1, &rec.tagado.data[0], sizeof(tgtdat1));	    
	    memcpy(&tgtdat2, &rec.tagado.data[1], sizeof(tgtdat2));	    
            break; // later

        case REC_PCTARGET:
	  if (!ReadFlag.PCTARGET){
	    ndelim  = (rec.header.len - sizeof(rec.header))/(4*sizeof(long));
            pointer = (long *)&rec.buffer[sizeof(rec.header)] ;
            --pointer;  i=0;
            for (int k=0; k<ndelim ; k++) {
                VtgtLinear[k] = *++pointer;
                VtgtRotary[k] = *++pointer ;
                HtgtLinear[k] = *++pointer ;
                HtgtRotary[k] = *++pointer ;

		// force 0 for +/-1 tiny readout as target position. 
		if (abs(HtgtRotary[k])<=1) HtgtRotary[k]=0;
		if (k==0) {
		  printf("@%8d%8d%8d%8d\n", i, k, HtgtRotary[k], VtgtRotary[k]);
		} else {
		  TgtIndex[k] = i;
		  if ((HtgtRotary[k] != HtgtRotary[k-1])||(VtgtRotary[k] != VtgtRotary[k-1])) {
		    TgtIndex[k] = ++i ;
		    ++nTgtIndex;
		  printf("@%8d%8d%8d%8d\n", i, k, HtgtRotary[k], VtgtRotary[k]);
		  }
		}							  
            }
	    printf("Number of Delimiters :%4d\n",ndelim);
	    ReadFlag.PCTARGET = 1;
	  }
	  break;


        case REC_WCMADO:
	  if (!ReadFlag.WCMADO){
            fprintf(stdout,"Reading WCM information\n");
            memcpy(&wcmdat, &rec.wcmado.data,sizeof(wcmdat));
            for (bid=0;bid<120;bid++){
                wcmdist[bid] = wcmdat.fillDataM[bid*3];
		Nwcmtot+=wcmdist[bid]*fillpat[bid];
            }
	    runinfo.WcmAve  =  Nwcmtot/float(NFilledBunch);
	    ReadFlag.WCMADO = 1;
	  }
	  break;

        case REC_BEAMADO:
	  if (!ReadFlag.BEAMADO){
            fprintf(stdout,"Reading Beam Information\n");
            memcpy(&beamdat, &rec.beamado.data, sizeof(beamdat));
            runinfo.BeamEnergy = beamdat.beamEnergyM;
	    fprintf(stdout,"Beam Energy : %8.2f\n",runinfo.BeamEnergy);
	    fprintf(stdout,"RHIC Beam : %1d\n",runinfo.RHICBeam);

	    // Add 7 ns for injection measurements
	    if (runinfo.BeamEnergy < 30) dproc.tshift+=7;

            for (bid=0;bid<120;bid++) {
                pat = beamdat.polarizationFillPatternS[bid*3];
                if (pat>0) {
                    spinpat[bid] = 1;
                } else if (pat<0) {
                    spinpat[bid] = -1;
                } else {
                    spinpat[bid] = 0;
                }
                pat = beamdat.measuredFillPatternM[bid*3];
                if (pat>0) {
                    fillpat[bid] = 1;
		    NFilledBunch++;
                } else if (pat<0) {
                    fillpat[bid] = -1;
		    NFilledBunch++;
                } else {
                    fillpat[bid] = 0;
                }
            }
	    //Impremented to recover spin pattern screwed data (Nov.15,'05) 
	    //FlipSpinPattern(1);

            cout << "Spin Pattern Used : " << endl;
            char pattern[3][5] ;
            sprintf(pattern[0],"-");
            sprintf(pattern[1],".");
            sprintf(pattern[2],"+");

            for (bid=0;bid<120;bid++) {
                if (bid%10 == 0) cout << " ";
                cout << pattern[spinpat[bid]+1];
            }
            cout <<endl;
            cout << "Fill Pattern Used : " << endl;
            for (bid=0;bid<120;bid++) {
                if (bid%10 == 0) cout << " ";
                cout << pattern[fillpat[bid]+1];
            }
            cout<<endl;

	    ReadFlag.BEAMADO = 1;
	  }
	  break; 

        case REC_END:
	  if (!Flag.feedback){
            fprintf(stdout,"End of data stream \n");
            fprintf(stdout,"End Time: %s\n", 
                    ctime(&rec.end.header.timestamp.time));
            fprintf(stdout,"%ld records found\n",Nread);
            fprintf(stdout,"Total number of counts \n");
            fprintf(stdout,"%ld Carbons are found in\n",Nevcut);
            fprintf(stdout,"%ld Total events of intended fill pattern\n",
                    Nevtot);
            fprintf(stdout,"Data Comment: %s\n", rec.end.comment);
	    runinfo.StopTime=rec.end.header.timestamp.time;
	  }
	  if (end_process(cfginfo)!=0) {
	    fprintf(stdout, "Error at end process \n");
	  }
	  break;

        case REC_READRAW:
            break; // later

        case REC_READSUB:
            break; // later

        case REC_READALL:
            break; // later

        case REC_WFDV8SCAL:
            break; // later

        case REC_SCALERS:
	  if (!Flag.feedback){
            fprintf(stdout,"Scaler readtime \n");
            fprintf(stdout,"Scaler End Time: %s\n", 
                    ctime(&rec.scal.header.timestamp.time));
	  }
	  break; 

        case REC_READAT:
	  // Display configuration just once
	  if (!READ_FLAG){
            if (printConfig(cfginfo)!=0) {
                perror(" error in printing configuration");
                exit(1); 
	    }
	    READ_FLAG=1;
	  }


            event.delim = rec.header.timestamp.delim;
            nreadsi = 0;  // number of Si already read
            nreadbyte = 0;  // number of bite already read
            
            recordReadATStruct *ATPtr;
            
            for (int i=0; i< rec.header.len - sizeof(recordHeaderStruct);) {
                
                ATPtr = (recordReadATStruct *) (&rec.data.rec[i]);
                event.stN = ATPtr -> subhead.siNum; // si number
                Nevent = ATPtr -> subhead.Events + 1; 
                i += sizeof(subheadStruct) + Nevent*sizeof(ATStruct);
                if (i > rec.header.len - sizeof(recordHeaderStruct)) {
                    cout << "Broken record "<< rec.header.num << "(" 
                         << rec.header.len << " bytes). Last subhead: siNum= "
                         << event.stN << " Events= " << Nevent << endl; 
                    break;
                }
                
                for (j=0; j<Nevent; j++) {
                    Nread++;
                    if (fmod(float(Nread),float(THINOUT))==0) {

                        event.amp  = ATPtr -> data[j].a;
                        event.tdc  = ATPtr -> data[j].t; 
                        event.intg = ATPtr -> data[j].s;
                        event.bid  = ATPtr -> data[j].b;
                        event.tdcmax = ATPtr -> data[j].tmax; 
                        event.rev0 = ATPtr -> data[j].rev0;
                        event.rev = ATPtr -> data[j].rev;
                        cntr.revolusion=event.delim*512 + event.rev*2 + event.rev0 ;
                        /*
                        cout << " i " <<i
                          << " Nevent " << Nevent
                          << " St " << event.stN  
                          << " amp " << event.amp 
                          << " tdc " << event.tdc 
                          << " bid " << event.bid 
			     << " rev0 " << event.rev0 
			     << " rev  " << event.rev 
                             << " revolusion #=" << cntr.revolusion << endl;
                        */

			if ((fillpat[event.bid]==1)||(dproc.CMODE==1)) {
			     // Event Processing
                            if (event_process(&event,cfginfo)!=0) {
                                fprintf(stdout, 
                                        "Error event process Si:%d Ev:%d\n",
                                        nreadsi,j);
                            }
			}
                    }

		    //cout << event.delim*512 + event.rev*2 + event.rev0 << endl;

                    if (Nread%1000000==0) {
		      if (Flag.feedback){
			printf("Feedback Mode Ncounts = %ld \r", Nread) ;
		      }else{
			printf("Proccesing  Ncounts   = %ld \r", Nread);
		      }
                        fflush(stdout);
                    }
                }
            }
            break;
            
        case REC_RHIC_CONF:
	  if (!ReadFlag.RHICCONF){
            fprintf(stdout,"Read configure information\n");
            cfginfo = (recordConfigRhicStruct *)
                malloc(sizeof(recordConfigRhicStruct)+
                       (rec.cfg.data.NumChannels-1)*sizeof(SiChanStruct));

            memcpy(cfginfo, &rec.cfg, sizeof(recordConfigRhicStruct)+
                   (rec.cfg.data.NumChannels-1)*sizeof(SiChanStruct));
            // when we mandatory provide cfg info  
            if (dproc.RECONFMODE==1) {
                reConfig(cfginfo);
            }

	    // calculate Run Constants
	    calcRunConst(cfginfo);

            if (dproc.MESSAGE == 1) exit(0);
	    ReadFlag.RHICCONF = 1;
	  }
	  break;

        default:    // unknown record
            fprintf(stdout,"Encountered Unknown Record 0x%x\n",
                    rec.header.type & REC_TYPEMASK);
            break;
        }
    }
    fprintf(stdout,"End of loop\n");
    return(0);
}



//
// Class name  :
// Method name : calcRunConst
//
// Description : caluclate Run Constants
// Input       : recordConfigRhicStruct *cfginfo
// Return      : 
//             : 
//
void
calcRunConst(recordConfigRhicStruct *cfginfo){

  // Determine the TDC count unit (ns/channel)
  runconst.Ct = cfginfo->data.WFDTUnit/2.;

  // ToF Distance [cm]
  float L = cfginfo->data.TOFLength;

  // C12 Mass
  float m = MASS_12C;

  // speed of light
  float c = C_CMNS;


  // Calculate kinematic constants for the Run
  runconst.E2T = M_SQRT1_2/c * sqrt(m) * L;
  runconst.M2T = L/2/M_SQRT2/c/sqrt(m);         // [ns/keV^(1/2)]
  runconst.T2M = 2*c*c/L/L;

  printf("Kinematic Const. E2T =%10.2f\n",runconst.E2T);

  return;
}





