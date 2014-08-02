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

#include "DrawObjContainer.h"

#include "utils/utils.h"

using namespace std;

struct Result
{
   map< int, vector< vector<double> > >   second;
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
         for (map< int, vector< vector<double> > >::const_iterator it = result.second.begin(); it != result.second.end(); it++)
         {
            vector<double> values = it->second[det];
            for(vector<double>::const_iterator it2 = values.begin(); it2 != values.end(); it2++)
            {
               double value = *it2;
               if (min_value > value)
               {
                  min_value = value;
               }
               if (max_value < value)
               {
                  max_value = value;
               }
            }
         }

         // also we use information about mean values and their width
         // to calculate limits for plots that are not interested in outliers (such as PlotMean())
         for (map< int, vector< vector<double> > >::const_iterator it = result.second.begin(); it != result.second.end(); it++)
         {
            vector<double> values = it->second[det];
            for(vector<double>::const_iterator it2 = values.begin(); it2 != values.end(); it2++)
            {
               double value = *it2;
               if (!isnan(value))
               {
                  mean_sum += value;
                  count++;
               }
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
         for (map< int, vector< vector<double> > >::const_iterator it = result.second.begin(); it != result.second.end(); it++)
         {
            vector<double> values = it->second[det];
            for(vector<double>::const_iterator it2 = values.begin(); it2 != values.end(); it2++)
            {
               double value = *it2;
               if (!isnan(value))
               {
                  sigma_sum += (mean - value) * (mean - value);
                  count_crosscheck++;
               }
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


Color_t GetLineColor(int det)
{
   Color_t  line_color = det + 2;
   if (line_color == 5)
   {
      line_color = 28;
   }
   return line_color;
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
      Color_t  line_color = det + 2;
      if (line_color == 5)
      {
         line_color = 28;
      }
      g->SetMarkerColor(line_color);

      for (map< int, vector< vector<double> > >::iterator it = r1.second.begin(); it != r1.second.end(); it++)
      {
         double fill_id = it->first;
         vector<double> values1 = it->second.at(det);
         if (!r2.second.count(fill_id))
         {
            continue;
         }
         vector<double> values2 = r2.second.at(fill_id).at(det);

         for(vector<double>::const_iterator i1 = values1.begin(); i1 != values1.end(); i1++)
         for(vector<double>::const_iterator i2 = values2.begin(); i2 != values2.end(); i2++)
         {
            g->Set(i+1);
            g->SetPoint(i, *i1, *i2);
            cout << polIdName << "\t" << det << "\t" << fill_id << "\t" << *i1 << "\t" << *i2 << endl;
            ++i;
         }
      }

      h->GetListOfFunctions()->Add(g, "p");
   }
   h->SetXTitle(r1.YTitle.c_str());
   h->SetYTitle(r2.YTitle.c_str());

   o[name] = h;
}


EPolarimeterId	parsePolIdFromCdevKey(const string &key)
{
#define CHAR_PAIR(c1, c2) (((uint16_t)c1) << 8) | ((uint16_t)c2)
   switch(CHAR_PAIR(key[0], key[10]))
   {
   case CHAR_PAIR('b', '1'):
      return kB1U;
   case CHAR_PAIR('y', '1'):
      return kY1D;
   case CHAR_PAIR('b', '2'):
      return kB2D;
   case CHAR_PAIR('y', '2'):
      return kY2U;
   default:
      Error("masym", "Can't parse polarimeter");
      exit(EXIT_FAILURE);
   }
#undef CHAR_PAIR
}


bool FillBiasCurrent(int fill_id, Short_t polId, double startTime, double endTime, map<Short_t, Result> &rBiasCurrent)
{
   map<string, double> bias_mean_value;
   CachingLogReader<SshLogReader>	*bias_current_reader = NULL;

   // exportDataLogger seems to have problems exporting differently timestamped values
   // from different loggers, so we have to read out them separately
   switch(gRunConfig.GetBeamId((EPolarimeterId)polId))
   {
      case kBLUE_BEAM:
      {
         static CachingLogReader<SshLogReader> _reader(
               "RHIC/Polarimeter/Blue/biasReadbacks"
               );
         bias_current_reader = &_reader;
         break;
      }
      case kYELLOW_BEAM:
      {
         static CachingLogReader<SshLogReader> _reader(
               "RHIC/Polarimeter/Yellow/biasReadbacks"
               );
         bias_current_reader = &_reader;
         break;
      }
      default:
      Fatal("malpha", "Unknown beam type");
   }

   int retval = bias_current_reader->ReadTimeRangeMean(startTime, endTime, &bias_mean_value);

   if (retval)
   {
      Fatal("malpha", "Some problems with SshLogReader");
   }

   bool any_values_retrieved = false;
   for(map<string, double>::const_iterator it = bias_mean_value.begin(); it != bias_mean_value.end(); it++)
   {
      const string &key = it->first;
      double value = it->second;
      Info("malpha", "Mean %s equals to %f", key.c_str(), value);

      EPolarimeterId ssh_PolId = parsePolIdFromCdevKey(key);
      if (ssh_PolId != polId)
      {
         continue;
      }
      int	ssh_DetId = key[15] - '0';

      if (value > 100)
      {
         continue;
      }

      rBiasCurrent[polId].second[fill_id].resize(N_DETECTORS);
      rBiasCurrent[polId].second[fill_id][ssh_DetId-1].push_back(value);

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


void ProcessList(const string &filelist, const string &results_dir, const string &suffix, map<Short_t, Result> &rBiasCurrent)
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

      Info("malpha", "Run %s, %f, %f", runName.c_str(), startTime, ssh_endTime);

      if (ssh_endTime < startTime)
      {
         Info("malpha", "Invalid endtime. Skipping...");
         continue;
      }

      int fill_id = ParseFillId(runName);
      if (FillBiasCurrent(fill_id, polId, startTime, ssh_endTime, rBiasCurrent))
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

   gROOT->Macro("~/rootmacros/styles/style_malpha.C");

   map<Short_t, Result> rAlphaBeamCurrent;
   map<Short_t, Result> rSweepBeamCurrent;

   ProcessList("/eicdata/eic0005/veprbl/runXX/lists/run13_alpha_study_novoltagevariation", "/eicdata/eic0005/veprbl", "", rAlphaBeamCurrent);
   ProcessList("/eicdata/eic0005/runXX/lists/run13_phys", "/eicdata/eic0005/veprbl/phys", "", rSweepBeamCurrent);

   TFile *f1 = new TFile("output.root", "RECREATE");
   DrawObjContainer *oc = new DrawObjContainer(f1);
   PolarimeterIdSetConstIter iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      Short_t polId = *iPolId;
      string  polIdName = RunConfig::AsString(*iPolId);

      rAlphaBeamCurrent[polId].YTitle = "I_{bias}\\ during\\ alpha,\\ \\mu A";
      rSweepBeamCurrent[polId].YTitle = "I_{bias}\\ during\\ sweep,\\ \\mu A";

      oc->d[polIdName] = new DrawObjContainer(f1->mkdir(polIdName.c_str()));
      DrawObjContainer *sub_oc = oc->d[polIdName];

      PlotCorrelation(sub_oc, polIdName, "hAlphaBC_vs_SweepBC", rAlphaBeamCurrent[polId], rSweepBeamCurrent[polId]);
   }

   TCanvas r("r");
   oc->SaveAllAs(DrawObjContainer::FORMAT_EPS, r, "^.*$", "/tmp/", false);
   oc->Write();
   f1->Close();
   delete f1;

   return EXIT_SUCCESS;
}
