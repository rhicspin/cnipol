/************************************************************************
 *      		WFD memory test for 				*
 *      CMC100 CAMAC USB controller with cmcamac driver.	        *
 *	D.Svirida & I.Alekseev @itep.ru, 2005-10			*
 *	file: libcmc.c							*
 ************************************************************************/
#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#define min(a,b) ((a) < (b)) ? (a) : (b)

#include "../libcmc/libcmc.h"

#define CD 20	// Actual cach delay. Need only 7 us, 20 us is for safety
#define CACHEDELAY(CYCLES) 


typedef union {
	unsigned short s[2];
	unsigned int l;
} ls;

int START = 0;
int STOP = 0x4000000;
int LSTOP = 0x400;
int CR = 1;
int N = 4;
int VIRTMASK = 0xF;
int LOOP = 1;
int MASK = 0xFFFF;
int RETRY = 1;
int FOP = 0;
int EXTCLK = 0;
int ANERR = 1;
int TEST = 0xF;
char FCOPT = 'R';
int FCACC = 0;
int FROPT = 0;
int REPEAT = 5;
int MULT = 100;
int BLKSIZE = 0x200;
int MODECSR = 0x012;
int WFDVER = 9;
unsigned short * WD;
unsigned short * RD;
unsigned short * RDD;
unsigned int seed;
int iStop = 0;

const char tests[5][4] = {"LUT", "Dir", "Ind", "GCC", "Evt"};


void termhandler(int isig) {
    iStop = 1;
}

void fcopt(int i) {
;
}

void fcacc(int i) {
;
}

int lutclose(void) {
    int i;
    C_WRITE(N, 9, 16, 0xF);	// Select ALL Virtexes
    C_WRITE(N, 2, 17, 0); 	// Max TRG
    C_WRITE(N, 3, 17, 0x1040); 	// Closed win
    C_WRITE(N, 1, 17, 0); 	// Stop dataflow from Virtexes
    C_READ(N, 2, 9, i);		// RS_ADR
    for (i=0; i<512; i++) 
	C_WRITE(N, 0, 17, 0x00FF); // Write all LUT completely closed
    CACHEDELAY(10000);
    return 10517;
}

void MemDump(unsigned int addr) {
    int i, j;
    unsigned short D;
    C_WRITE(N, 9, 16, 0x10); 	// Select memory chip
    C_WRITE(N, 1, 16, 0); 	// CSR, no caching
    C_WRITE(N, 0, 17, (addr & 0xFFFF));
    C_WRITE(N, 1, 17, ((addr>>16) & 0xFFFF));
    for (i=addr; ; i+=16) {
	printf("%8.8X:", i);
	for (j=0; j<8; j++) {
	    C_READ(N, 0, 0, D);
	    printf("  %4.4X", D);
	}
	printf("\n");
	if (((i-addr) & 0xFF) == 0xF0) {
	    printf("\nPress <Enter> to continue, Q<Enter> to quit\n");
	    if (toupper(getchar()) == 'Q') break;
	}
    }
    getchar();	// take away <Enter>
}

void CamacFun(int N, int A, int F, int *D, int *Q, int *X)
{
    int status;
    int qx;
    status = CMC_Single(Camac, N, F, A, *D);
    if (status >= 0) {
	if (F<8) *D = status & 0xFFFFFF;
	qx = CMC_GetQX();
	*Q = 1 & (qx>>1);
	*X = 1 & qx;
    } else {
	if (F<8) *D = 0;
	*Q = 0;
	*X = 0;
    }
}

void Bur(void) {

    char cmd[1000];
    const char delim[] = "\t\n =:";
    char *com;
    int Ns, A, F, D, k, i, Q, X;
    int Help;

    Ns = N;
    A = 0;
    F = 0;
    D = 0;
    for (;;) {
	Help = 0;
	k = 1;
	printf("BUR>"); 
	fgets(cmd, sizeof(cmd), stdin);
	com = strtok(cmd, delim);
	for(;;) {
	    if (com == NULL) break;
	    switch (toupper(com[0])) {
	    case 'E':
	    case 'Q':
	    case 'X':
		return;
	    case 'C':
		k = strtol(&com[1], NULL, 0);
		if (k == 0) k = 999999999;
		break;
	    case 'N':
		Ns = strtol(&com[1], NULL, 0);
		break;
	    case 'A':
		A = strtol(&com[1], NULL, 0);
		break;
	    case 'F':
		F = strtol(&com[1], NULL, 0);
		break;
	    case 'W':
		F = F | 16;
		D = strtol(&com[1], NULL, 0);
		break;
	    case 'D':
		D = strtol(&com[1], NULL, 0);
		break;
	    case 'R':
		F = F & 15;
		break;
	    case 'H':
		Help = 1;
		break;
	    }
	    com = strtok(NULL, delim);
	}
	if (Help) {
	    printf("Manual CAMAC control. Commands:\n");
	    printf("[Ndd] [Add] [Fdd] [Ddd] NAF and data in any order;\n");
	    printf("W[dd] == Ddd and set F16; R == clear F16;\n");
	    printf("C[dd] - repeat (if dd==0 forever), Ctrl-\\ to stop;\n");
	    printf("Q - exit.\n");
	} else {
	    iStop = 0;
	    signal(SIGTERM, termhandler);
	    signal(SIGINT, termhandler);
	    signal(SIGQUIT, termhandler);
	    for (i=0; i<k; i++) {
		if (iStop) break;
		CamacFun(Ns, A, F, &D, &Q, &X);
		if (i == 0) {
		    if (F<8) {
			printf("N%d A%d F%d => R0x%X Q(%d) X(%d)", Ns, A, F, D, Q, X);
		    } else if (F>15 && F<24) {
			printf("N%d A%d F%d W0x%X => Q(%d) X(%d)", Ns, A, F, D, Q, X);
		    } else {
			printf("N%d A%d F%d => Q(%d) X(%d)", Ns, A, F, Q, X);		    
		    }
		    if (k>1) printf(" repeat=%d, <Ctrl>-C to stop", k);
		    printf("\n");
		}
	    }
	    signal(SIGTERM, SIG_DFL);
	    signal(SIGINT, SIG_DFL);
	    signal(SIGQUIT, SIG_DFL);
	}
    }
}


