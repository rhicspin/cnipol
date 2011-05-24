/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolPreprocHists.h"

#include "TDirectoryFile.h"
#include "TH2F.h"
#include "TH1D.h"
#include "TPaveStats.h"

#include "AnaInfo.h"
#include "RunInfo.h"

ClassImp(CnipolPreprocHists)

using namespace std;

/** Default constructor. */
CnipolPreprocHists::CnipolPreprocHists() : DrawObjContainer()
{
   BookHists();
}


CnipolPreprocHists::CnipolPreprocHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
}


/** Default destructor. */
CnipolPreprocHists::~CnipolPreprocHists()
{
}


/** */
void CnipolPreprocHists::BookHists(string sid)
{ //{{{

   o["hTvsA"] = new TH2F("hTvsA", "hTvsA", 255, 0, 255, 80, 10, 90);
   ((TH1*) o["hTvsA"])->SetOption("colz LOGZ");
   ((TH1*) o["hTvsA"])->SetTitle(";Amplitude, ADC;TDC;");

   o["hTimeVsEnergyA"] = new TH2F("hTimeVsEnergyA", "hTimeVsEnergyA", 100, 0, 2500, 60, 0, 120);
   ((TH1*) o["hTimeVsEnergyA"])->SetOption("colz LOGZ");
   ((TH1*) o["hTimeVsEnergyA"])->SetTitle(";Deposited Energy, keV;Time, ns;");

   o["hFitMeanTimeVsEnergyA"] = new TH1D("hFitMeanTimeVsEnergyA", "hFitMeanTimeVsEnergyA", 100, 0, 2500);
   ((TH1*) o["hFitMeanTimeVsEnergyA"])->GetYaxis()->SetRangeUser(0, 120);
   ((TH1*) o["hFitMeanTimeVsEnergyA"])->SetTitle(";Deposited Energy, keV;Mean Time, ns;");

   for (UShort_t iCh=1; iCh<=N_SILICON_CHANNELS; iCh++) {

      char   hName[256];
      string sChId("  ");

      sprintf(&sChId[0], "%02d", iCh);

      //sprintf(hName, "hTvsA_ch%02d", iCh);
      //o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
      //((TH1*) o[hName])->SetOption("colz LOGZ");
      //((TH1*) o[hName])->SetTitle(";Amplitude, ADC;TDC;");

      // Time vs Energy from amplitude
      sprintf(hName, "hTimeVsEnergyA_ch%02d", iCh);
      o[hName] = new TH2F(hName, hName, 100, 0, 2500, 60, 0, 120);
      ((TH1*) o[hName])->SetOption("colz LOGZ");
      ((TH1*) o[hName])->SetTitle(";Deposited Energy, keV;Time, ns;");

      sprintf(hName, "hFitMeanTimeVsEnergyA_ch%02d", iCh);
      o[hName] = new TH1D(hName, hName, 100, 0, 2500);
      ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 120);
      ((TH1*) o[hName])->SetTitle(";Deposited Energy, keV;Mean Time, ns;");
   }
} //}}}


/** */
//void CnipolPreprocHists::Fill(ChannelEvent *ch, string sid)
//{ //{{{
//} //}}}


/** */
void CnipolPreprocHists::FillPreProcess(ChannelEvent *ch)
{ //{{{

   ((TH1*) o["hTvsA"])->Fill(ch->GetAmpltd(), ch->GetTdc());
   ((TH1*) o["hTimeVsEnergyA"])->Fill(ch->GetEnergyA(), ch->GetTime());

   UChar_t chId  = ch->GetChannelId();

   string sChId("  ");
   sprintf(&sChId[0], "%02d", chId);

   //((TH1*) o["hTvsA_ch"+sChId]) -> Fill(ch->GetAmpltd(), ch->GetTdc());
   ((TH1*) o["hTimeVsEnergyA_ch"+sChId]) -> Fill(ch->GetEnergyA(), ch->GetTime());

} //}}}


/** */
void CnipolPreprocHists::SaveAllAs(TCanvas &c, string pattern, string path)
{ //{{{
   //Warning("SaveAllAs", "executing...");
   DrawObjContainer::SaveAllAs(c, pattern, path);

   string cName = "c_combo";

   // Draw superimposed histos
   TH1* h1 = (TH1*) o["hTimeVsEnergyA"];
   TH1* h2 = (TH1*) o["hFitMeanTimeVsEnergyA"];

   c.cd();
   char *l = strstr(h1->GetOption(), "LOGZ");
   //printf("XXX1: set logz %s\n", ((TH1*)io->second)->GetOption());
   if (l) { c.SetLogz(kTRUE);
      //printf("XXX2: set logz \n");
   } else { c.SetLogz(kFALSE); }

   h1->Draw();
   h2->Draw("sames");

   c.Modified();
   c.Update();

   TPaveStats *stats = (TPaveStats*) h2->FindObject("stats");

   if (stats) {
      stats->SetX1NDC(0.84);
      stats->SetX2NDC(0.99);
      stats->SetY1NDC(0.10);
      stats->SetY2NDC(0.50);
   } else {
      printf("could not find stats\n");
      return;
   }

   string fName = path + "/c_combo.png";
   printf("path: %s\n", fName.c_str());

   c.SetName(cName.c_str());
   c.SetTitle(cName.c_str());

   TText signature;
   signature.SetTextSize(0.03);
   signature.DrawTextNDC(0, 0, fSignature.c_str());

   if (TPRegexp(pattern).MatchB(fName.c_str())) {
      c.SaveAs(fName.c_str());
      gSystem->Chmod(fName.c_str(), 0775);
   } else {
      //Warning("SaveAllAs", "Histogram %s name does not match pattern. Skipped", fName.c_str());
   }
} //}}}
