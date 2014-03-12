/************************************************
 *	RHIC polarimeters - 2010		*
 *	I. Alekseev & D. Svirida		*
 *	Send figure with some results to CDEV 	*
 ************************************************/

#include <cdevDevice.h>
#include <cdevData.h>
#include <cdevGroup.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

#define MAXWAIT 30

int main(int argc, char **argv)
{
    int i;
    cdevData data;
    cdevGroup group;
    BYTE *ptr;
    int len;
    int file;
    int tm;
    int irc;
    char cmd[256];

    printf("Starting sndpic...\n");

    if (argc != 4) {
	printf("SNDPIC-Usage: sndpic device param picfile.gif\n");
	return -10; 
    }
    
    file = open(argv[3], O_RDONLY);
    if (file <= 0) {
	printf("SNDPIC-Can not open file %s : %s\n", argv[3], strerror(errno));
	return -errno;
    }
    
    len = lseek(file, 0, SEEK_END);
    ptr = (BYTE *) malloc(len);
    if (ptr == NULL) {
	printf("SNDPIC-Can not alloc %d bytes : %s\n", len, strerror(errno));    
	close(file);
	return -20;
    }
    lseek(file, 0, SEEK_SET);
    len = read(file, ptr, len);

    cdevData::addTag((char *)"timeStamp");
    cdevDevice & pol = cdevDevice::attachRef(argv[1]);

    tm = time(NULL);
    data.insert((char *)"timeStamp", tm);
    data.insert((char *)"value", ptr, len);

    sprintf(cmd, "set %s", argv[2]);

    group.start();
    irc = pol.sendNoBlock(cmd, &data, NULL);
    group.end();
    if (irc != 0) printf("SNDPIC - CDEV Error %d\n",irc);

    for (i=0; i<MAXWAIT; i++) {
	group.pend(1.0);
	if (group.allFinished()) {
	    printf("SNDPIC - Finished took %d seconds to send. Time stamp = %d\n", i, tm);
	    irc = 0;
	    break;
	}
    }
    
    if (i == MAXWAIT) {
	 printf("SNDPIC-Error picture was not sent in %d seconds. Time stamp = %d\n", i,tm);
	 irc = -1;
    }

    free(ptr);
    close(file);
    return irc;
}
