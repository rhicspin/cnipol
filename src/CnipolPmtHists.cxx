/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolPmtHists.h"


ClassImp(CnipolPmtHists)

using namespace std;

/** Default constructor. */
CnipolPmtHists::CnipolPmtHists() : DrawObjContainer()
{
   BookHists();
}


CnipolPmtHists::CnipolPmtHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
}


/** Default destructor. */
CnipolPmtHists::~CnipolPmtHists()
{
}


/** */
void CnipolPmtHists::BookHists()
{
   char hName[256];

   fDir->cd();

   for (int iChId=N_SILICON_CHANNELS+1; iChId<=N_SILICON_CHANNELS+4; iChId++)
   {
      string sChId("  ");
      sprintf(&sChId[0], "%02d", iChId);

      sprintf(hName, "hTdc_ch%02d", iChId);
      o[hName] = new TH1I(hName, hName, 100, 0, 100);
      ((TH1*) o[hName])->SetOption("hist NOIMG");
      ((TH1*) o[hName])->SetTitle("; TDC; Events;");

      sprintf(hName, "hAdcAmpltd_ch%02d", iChId);
      o[hName] = new TH1I(hName, hName, 255, 0, 255);
      ((TH1*) o[hName])->SetOption("hist NOIMG");
      ((TH1*) o[hName])->SetTitle("; Amplitude, ADC; Events;");

      sprintf(hName, "hTvsA_ch%02d", iChId);
      o[hName] = new TH2I(hName, hName, 255, 0, 255, 90, 0, 90);
      ((TH1*) o[hName])->SetOption("colz LOGZ NOIMG");
      ((TH1*) o[hName])->SetTitle("; Amplitude, ADC; TDC;");

      sprintf(hName, "hTvsI_ch%02d", iChId);
      o[hName] = new TH2I(hName, hName, 255, 0, 255, 90, 0, 90);
      ((TH1*) o[hName])->SetOption("colz LOGZ NOIMG");
      ((TH1*) o[hName])->SetTitle("; Integral, ADC; TDC;");
   }
}


/** */
void CnipolPmtHists::FillPassOne(ChannelEvent *ch)
{
   UChar_t chId = ch->GetChannelId();

   string sChId("  ");
   sprintf(&sChId[0], "%02d", chId);

   ((TH1*) o["hAdcAmpltd_ch" + sChId]) -> Fill(ch->GetAmpltd());
   ((TH1*) o["hTdc_ch"       + sChId]) -> Fill(ch->GetTdc());
   ((TH1*) o["hTvsA_ch"      + sChId]) -> Fill(ch->GetAmpltd(), ch->GetTdc());
   ((TH1*) o["hTvsI_ch"      + sChId]) -> Fill(ch->GetIntgrl(), ch->GetTdc());
}


/** */
void CnipolPmtHists::PostFillPassOne(DrawObjContainer *oc)
{
   Info("PostFillPassOne", "Starting...");

   for (int iChId=N_SILICON_CHANNELS+1; iChId<=N_SILICON_CHANNELS+4; iChId++)
   {
      string sChId("  ");
      sprintf(&sChId[0], "%02d", iChId);

      //sprintf(hName, "hTdc_ch%02d", iChId);
      TFitResultPtr fitres = ((TH1*) o["hTdc_ch"+sChId])->Fit("gaus", "I M S R");

      if (fitres.Get()) {
         //chi2Ndf  = fitres->Ndf() > 0 ? fitres->Chi2()/fitres->Ndf() : -1;
         //sigma    = fitres->Value(0);
         if (iChId == 73) {
            gAnaMeasResult->fPmtV1T0    = fitres->Value(1);
            gAnaMeasResult->fPmtV1T0Err = fitres->FitResult::Error(1);
         } 

         if (iChId == 75) {
            gAnaMeasResult->fPmtS1T0    = fitres->Value(1);
            gAnaMeasResult->fPmtS1T0Err = fitres->FitResult::Error(1);
         } 
         //mean2    = fitres->Value(2);
         //mean2Err = fitres->FitResult::Error(2);
      }
   }
}
