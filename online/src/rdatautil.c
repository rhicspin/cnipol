#define _FILE_OFFSET_BITS 64	    // to handle >2Gb files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define f2cFortran
#include <cfortran.h>
#include <hbook.h>
#include <errno.h>
#include "rhicpol.h"
#include "rpoldata.h"

FILE *fin;

struct {
    int idiv;		// divider
    float nsperchan;	// ns, WFD time unit, twice LSB of time measurement
    float emin;		// keV
    float trgmin;	// keV
    int ifine;
    float ecoef[96];	// keV/channel
    float edead[96];	// kev
    float tmin[96]; 	// ns
    int   mark[96];
} sipar_;

struct {
    int intp;
    int iraw;
} runpars_;

void mybook_(int *num);
void histrate_(long *data, long *num);
void histdelim_(long *data, long *num);
void histtagmov_(long *data, long *num);

//	Common /atdata/ (for n-tuple)
struct {
    float a;	// keV
    float t;	// ns
    float tmax; // ns
    float s;	// keV*ns
    long b;	// bunch number: 0-59
    long d; 	// delimiter value
    long rev;	// revolution number
    long jet;	// jet polarization (0-3)
} atdata_;

//	Common /atraw/ (for n-tuple)
struct {
    long ia;
    long it;
    long itmax;
    long is;
    long ib;	// bunch number: 0-59
    long id; 	// delimiter value
    long irev;	// revolution number
    long ijet;	// jet polarization (0-3)
} atraw_;

struct {
    int iquest[100];
} quest_;

//	Common /rhic/
struct {
    int fillpat[120];
    int polpat[120];
} rhic_;
//	Commons /poldat/ /beamdat/ /tgtdat1/ /tgtdat2/
polDataStruct poldat_;
beamDataStruct beamdat_;
targetDataStruct tgtdat1_;
targetDataStruct tgtdat2_;
long long scalers_[6]; // LecRoy 48-bit scalers

int sscal_[288];

int icopen_(char *fname, int *len, int dummy)
{
    char *name;
    name = (char *)malloc(*len+1);
    memcpy(name, fname, *len);
    name[*len] = '\0';
    fin = fopen(name, "rb");
    if (fin == 0) {
	printf("RH2HBOOK-ERR : Can not open file %s : %s.\n", name, strerror(errno));
	poldat_.statusS |= (STATUS_ERROR | ERR_INT);
	free(name);
	return -1;
    }
    free(name);
    return 0;
}

void icclose_(void)
{
    close(fin);
}

