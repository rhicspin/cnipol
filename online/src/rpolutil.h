#ifndef POLUTIL_H
#define POLUTIL_H
#include "rpoldata.h"

int readConfig(char *fname, int update);
int CheckConfig(void);
void CreateLookup(float tshift);
int polWrite(recordHeaderStruct *header, long *data);

void camacClose(void);
int camacOpen(void);
void setInhibit(void);
void resetInhibit(void);
void nsleep(double time);

int setRing(void);
void pulseAllProgs(void);
int initWFDs(void);
void initScalers(void);
int getNumberOfEvents(void);
int getEvents(int Number);
void readWFD(void);
void readMemory(void);
void readScalers(void);
long long totalSum(long *data, int len);

void closeDataFile(char * comment);
int openDataFile(char *fname, char *comment, int noAdo);

void setAlarm(float mTime);
void clearAlarm(void);
void alarmHandler(int sig);

void getAdoInfo(void);
void getWcmInfo(void);
void GetTargetEncodings(long *res);
void UpdateProgress(int evDone, int rate);
void UpdateStatus(void);
void UpdateMessage(char *message);

void polexit(void);

void setOutInhibit(void);
void resetOutInhibit(void);
void clearVetoFlipFlop(void);
void fastInitWFDs(void);
void getJetStatus(unsigned short * data);
int testJetVeto(void);
static char * getJetStatusString(void);
void writeJetStatus(void);

#endif
