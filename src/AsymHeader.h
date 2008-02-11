

#define RHIC_REVOLUTION_FREQ 78e3  // RHIC Revolution Frequency 78 [kHz]
#define RHIC_MAX_FILL 120          // RHIC Maximum Fill Bunches
#define RAMPTIME 350               // duration of the ramp measurement (sec)
#define MAXDELIM 410               // maximum number of delimiter (dynamic motion only)
#define SEC_PER_DELIM 1            // second per deliminter
#define TARGETINDEX 1000           // maximum target full array size including static motion
#define NTBIN 14                   // number of -t bin

#define NSTRIP 72                  // Number of strip channels
#define NSTRIP_PER_DETECTOR 12     // Number of strips per detector
#define NTGTWFD 4                  // Number of target WFD channels
#define NDETECTOR 6                // Number of detectors
#define NBUNCH 120                 // Maximum bunch number
#define R2D 57.29577951            // [rad] -> [degree]
#define MHz 1e-6                   // [Hz]  -> [MHz]
#define G2k 1e6                    // [GeV] -> [keV]
#define k2G 1e-6                   // [keV] -> [GeV]
#define MASS_12C 11.187e6          // Mass Carbon in [keV]
#define C_CMNS 29.98               // Speed of Light in [cm/ns]
const int TGT_OPERATION=6;         // if nTgtIndex>TGT_OPERATION, then "scan", otherwise "fixed"
const int TOT_WFD_CH=NSTRIP+NTGTWFD;//Total WFD channels including target WFD channels.
