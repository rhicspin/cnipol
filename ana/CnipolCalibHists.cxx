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
   char hName[256];

   sprintf(hName, "hDLVsChannel");
   o[hName]      = new TH1F(hName, hName, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   ((TH1*) o[hName])->SetTitle(";Channel Id;Dead Layer, #mug/cm^{2};");

   sprintf(hName, "hT0VsChannel");
   o[hName]      = new TH1F(hName, hName, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   ((TH1*) o[hName])->SetTitle(";Channel Id;T0, ns;");

   sprintf(hName, "hChi2NdfVsChannel");
   o[hName]      = new TH1F(hName, hName, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   ((TH1*) o[hName])->SetTitle(";Channel Id;#chi^{2}/ndf;");

   sprintf(hName, "hFitStatusVsChannel");
   o[hName]      = new TH1I(hName, hName, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   ((TH1*) o[hName])->SetTitle(";Channel Id;Fit Status;");

} //}}}


/** */
void CnipolCalibHists::Fill(ChannelEvent *ch, string sid)
{ //{{{
   //UChar_t      chId = ch->GetChannelId();
} //}}}


/** */
void CnipolCalibHists::FillPreProcess(ChannelEvent *ch)
{
}


/** */
void CnipolCalibHists::PostFill()
{ //{{{

   Calibrator *calibrator = gAsymRoot->fEventConfig->GetCalibrator();

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