int readandfill_(int* subrun)
{
//	buffer to read the next record - static due to its size
    static union {
	recordHeaderStruct   header;
	recordBeginStruct    begin;
	recordSubrunStruct   subrun;
        recordDataStruct     data;
        recordWFDV8ArrayStruct wfd;
        recordEndStruct      end;
        recordConfigRhicStruct cfg;
        recordPolAdoStruct   pol;
        recordBeamAdoStruct  beam;
        recordTagAdoStruct   tag;
        recordScalersStruct  scal;
	recordHJetSwitchStruct jet;
        char                 buffer[BSIZE*sizeof(int)];
    } rec;
//	data structure pointers
    recordReadATStruct * ATPtr;
    recordReadWaveStruct * wavePtr;
    recordReadWave120Struct * wave120Ptr;
    recordALLModeStruct * ALLPtr;
    recordALLMode120Struct * ALL120Ptr;
    
    float data[1536];
    char waveform[100];
    int i, j, k, l, m, n, cnt=0, cnt1=0;
    int recRing;
    long s1, s2, s3, s4;
    int curSubrun = -1;
    
    memset(&poldat_, 0, sizeof(poldat_));    
    memset(&rhic_, 0, sizeof(rhic_));    
    memset(&sscal_, 0, sizeof(sscal_));
    memset(&sipar_, 0, sizeof(sipar_));
    memset(&scalers_, 0, sizeof(scalers_));

    for(;;) {
	i = fread(&rec.header, sizeof(recordHeaderStruct), 1, fin); 
	if (feof(fin)) break;
	if (i != 1) {
	    printf("R2HBOOK-ERR : IO error on input file (head) %s.\n", strerror(errno));
	    poldat_.statusS |= (STATUS_ERROR | ERR_INT);
	    break;
	}
	if (rec.header.len > BSIZE*sizeof(int)) {
	    poldat_.statusS |= WARN_INT;
	    printf("R2HBOOK-WARN : Very large record (%d).\n", rec.header.len);
	    i = fseek(fin, rec.header.len - sizeof(recordHeaderStruct), 
		SEEK_CUR);
	    if (feof(fin)) break;
	    if (i != 0) {
		printf("R2HBOOK-ERR : IO error on input file (seek) %s.\n", strerror(errno));
		poldat_.statusS |= (STATUS_ERROR | ERR_INT);
		break;
	    }
	    continue;
	}
	i = fread(&rec.begin.version, rec.header.len - sizeof(recordHeaderStruct), 1, fin); 
	if (feof(fin)) {
	    poldat_.statusS |= WARN_INT;
	    printf("R2HBOOK-WARN : Unexpected end of file.\n");
	    break;
	}
	if (i != 1) {
	    printf("R2HBOOK-ERR : IO error on input file (data) %s.\n", strerror(errno));
	    poldat_.statusS |= (STATUS_ERROR | ERR_INT);
	    break;
	}
	recRing |= rec.header.type & (~(REC_TYPEMASK | REC_FROMMEMORY | REC_120BUNCH));
//	printf("Record %5d type %8.8X  len %8d timestamp %8.8X\n", 
//	    rec.header.num, rec.header.type, rec.header.len, rec.header.timestamp.time);
	if ((rec.header.type & REC_TYPEMASK) == REC_SUBRUN) {
	    curSubrun = rec.subrun.subrun;
	    if (curSubrun == *subrun) printf("Processing subrun %d\n", curSubrun);
	}
	if ((curSubrun >= 0) && (curSubrun != *subrun)) continue;
	switch(rec.header.type & REC_TYPEMASK) {
	case REC_BEGIN:
	    if (rec.begin.version < 20000) {
		printf("R2HBOOK-ERR : DAQ versions < 2.0.0 are not supported\n");
		poldat_.statusS |= (STATUS_ERROR | ERR_INT);
		exit(-2);
	    }
	    printf("R2HBOOK-INFO : Begin of data set version=%d for ", rec.begin.version);
	    if (rec.header.type & REC_YELLOW) {
		printf("YELLOW");
	    } else if (rec.header.type & REC_BLUE) {
		printf("BLUE");
	    } else printf("unknown");
	    printf(" ring.\n%s : %s", rec.begin.comment,
		ctime(&rec.header.timestamp.time));
	    break;
	case REC_SUBRUN:
	    break;
	case REC_END:
	    printf("R2HBOOK-INFO : End of data set at record %d : %s\n", 
		rec.header.num, rec.end.comment);
	    break;
	case REC_POLADO:
	    memcpy(&poldat_, &rec.pol.data, sizeof(poldat_));
	    break; 	
	case REC_BEAMADO:
	    memcpy(&beamdat_, &rec.beam.data, sizeof(beamdat_));	    
	    for (i=0;i<120;i++) {
		rhic_.fillpat[i] = beamdat_.measuredFillPatternM[3*i];
		rhic_.polpat[i] = beamdat_.polarizationFillPatternS[3*i];
	    }
	    printf("Fill pattern: ");
	    for (i=0;i<120;i++) {
		printf("%c",(rhic_.fillpat[i]!=0)?'|':'_');
	    }
	    printf("\n");
	    printf("Pol. pattern: ");
	    for (i=0;i<120;i++) {
		char cc;
		if (rhic_.polpat[i] > 0) cc='+';
		else {
		    if (rhic_.polpat[i] < 0) cc='-';
		    else cc='0';
		}
		printf("%c", cc);
	    }
	    printf("\n");
	    break;
	case REC_TAGADO:
	    memcpy(&tgtdat1_, &rec.tag.data[0], sizeof(tgtdat1_));	    
	    memcpy(&tgtdat2_, &rec.tag.data[1], sizeof(tgtdat2_));	    
	    break;
	case REC_RHIC_CONF:
	    sipar_.idiv = rec.cfg.data.CSR.split.iDiv;
	    sipar_.ifine = rec.cfg.data.CSR.split.Mod2D;
	    sipar_.emin = rec.cfg.data.Emin;
	    sipar_.trgmin = rec.cfg.data.TrigMin;
	    sipar_.nsperchan = rec.cfg.data.WFDTUnit;
	    k = 0;
	    for (i=0; i<rec.cfg.data.NumChannels; i++) {
		if (rec.cfg.data.chan[i].CamacN == 0) continue;
//printf("found channel %d at location %d %d\n", i+1, rec.cfg.data.chan[i].CamacN,rec.cfg.data.chan[i].VirtexN);
		sipar_.ecoef[i] = rec.cfg.data.chan[i].ecoef;
		sipar_.edead[i] = rec.cfg.data.chan[i].edead;
		sipar_.tmin[i] = 
		    rec.cfg.data.chan[i].Window.split.Beg * sipar_.nsperchan;
		j = i + 1;
		mybook_(&j);
		for (m=0; m<256; m++) {		// carbon lookups
		    data[m] = (rec.cfg.data.chan[i].LookUp[m] & 0xFF) * 0.5 * sipar_.nsperchan;
		    data[m+256] = ((rec.cfg.data.chan[i].LookUp[m] >>8) & 0xFF) * 0.5 * sipar_.nsperchan;
		}
		HPAKAD(800 + j, &data[0]);
		HPAKAD(900 + j, &data[256]);
		for (m=0; m<256; m++) {		// carbon lookups raw
		    data[m] = (rec.cfg.data.chan[i].LookUp[m] & 0xFF);
		    data[m+256] = ((rec.cfg.data.chan[i].LookUp[m] >>8) & 0xFF);
		}
		HPAKAD(1800 + j, &data[0]);
		HPAKAD(1900 + j, &data[256]);
		for (m=0; m<256; m++) {		// integral lookups raw
		    data[m] = (rec.cfg.data.chan[i].LookUp[m+256] & 0xFF) 
			* (1 << (sipar_.idiv+2));
		    data[m+256] = ((rec.cfg.data.chan[i].LookUp[m+256] >>8) & 0xFF)
			* (1 << (sipar_.idiv+2));
		}
		HPAKAD(2000 + j, &data[0]);
		HPAKAD(2100 + j, &data[256]);
		k++;
	    }
	    printf("Configuration OK : %d Si found.\n", k);
	    break;
	case REC_READALL:
	    atraw_.id = rec.header.timestamp.delim;
	    atdata_.d = atraw_.id;
	    if (rec.header.type & REC_120BUNCH) {
		for (i = 0; i < rec.header.len - sizeof(recordHeaderStruct);) {
		    ALL120Ptr = (recordALLMode120Struct *) (&rec.data.rec[i]);
		    k = ALL120Ptr->subhead.siNum;
		    l = ALL120Ptr->subhead.Events + 1;
		    i += sizeof(subheadStruct) + l*sizeof(ALLMode120Struct);
		    if (i > rec.header.len - sizeof(recordHeaderStruct)) {
			printf("Broken record %d (%d bytes). Last subhead: siNum=%d  Events=%d\n", 
			    rec.header.num, rec.header.len, k+1, l);
			break;
		    }
		    for (j = 0; j < l; j++) {
			memcpy(waveform, ALL120Ptr->data[j].d, sizeof(ALL120Ptr->data[j].d));
			atraw_.ia = ALL120Ptr->data[j].a;
			atdata_.a = atraw_.ia * sipar_.ecoef[k] + sipar_.edead[k];
			atraw_.it = ALL120Ptr->data[j].t;
			atdata_.t = atraw_.it * 0.5 * sipar_.nsperchan;
			atraw_.itmax = ALL120Ptr->data[j].tmax;
			atdata_.tmax = atraw_.it * 0.5 * sipar_.nsperchan;
			atraw_.is = ALL120Ptr->data[j].s << (2+sipar_.idiv);
			atdata_.s = (atraw_.is * sipar_.ecoef[k] + sipar_.edead[k]) * sipar_.nsperchan;
			atraw_.ib = ALL120Ptr->data[j].b;
			atdata_.b = atraw_.ib;
			atraw_.irev = 2*ALL120Ptr->data[j].rev +  ALL120Ptr->data[j].rev0;
			atdata_.rev = atraw_.irev;
			m = sizeof(ALL120Ptr->data[j].d); 
			n = REC_READSUB;
			wfana_(waveform, &m, &n);
			if (runpars_.intp) HFNT(k+1);
			if (cnt1 > MAXNTEVENTS && runpars_.intp == 1) {
			    printf("Warning: Number of events is above NTuple limit (%d)\n", MAXNTEVENTS);
			    runpars_.intp = 0;
			}
			cnt1++;
		    }
		}
	    } else {
		for (i = 0; i < rec.header.len - sizeof(recordHeaderStruct);) {
		    ALLPtr = (recordALLModeStruct *) (&rec.data.rec[i]);
		    k = ALLPtr->subhead.siNum;
		    l = ALLPtr->subhead.Events + 1;
		    i += sizeof(subheadStruct) + l*sizeof(ALLModeStruct);
		    if (i > rec.header.len - sizeof(recordHeaderStruct)) {
			printf("Broken record %d (%d bytes). Last subhead: siNum=%d  Events=%d\n", 
			    rec.header.num, rec.header.len, k+1, l);
			break;
		    }
		    for (j = 0; j < l; j++) {
			memcpy(waveform, ALLPtr->data[j].d, sizeof(ALLPtr->data[j].d));
			atraw_.ia = ALLPtr->data[j].a;
			atdata_.a = atraw_.ia * sipar_.ecoef[k] + sipar_.edead[k];
			atraw_.it = ALLPtr->data[j].t;
			atdata_.t = atraw_.it * 0.5 * sipar_.nsperchan;
			atraw_.itmax = ALLPtr->data[j].tmax;
			atdata_.tmax = atraw_.it * 0.5 * sipar_.nsperchan;
			atraw_.is = ALLPtr->data[j].s << (2+sipar_.idiv);
			atdata_.s = (atraw_.is * sipar_.ecoef[k] + sipar_.edead[k]) * sipar_.nsperchan;
			atraw_.ib = ALLPtr->data[j].b;
			atdata_.b = atraw_.ib;
			atraw_.irev = 2*ALLPtr->data[j].rev +  ALLPtr->data[j].rev0;
			atdata_.rev = atraw_.irev;
			m = sizeof(ALLPtr->data[j].d); 
			n = REC_READSUB;
			wfana_(waveform, &m, &n);
			if (runpars_.intp) HFNT(k+1);
			if (cnt1 > MAXNTEVENTS && runpars_.intp == 1) {
			    printf("Warning: Number of events is above NTuple limit (%d)\n", MAXNTEVENTS);
			    runpars_.intp = 0;
			}
			cnt1++;
		    }
		}
	    }
	    break;
	case REC_READRAW:
	case REC_READSUB:
	    atraw_.id = rec.header.timestamp.delim;
	    atdata_.d = atraw_.id;
	    if (rec.header.type & REC_120BUNCH) {
		for (i = 0; i < rec.header.len - sizeof(recordHeaderStruct);) {
		    wave120Ptr = (recordReadWave120Struct *) (&rec.data.rec[i]);
		    k = wave120Ptr->subhead.siNum;
		    l = wave120Ptr->subhead.Events + 1;
		    i += sizeof(subheadStruct) + l*sizeof(waveform120Struct);
		    if (i > rec.header.len - sizeof(recordHeaderStruct)) {
			printf("Broken record %d (%d bytes). Last subhead: siNum=%d  Events=%d\n", 
			    rec.header.num, rec.header.len, k+1, l);
			break;
		    }
		    for (j = 0; j < l; j++) {
			memcpy(waveform, wave120Ptr->data[j].d, sizeof(wave120Ptr->data[j].d));
			atraw_.ia = 0;
			atdata_.a = 0;
			atraw_.it = 0;
			atdata_.t = 0;
			atraw_.itmax = 0;
			atdata_.tmax = 0;
			atraw_.is = 0;
			atdata_.s = 0;
			atraw_.ib = wave120Ptr->data[j].b;
			atdata_.b = atraw_.ib;
			atraw_.irev = *((long *)(wave120Ptr->data[j].rev));
			atraw_.irev = 2*atraw_.irev  + wave120Ptr->data[j].rev0;
			atdata_.rev = atraw_.irev;
			m = sizeof(wave120Ptr->data[j].d); 
			n = rec.header.type & REC_TYPEMASK;
			wfana_(waveform, &m, &n);
			if (runpars_.intp) HFNT(k+1);
			if (cnt1 > MAXNTEVENTS && runpars_.intp == 1) {
			    printf("Warning: Number of events is above NTuple limit (%d)\n", MAXNTEVENTS);
			    runpars_.intp = 0;
			}
			cnt1++;
		    }
		}
	    } else {
		for (i = 0; i < rec.header.len - sizeof(recordHeaderStruct);) {
		    wavePtr = (recordReadWaveStruct *) (&rec.data.rec[i]);
		    k = wavePtr->subhead.siNum;
		    l = wavePtr->subhead.Events + 1;
		    i += sizeof(subheadStruct) + l*sizeof(waveformStruct);
		    if (i > rec.header.len - sizeof(recordHeaderStruct)) {
			printf("Broken record %d (%d bytes). Last subhead: siNum=%d  Events=%d\n", 
			    rec.header.num, rec.header.len, k+1, l);
			break;
		    }
		    for (j = 0; j < l; j++) {
			memcpy(waveform, wavePtr->data[j].d, sizeof(wavePtr->data[j].d));
			atraw_.ia = 0;
			atdata_.a = 0;
			atraw_.it = 0;
			atdata_.t = 0;
			atraw_.itmax = 0;
			atdata_.tmax = 0;
			atraw_.is = 0;
			atdata_.s = 0;
			atraw_.ib = wavePtr->data[j].b;
			atdata_.b = atraw_.ib;
			atraw_.irev = *((long *)(wavePtr->data[j].rev));
			atraw_.irev = 2*atraw_.irev  + wavePtr->data[j].rev0;
			atdata_.rev = atraw_.irev;
			m = sizeof(wavePtr->data[j].d); 
			n = rec.header.type & REC_TYPEMASK;
			wfana_(waveform, &m, &n);
			if (runpars_.intp) HFNT(k+1);
			if (cnt1 > MAXNTEVENTS && runpars_.intp == 1) {
			    printf("Warning: Number of events is above NTuple limit (%d)\n", MAXNTEVENTS);
			    runpars_.intp = 0;
			}
			cnt1++;
		    }
		}
	    }
	    break;
	case REC_READAT:
	    atraw_.id = rec.header.timestamp.delim;
	    atdata_.d = atraw_.id;
	    for (i = 0; i < rec.header.len - sizeof(recordHeaderStruct);) {
		ATPtr = (recordReadATStruct *) (&rec.data.rec[i]);
		k = ATPtr->subhead.siNum;
		l = ATPtr->subhead.Events + 1;
		i += sizeof(subheadStruct) + l*sizeof(ATStruct);
		if (i > rec.header.len - sizeof(recordHeaderStruct)) {
		    printf("Broken record %d (%d bytes). Last subhead: siNum=%d  Events=%d\n", 
			rec.header.num, rec.header.len, k+1, l);
		    break;
		}
		for (j = 0; j < l; j++) {
		    atraw_.ia = ATPtr->data[j].a;
		    atdata_.a = atraw_.ia * sipar_.ecoef[k] + sipar_.edead[k];
		    atraw_.it = ATPtr->data[j].t;
		    atdata_.t = atraw_.it * 0.5 * sipar_.nsperchan;
		    atraw_.itmax = ATPtr->data[j].tmax;
		    atdata_.tmax = atraw_.it * 0.5 * sipar_.nsperchan;
		    atraw_.is = ATPtr->data[j].s << (2+sipar_.idiv);
		    atdata_.s = (atraw_.is * sipar_.ecoef[k] + sipar_.edead[k]) * sipar_.nsperchan;
		    atraw_.ib = ATPtr->data[j].b;
		    atdata_.b = atraw_.ib;
		    atraw_.irev = 2*ATPtr->data[j].rev +  ATPtr->data[j].rev0;
		    atdata_.rev = atraw_.irev;
		    if (runpars_.intp) HFNT(k+1);
		    if (cnt1 > MAXNTEVENTS && runpars_.intp == 1) {
			printf("Warning: Number of events is above NTuple limit (%d)\n", MAXNTEVENTS);
			runpars_.intp = 0;
		    }
		    cnt1++;
		}
	    }
	    break;
	case REC_WFDV8SCAL:
	    for (i=0; i<1536; i++) data[i] = rec.wfd.hist[i];
	    s1 = 0;
	    for (i=0; i<128; i++) s1 += rec.wfd.hist[i];
	    s2 = 0;
	    for (i=0; i<128; i++) s2 += rec.wfd.hist[i+128];
	    s3 = 0;
	    for (i=0; i<128; i++) s3 += rec.wfd.hist[i+256];
	    s4 = 0;
	    for (i=0; i<128; i++) s4 += rec.wfd.hist[i+384];	    
	    j = rec.wfd.siNum + 1;
	    HPAKAD(200 + j, &data[0]);
	    HPAKAD(300 + j, &data[128]);
	    HPAKAD(400 + j, &data[256]);
	    HPAKAD(500 + j, &data[384]);
	    HPAKAD(1300 + j, &data[128]);
	    HPAKAD(1400 + j, &data[256]);
	    HPAKAD(1500 + j, &data[384]);
	    if (sipar_.ifine == 0) {
		HPAKAD(600 + j, &data[512]);
		HPAKAD(1600 + j, &data[512]);
	    } else {
		HPAKAD(700 + j, &data[512]);
		HPAKAD(1700 + j, &data[512]);
	    }
	    printf("Si%02d : %12d %12d %12d %12d %12d    %12d  %12d  %12d  %12d",
		rec.wfd.siNum + 1, rec.wfd.scalers[0], rec.wfd.scalers[1],
		rec.wfd.scalers[2], rec.wfd.scalers[3], rec.wfd.scalers[4],
		s1, s2, s3, s4);
	    if (s2 < rec.wfd.scalers[2] || s3 != rec.wfd.scalers[0] || 
		s4 != rec.wfd.scalers[1] || s1 < s2 + s3 + s4) {
		printf(" <- !!!");
		sipar_.mark[rec.wfd.siNum]++;
	    }
	    printf("\n");
	    cnt+=(rec.wfd.scalers[0] + rec.wfd.scalers[1] + rec.wfd.scalers[2]);
	    for (i=0;i<3;i++) {
		sscal_[rec.wfd.siNum * 3+i] += rec.wfd.scalers[i];
	    }
	    break;
	case REC_SCALERS:
	    memcpy(&scalers_, &rec.scal.data[0], sizeof(scalers_));	    
	    break;
	case REC_HJETSWITCH:
	    atraw_.ijet = rec.jet.data[1]/JET_PLUS;
	    atdata_.jet = atraw_.ijet;
//	    printf("JETSWITCH: to %d at %d events\n", atraw_.ijet, cnt1);
	    break;
	case REC_PCTARGET:
	    i = (rec.header.len - sizeof(rec.header))/(4*sizeof(long));
	    histdelim_((long *)&rec.buffer[sizeof(rec.header)], (long *)&i);
	    break;
	case REC_COUNTRATE:
	    i = (rec.header.len - sizeof(rec.header))/(sizeof(long));
	    histrate_((long *)&rec.buffer[sizeof(rec.header)], (long *)&i);
	    break;
	case REC_TAGMOVEADO:
	    i = (rec.header.len - sizeof(rec.header))/(2*sizeof(long));
	    histtagmov_((long *)&rec.buffer[sizeof(rec.header)], (long *)&i);	    
	    break;
	case REC_WCMADO:
	    break;
	default:	// skip any unknown records
	    printf("Unknown record %8.8X encountered in input data file\n",
		rec.header.type & REC_TYPEMASK);
	    break;
	}
    }
    printf("Total event number from scalers: %d, from data: %d\n", cnt, cnt1);
    return recRing;
}
