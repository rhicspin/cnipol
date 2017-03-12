
#include <string>
#include <climits>

#include "AnaFillExternResult.h"
#include "AnaGlobResult.h"
#include "BiasCurrentUtil.h"
#include "CachingLogReader.h"
#include "SshLogReader.h"
#include "Target.h"

ClassImp(AnaFillExternResult)

using namespace std;


/** */
AnaFillExternResult::AnaFillExternResult(UInt_t fillId) : TObject(),
   fBluIntensGraph(0), fYelIntensGraph(0),
   fBluRotCurStarGraph(0), fYelRotCurStarGraph(0), fBluRotCurPhenixGraph(0),
   fYelRotCurPhenixGraph(0), fBluSnakeCurGraph(0), fYelSnakeCurGraph(0),
   fAgsPolFitGraph(0),
   fTimeEventLumiOn(LONG_MAX), fTimeEventLumiOff(LONG_MIN)
{
   LoadInfo(fillId);
}


/** */
AnaFillExternResult::~AnaFillExternResult() { }


TGraphErrors* AnaFillExternResult::GetGrBluIntens() const { return fBluIntensGraph; }
TGraphErrors* AnaFillExternResult::GetGrYelIntens() const { return fYelIntensGraph; }


/** */
TGraphErrors* AnaFillExternResult::MakeGraph(const std::map<opencdev::cdev_time_t, double> &values, int thin_out_factor)
{
   TGraphErrors *gr = new TGraphErrors(values.size() / thin_out_factor);
   int i = 0;

   for(std::map<opencdev::cdev_time_t, double>::const_iterator it = values.begin(); it != values.end(); it++)
   {
      opencdev::cdev_time_t time = it->first;
      double value = it->second;

      if ((i % thin_out_factor) == 0)
      {
         gr->SetPoint(i / thin_out_factor, time, value);
      }
      i++;
   }

   return gr;
}


void AnaFillExternResult::LoadAgsInfo(opencdev::LogReader &log_reader)
{
   opencdev::cdev_time_t ags_time_start = fTimeEventLumiOn - 60*60*5; // These are intended to be plot limits - FIXME
   opencdev::cdev_time_t ags_time_end = fTimeEventLumiOff + 60*60*3;
   opencdev::result_t ags_pol_result;

   log_reader.query_timerange("Ags/Polarized_protons/CNIpolarimeter", ags_time_start, ags_time_end, &ags_pol_result);

   {
      std::map<opencdev::cdev_time_t, double> &values = ags_pol_result["start"];
      fAgsPolFitGraph = new TGraphErrors(values.size());
      int i = 0;

       for(std::map<opencdev::cdev_time_t, double>::const_iterator it = values.begin(); it != values.end(); it++)
       {
          opencdev::cdev_time_t time = it->first;
          double startTime = it->second;

          fAgsPolFitGraph->SetPoint(i, (startTime + time) / 2, ags_pol_result["AgsPolarFit"][time]);
          fAgsPolFitGraph->SetPointError(i, fabs(startTime - time) / 2, ags_pol_result["AgsPolarFitErr"][time]);
          i++;
       }
   }

#if 0
   ags_pol_result.clear();
   log_reader.set_additional_args(" -cells scope.xki:c4RawDataM\\[.\\]");
   log_reader.query_timerange("RHIC/Injection/BlueScope", ags_time_start, ags_time_end, &ags_pol_result);
   {
      std::map<opencdev::cdev_time_t, double> &values = ags_pol_result["scope.xki:c4RawDataM[.]"];

      for(std::map<opencdev::cdev_time_t, double>::const_iterator it = values.begin(); it != values.end(); it++)
      {
         opencdev::cdev_time_t time = it->first;
         TLine *l = new TLine(time, 0, time, 80);
         l->SetLineColor(kBlue);
         fKickerLines.push_back(l);
      }
   }

   ags_pol_result.clear();
   log_reader.set_additional_args(" -cells scope.yki:c4RawDataM:value\\[.\\]");
   log_reader.query_timerange("RHIC/Injection/YellowScope", ags_time_start, ags_time_end, &ags_pol_result);
   {
      std::map<opencdev::cdev_time_t, double> &values = ags_pol_result["scope.yki:c4RawDataM:value[.]"];
      for(std::map<opencdev::cdev_time_t, double>::const_iterator it = values.begin(); it != values.end(); it++)
      {
         opencdev::cdev_time_t time = it->first;
         TLine *l = new TLine(time, 20, time, 100);
         l->SetLineColor(kYellow);
         fKickerLines.push_back(l);
      }
   }
#endif
}


