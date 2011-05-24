/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolHists.h"

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
void CnipolHists::BookHists(string sid)
{ //{{{
   char hName[256];

   fDir->cd();

   //char  formula[100], fname[100];
   //sprintf(formula, "(%f/sqrt(x+130))-23", RunConst::E2T);

   sprintf(hName, "hTvsA%s", sid.c_str());
   o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
   ((TH1*) o[hName])->SetOption("colz LOGZ");
   ((TH1*) o[hName])->SetTitle(";Amplitude, ADC;TDC;");

   sprintf(hName, "hTvsI%s", sid.c_str());
   o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
   ((TH1*) o[hName])->SetOption("colz LOGZ");
   ((TH1*) o[hName])->SetTitle(";Integral, ADC;TDC;");

   // Time vs Energy from amplitude
   sprintf(hName, "hTimeVsEnergyA%s", sid.c_str());
   o[hName] = new TH2F(hName, hName, 100, 0, 2500, 60, 0, 120);
   ((TH1*) o[hName])->SetOption("colz LOGZ");
   ((TH1*) o[hName])->SetTitle(";Deposited Energy, keV;Time, ns;");

   //sprintf(fname, "banana_nominal%s", sid.c_str());
   //TF1 *banana_cut_l = new TF1(fname, formula, 0, 1500);
   //((TH1D*) o[hName])->GetListOfFunctions()->Add(banana_cut_l);

   // TOF vs Kinematic Energy
   sprintf(hName, "hTofVsKinEnergyA%s", sid.c_str());
   //o[hName] = new TH2F(hName, hName, 255, 0, 1785, 100, 20, 120);
   o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 100);
   ((TH1*) o[hName])->SetOption("colz LOGZ NOIMG");
   ((TH1*) o[hName])->SetTitle(";Kinematic Energy, keV;ToF, ns;");

   // Kinematic Energy
   sprintf(hName, "hKinEnergyA_o%s", sid.c_str());
   o[hName] = new TH1F(hName, hName, 25, 22.5, 1172.2);
   ((TH1F*) o[hName])->SetTitle(";Kinematic Energy, keV;;");

   // Spin vs Strip Id
   sprintf(hName, "hSpinVsChannel%s", sid.c_str());
   o[hName] = new TH2I(hName, hName, N_CHANNELS, 1, N_CHANNELS+1, NUM_SPIN_STATES, -1.5, 1.5);
   ((TH1*) o[hName])->SetOption("colz");
   ((TH1*) o[hName])->SetTitle(";Channel Id;Spin State;");

   // Spin vs Bunch Id
   sprintf(hName, "hSpinVsBunch%s", sid.c_str());
   o[hName] = new TH2I(hName, hName, NBUNCH, 0, NBUNCH, NUM_SPIN_STATES, -1.5, 1.5);
   ((TH1*) o[hName])->SetOption("colz NOIMG");
   ((TH1*) o[hName])->SetTitle(";Bunch Id;Spin State;");

   // Time vs Energy from amplitude
   //sprintf(hName, "hTimeVsFunnyEnergyA%s", sid.c_str());
   //o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 100);
   //((TH2F*) o[hName])->SetOption("colz LOGZ NOIMG");
   //((TH2F*) o[hName])->SetTitle(";Funny Energy, keV;Time, ns;");

   //sprintf(fname, "banana_nominal_funny%s", sid.c_str());
   //banana_cut_l = new TF1(fname, formula, 0, 1500);
   //((TH1D*) o[hName])->GetListOfFunctions()->Add(banana_cut_l);

   //sprintf(hName, "hFitMeanTimeVsEnergyA%s", sid.c_str());
   //o[hName] = new TH1D(hName, hName, 100, 0, 2500);
   //((TH1*) o[hName])->SetOption("NOIMG");
   //((TH1*) o[hName])->SetTitle(";Deposited Energy, keV;Mean Time, ns;");
   //((TH1*) o[hName])->GetYaxis()->SetRangeUser(0, 120);

   //sprintf(hName, "hDLVsEnergyA%s", sid.c_str());
   //o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 400);
   //((TH1*) o[hName])->SetOption("colz NOIMG");

   //sprintf(hName, "hDLVsTotalEnergy%s", sid.c_str());
   //o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 400);
   //((TH1*) o[hName])->SetOption("colz NOIMG");

   //sprintf(hName, "hDLVsTime%s", sid.c_str());
   //o[hName] = new TH2F(hName, hName, 100, 0, 100, 100, 0, 400);
   //((TH1*) o[hName])->SetOption("colz NOIMG");

   //sprintf(hName, "hTotalEnergyVsEnergyA%s", sid.c_str());
   //o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 2000);
   //((TH1*) o[hName])->SetOption("colz NOIMG");

   //sprintf(hName, "hTof%s", sid.c_str());
   //o[hName] = new TH1D(hName, hName, 100, 0, 100);
   //((TH1*) o[hName])->SetOption("NOIMG");

   sprintf(hName, "hPseudoMass%s", sid.c_str());
   o[hName] = new TH1D(hName, hName, 50, 0, 20);
   ((TH1*) o[hName])->SetOption("hist NOIMG");

   char dName[256];
   //string sSi("  ");
   DrawObjContainer        *oc;
   DrawObjContainerMapIter  isubdir;

   //for (int i=0; i<TOT_WFD_CH; i++)
   for (int i=1; i<=N_SILICON_CHANNELS; i++) {

      sprintf(dName, "channel%02d", i);

      isubdir = d.find(dName);

      if ( isubdir == d.end()) { // if dir not found
         oc = new DrawObjContainer();
         oc->fDir = new TDirectoryFile(dName, dName, "", fDir);
      } else {
         oc = isubdir->second;
      }

      sprintf(hName, "hTvsA%s_st%02d", sid.c_str(), i);
      oc->o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
      ((TH1*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH1*) oc->o[hName])->SetTitle(";Amplitude, ADC;TDC;");

      sprintf(hName, "hTvsI%s_st%02d", sid.c_str(), i);
      //sprintf(hName, "hTvsI", i+1);
      oc->o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
      ((TH1*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH1*) oc->o[hName])->SetTitle(";Integral, ADC;TDC;");

      //sprintf(fname, "banana_nominal%s_st%02d", sid.c_str(), i);
      //banana_cut_l = new TF1(fname, formula, 0, 1500);

      // Time vs Energy from amplitude
      sprintf(hName, "hTimeVsEnergyA%s_st%02d", sid.c_str(), i);
      oc->o[hName] = new TH2F(hName, hName, 100, 0, 2500, 60, 0, 120);
      ((TH1*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH1*) oc->o[hName])->SetTitle(";Deposited Energy, keV;Time, ns;");
      //((TH1*) oc->o[hName])->GetListOfFunctions()->Add(banana_cut_l);

      // TOF vs Kinematic Energy
      sprintf(hName, "hTofVsKinEnergyA%s_ch%02d", sid.c_str(), i);
      oc->o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 100);
      ((TH1*) oc->o[hName])->SetOption("colz LOGZ NOIMG");
      ((TH1*) oc->o[hName])->SetTitle(";Kinematic Energy, keV;ToF, ns;");

      sprintf(hName, "hFitMeanTimeVsEnergyA%s_st%02d", sid.c_str(), i);
      oc->o[hName] = new TH1D(hName, hName, 100, 0, 2500);
      //((TH2F*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH1*) oc->o[hName])->SetTitle(";Deposited Energy, keV;Mean Time, ns;");
      ((TH1*) oc->o[hName])->GetYaxis()->SetRangeUser(0, 120);

      //sprintf(hName, "hDLVsEnergyA%s_st%02d", sid.c_str(), i);
      //oc->o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 400);
      //((TH1*) oc->o[hName])->SetOption("colz NOIMG");
      ////((TH2F*) oc->o[hName])->GetYaxis()->SetRangeUser(0, 20000);

      //sprintf(hName, "hDLVsTotalEnergy%s_st%02d", sid.c_str(), i);
      //oc->o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 400);
      //((TH1*) oc->o[hName])->SetOption("colz NOIMG");

      //sprintf(hName, "hDLVsTime%s_st%02d", sid.c_str(), i);
      //oc->o[hName] = new TH2F(hName, hName, 100, 0, 100, 100, 0, 400);
      //((TH1*) oc->o[hName])->SetOption("colz NOIMG");

      //sprintf(hName, "hTotalEnergyVsEnergyA%s_st%02d", sid.c_str(), i);
      //oc->o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 2000);
      //((TH1*) oc->o[hName])->SetOption("colz NOIMG");

      sprintf(hName, "hPseudoMass%s_ch%02d", sid.c_str(), i);
      oc->o[hName] = new TH1D(hName, hName, 50, 0, 20);
      ((TH1*) oc->o[hName])->SetOption("hist NOIMG");

      // Delim (time) vs Spin vs Channel Id
      sprintf(hName, "hSpinVsDelim%s_st%02d", sid.c_str(), i);
      oc->o[hName] = new TH2I(hName, hName, 1, 0, 1, NUM_SPIN_STATES, -1.5, 1.5);
      ((TH1*) oc->o[hName])->SetOption("NOIMG");

      // If this is a new directory then we need to add it to the list
      if ( isubdir == d.end()) {
         d[dName] = oc;
      }
   }

   //delete banana_cut_l;
} //}}}


