#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <numeric>
#include <stdint.h>

#include "malpha.h"

#include "TEnv.h"
#include "TROOT.h"
#include "TLegend.h"

#include <opencdev.h>

#include "MAlphaAnaInfo.h"
#include "AsymHeader.h"
#include "MeasInfo.h"
#include "BiasCurrentUtil.h"

#include "DrawObjContainer.h"

#include "utils/utils.h"

using namespace std;

typedef double Time;
typedef string RunName;
typedef int    DetectorId;
struct ResultMean
{
   map<Time, double> first;
   map< Time, vector<double> >   second;
   string   YTitle;

   // used for histogram limits
   double       min_value, max_value;
   double       mean, sigma;
};


/** */
void FillFromHist(TH1F *h, double startTime, ResultMean &result, ResultMean &result_err)
{
   // there are few alpha runs which don't have any data in their channels, skip them
   if ((!h) || (!h->GetEntries())) return;

   TFitResultPtr fitres = h->Fit("pol0", "QS"); // Q: quiet, S: return fitres

   result.first[startTime] = fitres->Value(0);
   result_err.first[startTime] = fitres->FitResult::Error(0);

   result.second[startTime].resize(N_DETECTORS);
   result_err.second[startTime].resize(N_DETECTORS);

   for (int det = 0; det < N_DETECTORS; det++)
   {
      int xmin = 0.5 + det * NSTRIP_PER_DETECTOR;
      int xmax = xmin + NSTRIP_PER_DETECTOR;

      TFitResultPtr fitres = h->Fit("pol0", "QS", "", xmin, xmax); // Q: quiet, S: return fitres

      if (fitres.Get())
      {
         result.second[startTime][det] = fitres->Value(0);
         result_err.second[startTime][det] = fitres->FitResult::Error(0);
      }
      else
      {
         result.second[startTime][det] = NAN;
         result_err.second[startTime][det] = NAN;
      }
   }

   result.YTitle = h->GetYaxis()->GetTitle();
}