/** */
void AnaFillExternResult::LoadInfo(UInt_t fillId)
{
   if (fillId == 0)
   {
      Error("LoadInfo", "fillId == 0");
      return;
   }

   CachingLogReader<SshLogReader> log_reader;
   {
      std::vector<opencdev::cdev_time_t> ev_lumi_on, ev_lumi_off;
      log_reader.get_fill_events(fillId, "ev-lumi", &ev_lumi_on);
      log_reader.get_fill_events(fillId, "ev-lumi-off", &ev_lumi_off);
      if ((!ev_lumi_on.empty()) && (!ev_lumi_off.empty())) {
         fTimeEventLumiOn = ev_lumi_on[0];
         fTimeEventLumiOff = ev_lumi_off[0];
      } else {
         fTimeEventLumiOn = 0;
         fTimeEventLumiOff = 0;
      }
   }

   opencdev::result_t result;

   if (fTimeEventLumiOn && fTimeEventLumiOff) {
      log_reader.query_timerange("RHIC/BeamIons", fTimeEventLumiOn, fTimeEventLumiOff, &result);
      log_reader.query_timerange("RHIC/PowerSupplies/rot-ps", fTimeEventLumiOn, fTimeEventLumiOff, &result);
      log_reader.query_timerange("RHIC/PowerSupplies/snake-ps", fTimeEventLumiOn, fTimeEventLumiOff, &result);
   }

   fBluIntensGraph = MakeGraph(result["bluDCCTtotal"], 100);
   fYelIntensGraph = MakeGraph(result["yelDCCTtotal"], 100);
   fBluRotCurStarGraph = MakeGraph(result["bi5-rot3-outer"]);
   fYelRotCurStarGraph = MakeGraph(result["yo5-rot3-outer"]);
   fBluRotCurPhenixGraph = MakeGraph(result["yi7-rot3-outer"]);
   fYelRotCurPhenixGraph = MakeGraph(result["bo7-rot3-outer"]);
   fBluSnakeCurGraph = MakeGraph(result["bo3-snk7-outer"]);
   fYelSnakeCurGraph = MakeGraph(result["yi3-snk7-outer"]);

   /*
    * I'm not sure if we still need this. Below is the Grant's original commit description.
    *
    * Two Fills in Run13 have get incorrect start times from cdev. This hard
    * codes the start time for those two fills to be the time of the first
    * measurement. Note: Fill 17699 is the fill we created to seperate the two
    * fills with the same fill number. There were two fills originally in fill
    * 17600
    */
   if (fillId == 17443) fTimeEventLumiOn = (time_t)1367362617;
   if (fillId == 17699) fTimeEventLumiOn = (time_t)1370651445;

   opencdev::result_t bc_result;
   log_reader.set_additional_args("");
   log_reader.query_fill("RHIC/Polarimeter/Blue/biasReadbacks", fillId, &bc_result);
   log_reader.query_fill("RHIC/Polarimeter/Yellow/biasReadbacks", fillId, &bc_result);
   for(opencdev::result_t::const_iterator it = bc_result.begin(); it != bc_result.end(); it++)
   {
      const string &key = it->first;
      const map<opencdev::cdev_time_t, double> &values = it->second;

      EPolarimeterId polId = BiasCurrentUtil::ParseLoggerPolId(key);
      TGraphErrors *gr = MakeGraph(values);
      fBCCurGraph[polId].push_back(gr);
   }

   if (fTimeEventLumiOn && fTimeEventLumiOff) {
      LoadAgsInfo(log_reader);
   }
}


/** */
void AnaFillExternResult::Process()
{
   //Info("Process", "Executing...");
}
