/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolHists.h"

#include "THStack.h"

#include "AnaInfo.h"
#include "RunInfo.h"
#include "AsymRoot.h"

ClassImp(CnipolHists)

using namespace std;

/** Default constructor. */
CnipolHists::CnipolHists() : DrawObjContainer()
{
   BookHists();
   BookHists("_cut1");
   BookHists("_cut2");
}


CnipolHists::CnipolHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
   BookHists("_cut1");
   BookHists("_cut2");
}


/** Default destructor. */
CnipolHists::~CnipolHists()
{
}


/** */
void CnipolHists::BookHists(string cutid)
{ //{{{
   char hName[256];

   fDir->cd();

   //char  formula[100], fname[100];
   //sprintf(formula, "(%f/sqrt(x+130))-23", RunConst::E2T);

   // Time vs Energy from amplitude
   sprintf(hName, "hTimeVsEnergyA%s", cutid.c_str());
   o[hName] = new TH2F(hName, hName, 100, 0, 2000, 60, 0, 120);
   ((TH1*) o[hName])->SetOption("colz LOGZ NOIMG");
   ((TH1*) o[hName])->SetTitle(";Deposited Energy, keV;Time, ns;");

   //sprintf(fname, "banana_nominal%s", cutid.c_str());
   //TF1 *banana_cut_l = new TF1(fname, formula, 0, 1500);
   //((TH1D*) o[hName])->GetListOfFunctions()->Add(banana_cut_l);

   // TOF vs Kinematic Energy
   sprintf(hName, "hTofVsKinEnergyA%s", cutid.c_str());
   //o[hName] = new TH2F(hName, hName, 255, 0, 1785, 100, 20, 120);
   o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 100);
   ((TH1*) o[hName])->SetOption("colz LOGZ NOIMG");
   ((TH1*) o[hName])->SetTitle(";Kinematic Energy, keV;ToF, ns;");

   // Kinematic Energy
   sprintf(hName, "hKinEnergyA_o%s", cutid.c_str());
   o[hName] = new TH1F(hName, hName, 25, 22.5, 1172.2);
   ((TH1*) o[hName])->SetOption("hist NOIMG");
   ((TH1*) o[hName])->SetTitle(";Kinematic Energy, keV;;");

   // Spin vs Strip Id
   sprintf(hName, "hSpinVsChannel%s", cutid.c_str());
   o[hName] = new TH2I(hName, hName, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5, N_SPIN_STATES, -1.5, 1.5);
   ((TH1*) o[hName])->SetOption("colz NOIMG");
   ((TH1*) o[hName])->SetTitle(";Channel Id;Spin State;");

   // Spin vs Bunch Id
   sprintf(hName, "hSpinVsBunch%s", cutid.c_str());
   o[hName] = new TH2I(hName, hName, N_BUNCHES, 0, N_BUNCHES, N_SPIN_STATES, -1.5, 1.5);
   ((TH1*) o[hName])->SetOption("colz NOIMG");
   ((TH1*) o[hName])->SetTitle(";Bunch Id;Spin State;");

   // Time vs Energy from amplitude
   //sprintf(hName, "hTimeVsFunnyEnergyA%s", cutid.c_str());
   //o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 100);
   //((TH2F*) o[hName])->SetOption("colz LOGZ NOIMG");
   //((TH2F*) o[hName])->SetTitle(";Funny Energy, keV;Time, ns;");

   //sprintf(fname, "banana_nominal_funny%s", cutid.c_str());
   //banana_cut_l = new TF1(fname, formula, 0, 1500);
   //((TH1D*) o[hName])->GetListOfFunctions()->Add(banana_cut_l);

   //sprintf(hName, "hFitMeanTimeVsEnergyA%s", cutid.c_str());
   //o[hName] = new TH1D(hName, hName, 100, 0, 2000);
   //((TH1*) o[hName])->SetOption("NOIMG");
   //((TH1*) o[hName])->SetTitle(";Deposited Energy, keV;Mean Time, ns;");
   //((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 120);

   //sprintf(hName, "hDLVsEnergyA%s", cutid.c_str());
   //o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 400);
   //((TH1*) o[hName])->SetOption("colz NOIMG");

   //sprintf(hName, "hDLVsTotalEnergy%s", cutid.c_str());
   //o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 400);
   //((TH1*) o[hName])->SetOption("colz NOIMG");

   //sprintf(hName, "hDLVsTime%s", cutid.c_str());
   //o[hName] = new TH2F(hName, hName, 100, 0, 100, 100, 0, 400);
   //((TH1*) o[hName])->SetOption("colz NOIMG");

   //sprintf(hName, "hTotalEnergyVsEnergyA%s", cutid.c_str());
   //o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 2000);
   //((TH1*) o[hName])->SetOption("colz NOIMG");

   //sprintf(hName, "hTof%s", cutid.c_str());
   //o[hName] = new TH1D(hName, hName, 100, 0, 100);
   //((TH1*) o[hName])->SetOption("NOIMG");

   char dName[256];
   //string sSi("  ");
   DrawObjContainer        *oc;
   DrawObjContainerMapIter  isubdir;

   set<UShort_t>::const_iterator iCh;
   set<UShort_t>::const_iterator iChB = gRunInfo->fSiliconChannels.begin();
   set<UShort_t>::const_iterator iChE = gRunInfo->fSiliconChannels.end();

   for (iCh=iChB; iCh!=iChE; ++iCh) {

      sprintf(dName, "channel%02d", *iCh);

      isubdir = d.find(dName);

      if ( isubdir == d.end()) { // if dir not found
         oc = new DrawObjContainer();
         oc->fDir = new TDirectoryFile(dName, dName, "", fDir);
      } else {
         oc = isubdir->second;
      }

      //sprintf(fname, "banana_nominal%s_ch%02d", cutid.c_str(), *iCh);
      //banana_cut_l = new TF1(fname, formula, 0, 1500);

      // Time vs Energy from amplitude
      sprintf(hName, "hTimeVsEnergyA%s_ch%02d", cutid.c_str(), *iCh);
      oc->o[hName] = new TH2F(hName, hName, 100, 0, 2000, 60, 0, 120);
      ((TH1*) oc->o[hName])->SetTitle(";Deposited Energy, keV;Time, ns;");
      //((TH1*) oc->o[hName])->GetListOfFunctions()->Add(banana_cut_l);

      if (cutid == "_cut2")
		   ((TH1*) oc->o[hName])->SetOption("colz LOGZ");
      else
		   ((TH1*) oc->o[hName])->SetOption("colz LOGZ NOIMG");

      // TOF vs Kinematic Energy
      sprintf(hName, "hTofVsKinEnergyA%s_ch%02d", cutid.c_str(), *iCh);
      oc->o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 100);
      ((TH1*) oc->o[hName])->SetOption("colz LOGZ NOIMG");
      ((TH1*) oc->o[hName])->SetTitle(";Kinematic Energy, keV;ToF, ns;");

      sprintf(hName, "hFitMeanTimeVsEnergyA%s_ch%02d", cutid.c_str(), *iCh);
      oc->o[hName] = new TH1D(hName, hName, 100, 0, 2000);
      ((TH2F*) oc->o[hName])->SetOption("E1 NOIMG");
      ((TH1*) oc->o[hName])->SetTitle(";Deposited Energy, keV;Mean Time, ns;");
      ((TH1*) oc->o[hName])->GetYaxis()->SetRangeUser(0, 120);

      //sprintf(hName, "hDLVsEnergyA%s_ch%02d", cutid.c_str(), *iCh);
      //oc->o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 400);
      //((TH1*) oc->o[hName])->SetOption("colz NOIMG");
      ////((TH2F*) oc->o[hName])->GetYaxis()->SetRangeUser(0, 20000);

      //sprintf(hName, "hDLVsTotalEnergy%s_ch%02d", cutid.c_str(), *iCh);
      //oc->o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 400);
      //((TH1*) oc->o[hName])->SetOption("colz NOIMG");

      //sprintf(hName, "hDLVsTime%s_ch%02d", cutid.c_str(), *iCh);
      //oc->o[hName] = new TH2F(hName, hName, 100, 0, 100, 100, 0, 400);
      //((TH1*) oc->o[hName])->SetOption("colz NOIMG");

      //sprintf(hName, "hTotalEnergyVsEnergyA%s_ch%02d", cutid.c_str(), *iCh);
      //oc->o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 2000);
      //((TH1*) oc->o[hName])->SetOption("colz NOIMG");

      // Delim (time) vs Spin vs Channel Id
      sprintf(hName, "hSpinVsDelim%s_ch%02d", cutid.c_str(), *iCh);
      oc->o[hName] = new TH2I(hName, hName, 1, 0, 1, N_SPIN_STATES, -1.5, 1.5);
      ((TH1*) oc->o[hName])->SetOption("NOIMG");

      // If this is a new directory then we need to add it to the list
      if ( isubdir == d.end()) {
         d[dName] = oc;
      }
   }

   //delete banana_cut_l;
} //}}}


