#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <TFile.h>
#include <TTree.h>
#include "rhicpol.h"
#include "rpoldata.h"

//	Common /atraw/ (for n-tuple)
struct {
    unsigned long rev;	// revolution number
    unsigned short s;	// square
    unsigned char a;	// amplitude at maximum
    unsigned char t;	// time at 1/4 of maximum
    unsigned char tmax;	// time at maximum
    unsigned char b;	// bunch number: 0-59
    signed char pj;	// jet polarization (-1, 0, +1)
    signed char pb;	// beam polarization (-1, 0, +1)
    signed char f;	// beam bunch (0 - empty, 1 - filled)
} atraw;

//	Common /rhic/
struct {
    int fillpat[120];
    int polpat[120];
} rhic;

//	Common /beamdat/
beamDataStruct beamdat;
configRhicDataStruct Conf;
SiChanStruct *SiConf = NULL;
TTree **SiTree = NULL;
TFile * FileOut = NULL;

void mybook(int si)
{
    char shortname[10];
    char longname[80];
    sprintf(shortname, "Si%3.3d", si+1);
    sprintf(longname, "Root n-tuple for Si %3d", si+1);
    SiTree[si] = new TTree(shortname, longname);
    SiTree[si]->Branch("data", &atraw, "REV/i:S/s:A/b:T/b:TMAX/b:B/b:PJ/B:PB/B:F/B");
}

void myfill(int si)
{
    SiTree[si]->Fill();
}

