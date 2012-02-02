/************************************************************************
 *   Manual CAMAC controller for CMC100 CAMAC USB controller with cmcamac driver.	*
 *	D.Svirida & I.Alekseev @itep.ru, 2005-10			*
 *	file: burcmc.c							*
 ************************************************************************/
#include <ctype.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
// Added #include <stdlib.h> Nov. 17, 2008
#include <stdlib.h>
#include "../libcmc/libcmc.h"

int CircleStop = 0;

void sig_quit_handler(int sig)
{
    CircleStop = 1;
}

int Begin(int num)
{
	signal(SIGQUIT, sig_quit_handler);
	signal(SIGINT, sig_quit_handler);
	Camac = CMC_Open(num);
	return (Camac < 0);
}

void End(void)
{
	CMC_Close(Camac);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGINT, SIG_DFL);
}

void CamacFun(int N, int A, int F, int *D, int *Q, int *X)
{
    int status;
    int qx;
    status = CMC_Single(Camac, N, F, A, *D);
    if (status >= 0) {
	if (F<8) *D = status & 0xFFFFFF;
	qx = CMC_GetQX();
	*Q = 1 & (qx >> 1);
	*X = 1 & qx;
    } else {
	if (F<8) *D = 0;
	*Q = 0;
	*X = 0;
    }
}

int main(int argc, char **argv)
{
    char cmd[80];
    const char delim[] = "\t\n =:";
    char *com;
    int N, A, F, D, k, i, Q, X;
    int Help;
    int num;
    
    num = (argc > 1) ? strtol(argv[1], NULL, 0) : 1;
    
    if (Begin(num) != 0) {
	printf("Can not open CAMAC #%d. Try another crate: burcmc <crate_number>. Exitting....\n", num);
	End();
	exit(0);
    }
    N = 1;
    A = 0;
    F = 0;
    D = 0;
    for (;;) {
	Help = 0;
	k = 1;
	printf("CAMAC>"); 
	fgets(cmd, sizeof(cmd), stdin);
	com = strtok(cmd, delim);
	for(;;) {
	    if (com == NULL) break;
	    switch (toupper(com[0])) {
	    case 'E':
	    case 'Q':
	    case 'X':
		goto quit;
	    case 'C':
		k = strtol(&com[1], NULL, 0);
		if (k == 0) k = 999999999;
		break;
	    case 'N':
		N = strtol(&com[1], NULL, 0);
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
	    printf("Q, E, X - exit.\n");
	    printf("Startup: bur [num]. num - selects crate number.\n");
	} else {
	    CircleStop = 0;
	    for (i=0; i<k; i++) {
		if (CircleStop) break;
		CamacFun(N, A, F, &D, &Q, &X);
		if (i == 0) {
		    if (F<8) {
			printf("N%d A%d F%d => R0x%X Q(%d) X(%d)", N, A, F, D, Q, X);
		    } else if (F>15 && F<24) {
			printf("N%d A%d F%d W0x%X => Q(%d) X(%d)", N, A, F, D, Q, X);
		    } else {
			printf("N%d A%d F%d => Q(%d) X(%d)", N, A, F, Q, X);		    
		    }
		    if (k>1) printf(" repeat=%d", k);
		    printf("\n");
		}
	    }
	}
    }
quit:
    End();
}