/** */
void CnipolHists::BookHistsExtra(string cutid)
{ //{{{
   //char hName[256];

   //TH2F* hData = (TH2F*) o["hTimeVsEnergyA"+cutid];

   //char  formula[100], fname[100];
   //float sigma = RunConst::M2T * ]*gAnaInfo->MassSigmaAlt :

   //sprintf(formula, "%f/sqrt(x)+(%f)/sqrt(x)", RunConst::E2T, sigma);
   //sprintf(formula, "%f/sqrt(x)", RunConst::E2T);
   //sprintf(fname, "banana_cut_l_ch%d_mode%d", i, j);
   //sprintf(fname, "banana_cut_l");

   //TF1 *banana_cut_l = new TF1(fname, formula, 0, 1500);

   //if (hData) {
      //hData->GetListOfFunctions()->Add(banana_cut_l);
      //hData->GetListOfFunctions()->Add(banana_cut_h);
   //}
} //}}}


/** */
void CnipolHists::Fill(ChannelEvent *ch, string cutid)
{ //{{{

   if (cutid != "_cut2") return;

   UChar_t chId  = ch->GetChannelId();
   //UChar_t detId = ch->GetDetectorId();

   // by detector id
   //DrawObjContainer &sd1 = d["Kinema"];

   //string sDetId(" ");
   //sprintf(&sDetId[0], "%1d", detId);
   //char hName[256];
   
   //if (cutid == "_cut2") { // fill these if only pass the carbon mass cut
   //   //((TH1F*) d["Kinema2"]->o["energy_spectrum_all"])->Fill( fabs(ch->GetMandelstamT()) );
   //   //sprintf(hName,"energy_spectrum_det%d", detId);
   //   //((TH1F*) d["Kinema2"]->o[hName])->Fill( fabs(ch->GetMandelstamT()) );
   //}

   // by channel id
   string sChId("  ");
   sprintf(&sChId[0], "%02d", chId);

   DrawObjContainer *sd = d["channel"+sChId];

   // Full kinematic carbon kinEnergy
   //Float_t kinEnergyEst = ch->GetKinEnergyAEstimate();
   //Float_t kinEnergyEst = ch->GetKinEnergyAEstimateEDepend();
   //Float_t kinEnergy = ch->GetKinEnergyA();
   Float_t kinEnergy    = ch->GetKinEnergyAEDepend();

   //Float_t tofEst    = ch->GetTimeOfFlightEstimate();
   Float_t tof       = ch->GetTimeOfFlight();

   ((TH1*) o["hKinEnergyA_o"+cutid]) -> Fill(kinEnergy);

   ((TH1*) sd->o["hTimeVsEnergyA"+cutid+"_ch"+sChId]) -> Fill(ch->GetEnergyA(), ch->GetTime());
   ((TH2*) sd->o["hTofVsKinEnergyA"+cutid+"_ch"+sChId]) -> Fill(kinEnergy, tof);

   //((TH1*)     o["hKinEnergyA_o"+cutid])              -> Fill(kinEnergy);
   
   UChar_t bId = ch->GetBunchId();

   ((TH1*) o["hSpinVsChannel"+cutid]) -> Fill(chId, gSpinPattern[bId]);
   //((TH1*) o["hSpinVsBunch"+cutid]) -> Fill(bId, gSpinPattern[bId]);

   //ds XXX
   //UShort_t tstep = 0;

   //if (gRunInfo->Run == 5) {
   //   tstep = ch->GetDelimiterId();
   //   //NDcounts[(int)(st/12)][event->bid][TgtIndex[delim]]++;
   //} else if (gRunInfo->Run >= 6) {
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
   //   Warning("Fill", "Target tstep size is not defined for Run %d", gRunInfo->Run);
   //}

   //int ss_code = gSpinPattern[bId] == 1 ? 0 : (gSpinPattern[bId] == -1 ? 1 : 2);
   //cntr_tgt.reg.NStrip[tstep][ss_code][chId - 1]++;
   
   UInt_t ttime = ch->GetRevolutionId()/RHIC_REVOLUTION_FREQ;
   //((TH2F*) sd->o["hSpinVsDelim"+cutid+"_ch"+sChId])->Fill(ch->GetDelimiterId(), gSpinPattern[bId]);
   ((TH2*) sd->o["hSpinVsDelim"+cutid+"_ch"+sChId])->Fill(ttime, gSpinPattern[bId]);

   //((TH2*)     o["hTimeVsFunnyEnergyA"+cutid])               ->Fill(ch->GetFunnyEnergyA(), ch->GetTime());

   //((TH2*) sd->o["hDLVsEnergyA"+cutid+"_ch"+sChId])          ->Fill(ch->GetEnergyA(), kinEnergy - ch->GetEnergyA());
   //((TH2*)     o["hDLVsEnergyA"+cutid])                      ->Fill(ch->GetEnergyA(), kinEnergyEst - ch->GetEnergyA());
   //((TH2*) sd->o["hDLVsTotalEnergy"+cutid+"_ch"+sChId])      ->Fill(kinEnergy, kinEnergy - ch->GetEnergyA());
   //((TH2*)     o["hDLVsTotalEnergy"+cutid])                  ->Fill(kinEnergyEst, kinEnergyEst - ch->GetEnergyA());
   //((TH2*) sd->o["hDLVsTime"+cutid+"_ch"+sChId])             ->Fill(ch->GetTime(), kinEnergy - ch->GetEnergyA());
   //((TH2*)     o["hDLVsTime"+cutid])                         ->Fill(ch->GetTime(), kinEnergyEst - ch->GetEnergyA());
   //((TH2*) sd->o["hTotalEnergyVsEnergyA"+cutid+"_ch"+sChId]) ->Fill(ch->GetEnergyA(), kinEnergy);
   //((TH2*)     o["hTotalEnergyVsEnergyA"+cutid])             ->Fill(ch->GetEnergyA(), kinEnergyEst);

   //((TH1*)     o["hTof"+cutid])                              ->Fill(tofEst);
} //}}}