void readandfill(FILE *fin)
{
//	buffer to read the next record - static due to its size
    static union {
	recordHeaderStruct   header;
	recordBeginStruct    begin;
        recordDataStruct     data;
        recordEndStruct      end;
        recordConfigRhicStruct cfg;
        recordBeamAdoStruct  beam;
	recordHJetSwitchStruct jet;
        char                 buffer[BSIZE*sizeof(int)];
    } rec;
//	data structure pointers
    recordReadATStruct * ATPtr;
    recordALLModeStruct * ALLPtr;
    recordALLMode120Struct * ALL120Ptr;
    
    int i, j, k, l, m, n, cnt1=0;
    int recRing;
    long delim;
    const int jet2pol[4] = {0, 1, -1, 0};
    
    memset(&beamdat, 0, sizeof(beamdat));    
    memset(&rhic, 0, sizeof(rhic));    
    memset(&Conf, 0, sizeof(Conf));    

    for(;;) {
	i = fread(&rec.header, sizeof(recordHeaderStruct), 1, fin); 
	if (feof(fin)) break;
	if (i != 1) {
	    printf("R2HBOOK-ERR : IO error on input file (head) %s.\n", strerror(errno));
	    break;
	}
	if (rec.header.len > BSIZE*sizeof(int)) {
	    printf("R2HBOOK-WARN : Very large record (%d).\n", rec.header.len);
	    i = fseek(fin, rec.header.len - sizeof(recordHeaderStruct), 
		SEEK_CUR);
	    if (feof(fin)) break;
	    if (i != 0) {
		printf("R2HBOOK-ERR : IO error on input file (seek) %s.\n", strerror(errno));
		break;
	    }
	    continue;
	}
	i = fread(&rec.begin.version, rec.header.len - sizeof(recordHeaderStruct), 1, fin); 
	if (feof(fin)) {
	    printf("R2HBOOK-WARN : Unexpected end of file.\n");
	    break;
	}
	if (i != 1) {
	    printf("R2HBOOK-ERR : IO error on input file (data) %s.\n", strerror(errno));
	    break;
	}
	recRing |= rec.header.type & (~(REC_TYPEMASK | REC_FROMMEMORY | REC_120BUNCH));
//	printf("Record %5d type %8.8X  len %8d timestamp %8.8X\n", 
//	    rec.header.num, rec.header.type, rec.header.len, rec.header.timestamp.time);
	switch(rec.header.type & REC_TYPEMASK) {
	case REC_BEGIN:
	    if (rec.begin.version < 20000) {
		printf("R2HBOOK-ERR : DAQ versions < 2.0.0 are not supported\n");
		exit(-2);
	    }
	    printf("R2HBOOK-INFO : Begin of data set version=%d for ", rec.begin.version);
	    if (rec.header.type & REC_JET) printf("JET-");
	    if (rec.header.type & REC_YELLOW) {
		printf("YELLOW");
	    } else if (rec.header.type & REC_BLUE) {
		printf("BLUE");
	    } else printf("unknown");
	    printf(" ring.\n%s : %s", rec.begin.comment,
		ctime(&rec.header.timestamp.time));
	    break;
	case REC_END:
	    printf("R2ROOT-INFO : End of data set at record %d : %s\n", 
		rec.header.num, rec.end.comment);
	    break;
	case REC_POLADO:
	    break; 	
	case REC_BEAMADO:
	    memcpy(&beamdat, &rec.beam.data, sizeof(beamdat));	    
	    for (i=0;i<120;i++) {
		rhic.fillpat[i] = beamdat.measuredFillPatternM[3*i];
		rhic.polpat[i] = beamdat.polarizationFillPatternS[3*i];
	    }
	    printf("Fill pattern: ");
	    for (i=0;i<120;i++) {
		printf("%c",(rhic.fillpat[i]!=0)?'|':'_');
	    }
	    printf("\n");
	    printf("Pol. pattern: ");
	    for (i=0;i<120;i++) {
		char cc;
		if (rhic.polpat[i] > 0) cc='+';
		else {
		    if (rhic.polpat[i] < 0) cc='-';
		    else cc='0';
		}
		printf("%c", cc);
	    }
	    printf("\n");
	    break;
	case REC_WCMADO:
	    break;
	case REC_TAGADO:
	    break;
	case REC_RHIC_CONF:
	    memcpy(&Conf, &rec.cfg.data, sizeof(Conf));
	    SiConf = (SiChanStruct *) malloc(Conf.NumChannels * sizeof(SiChanStruct));
	    memcpy(SiConf, rec.cfg.chanconf, rec.header.len - sizeof(Conf) - sizeof(rec.header));
	    SiTree = (TTree **) malloc(Conf.NumChannels * sizeof(TTree *));
	    memset(SiTree, 0, Conf.NumChannels * sizeof(TTree *));
	    for (i=0; i<Conf.NumChannels; i++) mybook(i);
	    printf("Configuration OK : %d Si found.\n", Conf.NumChannels);
	    break;
	case REC_READALL:
	    delim = rec.header.timestamp.delim;
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
			atraw.a = ALL120Ptr->data[j].a;
			atraw.t = ALL120Ptr->data[j].t;
			atraw.tmax = ALL120Ptr->data[j].tmax;
			atraw.s = ALL120Ptr->data[j].s << (2+Conf.CSR.split.iDiv);
			atraw.b = ALL120Ptr->data[j].b;
			atraw.rev = 512*delim + 2*ALL120Ptr->data[j].rev +  ALL120Ptr->data[j].rev0;
			atraw.f = rhic.fillpat[3*atraw.b];
			atraw.pb = rhic.polpat[3*atraw.b];
			myfill(k);
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
			atraw.a = ALLPtr->data[j].a;
			atraw.t = ALLPtr->data[j].t;
			atraw.tmax = ALLPtr->data[j].tmax;
			atraw.s = ALLPtr->data[j].s << (2+Conf.CSR.split.iDiv);
			atraw.b = ALLPtr->data[j].b;
			atraw.rev = 512*delim + 2*ALLPtr->data[j].rev +  ALLPtr->data[j].rev0;
			atraw.f = rhic.fillpat[3*atraw.b];
			atraw.pb = rhic.polpat[3*atraw.b];
			myfill(k);
			cnt1++;
		    }
		}
	    }
	    break;
	case REC_READRAW:
	case REC_READSUB:
	    break;
	case REC_READAT:
	    delim = rec.header.timestamp.delim;
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
		    atraw.a = ATPtr->data[j].a;
		    atraw.t = ATPtr->data[j].t;
		    atraw.tmax = ATPtr->data[j].tmax;
		    atraw.s = ATPtr->data[j].s << (2+Conf.CSR.split.iDiv);
		    atraw.b = ATPtr->data[j].b;
		    atraw.rev = 512*delim + 2*ATPtr->data[j].rev +  ATPtr->data[j].rev0;
		    atraw.f = rhic.fillpat[3*atraw.b];
		    atraw.pb = rhic.polpat[3*atraw.b];
		    myfill(k);
		    cnt1++;
		}
	    }
	    break;
	case REC_WFDV8SCAL:
	    break;
	case REC_SCALERS:
	    break;
	case REC_HJETSWITCH:
	    atraw.pj = jet2pol[rec.jet.data[1]/JET_PLUS & 3];
//	    printf("JETSWITCH: to %d at %d events\n", atraw_.ijet, cnt1);
	    break;
	default:	// skip any unknown records
	    printf("Unknown record %8.8X encountered in input data file\n",
		rec.header.type & REC_TYPEMASK);
	    break;
	}
    }
    printf("Total event number from data: %d\n", cnt1);
}

int main(int argc, char **argv)
{
    FILE *fin;
    
    if (argc < 0) {
	printf("Usage jet2root filein.data fileout.root\n");
	return -1;
    }
    fin = fopen(argv[1], "rb");
    if (fin == NULL) {
	printf("Cannot open input file %s.\n", argv[1]);
	return -2;
    }
    FileOut = new TFile(argv[2], "RECREATE");
    if (!FileOut->IsOpen()) {
	printf("Cannot open output file %s.\n", argv[2]);
	return -3;
    }
    readandfill(fin);
    FileOut->Write();
    FileOut->Close();
    return 0;
}
