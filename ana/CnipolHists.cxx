/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolHists.h"

#include "utils/utils.h"

#include "TF1.h"
#include "TFitResultPtr.h"
#include "THStack.h"
#include "TProfile.h"

#include "AnaInfo.h"
#include "MeasInfo.h"
#include "AsymRoot.h"

ClassImp(CnipolHists)

using namespace std;

/** Default constructor. */
CnipolHists::CnipolHists() : DrawObjContainer()
{
   BookHists();
}


CnipolHists::CnipolHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
}


/** Default destructor. */
CnipolHists::~CnipolHists()
{
}


/** */
void CnipolHists::BookHists()
{
   string shName;
   TH1*   hist;

   fDir->cd();

   //char  formula[100], fname[100];
   //sprintf(formula, "(%f/sqrt(x+130))-23", RunConst::E2T);

   // Time vs Energy from amplitude
   shName = "hTimeVsEnergyA";
   o[shName] = new TH2S(shName.c_str(), shName.c_str(), 80, 100, 1700, 80, 20, 100);
   ((TH1*) o[shName])->SetOption("colz LOGZ NOIMG");
   ((TH1*) o[shName])->SetTitle("; Deposited Energy, keV; Time, ns;");

   //sprintf(fname, "banana_nominal");
   //TF1 *banana_cut_l = new TF1(fname, formula, 0, 1500);
   //((TH1D*) o[shName])->GetListOfFunctions()->Add(banana_cut_l);

   // TOF vs Kinematic Energy
   shName = "hTofVsKinEnergyA";
   //o[shName] = new TH2F(shName, shName, 255, 0, 1785, 100, 20, 120);
   o[shName] = new TH2F(shName.c_str(), shName.c_str(), 80, 100, 1700, 60, 10, 110);
   ((TH1*) o[shName])->SetTitle("; Kinematic Energy, keV; ToF, ns;");
   ((TH1*) o[shName])->SetOption("colz LOGZ NOIMG");

   // Energy spectrum. Projection of hTofVsKinEnergyA
   shName = "hKinEnergyA";
   o[shName] = new TH1D(shName.c_str(), shName.c_str(), 80, 100, 1700);
   ((TH1*) o[shName])->SetOption("E1");
   ((TH1*) o[shName])->SetTitle("; Kinematic Energy, keV; Events;");

   // Kinematic Energy
   shName = "hKinEnergyA_o";
   hist = new TH1F(shName.c_str(), shName.c_str(), 25, 22.5, 1172.2);
   hist->SetOption("E1 NOIMG");
   hist->SetTitle("; Kinematic Energy, keV; Events;");
   o[shName] = hist;
   fhKinEnergyA_o = hist;

   // Spin vs Strip Id
   shName = "hSpinVsChannel";
   hist = new TH2I(shName.c_str(), shName.c_str(), N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5, N_SPIN_STATES, -1.5, 1.5);
   hist->SetOption("colz NOIMG");
   hist->SetTitle("; Channel Id; Spin State;");
   o[shName] = hist;
   fhSpinVsChannel = hist;

   // Spin vs Bunch Id
   shName = "hSpinVsBunch";
   hist = new TH2I(shName.c_str(), shName.c_str(), N_BUNCHES, 0.5, N_BUNCHES+0.5, N_SPIN_STATES, -1.5, 1.5);
   hist->SetOption("colz NOIMG");
   hist->SetTitle("; Bunch Id; Spin State;");
   o[shName] = hist;
   fhSpinVsBunch = hist;

   // Event count vs channel id
   shName = "hEventsVsChannel";
   o[shName] = new TH1I(shName.c_str(), shName.c_str(), N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   //((TH1*) o[shName])->SetOption("NOIMG");
   ((TH1*) o[shName])->SetTitle("; Channel Id; Events;");

   // Time vs Energy from amplitude
   //shName = "hTimeVsFunnyEnergyA";
   //o[shName] = new TH2F(shName.c_str(), shName.c_str(), 100, 0, 2000, 100, 0, 100);
   //((TH2F*) o[shName])->SetOption("colz LOGZ NOIMG");
   //((TH2F*) o[shName])->SetTitle(";Funny Energy, keV;Time, ns;");

   //sprintf(fname, "banana_nominal_funny";
   //banana_cut_l = new TF1(fname, formula, 0, 1500);
   //((TH1D*) o[shName])->GetListOfFunctions()->Add(banana_cut_l);

   //shName = "hFitMeanTimeVsEnergyA";
   //o[shName] = new TH1D(shName.c_str(), shName.c_str(), 100, 0, 2000);
   //((TH1*) o[shName])->SetOption("NOIMG");
   //((TH1*) o[shName])->SetTitle(";Deposited Energy, keV;Mean Time, ns;");
   //((TH1*) o[shName])->GetYaxis()->SetRangeUser(0, 120);

   //shName = "hDLVsEnergyA";
   //o[shName] = new TH2F(shName.c_str(), shName.c_str(), 100, 0, 2000, 100, 0, 400);
   //((TH1*) o[shName])->SetOption("colz NOIMG");

   //shName = "hDLVsTotalEnergy";
   //o[shName] = new TH2F(shName.c_str(), shName.c_str(), 100, 0, 2000, 100, 0, 400);
   //((TH1*) o[shName])->SetOption("colz NOIMG");

   //shName = "hDLVsTime";
   //o[shName] = new TH2F(shName.c_str(), shName.c_str(), 100, 0, 100, 100, 0, 400);
   //((TH1*) o[shName])->SetOption("colz NOIMG");

   //shName = "hTotalEnergyVsEnergyA";
   //o[shName] = new TH2F(shName.c_str(), shName.c_str(), 100, 0, 2000, 100, 0, 2000);
   //((TH1*) o[shName])->SetOption("colz NOIMG");

   //shName = "hTof";
   //o[shName] = new TH1D(shName.c_str(), shName.c_str(), 100, 0, 100);
   //((TH1*) o[shName])->SetOption("NOIMG");

   shName = "hLongiTimeDiffVsEnergyA";
   hist = new TH2I(shName.c_str(), shName.c_str(), 40, 200, 800, 40, -20, 20);
   hist->SetOption("colz");
   hist->SetTitle("; Deposited Energy, keV; Time Diff, ns;");
   o[shName] = hist;

   shName = "hLongiTimeDiffVsEnergyA_pfx";
   hist = new TProfile(shName.c_str(), shName.c_str(), 40, 200, 800, -20, 20);
   hist->SetTitle("; Deposited Energy, keV; Time Diff, ns;");
   o[shName] = hist;

   shName = "hLongiTimeDiff";
   hist = new TH1D(shName.c_str(), shName.c_str(), 40, -20, 20);
   hist->SetOption("hist");
   hist->SetTitle("; Time Diff, ns; Events;");
   o[shName] = hist;


   DrawObjContainer        *oc;
   DrawObjContainerMapIter  isubdir;

   ChannelSetIter iCh = gMeasInfo->fSiliconChannels.begin();

   for (; iCh!=gMeasInfo->fSiliconChannels.end(); ++iCh) {

      string sChId("  ");
      sprintf(&sChId[0], "%02d", *iCh);

      string dName = "channel" + sChId;

      isubdir = d.find(dName);

      if ( isubdir == d.end()) { // if dir not found
         oc = new DrawObjContainer();
         oc->fDir = new TDirectoryFile(dName.c_str(), dName.c_str(), "", fDir);
      } else {
         oc = isubdir->second;
      }

      oc->fDir->cd();

      //sprintf(fname, "banana_nominal_ch%02d";
      //banana_cut_l = new TF1(fname, formula, 0, 1500);

      // Time vs Energy from amplitude
      shName = "hTimeVsEnergyA_ch" + sChId;
      hist = new TH2F(shName.c_str(), shName.c_str(), 80, 100, 1700, 80, 20, 100);
		hist->SetOption("colz LOGZ");
      hist->SetTitle("; Deposited Energy, keV; Time, ns;");
      //((TH1*) oc->o[shName])->GetListOfFunctions()->Add(banana_cut_l);
      oc->o[shName] = hist;
      fhTimeVsEnergyA_ch[*iCh-1] = hist;

      // TOF vs Kinematic Energy
      shName = "hTofVsKinEnergyA_ch" + sChId;
      oc->o[shName] = new TH2F(shName.c_str(), shName.c_str(), 80, 100, 1700, 60, 10, 110);
      ((TH1*) oc->o[shName])->SetOption("colz LOGZ NOIMG");
      ((TH1*) oc->o[shName])->SetTitle("; Kinematic Energy, keV; ToF, ns;");

      shName = "hKinEnergyA_ch" + sChId;
      oc->o[shName] = new TH1D(shName.c_str(), shName.c_str(), 80, 100, 1700);
      ((TH1*) oc->o[shName])->SetOption("E1 NOIMG");
      ((TH1*) oc->o[shName])->SetTitle("; Kinematic Energy, keV; ;");

      //sprintf(hName, "hFitMeanTimeVsEnergyA_ch%02d", *iCh);
      //oc->o[hName] = new TH1D(hName, hName, 80, 100, 1700);
      //((TH2F*) oc->o[hName])->SetOption("E1 NOIMG");
      //((TH1*) oc->o[hName])->SetTitle(";Deposited Energy, keV;Mean Time, ns;");
      //((TH1*) oc->o[hName])->GetYaxis()->SetRangeUser(0, 120);

      //sprintf(hName, "hDLVsEnergyA_ch%02d", *iCh);
      //oc->o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 400);
      //((TH1*) oc->o[hName])->SetOption("colz NOIMG");
      ////((TH2F*) oc->o[hName])->GetYaxis()->SetRangeUser(0, 20000);

      //sprintf(hName, "hDLVsTotalEnergy_ch%02d", *iCh);
      //oc->o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 400);
      //((TH1*) oc->o[hName])->SetOption("colz NOIMG");

      //sprintf(hName, "hDLVsTime_ch%02d", *iCh);
      //oc->o[hName] = new TH2F(hName, hName, 100, 0, 100, 100, 0, 400);
      //((TH1*) oc->o[hName])->SetOption("colz NOIMG");

      //sprintf(hName, "hTotalEnergyVsEnergyA_ch%02d", *iCh);
      //oc->o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 2000);
      //((TH1*) oc->o[hName])->SetOption("colz NOIMG");

      // Delim (time) vs Spin vs Channel Id
      shName = "hSpinVsDelim_ch" + sChId;
      oc->o[shName] = new TH2I(shName.c_str(), shName.c_str(), 1, 0, 1, N_SPIN_STATES, -1.5, 1.5);
      ((TH1*) oc->o[shName])->SetOption("NOIMG");

      SpinStateSetIter iSS = gRunConfig.fSpinStates.begin();
   
      for (; iSS!=gRunConfig.fSpinStates.end(); ++iSS) {
   
         string sSS = gRunConfig.AsString(*iSS);

         // TOF vs Kinematic Energy
         shName = "hTofVsKinEnergyA_ch" + sChId + "_" + sSS;
         oc->o[shName] = new TH2F(shName.c_str(), shName.c_str(), 80, 100, 1700, 60, 10, 110);
		   ((TH1*) oc->o[shName])->SetOption("colz LOGZ NOIMG");
         ((TH1*) oc->o[shName])->SetTitle(";Deposited Energy, keV;Time, ns;");

         shName = "hLongiTimeDiffVsEnergyA_ch" + sChId + "_" + sSS;
         oc->o[shName] = new TH2I(shName.c_str(), shName.c_str(), 40, 200, 800, 40, -20, 20);
         ((TH1*) oc->o[shName])->SetOption("colz NOIMG");
         ((TH1*) oc->o[shName])->SetTitle(";Deposited Energy, keV;Time Diff, ns;");
      }

      shName = "hLongiTimeDiffVsEnergyA_ch" + sChId;
      oc->o[shName] = new TH2I(shName.c_str(), shName.c_str(), 40, 200, 800, 40, -20, 20);
      ((TH1*) oc->o[shName])->SetOption("colz NOIMG");
      ((TH1*) oc->o[shName])->SetTitle(";Deposited Energy, keV;Time Diff, ns;");

      shName = "hLongiTimeDiffVsEnergyA_pfx_ch" + sChId;
      oc->o[shName] = new TProfile(shName.c_str(), shName.c_str(), 40, 200, 800, -20, 20);
      ((TH1*) oc->o[shName])->SetOption("NOIMG");
      ((TH1*) oc->o[shName])->SetTitle(";Deposited Energy, keV;Time Diff, ns;");

      shName = "hLongiTimeDiff_ch" + sChId;
      oc->o[shName] = new TH1D(shName.c_str(), shName.c_str(), 40, -20, 20);
      ((TH1*) oc->o[shName])->SetOption("hist");
      ((TH1*) oc->o[shName])->SetTitle(";Time Diff, ns;Events;");

      // If this is a new directory then we need to add it to the list
      if ( isubdir == d.end()) {
         d[dName] = oc;
      }
   }

   //delete banana_cut_l;

   // Speed up
   iCh = gMeasInfo->fSiliconChannels.begin();

   for (; iCh!=gMeasInfo->fSiliconChannels.end(); ++iCh) {

      string sChId("  ");
      sprintf(&sChId[0], "%02d", *iCh);

      DrawObjContainer *oc_ch = d.find("channel" + sChId)->second;

      //fhTofVsKinEnergyA_ch[*iCh-1] = (TH2*) oc_ch->o.find("hTofVsKinEnergyA_ch" + sChId)->second;
      fhSpinVsDelim_ch[*iCh-1]     = (TH1*) oc_ch->o.find("hSpinVsDelim_ch"     + sChId)->second;

      SpinStateSetIter iSS = gRunConfig.fSpinStates.begin();

      for (; iSS!=gRunConfig.fSpinStates.end(); ++iSS) {

         string   sSS = gRunConfig.AsString( *iSS );
         UShort_t nSS = gRunConfig.AsIndex(*iSS);

         fhTofVsKinEnergyA_ch_ss[*iCh-1][nSS]        = (TH2*) oc_ch->o.find("hTofVsKinEnergyA_ch" + sChId + "_" + sSS)->second;
         fhLongiTimeDiffVsEnergyA_ch_ss[*iCh-1][nSS] = (TH2*) oc_ch->o.find("hLongiTimeDiffVsEnergyA_ch" + sChId + "_" + sSS)->second;
      }
   }

}


/** */
void CnipolHists::PreFill()
{
   char dName[256];
   char hName[256];

   set<UShort_t>::const_iterator iCh;
   set<UShort_t>::const_iterator iChB = gMeasInfo->fSiliconChannels.begin();
   set<UShort_t>::const_iterator iChE = gMeasInfo->fSiliconChannels.end();

   for (iCh=iChB; iCh!=iChE; ++iCh) {

      sprintf(dName, "channel%02d", *iCh);
      DrawObjContainer *oc = d.find(dName)->second;

      sprintf(hName, "hSpinVsDelim_ch%02d", *iCh);
      ((TH1*) oc->o[hName])->SetBins(gNDelimeters, 0, gNDelimeters, N_SPIN_STATES, -1.5, 1.5);
   }
}


/** */
void CnipolHists::Fill(ChannelEvent *ch)
{
   UChar_t chId  = ch->GetChannelId();
   //UChar_t detId = ch->GetDetectorId();
   UChar_t bId = ch->GetBunchId() + 1;

   // by detector id
   //DrawObjContainer &sd1 = d["Kinema"];

   // fill these if only pass the carbon mass cut
   //((TH1F*) d["Kinema2"]->o["energy_spectrum_all"])->Fill( fabs(ch->GetMandelstamT()) );
   //sprintf(hName,"energy_spectrum_det%d", detId);
   //((TH1F*) d["Kinema2"]->o[hName])->Fill( fabs(ch->GetMandelstamT()) );

   // by channel id
   //string sChId("  ");
   //sprintf(&sChId[0], "%02d", chId);

   //DrawObjContainer *sd = d.find("channel" + sChId)->second;

   string   sSS = gRunConfig.AsString( gMeasInfo->GetBunchSpin(bId) );
   UShort_t nSS = gRunConfig.AsIndex(  gMeasInfo->GetBunchSpin(bId) );

   // Full kinematic carbon kinEnergy
   Float_t depEnergy    = ch->GetEnergyA();
   //Float_t kinEnergyEst = ch->GetKinEnergyAEstimate();
   //Float_t kinEnergyEst = ch->GetKinEnergyAEstimateEDepend();
   //Float_t kinEnergy = ch->GetKinEnergyA();
   Float_t kinEnergy    = ch->GetKinEnergyAEDepend();
   Float_t timeDiff     = ch->GetTdcAdcTimeDiff();

   //Float_t tofEst    = ch->GetTimeOfFlightEstimate();
   Float_t tof       = ch->GetTimeOfFlight();

   //((TH1*) o.find("hKinEnergyA_o")->second) -> Fill(kinEnergy);
   //((TH1*) sd->o.find("hTimeVsEnergyA_ch" + sChId)->second) -> Fill(depEnergy, ch->GetTime());
   //((TH2*) sd->o.find("hTofVsKinEnergyA_ch" + sChId)->second) -> Fill(kinEnergy, tof);
   //((TH2*) sd->o.find("hTofVsKinEnergyA_ch" + sChId + "_" + sSS)->second) -> Fill(kinEnergy, tof);
   //((TH2*) sd->o.find("hLongiTimeDiffVsEnergyA_ch" + sChId + "_" + sSS)->second) -> Fill(depEnergy, timeDiff);

   fhKinEnergyA_o  -> Fill(kinEnergy);
   fhSpinVsChannel -> Fill(chId, gMeasInfo->GetBunchSpin(bId));
   fhSpinVsBunch   -> Fill(bId,  gMeasInfo->GetBunchSpin(bId));

   fhTimeVsEnergyA_ch[chId-1] -> Fill(depEnergy, ch->GetTime());
   //fhTofVsKinEnergyA_ch[chId-1] -> Fill(kinEnergy, tof);

   fhTofVsKinEnergyA_ch_ss[chId-1][nSS]        -> Fill(kinEnergy, tof);
   fhLongiTimeDiffVsEnergyA_ch_ss[chId-1][nSS] -> Fill(depEnergy, timeDiff);

   //ds XXX
   //UShort_t tstep = 0;

   //if (gMeasInfo->Run == 5) {
   //   tstep = ch->GetDelimiterId();
   //   //NDcounts[(int)(st/12)][event->bid][TgtIndex[delim]]++;
   //} else if (gMeasInfo->Run >= 6) {
   //   UInt_t ttime = ch->GetRevolutionId()/RHIC_REVOLUTION_FREQ;

   //   if (ttime < MAXDELIM) {
   //      tstep = TgtIndex[ttime];
   //      //++cntr.good[TgtIndex[ttime]];
   //      //NDcounts[(int)(st/12)][event->bid][TgtIndex[ttime]]++;
   //   } else if (!gAnaInfo->HasAlphaBit()) {
   //      Error("Fill", "Time constructed from revolution #%d exeeds MAXDELIM=%d defined\n" \
   //            "Perhaps calibration data? Try running with --calib option", ttime, MAXDELIM);
   //   }
   //} else {
   //   Warning("Fill", "Target tstep size is not defined for Run %d", gMeasInfo->Run);
   //}

   //int ss_code = gSpinPattern[bId] == 1 ? 0 : (gSpinPattern[bId] == -1 ? 1 : 2);
   //cntr_tgt.reg.NStrip[tstep][ss_code][chId - 1]++;
   
   UInt_t ttime = ch->GetRevolutionId()/RHIC_REVOLUTION_FREQ;

   //((TH2F*) sd->o["hSpinVsDelim_ch"+sChId])->Fill(ch->GetDelimiterId(), gSpinPattern[bId]);

   //((TH2*) sd->o.find("hSpinVsDelim_ch" + sChId)->second)->Fill(ttime, gMeasInfo->GetBunchSpin(bId));
   fhSpinVsDelim_ch[chId-1] -> Fill(ttime, gMeasInfo->GetBunchSpin(bId));

   //((TH2*)     o["hTimeVsFunnyEnergyA"])            ->Fill(ch->GetFunnyEnergyA(), ch->GetTime());

   //((TH2*) sd->o["hDLVsEnergyA_ch"+sChId])          ->Fill(depEnergy,     kinEnergy    - depEnergy);
   //((TH2*)     o["hDLVsEnergyA"])                   ->Fill(depEnergy,     kinEnergyEst - depEnergy);
   //((TH2*) sd->o["hDLVsTotalEnergy_ch"+sChId])      ->Fill(kinEnergy,     kinEnergy    - depEnergy);
   //((TH2*)     o["hDLVsTotalEnergy"])               ->Fill(kinEnergyEst,  kinEnergyEst - depEnergy);
   //((TH2*) sd->o["hDLVsTime_ch"+sChId])             ->Fill(ch->GetTime(), kinEnergy    - depEnergy);
   //((TH2*)     o["hDLVsTime"])                      ->Fill(ch->GetTime(), kinEnergyEst - depEnergy);
   //((TH2*) sd->o["hTotalEnergyVsEnergyA_ch"+sChId]) ->Fill(depEnergy,     kinEnergy);
   //((TH2*)     o["hTotalEnergyVsEnergyA"])          ->Fill(depEnergy,     kinEnergyEst);

   //((TH1*)     o["hTof"])                              ->Fill(tofEst);
}


/** */
void CnipolHists::FillDerived()
{
   Info("FillDerived", "Starting...");

   // Fill derivative histograms
   TH1* hKinEnergyA      = (TH1*) o["hKinEnergyA"];
   TH2* hTimeVsEnergyA   = (TH2*) o["hTimeVsEnergyA"];
   TH2* hTofVsKinEnergyA = (TH2*) o["hTofVsKinEnergyA"];
   TH1* hEventsVsChannel = (TH1*) o["hEventsVsChannel"];

   TH2* hLongiTimeDiffVsEnergyA     = (TH2*) o["hLongiTimeDiffVsEnergyA"];
   TH1* hLongiTimeDiff              = (TH1*) o["hLongiTimeDiff"];
   
   set<UShort_t>::const_iterator iCh;
   set<UShort_t>::const_iterator iChB = gMeasInfo->fSiliconChannels.begin();
   set<UShort_t>::const_iterator iChE = gMeasInfo->fSiliconChannels.end();

   for (iCh=iChB; iCh!=iChE; ++iCh) {

      string sChId("  ");
      sprintf(&sChId[0], "%02d", *iCh);

      DrawObjContainer *oc = d.find("channel" + sChId)->second;

      // now loop over spin states and add up histograms
      TH2* hTofVsKinEnergyA_ch        = (TH2*) oc->o["hTofVsKinEnergyA_ch" + sChId];
      //TH2* hTofVsKinEnergyA_test_ch   = (TH2*) oc->o["hTofVsKinEnergyA_test_ch" + sChId];
      TH2* hLongiTimeDiffVsEnergyA_ch = (TH2*) oc->o["hLongiTimeDiffVsEnergyA_ch" + sChId];

      SpinStateSetIter iSS = gRunConfig.fSpinStates.begin();

      for (; iSS!=gRunConfig.fSpinStates.end(); ++iSS) {
   
         string sSS = gRunConfig.AsString(*iSS);

         TH2* hLongiTimeDiffVsEnergyA_ch_ss = (TH2*) oc->o["hLongiTimeDiffVsEnergyA_ch" + sChId + "_" + sSS];
         hLongiTimeDiffVsEnergyA_ch->Add(hLongiTimeDiffVsEnergyA_ch_ss);

         TH2* hTofVsKinEnergyA_ch_ss = (TH2*) oc->o["hTofVsKinEnergyA_ch" + sChId + "_" + sSS];
         hTofVsKinEnergyA_ch->Add(hTofVsKinEnergyA_ch_ss);
      }

      TH2* hTimeVsEnergyA_ch = (TH2*) oc->o["hTimeVsEnergyA_ch" + sChId];
      hTimeVsEnergyA->Add(hTimeVsEnergyA_ch);

      hTofVsKinEnergyA->Add(hTofVsKinEnergyA_ch);

      // Fill final kinematic histograms using calib constants
      //TH2* hTofVsKinEnergyA_ch = (TH2*) oc->o["hTofVsKinEnergyA_ch"+sChId];
      //ConvertRawToKin(hTVsA_ch, hTofVsKinEnergyA_ch, *iCh);

      hEventsVsChannel->SetBinContent(*iCh, hTofVsKinEnergyA_ch->GetEntries());
     
      // Fill hKinEnergyA as a projection of hTofVsKinEnergyA
      TH1*  hKinEnergyA_ch = (TH1*) oc->o["hKinEnergyA_ch" + sChId];
      TH1D* hProjTmp       = hTofVsKinEnergyA_ch->ProjectionX();
      utils::CopyBinContentError(hProjTmp, hKinEnergyA_ch);
      hKinEnergyA->Add(hKinEnergyA_ch);

      // longitudinal hists
      TH1* hLongiTimeDiffVsEnergyA_pfx_ch = (TH1*) oc->o["hLongiTimeDiffVsEnergyA_pfx_ch" + sChId];
      TProfile *hProfTmp = hLongiTimeDiffVsEnergyA_ch->ProfileX();
      hLongiTimeDiffVsEnergyA_pfx_ch->Add(hProfTmp);

      TH1* hLongiTimeDiff_ch = (TH1*) oc->o["hLongiTimeDiff_ch" + sChId];
      hProjTmp = hLongiTimeDiffVsEnergyA_ch->ProjectionY();
      hLongiTimeDiff_ch->Add(hProjTmp);

      hLongiTimeDiffVsEnergyA->Add(hLongiTimeDiffVsEnergyA_ch);

      hLongiTimeDiff->Add(hLongiTimeDiff_ch);
   }

   TProfile* hLongiTimeDiffVsEnergyA_pfx = (TProfile*) o["hLongiTimeDiffVsEnergyA_pfx"];

   TProfile *hProfTmp = hLongiTimeDiffVsEnergyA->ProfileX();
   hLongiTimeDiffVsEnergyA_pfx->Add(hProfTmp);
}


/** */
void CnipolHists::PostFill()
{
   Info("PostFill", "Starting...");

   // Fit energy slope with an exponential func
   TF1 *fitfunc = new TF1("fitfunc", "expo", 450, 850);

   //fitfunc->SetParNames("slope");
   fitfunc->SetParameter(0, 0);
   fitfunc->SetParameter(1, 0);
   fitfunc->SetParLimits(1, -1, 1);

   TH1* hKinEnergyA = (TH1*) o["hKinEnergyA"];
      
   if (hKinEnergyA->Integral() <= 0) {
      Error("PostFill", "Kin. energy distribution is empty");
      return;
   }

   TFitResultPtr fitres = hKinEnergyA->Fit("fitfunc", "M E S R");

   if ( fitres.Get() && fitres->Ndf()) {
      gAnaMeasResult->fFitResEnergySlope = fitres;
   } else {
      Error("PostFill", "Something is wrong with energy slope fit");
      hKinEnergyA->GetListOfFunctions()->Clear();
   }

   delete fitfunc;

   // Fit channel histograms
   ChannelSetIter iCh = gMeasInfo->fSiliconChannels.begin();

   for (; iCh!=gMeasInfo->fSiliconChannels.end(); ++iCh) {

      string sChId("  ");
      sprintf(&sChId[0], "%02d", *iCh);

      DrawObjContainer *oc = d.find("channel"+sChId)->second;

      TH1* hKinEnergyA_ch = (TH1*) oc->o["hKinEnergyA_ch" + sChId];
      
      if (hKinEnergyA_ch->Integral() <= 0) {
         Error("PostFill", "Kin. energy distribution for channel %s is empty", sChId.c_str());
         continue;
      }

      TF1 *fitfunc = new TF1("fitfunc", "expo", 450, 850);

      //fitfunc->SetParNames("slope");
      fitfunc->SetParameter(0, 0);
      fitfunc->SetParameter(1, 0);
      fitfunc->SetParLimits(1, -1, 1);

      TFitResultPtr fitres = hKinEnergyA_ch->Fit("fitfunc", "M E S R");

      if ( fitres.Get() && fitres->Ndf()) {
         // do nothing yet
      } else {
         Error("PostFill", "Something is wrong with energy slope fit for channel %s", sChId.c_str());
         hKinEnergyA_ch->GetListOfFunctions()->Clear();
      }

      delete fitfunc;
   }
}


/** */
void CnipolHists::SaveAllAs(TCanvas &c, std::string pattern, string path, Bool_t thumbs)
{
   //Info("SaveAllAs", "executing...");
   DrawObjContainer::SaveAllAs(c, pattern, path, thumbs);

   //ds XXX
   return;

   string strThumb = thumbs ? "_thumb" : "" ;

   // Draw superimposed for all channels
   set<UShort_t>::const_iterator iCh;
   set<UShort_t>::const_iterator iChB = gMeasInfo->fSiliconChannels.begin();
   set<UShort_t>::const_iterator iChE = gMeasInfo->fSiliconChannels.end();

   for (iCh=iChB; iCh!=iChE; ++iCh)
	{
      string sSi("  ");
      sprintf(&sSi[0], "%02d", *iCh);
      string dName = "channel" + sSi;
      string cName = "c_combo_ch" + sSi + strThumb;

      DrawObjContainer* oc = d.find(dName)->second;

      THStack hstack(cName.c_str(), cName.c_str());

      TH1* h1 = (TH1*) oc->o["hTimeVsEnergyA_ch" + sSi];
		hstack.Add(h1);

      TH1* h2 = (TH1*) oc->o["hFitMeanTimeVsEnergyA_ch" + sSi];
		hstack.Add(h2);

      string subPath = path + "/" + dName;

		SaveHStackAs(c, hstack, subPath);
		//DrawObjContainer::SaveHStackAs(c, hstack, subPath);
   }
}


/** */
void CnipolHists::ConvertRawToKin(TH2* hRaw, TH2* hKin, UShort_t chId)
{
   for (Int_t ibx=0; ibx<=hRaw->GetNbinsX(); ++ibx) {
      for (Int_t iby=0; iby<=hRaw->GetNbinsY(); ++iby) {

         Double_t bcont  = hRaw->GetBinContent(ibx, iby);
         Double_t bAdc   = hRaw->GetXaxis()->GetBinCenter(ibx);
         Double_t bTdc   = hRaw->GetYaxis()->GetBinCenter(iby);

         Double_t kinE   = gAsymRoot->fEventConfig->fCalibrator->GetEnergyA(bAdc, chId);
         Double_t kinToF = gAsymRoot->fEventConfig->fCalibrator->GetTimeOfFlight(bTdc, chId);
         
         //Int_t bin = hKin->FindBin(kinE, kinToF);
         hKin->Fill(kinE, kinToF, bcont);
      }
   }
}
