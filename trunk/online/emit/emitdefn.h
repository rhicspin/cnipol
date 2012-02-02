#ifndef EMITDEFN_H
#define EMITDEFN_H

#define MAXMTIME 600  
#define MAXBUNCH 120  
#define MINFITEVT 100

#define MAXSTEPS 100000
#define BINSIZE 10
#define MMPERSTEP 1.366680E-03	// 731.7 mm/step

// Crude cut limits - rectangular cuts
#define EMIN 400.
#define EMAX 600.
#define TOFMIN 30.
#define TOFMAX 70.

typedef struct {
    float peak;
    float width;
    float relwidth;
    float peaktoEvt;
    float peakb[360];
    float widthb[360];
    float relwidthb[360];
    float peaktoEvtb[360];
} emitDataStruct;

#endif /* EMITDEFN_H */
