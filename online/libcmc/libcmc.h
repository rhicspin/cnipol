/************************************************************************
 *   Library for CMC100 CAMAC USB controller with cmcamac driver.	*
 *	D.Svirida & I.Alekseev @itep.ru, 2005				*
 *	file: libcmc.h							*
 ************************************************************************/
#ifndef LIBCMC_H
#define LIBCMC_H

#include <stdio.h>

#define MAXCRATES	8

// Chain commands
#define CMC_CHAINFIRST	0xFFFFFFFF
#define CMC_CHAINSECOND	0

#define CMC_CMDNFA	(0 << 24)
#define CMC_CMDDATA	(1 << 24)
#define CMC_CMDREPEAT	(2 << 24)
#define	CMC_CMDPROGWR	(3 << 24)
#define CMC_CMDJUMP	(4 << 24)
#define CMC_CMDDELAY	(5 << 24)
#define CMC_CMDLOADCNT	(6 << 24)
#define CMC_CMDDECCNT	(7 << 24)
#define CMC_CMDCJUMP	(8 << 24)
#define CMC_CMDLOADLOOP	(9 << 24)
#define CMC_CMDPATCHWR	(10 << 24)
#define CMC_CMDPATCHRD	(11 << 24)
#define CMC_CMDINSERT	(12 << 24)
#define CMC_CMDPROGRD	(13 << 24)
#define CMC_CMDFLUSH	(14 << 24)
#define CMC_CMDLOADACC	(16 << 24)
#define CMC_CMDANDACC	(17 << 24)
#define CMC_CMDXORACC	(18 << 24)
#define CMC_CMDREADACC	(19 << 24)
#define CMC_CMDSTOP	(31 << 24)


#define CMC_FASTNFA(S,M,W,Q,E,L,N,F,A)	((A & 15) | ((F & 31)<<4) | ((N & 31)<<9) | \
			((L & 3)<<14) | ((E & 1)<<16) | ((Q & 1)<<17) | \
			((W & 3)<<18) | ((M & 1)<<20) | ((S & 7)<<21) | CMC_CMDNFA)
#define CMC_STDNFA(N,F,A)	CMC_FASTNFA(4,0,0,0,0,0,N,F,A)


// CMC100 replies
#define CMC_CMASK	0x70000000
#define CMC_LMASK	0x04000000
#define CMC_QMASK	0x02000000
#define CMC_XMASK	0x01000000
#define CMC_DMASK	0x00FFFFFF

typedef struct {
	int *wdata; 	/* chain to write buffer	*/
	int *rdata;	/* read buffer			*/
	int wlen;	/* write buffer size		*/
	int rlen;	/* read buffer size		*/
	int wptr;	/* filled portion of write buffer	*/
	int rptr;	/* really read from device	*/
} CMC_chain;

#ifdef __cplusplus
extern "C" {
#endif

CMC_chain *CMC_AllocateChain(int wlen, int rlen);
void CMC_ReleaseChain(CMC_chain *chain);
void CMC_ResetChain(CMC_chain *chain);
int  CMC_CommitChain(CMC_chain *chain, int C);
int  CMC_Add2Chain(CMC_chain *chain, int cmd);
void CMC_DumpChain(CMC_chain *ch, size_t wc, size_t rc, FILE *f); 
int  CMC_Single(int C, int N, int F, int A, int data);
int  CMC_Open(int C);
void CMC_Close(int C);
int  CMC_Reset(int C);
int  CMC_R1(int C);
int  CMC_GetQX(void);

#ifdef __cplusplus
    }
#endif


/*	Our "standard" calls	*/
extern int Camac;
#define C_READ(N, A, F, D) 	D = CMC_Single(Camac, N, F, A, 0);
#define C_READ_L(N, A, F, D)	C_READ(N, A, F, D);
#define C_WRITE(N, A, F, D) 	CMC_Single(Camac, N, F, A, D);
#define C_WRITE_L(N, A, F, D) 	C_WRITE(N, A, F, D);
#define C_INIT(C)		(Camac = CMC_Open(C))
#define C_CLOSE()		CMC_Close(Camac);
#define C_CLRINH()		CMC_Single(Camac, 30, 24, 9, 0);
#define C_SETINH()		CMC_Single(Camac, 30, 26, 9, 0);
#define C_CLRLAM()		CMC_Single(Camac, 30, 24, 10, 0);
#define C_RESET()		CMC_Single(Camac, 30, 17, 0, 0);
#define C_QX(D)			CMC_GetQX();

#ifdef __cplusplus
extern "C" {
#endif

int C_WRITE_B(int N, int A, int F, unsigned short * Data, int len);
int C_READ_B(int N, int A, int F, unsigned short * Data, int len, char fcopt, int fcacc);

#ifdef __cplusplus
    }
#endif

#endif /* LIBCMC_H */