int PulseProg() {
    int j, D;
    struct timespec ts = {0, 100000000};

    C_WRITE(N, 8, 16, 8);
    printf("Pulsing PROG.");
    for (j=0; j<40; j++) {
	C_READ(N, 8, 0, D);
	if (D & 0x8000) break;
	nanosleep(&ts, NULL);
	printf(".");
	fflush(stdout);
    }
    if (j != 40) {
	printf("DONE\n\n");
	return 0;
    } else {
	printf("\n\t!!! ERROR waiting for DONE !!!\n\n");
	return 1;
    }
}

int analyse(char * str) {
    char known[][4]= {"RU", "N", "VI", "ST", "TO", "LU", "LO", 
		      "RE", "MA", "CL", "A", "TE", "FC", "FR", 
		      "SA", "Q", "MU", "CR", "BL", "D", "BU",
		      "MO", "VE", "P"};
    int i, j, cmd, adr;
    char *tok;
    const char tst[]="LDIGE";
    
    if (str == NULL || strlen(str) == 0) {
	printf("\tRUn -- run tests\n");
	printf("\tQuit -- exit program\n");
	printf("\tBUr -- manual CAMAC controller\n");
	printf("\tPulseprog -- reconfigure Virtecies\n");
	printf("\tDump [<h>] -- Dump memory starting at <h> bytes\n");
	printf("\tCRate <n> -- Camac crate number\n");
	printf("\tN <n> -- Camac station number\n");
	printf("\tVirtexmask <h> -- virtex mask for LUT/GCC tests\n");
	printf("\tSTart <h> -- start address for testing (bytes)\n");
	printf("\tTOp <h> -- top address in RAM for testing (bytes)\n");
	printf("\tLUttop <h> -- top address in LUT for testing (bytes)\n");
	printf("\tBLocksize <h> -- block size for bulk read/write (bytes)\n");
	printf("\tLOop <n> -- number of passes for tests\n");
	printf("\tREtry <n> -- number of write retries\n");
	printf("\tMAsk <h> -- bit mask for cheking\n");
	printf("\tCLocks I|E -- internal or external clocks\n");
	printf("\tAnalyse T|E -- analyse time | errors\n");
	printf("\tTEst [L][D][I][G] -- execute LUT | Direct | Indirect | GCC test\n");
	printf("\tFCamacoption R|A|D<n> -- FastCamac option, n=0..7\n");
	printf("\tFRamoption 0|1|2 -- FastCamac RAM option\n");
	printf("\tSAmewords <n> -- number of same words to write for Indirect test\n");
	printf("\tMUltiple <n> -- read/wrte LUT n times for better time estimate\n");
	printf("\tMOdeCSR <h> -- for the event check test -- CSR value defining the mode\n");
	printf("\tVErsion <n> -- for the event check test -- WFD version (8-9)\n");
	return -1;
    }
    adr = -1;
    if (str[0] == '*') return -1;
    tok = strtok(str, "= \t\n");
    if (tok == NULL || strlen(tok) == 0) return -1;
    for (i=0, cmd=-1; i<(int)(sizeof(known)/sizeof(known[0])); i++) {
	if (!strncasecmp(tok, known[i], min(strlen(known[i]), strlen(tok)))) {
	    if (cmd == -1) 
		cmd = i;
	    else {
		printf("\nWARN-Ambiguous command: %s\n\n", tok);
		return -1;
	    }
	}
    }
    if (cmd == -1) {
	printf("\nWARN-Unknown command: %s\n\n", tok);
	return -1;
    } 
    if (cmd == 0) return 0;
    if (cmd == 15) return -2;
    if (cmd == 20) { 
	if (CR < 0) {
	    printf("CAMAC not opened. Try CR <crate_number> first.\n");
	} else {
	    Bur(); 
	}
	return cmd; 
    }
    if (cmd == 23) { PulseProg(); return cmd; }
    tok = strtok(NULL, "= \t\n");
    if (tok == NULL || strlen(tok) == 0) {
	if (cmd == 19) {
	    adr = START;
	} else {
	    printf("\nWARN-Need an argument\n\n");
	    return -1;
	}
    }
    switch (cmd) {
	case 1:	N = strtol(tok, NULL, 0); 		break;	// Station
	case 2:	VIRTMASK = strtol(tok, NULL, 16) & 0xF;	break;	// Virtex mask
	case 3:	START = strtol(tok, NULL, 16) & 0xFFFFFFFC; break; // Start address
	case 4:	STOP = strtol(tok, NULL, 16) & 0xFFFFFFFC;  break; // Top address
	case 5:	LSTOP = strtol(tok, NULL, 16) & 0xFFFFFFFC; break; // LUT top address
	case 6:	LOOP = strtol(tok, NULL, 0); 		break;	// Test iterations
	case 7:	RETRY = strtol(tok, NULL, 0); 		break;	// Write retries
	case 8:	MASK = strtol(tok, NULL, 16) & 0xFFFF;	break;	// Check mask
	case 9:	EXTCLK = (toupper(tok[0])=='E') ? 1 : 0; break;	// Clocks
	case 10: ANERR = (toupper(tok[0])=='T') ? 0 : 1; break;	// Analysis
	case 11:	// Tests
	    TEST = 0;
	    for (i=0; i<(int)strlen(tok); i++) for (j=0;j<(int)strlen(tst); j++)
		if (toupper(tok[i]) == tst[j]) TEST |= (1 << j);
	    break;
	case 12: 	// FastCamac option/acceleration
	    FCOPT = toupper(tok[0]); 
	    FCACC = strtol(&tok[1], NULL, 0) & 7;
	    break; 
	case 13: 	// FC RAM option
	    FROPT = strtol(tok, NULL, 0) & 3;
	    if (FROPT == 2) FROPT = 3;
	    break;
	case 14: REPEAT = strtol(tok, NULL, 0) & 0xF;	break;	// same words for indirect
	case 16: MULT = strtol(tok, NULL, 0);		break;	// repetitions of LUT read
	case 17: 	// Crate
	    CR = strtol(tok, NULL, 0);
	    C_CLOSE();
	    i = C_INIT(CR);
	    if (i < 0) {
		printf("Cannot open crate %d, Reason: %s\n", CR, strerror(-i)); 
		CR = -1;
	    }
	    break;
	case 18: BLKSIZE = strtol(tok, NULL, 16) & 0xFFFFFFFC; break; // Block size
	case 19: 
	    if (adr == -1) adr = strtol(tok, NULL, 16) & 0xFFFFFFFC;
	    MemDump(adr); 
	    break;
	case 21: MODECSR = (strtol(tok, NULL, 16) & 0x3F3F) | 0x10; break; // Mode CSR for event check
	case 22: WFDVER = (strtol(tok, NULL, 0) & 1) + 8 ; break; // WFD VErsion
	default:    printf("\nWARN-Internal error\n\n"); return -1;	    
    }
    return cmd;
}

