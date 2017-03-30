#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <numeric>
#include <stdint.h>

#include <mysql++.h>

#include "malpha.h"

#include "TEnv.h"
#include "TROOT.h"
#include "TLegend.h"

#include "MAlphaAnaInfo.h"
#include "AsymHeader.h"
#include "MeasInfo.h"
#include "SshLogReader.h"
#include "CachingLogReader.h"
#include "BiasCurrentUtil.h"
#include "RunConfig.h"

#include "DrawObjContainer.h"

#include "utils/utils.h"

using namespace std;

struct Result
{
   map<string, vector<double> >   second;
   string   YTitle;

   // used for histogram limits
   double       min_value, max_value;
   double       mean, sigma;
};


void FillDeviceMaxMin(map<Short_t, Result> &results)
{
   double min_value = FLT_MAX;
   double max_value = -FLT_MAX;

   double mean_sum = 0;
   int count = 0;

   for (map<Short_t, Result>::const_iterator i = results.begin(); i != results.end(); i++)
   {
      const Result &result = i->second;

      for (int det = 0; det < N_DETECTORS; det++)
      {
         // find min and max values
         for (map<string, vector<double> >::const_iterator it = result.second.begin(); it != result.second.end(); it++)
         {
            double value = it->second[det];
            if (min_value > value)
            {
               min_value = value;
            }
            if (max_value < value)
            {
               max_value = value;
            }
         }

         // also we use information about mean values and their width
         // to calculate limits for plots that are not interested in outliers (such as PlotMean())
         for (map<string, vector<double> >::const_iterator it = result.second.begin(); it != result.second.end(); it++)
         {
            double value = it->second[det];
            if (!std::isnan(value))
            {
               mean_sum += value;
               count++;
            }
         }
      }
   }
   double mean = mean_sum / count;

   int count_crosscheck = 0;
   double sigma_sum = 0;
   for (map<Short_t, Result>::const_iterator i = results.begin(); i != results.end(); i++)
   {
      const Result &result = i->second;

      for (int det = 0; det < N_DETECTORS; det++)
      {
         for (map<string, vector<double> >::const_iterator it = result.second.begin(); it != result.second.end(); it++)
         {
            double value = it->second[det];
            if (!std::isnan(value))
            {
               sigma_sum += (mean - value) * (mean - value);
               count_crosscheck++;
            }
         }
      }
   }
   assert(count == count_crosscheck);
   double sigma = sqrt(sigma_sum / count);

   for (map<Short_t, Result>::iterator i = results.begin(); i != results.end(); i++)
   {
      Result &result = i->second;
      result.min_value = min_value;
      result.max_value = max_value;
      result.mean = mean;
      result.sigma = sigma;
   }
}


void PlotCorrelation(DrawObjContainer *oc, const string &polIdName, const char *name, Result &r1, Result &r2)
{
   ObjMap	&o = oc->o;
   TString	hname(name);
   hname += "_";
   hname += polIdName;
   TH2F	*h = new TH2F(hname, hname,
                      r1.max_value - r1.min_value, r1.min_value, r1.max_value,
                      r2.max_value - r2.min_value, r2.min_value, r2.max_value);

   for (int det = 0; det < N_DETECTORS; det++)
   {
      int	i = 0;
      TGraph *g = new TGraph(0);
      g->SetMarkerColor(RunConfig::DetAsColor(det));

      for (map<string, vector<double> >::iterator it = r1.second.begin(); it != r1.second.end(); it++)
      {
         const string &run_name = it->first;
         double value1 = it->second.at(det);
         if (!r2.second.count(run_name))
         {
            continue;
         }
         double value2 = r2.second.at(run_name).at(det);

         g->Set(i+1);
         g->SetPoint(i, value1, value2);
         cout << polIdName << "\t" << det << "\t" << run_name << "\t" << value1 << "\t" << value2 << endl;
         ++i;
      }

      h->GetListOfFunctions()->Add(g, "p");
   }
   h->SetXTitle(r1.YTitle.c_str());
   h->SetYTitle(r2.YTitle.c_str());

   o[name] = h;
}


bool FillBiasCurrent(const string &run_name, Short_t polId, double startTime, double endTime, map<Short_t, Result> &rBiasCurrent)
{
   opencdev::mean_result_t bias_mean_value;
   CachingLogReader<SshLogReader> log_reader;

   string logger_name = BiasCurrentUtil::GetBiasCurrentLoggerName((EPolarimeterId)polId);
   log_reader.query_timerange_mean(logger_name, startTime, endTime, &bias_mean_value);

   bool any_values_retrieved = false;
   for(opencdev::mean_result_t::const_iterator it = bias_mean_value.begin(); it != bias_mean_value.end(); it++)
   {
      const string &key = it->first;
      double value = it->second;
      Info("malpha", "Mean %s equals to %f", key.c_str(), value);

      EPolarimeterId ssh_PolId = BiasCurrentUtil::ParseLoggerPolId(key);
      if (ssh_PolId != polId)
      {
         continue;
      }
      int	ssh_DetId = key[15] - '0';

      if (value > 100)
      {
         continue;
      }

      rBiasCurrent[polId].second[run_name].resize(N_DETECTORS);
      rBiasCurrent[polId].second[run_name][ssh_DetId-1] = value;

      any_values_retrieved = true;
   }
   return any_values_retrieved;
}

