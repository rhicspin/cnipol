
#include <time.h>
#include <map>

#include "manalyze.h"

#include "TColor.h"
#include "TStyle.h"

#include "utils/utils.h"

#include "EventConfig.h"

using namespace std;

EventConfig *gRC;
UShort_t gPolId    = 0;
UShort_t gEnergyId = 0;
map<UInt_t, UInt_t> flattopTimes;

void manalyze(UShort_t polId, UShort_t eId)
{
   gPolId    = polId;
   gEnergyId = eId;

   initialize();
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
      //filelist = "list_B1U_ramp.dat"; color = kBlue-4; break;
      filelist = "list_Y1D_ramp.dat"; color = kOrange; break;
      //filelist = "list_B2D_ramp.dat"; color = kBlue-4; break;
      //filelist = "list_Y2U_ramp.dat"; color = kOrange; break;
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

   TH2* hAsymVsEnergy = new TH2F("hAsymVsEnergy", "hAsymVsEnergy", 1, 0, 450, 1, 0, 0.01);
   hAsymVsEnergy->GetXaxis()->SetTitle("Beam Energy, GeV");
   hAsymVsEnergy->GetYaxis()->SetTitle("Asymmetry");

   TGraphErrors *grAsymVsEnergy = new TGraphErrors();
   grAsymVsEnergy->SetName("grAsymVsEnergy");
   grAsymVsEnergy->SetMarkerStyle(kFullCircle);
   grAsymVsEnergy->SetMarkerSize(1);
   grAsymVsEnergy->SetMarkerColor(color);

   TH2* hAnaPowerVsEnergy = new TH2F("hAnaPowerVsEnergy", "hAnaPowerVsEnergy", 1, 0, 450, 1, 0.01, 0.02);
   hAnaPowerVsEnergy->GetXaxis()->SetTitle("Analyzing Power");
   hAnaPowerVsEnergy->GetYaxis()->SetTitle("Asymmetry");

   TGraphErrors *grAnaPowerVsEnergy = new TGraphErrors();
   grAnaPowerVsEnergy->SetName("grAnaPowerVsEnergy");
   grAnaPowerVsEnergy->SetMarkerStyle(kFullCircle);
   grAnaPowerVsEnergy->SetMarkerSize(1);
   grAnaPowerVsEnergy->SetMarkerColor(color);

   TH2* hPolarVsEnergy = new TH2F("hPolarVsEnergy", "hPolarVsEnergy", 1, 0, 450, 1, 0, 80);
   hPolarVsEnergy->GetXaxis()->SetTitle("Beam Energy, GeV");
   hPolarVsEnergy->GetYaxis()->SetTitle("Polarization, %");

   TGraphErrors *grPolarVsEnergy = new TGraphErrors();
   grPolarVsEnergy->SetName("grPolarVsEnergy");
   grPolarVsEnergy->SetMarkerStyle(kFullCircle);
   grPolarVsEnergy->SetMarkerSize(1);
   grPolarVsEnergy->SetMarkerColor(color);

   // Profile plots
   TH2* hRVsEnergy = new TH2F("hRVsEnergy", "hRVsEnergy", 1, 0, 450, 1, -0.2, 0.5);
   hRVsEnergy->GetXaxis()->SetTitle("Beam Energy, GeV");
   hRVsEnergy->GetYaxis()->SetTitle("r");

   TH1* hRVsEnergyBin = new TH1F("hRVsEnergyBin", "hRVsEnergyBin", 50, 0, 450);
   hRVsEnergyBin->GetXaxis()->SetTitle("Beam Energy, GeV");
   hRVsEnergyBin->GetYaxis()->SetTitle("r");
   hRVsEnergyBin->GetYaxis()->SetRangeUser(-0.2, 0.5);
   hRVsEnergyBin->SetMarkerStyle(kFullCircle);
   hRVsEnergyBin->SetMarkerSize(1);
   hRVsEnergyBin->SetMarkerColor(color);

   TGraphErrors *grRVsEnergy = new TGraphErrors();
   grRVsEnergy->SetName("grRVsEnergy");
   grRVsEnergy->SetMarkerStyle(kFullCircle);
   grRVsEnergy->SetMarkerSize(1);
   grRVsEnergy->SetMarkerColor(color);

   // 
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
      //cout << (((TObjString*) o)->GetName()) << endl;
      fileName = "";
      fileName += "/data1/run11/root/" + string(((TObjString*) o)->GetName()) + "/" + string(((TObjString*) o)->GetName()) + ".root";

      TFile *f = new TFile(fileName, "READ");
      if (!f) {
         gSystem->Error("", "file not found\n");
         continue; //exit(-1);
      }

      gSystem->Info("", "file found: %s", fileName.Data());

      gRC = (EventConfig*) f->FindObjectAny("EventConfig");

      if (!gRC) {
         gSystem->Error("", "RC not found\n");
         continue;
         //gRC->Print();
      }

      UInt_t   beamEnergy = (UInt_t) (gRC->fRunInfo->BeamEnergy + 0.5);
      Double_t runId = gRC->fRunInfo->RUNID;
      UInt_t   fillId = (UInt_t) runId;

      //if (flattopTimes.find(fillId) == flattopTimes.end()) 
      //   flattopTimes[fillId] = UINT_MAX;

      if ( beamEnergy == 250) {
         flattopTimes[fillId] = gRC->fRunInfo->StartTime;
      }

      delete f;
   }

   map<UInt_t, UInt_t>::iterator ift;

   for (ift=flattopTimes.begin(); ift!=flattopTimes.end(); ++ift) {
      printf("%d -> %d\n", ift->first, ift->second);
   }

   //return;
   //next->Begin();
   next->Reset();
   //*next = next->Begin();

   while (next && (o = (*next)()) )
   {
      //chain->AddFile(((TObjString*) o)->GetName());
      //cout << (((TObjString*) o)->GetName()) << endl;
      fileName = "";
      //fileName += "/usr/local/polarim/root/" + string(((TObjString*) o)->GetName()) + "/" + string(((TObjString*) o)->GetName()) + ".root";
      fileName += "/data1/run11/root/" + string(((TObjString*) o)->GetName()) + "/" + string(((TObjString*) o)->GetName()) + ".root";

      TFile *f = new TFile(fileName, "READ");
      if (!f) {
         gSystem->Error("", "file not found\n");
         continue; //exit(-1);
      }

      gSystem->Info("", "file found: %s", fileName.Data());

      gRC = (EventConfig*) f->FindObjectAny("EventConfig");

      if (!gRC) {
         gSystem->Error("", "RC not found\n");
         continue;
      }

      //gRC->Print();

      //Double_t energy = gRC->fRunInfo->BeamEnergy;
      //printf("%f, %d\n", energy, beamEnergy);

      //gH = new DrawObjContainer(f);
      //gH->d["profile"] = new CnipolProfileHists();
      //gH->ReadFromDir(f);

      //TH1* h = (TH1*) gH->d["profile"]->o[histName];
      //h->Print();
      //h->SetBit(TH1::kIsAverage);

      //if ( beamEnergy == 100 && gRC->fRunInfo->StartTime > flattopTimes[fillId]) {
      //   beamEnergy = 400;
      //}

      Double_t day = (gRC->fRunInfo->StartTime - firstDay)/60./60./24.;
      char strTime[80];
      strftime(strTime, 80, "%X", localtime(&gRC->fRunInfo->StartTime));

      //grPolar->SetPoint(i, gRC->fRunInfo->RUNID, gRC->fAnaResult->sinphi[0].P[0]);
      //if ((gPolId == 1 || gPolId ==2) && day < 50) continue;

      Double_t runId            = gRC->fRunInfo->RUNID;
      UInt_t   fillId           = (UInt_t) runId;
      UInt_t   beamEnergy       = (UInt_t) (gRC->fRunInfo->BeamEnergy + 0.5);
      Float_t  ana_power        = gRC->fAnaResult->A_N[1];
      Float_t  asymmetry        = gRC->fAnaResult->sinphi[0].P[0] * gRC->fAnaResult->A_N[1];
      Float_t  asymmetry_err    = gRC->fAnaResult->sinphi[0].P[1] * gRC->fAnaResult->A_N[1];
      Float_t  polarization     = gRC->fAnaResult->sinphi[0].P[0] * 100.;
      Float_t  polarization_err = gRC->fAnaResult->sinphi[0].P[1] * 100.;
      Float_t  profileRatio     = gRC->fAnaResult->fIntensPolarR;
      Float_t  profileRatioErr  = gRC->fAnaResult->fIntensPolarRErr;
      Float_t  tzero            = gRC->fCalibrator->fChannelCalibs[7].fT0Coef;
      Float_t  tzeroErr         = gRC->fCalibrator->fChannelCalibs[7].fT0CoefErr;

      //printf("tzero: %f %f %f %d %f \n", tzero, tzeroErr, runId, gRC->fRunInfo->StartTime, asymmetry);
      printf("%8.3f, %s, %3d, %f, %f, %f, %f, %f \n", runId, strTime, beamEnergy, asymmetry, asymmetry_err, ana_power, polarization, polarization_err);

      if (gEnergyId == 0 && beamEnergy !=  24) continue;
      if (gEnergyId == 1 && beamEnergy != 250) continue;

      if (polarization <= 1 || polarization_err > 5) {
         continue;
         //polarization     = 0;
         //polarization_err = 0;
      }

      //grPolar->SetPoint(i, day, tzero);
      //grPolar->SetPointError(i, 0, tzeroErr);

      grAsymVsEnergy->SetPoint(i, beamEnergy, asymmetry);
      grAsymVsEnergy->SetPointError(i, 0, asymmetry_err);

      grAnaPowerVsEnergy->SetPoint(i, beamEnergy, ana_power);
      grAnaPowerVsEnergy->SetPointError(i, 0, 0);

      grPolarVsEnergy->SetPoint(i, beamEnergy, polarization);
      grPolarVsEnergy->SetPointError(i, 0, polarization_err);

      // Profiles
      grRVsEnergy->SetPoint(i, beamEnergy, profileRatio);
      grRVsEnergy->SetPointError(i, 0, profileRatioErr);

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
      delete f;
   }

   string imageName;
   TCanvas *c = new TCanvas("cName", "cName", 1200, 600);

   hAsymVsEnergy->GetListOfFunctions()->Add(grAsymVsEnergy, "p");
   hAsymVsEnergy->Draw();
   imageName = "hAsymVsEnergy_" + filelist + ".png";
   c->SaveAs(imageName.c_str());

   hAnaPowerVsEnergy->GetListOfFunctions()->Add(grAnaPowerVsEnergy, "p");
   hAnaPowerVsEnergy->Draw();
   imageName = "hAnaPowerVsEnergy_" + filelist + ".png";
   c->SaveAs(imageName.c_str());

   hPolarVsEnergy->GetListOfFunctions()->Add(grPolarVsEnergy, "p");
   hPolarVsEnergy->Draw();
   imageName = "hPolarVsEnergy_" + filelist + ".png";
   c->SaveAs(imageName.c_str());

   // Profiles
   hRVsEnergy->GetListOfFunctions()->Add(grRVsEnergy, "p");
   hRVsEnergy->Draw();
   imageName = "hRVsEnergy_" + filelist + ".png";
   c->SaveAs(imageName.c_str());

   utils::BinGraph(grRVsEnergy, hRVsEnergyBin);

   TF1 *f1 = new TF1("f1", "[0] + [1]*x");
   hRVsEnergyBin->Fit("f1");

   hRVsEnergyBin->Draw();
   imageName = "hRVsEnergyBin_" + filelist + ".png";
   c->SaveAs(imageName.c_str());

   hPolar->GetListOfFunctions()->Add(grPolar, "p");
   hPolar->Draw();
   imageName = "polar_" + filelist + "_" + sEnergyId + ".png";
   c->SaveAs(imageName.c_str());

   //hPolarVsTime->GetListOfFunctions()->Add(grPolarVsTime, "p");
   //hPolarVsTime->Draw();
   grPolarVsTime->Draw("AP");
   //grPolarVsTime->Print();
   grPolarVsTime->GetHistogram()->GetXaxis()->SetTimeDisplay(1);
   //grPolarVsTime->GetHistogram()->GetXaxis()->SetTimeFormat("%d/%m/%y%F2011-02-01 12:00:00");
   grPolarVsTime->GetHistogram()->GetXaxis()->SetTimeFormat("%d/%m/%y");
   //grPolarVsTime->GetHistogram()->GetXaxis()->SetTimeOffset(6*3600, "local");
   //grPolarVsTime->GetHistogram()->GetXaxis()->SetTimeFormat("%H:%M:%S");
   //grPolarVsTime->Update("AP");
   c->Update();
   imageName = "hPolarVsTime_" + filelist + "_" + sEnergyId + ".png";
   c->SaveAs(imageName.c_str());

   hPolarVsFill->GetListOfFunctions()->Add(grPolarVsFill, "p");
   hPolarVsFill->Draw();
   imageName = "hPolarVsFill_" + filelist + "_" + sEnergyId + ".png";
   c->SaveAs(imageName.c_str());

   utils::BinGraph(grPolarVsFill, hPolarVsFillBin);

   hPolarVsFillBin->Draw();
   imageName = "hPolarVsFillBin_" + filelist + "_" + sEnergyId + ".png";
   c->SaveAs(imageName.c_str());

   hRVsTime->GetListOfFunctions()->Add(grRVsTime, "p");
   hRVsTime->Draw();
   imageName = "hRVsTime_" + filelist + "_" + sEnergyId + ".png";
   c->SaveAs(imageName.c_str());

   hRVsFill->GetListOfFunctions()->Add(grRVsFill, "p");
   hRVsFill->Draw();
   imageName = "hRVsFill_" + filelist + "_" + sEnergyId + ".png";
   c->SaveAs(imageName.c_str());

   utils::BinGraph(grRVsFill, hRVsFillBin);

   hRVsFillBin->Draw();
   imageName = "profr_fill_bin_" + filelist + "_" + sEnergyId + ".png";
   c->SaveAs(imageName.c_str());
}
