/* main program for analyzing emittance measurement data from RHIC polarimeters
 Ron Gill - Feb. 18, 2009
 Only analyze "out" going target movement - hout.
 Corrections for Run2010 Svirida & Alekseev
*/
#define _FILE_OFFSET_BITS 64	    // to handle >2Gb files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>

#include <TROOT.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TF1.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TStyle.h>
#include <TPave.h>
#include <TPad.h>
#include <TPaveLabel.h>
#include <TPaveStats.h>
#include <TText.h>
#include <TPavesText.h>

#include "rhicpol.h"
#include "rpoldata.h"
#include "emitdefn.h"

int checkSt(char *wchtarg, int strip);
void sendemit(char *devName);

struct SIPARSTRUCT {
    int idiv;		// divider
    float nsperchan;	// ns, WFD time unit, twice LSB of time measurement
    float emin;		// keV
    float trgmin;	// keV
    int ifine;
    float t0[96];	// t0
    float ecoef[96];	// keV/channel
    float edead[96];	// kev
    float tmin[96]; 	// ns
    int   mark[96];
} sipar;

//	Common /atdata/ (for n-tuple)
struct ATDATASTRUCT {
    int st;    	// strip number
    float e;    // keV
    float tof;  // ns, t0 subtracted
    long b;	// bunch number: 0-59
    long orbit;	// orbit (revolution) number
    float stp;  // target step
    float pos;  // target position
} atdata;

//	Common /evtcnt/ (for emittance)
struct EVCNTSTRUCT {
    float step;
    float position;
    float normEvt;
    float totEvt;
} evtcnt;

//	Common /rhic/
struct RHICSTRUCT {
    int fillpat[MAXBUNCH];
    int polpat[MAXBUNCH];
    int nbfill[MAXBUNCH];
} rhic;

//	Commons /poldat/ /beamdat/ /tgtdat1/ /tgtdat2/
polDataStruct poldat;
beamDataStruct beamdat;
targetDataStruct tgtdat1;
targetDataStruct tgtdat2;
emitDataStruct tocdev;

long long scalers[6]; // LecRoy 48-bit scalers
int sscal[288];

