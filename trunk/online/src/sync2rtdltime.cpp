#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include "rcdev.h"

//--------------------------------------- getRTDLtime

time_t getRTDLtime(void)
{
    cdevData data;
    unsigned long secnec[2];
    int irc;

    cdevDevice & dev = cdevDevice::attachRef("fecSystem.12a-polar1");
    
    irc = 0;
    DEVSEND(dev, "get currentTime", NULL, data, stdout, irc);
    if (irc != 0) return 0;
    data.get("value", &secnec[0]);
    return ((time_t)secnec[0]);
}

int main(int argc, char** argv)
{
    time_t now_t;
    time_t linux_t;
    time_t nlinux_t;
    char sss[80];

    now_t = getRTDLtime();
    if (now_t == 0) return -20;
    time(&linux_t);
    strncpy(sss,ctime(&now_t),sizeof(sss));
    sss[strlen(sss)-1]='\0';
    if (stime(&now_t)) {
	printf("Cannot syncronize time: %s\nCurrent difference: %ld sec.\n", 
	    strerror(errno), linux_t - now_t);
	return -10;
    }
    time(&nlinux_t);
    printf("RTDL time: %s; Difference: %ld sec.; New LINUX time: %s", 
	sss, linux_t - now_t, ctime(&nlinux_t));
    return 0;
}