/** */
void CnipolHists::BookHistsExtra(string sid)
{ //{{{
   //char hName[256];

   //TH2F* hData = (TH2F*) o["hTimeVsEnergyA"+sid];

   //char  formula[100], fname[100];
   //float sigma = RunConst::M2T * ]*gAnaInfo->MassSigmaAlt :

   //sprintf(formula, "%f/sqrt(x)+(%f)/sqrt(x)", RunConst::E2T, sigma);
   //sprintf(formula, "%f/sqrt(x)", RunConst::E2T);
   //sprintf(fname, "banana_cut_l_st%d_mode%d", i, j);
   //sprintf(fname, "banana_cut_l");

   //TF1 *banana_cut_l = new TF1(fname, formula, 0, 1500);

   //if (hData) {
      //hData->GetListOfFunctions()->Add(banana_cut_l);
      //hData->GetListOfFunctions()->Add(banana_cut_h);
   //}
} //}}}


///** */
//Int_t CnipolHists::Write(const char* name, Int_t option, Int_t bufsize)
//{ //{{{
//   return DrawObjContainer::Write(name, option, bufsize);
//
//   // Temporary
//   if (!fDir) {
//      Error("Write", "Directory fDir not defined");
//      return 0;
//   }
//
//   fDir->cd();
//
//   ObjMapIter io;
//
//   for (io=o.begin(); io!=o.end(); ++io) {
//      //sprintf(cName, "c_%s", io->first.c_str());
//      if (io->second) io->second->Write();
//   }
//
//   DrawObjContainerMapIter isubd;
//
//   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {
//      string sname(isubd->first);
//      if (sname.find("channel") != string::npos ||
//          sname.find("preproc") != string::npos )// ||
//          //sname.find("scalers") != string::npos )// ||
//          //sname.find("Kinema2") != string::npos )
//      {//continue;
//         isubd->second->Write();
//      }
//   }
//
//   return 1;
//} //}}}


