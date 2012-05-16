/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "AsymRoot.h"
#include "CnipolCalibHists.h"
#include "ChannelEventId.h"


ClassImp(CnipolCalibHists)

using namespace std;


/** Default constructor. */
CnipolCalibHists::CnipolCalibHists() : DrawObjContainer()
{
   BookHists();
}


/** */
CnipolCalibHists::CnipolCalibHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
}


/** Default destructor. */
CnipolCalibHists::~CnipolCalibHists()
{
}


void CnipolCalibHists::BookHists(std::string cutid)
{ //{{{
   string shName;
   TH1*   hist;

   fDir->cd();

   shName = "hDLVsChannel";
   hist = new TH1F(shName.c_str(), shName.c_str(), N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   hist->SetTitle("; Channel Id; Dead Layer, #mug/cm^{2};");
   hist->SetOption("E1 GRIDX");
   o[shName] = hist;

   shName = "hT0VsChannel";
   hist = new TH1F(shName.c_str(), shName.c_str(), N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   hist->SetTitle("; Channel Id; T0, ns;");
   hist->SetOption("E1 GRIDX");
   o[shName] = hist;

   shName = "hChi2NdfVsChannel";
   hist = new TH1F(shName.c_str(), shName.c_str(), N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   hist->SetTitle("; Channel Id; #chi^{2}/ndf;");
   hist->SetOption("hist XY GRIDX");
   o[shName] = hist;

   shName = "hFitStatusVsChannel";
   hist = new TH1I(shName.c_str(), shName.c_str(), N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   hist->SetTitle("; Channel Id; Fit Status;");
   hist->SetOption("hist GRIDX");
   hist->GetYaxis()->SetRangeUser(0, 2.2);
   o[shName] = hist;

   DrawObjContainer        *oc;
   DrawObjContainerMapIter  isubdir;

   for (int iChId=1; iChId<=N_SILICON_CHANNELS; iChId++)
   {
      string sChId(MAX_CHANNEL_DIGITS, ' ');
      sprintf(&sChId[0], "%02d", iChId);

      string dName = "channel" + sChId;

      isubdir = d.find(dName);

      if ( isubdir == d.end()) { // if dir not found
         oc = new DrawObjContainer();
         oc->fDir = new TDirectoryFile(dName.c_str(), dName.c_str(), "", fDir);
      } else {
         oc = isubdir->second;
      }

      oc->fDir->cd();

      shName = "hAdcAmpltd_ch" + sChId;
      hist = new TH1I(shName.c_str(), shName.c_str(), 255, 0, 255);
      hist->SetOption("hist NOIMG");
      hist->SetTitle("; Amplitude, ADC; Events;");
      hist->SetFillColor(kGray);

      shName = "hMeanTimeVsEnergy" + sChId;
      hist = new TH1F(shName.c_str(), shName.c_str(), N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      hist->SetTitle(";Deposited Energy, keV; Mean Time, ns;");
      hist->SetOption("E1 GRIDX");
      oc->o[shName] = hist;

      // If this is a new directory then we need to add it to the list
      if ( isubdir == d.end()) {
         d[dName] = oc;
      }
   }

} //}}}


/** */
void CnipolCalibHists::PostFill()
{ //{{{
   Calibrator *calibrator = gAsymRoot->GetCalibrator();

   //ChannelCalibMapIter iCh  = calibrator->fChannelCalibs.begin();
   //ChannelCalibMapIter iChE = calibrator->fChannelCalibs.end();
   //for (; iCh!=iCh; ++iCh) {
   //}

   TH1* hDLVsChannel        = (TH1*) o["hDLVsChannel"];
   TH1* hT0VsChannel        = (TH1*) o["hT0VsChannel"];
   TH1* hChi2NdfVsChannel   = (TH1*) o["hChi2NdfVsChannel"];
   TH1* hFitStatusVsChannel = (TH1*) o["hFitStatusVsChannel"];

   for (Int_t i=1; i<=hDLVsChannel->GetNbinsX(); ++i) {

      hDLVsChannel->SetBinContent(i, calibrator->GetDLWidth(i));
      hDLVsChannel->SetBinError(i, calibrator->GetDLWidthErr(i));

      hT0VsChannel->SetBinContent(i, calibrator->GetT0Coef(i));
      hT0VsChannel->SetBinError(i, calibrator->GetT0CoefErr(i));

      hChi2NdfVsChannel->SetBinContent(i, calibrator->GetBananaChi2Ndf(i));
      hFitStatusVsChannel->SetBinContent(i, calibrator->GetFitStatus(i));
   }

} //}}}