void printconf(void) {
    int i;
    const char errtime[2][5] = {"TIME", "ERR"};
    const char clk[2][5] = {"INT", "EXT"};
    const char ramopt[4][12] = {"NonCashed", "Cashed", "", "BothEdges"};
    
    printf("CN: %d %d  VirtMask: %X  Loops: %d  Analyse %s for:", 
	CR, N, VIRTMASK, LOOP, errtime[ANERR]);
    for (i=0; i<(int)(sizeof(tests)/sizeof(tests[0])); i++)
	if ((TEST >> i) & 1) printf(" %s", tests[i]);
    printf("\nV%1.1d Ranges: RAM: 0x%X-0x%X LUT: 0x%X-0x%X  FCAMACopt: %c%d  FRAMopt: %s\n", 
	WFDVER, START, STOP, START, LSTOP, FCOPT, FCACC, ramopt[FROPT]);
    printf("BLSiz: 0x%X  WrRetry: %d  Same: %d  Mask: 0x%X  Clk: %s  Mult: %d  CSR: %4.4X\n",
	BLKSIZE, RETRY, REPEAT, MASK, clk[EXTCLK], MULT, MODECSR);
    printf("\n");
}

int LUTWrite(void) {
    int i, j, k, l, towrite;
    C_WRITE(N, 9, 16, VIRTMASK);	// All virt. at a time
    for (l=0; l<=(MULT-1)*(1-ANERR) && iStop==0; l++) {
	for (k=0; k<RETRY && iStop==0; k++) {
	    C_READ(N, 2, 9, i);		// RS_ADR
	    srand(seed);
	    for (j=START; j<LSTOP; j+=BLKSIZE) {
		towrite = ((j < LSTOP - BLKSIZE) ? BLKSIZE : (LSTOP - j))/2;
		for (i=0; i<towrite; i++)
		    WD[i] = ((rand() >> 5) + (rand() << 10)) & 0xFFFF;
		C_WRITE_B(N, 0, 17, WD, towrite);
	    }
	}
    }
    return (1+RETRY*(1+(LSTOP-START)/2))*((MULT-1)*(1-ANERR)+1);
}

int LUTRead(int * err) {
    *err = 0;
    int i, j, k, l, toread, ops;
    ops = 0;
    for (l=0; l<=(MULT-1)*(1-ANERR) && iStop==0; l++) {
	for (k=0; k<4; k++) {
	    if (!(VIRTMASK & (1<< k))) continue;
	    ops ++;
	    C_WRITE(N, 9, 16, (1<<k));	// Select virt.
	    C_READ(N, 2, 9, i);		// RS_ADR
	    srand(seed);
	    fcopt(1);
	    for (j=START; j<LSTOP; j+=BLKSIZE) {
		toread = ((j < LSTOP - BLKSIZE) ? BLKSIZE : (LSTOP - j))/2;
		for (i=0; i<toread; i++)
		    WD[i] = ((rand() >> 5) + (rand() << 10)) & 0xFFFF;
		C_READ_B(N, 0, 1, RD, toread, 'R', 4);
		for (i=0; i<toread; i++) if ((RD[i] & MASK) != (WD[i] & MASK)) {
		    (*err)++;
		    if (ANERR) printf("\nV%1.1d: Error at %6.6X: read=%4.4X != write=%4.4X XOR=%4.4X",
			k, j+2*i, RD[i], WD[i], WD[i]^RD[i]);
		}
	    }
	    fcopt(0);
	}
    }
    (*err) /= ((MULT-1)*(1-ANERR)+1); // Don't confuse people with multiple passes
    return ops*(2+(LSTOP-START)/2);
}

