/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolScalerHists.h"

ClassImp(CnipolScalerHists)

using namespace std;

/** Default constructor. */
CnipolScalerHists::CnipolScalerHists() : DrawObjContainer()
{
   BookHists();
}


CnipolScalerHists::CnipolScalerHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
}


/** Default destructor. */
CnipolScalerHists::~CnipolScalerHists()
{
}


/** */
void CnipolScalerHists::BookHists(string sid)
{ //{{{
   fDir->cd();

   if (!sid.empty()) sid = "_" + sid;

   char hName[256];

   //TDirectory *dir = new TDirectoryFile("scalers", "scalers", "", fDir);
   //DrawObjContainer *scalers = new DrawObjContainer(dir);

   sprintf(hName, "hBunches%s", sid.c_str());
   o[hName] = new TH1F(hName, hName, 120, 0, 120);
   ((TH1F*) o[hName])->GetXaxis()->SetTitle("Bunch Id");

   sprintf(hName, "hKinEnergy%s", sid.c_str());
   o[hName] = new TH2F(hName, hName, 16, 200, 1000, 64, 0, 128);
   ((TH2F*) o[hName])->SetOption("colz LOGZ");
   ((TH2F*) o[hName])->GetXaxis()->SetTitle("Kinematic Energy, keV");
   ((TH2F*) o[hName])->GetYaxis()->SetTitle("Time, ns");

   string sCh("  ");

   for (int iCh=1; iCh<=TOT_WFD_CH; iCh++) {

      sprintf(&sCh[0], "%02d", iCh);

      // Corresponds to 200 histograms in rhic2hbook.f
      sprintf(hName, "hBunches_ch%s%s", sCh.c_str(), sid.c_str());
      o[hName] = new TH1F(hName, hName, 120, 0, 120);
      ((TH1F*) o[hName])->GetXaxis()->SetTitle("Bunch Id");

      // Corresponds to 300 histograms in rhic2hbook.f
      sprintf(hName, "hKinEnergy_ch%s%s", sCh.c_str(), sid.c_str());
      o[hName] = new TH2F(hName, hName, 16, 200, 1000, 64, 0, 128);
      ((TH2F*) o[hName])->SetOption("colz LOGZ");
      ((TH2F*) o[hName])->GetXaxis()->SetTitle("Kinematic Energy, keV");
      ((TH2F*) o[hName])->GetYaxis()->SetTitle("Time, ns");
   }

   //d["scalers"] = scalers;
} //}}}


/** */
void CnipolScalerHists::Print(const Option_t* opt) const
{ //{{{
   opt = "";

   //printf("CnipolHists:\n");
   DrawObjContainer::Print();
} //}}}


/**
 * We assume 1536 entries in the hData arrays.
 */
void CnipolScalerHists::Fill(Long_t *hData, UShort_t chId, std::string sid)
{ //{{{
   if (!sid.empty()) sid = "_" + sid;

   //DrawObjContainer *oc = d["scalers"];

   string sCh("  ");
   char   hName[256];

   sprintf(&sCh[0], "%02d", chId);

   Double_t hData1[128+2];//, hData5[1024+2];

   // convert input array to doubles with under/overflows
   for (int i=0,   j=1; i<128;  i++, j++) hData1[j] = hData[i];
   //for (int i=512, j=1; i<1536; i++, j++) hData5[j] = hData[i];

   sprintf(hName, "hBunches_ch%s%s", sCh.c_str(), sid.c_str());
   TH1* hBunchesChannel = (TH1*) o[hName];
   hBunchesChannel->SetContent(hData1); // -1 because there is an underflow bin

   sprintf(hName, "hBunches%s", sid.c_str());
   TH1* hBunchesAllChannels = (TH1*) o[hName];
   hBunchesAllChannels->Add(hBunchesChannel);

   sprintf(hName, "hKinEnergy_ch%s%s", sCh.c_str(), sid.c_str());
   TH1* hTmpPtr = (TH1*) o[hName];
   //((TH1*) o[hName])->SetContent(hData5);

   for (Int_t ix=1; ix<=hTmpPtr->GetNbinsX(); ix++) {
      for (Int_t iy=1; iy<=hTmpPtr->GetNbinsY(); iy++) {
         
         Int_t ib = 512 + ix-1 + (hTmpPtr->GetNbinsX())*(iy-1);
         double content = hData[ib];
         //printf("ix, iy, ib, content: %d, %d, %d, %f\n", ix, iy, ib, content);
         hTmpPtr->SetBinContent(ix, iy, content);
      }
   }

   sprintf(hName, "hKinEnergy%s", sid.c_str());
   TH1* hKinEnergyAllChannels = (TH1*) o[hName];
   hKinEnergyAllChannels->Add(hTmpPtr);

} //}}}
