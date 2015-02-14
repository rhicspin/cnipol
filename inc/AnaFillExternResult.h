
#ifndef AnaFillExternResult_h
#define AnaFillExternResult_h

#include <map>
#include <vector>
#include <time.h>

#include "TObject.h"
#include "TGraphErrors.h"

#include <opencdev.h>

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
   std::map< EPolarimeterId, std::vector<TGraphErrors*> > fBCCurGraph;
   time_t        fTimeEventLumiOn;
   time_t        fTimeEventLumiOff;

   TGraphErrors* MakeGraph(const std::map<opencdev::cdev_time_t, double> &values);
   void LoadInfo(UInt_t fillId);

public:

   AnaFillExternResult(UInt_t fillId);
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