UInt_t ParseFillId(string fRunName)
{
   TObjArray *subStrL = TPRegexp("^(\\d+)\\.\\d{3}(|\\.alpha0)$").MatchS(fRunName);

   if (subStrL->GetEntriesFast() < 1) {
      Error("GetFillId", "Cannot extract fill Id from run name");
      return 0;
   }

   TString sfillid = ((TObjString *) subStrL->At(1))->GetString();
   delete subStrL;

   UInt_t fFillId = sfillid.Atoi();

   if (fFillId <=0 ) {
      Error("GetFillId", "Invalid fill ID");
      return 0;
   }

   return fFillId;
}


void ProcessList(const string &filelist, const string &results_dir, const string &suffix, map<Short_t, Result> &rBiasCurrent, int mode)
{
   mysqlpp::Connection  fConnection("cnipol", "pc2pc.phy.bnl.gov", "cnipol", "(n!P0l", 3306);
   long total = 0, with_meas = 0;

   // Fill chain with all input files from filelist
   TObject *o;
   TIter   *next = new TIter(utils::getFileList(filelist));

   // Loop over the runs and record the time of the last flattop measurement in the fill
   while (next && (o = (*next)()) )
   {
      string measId   = string(((TObjString*) o)->GetName());
      mysqlpp::Query       select_query = fConnection.query();

      select_query << "SELECT polarimeter_id, start_time, stop_time FROM `run_info` "
         << "WHERE run_name = "
         << mysqlpp::quote << measId
         << ";";
      mysqlpp::StoreQueryResult res = select_query.store();
      if (res.num_rows() < 1)
      {
         Warning("malpha", "No DB record for %s", measId.c_str());
         continue;
      }
      assert(res.num_rows() == 1);

      string   runName      = measId;
      Short_t  polId        = res[0]["polarimeter_id"];
      Double_t startTime    = (time_t)((mysqlpp::DateTime)res[0]["start_time"]);
      Double_t ssh_endTime  = (time_t)((mysqlpp::DateTime)res[0]["stop_time"]);

      if (ssh_endTime < startTime)
      {
         Info("malpha", "Invalid endtime. Skipping...");
         continue;
      }

      if (mode == 0)
      {
         // nothing
      }
      else if(mode == 1)
      {
         startTime = ssh_endTime;
         ssh_endTime += 300;
      }
      else if(mode == 2)
      {
         ssh_endTime = startTime;
         startTime -= 300;
      }

      if (FillBiasCurrent(runName, polId, startTime, ssh_endTime, rBiasCurrent))
      {
         with_meas++;
      }
      total++;
   }

   FillDeviceMaxMin(rBiasCurrent);

   cout << "============================================" << endl;
   cout << "BC retrieval success rate for " << filelist << " is " << with_meas / (double)total * 100 << endl;
   cout << "total = " << total << endl;
   cout << "with_meas = " << with_meas << endl;
   cout << "============================================" << endl;
}


/** */
int main(int argc, char *argv[])
{
   setbuf(stdout, NULL);

   // Create a default one
   gMeasInfo = new MeasInfo();

   // Do not attempt to recover files
   gEnv->SetValue("TFile.Recover", 0);

   gROOT->Macro(CNIPOL_ROOT_DIR "/contrib/styles/style_malpha.C");

   map<Short_t, Result> rSweepBeamCurrent;
   map<Short_t, Result> rPostSweepBeamCurrent;
   map<Short_t, Result> rPreSweepBeamCurrent;

   ProcessList("/eicdata/eic0005/runXX/lists/run13_phys", "/eicdata/eic0005/veprbl/phys", "", rSweepBeamCurrent, 0);
   ProcessList("/eicdata/eic0005/runXX/lists/run13_phys", "/eicdata/eic0005/veprbl/phys", "", rPostSweepBeamCurrent, 1);
   ProcessList("/eicdata/eic0005/runXX/lists/run13_phys", "/eicdata/eic0005/veprbl/phys", "", rPreSweepBeamCurrent, 2);

   TFile *f1 = new TFile("output.root", "RECREATE");
   DrawObjContainer *oc = new DrawObjContainer(f1);
   PolarimeterIdSetConstIter iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      Short_t polId = *iPolId;
      string  polIdName = RunConfig::AsString(*iPolId);

      rSweepBeamCurrent[polId].YTitle = "I_{bias}\\ during\\ sweep,\\ \\mu A";
      rPostSweepBeamCurrent[polId].YTitle = "I_{bias}\\ after\\ sweep,\\ \\mu A";
      rPreSweepBeamCurrent[polId].YTitle = "I_{bias}\\ before\\ sweep,\\ \\mu A";

      oc->d[polIdName] = new DrawObjContainer(f1->mkdir(polIdName.c_str()));
      DrawObjContainer *sub_oc = oc->d[polIdName];

      PlotCorrelation(sub_oc, polIdName, "hSweepBC_vs_PostSweepBC", rSweepBeamCurrent[polId], rPostSweepBeamCurrent[polId]);
      PlotCorrelation(sub_oc, polIdName, "hSweepBC_vs_PreSweepBC", rSweepBeamCurrent[polId], rPreSweepBeamCurrent[polId]);
   }

   TCanvas r("r");
   oc->SaveAllAs(DrawObjContainer::FORMAT_EPS, r, "^.*$", "/tmp/", false);
   oc->Write();
   f1->Close();
   delete f1;

   return EXIT_SUCCESS;
}