int DirectWrite(void) {
    int i, j, k, towrite;
    ls final;
    C_WRITE(N, 9, 16, 0x10); 	// Select memory chip
    C_WRITE(N, 1, 16, 0);	// CSR
    for (k=0; k<RETRY && iStop==0; k++) {
	srand(seed);
	C_WRITE(N, 0, 17, (START & 0xFFFF));
	C_WRITE(N, 1, 17, ((START>>16) & 0xFFFF));
	for (j=START; j<STOP && iStop==0; j+=BLKSIZE) {
	    towrite = ((j < STOP - BLKSIZE) ? BLKSIZE : (STOP - j))/2;
	    for (i=0; i<towrite; i++)
		WD[i] = ((rand() >> 5) + (rand() << 10)) & 0xFFFF;
	    C_WRITE_B(N, 0, 16, WD, towrite);
	    C_READ(N, 0, 1, final.s[0]);
	    C_READ(N, 1, 1, final.s[1]);
	    if (final.l != (unsigned)(j + 2*towrite)) {
		printf("\nFinal pointer error: read=%4.4X != expected=%4.4X (Retry %d)\n",
		    final.l, j+2*towrite, k);
		return 0;
	    }
	}
    }
    return 2+RETRY*(2+(STOP-START)/2 + 2*((STOP-START)/BLKSIZE+1));
}

int DirectRead(int * err) {
    int i, j, toread;
    ls final;
    *err = 0;
    C_WRITE(N, 9, 16, 0x10); 	// Select memory chip
    C_WRITE(N, 1, 16, (FROPT << 1)); 	// CSR
    CACHEDELAY(CD);
    C_WRITE(N, 0, 17, (START & 0xFFFF));
    C_WRITE(N, 1, 17, ((START>>16) & 0xFFFF));
    CACHEDELAY(CD);
    fcopt(1);
    for (j=START; j<STOP && iStop==0; j+=BLKSIZE) {
	toread = ((j < STOP - BLKSIZE) ? BLKSIZE : (STOP - j))/2;
	for (i=0; i<toread; i++)
	    WD[i] = ((rand() >> 5) + (rand() << 10)) & 0xFFFF;
	C_READ_B(N, 0, 0, RD, toread, FCOPT, FCACC);
	if (ANERR) { // Always read twice if error analysis is reqired
	    fcopt(0);
	    CACHEDELAY(CD); // In case we stopped reading at the edge of a page
	    C_WRITE(N, 0, 17, (j & 0xFFFF));
	    C_WRITE(N, 1, 17, ((j>>16) & 0xFFFF));
	    CACHEDELAY(CD); 
	    fcopt(1);
	    C_READ_B(N, 0, 0, RDD, toread, FCOPT, FCACC);
	    C_READ(N, 0, 1, final.s[0]);
	    C_READ(N, 1, 1, final.s[1]);
	    if (final.l != (unsigned)(j + 2*toread)) {
		printf("\nFinal pointer error: read=%4.4X != expected=%4.4X\n",
		    final.l, j+2*toread);
		return 0;
	    }
	}
	for (i=0; i<toread; i++) {
	    if (((RD[i] & MASK) != (WD[i] & MASK)) || 
		((ANERR != 0) && ((RDD[i] & MASK) != (WD[i] & MASK)))) {
		(*err)++;
		if (ANERR) printf("\nError at %6.6X: read=%4.4X;%4.4X != write=%4.4X XOR=%4.4X",
		    j+2*i, RD[i], RDD[i], WD[i], WD[i]^RD[i]);
	    }
	}
    }
    fcopt(0);
    C_READ(N, 0, 1, final.s[0]);
    C_READ(N, 1, 1, final.s[1]);
    if (final.l != (unsigned)STOP) {
	printf("\nFinal pointer error: read=%4.4X != expected=%4.4X\n",
	    final.l, STOP);
	return 0;
    }
    return 4+2*CD+(STOP-START)/2*(ANERR+1)+(2+2*CD)*ANERR*((STOP-START)/BLKSIZE+1);	
}

int IndirectWrite(void) {
    int i, j, k, towrite;
    ls final;
    C_WRITE(N, 8, 16, ((EXTCLK & 1) | (REPEAT << 8)));
    C_WRITE(N, 9, 16, 0x10); 	// Select memory chip
    C_WRITE(N, 1, 16, 1);	// CSR: Allow the dataflow to fifo, no caching
    for (k=0; k<RETRY && iStop==0; k++) {
	srand(seed);
	C_WRITE(N, 0, 17, (START & 0xFFFF));
	C_WRITE(N, 1, 17, ((START>>16) & 0xFFFF));
	for (j=START; j<STOP && iStop==0; j+=(BLKSIZE*(REPEAT+1))) {
	    towrite = ((j < STOP - (BLKSIZE*(REPEAT+1))) ? BLKSIZE : ((STOP - j)/(REPEAT+1)))/2;
	    for (i=0; i<towrite; i++)
		WD[i] = ((rand() >> 5) + (rand() << 10)) & 0xFFFF;
	    C_WRITE_B(N, 12, 16, WD, towrite);
	    C_READ(N, 0, 1, final.s[0]);
	    C_READ(N, 1, 1, final.s[1]);
	    if (final.l != (unsigned)(j + 2*towrite*(REPEAT+1))) {
		printf("\nFinal pointer error: read=%4.4X != expected=%4.4X (Retry %d)\n",
		    final.l, j+2*towrite*(REPEAT+1), k);
		return 0;
	    }
	}
    }
    return 3+RETRY*(2+(STOP-START)/(REPEAT+1)/2 + 2*((STOP-START)/(BLKSIZE*(REPEAT+1))+1));
}

