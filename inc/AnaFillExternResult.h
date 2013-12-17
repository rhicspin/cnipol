
#ifndef AnaFillExternResult_h
#define AnaFillExternResult_h

#include <map>
#include <time.h>

#include "TObject.h"
#include "TGraphErrors.h"

#include "AsymCommon.h"


class AnaFillResult;


/** */
class AnaFillExternResult : public TObject
{
public:
  friend class AnaFillResult;

protected:

   TGraphErrors *fBluIntensGraph;
   TGraphErrors *fYelIntensGraph;
   TGraphErrors *fBluRotCurStarGraph;
   TGraphErrors *fYelRotCurStarGraph;
   TGraphErrors *fBluRotCurPhenixGraph;
   TGraphErrors *fYelRotCurPhenixGraph;
   TGraphErrors *fBluSnakeCurGraph;
   TGraphErrors *fYelSnakeCurGraph;
   time_t        fTimeEventLumiOn;
   time_t        fTimeEventLumiOff;

public:

protected:

   void ReadInfo(std::ifstream &file);

public:

   AnaFillExternResult();
   AnaFillExternResult(std::ifstream &file);
   ~AnaFillExternResult();

   TGraphErrors* GetGrBluIntens() const;
   TGraphErrors* GetGrYelIntens() const;
   time_t        GetLumiOnTime()  const { return fTimeEventLumiOn; }
   time_t        GetLumiOffTime() const { return fTimeEventLumiOff; }

   void Print(const Option_t* opt="") const;
   //void PrintAsPhp(FILE *f=stdout) const;

   void Process();

   ClassDef(AnaFillExternResult, 1)
};

#endif