///** */
//Int_t CnipolHists::Write(const char* name, Int_t option, Int_t bufsize) const
//{
//   return ((const CnipolHists*) this)->Write(name, option, bufsize);
//}


/** */
void CnipolHists::Fill(ChannelEvent *ch, string sid)
{ //{{{
   UChar_t chId  = ch->GetChannelId();
   //UChar_t detId = ch->GetDetectorId();

   // by detector id
   //DrawObjContainer &sd1 = d["Kinema"];

   //string sDetId(" ");
   //sprintf(&sDetId[0], "%1d", detId);
   //char hName[256];
   
   //if (sid == "_cut2") { // fill these if only pass the carbon mass cut
   //   //((TH1F*) d["Kinema2"]->o["energy_spectrum_all"])->Fill( fabs(ch->GetMandelstamT()) );
   //   //sprintf(hName,"energy_spectrum_det%d", detId);
   //   //((TH1F*) d["Kinema2"]->o[hName])->Fill( fabs(ch->GetMandelstamT()) );
   //}

   // by channel id
   string sChId("  ");
   sprintf(&sChId[0], "%02d", chId);

   DrawObjContainer *sd = d["channel"+sChId];

   ((TH1*) sd->o["hTvsA"+sid+"_st"+sChId]) -> Fill(ch->GetAmpltd(), ch->GetTdc());
   ((TH1*) sd->o["hTvsI"+sid+"_st"+sChId]) -> Fill(ch->GetIntgrl(), ch->GetTdc());
   ((TH1*) sd->o["hTimeVsEnergyA"+sid+"_st"+sChId]) -> Fill(ch->GetEnergyA(), ch->GetTime());

   // Full kinematic carbon kinEnergy
   //Float_t kinEnergyEst = ch->GetKinEnergyAEstimate();
   //Float_t kinEnergyEst = ch->GetKinEnergyAEstimateEDepend();
   //Float_t kinEnergy = ch->GetKinEnergyA();
   Float_t kinEnergy    = ch->GetKinEnergyAEDepend();

   //Float_t tofEst    = ch->GetTimeOfFlightEstimate();
   Float_t tof       = ch->GetTimeOfFlight();

   ((TH2*) sd->o["hTofVsKinEnergyA"+sid+"_ch"+sChId]) -> Fill(kinEnergy, tof);
   //((TH1*)     o["hKinEnergyA_o"+sid])              -> Fill(kinEnergy);

   ((TH1*) o["hKinEnergyA_o"+sid]) -> Fill(kinEnergy);
   
   UChar_t bId = ch->GetBunchId();

   ((TH1*) o["hSpinVsChannel"+sid]) -> Fill(chId, gSpinPattern[bId]);
   //((TH1*) o["hSpinVsBunch"+sid]) -> Fill(bId, gSpinPattern[bId]);

   //ds XXX
   UShort_t tstep = 0;

   if (gRunInfo->Run == 5) {
      tstep = ch->GetDelimiterId();
      //NDcounts[(int)(st/12)][event->bid][TgtIndex[delim]]++;
   } else if (gRunInfo->Run >= 6) {
      UInt_t ttime = ch->GetRevolutionId()/RHIC_REVOLUTION_FREQ;

      if (ttime < MAXDELIM) {
         tstep = TgtIndex[ttime];
         //++cntr.good[TgtIndex[ttime]];
         //NDcounts[(int)(st/12)][event->bid][TgtIndex[ttime]]++;
      } else if (!gAnaInfo->HasAlphaBit()) {
         Error("Fill", "Time constructed from revolution #%d exeeds MAXDELIM=%d defined\n" \
               "Perhaps calibration data? Try running with --calib option", ttime, MAXDELIM);
      }
   } else {
      Warning("Fill", "Target tstep size is not defined for Run %d", gRunInfo->Run);
   }

   int ss_code = gSpinPattern[bId] == 1 ? 0 : (gSpinPattern[bId] == -1 ? 1 : 2);
   cntr_tgt.reg.NStrip[tstep][ss_code][chId - 1]++;
   
   UInt_t ttime = ch->GetRevolutionId()/RHIC_REVOLUTION_FREQ;
   //((TH2F*) sd->o["hSpinVsDelim"+sid+"_st"+sChId])->Fill(ch->GetDelimiterId(), gSpinPattern[bId]);
   ((TH2*) sd->o["hSpinVsDelim"+sid+"_st"+sChId])->Fill(ttime, gSpinPattern[bId]);

   //((TH2*)     o["hTimeVsFunnyEnergyA"+sid])               ->Fill(ch->GetFunnyEnergyA(), ch->GetTime());

   //((TH2*) sd->o["hDLVsEnergyA"+sid+"_st"+sChId])          ->Fill(ch->GetEnergyA(), kinEnergy - ch->GetEnergyA());
   //((TH2*)     o["hDLVsEnergyA"+sid])                      ->Fill(ch->GetEnergyA(), kinEnergyEst - ch->GetEnergyA());
   //((TH2*) sd->o["hDLVsTotalEnergy"+sid+"_st"+sChId])      ->Fill(kinEnergy, kinEnergy - ch->GetEnergyA());
   //((TH2*)     o["hDLVsTotalEnergy"+sid])                  ->Fill(kinEnergyEst, kinEnergyEst - ch->GetEnergyA());
   //((TH2*) sd->o["hDLVsTime"+sid+"_st"+sChId])             ->Fill(ch->GetTime(), kinEnergy - ch->GetEnergyA());
   //((TH2*)     o["hDLVsTime"+sid])                         ->Fill(ch->GetTime(), kinEnergyEst - ch->GetEnergyA());
   //((TH2*) sd->o["hTotalEnergyVsEnergyA"+sid+"_st"+sChId]) ->Fill(ch->GetEnergyA(), kinEnergy);
   //((TH2*)     o["hTotalEnergyVsEnergyA"+sid])             ->Fill(ch->GetEnergyA(), kinEnergyEst);

   //((TH1*)     o["hTof"+sid])                              ->Fill(tofEst);

   //Float_t mass = ch->GetCarbonMassEstimate();
   Float_t mass = ch->GetCarbonMass();
   //cout << "mass: " << mass << endl;

   ((TH1*) sd->o["hPseudoMass"+sid+"_ch"+sChId]) -> Fill(mass);
} //}}}


