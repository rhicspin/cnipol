#ifndef globals_h
#define globals_h


enum EMeasType {kMEASTYPE_UNKNOWN     = 0x00000000,
                kMEASTYPE_UNDEF       = 0x11111111,
                kMEASTYPE_TEST        = 0x00010000,
                kMEASTYPE_ALPHA       = 0x00000001,
                kMEASTYPE_SWEEP       = 0x00000002,  // profile sweep
                kMEASTYPE_FIXED       = 0x00000004,
                kMEASTYPE_RAMP        = 0x00000008,
                kMEASTYPE_EMIT_SCAN   = 0x00000010,
                kMEASTYPE_TARGET_SCAN = 0x00000020,
                kMEASTYPE_PROFILE_T   = 0x00000040,  // profile by time
                kMEASTYPE_PROFILE_E   = 0x00000080}; // profile events

extern char polCDEVName[4][20];

extern EMeasType gMeasType;   // measurement type must be provided

extern int theVoltage_beg;
extern int theVoltage_end;

#endif
