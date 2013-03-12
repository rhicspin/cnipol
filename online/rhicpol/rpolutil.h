#ifndef POLUTIL_H
#define POLUTIL_H

#include "rcdev.h"
#include "globals.h"
#include "rpoldata.h"

#ifdef __cplusplus
    extern "C" {
#endif

//	Config functions (rpolutil.c)
int  readConfig(char *fname, int update);
int  CheckConfig(void);
void CreateLookup(float tshift);

//	Data file write (rpolutil.c)
int  polWrite(recordHeaderStruct *header, long *data);
void closeDataFile(char * comment);
int  openDataFile(const char *fname, char *comment, bool useCDEV);
void writeSubrun(int N);

//	Camac & hardware functions (rpolutil.c)
void camacClose(void);
int  camacOpen(void);
void setInhibit(void);
void resetInhibit(void);
int  setOutReg(void);
void pulseAllProgs(void);
int  initWFDs(void);
void fastInitWFDs(int clr_hist);
void initScalers(void);
int  getNumberOfEvents(void);
int  getEvents(int Number);
void readWFD(void);
void readMemory(void);
void readScalers(void);

//	General functions (rpolutil.c)
void  nsleep(double time);
long long totalSum(long *data, int len);
void  setAlarm(float mTime);
void  clearAlarm(void);
void  alarmHandler(int sig);
void  polexit(void);
char* stat2str(int stat);

//	Jet functions	(rpolutil.c)
void  setOutInhibit(void);
void  resetOutInhibit(void);
void  clearVetoFlipFlop(void);
void  getJetStatus(unsigned short * data);
int   testJetVeto(void);
char* getJetStatusString(void);
void  writeJetStatus(void);
int   getJetBits(void);
int   testCarbTarg(void);
void  writeCarbTarg(void);

//	CDEV functions	(rpolutilp.cpp)
void      getJetPosition(void);
int       getTargetMovementInfo(long **data);
void      getCdevInfo();
void      getCdevInfoMachineParams();
void      getCdevInfoTargetParams(cdevDevice &polDev);
void      getCdevInfoBeamData(beamDataStruct *bds);
void      getWcmInfo();
EMeasType getCDEVMeasType();
int       getVoltage();
void      getCarbTarg(carbTargStat * targstat);
void      GetTargetEncodings(long *res);
void      UpdateProgress(int evDone, int rate, double ts);
void      UpdateStatus(void);
void      UpdateMessage(char *message);
void      ProgV124(int ring);

#ifdef __cplusplus
    }
#endif

#endif
