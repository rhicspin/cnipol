#include "rhicpol.h"
#include "rpoldata.h"
#include "Asym.h"

//input data files
char datafile[256];
char datafile2[256];

//the number of strips
const unsigned short num_strips=76;
const unsigned short strips_per_detector=12;
const unsigned short num_detectors=6;


//tells whether a strip is alive or not, so we know whether to include it in the dead layer average
bool is_strip_alive[num_strips];

//tells which strips are dead, so that is_strip_alive can be generated
const unsigned short num_dead_strips=1;
short dead_strips[num_dead_strips]={65};

//average deadwidth[which file][which detector]
float deadwidth[2][num_detectors];
float tempwidth[num_detectors];

//whether the dead layers from the two input files are consistent
bool dead_layers_consistent;


int readDLayer(char *infile);