int IndirectRead(int * err) {
    int i, j, toread, top, lastrand;
    *err = 0;
    top = STOP - ((STOP - START)%(2*(REPEAT+1)));
    C_WRITE(N, 9, 16, 0x10); 	// Select memory chip
    C_WRITE(N, 1, 16, (FROPT << 1)); 	// CSR
    CACHEDELAY(CD);
    C_WRITE(N, 0, 17, (START & 0xFFFF));
    C_WRITE(N, 1, 17, ((START>>16) & 0xFFFF));
    CACHEDELAY(CD);
    fcopt(1);
    for (j=START; j<top && iStop==0; j+=BLKSIZE) {
	toread = ((j < top - BLKSIZE) ? BLKSIZE : (top - j))/2;
	for (i=0; i<toread; i++) {
	    if (((j-START+2*i)%(2*(REPEAT+1))) == 0)
		lastrand = ((rand() >> 5) + (rand() << 10)) & 0xFFFF;
	    WD[i] = lastrand;
	}
	C_READ_B(N, 0, 0, RD, toread, FCOPT, FCACC);
	if (ANERR) { // Always read twice if error analysis is reqired
	    fcopt(0);
	    CACHEDELAY(CD); // In case we stopped reading at the edge of a page
	    C_WRITE(N, 0, 17, (j & 0xFFFF));
	    C_WRITE(N, 1, 17, ((j>>16) & 0xFFFF));
	    CACHEDELAY(CD); 
	    fcopt(1);
	    C_READ_B(N, 0, 0, RDD, toread, FCOPT, FCACC);
	}
	for (i=0; i<toread; i++) {
	    if (((RD[i] & MASK) != (WD[i] & MASK)) || 
		((ANERR != 0) && ((RDD[i] & MASK) != (WD[i] & MASK)))) {
		(*err)++;
		if (ANERR) printf("\nError at %6.6X (SAME+%2.2d): read=%4.4X;%4.4X != write=%4.4X XOR=%4.4X",
		    j+2*i, ((j-START+2*i)%(2*(REPEAT+1))), RD[i], RDD[i], WD[i], WD[i]^RD[i]);
	    }
	}
    }
    fcopt(0);
    return 4+2*CD+(top-START)/2*(ANERR+1)+(2+2*CD)*ANERR*((top-START)/BLKSIZE+1);	
}

#define CSR 	0x8012	// Activate Delim (15), allow writing to mem (4)
//#define CSRMASK	0x3FFC	// Bits which may randomly vary in CSR
#define CSRMASK	0x3FC0	// Bits which may randomly vary in CSR

int GCCWrite(void) {
    int i, j, k, nvirt, towrite;
    unsigned short D, DD;
    ls final;
    for (i=0,nvirt=0; i<4; i++) if(((VIRTMASK >> i) & 1) != 0) nvirt++;
    for (k=0; k<RETRY && iStop==0; k++) {
	srand(seed);
	C_WRITE(N, 9, 16, VIRTMASK);
	C_READ(N, 6, 9, D);		   // RS_DEL
	C_WRITE(N, 1, 17, (CSR & 0x7FEF)); // Prepare CSR (no DELIM, no ENMEM), set SC_ALLOW
	C_READ(N, 6, 9, D);		   // RS_DEL (to reset RQ_DEL after SC_ALLOW set)
	// It looks like the previous value of CSR goes to memory
	D = (((rand() >> 5) + (rand() << 10)) & CSRMASK) | (CSR & 0x7FFF);
	C_WRITE(N, 1, 17, D); // Prepare CSR with no DELIM bit
	CACHEDELAY(20);
	C_WRITE(N, 9, 16, 0x10);	// Select memory chip
	C_WRITE(N, 1, 16, 1);		// CSR : Allow dataflow to fifo, no caching
	C_WRITE(N, 0, 17, (START & 0xFFFF));
	C_WRITE(N, 1, 17, ((START>>16) & 0xFFFF));
	C_WRITE(N, 9, 16, VIRTMASK);
	for (j=START; j<STOP && iStop==0; j+=2*nvirt*BLKSIZE) {
	    towrite = ((j < STOP - 2*nvirt*BLKSIZE) ? BLKSIZE : ((STOP - j)/(2*nvirt)))/2;
	    for (i=0; i<towrite; i++)
		WD[i] = (((rand() >> 5) + (rand() << 10)) & CSRMASK) | CSR;
	    C_WRITE_B(N, 1, 17, WD, towrite);	
	    C_READ(N, 6, 1, D);		// All virtexes must give the same result
	    C_READ(N, 6, 1, DD);	// All virtexes must give the same result
	    if (D != (((j - START)/(4*nvirt) + towrite) & 0xFFFF)) {
		printf("\nGCC CNT write error: read=%4.4X;%4.4X != expectd=%4.4X (Retrty %d)\n",
		    D, DD, (((j - START)/(4*nvirt) + towrite) & 0xFFFF), k);
		return 0;
	    }
	    C_WRITE(N, 9, 16, 0x10);	// Select memory chip
	    C_READ(N, 0, 1, final.s[0]);
	    C_READ(N, 1, 1, final.s[1]);
	    C_WRITE(N, 9, 16, VIRTMASK);
	    if (final.l != (unsigned)(j + 4*towrite*nvirt)) {
		printf("\nFinal pointer error: read=%4.4X != expected=%4.4X (Retry %d)\n",
		    final.l, j+4*towrite*nvirt, k);
		return 0;
	    }
	}
	C_WRITE(N, 1, 17, 0); // Stop dataflow from Virtexes
    }
    return RETRY*(10+CD+(STOP-START)/(4*nvirt)+6*(STOP-START)/(2*nvirt*BLKSIZE));
}

