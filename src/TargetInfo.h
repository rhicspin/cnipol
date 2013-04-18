#ifndef TargetInfo_h
#define TargetInfo_h

#include "AsymCommon.h"


/** */
class TargetInfo
{

public:

   float  x;                    // (arbitarary) target postion [mm]
   int    vector;
   long   eventID;
   int    VHtarget;             // Vertical:[0], Horizontal:[1]
   int    Index[MAXDELIM];
   int    Linear[MAXDELIM][2];   
   int    Rotary[MAXDELIM][2];
   float  X[MAXDELIM];          // target position [mm] array excluding static position 
   float  Interval[MAXDELIM];   // time interval of given target postiion [sec]
   float  Time[MAXDELIM];       // duration from measurement start in [sec]
   struct StructAll {
      float x[TARGETINDEX];    // target position in [mm] including static position
   } all;

   UInt_t fNDelim;

public:
   TargetInfo();
   ~TargetInfo();
   void Print(const Option_t* opt="") const;
};

#endif
