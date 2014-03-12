/************************************************************************
 *   Library for CMC100 CAMAC USB controller with cmcamac driver.       *
 *      D.Svirida & I.Alekseev @itep.ru, 2005-10                        *
 *      file: libcmc.c                                                  *
 ************************************************************************/

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
// Added #include <stdlib.h> and <string.h> Nov. 17, 2008
#include <stdlib.h>
#include <string.h>
#include "libcmc.h"

#define CMC_CTL_RESET   _IO('Z', 0)
#define CMC_CTL_R1      _IO('Z', 1)

int __Crates[MAXCRATES] = {-1, -1, -1, -1, -1, -1, -1, -1};
int __QX;

/* Open Crate number C, return crate number on success, negative on error       */
int CMC_Open(int C)
{
   char name[256];
   if (C < 0 || C >= MAXCRATES) return -EINVAL;
   CMC_Close(C);
   sprintf(name, "/dev/usb/cmcamac%d", C);
   __Crates[C] = open(name, O_RDWR);
   if (__Crates[C] < 0) {
       sprintf(name, "/dev/cmcamac%d", C);
       __Crates[C] = open(name, O_RDWR);
       if (__Crates[C] < 0)
       {
           __Crates[C] = -1;
           return -errno;
       }
   }
   return C;
}

/* Close Crate number C */
void CMC_Close(int C)
{
   if (C < 0 || C >= MAXCRATES) return;
   if (__Crates[C] >= 0) close(__Crates[C]);
   __Crates[C] = -1;
}

int CMC_Reset(int C)
{
   if (C < 0 || C >= MAXCRATES || __Crates[C] < 0) return -ENODEV;
   return ioctl(__Crates[C], CMC_CTL_RESET);
}

int CMC_R1(int C)
{
   if (C < 0 || C >= MAXCRATES || __Crates[C] < 0) return -ENODEV;
   return ioctl(__Crates[C], CMC_CTL_R1);
}


/**
 * Allocates a new chain and read/write buffers. Return new chain on success,
 * NULL on failure
 */
CMC_chain *CMC_AllocateChain(int wlen, int rlen)
{
   CMC_chain *chain;

   chain = (CMC_chain *)malloc(sizeof(CMC_chain));
   if (chain == NULL) return NULL;
   memset(chain, 0, sizeof(CMC_chain));

   if (wlen < 3) wlen = 1024;
   chain->wdata = (int *)malloc(wlen*sizeof(int));
   if (chain->wdata == NULL) {
       free(chain);
       return NULL;
   }
   chain->wlen = wlen;

   if (rlen < 1) rlen = 1;
   chain->rdata = (int *)malloc(rlen*sizeof(int));
   if (chain->rdata == NULL) {
       free(chain->wdata);
       free(chain);
       return NULL;
   }
   chain->rlen = rlen;

   chain->wdata[0] = CMC_CHAINFIRST;
   chain->wdata[1] = CMC_CHAINSECOND;
   chain->wptr = 2;
   chain->rptr = 0;

   return chain;
}


/* Free space from the chain    */
void CMC_ReleaseChain(CMC_chain *chain)
{
   if (chain == NULL) return;
   if (chain->wdata != NULL) free(chain->wdata);
   if (chain->rdata != NULL) free(chain->rdata);
   free(chain);
}


/* Prepares the same chain for reuse    */
void CMC_ResetChain(CMC_chain *chain)
{
   if (chain == NULL) return;
   chain->wdata[0] = CMC_CHAINFIRST;
   chain->wdata[1] = CMC_CHAINSECOND;
   chain->wptr = 2;
   chain->rptr = 0;
}


/* Adds another command to the chain. Reallocates the wdata segment if te chain becomes too long */
int CMC_Add2Chain(CMC_chain *chain, int cmd)
{
   if (chain == NULL) return -ENOMEM;
   // Reallocate if necessary
   if (chain->wptr >= chain->wlen) {
       int *wd = (int *) realloc(chain->wdata, 2*chain->wlen*sizeof(int));
       if (wd == NULL) {
           CMC_ReleaseChain(chain);
           return -ENOMEM;
       }
       chain->wlen *= 2;
       chain->wdata = wd;
   }
   chain->wdata[chain->wptr] = cmd;
   chain->wptr++;
   return chain->wptr;
}


/*      Print chain contents (for debugging)    */
void CMC_DumpChain(CMC_chain *ch, size_t wc, size_t rc, FILE *f)
{
   int i, l;
   fprintf(f, "\n\t\t============ CMC100 chain dump ============\n");
   if (ch == NULL) {
       fprintf(f, "NULL chain.\n");
       return;
   }
   if (ch->wdata == NULL || ch->wlen <= 0) {
       fprintf(f, "Strange: no write buffer.\n");
   } else {
       fprintf(f, "Write pointer = %d.\n", ch->wptr);
       l = (ch->wptr > wc) ? wc : ch->wptr;
       for(i=0; i<l; i++) {
           fprintf(f, "%8.8lX ", ch->wdata[i]);
           if ((i & 7) == 7) fprintf(f, "\n");
       }
       if (i & 7) fprintf(f, "\n");
   }
   if (ch->rdata == NULL || ch->rlen <= 0) {
       fprintf(f, "Strange: no read buffer.\n");
   } else {
       fprintf(f, "Read pointer = %d.\n", ch->rptr);
       l = (ch->rptr > rc) ? rc : ch->rptr;
       for(i=0; i<l; i++) {
           fprintf(f, "%8.8lX ", ch->rdata[i]);
           if ((i & 7) == 7) fprintf(f, "\n");
       }
       if (i & 7) fprintf(f, "\n");
   }
}