void FillDeviceMaxMin(map<Short_t, ResultMean> &results)
{
   double min_value = FLT_MAX;
   double max_value = -FLT_MAX;

   double mean_sum = 0;
   int count = 0;

   for (map<Short_t, ResultMean>::const_iterator i = results.begin(); i != results.end(); i++)
   {
      const ResultMean &result = i->second;

      for (int det = 0; det < N_DETECTORS; det++)
      {
         // find min and max values
         for (map< Time, vector<double> >::const_iterator it = result.second.begin(); it != result.second.end(); it++)
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
         for (map< Time, vector<double> >::const_iterator it = result.second.begin(); it != result.second.end(); it++)
         {
            double value = it->second[det];
            if (!isnan(value))
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
   for (map<Short_t, ResultMean>::const_iterator i = results.begin(); i != results.end(); i++)
   {
      const ResultMean &result = i->second;

      for (int det = 0; det < N_DETECTORS; det++)
      {
         for (map< Time, vector<double> >::const_iterator it = result.second.begin(); it != result.second.end(); it++)
         {
            double value = it->second[det];
            if (!isnan(value))
            {
               sigma_sum += (mean - value) * (mean - value);
               count_crosscheck++;
            }
         }
      }
   }
   assert(count == count_crosscheck);
   double sigma = sqrt(sigma_sum / count);

   for (map<Short_t, ResultMean>::iterator i = results.begin(); i != results.end(); i++)
   {
      ResultMean &result = i->second;
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


/** */
void PlotMean(DrawObjContainer *oc, const string &polIdName, const char *name, ResultMean &result, ResultMean &result_err, map<Time, RunName> &runNameD, double min_startTime, double max_startTime)
{
   if (result.first.empty() && result.second.empty())
   {
      return;
   }

   ObjMap	&o = oc->o;
   TH1F  *h;

   TString hname(name);
   hname += polIdName;
   if (max_startTime)
   {
      h = new TH1F(hname, name, 100 * result.first.size(), -86400, max_startTime - min_startTime + 86400);
   }
   else
   {
      h = new TH1F(hname, name, result.first.size(), 0.0, 1.0);
   }
   o[name] = h;

   int bin = 1;
   for (map< Time, double >::iterator it = result.first.begin(); it != result.first.end(); it++)
   {
      double startTime = it->first;
      const RunName &runName = runNameD[startTime];
      double value = it->second;

      if (max_startTime)
      {
         bin = h->FindBin(startTime - min_startTime);
      }

      h->SetBinContent(bin, value);
      h->SetBinError(bin, result_err.first[startTime]);

      if (!max_startTime)
      {
         h->GetXaxis()->SetBinLabel(bin, runName.c_str());
         bin++;
      }
   }

   if (max_startTime)
   {
      h->GetXaxis()->SetTimeDisplay(1);
      h->GetXaxis()->SetTimeFormat("%m/%d/%y");
      h->GetXaxis()->SetTimeOffset(min_startTime);
      h->GetXaxis()->SetNdivisions(8);
   }
   h->SetYTitle(result.YTitle.c_str());

   double vpadding = (result.max_value - result.min_value) * 0.15;
   h->GetYaxis()->SetRangeUser(result.min_value - vpadding, result.max_value + vpadding);

   TH1F *hdet = 0;
   if (!max_startTime) // check if we are doing by_run plots
   {
      // we only do by_run distribution, since by_day will be the same
      for (int det = 0; det < N_DETECTORS; det++)
      {
         TString hname(name);
         hname += "_distribution";
         hname += (det + 1);
         hname += "_";
         hname += polIdName;
         hdet = new TH1F(hname, hname, 100, result.min_value, result.max_value);
         hdet->SetXTitle(h->GetYaxis()->GetTitle());
         hdet->SetLineColor(GetLineColor(det));
         for (map< Time, vector<double> >::iterator it = result.second.begin(); it != result.second.end(); it++)
         {
            double value = it->second[det];
            hdet->Fill(value);
         }
         hdet->Fit("gaus", "QW"); // Q: quiet, W: all weights = 1
         o[hname.Data()] = hdet;
      }
   }

   TString  canvasName("c");
   canvasName += name;
   canvasName += "_";
   canvasName += polIdName;
   TString  title(name);
   const char *cut_str = " (cut: |val-mean_i|<3*sigma_i)";
   title += cut_str;
   TCanvas *c = new TCanvas(canvasName, "", 800, 500);
   double bm = gStyle->GetPadBottomMargin();
   TLegend *leg = new TLegend(0.14,bm+0.02,0.22,bm+0.202);
   TH1F  *host, *det_host;
   vector<TH1F*>  det_hosts;
   TString	host_name("_");
   host_name += name;
   host_name += polIdName;
   if (max_startTime)
   {
      host = new TH1F(host_name, title, 100 * result.first.size(), -86400, max_startTime - min_startTime + 86400);
   }
   else
   {
      host = new TH1F(host_name, title, result.first.size(), 0.0, result.first.size());
   }

   double canvas_min = result.mean - 3*result.sigma;
   double canvas_max = result.mean + 3*result.sigma;
   host->GetYaxis()->SetRangeUser(canvas_min, canvas_max);

   for (int det = 0; det < N_DETECTORS; det++)
   {
      TString   det_host_name(name);
      det_host_name += (det+1);
      det_host_name += "_";
      det_host_name += polIdName;
      TString  det_title(det_host_name);
      det_title += cut_str;

      if (max_startTime)
      {
         det_host = new TH1F(det_host_name, det_title, 100 * result.first.size(), -86400, max_startTime - min_startTime + 86400);
      }
      else
      {
         det_host = new TH1F(det_host_name, det_title, result.first.size(), 0.0, result.first.size());
      }
      o[det_host_name.Data()] = det_host;

      TGraphErrors *g = new TGraphErrors(result.second.size());
      TGraphErrors *det_g = new TGraphErrors(result.second.size());
      Color_t line_color = GetLineColor(det);
      g->SetLineColor(line_color);
      g->SetMarkerColor(line_color);
      g->SetMarkerStyle(20);
      g->SetMarkerSize(0.4);
      det_g->SetLineColor(line_color);
      det_g->SetMarkerColor(line_color);
      det_g->SetMarkerStyle(20);
      det_g->SetMarkerSize(0.4);
      int i = 0;
      double xval = -0.5;

      for (map< Time, vector<double> >::iterator it = result.second.begin(); it != result.second.end(); it++)
      {
         double startTime = it->first;
         const RunName &runName = runNameD[startTime];
         double value = it->second[det];

         if (max_startTime)
         {
            xval = startTime - min_startTime;
         }
         else
         {
            xval++;
            if (i % 5 == 0)
            {
               TString label(runName);
               label = label.ReplaceAll(".alpha0", "");
               host->GetXaxis()->SetBinLabel(i + 1, label.Data());
               det_host->GetXaxis()->SetBinLabel(i + 1, label.Data());
            }
         }

         if (isnan(value))
         {
            g->RemovePoint(i);
            det_g->RemovePoint(i);
            continue;
         }

         if (value > canvas_max)
         {
            Warning("malpha", "value %f of %s is more than canvas_max of %f",
               value, result.YTitle.c_str(), canvas_max);
         }
         if (value < canvas_min)
         {
            Warning("malpha", "value %f of %s is less than canvas_min of %f",
               value, result.YTitle.c_str(), canvas_min);
         }

         g->SetPoint(i, xval, value);
         if (gMeasInfo->IsRunYear(2013))
         {
            g->SetPointError(i, 0, result_err.second[startTime][det]);
         }
         det_g->SetPoint(i, xval, value);
         det_g->SetPointError(i, 0, result_err.second[startTime][det]);
         i++;
      }
      TString sDet("Det");
      sDet += (det + 1);
      if (max_startTime)
      {
         TF1   fit_daily("fit_daily", "pol0");
         TF1   det_fit_daily("det_fit_daily", "pol0");
         fit_daily.SetLineColor(line_color);
         fit_daily.SetLineWidth(0.5);
         det_fit_daily.SetLineColor(kBlack);
         det_fit_daily.SetLineWidth(0.5);

         if (gMeasInfo->IsRunYear(2013))
         {
            Warning("malpha", "detected run13 : will fit only during beamtime");
            const double fit_min = 1362096000 - min_startTime; // 03/01/2013 00:00:00
            const double fit_max = 1372636800 - min_startTime; // 07/01/2013 00:00:00
            g->Fit(&fit_daily, "Q", "", fit_min, fit_max); // Q: quiet
            det_g->Fit(&det_fit_daily, "Q", "", fit_min, fit_max);   // Q: quiet
         }
         else
         {
            g->Fit(&fit_daily, "Q"); // Q: quiet
            det_g->Fit(&det_fit_daily, "Q");   // Q: quiet
         }
      }
      g->SetName(sDet);
      det_g->SetName(sDet);

      host->GetListOfFunctions()->Add(g, "p");
      leg->AddEntry(g, sDet, "pl");
      det_host->GetListOfFunctions()->Add(det_g, "p");

      det_host->SetOption("DUMMY GRIDX GRIDY");
      det_host->GetXaxis()->SetTimeDisplay(1);
      det_host->GetXaxis()->SetTimeFormat("%m/%d/%y");
      det_host->GetXaxis()->SetTimeOffset(min_startTime);
      det_host->GetXaxis()->SetNdivisions(8);
      det_host->SetXTitle(sDet);
      det_host->SetYTitle(h->GetYaxis()->GetTitle());
      if (!max_startTime)
      {
         det_host->SetLabelOffset(0.0);
      }
      det_hosts.push_back(det_host);
   }

   host->SetOption("DUMMY GRIDX GRIDY");
   host->GetXaxis()->SetTimeDisplay(1);
   host->GetXaxis()->SetTimeFormat("%m/%d/%y");
   host->GetXaxis()->SetTimeOffset(min_startTime);
   host->GetXaxis()->SetNdivisions(8);
   host->SetYTitle(h->GetYaxis()->GetTitle());
      if (!max_startTime)
      {
         host->SetLabelOffset(0.0);
      }

   for (vector<TH1F*>::iterator it = det_hosts.begin(); it != det_hosts.end(); it++)
   {
      TH1F  *det_host = *it;
      det_host->GetYaxis()->SetRangeUser(canvas_min, canvas_max);
   }

   host->Draw();
   leg->Draw();
   o[canvasName.Data()] = c;

   static TCanvas *dummy = new TCanvas();
   dummy->cd(); // Don't let anybody draw on our canvas
}


void PlotCorrelation(DrawObjContainer *oc, const string &polIdName, const char *name, ResultMean &r1, ResultMean &r1_err, ResultMean &r2, ResultMean &r2_err)
{
   if ((r1.first.empty() && r1.second.empty()) || (r2.first.empty() && r2.second.empty()))
   {
      return;
   }

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
      TGraphErrors *g = new TGraphErrors(r1.second.size());
      Color_t  line_color = det + 2;
      if (line_color == 5)
      {
         line_color = 28;
      }
      g->SetMarkerColor(line_color);

      for (map< Time, vector<double> >::iterator it = r1.second.begin(); it != r1.second.end(); it++)
      {
         double startTime = it->first;
         double value1 = it->second[det];
         if ((!r2.second.count(startTime)) || (!r2.second[startTime].size()))
         {
            continue;
         }
         double value2 = r2.second[startTime][det];

         g->SetPoint(i, value1, value2);
         g->SetPointError(i, r1_err.second[startTime][det], r2_err.second[startTime][det]);
         ++i;
      }

      h->GetListOfFunctions()->Add(g, "p");
   }
   h->SetXTitle(r1.YTitle.c_str());
   h->SetYTitle(r2.YTitle.c_str());

   o[name] = h;
}


/**
 * Takes in a AmGain plot. Calculates the slope of the Gain vs BiasCurrent
 * correlation.  Uses that slope value and BiasCurrent values to make a plot
 * AmGainCorrected containing the corrected values.
 */
vector<double> DoAmGainCorrection(ResultMean &rhAmGain, ResultMean &rhAmGainErr, ResultMean &rBiasCurrent, ResultMean &rhAmGainCorrected, ResultMean &rhAmGainCorrectedErr)
{
   vector<double>       slope;

   for(int det = 0; det < N_DETECTORS; det++)
   {
      int count = 0;
      for (map< Time, vector<double> >::iterator it = rhAmGain.second.begin(); it != rhAmGain.second.end(); it++)
      {
         Time   time = it->first;
         vector<double>    &det_gain = it->second;
         double    bias_current = rBiasCurrent.second[time].at(det);

         if (rBiasCurrent.second.count(time) && (!isnan(bias_current)) && !isnan(det_gain.at(det)))
         {
            count++;
         }
      }
      TGraph g(count);
      int i = 0;
      for (map< Time, vector<double> >::iterator it = rhAmGain.second.begin(); it != rhAmGain.second.end(); it++)
      {
         Time   time = it->first;
         vector<double>    &det_gain = it->second;
         double    bias_current = rBiasCurrent.second[time].at(det);

         if (rBiasCurrent.second.count(time) && (!isnan(bias_current)) && !isnan(det_gain.at(det)))
         {
            g.SetPoint(i, bias_current, det_gain.at(det));
            i++;
         }
      }
      assert(i == count);
      TFitResultPtr fitres = g.Fit("pol1", "QS"); // Q: quiet, S: return fitres
      assert(!isnan(fitres->Value(1)));
      slope.push_back(fitres->Value(1));
   }

   rhAmGainCorrected = rhAmGain;
   rhAmGainCorrectedErr = rhAmGainErr;
   rhAmGainCorrected.YTitle = "Corrected americium gain, ADC/keV";

   for (map< Time, vector<double> >::iterator it = rhAmGain.second.begin(); it != rhAmGain.second.end(); it++)
   {
      Time   time = it->first;
      vector<double>    &det_gain = it->second;
      for(int det = 0; det < N_DETECTORS; det++)
      {
         if (rBiasCurrent.second.count(time))
         {
            rhAmGainCorrected.second[time].at(det) = det_gain.at(det) - slope[det] * rBiasCurrent.second[time].at(det);
         }
         else
         {
            // If we can't correct the value, remove it.
            rhAmGainCorrected.second[time].at(det) = NAN;
         }
      }
   }
   return slope;
}


void FillDetectorAverage(ResultMean &result, ResultMean &result_err, double startTime)
{
   if (result.second.count(startTime) && !result.second[startTime].empty())
   {
      result.first[startTime] = accumulate(
         result.second[startTime].begin(),
         result.second[startTime].end(),
         (double)0
         ) / result.second[startTime].size();
      result_err.first[startTime] = accumulate(
         result_err.second[startTime].begin(),
         result_err.second[startTime].end(),
         (double)0
         ) / result_err.second[startTime].size();
   }
}


void FillBiasCurrent(opencdev::LogReader *log_reader, EPolarimeterId polId, double startTime, double endTime, map< Short_t, ResultMean > &rBiasCurrent, map< Short_t, ResultMean > &rBiasCurrentErr)
{
   opencdev::mean_result_t bias_mean_value;

   string logger_name = BiasCurrentUtil::GetBiasCurrentLoggerName(polId);
   log_reader->query_timerange_mean(logger_name, startTime, endTime, &bias_mean_value);

   rBiasCurrent[polId].second[startTime] = BiasCurrentUtil::FillBiasCurrentMeanValue(bias_mean_value, polId);
   rBiasCurrentErr[polId].second[startTime].resize(N_DETECTORS, (double)0.0);

   FillDetectorAverage(rBiasCurrent[polId], rBiasCurrentErr[polId], startTime);
   rBiasCurrent[polId].YTitle = "BiasCurrent, \\mu A";
}

void FillBeamCurrent(opencdev::LogReader *log_reader, int fill_id, EPolarimeterId polId, double startTime, map< Short_t, ResultMean > &rBeamCurrent, map< Short_t, ResultMean > &rBeamCurrentErr)
{
   opencdev::result_t beam_mean_value;

   log_reader->query_fill("RHIC/BeamIons", fill_id, &beam_mean_value);

   for(opencdev::result_t::const_iterator it = beam_mean_value.begin(); it != beam_mean_value.end(); it++)
   {
      const string &key = it->first;
      const map<opencdev::cdev_time_t, double> &values = it->second;

      if ((key == "bluDCCTtotal") && (gRunConfig.GetBeamId(polId) != kBLUE_BEAM))
      {
         continue;
      }
      else if ((key == "yelDCCTtotal") && (gRunConfig.GetBeamId(polId) != kYELLOW_BEAM))
      {
         continue;
      }

      double value_acc = 0.0;
      int value_count = 0;
      for(map<opencdev::cdev_time_t, double>::const_iterator it2 = values.begin(); it2 != values.end(); it2++)
      {
         double value = it2->second;
         if (value > 50.0)
         {
            value_acc += value;
            value_count++;
         }
      }
      double beam_current = value_acc / value_count;
      Info("malpha", "Mean %s equals to %f", key.c_str(), beam_current);

      rBeamCurrent[polId].second[startTime].resize(N_DETECTORS);
      for(int i = 0; i < N_DETECTORS; i++)
      {
         rBeamCurrent[polId].second[startTime][i] = beam_current;
      }
      rBeamCurrentErr[polId].second[startTime].resize(N_DETECTORS);
      for(int i = 0; i < N_DETECTORS; i++)
      {
         rBeamCurrentErr[polId].second[startTime][i] = 0;
      }
   }
   FillDetectorAverage(rBeamCurrent[polId], rBeamCurrentErr[polId], startTime);
   rBeamCurrent[polId].YTitle = "Beam intensity (10^11 protons)";
}


/** */
int main(int argc, char *argv[])
{
   setbuf(stdout, NULL);

   // Create a default one
   gMeasInfo = new MeasInfo();

   // Do not attempt to recover files
   gEnv->SetValue("TFile.Recover", 0);

   MAlphaAnaInfo mAlphaAnaInfo;
   mAlphaAnaInfo.ProcessOptions(argc, argv);
   mAlphaAnaInfo.VerifyOptions();

   opencdev::LocalLogReader log_reader(mAlphaAnaInfo.GetSlowControlLogDir());

   gROOT->Macro("~/rootmacros/styles/style_malpha.C");

   string filelist = mAlphaAnaInfo.GetMListFullPath();

   Info("malpha", "Starting first pass...");

   map< Short_t, ResultMean > rhAmGain;
   map< Short_t, ResultMean > rhAmGainErr;
   map< Short_t, ResultMean > rhAmGainCorrected;
   map< Short_t, ResultMean > rhAmGainCorrectedErr;
   map< Short_t, ResultMean > rhGdGain_over_AmGain;
   map< Short_t, ResultMean > rhGdGain_over_AmGainErr;
   map< Short_t, ResultMean > rhAmGdGain_over_AmGain;
   map< Short_t, ResultMean > rhAmGdGain_over_AmGainErr;
   map< Short_t, ResultMean > rDeadLayerEnergy;
   map< Short_t, ResultMean > rDeadLayerEnergyErr;
   map< Short_t, ResultMean > rDeadLayerSize;
   map< Short_t, ResultMean > rDeadLayerSizeErr;
   map< Short_t, ResultMean > rBeamCurrent;
   map< Short_t, ResultMean > rBeamCurrentErr;
   map< Short_t, ResultMean > rBiasCurrent;
   map< Short_t, ResultMean > rBiasCurrentErr;
   double max_startTime = -1;
   double min_startTime = -1;
   map< Short_t, map<Time, string> > runNameD;

   // Fill chain with all input files from filelist
   TObject *o;
   TIter   *next = new TIter(utils::getFileList(filelist));

   // Loop over the runs and record the time of the last flattop measurement in the fill
   while (next && (o = (*next)()) )
   {
      string measId   = string(((TObjString*) o)->GetName());
      string fileName = mAlphaAnaInfo.GetResultsDir() + "/" + measId + "/" + measId + mAlphaAnaInfo.GetSuffix() + ".root";
      TFile f(fileName.c_str(), "READ");

      if (!f.IsOpen())
      {
         Error("malpha", "file not found. Skipping...");
         continue;
      }

      if (f.IsZombie())
      {
         Error("malpha", "file is zombie %s. Skipping...", fileName.c_str());
         continue;
      }

      Info("malpha", "Found file: %s", fileName.c_str());

      EventConfig *gMM = (EventConfig*) f.FindObjectAny("measConfig");

      if (!gMM)
      {
         Error("malpha", "MM not found. Skipping...");
         continue;
      }

      int      alphaSources = gMM->fMeasInfo->GetAlphaSourceCount();
      string   runName      = gMM->fMeasInfo->GetRunName();
      Short_t  polId        = gMM->fMeasInfo->fPolId;
      Double_t startTime    = gMM->fMeasInfo->fStartTime;
      Double_t ssh_endTime  = gMM->fMeasInfo->fStopTime;

      if ((gMM->fMeasInfo->RUNID == 70213)
          || (int(gMM->fMeasInfo->RUNID) == 17400) // this one takes too long to process
          || (gMM->fMeasInfo->GetRunName() == "17514.101.alpha0")
          || (gMM->fMeasInfo->GetRunName() == "17515.101.alpha0")
          || (gMM->fMeasInfo->GetRunName() == "17517.101.alpha0")
          || (gMM->fMeasInfo->GetRunName() == "17518.101.alpha0")
          || (gMM->fMeasInfo->GetRunName() == "17519.101.alpha0")
          || (gMM->fMeasInfo->GetRunName() == "17520.101.alpha0")
          || (gMM->fMeasInfo->GetRunName() == "17521.101.alpha0")
          || (gMM->fMeasInfo->GetRunName() == "17524.101.alpha0")
          || (gMM->fMeasInfo->GetRunName() == "17526.101.alpha0")
          || (gMM->fMeasInfo->GetRunName() == "17527.101.alpha0")
          || (gMM->fMeasInfo->GetRunName() == "17529.101.alpha0")
          || (gMM->fMeasInfo->GetRunName() == "17530.101.alpha0")
          || (gMM->fMeasInfo->GetRunName() == "17533.101.alpha0")
          || (gMM->fMeasInfo->GetRunName() == "17534.101.alpha0"))
      {
         Info("malpha", "File %s is blacklisted. Skipping", fileName.c_str());
         continue;
      }

      if (runNameD[polId].count(startTime))
      {
         Error("malpha", "Duplicate time = %f, %s, %s",
               startTime, runName.c_str(), runNameD[polId][startTime].c_str());
         exit(EXIT_FAILURE);
      }
      runNameD[polId][startTime] = runName;

      if ((max_startTime == -1) || (max_startTime < startTime))
      {
         max_startTime = startTime;
      }
      if ((min_startTime == -1) || (min_startTime > startTime))
      {
         min_startTime = startTime;
      }

      FillBiasCurrent(&log_reader, (EPolarimeterId)polId, startTime, ssh_endTime, rBiasCurrent, rBiasCurrentErr);

      int fill_id = gMM->fMeasInfo->GetFillId();
      if (fill_id)
      {
         FillBeamCurrent(&log_reader, fill_id, (EPolarimeterId)polId, startTime, rBeamCurrent, rBeamCurrentErr);
      }

      TH1F  *hAmGain = (TH1F*) f.FindObjectAny("hAmGain");

      FillFromHist(hAmGain, startTime, rhAmGain[polId], rhAmGainErr[polId]);

      if (alphaSources == 2)
      {
         TH1F  *hGdGain = (TH1F*) f.FindObjectAny("hGdGain");
         TH1F  *hAmGdGain = (TH1F*) f.FindObjectAny("hAmGdGain");
         TH1F  *hGdGain_over_AmGain = new TH1F((*hGdGain) / (*hAmGain));
         TH1F  *hAmGdGain_over_AmGain = new TH1F((*hAmGdGain) / (*hAmGain));
         TH1F  *hDeadLayerEnergy = (TH1F*) f.FindObjectAny("hDeadLayerEnergy");
         TH1F  *hDeadLayerSize = (TH1F*) f.FindObjectAny("hDeadLayerSize");

         FillFromHist(hGdGain_over_AmGain, startTime, rhGdGain_over_AmGain[polId], rhGdGain_over_AmGainErr[polId]);
         rhGdGain_over_AmGain[polId].YTitle = "g_Gd / g_Am";
         FillFromHist(hAmGdGain_over_AmGain, startTime, rhAmGdGain_over_AmGain[polId], rhAmGdGain_over_AmGainErr[polId]);
         rhAmGdGain_over_AmGain[polId].YTitle = "g_AmGd / g_Am";
         FillFromHist(hDeadLayerEnergy, startTime, rDeadLayerEnergy[polId], rDeadLayerEnergyErr[polId]);
         FillFromHist(hDeadLayerSize, startTime, rDeadLayerSize[polId], rDeadLayerSizeErr[polId]);
      }
   }

   TFile *f1 = new TFile(mAlphaAnaInfo.fOutputFileName.c_str(), "RECREATE");
   DrawObjContainer *oc = new DrawObjContainer(f1);
   PolarimeterIdSetConstIter iPolId;

   FillDeviceMaxMin(rhAmGain);
   FillDeviceMaxMin(rhGdGain_over_AmGain);
   FillDeviceMaxMin(rhAmGdGain_over_AmGain);
   FillDeviceMaxMin(rDeadLayerEnergy);
   FillDeviceMaxMin(rDeadLayerSize);
   FillDeviceMaxMin(rBiasCurrent);
   FillDeviceMaxMin(rBeamCurrent);

   map<int, vector<double> > slope;

   for (iPolId = gRunConfig.fPolarimeters.begin(); iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      Short_t polId = *iPolId;
      string  polIdName = RunConfig::AsString(*iPolId);

      oc->d[polIdName] = new DrawObjContainer(f1->mkdir(polIdName.c_str()));
      DrawObjContainer *sub_oc = oc->d[polIdName];

      slope[*iPolId] = DoAmGainCorrection(rhAmGain[polId], rhAmGainErr[polId], rBiasCurrent[polId], rhAmGainCorrected[polId], rhAmGainCorrectedErr[polId]);

      PlotMean(sub_oc, polIdName, "hAmGain_by_day", rhAmGain[polId], rhAmGainErr[polId], runNameD[polId], min_startTime, max_startTime);
      PlotMean(sub_oc, polIdName, "hAmGainCorrected_by_day", rhAmGainCorrected[polId], rhAmGainCorrectedErr[polId], runNameD[polId], min_startTime, max_startTime);
      PlotMean(sub_oc, polIdName, "hGdGain_over_AmGain_by_day", rhGdGain_over_AmGain[polId], rhGdGain_over_AmGainErr[polId], runNameD[polId], min_startTime, max_startTime);
      PlotMean(sub_oc, polIdName, "hAmGdGain_over_AmGain_by_day", rhAmGdGain_over_AmGain[polId], rhAmGdGain_over_AmGainErr[polId], runNameD[polId], min_startTime, max_startTime);
      PlotMean(sub_oc, polIdName, "hDeadLayerEnergy_by_day", rDeadLayerEnergy[polId], rDeadLayerEnergyErr[polId], runNameD[polId], min_startTime, max_startTime);
      PlotMean(sub_oc, polIdName, "hDeadLayerSize_by_day", rDeadLayerSize[polId], rDeadLayerSizeErr[polId], runNameD[polId], min_startTime, max_startTime);
      PlotMean(sub_oc, polIdName, "hBiasCurrent_by_day", rBiasCurrent[polId], rBiasCurrentErr[polId], runNameD[polId], min_startTime, max_startTime);
      PlotMean(sub_oc, polIdName, "hAmGain_by_run", rhAmGain[polId], rhAmGainErr[polId], runNameD[polId], 0, 0);
      PlotMean(sub_oc, polIdName, "hAmGainCorrected_by_run", rhAmGainCorrected[polId], rhAmGainCorrectedErr[polId], runNameD[polId], 0, 0);
      PlotMean(sub_oc, polIdName, "hGdGain_over_AmGain_by_run", rhGdGain_over_AmGain[polId], rhGdGain_over_AmGainErr[polId], runNameD[polId], 0, 0);
      PlotMean(sub_oc, polIdName, "hAmGdGain_over_AmGain_by_run", rhAmGdGain_over_AmGain[polId], rhAmGdGain_over_AmGainErr[polId], runNameD[polId], 0, 0);
      PlotMean(sub_oc, polIdName, "hDeadLayerEnergy_by_run", rDeadLayerEnergy[polId], rDeadLayerEnergyErr[polId], runNameD[polId], 0, 0);
      PlotMean(sub_oc, polIdName, "hDeadLayerSize_by_run", rDeadLayerSize[polId], rDeadLayerSizeErr[polId], runNameD[polId], 0, 0);
      PlotMean(sub_oc, polIdName, "hBiasCurrent_by_run", rBiasCurrent[polId], rBiasCurrentErr[polId], runNameD[polId], 0, 0);

      PlotCorrelation(sub_oc, polIdName, "hBiasCurrent_AmGain", rBiasCurrent[polId], rBiasCurrentErr[polId], rhAmGain[polId], rhAmGainErr[polId]);
      PlotCorrelation(sub_oc, polIdName, "hBiasCurrent_DeadLayerSize", rBiasCurrent[polId], rBiasCurrentErr[polId], rDeadLayerSize[polId], rDeadLayerSizeErr[polId]);
      PlotCorrelation(sub_oc, polIdName, "hBiasCurrent_BeamCurrent", rBiasCurrent[polId], rBiasCurrentErr[polId], rBeamCurrent[polId], rBeamCurrentErr[polId]);
   }

   if (mAlphaAnaInfo.HasGraphBit())
   {
      TCanvas r("r");
      oc->SaveAllAs(DrawObjContainer::FORMAT_EPS, r, "^.*$", mAlphaAnaInfo.GetImageDir(), false);
   }
   oc->Write();
   f1->Close();
   delete f1;

   cout << "// ============= malpha generated for " << mAlphaAnaInfo.GetMListFileName() << " begin" << endl;
   cout << "//";
   for(int i = 0; i < argc; i++)
   {
      cout << " " << argv[i];
   }
   cout << endl;
   cout << "// version " << mAlphaAnaInfo.fAsymVersion << endl;
   cout << "map<int, vector<double> > slope; // {pol_id, det} -> slope" << endl;
   for (iPolId = gRunConfig.fPolarimeters.begin(); iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      cout << "slope[" << *iPolId << "].resize(" << N_DETECTORS << ");" << endl;
      for(int det = 0; det < N_DETECTORS; det++)
      {
         cout << "slope[" << *iPolId << "][" << det << "] = " << slope.at(*iPolId).at(det) << ";" << endl;
      }
   }
   cout << "// ============= malpha generated end" << endl;

   return EXIT_SUCCESS;
}
