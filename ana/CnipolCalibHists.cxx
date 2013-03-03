/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "AsymRoot.h"
#include "CnipolCalibHists.h"
#include "ChannelEventId.h"

#include "utils/utils.h"


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
{
   string shName;
   TH1*   hist;

   fDir->cd();

   TAttMarker styleMarker;
   styleMarker.SetMarkerStyle(kFullCircle);
   styleMarker.SetMarkerSize(0.8);
   styleMarker.SetMarkerColor(kGreen+3);

   shName = "hDLVsChannel";
   hist = new TH1F(shName.c_str(), shName.c_str(), N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   hist->SetTitle("; Channel Id; Dead Layer, #mug/cm^{2};");
   hist->SetOption("P E1 GRIDX");
   styleMarker.Copy(*hist);
   o[shName] = hist;

   shName = "hT0VsChannel";
   hist = new TH1F(shName.c_str(), shName.c_str(), N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   hist->SetTitle("; Channel Id; T0, ns;");
   hist->SetOption("P E1 GRIDX");
   styleMarker.Copy(*hist);
   o[shName] = hist;

   shName = "hChi2NdfVsChannel";
   hist = new TH1F(shName.c_str(), shName.c_str(), N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   hist->SetTitle("; Channel Id; #chi^{2}/ndf;");
   hist->SetOption("P XY GRIDX");
   styleMarker.Copy(*hist);
   o[shName] = hist;

   shName = "hLogChi2NdfVsChannel";
   hist = new TH1F(shName.c_str(), shName.c_str(), N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   hist->SetTitle("; Channel Id; log(#chi^{2}/ndf);");
   hist->SetOption("P GRIDX");
   styleMarker.Copy(*hist);
   o[shName] = hist;

   shName = "hFitStatusVsChannel";
   hist = new TH1I(shName.c_str(), shName.c_str(), N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   hist->SetTitle("; Channel Id; Fit Status;");
   hist->SetOption("hist GRIDX");
   //hist->GetYaxis()->SetRangeUser(0, 2.2);
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

}


/** */
void CnipolCalibHists::PostFill()
{
   Calibrator *calibrator = gAsymRoot->GetCalibrator();

   //ChannelCalibMapIter iCh  = calibrator->fChannelCalibs.begin();
   //ChannelCalibMapIter iChE = calibrator->fChannelCalibs.end();
   //for (; iCh!=iCh; ++iCh) {
   //}

   TH1* hDLVsChannel            = (TH1*) o["hDLVsChannel"];
   TH1* hT0VsChannel            = (TH1*) o["hT0VsChannel"];
   TH1* hChi2NdfVsChannel       = (TH1*) o["hChi2NdfVsChannel"];
   TH1* hLogChi2NdfVsChannel    = (TH1*) o["hLogChi2NdfVsChannel"];
   TH1* hFitStatusVsChannel     = (TH1*) o["hFitStatusVsChannel"];

   for (Int_t i=1; i<=hDLVsChannel->GetNbinsX(); ++i) {

      hDLVsChannel->SetBinContent(i, calibrator->GetDLWidth(i));
      hDLVsChannel->SetBinError(i, calibrator->GetDLWidthErr(i));

      hT0VsChannel->SetBinContent(i, calibrator->GetT0Coef(i));
      hT0VsChannel->SetBinError(i, calibrator->GetT0CoefErr(i));

      hChi2NdfVsChannel->SetBinContent(i, calibrator->GetBananaChi2Ndf(i));
      Double_t logChi2 = calibrator->GetBananaChi2Ndf(i) <= 0 ? 0 : TMath::Log( calibrator->GetBananaChi2Ndf(i)) ;
      hLogChi2NdfVsChannel->SetBinContent(i, logChi2 );

      hFitStatusVsChannel->SetBinContent(i, calibrator->GetFitStatus(i));
   }

   // Visualize the channel selection decision based on chi2
   Double_t meanDL = calibrator->GetMeanChannel().fDLWidth;
   Double_t rmsDL  = calibrator->GetMeanChannel().fDLWidthErr;

   TLine* lineMean = new TLine(1, meanDL, hDLVsChannel->GetNbinsX(), meanDL);
   lineMean->SetLineWidth(2);
   lineMean->SetLineColor(kGreen);

   TLine* lineRMSup = new TLine(1, meanDL+3.0*rmsDL, hDLVsChannel->GetNbinsX(), meanDL+3.0*rmsDL);
   lineRMSup->SetLineWidth(2);
   lineRMSup->SetLineColor(kMagenta);

   TLine* lineRMSdown  = new TLine(1, meanDL-3.0*rmsDL, hDLVsChannel->GetNbinsX(), meanDL-3.0*rmsDL);
   lineRMSdown->SetLineWidth(2);
   lineRMSdown->SetLineColor(kMagenta);

   hDLVsChannel->GetListOfFunctions()->Add(lineMean);
   hDLVsChannel->GetListOfFunctions()->Add(lineRMSup);
   hDLVsChannel->GetListOfFunctions()->Add(lineRMSdown);
   hDLVsChannel->GetListOfFunctions()->SetOwner();

   if (hDLVsChannel->GetYaxis()->GetXmax() < meanDL+3.0*rmsDL)
      hDLVsChannel->SetMaximum(1.1*(meanDL+3.0*rmsDL));

   if (hDLVsChannel->GetYaxis()->GetXmin() < meanDL-3.0*rmsDL)
      hDLVsChannel->SetMinimum((meanDL-3.0*rmsDL));

   // T0
   Double_t meanT0 = calibrator->GetMeanChannel().fT0Coef;
   Double_t rmsT0  = calibrator->GetMeanChannel().fT0CoefErr;

   lineMean = new TLine(1, meanT0, hDLVsChannel->GetNbinsX(), meanT0);
   lineMean->SetLineWidth(2);
   lineMean->SetLineColor(kGreen);

   lineRMSup = new TLine(1, meanT0+3.0*rmsT0, hDLVsChannel->GetNbinsX(), meanT0+3.0*rmsT0);
   lineRMSup->SetLineWidth(2);
   lineRMSup->SetLineColor(kMagenta);

   lineRMSdown  = new TLine(1, meanT0-3.0*rmsT0, hDLVsChannel->GetNbinsX(), meanT0-3.0*rmsT0);
   lineRMSdown->SetLineWidth(2);
   lineRMSdown->SetLineColor(kMagenta);

   hT0VsChannel->GetListOfFunctions()->Add(lineMean);
   hT0VsChannel->GetListOfFunctions()->Add(lineRMSup);
   hT0VsChannel->GetListOfFunctions()->Add(lineRMSdown);
   hT0VsChannel->GetListOfFunctions()->SetOwner();

   if (hT0VsChannel->GetYaxis()->GetXmax() < meanT0+3.0*rmsT0)
      hT0VsChannel->SetMaximum(1.1*(meanT0+3.0*rmsT0));

   if (hT0VsChannel->GetYaxis()->GetXmin() > meanT0-3.0*rmsT0)
      hT0VsChannel->SetMinimum((meanT0-3.0*rmsT0));

   // Visualize the channel selection decision based on chi2
   Double_t meanChi2 = calibrator->GetMeanOfLogsChannel().GetBananaChi2Ndf();
   meanChi2 = meanChi2 < 0 ? 0 : meanChi2;
   Double_t rmsChi2  = calibrator->GetRMSOfLogsBananaChi2Ndf();

   lineMean = new TLine(1, meanChi2, hDLVsChannel->GetNbinsX(), meanChi2);
   lineMean->SetLineWidth(2);
   lineMean->SetLineColor(kGreen);

   TLine* lineRMS  = new TLine(1, meanChi2+3.0*rmsChi2, hDLVsChannel->GetNbinsX(), meanChi2+3.0*rmsChi2);
   lineRMS->SetLineWidth(2);
   lineRMS->SetLineColor(kMagenta);

   hLogChi2NdfVsChannel->GetListOfFunctions()->Add(lineMean);
   hLogChi2NdfVsChannel->GetListOfFunctions()->Add(lineRMS);
   hLogChi2NdfVsChannel->GetListOfFunctions()->SetOwner();

   if (hLogChi2NdfVsChannel->GetYaxis()->GetXmax() < meanChi2+3.0*rmsChi2)
      hLogChi2NdfVsChannel->SetMaximum(1.1*(meanChi2+3.0*rmsChi2));
}
