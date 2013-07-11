#include <cstdlib>
#include <string>
#include <map>
#include <vector>

#include "malpha.h"

#include "TEnv.h"
#include "TROOT.h"
#include "TLegend.h"

#include "MAlphaAnaInfo.h"
#include "AsymHeader.h"
#include "MeasInfo.h"

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
      h->GetXaxis()->SetTimeFormat("%d.%m.%Y");
      h->GetXaxis()->SetTimeOffset(min_startTime);
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
   TLegend *leg = new TLegend(0.15,0.1,0.85,0.3);
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

      if (max_startTime)
      {
         det_host = new TH1F(det_host_name, "", 100 * result.first.size(), -86400, max_startTime - min_startTime + 86400);
      }
      else
      {
         det_host = new TH1F(det_host_name, "", result.first.size(), 0.0, result.first.size());
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
      det_g->SetLineColor(line_color);
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
            host->GetXaxis()->SetBinLabel(i + 1, runName.c_str());
            det_host->GetXaxis()->SetBinLabel(i + 1, runName.c_str());
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
      TString sDet("Detector");
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
            sDet += ") keV";
         }
      }
      g->SetName(sDet);
      det_g->SetName(sDet);

      host->GetListOfFunctions()->Add(g, "pl");
      leg->AddEntry(g, sDet, "pl");
      det_host->GetListOfFunctions()->Add(det_g, "pl");

      TString  det_title(det_host_name);
      det_title += cut_str;
      det_title += sDet;
      det_host->SetTitle(det_title.ReplaceAll("_", "\\_"));
      det_host->SetOption("DUMMY GRIDX GRIDY");
      det_host->GetXaxis()->SetTimeDisplay(1);
      det_host->GetXaxis()->SetTimeFormat("%d.%m.%Y");
      det_host->GetXaxis()->SetTimeOffset(min_startTime);
      det_host->SetYTitle(h->GetYaxis()->GetTitle());
      det_hosts.push_back(det_host);
   }

   host->SetOption("DUMMY GRIDX GRIDY");
   host->GetXaxis()->SetTimeDisplay(1);
   host->GetXaxis()->SetTimeFormat("%d.%m.%Y");
   host->GetXaxis()->SetTimeOffset(min_startTime);
   host->SetYTitle(h->GetYaxis()->GetTitle());

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

   map< Short_t, ResultMean > rhAmGdGain_over_AmGain;
   map< Short_t, ResultMean > rhAmGdGain_over_AmGainErr;
   map< Short_t, ResultMean > rAmGdFit0Coef;
   map< Short_t, ResultMean > rAmGdFit0CoefErr;
   map< Short_t, ResultMean > rDeadLayerEnergy;
   map< Short_t, ResultMean > rDeadLayerEnergyErr;
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

      if (alphaSources != 2)
      {
         Info("malpha", "Not enough alpha sources in %s. Skipping", fileName.c_str());
         continue;
      }

      if (gMM->fMeasInfo->RUNID == 70213)
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

      TH1F  *hAmGdFit0Coef = (TH1F*) f->FindObjectAny("hAmGdFit0Coef");
      TH1F  *hAmGdAmpCoef_over_AmAmpCoef = (TH1F*) f->FindObjectAny("hAmGdAmpCoef_over_AmAmpCoef");
      TH1F  *hDeadLayerEnergy = (TH1F*) f->FindObjectAny("hDeadLayerEnergy");

      FillFromHist(hAmGdFit0Coef, startTime, rAmGdFit0Coef[polId], rAmGdFit0CoefErr[polId]);
      FillFromHist(hAmGdAmpCoef_over_AmAmpCoef, startTime, rhAmGdGain_over_AmGain[polId], rhAmGdGain_over_AmGainErr[polId]);
      FillFromHist(hDeadLayerEnergy, startTime, rDeadLayerEnergy[polId], rDeadLayerEnergyErr[polId]);

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

      PlotMean(sub_oc, polIdName, "hAmGdFit0Coef_by_day", rAmGdFit0Coef[polId], rAmGdFit0CoefErr[polId], runNameD[polId], min_startTime, max_startTime);
      PlotMean(sub_oc, polIdName, "hAmGdGain_over_AmGain_by_day", rhAmGdGain_over_AmGain[polId], rhAmGdGain_over_AmGainErr[polId], runNameD[polId], min_startTime, max_startTime);
      PlotMean(sub_oc, polIdName, "hDeadLayerEnergy_by_day", rDeadLayerEnergy[polId], rDeadLayerEnergyErr[polId], runNameD[polId], min_startTime, max_startTime);
      PlotMean(sub_oc, polIdName, "hAmGdFit0Coef_by_run", rAmGdFit0Coef[polId], rAmGdFit0CoefErr[polId], runNameD[polId], 0, 0);
      PlotMean(sub_oc, polIdName, "hAmGdGain_over_AmGain_by_run", rhAmGdGain_over_AmGain[polId], rhAmGdGain_over_AmGainErr[polId], runNameD[polId], 0, 0);
      PlotMean(sub_oc, polIdName, "hDeadLayerEnergy_by_run", rDeadLayerEnergy[polId], rDeadLayerEnergyErr[polId], runNameD[polId], 0, 0);
   }

   oc->Write();
   f1->Close();
   delete f1;

   return EXIT_SUCCESS;
}
