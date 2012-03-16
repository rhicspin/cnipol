/* functions to get spin and fill patterns from CDEV
 */
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include "rcdev.h"

typedef struct {
    short int cdevFillPattern[360];
    short int cdevSpinPattern[360];    
} patternDataStruct;

extern patternDataStruct patternData;

extern "C" {
    void getSpin(int ring);
}

void getSpin(int ring) {

  char bucketsCDEVName[2][20] = {"buckets.yellow", "buckets.blue"};
  int irc=0;
  cdevData data;

  cdevDevice & buckets =  cdevDevice::attachRef(bucketsCDEVName[ring]);

  if(!DEVSEND(buckets, "get polarizationFillPatternS", NULL, &data, stdout, irc)) {
    data.get("value", patternData.cdevSpinPattern);
    //	It looks that they changed type to char ...
    for (int i=0; i<360; i++) if (patternData.cdevSpinPattern[i] > 127)
      patternData.cdevSpinPattern[i] -= 256;
  }

    if (irc != 0) printf("%d errors getting Spin Pattern from CDEV.\n", irc);
}

extern "C" {
    void getFill(int ring);
}

void getFill(int ring) {

  char bucketsCDEVName[2][20] = {"buckets.yellow", "buckets.blue"};
  int irc=0;
  cdevData data;

  cdevDevice & buckets =  cdevDevice::attachRef(bucketsCDEVName[ring]);

  if(!DEVSEND(buckets, "get measuredFillPatternM", NULL, &data, stdout, irc))
    data.get("value", patternData.cdevFillPattern);    

    if (irc != 0) printf("%d errors getting Fill Pattern from CDEV.\n", irc);
}