int main(int argc, char **argv)
{
    const char polName[4][20] = {"BLUE-1", "BLUE-2", "YELLOW-1", "YELLOW-2"};
    float data[1536];
    int i, j, k, n;
    int St, nRecEvt;
    int measTime = -1;
    int iSend = 0;	// don't send by default
    int nbunch = 0;     // number of filled bunches
    int tlen, recRing;
    long delimtr;
    long orbitNo, firstOrb, lastOrb;
    int stepCh, tarWfdCh, maxSiCh;
    int nStepCnt=0;
    int fitRing;
    
    long startHLin, startHRot, startVLin, startVRot;
    float chkEvt = 1E6;  
// for making orbit-step tables
    int *vStepOrb;
// target position read from CDEV
    long *targPtr;

    float sumPeak;
    float sumWidth;
    float cutEMIN;
    float cutEMAX;
    float cutTOFMIN;
    float cutTOFMAX;

    char InFile[256] = "";
    char RootFile[256] = "";
    char psFile[256] = "emit_default.ps";
    char devName[128] = "";
    char str1[100];
    char str2[100];
    char targID[20];
    
    FILE *fin;

// set memory for orbit arrays
    vStepOrb = (int *)malloc(MAXSTEPS*sizeof(int));

//	buffer to read the next record - static due to its size
    static union {
	recordHeaderStruct   header;
        recordBeginStruct    begin;
        recordDataStruct     data;
        recordWFDV8ArrayStruct wfd;
        recordEndStruct      end;
        recordConfigRhicStruct cfg;
        recordPolAdoStruct   pol;
        recordBeamAdoStruct  beam;
        recordTagAdoStruct   tag;
        recordScalersStruct  scal;
	recordHJetSwitchStruct jet;
        char buffer[BSIZE*sizeof(int)];
    } rec;

//	data structure pointers
    recordReadATStruct * ATPtr;
    recordReadWaveStruct * wavePtr;
    recordReadWave120Struct * wave120Ptr;
    recordALLModeStruct * ALLPtr;
    recordALLMode120Struct * ALL120Ptr;
    
    memset(&poldat, 0, sizeof(poldat));    
    memset(&rhic, 0, sizeof(rhic));    
    memset(&sscal, 0, sizeof(sscal));
    memset(&sipar, 0, sizeof(sipar));
    memset(&tocdev, 0, sizeof(tocdev));
    memset(&scalers, 0, sizeof(scalers));
    
    for (i=0; i<sizeof(targID); i++) targID[i] = '\0';
    RootFile[0] = '\0';

    // get arguments
    while ((i = getopt(argc, argv, "hd:f:o:p:")) != -1) 
	switch(i) {
	case 'f':
    	    strncpy(InFile, optarg, sizeof(InFile));
	    break;
	case 'o':
// Put ROOT file stuff here.
    	    strncpy(RootFile, optarg, sizeof(RootFile));
	    break;
	case 'p':
    	    strncpy(psFile, optarg, sizeof(psFile));
	    break;
	case 'd':
    	    strncpy(devName, optarg, sizeof(devName));
	    iSend = 1;
	    break;
	case 'h':
	default:
	    printf("Analyze emittance measurement data\n");
	    printf("Usage: ./emit [-h] [-d <devicename>] [-p <psfile>] -f <data file> -o <root output> \n");
	    return 1;
    }

    // open data file
    fin = fopen(InFile, "rb"); 
    if (fin == NULL) {
	 printf("EMIT-ERR : Must specify input file %s\n", InFile);
	 return -1;
    }

    if (strlen(RootFile) <= 0) {
	sprintf(RootFile,"%s.root",InFile);
    }
    TFile *rootOut = new TFile(RootFile, "RECREATE");

// loop over records, find target and orbit #
    firstOrb = -1;
    lastOrb = -1;
    for (;;) {
	i = fread(&rec.header, sizeof(recordHeaderStruct), 1, fin); 
	if (feof(fin)) break;
	if (i != 1) {
	    printf("EMIT-ERR : IO error on input file (head) %s.\n", strerror(errno));
	    poldat.statusS |= (STATUS_ERROR | ERR_INT);
	    break;
	}
	if (rec.header.len > BSIZE*sizeof(int)) {
	    poldat.statusS |= WARN_INT;
	    printf("EMIT-WARN : Very large record (%d).\n", rec.header.len);
	    i = fseek(fin, rec.header.len - sizeof(recordHeaderStruct), SEEK_CUR);
	    if (feof(fin)) break;
	    if (i != 0) {
		printf("EMIT-ERR : IO error on input file (seek) %s.\n", strerror(errno));
		poldat.statusS |= (STATUS_ERROR | ERR_INT);
		break;
	    }
	    continue;
	}
	i = fread(&rec.begin.version, rec.header.len - sizeof(recordHeaderStruct), 1, fin); 
	if (feof(fin)) {
	    poldat.statusS |= WARN_INT;
	    printf("EMIT-WARN : Unexpected end of file.\n");
	    break;
	}
	if (i != 1) {
	    printf("EMIT-ERR : IO error on input file (data) %s.\n", strerror(errno));
	    poldat.statusS |= (STATUS_ERROR | ERR_INT);
	    break;
	}
	recRing |= rec.header.type & (~(REC_TYPEMASK | REC_FROMMEMORY | REC_120BUNCH));
	switch (rec.header.type & REC_TYPEMASK) {

	case REC_BEGIN:
	    if (rec.begin.version < 20000) {
		printf("EMIT-ERR : DAQ versions < 2.0.0 are not supported\n");
		poldat.statusS |= (STATUS_ERROR | ERR_INT);
		exit(-2);
	    }
	    printf("EMIT-INFO : Begin of data set version=%d for ", rec.begin.version);
	    if (rec.header.type & REC_YELLOW) {
		printf("YELLOW");
		fitRing = 1;
	    }
	    else if (rec.header.type & REC_BLUE) {
		printf("BLUE");
		fitRing = 0;
	    } else {
		printf("unknown");
	    }
	    printf(" ring.\n%s : %s", rec.begin.comment, ctime(&rec.header.timestamp.time));
	    break;

        case REC_RHIC_CONF:
    	    tarWfdCh = rec.cfg.data.NumChannels-4; // first channel for target
    	    maxSiCh = rec.cfg.data.NumChannels-4;
    	    if (recRing && REC_BLUE) tarWfdCh += 2;	// 2010 - Motor channels are yellow-vert, yellow-hor, blue-vert, blue-hor
	    break;
	  
	case REC_READAT:
	    for (i = 0; i < rec.header.len - sizeof(recordHeaderStruct);) {
		ATPtr = (recordReadATStruct *) (&rec.data.rec[i]);
		St = ATPtr->subhead.siNum;
		delimtr = rec.header.timestamp.delim;
		nRecEvt = ATPtr->subhead.Events + 1;
		i += sizeof(subheadStruct) + nRecEvt*sizeof(ATStruct);
		if (i > rec.header.len - sizeof(recordHeaderStruct)) {
	    	    printf("Broken record %d (%d bytes). Last subhead: siNum=%d  Events=%d\n", rec.header.num, rec.header.len, St+1, nRecEvt);
	    	    break;
		}
		if (St == tarWfdCh || St == tarWfdCh+1) {	// we 'or' here vertical and horizontal...
		    for (j=0; j<nRecEvt; j++) {
	    		orbitNo = delimtr*512 + 2*ATPtr->data[j].rev +  ATPtr->data[j].rev0;
        		if (firstOrb < 0) firstOrb = orbitNo;
	    		lastOrb = orbitNo;
	    		vStepOrb[nStepCnt] = orbitNo;
	    		nStepCnt++;
	    	    }
		}
	    }
	    break;
	  
	case REC_PCTARGET:
	    i = (rec.header.len - sizeof(rec.header))/(4*sizeof(long));
	    targPtr = (long *)&rec.buffer[sizeof(rec.header)];
	    --targPtr;
	    startHLin = *++targPtr;
	    startHRot = *++targPtr;
	    startVLin = *++targPtr;
	    startVRot = *++targPtr; 
	    break;
	    
        case REC_POLADO:
    	    printf("Reading REC_POLADO\n");
	    memcpy(&poldat, &rec.pol.data, sizeof(poldat));
	    tlen = strlen(rec.pol.data.targetIdS);
	   
	    memcpy(targID, rec.pol.data.targetIdS, tlen*sizeof(char));
	    if (targID == NULL) {
		printf("EMIT-WARN : Target id not found in data file. Using vertical target information.\n");
		sprintf(targID,"Vert");		// assume vertical target
	    }
	   
	    stepCh = tarWfdCh;
	    if (strncmp(targID,"H",1) == 0) stepCh++;	// 2010 - Motor channels are yellow-vert, yellow-hor, blue-vert, blue-hor
	   
	    printf("Target is %s  Step Ch is %d\n",targID, stepCh);
	    printf("Beam Energy = %f\n",poldat.beamEnergyS);
	    // find measurement duration to determine max orbit number
	    measTime = rec.pol.data.stopTimeS-rec.pol.data.startTimeS;
	    if (measTime<=0 || measTime>MAXMTIME) {
		measTime = MAXMTIME;
		printf("EMIT-WARN : Measurement time not found or too large. Default value is %d sec.\n", measTime);
	    } else {
		printf("Measurement time: %d sec.\n",measTime);
	    }
	    fseek(fin, 0, SEEK_END);  // finished w/ first loop, goto eof
	    break;
	  
        default:	// skip other records
	    break;
        } 
    } 
    rewind(fin);  // go back to beginning of file
   
//***************************************************************************************************
    printf("Finished first data file pass.\n");
    printf("Emittance Scan - %s E=%7.2f GeV\n", devName, poldat.beamEnergyS);

    if (firstOrb < 0 || lastOrb <= 0 || lastOrb < firstOrb) {
	printf("EMIT-ERR : No target motion data were found.\n");
	poldat.statusS |= 0x00000040;  // W-NODATA no data to analyze
	if (iSend) sendemit(devName);
	return 1;
    } 
    cutEMIN = EMIN;
    cutEMAX = EMAX;
    cutTOFMIN = TOFMIN;
    cutTOFMAX = TOFMAX;

// Re-order the data by orbit number...
// First half of data - target moving out

// Make a histogram of of steps vs. revolution...
    float *xstep = (float *)malloc(nStepCnt*sizeof(float));
    float *yrev = (float *)malloc(nStepCnt*sizeof(float));

    TH1F *hsrev;
    hsrev = new TH1F("hsrev", "Revolution vs. Steps", nStepCnt-1, 0, nStepCnt-1);	
    hsrev->GetXaxis()->SetTitle("Step No.");
    hsrev->GetYaxis()->SetTitle("Revolution No.");	
    hsrev->GetXaxis()->CenterTitle();
    hsrev->GetYaxis()->CenterTitle();
	
    float derv;
    int midStepCnt = 0;
    int nderv = 0;
    int maxderv = 0;
	
// Find the maximum difference in adjacent revolution (orbit) numbers.
// Reverse direction starts after the maximum after midStepCnt steps.
// *** Skip the first 10 and last 10 steps to avoid spurious events. ***
    for (i=9; i<nStepCnt-10; i++) {
        xstep[i] = i;
        yrev[i] = vStepOrb[i];
        hsrev->SetBinContent(i, vStepOrb[i]);
        nderv = vStepOrb[i+1] - vStepOrb[i];
        if (nderv > maxderv) {
    	    maxderv = nderv;
    	    midStepCnt = i;
	}
    }
	
    int nbins = midStepCnt + 1;
    if (nbins > 200) nbins = 200;
    int delta = 3*midStepCnt/nbins;
    int nOrbs = (lastOrb - firstOrb)/2;
    int totOrbs = lastOrb - firstOrb + 1;
    int outOrb = vStepOrb[midStepCnt];

// The graph below was for testing purposes.
/*	
	TGraph *gst;
	TCanvas *cgraph;
	
	cgraph = new TCanvas();
	cgraph->SetFillColor(10);
	
	gst = new TGraph(nStepCnt-1, xstep, yrev);
	gst->GetXaxis()->SetTitle("Steps");
	gst->GetXaxis()->CenterTitle();
	gst->GetYaxis()->SetTitle("Revolution No.");
	gst->GetYaxis()->CenterTitle();
	
	gst->Draw("AL");
	cgraph->Print("graph.ps");
*/
    printf("First Orbit = %d Last Orbit = %d Number Orbits = %d Orbits at midpoint = %d\n",firstOrb,lastOrb,totOrbs,vStepOrb[midStepCnt]);
    printf("Number of steps = %d First step orbit = %d Last step orbit = %d Midpoint steps = %d\n",nStepCnt,vStepOrb[0],vStepOrb[nStepCnt-1],midStepCnt);
    
    int *stepOrb;
    stepOrb = (int *)malloc((lastOrb+1)*sizeof(int));
    memset(stepOrb, 0, sizeof(stepOrb));
    
    for (i=firstOrb; i<vStepOrb[midStepCnt]; i++) {
	for (j=0; j<=midStepCnt; j++) {
    	    if (vStepOrb[j] >= i) {
    	        stepOrb[i] = j;
    	        break;
    	    }
    	}
    }

    k = 2*(midStepCnt);
    for (i=vStepOrb[midStepCnt+1]; i<lastOrb; i++) {
        for (j=midStepCnt+1; j<nStepCnt; j++) {
	    if (vStepOrb[j] >= i) {
    	        n = k - j;
    	        if (n >= 0) stepOrb[i] = n;
    	        break;
    	    }
    	}
    }

// Set up histograms...
    TH1D *hsum;
    TH1D *hout;
    TH1D *hret;
    TH1D *hbunch[MAXBUNCH];

    hsum = new TH1D("hsum","Profile - All Bunches",nbins,1,midStepCnt-delta);
    hsum->GetXaxis()->SetTitle("Profile - All Bunches");
    hsum->GetXaxis()->CenterTitle();
    hsum->GetXaxis()->SetLabelSize(0.05);
    
    hout = new TH1D("hout","Profile - Moving Out",nbins,1,midStepCnt-delta);
    hout->GetXaxis()->SetTitle("Profile - Moving Out");
    hout->GetXaxis()->CenterTitle();
    hout->GetXaxis()->SetLabelSize(0.05);
    
    hret = new TH1D("hret","Profile - Moving Back",nbins,1,midStepCnt-delta);
    hret->GetXaxis()->SetTitle("Profile - Moving Back");
    hret->GetXaxis()->CenterTitle();
    hret->GetXaxis()->SetLabelSize(0.05);
    hret->SetLineColor(kMagenta);
  
    for (i=0; i<MAXBUNCH; i++) {
        sprintf(str1,"hbunch%03d",i+1);
        sprintf(str2,"Each Bunch %d",i+1);
        hbunch[i] = new TH1D(str1,str2,nbins,1,midStepCnt-delta);
    }

    // initialize counters
    evtcnt.step = 0;
    evtcnt.totEvt = 0;
    printf("Starting second pass on data file...\n");
    // read records for second time to get events
    for(;;) {
	i = fread(&rec.header, sizeof(recordHeaderStruct), 1, fin); 
	if (feof(fin)) break;
	if (i != 1) {
	    printf("EMIT-ERR : IO error on second loop of file (head) %s.\n", strerror(errno));
	    poldat.statusS |= (STATUS_ERROR | ERR_INT);
	    break;
	}
	if (rec.header.len > BSIZE*sizeof(int)) {
	    poldat.statusS |= WARN_INT;
	    printf("EMIT-WARN : Very large record (%d).\n", rec.header.len);
	    i = fseek(fin, rec.header.len - sizeof(recordHeaderStruct), SEEK_CUR);
	    if (feof(fin)) break;
	    if (i != 0) {
		printf("EMIT-ERR : IO error on input file (seek) %s.\n", strerror(errno));
		poldat.statusS |= (STATUS_ERROR | ERR_INT);
		break;
	    }
	    continue;
	}
	i = fread(&rec.begin.version, rec.header.len - sizeof(recordHeaderStruct), 1, fin); 
	if (feof(fin)) {
	    poldat.statusS |= WARN_INT;
	    printf("EMIT-WARN : Unexpected end of file.\n");
	    break;
	}
	if (i != 1) {
	    printf("EMIT-ERR : IO error on input file (data) %s.\n", strerror(errno));
	    poldat.statusS |= (STATUS_ERROR | ERR_INT);
	    break;
	}

	switch(rec.header.type & REC_TYPEMASK) {
	case REC_BEGIN:
	    if (rec.begin.version < 20000) {
		printf("EMIT-ERR : DAQ versions < 2.0.0 are not supported\n");
		poldat.statusS |= (STATUS_ERROR | ERR_INT);
		exit(-2);
	    }
	    printf("EMIT-INFO : Begin of data set version=%d for ", rec.begin.version);
	    if (rec.header.type & REC_YELLOW) {
		printf("YELLOW");
		fitRing = 1;
	    } else if (rec.header.type & REC_BLUE) {
		printf("BLUE");
    		fitRing = 0;
	    } else {
		printf("unknown");
	    }
	    printf(" ring.\n%s : %s", rec.begin.comment, ctime(&rec.header.timestamp.time));
	    break;
	  
	case REC_END:
	    printf("EMIT-INFO : End of data set at record %d : %s\n", rec.header.num, rec.end.comment);
	    break;
	    
	case REC_POLADO:
	    memcpy(&poldat, &rec.pol.data, sizeof(poldat));
	    break;
	  
	case REC_BEAMADO:
	    memcpy(&beamdat, &rec.beam.data, sizeof(beamdat));	    
	    for (i=0;i<MAXBUNCH;i++) {
		rhic.fillpat[i] = beamdat.measuredFillPatternM[3*i];
		rhic.polpat[i] = beamdat.polarizationFillPatternS[3*i];
	    }
	    printf("Fill pattern: ");
	    for (i=0;i<MAXBUNCH;i++) {
		printf("%c", (rhic.fillpat[i] != 0) ? '|' : '_');
		if (rhic.fillpat[i]!=0) nbunch++;
	    }
	    printf("\n");
	    break;
	  
	case REC_TAGADO:
	    memcpy(&tgtdat1, &rec.tag.data[0], sizeof(tgtdat1));	    
	    memcpy(&tgtdat2, &rec.tag.data[1], sizeof(tgtdat2));	    
	    break;
	  
	case REC_RHIC_CONF:
	    sipar.idiv = rec.cfg.data.CSR.split.iDiv;
	    sipar.ifine = rec.cfg.data.CSR.split.Mod2D;
	    sipar.emin = rec.cfg.data.Emin;
	    sipar.trgmin = rec.cfg.data.TrigMin;
	    sipar.nsperchan = rec.cfg.data.WFDTUnit;
	    k = 0;
	    for (i=0; i<rec.cfg.data.NumChannels; i++) {
		if (rec.cfg.data.chan[i].CamacN == 0) continue;
		sipar.t0[i] = rec.cfg.data.chan[i].t0;
		sipar.ecoef[i] = rec.cfg.data.chan[i].ecoef;
		sipar.edead[i] = rec.cfg.data.chan[i].edead;
		sipar.tmin[i] = rec.cfg.data.chan[i].Window.split.Beg * sipar.nsperchan;
		k++;
	    }
	    printf("Configuration OK : %d Si found.\n", k);
	    break;
	  
	case REC_READALL:
	    break;   // not currently used for emittance
	  
	case REC_READRAW:
	    break;
	  
	case REC_READSUB:
	    break; // not currently used for emittance
	  
	case REC_READAT:
	    for (i = 0; i < rec.header.len - sizeof(recordHeaderStruct);) {
		ATPtr = (recordReadATStruct *) (&rec.data.rec[i]);
		St = ATPtr->subhead.siNum;
		delimtr = rec.header.timestamp.delim;
		nRecEvt = ATPtr->subhead.Events + 1;
		i += sizeof(subheadStruct) + nRecEvt*sizeof(ATStruct);
		if (i > rec.header.len - sizeof(recordHeaderStruct)) {
		    printf("Broken record %d (%d bytes). Last subhead: siNum=%d  Events=%d\n", rec.header.num, rec.header.len, St+1, nRecEvt);
		    break;
		}
		if (checkSt(targID, St)) {
		    for (j = 0; j < nRecEvt; j++) {
			atdata.st = St+1;
			atdata.e = ATPtr->data[j].a * sipar.ecoef[St] + sipar.edead[St];
			atdata.tof = ATPtr->data[j].t * 0.5 * sipar.nsperchan - sipar.t0[St];
			atdata.b = ATPtr->data[j].b;
			atdata.orbit = delimtr*512 + 2*ATPtr->data[j].rev + ATPtr->data[j].rev0;
			if (St < maxSiCh && rhic.fillpat[atdata.b]) {    // data adc AND filled bunch
			    if (atdata.orbit < firstOrb || atdata.orbit > lastOrb) { // Outside target moving range
				continue;
			    } else {
// Crude cuts on the data (for now = 3/3/09)...
				if (atdata.e >= cutEMIN && atdata.e <= cutEMAX && atdata.tof >= cutTOFMIN && atdata.tof <= cutTOFMAX) {
            			    k = atdata.orbit;
            			    if (stepOrb[k] > 0) {
            				hsum->Fill(stepOrb[k]);
            				if (k < vStepOrb[midStepCnt]) { 
            				    hout->Fill(stepOrb[k]);
					    hbunch[atdata.b]->Fill(stepOrb[k]);
            				} else if (k >= vStepOrb[midStepCnt+1]) {
            				    hret->Fill(stepOrb[k]);
            				}
            			    }
				}
			    }   
        		    evtcnt.totEvt++;
			    if (evtcnt.totEvt == chkEvt) {
				printf("Finished %dM events \n", (int)(evtcnt.totEvt/1E6));
				chkEvt += 1E6;
			    }
			} // if (St < maxSiCh)...
		    } // for (j = 0; j < nRecEvt...
		} // if (checkSt(targID, St...
	    } // for (i = 0; i < rec.header.len...
	    break;
	  
	case REC_WFDV8SCAL:
	    break; // don't use scaler histos for emittance
	  
	case REC_SCALERS:
	    memcpy(&scalers, &rec.scal.data[0], sizeof(scalers));	    
	    break;
	  
	case REC_PCTARGET:
	    break;
	  
	case REC_WCMADO:
	    break;
	  
	default:	// skip any unknown records
	    printf("Unknown record %8.8X encountered in input data file\n", rec.header.type & REC_TYPEMASK);
	    break;
	} // switch 
    } // for(;;)... 

    if (fitRing == 0) {
	printf("Emittance scan fits for BLUE ring\n");
    } else {
	printf("Emittance scan fits for YELLOW ring\n");
    }
    printf("\nBeam energy = %7.2f GeV\n", poldat.beamEnergyS);
    printf("Target = %s\n", poldat.targetIdS);
    printf("Total event number from data: %8.0f\n", evtcnt.totEvt);

// fit histograms if events found
    if (nbunch <= 0 || evtcnt.totEvt < MINFITEVT) {
        printf("EMIT-ERR : No bunches or less than %d events found - No fits performed\n",MINFITEVT);
        poldat.statusS |= 0x00000020;  // W-NODATA no data to analyze
	if (iSend) sendemit(devName);
    } else {
        TH1D *hphisto;
        TH1D *hwhisto;
     
        gStyle->SetOptFit(0111);
        gStyle->SetEndErrorSize(0);
        gStyle->SetFrameBorderMode(0);

        float hpeak[nbunch],ehpeak[nbunch];
        float hwidth[nbunch], ehwidth[nbunch];
        float xbun[nbunch];
        float gstep[200], gcnts[200], gerr[200];
        float p1, p2, twidth, tlo;
        float chisq, mean, rms;
        float ePeak, eWidth;
        float hpmean, hprms, hwmean, hwrms;
        float hpmin, hpmax, hwmin, hwmax;
        int ndf, entries;
        int nbun = 0;
        int ringcolor = (fitRing == 0) ? kBlue : kRed; // kRed is the new kYellow
     
        TF1 *gfit = new TF1("gfit","gaus", 0, nOrbs);
        gfit->SetLineColor(8);		// 8 instead of kRed
        gfit->SetLineWidth(2);

// Set the errors for the histogram (hout, hret or hsum) that we are using...
	for (i=1; i<nbins; i++) {	// Bin 0 has underflows - ignore it
	    double binerr = hout->GetBinContent(i);
	    gcnts[i-1] = binerr;
	    gstep[i-1] = hout->GetBinCenter(i);
	    binerr = sqrt(binerr);
  	    gerr[i-1] = binerr;
	    hout->SetBinError(i, binerr);
	}

	TGraphErrors *gout = new TGraphErrors(nbins-1, gstep, gcnts, 0, gerr);
	gout->SetTitle("Profile - Moving Out");
	gout->GetXaxis()->SetTitle("Profile - Moving Out");
        gout->GetXaxis()->CenterTitle();
        gout->GetXaxis()->SetLabelSize(0.05);
        gout->SetMarkerStyle(20);
	gout->SetMarkerSize(0.5);
	gout->Fit(gfit);
	 
        sumPeak = gfit->GetParameter(1);
        twidth = gfit->GetParameter(2);
        ePeak = (float)gfit->GetParError(1);
        eWidth = (float)gfit->GetParError(2);
        eWidth *= MMPERSTEP;
        chisq = gfit->GetChisquare();
        ndf = gfit->GetNDF();
     
        mean = hout->GetMean();
        rms = hout->GetRMS();
        entries = (int)hout->GetEntries();
     
        tlo = sumPeak-twidth;
        if (tlo < 0) tlo = 0;

        hphisto = new TH1D("hphisto","Peak Distribution",100,tlo,sumPeak+twidth);
        hphisto->GetXaxis()->SetTitle("Step Number");
        hphisto->GetXaxis()->CenterTitle();
        hphisto->GetXaxis()->SetLabelSize(0.05);
        hphisto->GetYaxis()->SetLabelSize(0.05);
     
        sumWidth = MMPERSTEP * twidth;
        tlo = sumWidth-1;
        if (tlo < 0) tlo = 0;
        hwhisto = new TH1D("hwhisto","Width Distribution",100,tlo,sumWidth+1);
        hwhisto->GetXaxis()->SetTitle("Width (mm)");
        hwhisto->GetXaxis()->CenterTitle();
        hwhisto->GetXaxis()->SetLabelSize(0.05);
        hwhisto->GetYaxis()->SetLabelSize(0.05);

// Save values to be sent to cdev...
        tocdev.peak = sumPeak;
        tocdev.width = sumWidth;
        tocdev.relwidth = twidth/sumPeak;
        tocdev.peaktoEvt = sumPeak/(float)entries;
     
        for (i=0; i<MAXBUNCH; i++) {
    	    if (rhic.fillpat[i] != 0) {
    		hbunch[i]->Fit(gfit);
    		p1 = gfit->GetParameter(1);
    		p2 = gfit->GetParameter(2);

// Save values to be sent to cdev...
		k = 3*(i+1) - 2;
		tocdev.peakb[k] = p1;
		tocdev.widthb[k] = MMPERSTEP * p2;
		tocdev.relwidthb[k] = p2/p1;
		tocdev.peaktoEvtb[k] = p1/(float)hbunch[i]->GetEntries();
	   
    		p2 *= MMPERSTEP;
    		xbun[nbun] = i;
    		hpeak[nbun] = p1;
    		ehpeak[nbun] = gfit->GetParError(1);
    		hphisto->Fill(p1);
       
    		hwidth[nbun] = p2;
    		ehwidth[nbun] = MMPERSTEP * gfit->GetParError(2);
    		hwhisto->Fill(p2);
    		nbun++;
    	    }
    	}
// Set minimum amd maximum of plot at mean +/- 10 rms...
        hpmean = hphisto->GetMean();
        hprms = hphisto->GetRMS();
        hpmin = (float)100*floor((hpmean - 15.0*hprms)/100+1);
        hpmax = (float)100*floor((hpmean + 15.0*hprms)/100);
	if (hpmax - hpmin < 200) {
	    hpmin = hpmean - 100;
	    hpmax = hpmean + 100;
	}
        hphisto->GetXaxis()->SetRangeUser(hpmin, hpmax);

        hwmean = hwhisto->GetMean();
        hwrms = hwhisto->GetRMS();
// Adjust range (1000.*) since numbers are small. Otherwise always get zero...
        hwmin = (float)0.001*floor(1000.*(hwmean - 10.0*hwrms));
        hwmax = (float)0.001*floor(1000.*(hwmean + 10.0*hwrms));
	if (hwmax - hwmin < 0.5) {
	    hwmin = hwmean - 0.25;
	    hwmax = hwmean + 0.25;
	}
        hwhisto->GetXaxis()->SetRangeUser(hwmin, hwmax);

        TGraphErrors *gpeak = new TGraphErrors(nbun, xbun, hpeak, 0, ehpeak);
        gpeak->SetTitle("Peak Position");
        gpeak->SetMarkerColor(ringcolor);
        gpeak->SetMarkerStyle(20);
        gpeak->SetMarkerSize(0.8);
        gpeak->GetXaxis()->SetTitle("Peak vs Bunch");
        gpeak->GetXaxis()->CenterTitle();
        gpeak->GetXaxis()->SetLabelSize(0.05);
        gpeak->GetYaxis()->SetRangeUser(hpmin, hpmax);
//     gpeak->GetYaxis()->SetRangeUser(sumPeak-twidth, sumPeak+twidth);
	if (hpmax < 10000) gpeak->GetYaxis()->SetLabelSize(0.05);

        TGraphErrors *gwidth = new TGraphErrors(nbun, xbun, hwidth, 0, ehwidth);
        gwidth->SetTitle("Width (mm)");
        gwidth->SetMarkerColor(ringcolor);
        gwidth->SetMarkerStyle(20);
        gwidth->SetMarkerSize(0.8);
        gwidth->GetXaxis()->SetTitle("Width (mm) vs Bunch");
        gwidth->GetXaxis()->CenterTitle();
        gwidth->GetYaxis()->SetRangeUser(hwmin, hwmax);
//     gwidth->GetYaxis()->SetRangeUser(sumWidth-1, sumWidth+1);
        gwidth->GetXaxis()->SetLabelSize(0.05);
        gwidth->GetYaxis()->SetLabelSize(0.05);         

        TCanvas *c1 = new TCanvas("emitscan","Emittance Plots",425,550);
	c1->SetFillColor(kWhite);

	sprintf(str1,"Emittance Scan - %s %8.3f E =%7.2f GeV", 
	    polName[((int)(poldat.runIdS*10 - 10*((int) poldat.runIdS) + 0.01)) & 3], poldat.runIdS, poldat.beamEnergyS);
        TPaveLabel *lbl = new TPaveLabel(0.00, 0.96, 1.00, 1.00, str1);
        lbl->SetFillColor(kWhite);
        lbl->SetBorderSize(0);
        lbl->Draw();

        TPad *canv = new TPad("canv", " ", 0.00, 0.959, 1.00, 0.00);
        canv->SetFillColor(kWhite);
        canv->SetBorderSize(0);
        canv->Divide(2,3);
        canv->Draw();
    
        canv->cd(1);
        hphisto->SetFillColor(ringcolor);
        hphisto->Draw();
        canv->cd(2);
        gpeak->Draw("AP");

        canv->cd(3);
        hwhisto->SetFillColor(ringcolor);
        hwhisto->Draw();
        canv->cd(4);
        gwidth->Draw("AP");

        canv->cd(5);
	gout->Draw("AP");
        canv->Update();

	TPaveStats *st = (TPaveStats*)(gout->GetListOfFunctions()->FindObject("stats"));
	st->SetX1NDC(0);
	st->SetY1NDC(0);
	st->SetX2NDC(0);
	st->SetY2NDC(0);
     
        canv->cd(6);
	TPavesText *ttext = new TPavesText(0.00,0.80, 0.80,0.30);
	ttext->SetFillColor(kWhite);
	ttext->SetBorderSize(0);
	ttext->AddText(poldat.targetIdS);
	sprintf(str1,"Entries = %d", entries);
	ttext->AddText(str1);
	sprintf(str1,"Mean = %8.0f", mean);
	ttext->AddText(str1);
	sprintf(str1,"RMS = %8.2f", rms);
	ttext->AddText(str1);
	sprintf(str1,"#chi^{2} / NDF = %12.4e / %d", chisq, ndf);
	ttext->AddText(str1);
	sprintf(str1,"Peak = %8.1f #pm %8.1f (steps)", sumPeak, ePeak);
	ttext->AddText(str1);
 	sprintf(str1,"Width = %8.4f #pm %8.4f (mm)", sumWidth, eWidth);
	ttext->AddText(str1);
        ttext->SetTextSize(0.06);
        ttext->SetTextAlign(12);
        ttext->Draw();

// Print the entire TCanvas (c1) to a .ps file... 
        c1->Print(psFile);

// send fit results to CDEV
	if (iSend) sendemit(devName);

        rootOut->Write();
        rootOut->Close();
        fclose(fin);

        printf("Total event number from data: %8.0f\n", evtcnt.totEvt);
        return 0;
    } // if (nbunch <= 0 || evtcnt...
}

int checkSt(char *wchtarg, int strip)
{
    if(*wchtarg == 'H') {  // Horizontal
	if ((strip >= 12 && strip < 24) || (strip >= 48 && strip < 60)) {
    	    return 0;  // skip 90 deg detectors
	} else {
    	    return 1;
	}
    } else {       // Vertical - default
	return 1;    // allow all strips
    }
}