/** */
void CnipolHists::PreFill(string cutid)
{ //{{{
   char dName[256];
   char hName[256];

   set<UShort_t>::const_iterator iCh;
   set<UShort_t>::const_iterator iChB = gRunInfo->fSiliconChannels.begin();
   set<UShort_t>::const_iterator iChE = gRunInfo->fSiliconChannels.end();

   for (iCh=iChB; iCh!=iChE; ++iCh) {

      sprintf(dName, "channel%02d", *iCh);
      DrawObjContainer *oc = d.find(dName)->second;

      sprintf(hName, "hSpinVsDelim%s_ch%02d", cutid.c_str(), *iCh);
      ((TH1*) oc->o[hName])->SetBins(gNDelimeters, 0, gNDelimeters, N_SPIN_STATES, -1.5, 1.5);
   }
} //}}}


/** */
void CnipolHists::PostFill()
{ //{{{
   //char hName[256];

   vector<string> cutIds;
   vector<string>::const_iterator icut;
   cutIds.push_back("");
   cutIds.push_back("_cut1");
   cutIds.push_back("_cut2");

   for (icut=cutIds.begin(); icut!=cutIds.end(); ++icut) {

      string sCutId = *icut;

      TH1* hTimeVsEnergyA   = (TH1*) o["hTimeVsEnergyA"+sCutId];
      TH1* hTofVsKinEnergyA = (TH1*) o["hTofVsKinEnergyA"+sCutId];
   
      set<UShort_t>::const_iterator iCh;
      set<UShort_t>::const_iterator iChB = gRunInfo->fSiliconChannels.begin();
      set<UShort_t>::const_iterator iChE = gRunInfo->fSiliconChannels.end();

      for (iCh=iChB; iCh!=iChE; ++iCh) {

         string sChId("  ");
         sprintf(&sChId[0], "%02d", *iCh);

         DrawObjContainer *oc = d.find("channel"+sChId)->second;

         TH1* hTimeVsEnergyA_channel = (TH1*) oc->o["hTimeVsEnergyA"+sCutId+"_ch"+sChId];
         hTimeVsEnergyA->Add(hTimeVsEnergyA_channel);

         TH1* hTofVsKinEnergyA_channel = (TH1*) oc->o["hTofVsKinEnergyA"+sCutId+"_ch"+sChId];
         hTofVsKinEnergyA->Add(hTofVsKinEnergyA_channel);

         // Fill final kinematic histograms using calib constants
         //TH2* hTofVsKinEnergyA_channel = (TH2*) oc->o["hTofVsKinEnergyA"+sCutId+"_ch"+sChId];
         //ConvertRawToKin(hTVsA_channel, hTofVsKinEnergyA_channel, *iCh);
      }
   }
} //}}}