int GCCRead(int * err) {
    int i, j, nvirt, toread, top, lastrand, lastcount;
    *err = 0;
    for (i=0,nvirt=0; i<4; i++) if(((VIRTMASK >> i) & 1) != 0) nvirt++;
    top = STOP - ((STOP - START)%(4*nvirt));
    C_WRITE(N, 9, 16, 0x10); 	// Select memory chip
    C_WRITE(N, 1, 16, (FROPT << 1)); 	// CSR
    CACHEDELAY(CD);
    C_WRITE(N, 0, 17, (START & 0xFFFF));
    C_WRITE(N, 1, 17, ((START>>16) & 0xFFFF));
    CACHEDELAY(CD);
    fcopt(1);
    for (j=START,lastcount=0; j<top && iStop==0; j+=BLKSIZE) {
	toread = ((j < top - BLKSIZE) ? BLKSIZE : (top - j))/2;
	for (i=0; i<toread; i+=2) {
	    if (((j-START+2*i)%(4*nvirt)) == 0) {
		lastrand = (((rand() >> 5) + (rand() << 10)) & CSRMASK) | CSR;
		lastcount++;
	    }	
	    WD[i] = lastrand;
	    WD[i+1] = lastcount;
	}
	C_READ_B(N, 0, 0, RD, toread, FCOPT, FCACC);
	if (ANERR) { // Always read twice if error analysis is reqired
	    fcopt(0);
	    CACHEDELAY(CD); // In case we stopped reading at the edge of a page
	    C_WRITE(N, 0, 17, (j & 0xFFFF));
	    C_WRITE(N, 1, 17, ((j>>16) & 0xFFFF));
	    CACHEDELAY(CD); 
	    fcopt(1);
	    C_READ_B(N, 0, 0, RDD, toread, FCOPT, FCACC);
	}
	for (i=0; i<toread; i++) {
	    if (((RD[i] & MASK) != (WD[i] & MASK)) || 
		((ANERR != 0) && ((RDD[i] & MASK) != (WD[i] & MASK)))) {
		(*err)++;
		if (ANERR) printf("\nError at %6.6X (CNT+%2.2d): read=%4.4X;%4.4X != write=%4.4X XOR=%4.4X",
		    j+2*i, ((j-START+2*i)%(4*nvirt)), RD[i], RDD[i], WD[i], WD[i]^RD[i]);
	    }
	}
    }
    fcopt(0);
    return 4+2*CD+(top-START)/2*(ANERR+1)+(2+2*CD)*ANERR*((top-START)/BLKSIZE+1);
}

#define TRGLEVEL	40

int EventWrite(void) {
    int i, ops;
    unsigned int lastaddr, tnext;
    ls final;

    C_SETINH();		// set Inhibit
    C_WRITE(N, 9, 16, 0x10);	// Select memory Vx
    C_WRITE(N, 1, 16, 0); 	// CSR: Stop dataflow to FIFO
    C_WRITE(N, 0, 17, (START & 0xFFFF));
    C_WRITE(N, 1, 17, ((START>>16) & 0xFFFF));
    C_WRITE(N, 1, 16, 1); 	// CSR: Allow dataflow to FIFO
    ops = 6;
    for (i=0; i<4; i++) {
	if (((VIRTMASK >> i) & 1) == 0) continue;
	C_WRITE(N, 9, 16, (1 << i));	// Select Vx
	// CSR: Allow dataflow from Virtexes in selected mode
	C_WRITE(N, 1, 17, (MODECSR | (i << 6))); 
	ops += 2;
    }
    C_WRITE(N, 9, 16, VIRTMASK);	// Select Vx
    C_READ(N, 2, 9, i);			// RS_ADR
    for (i=0; i<TRGLEVEL; i++) 
	C_WRITE(N, 0, 17, 0x00FF); 	// Write ET LUT closed
    for (; i<512; i++) 
	C_WRITE(N, 0, 17, 0xFF00); 	// Write rest ET and IA LUT open
    C_WRITE(N, 3, 17, 0x5F02); 		// Win mostly open
    C_WRITE(N, 2, 17, 240-TRGLEVEL); 	// TRG
    C_CLRINH();			// clear Inhibit
    C_WRITE(N, 9, 16, 0x10);	// Select memory Vx
    ops += 518;
    tnext = time(NULL);
    for ( lastaddr=START; (int)lastaddr < STOP-100 && iStop == 0; ) {
	C_READ(N, 0, 1, final.s[0]);	// Read current pointer
	C_READ(N, 1, 1, final.s[1]);
	if (final.l > lastaddr) {
	    lastaddr = final.l;
	    tnext = time(NULL);
	}
	if (time(NULL)-tnext > 3) {
	    printf("\nError: No signals coming or rate too slow. Cannot run test.\n");
	    tnext = 0xFFFFFFFF;
	    break;
	}
    }
    C_SETINH();			// set Inhibit
    ops += lutclose();
    C_CLRINH();			// clear Inhibit
    C_WRITE(N, 9, 16, 0x10);	// Select memory Vx
    C_WRITE(N, 1, 16, 0); 	// CSR: Stop dataflow to FIFO
    C_READ(N, 0, 1, final.s[0]);	// Read current pointer
    C_READ(N, 1, 1, final.s[1]);
    ops += 7;
    if (tnext == 0xFFFFFFFF) return 0;
    return ops + (final.l - START)/2;
}