/** */
void CnipolHists::PreFill(string sid)
{
   char dName[256];
   char hName[256];

   for (int i=1; i<=N_SILICON_CHANNELS; i++) {

      sprintf(dName, "channel%02d", i);
      DrawObjContainer *oc = d.find(dName)->second;

      sprintf(hName, "hSpinVsDelim%s_st%02d", sid.c_str(), i);
      ((TH1*) oc->o[hName])->SetBins(gNDelimeters, 0, gNDelimeters, NUM_SPIN_STATES, -1.5, 1.5);
   }
}


/** */
void CnipolHists::PostFill()
{
   //char hName[256];

   vector<string> cutIds;
   vector<string>::const_iterator icut;
   cutIds.push_back("");
   cutIds.push_back("_cut1");
   cutIds.push_back("_cut2");

   for (icut=cutIds.begin(); icut!=cutIds.end(); ++icut) {

      string sCutId = *icut;

      TH1* hTvsA            = (TH1*) o["hTvsA"+sCutId];
      TH1* hTvsI            = (TH1*) o["hTvsI"+sCutId];
      TH1* hTimeVsEnergyA   = (TH1*) o["hTimeVsEnergyA"+sCutId];
      TH1* hTofVsKinEnergyA = (TH1*) o["hTofVsKinEnergyA"+sCutId];
      TH1* hPseudoMass      = (TH1*) o["hPseudoMass"+sCutId];
   
      for (UShort_t iCh=1; iCh<=N_SILICON_CHANNELS; iCh++) {


         string sChId("  ");
         sprintf(&sChId[0], "%02d", iCh);

         DrawObjContainer *oc = d.find("channel"+sChId)->second;

         TH2* hTVsA_channel = (TH2*) oc->o["hTvsA"+sCutId+"_st"+sChId];
         hTvsA->Add(hTVsA_channel);

         TH2* hTVsI_channel = (TH2*) oc->o["hTvsI"+sCutId+"_st"+sChId];
         hTvsI->Add(hTVsI_channel);

         TH1* hTimeVsEnergyA_channel = (TH1*) oc->o["hTimeVsEnergyA"+sCutId+"_st"+sChId];
         hTimeVsEnergyA->Add(hTimeVsEnergyA_channel);

         TH1* hPseudoMass_channel = (TH1*) oc->o["hPseudoMass"+sCutId+"_ch"+sChId];
         hPseudoMass->Add(hPseudoMass_channel);

         TH1* hTofVsKinEnergyA_channel = (TH1*) oc->o["hTofVsKinEnergyA"+sCutId+"_ch"+sChId];
         hTofVsKinEnergyA->Add(hTofVsKinEnergyA_channel);

         // Fill final kinematic histograms using calib constants
         //TH2* hTofVsKinEnergyA_channel = (TH2*) oc->o["hTofVsKinEnergyA"+sCutId+"_ch"+sChId];
         //ConvertRawToKin(hTVsA_channel, hTofVsKinEnergyA_channel, iCh);

      }
   }
}


