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
    char polCDEVName[2][20] = {"polarimeter.yel", "polarimeter.blu"};
    int N;
    int i;
    cdevData data;
    cdevGroup group;
    BYTE *ptr;
    int len;
    int file;
    int tm;

    if (argc != 3) {
	printf("SNDPIC-Usage: sndpic y|b picfile.gif\n");
	return -10; 
    }
    
    switch (tolower(argv[1][0])) {
    case 'y' :
	N = 0;
	break;
    case 'b' :
	N = 1;
	break;
    default:
	printf("SNDPIC-Ring must be yellow or blue - not %s\n", argv[1]);
	return -20;
    }
    
    file = open(argv[2], O_RDONLY);
    if (file <= 0) {
	printf("SNDPIC-Can not open file %s : %s\n", argv[2], strerror(errno));
	return -errno;
    }
    
    len = lseek(file, 0, SEEK_END);
    ptr = (BYTE *) malloc(len);
    if (ptr == NULL) {
	printf("SNDPIC-Can not alloc %d bytes : %s\n", len, strerror(errno));    
	close(file);
    }
    lseek(file, 0, SEEK_SET);
    len = read(file, ptr, len);

    cdevData::addTag("timeStamp");
    cdevDevice & pol = cdevDevice::attachRef(polCDEVName[N]);
        

    tm = time(NULL);
    data.insert("timeStamp", tm);
    data.insert("value", ptr, len);

    group.start();
    pol.sendNoBlock("set plotData", &data, NULL);
    group.end();
    
    pol.sendNoBlock("set plotData", &data, NULL);
    for (i=0; i<MAXWAIT; i++) {
	group.pend(1.0);
	if (group.allFinished()) break;
    }    
    if (i == MAXWAIT) printf("SNDPIC-Error picture was not send in %d seconds\n", i);
    
    free(ptr);
    close(file);
    return 0;
}
