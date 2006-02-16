#ifndef POLUTIL_H
#define POLUTIL_H
#include "rpoldata.h"

int readConfig(char *fname, int update);
int CheckConfig();
void CreateLookup(float tshift);
int polWrite(recordHeaderStruct *header, long *data);

void camacClose(void);
int camacOpen(void);
void setInhibit(void);
void resetInhibit(void);
void nsleep(double time);

int setRing();
int setClock();
void pulseAllProgs(void);
int initWFDs(void);
void initScalers(void);
int getEvents(int Number);
void readWFD();
void readMemory();
void readScalers();

void closeDataFile(char * comment);
int openDataFile(char *fname, char *comment, int noAdo);

void setAlarm(float mTime);
void clearAlarm(void);
void alarmHandler(int sig);

void getAdoInfo();

void polexit();

void setOutInhibit(void);
void resetOutInhibit(void);
void clearVetoFlipFlop(void);
void fastInitWFDs(void);
void getJetStatus(unsigned short * data);
int testJetVeto(void);
void printJetState(void);
void writeJetStatus(void);

#endif