/** */
void CnipolHists::SaveAllAs(TCanvas &c, std::string pattern, string path, Bool_t thumbs)
{ //{{{
   //Info("SaveAllAs", "executing...");
   DrawObjContainer::SaveAllAs(c, pattern, path, thumbs);

   string strThumb = thumbs ? "_thumb" : "" ;

   string cutid = "_cut2";

   // Draw superimposed for all channels
   set<UShort_t>::const_iterator iCh;
   set<UShort_t>::const_iterator iChB = gRunInfo->fSiliconChannels.begin();
   set<UShort_t>::const_iterator iChE = gRunInfo->fSiliconChannels.end();

   for (iCh=iChB; iCh!=iChE; ++iCh)
	{
      string sSi("  ");
      sprintf(&sSi[0], "%02d", *iCh);
      string dName = "channel" + sSi;
      string cName = "c_combo_ch" + sSi + strThumb;

      DrawObjContainer* oc = d.find(dName)->second;

      THStack hstack(cName.c_str(), cName.c_str());

      TH1* h1 = (TH1*) oc->o["hTimeVsEnergyA"+cutid+"_ch"+sSi];
		hstack.Add(h1);

      TH1* h2 = (TH1*) oc->o["hFitMeanTimeVsEnergyA"+cutid+"_ch"+sSi];
		hstack.Add(h2);

      string subPath = path + "/" + dName;

		SaveHStackAs(c, hstack, subPath);
		//DrawObjContainer::SaveHStackAs(c, hstack, subPath);
   }
} //}}}


/** */
void CnipolHists::ConvertRawToKin(TH2* hRaw, TH2* hKin, UShort_t chId)
{ //{{{
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
} //}}}
