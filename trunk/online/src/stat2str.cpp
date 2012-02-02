#include <string.h>

#include "rhicpol.h"

char * stat2str(int status) {
    static char ststr[80];
    char longnames[32][50]={
	"W-Internal warning, see logfile",
	"W-Cancelled",
	"W-No fill number availiable",
	"W-WFD unreliable",
	"W-CNTS small internal counts inconsistency",
	"","","","","","","","","","","",
	"E-This is not a real measurement, test only",
	"E-Severe internal error, see logfile",
	"E-Serious WFD error",
	"E-Camac access error",
	"E-Configuration file error",
	"E-Can't switch shaper LV PS",
	"E-Can't access RHIC ADO",
	"",
	"E-Can't start measurement script",
	"E-Target control error",
	"E-Device is busy",
	"","","","",""
    };
    char shortnames[32][10]={
	"WInt", "WCan", "WNoFill#", "WWFD",
	"WCNTS","","","","","","","","","","","",
	"ETest", "EIntr", "EWFD","ECAMAC","EConf","ELVPS",
	"EADO", "", "EStart", "ETarg",
	"EBUSY","","","","",""
    };

    int i,j,k;
    // Count active status bits
    for (i=0,j=0,k=-1;i<31;i++) if (((status>>i)&1) == 1) {j++;k=i;}
    if (j==0) {
	strcpy(ststr,"OK");
    } else if (j==1) {
	strcpy(ststr,&longnames[k][0]);
    } else {
	ststr[0]='\0';
	for (i=0;i<31;i++) 
	    if (((status>>i)&1) == 1) {
		strncpy(&ststr[strlen(ststr)], &shortnames[i][0], sizeof(ststr)-strlen(ststr)-1);
		strncpy(&ststr[strlen(ststr)]," ", sizeof(ststr)-strlen(ststr)-1);
	    }
    }
    return ststr;
}
