#ifndef RCDEV_H
#define RCDEV_H

#include <cdevDevice.h>
#include <cdevData.h>

#define DEVSEND(DEV, STR, IN, OUT, LOG, IRC)    ({\
    int jjjjj, iiiii;                            \
    for (jjjjj = 0; jjjjj<5; jjjjj++) {          \
        iiiii = DEV.send(STR, IN, OUT);          \
        if (iiiii == CDEV_SUCCESS) break;        \
    }                                            \
    if (iiiii != CDEV_SUCCESS) {                 \
        fprintf(LOG, "CDEV request %s failed with error code %d(0x%X)\n", STR, iiiii, iiiii);\
        IRC ++;                                  \
    }                                            \
    iiiii;                                       \
})

#endif /* RCDEV_H */
