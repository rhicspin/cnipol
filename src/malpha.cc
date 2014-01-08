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

#include "MAlphaAnaInfo.h"
#include "AsymHeader.h"
#include "MeasInfo.h"
#include "SshLogReader.h"
#include "CachingLogReader.h"

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


void GetDeviceMaxMin(const ResultMean &result, double *min_value, double *max_value)
{
   *min_value = FLT_MAX;
   *max_value = -FLT_MAX;

   for (int det = 0; det < N_DETECTORS; det++)
   {
      for (map< Time, vector<double> >::const_iterator it = result.second.begin(); it != result.second.end(); it++)
      {
         double value = it->second[det];
         if (*min_value > value)
         {
            *min_value = value;
         }
         if (*max_value < value)
         {
            *max_value = value;
         }
      }
   }
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
      h->GetXaxis()->SetTimeFormat("%d.%m.%y");
      h->GetXaxis()->SetTimeOffset(min_startTime);
      h->GetXaxis()->SetNdivisions(8);
   }
   h->SetYTitle(result.YTitle.c_str());

   double max_value, min_value;
   GetDeviceMaxMin(result, &min_value, &max_value);
   double vpadding = (max_value - min_value) * 0.15;
   h->GetYaxis()->SetRangeUser(min_value - vpadding, max_value + vpadding);

   vector<double> mean, sigma;
   TH1F *hdet = 0;
   for (int det = 0; det < N_DETECTORS; det++)
   {
      TString hname(name);
      if (max_startTime)
      {
         hname += "deleteme";
      }
      hname += "_distribution";
      hname += (det + 1);
      hname += "_";
      hname += polIdName;
      hdet = new TH1F(hname, hname, 100, min_value, max_value);
      hdet->SetXTitle(h->GetYaxis()->GetTitle());
      for (map< Time, vector<double> >::iterator it = result.second.begin(); it != result.second.end(); it++)
      {
         double value = it->second[det];
         hdet->Fill(value);
      }
      TFitResultPtr fitres = hdet->Fit("gaus", "QSW"); // Q: quiet, S: return fitres, W: all weights = 1
      if (fitres.Get())
      {
         mean.push_back(fitres->Value(1));
         sigma.push_back(fitres->Value(2));
      }
      else
      {
         mean.push_back(0);
         sigma.push_back(-1);
      }
      if (max_startTime)
      {
         delete hdet;
      }
      else
      {
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
   TCanvas *c = new TCanvas(canvasName);
   double bm = gStyle->GetPadBottomMargin();
   TLegend *leg = new TLegend(0.15,bm+0.02,0.85,bm+0.202);
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

   double canvas_min_value = FLT_MAX, canvas_max_value = -FLT_MAX;

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
      Color_t  line_color = det + 2;
      if (line_color == 5)
      {
         line_color = 28;
      }
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

         if (fabs(value - mean[det]) > 3*sigma[det])
         {
            g->RemovePoint(i);
            det_g->RemovePoint(i);
            continue;
         }

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

         if (canvas_min_value > value)
         {
            canvas_min_value = value;
         }
         if (canvas_max_value < value)
         {
            canvas_max_value = value;
         }

         g->SetPoint(i, xval, value);
         g->SetPointError(i, 0, result_err.second[startTime][det]);
         det_g->SetPoint(i, xval, value);
         det_g->SetPointError(i, 0, result_err.second[startTime][det]);
         i++;
      }
      TString sDet("Det");
      sDet += (det + 1);
      if (max_startTime)
      {
         TF1   fit_daily("fit_daily", "pol1");
         TF1   det_fit_daily("det_fit_daily", "pol1");
         fit_daily.SetLineColor(line_color);
         det_fit_daily.SetLineColor(kBlack);
         det_fit_daily.SetLineWidth(1);
         TFitResultPtr fitres = g->Fit(&fit_daily, "QS"); // Q: quiet, S: return fitres
         det_g->Fit(&det_fit_daily, "Q"); // Q: quiet
         char buf[256];
         if (fitres.Get())
         {
            sDet += ": (";
            snprintf(buf, sizeof(buf), "%.2f", fitres->Value(1)*60*60*24*30);
            sDet += buf;
            sDet += "\\pm";
            snprintf(buf, sizeof(buf), "%.2f", fitres->FitResult::Error(1)*60*60*24*30);
            sDet += buf;
            sDet += ")*t(Months) + (";
            snprintf(buf, sizeof(buf), "%.2f", fitres->Value(0));
            sDet += buf;
            sDet += "\\pm";
            snprintf(buf, sizeof(buf), "%.2f", fitres->FitResult::Error(0));
            sDet += buf;
            sDet += ")";
            if (strstr(name, "hDeadLayerEnergy"))
            {
               sDet += " keV";
            }
            else if (strstr(name, "hDeadLayerSize"))
            {
               sDet += " \\mu m";
            }
         }
      }
      g->SetName(sDet);
      det_g->SetName(sDet);

      host->GetListOfFunctions()->Add(g, "p");
      leg->AddEntry(g, sDet, "pl");
      det_host->GetListOfFunctions()->Add(det_g, "p");

      det_host->SetOption("DUMMY GRIDX GRIDY");
      det_host->GetXaxis()->SetTimeDisplay(1);
      det_host->GetXaxis()->SetTimeFormat("%d.%m.%y");
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
   host->GetXaxis()->SetTimeFormat("%d.%m.%y");
   host->GetXaxis()->SetTimeOffset(min_startTime);
   host->GetXaxis()->SetNdivisions(8);
   host->SetYTitle(h->GetYaxis()->GetTitle());
      if (!max_startTime)
      {
         host->SetLabelOffset(0.0);
      }

   double canvas_vpadding = (canvas_max_value - canvas_min_value) * 0.4;
   host->GetYaxis()->SetRangeUser(canvas_min_value - canvas_vpadding * 1.5, canvas_max_value + canvas_vpadding * 0.5);

   for (vector<TH1F*>::iterator it = det_hosts.begin(); it != det_hosts.end(); it++)
   {
      TH1F  *det_host = *it;
      det_host->GetYaxis()->SetRangeUser(canvas_min_value - canvas_vpadding * 0.3, canvas_max_value + canvas_vpadding * 0.3);
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
   double max_value1, min_value1, max_value2, min_value2;
   GetDeviceMaxMin(r1, &min_value1, &max_value1);
   GetDeviceMaxMin(r2, &min_value2, &max_value2);
   TString	hname(name);
   hname += "_";
   hname += polIdName;
   TH2F	*h = new TH2F(hname, hname,
                      max_value1 - min_value1, min_value1, max_value1,
                      max_value2 - min_value2, min_value2, max_value2);

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

   gROOT->Macro("~/rootmacros/styles/style_malpha.C");

   gRunConfig.fBeamEnergies.clear();
   gRunConfig.fBeamEnergies.insert(kINJECTION);
   gRunConfig.fBeamEnergies.insert(kBEAM_ENERGY_255);

   string filelist = mAlphaAnaInfo.GetMListFullPath();

   Info("malpha", "Starting first pass...");

   map< Short_t, ResultMean > rhAmGain;
   map< Short_t, ResultMean > rhAmGainErr;
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

      TFile *f = new TFile(fileName.c_str(), "READ");

      if (!f)
      {
         Error("malpha", "file not found. Skipping...");
         delete f;
         continue;
      }

      if (f->IsZombie())
      {
         Error("malpha", "file is zombie %s. Skipping...", fileName.c_str());
         f->Close();
         delete f;
         continue;
      }

      Info("malpha", "Found file: %s", fileName.c_str());

      EventConfig *gMM = (EventConfig*) f->FindObjectAny("measConfig");

      if (!gMM)
      {
         Error("malpha", "MM not found. Skipping...");
         f->Close();
         delete f;
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

      map<string, double> bias_mean_value;
      CachingLogReader<SshLogReader>	*bias_current_reader = NULL;

      // exportDataLogger seems to have problems exporting differently timestamped values
      // from different loggers, so we have to read out them separately
      switch(gRunConfig.GetBeamId((EPolarimeterId)polId))
      {
         case kBLUE_BEAM:
         {
            static CachingLogReader<SshLogReader> _reader(
                  "RHIC/Polarimeter/Blue/biasReadbacks",
                  "bi12-pol3.1-det1.i:currentM,bi12-pol3.1-det2.i:currentM,bi12-pol3.1-det3.i:currentM,"
                  "bi12-pol3.1-det4.i:currentM,bi12-pol3.1-det5.i:currentM,bi12-pol3.1-det6.i:currentM,"
                  "bi12-pol3.2-det1.i:currentM,bi12-pol3.2-det2.i:currentM,bi12-pol3.2-det3.i:currentM,"
                  "bi12-pol3.2-det4.i:currentM,bi12-pol3.2-det5.i:currentM,bi12-pol3.2-det6.i:currentM"
                  );
            bias_current_reader = &_reader;
            break;
         }
         case kYELLOW_BEAM:
         {
            static CachingLogReader<SshLogReader> _reader(
                  "RHIC/Polarimeter/Yellow/biasReadbacks",
                  "yo12-pol3.1-det1.i:currentM,yo12-pol3.1-det2.i:currentM,yo12-pol3.1-det3.i:currentM,"
                  "yo12-pol3.1-det4.i:currentM,yo12-pol3.1-det5.i:currentM,yo12-pol3.1-det6.i:currentM,"
                  "yo12-pol3.2-det1.i:currentM,yo12-pol3.2-det2.i:currentM,yo12-pol3.2-det3.i:currentM,"
                  "yo12-pol3.2-det4.i:currentM,yo12-pol3.2-det5.i:currentM,yo12-pol3.2-det6.i:currentM"
                  );
            bias_current_reader = &_reader;
            break;
         }
         default:
         Error("malpha", "Unknown beam type");
         return EXIT_FAILURE;
      }

      int retval = bias_current_reader->ReadTimeRangeMean(startTime, ssh_endTime, &bias_mean_value);

      if (retval)
      {
         Error("malpha", "Some problems with SshLogReader");
         return EXIT_FAILURE;
      }

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

         rBiasCurrent[polId].second[startTime].resize(N_DETECTORS);
         rBiasCurrent[polId].second[startTime][ssh_DetId-1] = value;
         rBiasCurrentErr[polId].second[startTime].resize(N_DETECTORS);
         rBiasCurrentErr[polId].second[startTime][ssh_DetId-1] = 0;
      }
      FillDetectorAverage(rBiasCurrent[polId], rBiasCurrentErr[polId], startTime);
      rBiasCurrent[polId].YTitle = "BiasCurrent";

      static CachingLogReader<SshLogReader> beam_intensity_reader(
         "RHIC/BeamIons",
         "bluDCCTtotal,yelDCCTtotal"
         );

      int fill_id = gMM->fMeasInfo->GetFillId();
      if (fill_id)
      {
         map<string, double> beam_mean_value;
         int retval = beam_intensity_reader.ReadFillMean(fill_id, &beam_mean_value);

         if (retval)
         {
            Error("malpha", "Some problems with SshLogReader");
            return EXIT_FAILURE;
         }

         for(map<string, double>::const_iterator it = beam_mean_value.begin(); it != beam_mean_value.end(); it++)
         {
            const string &key = it->first;
            double value = it->second;
            Info("malpha", "Mean %s equals to %f", key.c_str(), value);

            if ((key == "bluDCCTtotal") && (gRunConfig.GetBeamId((EPolarimeterId)polId) != kBLUE_BEAM))
            {
               continue;
            }
            else if ((key == "yelDCCTtotal") && (gRunConfig.GetBeamId((EPolarimeterId)polId) != kYELLOW_BEAM))
            {
               continue;
            }
            rBeamCurrent[polId].second[startTime].resize(N_DETECTORS);
            for(int i = 0; i < N_DETECTORS; i++)
            {
               rBeamCurrent[polId].second[startTime][i] = value;
            }
            rBeamCurrentErr[polId].second[startTime].resize(N_DETECTORS);
            for(int i = 0; i < N_DETECTORS; i++)
            {
               rBeamCurrentErr[polId].second[startTime][i] = 0;
            }
         }
         FillDetectorAverage(rBeamCurrent[polId], rBeamCurrentErr[polId], startTime);
         rBeamCurrent[polId].YTitle = "BeamCurrent";
      }

      TH1F  *hAmGain = (TH1F*) f->FindObjectAny("hAmGain");

      FillFromHist(hAmGain, startTime, rhAmGain[polId], rhAmGainErr[polId]);

      if (alphaSources == 2)
      {
         TH1F  *hGdGain = (TH1F*) f->FindObjectAny("hGdGain");
         TH1F  *hAmGdGain = (TH1F*) f->FindObjectAny("hAmGdGain");
         TH1F  *hGdGain_over_AmGain = new TH1F((*hGdGain) / (*hAmGain));
         TH1F  *hAmGdGain_over_AmGain = new TH1F((*hAmGdGain) / (*hAmGain));
         TH1F  *hDeadLayerEnergy = (TH1F*) f->FindObjectAny("hDeadLayerEnergy");
         TH1F  *hDeadLayerSize = (TH1F*) f->FindObjectAny("hDeadLayerSize");

         FillFromHist(hGdGain_over_AmGain, startTime, rhGdGain_over_AmGain[polId], rhGdGain_over_AmGainErr[polId]);
         rhGdGain_over_AmGain[polId].YTitle = "g_Gd / g_Am";
         FillFromHist(hAmGdGain_over_AmGain, startTime, rhAmGdGain_over_AmGain[polId], rhAmGdGain_over_AmGainErr[polId]);
         rhAmGdGain_over_AmGain[polId].YTitle = "g_AmGd / g_Am";
         FillFromHist(hDeadLayerEnergy, startTime, rDeadLayerEnergy[polId], rDeadLayerEnergyErr[polId]);
         FillFromHist(hDeadLayerSize, startTime, rDeadLayerSize[polId], rDeadLayerSizeErr[polId]);
      }

      f->Close();
      delete f;
   }

   TFile *f1 = new TFile(mAlphaAnaInfo.fOutputFileName.c_str(), "RECREATE");
   DrawObjContainer *oc = new DrawObjContainer(f1);
   PolarimeterIdSetConstIter iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      Short_t polId = *iPolId;
      string  polIdName = RunConfig::AsString(*iPolId);

      oc->d[polIdName] = new DrawObjContainer(f1->mkdir(polIdName.c_str()));
      DrawObjContainer *sub_oc = oc->d[polIdName];

      PlotMean(sub_oc, polIdName, "hAmGain_by_day", rhAmGain[polId], rhAmGainErr[polId], runNameD[polId], min_startTime, max_startTime);
      PlotMean(sub_oc, polIdName, "hGdGain_over_AmGain_by_day", rhGdGain_over_AmGain[polId], rhGdGain_over_AmGainErr[polId], runNameD[polId], min_startTime, max_startTime);
      PlotMean(sub_oc, polIdName, "hAmGdGain_over_AmGain_by_day", rhAmGdGain_over_AmGain[polId], rhAmGdGain_over_AmGainErr[polId], runNameD[polId], min_startTime, max_startTime);
      PlotMean(sub_oc, polIdName, "hDeadLayerEnergy_by_day", rDeadLayerEnergy[polId], rDeadLayerEnergyErr[polId], runNameD[polId], min_startTime, max_startTime);
      PlotMean(sub_oc, polIdName, "hDeadLayerSize_by_day", rDeadLayerSize[polId], rDeadLayerSizeErr[polId], runNameD[polId], min_startTime, max_startTime);
      PlotMean(sub_oc, polIdName, "hBiasCurrent_by_day", rBiasCurrent[polId], rBiasCurrentErr[polId], runNameD[polId], min_startTime, max_startTime);
      PlotMean(sub_oc, polIdName, "hAmGain_by_run", rhAmGain[polId], rhAmGainErr[polId], runNameD[polId], 0, 0);
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

   return EXIT_SUCCESS;
}
