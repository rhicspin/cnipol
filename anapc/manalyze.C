
#include <time.h>

#include "manalyze.h"

#include "TColor.h"
#include "TStyle.h"

#include "utils/utils.h"

#include "EventConfig.h"

EventConfig *gRC;

using namespace std;

UShort_t gPolId    = 0;
UShort_t gEnergyId = 0;

void manalyze(UShort_t polId, UShort_t eId)
{
   gPolId    = polId;
   gEnergyId = eId;

   initialize();
   //analyze_book_histograms();
   //analyze_fill_histograms(nEvents);
   //analyze_fill_histograms_derivative();
   //analyze_finalize();
}


void initialize()
{
   gStyle->SetOptTitle(0);
   gStyle->SetOptStat(0);
   gStyle->SetPadRightMargin(0.01);

   struct tm tm;
   time_t firstDay;

   if ( strptime("2011-01-01 00:00:00", "%Y-%m-%d %H:%M:%S", &tm) != NULL )
      firstDay = mktime(&tm);

   printf("firstDay %d\n", firstDay);

   TString filelist;
   Color_t color;

   switch (gPolId) {
   case 0:
      filelist = "list_B1U.dat"; color = kBlue-4; break;
   case 1:
      filelist = "list_Y1D.dat"; color = kOrange; break;
   case 2:
      filelist = "list_B2D.dat"; color = kBlue-4; break;
   case 3:
      filelist = "list_Y2U.dat"; color = kOrange; break;
   default:
      filelist = "list_B1U_15418.dat"; color = kBlue-4; break;
      //filelist = "list_Y2U_15418.dat"; color = kOrange; break;
      //filelist = "list_B2D_15418.dat"; color = kBlue-4; break;
      //filelist = "list_Y1D_15418.dat"; color = kOrange; break;
   }

   string sEnergyId;

   switch (gEnergyId) {
   case 0:
      sEnergyId = "inj"; break;
   case 1:
      sEnergyId = "250"; break;
   default:
      sEnergyId = "none"; break;
   }

   TString fileName;
   //string  histName = "hPolarUniProfileBin";
   string  histName = "hPolarVsIntensProfileBin";

   //TH1* havrg = new TH1F("havrg", "havrg", 20, -3, 3);
   TH1* havrg = new TH1F("havrg", "havrg", 22, 0, 1.1);

   havrg->GetYaxis()->SetRangeUser(0, 1.05);
   havrg->SetOption("p");
   havrg->SetMarkerStyle(kFullDotLarge);
   havrg->SetMarkerSize(1);
   havrg->SetMarkerColor(kRed);
   havrg->Sumw2();
   havrg->SetBit(TH1::kIsAverage);

   TH1* hProfR = new TH1F("hProfR", "hProfR", 1, 0, 1);

   TH2* hPolar = new TH2F("hPolar", "hPolar", 1, 20, 100, 1, 20, 80);
   hPolar->GetXaxis()->SetTitle("Days");
   hPolar->GetYaxis()->SetTitle("Polarization, %");
   TGraphErrors *grPolar = new TGraphErrors();
   grPolar->SetName("grPolar");
   grPolar->SetMarkerStyle(kFullCircle);
   grPolar->SetMarkerSize(1);
   grPolar->SetMarkerColor(color);

   TH2* hPolarVsTime = new TH2F("hPolarVsTime", "hPolarVsTime", 1, 20, 100, 1, 20, 80);
   hPolarVsTime->GetXaxis()->SetTitle("Date/Time");
   hPolarVsTime->GetYaxis()->SetTitle("Polarization, %");

   TGraphErrors *grPolarVsTime = new TGraphErrors();
   grPolarVsTime->SetName("grPolarVsTime");
   grPolarVsTime->SetMarkerStyle(kFullCircle);
   grPolarVsTime->SetMarkerSize(1);
   grPolarVsTime->SetMarkerColor(color);

   TH2* hPolarVsFill = new TH2F("hPolarVsFill", "hPolarVsFill", 1, 14900, 15500, 1, 20, 80);
   hPolarVsFill->GetXaxis()->SetTitle("Fill");
   hPolarVsFill->GetYaxis()->SetTitle("Polarization, %");
   TGraphErrors *grPolarVsFill = new TGraphErrors();
   grPolarVsFill->SetName("grPolarVsFill");
   grPolarVsFill->SetMarkerStyle(kFullCircle);
   grPolarVsFill->SetMarkerSize(1);
   grPolarVsFill->SetMarkerColor(color);

   TH1F* hPolarVsFillBin = new TH1F("hPolarVsFillBin", "hPolarVsFillBin", 600, 14900, 15500);
   hPolarVsFillBin->GetYaxis()->SetRangeUser(20, 80);
   hPolarVsFillBin->GetXaxis()->SetTitle("Fill");
   hPolarVsFillBin->GetYaxis()->SetTitle("Polarization, %");
   hPolarVsFillBin->SetMarkerStyle(kFullCircle);
   hPolarVsFillBin->SetMarkerSize(1);
   hPolarVsFillBin->SetMarkerColor(color);

   TH2* hRVsTime = new TH2F("hRVsTime", "hRVsTime", 1, 20, 100, 1, -0.1, 1);
   hRVsTime->GetXaxis()->SetTitle("Days");
   hRVsTime->GetYaxis()->SetTitle("r");
   TGraphErrors *grRVsTime = new TGraphErrors();
   grRVsTime->SetName("grRVsTime");
   grRVsTime->SetMarkerStyle(kFullCircle);
   grRVsTime->SetMarkerSize(1);
   grRVsTime->SetMarkerColor(color);

   TH2* hRVsFill = new TH2F("hRVsFill", "hRVsFill", 1, 14900, 15500, 1, -0.1, 1);
   hRVsFill->GetXaxis()->SetTitle("Fill");
   hRVsFill->GetYaxis()->SetTitle("r");
   TGraphErrors *grRVsFill = new TGraphErrors();
   grRVsFill->SetName("grRVsFill");
   grRVsFill->SetMarkerStyle(kFullCircle);
   grRVsFill->SetMarkerSize(1);
   grRVsFill->SetMarkerColor(color);

   TH1F* hRVsFillBin = new TH1F("hRVsFillBin", "hRVsFillBin", 600, 14900, 15500);
   hRVsFillBin->GetYaxis()->SetRangeUser(-0.1, 1);
   hRVsFillBin->GetXaxis()->SetTitle("Fill");
   hRVsFillBin->GetYaxis()->SetTitle("r");
   hRVsFillBin->SetMarkerStyle(kFullCircle);
   hRVsFillBin->SetMarkerSize(1);
   hRVsFillBin->SetMarkerColor(color);


   UInt_t i = 0;

   // Fill chain with all input files from filelist
   TObject *o;
   TIter   *next = new TIter(utils::getFileList(filelist));

   while (next && (o = (*next)()) )
   {
      //chain->AddFile(((TObjString*) o)->GetName());
      cout << (((TObjString*) o)->GetName()) << endl;
      fileName = "";
      //fileName += "/usr/local/polarim/root/" + string(((TObjString*) o)->GetName()) + "/" + string(((TObjString*) o)->GetName()) + ".root";
      fileName += "/data1/run11/root/" + string(((TObjString*) o)->GetName()) + "/" + string(((TObjString*) o)->GetName()) + ".root";

      TFile *f = new TFile(fileName, "READ");
      if (!f) {
         gSystem->Error("", "file not found\n");
         continue; //exit(-1);
      }

      gRC = (EventConfig*) f->FindObjectAny("EventConfig");
      if (!gRC) {
         gSystem->Error("", "RC not found\n");
         continue;
      //gRC->Print();
      }

      float beamEnergy = gRC->fRunInfo->BeamEnergy;

      //if (gEnergyId == 0 && (beamEnergy < 20 || beamEnergy > 30) ) continue;
      //if (gEnergyId == 1 &&  beamEnergy < 200) continue;

      //gH = new DrawObjContainer(f);
      //gH->d["profile"] = new CnipolProfileHists();
      //gH->ReadFromDir(f);

      //TH1* h = (TH1*) gH->d["profile"]->o[histName];
      //h->Print();
      //h->SetBit(TH1::kIsAverage);

      Double_t runId = gRC->fRunInfo->RUNID;
      
      Double_t day = (gRC->fRunInfo->StartTime - firstDay)/60./60./24.;
      //grPolar->SetPoint(i, gRC->fRunInfo->RUNID, gRC->fAnaResult->sinphi[0].P[0]);
      //if ((gPolId == 1 || gPolId ==2) && day < 50) continue;

      float polarization     = gRC->fAnaResult->sinphi[0].P[0] * 100.;
      float polarization_err = gRC->fAnaResult->sinphi[0].P[1] * 100.;

      //if (polarization <= 1 || polarization_err > 5) {
      //   continue;
      //   //polarization     = 0;
      //   //polarization_err = 0;
      //}

      float tzero    = gRC->fCalibrator->fChannelCalibs[7].fT0Coef;
      float tzeroErr = gRC->fCalibrator->fChannelCalibs[7].fT0CoefErr;

      printf("tzero: %f %f %f %f \n", tzero, tzeroErr, runId, gRC->fRunInfo->StartTime);

      float profileRatio    = gRC->fAnaResult->fIntensPolarR;
      float profileRatioErr = gRC->fAnaResult->fIntensPolarRErr;

      //grPolar->SetPoint(i, day, tzero);
      //grPolar->SetPointError(i, 0, tzeroErr);

      grPolar->SetPoint(i, day, polarization);
      grPolar->SetPointError(i, 0, polarization_err);

      grPolarVsTime->SetPoint(i, gRC->fRunInfo->StartTime, polarization);
      grPolarVsTime->SetPointError(i, 0, polarization_err);

      grPolarVsFill->SetPoint(i, runId, polarization);
      grPolarVsFill->SetPointError(i, 0, polarization_err);

      grRVsTime->SetPoint(i, day, profileRatio);
      grRVsTime->SetPointError(i, 0, profileRatioErr);

      grRVsFill->SetPoint(i, runId, profileRatio);
      grRVsFill->SetPointError(i, 0, profileRatioErr);

      //if (i == 0) havrg = h;
      //else havrg->Add(h);
      //havrg->Add(h);

      i++;
   }

   string imageName;
   TCanvas *c = new TCanvas("cName", "cName", 1200, 600);

   hPolar->GetListOfFunctions()->Add(grPolar, "p");
   hPolar->Draw();
   imageName = "polar_" + filelist + "_" + sEnergyId + ".png";
   c->SaveAs(imageName.c_str());

   //hPolarVsTime->GetListOfFunctions()->Add(grPolarVsTime, "p");
   //hPolarVsTime->Draw();
   grPolarVsTime->Draw("AP");
   grPolarVsTime->GetHistogram()->GetXaxis()->SetTimeDisplay(1);
   //grPolarVsTime->GetHistogram()->GetXaxis()->SetTimeFormat("%d/%m/%y%F2011-02-01 12:00:00");
   grPolarVsTime->GetHistogram()->GetXaxis()->SetTimeOffset(6*3600, "local");
   grPolarVsTime->GetHistogram()->GetXaxis()->SetTimeFormat("%H:%M:%S");
   //grPolarVsTime->Update("AP");
   c->Update();
   imageName = "hPolarVsTime_" + filelist + "_" + sEnergyId + ".png";
   c->SaveAs(imageName.c_str());

   hPolarVsFill->GetListOfFunctions()->Add(grPolarVsFill, "p");
   hPolarVsFill->Draw();
   imageName = "polar_fill_" + filelist + "_" + sEnergyId + ".png";
   c->SaveAs(imageName.c_str());

   utils::BinGraph(grPolarVsFill, hPolarVsFillBin);

   hPolarVsFillBin->Draw();
   imageName = "polar_fill_bin_" + filelist + "_" + sEnergyId + ".png";
   c->SaveAs(imageName.c_str());

   hRVsTime->GetListOfFunctions()->Add(grRVsTime, "p");
   hRVsTime->Draw();
   imageName = "profr_" + filelist + "_" + sEnergyId + ".png";
   c->SaveAs(imageName.c_str());

   hRVsFill->GetListOfFunctions()->Add(grRVsFill, "p");
   hRVsFill->Draw();
   imageName = "profr_fill_" + filelist + "_" + sEnergyId + ".png";
   c->SaveAs(imageName.c_str());

   utils::BinGraph(grRVsFill, hRVsFillBin);

   hRVsFillBin->Draw();
   imageName = "profr_fill_bin_" + filelist + "_" + sEnergyId + ".png";
   c->SaveAs(imageName.c_str());

   //return;

/*
   TF1 *mfPow = new TF1("mfPow", "[0]*TMath::Power(x, [1])", 0.1, 1);

   mfPow->SetParNames("P_{max}", "r");
   mfPow->SetParameter(0, 0.5);
   mfPow->SetParLimits(0, 0, 1);
   mfPow->SetParameter(1, 0.1);
   mfPow->SetParLimits(1, -2, 2);

   havrg->Fit("mfPow", "M E R");

   havrg->Print();
   havrg->GetYaxis()->SetRangeUser(0, 1.05);
   //havrg->Draw();
   //gPad->Update();
   //gPad->SaveAs("15221.2.v4.png");


   Double_t xmin, ymin, xmax, ymax;
   grPolar->ComputeRange(xmin, ymin, xmax, ymax);
   printf("%f %f %f %f\n", xmin, ymin, xmax, ymax);

   hPolar->GetListOfFunctions()->Add(grPolar, "p");
   //hPolar->GetXaxis()->SetLimits(xmin, xmax);
   //hPolar->GetYaxis()->SetLimits(ymin, ymax);
   //hPolar->GetYaxis()->SetLimits(0, 110);
   //hPolar->GetYaxis()->SetRangeUser(ymin, ymax);
   //hPolar->GetXaxis()->SetRangeUser(20, 100);
   hPolar->GetYaxis()->SetRangeUser(0, 70);
   //hPolar->GetYaxis()->SetRangeUser(-15, 25);
   //hPolar->GetYaxis()->SetRangeUser(-30, 10);
   hPolar->GetXaxis()->SetTitle("Days");
   hPolar->GetYaxis()->SetTitle("Polarization, %");
   //hPolar->GetYaxis()->SetTitle("t_0, ns");
   //TCanvas *c = new TCanvas("cName", "cName", 1200, 600);
   //hPolar->Draw();
*/
}