/* Executes the chain by writing it to the contoller and accepts the data in the reply */
int CMC_CommitChain(CMC_chain *chain, int C)
{
    int retval;
    if (__Crates[C] < 0) return -ENODEV;

    // Add PKTEND
    if ((retval = CMC_Add2Chain(chain, CMC_CMDFLUSH)) < 0) return retval;

    // Write the chain itself
    retval = write(__Crates[C], chain->wdata, sizeof(int)*chain->wptr);
    chain->wptr--;

    //fflush(Crates[C]);
    if (retval != (int) (sizeof(int)*(chain->wptr+1))) return -errno;     /* remove PKTEND */

    // Read the result (exactly one record)
    retval = read(__Crates[C], chain->rdata, sizeof(int)*chain->rlen);

    if (retval <= 0) {
        chain->rptr = 0;
        return -errno;
    }

    retval /= sizeof(int);
    chain->rptr = (retval > (int) chain->rlen) ? chain->rlen : retval;
    return retval;
}


/**
 * Executes one single CAMAC operation. Normally returns data[23:0] read if F<8
 * or 0 otherwise. Updates QX global variable. Takes a huge amount of time for
 * a single CAMAC operation
 */
int CMC_Single(int C, int N, int F, int A, int data)
{
    CMC_chain * ch;
    int retval;

    __QX = 0;
    if (C < 0 || C >= MAXCRATES) return -EINVAL;
    if (__Crates[C] < 0) {
        if ((retval = CMC_Open(C)) < 0) return retval;
    }
    if (!(ch = CMC_AllocateChain(0,0))) return -ENOMEM;
    if (F > 15 && F < 24) {
        if ((retval = CMC_Add2Chain(ch, CMC_CMDDATA | (data & 0xFFFFFF))) < 0) {
            CMC_ReleaseChain(ch);
            return retval;
        }
    }
    if ((retval = CMC_Add2Chain(ch, CMC_STDNFA(N,F,A))) < 0) {
        CMC_ReleaseChain(ch);
        return retval;
    }
    if ((retval = CMC_CommitChain(ch, C)) <= 0) {
        CMC_ReleaseChain(ch);
        return retval;
    }
    retval = (F < 8) ? (ch->rdata[0] & 0xFFFFFF) : 0;
    __QX = (ch->rdata[0] >> 24) & 3;
    CMC_ReleaseChain(ch);
    return retval;
}


int CMC_GetQX(void)
{
    return __QX;
}


/*      Our "standard" calls    */
int Camac = -1;

int C_WRITE_B(int N, int A, int F, unsigned short * Data, int len)
{
    CMC_chain * ch;
    int retval;
    int i;

    if (Camac < 0 || len < 0) return -EINVAL;
    __QX = 0;
    if (len == 0) return 0;
    if (Camac < 0) return -EINVAL;
    if (!(ch = CMC_AllocateChain(2*len+3, 0))) return -ENOMEM;
    for (i=0; i<len; i++) {
        if ((retval = CMC_Add2Chain(ch, CMC_CMDDATA | Data[i])) < 0) {
            CMC_ReleaseChain(ch);
            return retval;
        }
        if ((retval = CMC_Add2Chain(ch, CMC_STDNFA(N,(F | 0x10),A))) < 0) {
            CMC_ReleaseChain(ch);
            return retval;
        }
    }
    if ((retval = CMC_CommitChain(ch, Camac)) <= 0) {
        CMC_ReleaseChain(ch);
        return retval;
    }
    __QX = (ch->rdata[0] >> 24) & 3;
    CMC_ReleaseChain(ch);
    return len;
}

int C_READ_B(int N, int A, int F, unsigned short * Data, int len, char fcopt, int fcacc)
{
    CMC_chain * ch;
    int retval;
    int i;

    if (Camac < 0 || len < 0) return -EINVAL;
    __QX = 0;
    if (len == 0) return 0;
    if (!(ch = CMC_AllocateChain(0,len))) return -ENOMEM;

    i = 0;
    if (fcopt == 'A') i = 1;
    if (fcopt == 'D') i = 2;

    if (i == 1 && (retval = CMC_Add2Chain(ch, CMC_CMDLOADLOOP | (len & 0xFFFFF))) < 0) {
        CMC_ReleaseChain(ch);
        return retval;
    }

    if (i == 2 && (retval = CMC_Add2Chain(ch, CMC_CMDLOADLOOP | (((len+1)/2) & 0xFFFFF))) < 0) {
        CMC_ReleaseChain(ch);
        return retval;
    }

    if ((retval = CMC_Add2Chain(ch, CMC_FASTNFA(fcacc, 0, 0, 0, (i == 2) ? 1 : 0,
        i, N, F, A))) < 0) {
            CMC_ReleaseChain(ch);
            return retval;
    }

    if (i == 0 && (retval = CMC_Add2Chain(ch, CMC_CMDREPEAT | ((len - 1)&0xFFFFF))) < 0) {
            CMC_ReleaseChain(ch);
            return retval;
    }

    if (i == 2 && (len & 1) == 0 && (retval = CMC_Add2Chain(ch,
            CMC_FASTNFA(fcacc, 0, 0, 0, 0, 0, N, F, A))) < 0) {
            CMC_ReleaseChain(ch);
            return retval;
    }

    if ((retval = CMC_CommitChain(ch, Camac)) <= 0) {
        CMC_ReleaseChain(ch);
        return retval;
    }

    __QX = (ch->rdata[retval-1] >> 24) & 3;
    for (i=0; i<retval;i++) Data[i] = 0xFFFF & ch->rdata[i];

    CMC_ReleaseChain(ch);
    return retval;
}
