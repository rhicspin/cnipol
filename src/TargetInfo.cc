#include "TargetInfo.h"


/** */
TargetInfo::TargetInfo() : x(0), vector(0), eventID(0), VHtarget(-1), fNDelim(0)
{
   for (int i=0; i!=MAXDELIM; i++) {
      Index[i] = 0;

      for (int j=0; j!=2; j++) {
         Linear[i][j] = 0;
         Rotary[i][j] = 0;
      }

      X[i] = 0;
      Interval[i] = 0;
      Time[i] = 0;
   }

   for (int i=0; i!=TARGETINDEX; i++) {
      all.x[i] = 0;
   }
}


/** */
TargetInfo::~TargetInfo() { }


/** */
void TargetInfo::Print(const Option_t* opt) const
{
   printf("TargetInfo::Print()\n");
   printf("fNDelim: %d\n", fNDelim);
   printf("\n%5s %10s %10s %10s %10s %13s %13s %13s\n\n",
      "Index", "Linear", "Linear", "Rotary", "Rotary", "X", "Interval",
      "Time");

   for (UInt_t i=0; i!=fNDelim; i++) {
      printf("%5d %10d %10d %10d %10d %13.3f %13.3f %13.3f\n",
         Index[i], Linear[i][0], Linear[i][1], Rotary[i][0], Rotary[i][1],
         X[i], Interval[i], Time[i]);
   }
}
