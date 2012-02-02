#include <stdio.h>
#include "rcdev.h"
//#include "rpoldata.h"

void getJetPosition(int pos[9])
{
    cdevData data;
    int irc, i;
    FILE *LogFile;
    
    char rbpmName[8][20] = {"rbpm.b-g11-bhx", "rbpm.b-g12-bhx", "rbpm.b-g12-bvx",
	"rbpm.b-g11-bvx", "rbpm.y-g11-bhx", "rbpm.y-g12-bhx", "rbpm.y-g11-bvx", "rbpm.y-g12-bvx"};

    irc = 0;
    LogFile = stdout;
    cdevDevice & dev = cdevDevice::attachRef("stepper.12a-hjet.A.U");
    if(!DEVSEND(dev, "get absolutePosition", NULL, &data, LogFile, irc)) 
	data.get("value", &pos[0]);
    for (i=0; i<8; i++) {
	cdevDevice & rbpm = cdevDevice::attachRef(rbpmName[i]);
	if(!DEVSEND(rbpm, "get avgOrbPositionM", NULL, &data, LogFile, irc)) 
	    data.get("value", &pos[i+1]);
    }
}

int getCdevValue(char *dname, char *sval)
{
    cdevData cdat;
    int irc, cdevval;
    char command[100];
    FILE *aLogFile;

    sprintf(command,"get %s",sval);
    cdevDevice & dev = cdevDevice::attachRef(dname);
    if(!DEVSEND(dev, command, NULL, &cdat, aLogFile, irc)) 
	cdat.get("value", &cdevval);

    return cdevval;
}

int main()
{
    int pos[9];
    
    getJetPosition(pos);
    printf("Jet position is %d counts.\n", pos[0]);
    printf("Yellow beam position at IP12 is X = %5.1f   Y = %5.1f mm.\n", 
	(pos[5]+pos[6])/2000., (pos[7]+pos[8])/2000.);
    printf("Blue beam position at IP12 is X = %5.1f   Y = %5.1f mm.\n", 
	(pos[1]+pos[2])/2000., (pos[3]+pos[4])/2000.);

    printf("Yellow fill number = %d\n", getCdevValue("ringSpec.yellow", "fillNumberM"));
    printf("Blue fill number = %d\n", getCdevValue("ringSpec.blue", "fillNumberM"));
}