/** */
void CnipolHists::SaveAllAs(TCanvas &c, std::string pattern, string path)
{ //{{{
   //Warning("SaveAllAs", "executing...");
   DrawObjContainer::SaveAllAs(c, pattern, path);

   string sid = "_cut2";

   // Draw superimposed for all channels
   for (UShort_t i=1; i<=N_CHANNELS; i++) {

      //if (i+1 != 28) continue;

      string sSi("  ");
      sprintf(&sSi[0], "%02d", i);
      string dName = "channel" + sSi;
      string cName = "c_combo_st" + sSi;

      DrawObjContainer* oc = d.find(dName)->second;

      TH1* h1 = (TH1*) oc->o["hTimeVsEnergyA"+sid+"_st"+sSi];
      TH1* h2 = (TH1*) oc->o["hFitMeanTimeVsEnergyA"+sid+"_st"+sSi];

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

      string fName = path + "/" + dName + "/" + cName + ".png";
      //printf("Saving %s\n", fName.c_str());

      c.SetName(cName.c_str());
      c.SetTitle(cName.c_str());
      //c.SaveAs(fName.c_str());

      TText signature;
      signature.SetTextSize(0.03);
      signature.DrawTextNDC(0, 0, fSignature.c_str());

      if (TPRegexp(pattern).MatchB(fName.c_str())) {
         c.SaveAs(fName.c_str());
         gSystem->Chmod(fName.c_str(), 0775);
      } else {
         //Warning("SaveAllAs", "Histogram %s name does not match pattern. Skipped", fName.c_str());
      }
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
