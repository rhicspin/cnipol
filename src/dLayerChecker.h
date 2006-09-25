#include "rhicpol.h"
#include "rpoldata.h"
#include "Asym.h"
#include "AsymMain.h"
#include "AsymRunDB.h"

char *datadir;
char *sharedir;
char *tmpdir;
char ofile[256];

int fitresult;

bool runfit;


//input data files
char configfile[256];
char configfile2[256];

char runid[32];
char runid2[32];



recordConfigRhicStruct  cfginfo;

//the number of strips
const unsigned short num_strips=72;
const unsigned short strips_per_detector=12;
const unsigned short num_detectors=6;

const float chi2max=300.;
const float dlayer_consistency=3.; // [ug/cm2]

//average deadwidth[which file][which detector]
float deadwidth[2][num_detectors];
float tempwidth[num_detectors];

//whether the dead layers from the two input files are consistent
bool dead_layers_consistent;




void Usage(char* argv[]);
void getPreviousRun(bool thisrun=false); //if thisrun is true, then look at current run instead of previous
int readDLayer(char *infile);
bool isStripAlive(unsigned short strp);
void checkChi2(char *infile);