int EventRead(int *err) {
    int i, j, k;
    const int modlen[2][4] = {{49, 49, 3, 49}, {49, 49, 4, 49}};
    int ev[4][2];
    int next, nextnext, resyn, ops, toread, tocheck, left;
    
    memset(ev, 0, sizeof(ev));
    *err = 0;
    C_WRITE(N, 9, 16, 0x10); 	// Select memory chip
    C_WRITE(N, 1, 16, (FROPT << 1)); 	// CSR
    CACHEDELAY(CD);
    C_WRITE(N, 0, 17, (START & 0xFFFF));
    C_WRITE(N, 1, 17, ((START>>16) & 0xFFFF));
    CACHEDELAY(CD);
    ops = 4 + 2*CD;
    fcopt(1);
    for ( j=START, left=0; j < STOP-100 && iStop == 0; ) {
	toread = BLKSIZE - left;
	if (j + toread > STOP - 100) toread = STOP - 100 - j - left;
	C_READ_B(N, 0, 0, &RD[left/2], toread/2, FCOPT, FCACC);
	ops += (toread/2);
	if (ANERR) { // Always read twice if error analysis is reqired
	    fcopt(0);
	    CACHEDELAY(CD); // In case we stopped reading at the edge of a page
	    C_WRITE(N, 0, 17, (j & 0xFFFF));
	    C_WRITE(N, 1, 17, ((j>>16) & 0xFFFF));
	    CACHEDELAY(CD); 
	    fcopt(1);
	    C_READ_B(N, 0, 0, &RDD[left/2], toread/2, FCOPT, FCACC);
	    ops += (2*CD + toread/2);
	    for (i=0; i<(left+toread)/2; i++)
		if ((RD[i] & MASK) != (RDD[i] & MASK)) {
		    printf("\nError at %6.6X : read=%4.4X != %4.4X XOR=%4.4X",
			(j-left)+2*i, RD[i], RDD[i], RD[i] ^ RDD[i]);
		    (*err)++;
	    }
	}
	// Analyse next block
	for (i=0,next=0,nextnext=0,resyn=0; i<(left+toread)/2; ) {
	    next = i+((RD[i]&0x8000) ? 2 : modlen[WFDVER - 8][MODECSR & 3]);
	    nextnext = next + ((RD[next]&0x8000) ? 2 : modlen[WFDVER - 8][MODECSR & 3]);
	    if (nextnext >= (left+toread)/2) break;
	    if (((RD[i] & 0x7F3F) == MODECSR) &&
		((RD[next] & 0x7F3F) == MODECSR) &&
		((RD[nextnext] & 0x7F3F) == MODECSR)) {
		    ev[(RD[i] >> 6) & 3][(RD[i] >> 15) & 1]++;
		    if (resyn == 1 && ANERR) 
			printf("\n  Resynchronised at %6.6X",(j-left)+2*i);
		    i = next;
		    resyn = 0;
	    } else {
		if (resyn == 0) {
		    if (ANERR) printf("\nError at %6.6X(BL+%6.6X): CSR seq: %4.4X %4.4X %4.4X expect (&0x7F3F): %4.4X",
			    (j-left)+2*i, 2*i, RD[i], RD[next], RD[nextnext], MODECSR);
		    (*err)++;
		}
		i++;
		resyn = 1;
	    }
	}
	j += BLKSIZE - left;
	// Copy the rest of buffer for the analysis with the next block
	for ( k=0; i<(left+toread)/2; i++, k++) {
	    RD[k] = RD[i];
	    if (ANERR) RDD[k] = RDD[i];
	}
	left = 2*k;
    }
    fcopt(0);
    if (ANERR) {
	printf("\n");
	printf("  Channel:\t0\t\t1\t\t2\t\t3\n");
	printf("  Events:");
	for (i=0; i<4; i++) printf("\t%d\t", ev[i][0]);
	printf("\n  Delims:");
	for (i=0; i<4; i++) printf("\t%d\t", ev[i][1]);
	printf("\n");
    } 
    return ops;
}

