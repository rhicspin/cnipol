
#ifndef AnaFillExternResult_h
#define AnaFillExternResult_h

#include <map>
#include <vector>
#include <time.h>

#include "TObject.h"
#include "TGraphErrors.h"
#include "TLine.h"

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
   TGraphErrors *fAgsPolFitGraph;
   std::vector<TLine*> fKickerLines;
   time_t        fTimeEventLumiOn;
   time_t        fTimeEventLumiOff;

   TGraphErrors* MakeGraph(const std::map<opencdev::cdev_time_t, double> &values, int thin_out_factor=1);
   void LoadAgsInfo(opencdev::LogReader &log_reader);
   void LoadInfo(UInt_t fillId);

public:

   AnaFillExternResult() {};
   AnaFillExternResult(UInt_t fillId);
   ~AnaFillExternResult();

   TGraphErrors* GetGrBluIntens() const;
   TGraphErrors* GetGrYelIntens() const;
   time_t        GetLumiOnTime()  const { return fTimeEventLumiOn; }
   time_t        GetLumiOffTime() const { return fTimeEventLumiOff; }

   void Process();

   ClassDef(AnaFillExternResult, 1)
};

#endif
