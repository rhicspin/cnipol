#ifndef globals_h
#define globals_h

//#include "rpoldata.h"

extern char myColor[2][20];
//extern char polCDEVName[2][20];
extern char polCDEVName[4][20];
extern char specCDEVName[2][20];
extern char bucketCDEVName[2][20];
extern char targetCDEVName[4][30];
extern char muxCDEVName[2][20];

extern "C" {
	//void sendRunIdS(int N);
   //int  getJetBits(void);
   ////void UpdateStatus(void);
   //void getJetPosition(void);
   //int  getTargetMovementInfo(long **data);
   //void getCarbTarg(carbTargStat * targstat);
   //void getWcmInfo(void);
   //void getAdoInfo(char mode);
   //void GetTargetEncodings(long *res);
   //void UpdateProgress(int evDone, int rate, double ts);
   //void UpdateMessage(char * msg);
   int icopen_(char *fname, int *len, int dummy);
   int readandfill_(int* subrun);
   void icclose_(void);
   void wfana(unsigned char *data, int *len, int *chan);
   void mybook_(int *num);
   void histrate_(long *data, long *num);
   void histdelim_(long *data, long *num);
   void histtagmov_(long *data, long *num);
   void wfana_(float *data, int *len, int *chan);
}

#endif