void RunTest(void) {
    int i, j, k;
    int errs, ops;
    struct timeval t0, t1;
    double diffw, diffr;
    double s[sizeof(tests)/sizeof(tests[0])][2];
    double s2[sizeof(tests)/sizeof(tests[0])][2];
    int (*TestWrite[])(void) = {&LUTWrite, &DirectWrite, &IndirectWrite, &GCCWrite, &EventWrite};
    int (*TestRead[])(int *) = {&LUTRead, &DirectRead, &IndirectRead, &GCCRead, &EventRead};

    if (WD != NULL) free(WD);
    if (RD != NULL) free(RD);
    if (RDD != NULL) free(RDD);
    WD = (unsigned short *)malloc(BLKSIZE);
    RD = (unsigned short *)malloc(BLKSIZE);
    RDD = (unsigned short *)malloc(BLKSIZE);
    // Fastcamac acceleration for both Write and Read (all modules)
    C_RESET();	// CC32_reset
    C_CLRLAM();	// clear LAM_FF
    C_CLRINH();	// clear Inhibit
    C_READ_L(N, 9, 0, i);	// Test Module Presence
    if (CMC_GetQX() != 3) {
	printf("No WFD module found at CN=%d %d, won't run the tests\n\n", CR, N);
	return;
    }
    C_WRITE(N, 9, 16, 0xF);	// Select ALL Virtexes
    C_READ(N, 0, 9, i);		// Global reset
    CACHEDELAY(1000);
    C_WRITE(N, 8, 16, (EXTCLK & 1));	// External clocks
    fcopt(0);
    fcacc(1);
    iStop = 0;
    signal(SIGINT, termhandler);
    memset(s, 0, sizeof(s));
    memset(s2, 0, sizeof(s2));
    for (i=0; i<LOOP && iStop==0; i++) {
	seed = clock();
	for (j=0, k=0; j<32; j++) k |= (((seed >> j) & 1) << (31 - j));
	seed ^= k;
	srand(seed);
	printf("Pass %d with seed %X (first RAND %X)\n", i, seed, rand());
	for (j=0; j<(int)(sizeof(tests)/sizeof(tests[0])) && iStop==0; j++) {
	    if (((TEST >> j) & 1) == 0) continue;
	    lutclose();
	    // Write
	    srand(seed);
	    printf("\t%s\tWr...", tests[j]);
	    fflush(stdout);
	    gettimeofday(&t0, NULL);
	    ops = TestWrite[j]();
	    if (ops == 0) continue;
	    gettimeofday(&t1, NULL);
	    diffw = ((double)(t1.tv_sec-t0.tv_sec)*1E6 + 
		    (double)(t1.tv_usec-t0.tv_usec))*1000./ops;
	    s[j][0] += diffw;
	    s2[j][0] += diffw*diffw;
	    // Read
	    srand(seed);
	    printf(" Rd...", tests[j]);
	    fflush(stdout);
	    gettimeofday(&t0, NULL);
	    ops = TestRead[j](&errs);
	    gettimeofday(&t1, NULL);
	    diffr = ((double)(t1.tv_sec-t0.tv_sec)*1E6 +
		    (double)(t1.tv_usec-t0.tv_usec))*1000./ops;
	    s[j][1] += diffr;
	    s2[j][1] += diffr*diffr;
	    if (errs != 0 && ANERR != 0) printf("\n");
	    printf(" Errs: %d. Speed: W: %7.2f R: %7.2f ns/op\n",
		errs, diffw, diffr);
	}		
    }
    signal(SIGINT, SIG_DFL);
    if (i > 1) {
	printf(" <Speed>:");
	for (j=0; j<(int)(sizeof(tests)/sizeof(tests[0])); j++)
	    printf(" %5s        ", tests[j]);
	printf("\n  Write:");
	for (j=0; j<(int)(sizeof(tests)/sizeof(tests[0])); j++)
	    printf(" %7.1f(%4.1f)", s[j][0]/i, s2[j][0]/i-s[j][0]*s[j][0]/i/i);
	printf("\n  Read: ");
	for (j=0; j<(int)(sizeof(tests)/sizeof(tests[0])); j++)
	    printf(" %7.1f(%4.1f)", s[j][1]/i, s2[j][1]/i-s[j][1]*s[j][1]/i/i);
	printf("\n");
    }
    fcacc(0);			// No acceleration 
    C_WRITE(N, 8, 16, 0);	// Back to internal clocks;
}

int main(int argc, char **argv)
{
	int i;
	FILE *fcfg = NULL;
	char str[256];

	Camac = -1;

	printf("\nSvL-2004 -- Universal test for WFD modules (LUTs, SDRAM...)\n\n");

	fcfg = fopen("memtest.cfg", "rt");
	if (fcfg == NULL)  
	    printf("WARN-Cannot open configuration file memtest.cfg, using defaults...\n");
	else {
	    for (;;) {
		fgets(str, sizeof(str), fcfg);
		if (feof(fcfg)) break;
		analyse(str);
	    }
	}

	if (argc > 1) N = strtol(argv[1], NULL, 0);
	if (argc > 2) CR = strtol(argv[2], NULL, 0);

	if (Camac < 0 && CR >= 0) { 
	    if (C_INIT(CR) < 0) {
		printf("FATAL-Cannot open crate %d - %d\n", CR, Camac);
		return 1;
	    }
	}

	if (argc > 1) {	// Batch mode
	    printconf();
	    RunTest();
	    return 1;
	}

	analyse(NULL);
	printconf();

	for (;;) {
	    printf("Command>");
	    fflush(stdout);
	    fgets(str, sizeof(str), stdin);
	    i = analyse(str);
	    if (i == -2) break;
	    if (i == -1) { analyse(NULL); continue; }
	    printconf();
	    if (i != 0) continue; 
	    // Main test loop
	    if (CR >= 0) RunTest();
	    else printf("\nWARN-Cannot run test: no crate availiable\n\n");
	}    
	C_CLOSE();
	return 0;
}
