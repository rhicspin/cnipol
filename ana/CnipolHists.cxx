/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolHists.h"

ClassImp(CnipolHists)

using namespace std;

/** Default constructor. */
CnipolHists::CnipolHists() : DrawObjContainer()
{
   BookHists();
   BookHists("_cut1");
   BookHists("_cut2");
   BookPreProcess();
   //BookHistsExtra();
   //BookHistsExtra("_cut1");
}


CnipolHists::CnipolHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
   BookHists("_cut1");
   BookHists("_cut2");
   BookPreProcess();
   //BookHistsExtra();
   //BookHistsExtra("_cut1");
}


/** Default destructor. */
CnipolHists::~CnipolHists()
{
}


/** */
void CnipolHists::BookHists(string sid)
{ //{{{
   char hName[256];

   //fDir->Print();
   fDir->cd();

   //char  formula[100], fname[100];
   //sprintf(formula, "(%f/sqrt(x+130))-23", RunConst::E2T);

   sprintf(hName, "hTvsA%s", sid.c_str());
   o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 0, 80);
   ((TH2F*) o[hName])->SetOption("colz LOGZ");
   ((TH2F*) o[hName])->GetXaxis()->SetTitle("Amplitude, ADC");
   ((TH2F*) o[hName])->GetYaxis()->SetTitle("TDC");

   sprintf(hName, "hTvsI%s", sid.c_str());
   o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 0, 80);
   ((TH2F*) o[hName])->SetOption("colz LOGZ");
   ((TH2F*) o[hName])->GetXaxis()->SetTitle("Integral, ADC");
   ((TH2F*) o[hName])->GetYaxis()->SetTitle("TDC");

   // Time vs Energy from amplitude
   sprintf(hName, "hTimeVsEnergyA%s", sid.c_str());
   o[hName] = new TH2F(hName, hName, 50, 0, 2000, 60, 0, 120);
   ((TH2F*) o[hName])->SetOption("colz LOGZ");
   ((TH2F*) o[hName])->GetXaxis()->SetTitle("Deposited Energy, keV");
   ((TH2F*) o[hName])->GetYaxis()->SetTitle("Time, ns");

   //sprintf(fname, "banana_nominal%s", sid.c_str());
   //TF1 *banana_cut_l = new TF1(fname, formula, 0, 1500);
   //((TH1D*) o[hName])->GetListOfFunctions()->Add(banana_cut_l);

   // TOF vs Kinematic Energy
   sprintf(hName, "hTofVsKinEnergyA%s", sid.c_str());
   //o[hName] = new TH2F(hName, hName, 255, 0, 1785, 100, 20, 120);
   o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 100);
   ((TH2F*) o[hName])->SetOption("colz LOGZ");
   ((TH2F*) o[hName])->GetXaxis()->SetTitle("Kinematic Energy, keV");
   ((TH2F*) o[hName])->GetYaxis()->SetTitle("ToF, ns");

   // Kinematic Energy
   sprintf(hName, "hKinEnergyA_o%s", sid.c_str());
   o[hName] = new TH1F(hName, hName, 25, 22.5, 1172.2);
   ((TH1F*) o[hName])->GetXaxis()->SetTitle("Kinematic Energy, keV");

   // Spin vs Strip Id
   sprintf(hName, "hSpinVsChannel%s", sid.c_str());
   o[hName] = new TH2I(hName, hName, NSTRIP, 1, NSTRIP+1, NUM_SPIN_STATES, -1.5, 1.5);
   ((TH1*) o[hName])->SetOption("colz");
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Channel Id");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Spin State");

   // Spin vs Bunch Id
   sprintf(hName, "hSpinVsBunch%s", sid.c_str());
   o[hName] = new TH2I(hName, hName, NBUNCH, 0, NBUNCH, NUM_SPIN_STATES, -1.5, 1.5);
   ((TH1*) o[hName])->SetOption("colz");
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Bunch Id");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Spin State");

   // Time vs Energy from amplitude
   sprintf(hName, "hTimeVsFunnyEnergyA%s", sid.c_str());
   o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 100);
   ((TH2F*) o[hName])->SetOption("colz LOGZ");
   ((TH2F*) o[hName])->GetXaxis()->SetTitle("Funny Energy, keV");
   ((TH2F*) o[hName])->GetYaxis()->SetTitle("Time, ns");

   //sprintf(fname, "banana_nominal_funny%s", sid.c_str());
   //banana_cut_l = new TF1(fname, formula, 0, 1500);
   //((TH1D*) o[hName])->GetListOfFunctions()->Add(banana_cut_l);

   sprintf(hName, "hFitMeanTimeVsEnergyA%s", sid.c_str());
   o[hName] = new TH1D(hName, hName, 100, 0, 2000);
   //((TH2F*) o[hName])->SetOption("colz LOGZ");
   ((TH1D*) o[hName])->GetYaxis()->SetRangeUser(0, 100);
   ((TH1D*) o[hName])->GetXaxis()->SetTitle("Deposited Energy, keV");
   ((TH1D*) o[hName])->GetYaxis()->SetTitle("Mean Time, ns");

   sprintf(hName, "hDLVsEnergyA%s", sid.c_str());
   o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 400);
   ((TH2F*) o[hName])->SetOption("colz");
   //((TH2F*) o[hName])->GetYaxis()->SetRangeUser(0, 20000);

   sprintf(hName, "hDLVsTotalEnergy%s", sid.c_str());
   o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 400);
   ((TH2F*) o[hName])->SetOption("colz");

   sprintf(hName, "hDLVsTime%s", sid.c_str());
   o[hName] = new TH2F(hName, hName, 100, 0, 100, 100, 0, 400);
   ((TH2F*) o[hName])->SetOption("colz");

   sprintf(hName, "hTotalEnergyVsEnergyA%s", sid.c_str());
   o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 2000);
   ((TH2F*) o[hName])->SetOption("colz");

   sprintf(hName, "hTof%s", sid.c_str());
   o[hName] = new TH1D(hName, hName, 100, 0, 100);

   sprintf(hName, "hPseudoMass%s", sid.c_str());
   o[hName] = new TH1D(hName, hName, 100, 0, 20);

   char dName[256];
   //string sSi("  ");
   DrawObjContainer        *oc;
   DrawObjContainerMapIter  isubdir;

   //for (int i=0; i<TOT_WFD_CH; i++)
   for (int i=1; i<=NSTRIP; i++) {

      sprintf(dName, "channel%02d", i);

      isubdir = d.find(dName);

      if ( isubdir == d.end()) { // if dir not found
         oc = new DrawObjContainer();
         oc->fDir = new TDirectoryFile(dName, dName, "", fDir);
      } else {
         oc = isubdir->second;
      }

      sprintf(hName, "hTvsA%s_st%02d", sid.c_str(), i);
      oc->o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 0, 80);
      ((TH2F*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH2F*) oc->o[hName])->GetXaxis()->SetTitle("Amplitude, ADC");
      ((TH2F*) oc->o[hName])->GetYaxis()->SetTitle("TDC");

      sprintf(hName, "hTvsI%s_st%02d", sid.c_str(), i);
      //sprintf(hName, "hTvsI", i+1);
      oc->o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 0, 80);
      ((TH2F*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH2F*) oc->o[hName])->GetXaxis()->SetTitle("Integral, ADC");
      ((TH2F*) oc->o[hName])->GetYaxis()->SetTitle("TDC");

      //sprintf(fname, "banana_nominal%s_st%02d", sid.c_str(), i);
      //banana_cut_l = new TF1(fname, formula, 0, 1500);

      // Time vs Energy from amplitude
      sprintf(hName, "hTimeVsEnergyA%s_st%02d", sid.c_str(), i);
      oc->o[hName] = new TH2F(hName, hName, 100, 0, 2000, 60, 0, 120);
      ((TH2F*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH2F*) oc->o[hName])->GetXaxis()->SetTitle("Deposited Energy, keV");
      ((TH2F*) oc->o[hName])->GetYaxis()->SetTitle("Time, ns");
      //((TH2F*) oc->o[hName])->GetListOfFunctions()->Add(banana_cut_l);

      // TOF vs Kinematic Energy
      sprintf(hName, "hTofVsKinEnergyA%s_st%02d", sid.c_str(), i);
      oc->o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 100);
      ((TH2F*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH2F*) oc->o[hName])->GetXaxis()->SetTitle("Kinematic Energy, keV");
      ((TH2F*) oc->o[hName])->GetYaxis()->SetTitle("ToF, ns");

      sprintf(hName, "hFitMeanTimeVsEnergyA%s_st%02d", sid.c_str(), i);
      oc->o[hName] = new TH1D(hName, hName, 100, 0, 2000);
      //((TH2F*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH1D*) oc->o[hName])->GetYaxis()->SetRangeUser(0, 100);
      ((TH1D*) oc->o[hName])->GetXaxis()->SetTitle("Deposited Energy, keV");
      ((TH1D*) oc->o[hName])->GetYaxis()->SetTitle("Mean Time, ns");

      sprintf(hName, "hDLVsEnergyA%s_st%02d", sid.c_str(), i);
      oc->o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 400);
      ((TH2F*) oc->o[hName])->SetOption("colz");
      //((TH2F*) oc->o[hName])->GetYaxis()->SetRangeUser(0, 20000);

      sprintf(hName, "hDLVsTotalEnergy%s_st%02d", sid.c_str(), i);
      oc->o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 400);
      ((TH2F*) oc->o[hName])->SetOption("colz");

      sprintf(hName, "hDLVsTime%s_st%02d", sid.c_str(), i);
      oc->o[hName] = new TH2F(hName, hName, 100, 0, 100, 100, 0, 400);
      ((TH2F*) oc->o[hName])->SetOption("colz");

      sprintf(hName, "hTotalEnergyVsEnergyA%s_st%02d", sid.c_str(), i);
      oc->o[hName] = new TH2F(hName, hName, 100, 0, 2000, 100, 0, 2000);
      ((TH2F*) oc->o[hName])->SetOption("colz");

      sprintf(hName, "hPseudoMass%s_st%02d", sid.c_str(), i);
      oc->o[hName] = new TH1D(hName, hName, 100, 0, 20);

      // Delim (time) vs Spin vs Channel Id
      sprintf(hName, "hSpinVsDelim%s_st%02d", sid.c_str(), i);
      oc->o[hName] = new TH2I(hName, hName, 1, 0, 1, NUM_SPIN_STATES, -1.5, 1.5);

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
   //float sigma = RunConst::M2T * ]*dproc.MassSigmaAlt :

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


/** */
void CnipolHists::BookPreProcess()
{ //{{{
   char hName[256];
   string dName = "preproc";

   DrawObjContainer *oc;
   DrawObjContainerMapIter isubdir;

   isubdir = d.find(dName);

   if ( isubdir == d.end()) { // if dir not found
      oc = new DrawObjContainer();
      oc->fDir = new TDirectoryFile(dName.c_str(), dName.c_str(), "", fDir);
      oc->fDir->cd();
   } else {
      oc = isubdir->second;
   }

   sprintf(hName, "hTimeVsEnergyA");
   //oc->o[hName] = new TH2F(hName, hName, 255, 0, 1530, 100, 0, 100);
   //oc->o[hName] = new TH2F(hName, hName, 255, 0, 2550, 100, 0, 100);
   oc->o[hName] = new TH2F(hName, hName, 100, 0, 2500, 60, 0, 120);
   ((TH2F*) oc->o[hName])->SetOption("colz LOGZ");
   ((TH2F*) oc->o[hName])->GetXaxis()->SetTitle("Deposited Energy, keV");
   ((TH2F*) oc->o[hName])->GetYaxis()->SetTitle("Time, ns");

   sprintf(hName, "hFitMeanTimeVsEnergyA");
   //oc->o[hName] = new TH1D(hName, hName, 255, 0, 1530);
   //oc->o[hName] = new TH1D(hName, hName, 255, 0, 2550);
   oc->o[hName] = new TH1D(hName, hName, 100, 0, 2500);
   ((TH1D*) oc->o[hName])->GetYaxis()->SetRangeUser(0, 100);
   ((TH1D*) oc->o[hName])->GetXaxis()->SetTitle("Deposited Energy, keV");
   ((TH1D*) oc->o[hName])->GetYaxis()->SetTitle("Mean Time, ns");

   // If this is a new directory then we need to add it to the list
   if ( isubdir == d.end()) {
      d[dName] = oc;
   }
} //}}}


/** */
void CnipolHists::Print(const Option_t* opt) const
{ //{{{
   opt = "";

   //printf("CnipolHists:\n");
   DrawObjContainer::Print();
} //}}}


/** */
Int_t CnipolHists::Write(const char* name, Int_t option, Int_t bufsize)
{ //{{{
   return DrawObjContainer::Write(name, option, bufsize);

   // Temporary
   if (!fDir) {
      Error("Write", "Directory fDir not defined");
      return 0;
   }

   fDir->cd();

   ObjMapIter io;

   for (io=o.begin(); io!=o.end(); ++io) {
      //sprintf(cName, "c_%s", io->first.c_str());
      if (io->second) io->second->Write();
   }

   DrawObjContainerMapIter isubd;

   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {
      string sname(isubd->first);
      if (sname.find("channel") != string::npos ||
          sname.find("preproc") != string::npos )// ||
          //sname.find("scalers") != string::npos )// ||
          //sname.find("Kinema2") != string::npos )
      {//continue;
         isubd->second->Write();
      }
   }

   return 1;
} //}}}


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
   
   if (sid == "_cut2") { // fill these if only pass the carbon mass cut
      //((TH1F*) d["Kinema2"]->o["energy_spectrum_all"])->Fill( fabs(ch->GetMandelstamT()) );
      //sprintf(hName,"energy_spectrum_det%d", detId);
      //((TH1F*) d["Kinema2"]->o[hName])->Fill( fabs(ch->GetMandelstamT()) );
   }

   // by channel id
   string sSi("  ");
   sprintf(&sSi[0], "%02d", chId);

   DrawObjContainer *sd = d["channel"+sSi];
   //sd = d["channel"+sSi];

   //((TH2F*) sd.o["hTvsA"+sid+"_st"+sSi])                 ->Fill(ch->fChannel.fAmpltd, ch->fChannel.fTdc);
   ////((TH2F*)    o["hTvsA"+sid])                           ->Fill(ch->fChannel.fAmpltd, ch->fChannel.fTdc);
   //((TH2F*) sd.o["hTvsI"+sid+"_st"+sSi])                 ->Fill(ch->fChannel.fIntgrl, ch->fChannel.fTdc);
   ////((TH2F*)    o["hTvsI"+sid])                           ->Fill(ch->fChannel.fIntgrl, ch->fChannel.fTdc);
   ((TH2F*) sd->o["hTimeVsEnergyA"+sid+"_st"+sSi])        ->Fill(ch->GetEnergyA(), ch->GetTime());
   //((TH2F*)    o["hTimeVsEnergyA"+sid])                  ->Fill(ch->GetEnergyA(), ch->GetTime());

   //((TH2F*) sd.o["hTofVsKinEnergyA"+sid+"_st"+sSi])      ->Fill(ch->GetKinEnergyAEstimate(), tof);
   //((TH2F*)    o["hTofVsKinEnergyA"+sid])                ->Fill(ch->GetKinEnergyAEstimate(), tof);
   //((TH1F*)    o["hKinEnergyA_o"+sid])                   ->Fill(ch->GetKinEnergyAEstimate());

   // Full kinematic carbon energy
   //Float_t energyEst = ch->GetKinEnergyAEstimate();          // from all channel fit
   //Float_t energy    = ch->GetKinEnergyA();

   Float_t energyEst = ch->GetKinEnergyAEstimateEDepend(); // from all channel fit
   Float_t energy    = ch->GetKinEnergyAEDepend();

   Float_t tofEst    = ch->GetTimeOfFlightEstimate();
   //Float_t tof       = ch->GetTimeOfFlight();

   //((TH2F*) sd.o["hTofVsKinEnergyA"+sid+"_st"+sSi])      ->Fill(energy, tof);
   //((TH2F*) sd.o["hTofVsKinEnergyA"+sid+"_st"+sSi])      ->Fill(energyEst, tof);
   //((TH2F*)    o["hTofVsKinEnergyA"+sid])                ->Fill(energyEst, tofEst);
   ((TH1F*)    o["hKinEnergyA_o"+sid])                   ->Fill(energyEst);
   
   UChar_t bId = ch->GetBunchId();

   ((TH1*)     o["hSpinVsChannel"+sid])                  ->Fill(ch->GetChannelId(), spinpat[bId]);
   //((TH1*)     o["hSpinVsBunch"+sid])                    ->Fill(bId, spinpat[bId]);

   //ds XXX
   UShort_t tstep = 0;

   if (runinfo.Run == 5) {
      tstep = ch->GetDelimiterId();
      //NDcounts[(int)(st/12)][event->bid][TgtIndex[delim]]++;
   } else if (runinfo.Run >= 6) {
      UInt_t ttime = ch->GetRevolutionId()/RHIC_REVOLUTION_FREQ;

      if (ttime < MAXDELIM) {
         tstep = TgtIndex[ttime];
         //++cntr.good[TgtIndex[ttime]];
         //NDcounts[(int)(st/12)][event->bid][TgtIndex[ttime]]++;
      } else if (!dproc.CMODE) {
         Error("Fill", "Time constructed from revolution #%d exeeds MAXDELIM=%d defined\n" \
               "Perhaps calibration data? Try running with --calib option", ttime, MAXDELIM);
      }
   } else {
      Warning("Fill", "Target tstep size is not defined for Run %d", runinfo.Run);
   }

   int ss_code = spinpat[bId] == 1 ? 0 : (spinpat[bId] == -1 ? 1 : 2);
   cntr_tgt.reg.NStrip[tstep][ss_code][ch->GetChannelId()-1]++;

   
   UInt_t ttime = ch->GetRevolutionId()/RHIC_REVOLUTION_FREQ;
   //((TH2F*) sd->o["hSpinVsDelim"+sid+"_st"+sSi])->Fill(ch->GetDelimiterId(), spinpat[bId]);
   ((TH2F*) sd->o["hSpinVsDelim"+sid+"_st"+sSi])->Fill(ttime, spinpat[bId]);

   //ds: XXX
   return;

   //((TH2F*)    o["hTimeVsFunnyEnergyA"+sid])             ->Fill(ch->GetFunnyEnergyA(), ch->GetTime());

   ((TH2F*) sd->o["hDLVsEnergyA"+sid+"_st"+sSi])          ->Fill(ch->GetEnergyA(), energy - ch->GetEnergyA());
   //((TH2F*)    o["hDLVsEnergyA"+sid])                    ->Fill(ch->GetEnergyA(), energyEst - ch->GetEnergyA());
   ((TH2F*) sd->o["hDLVsTotalEnergy"+sid+"_st"+sSi])      ->Fill(energy, energy - ch->GetEnergyA());
   //((TH2F*)    o["hDLVsTotalEnergy"+sid])                ->Fill(energyEst, energyEst - ch->GetEnergyA());
   ((TH2F*) sd->o["hDLVsTime"+sid+"_st"+sSi])             ->Fill(ch->GetTime(), energy - ch->GetEnergyA());
   //((TH2F*)    o["hDLVsTime"+sid])                       ->Fill(ch->GetTime(), energyEst - ch->GetEnergyA());
   ((TH2F*) sd->o["hTotalEnergyVsEnergyA"+sid+"_st"+sSi]) ->Fill(ch->GetEnergyA(), energy);
   //((TH2F*)    o["hTotalEnergyVsEnergyA"+sid])           ->Fill(ch->GetEnergyA(), energyEst);

   ((TH1F*)    o["hTof"+sid])->Fill(tofEst);

   Float_t mass = ch->GetCarbonMassEstimate();
   //cout << "mass: " << mass << endl;

   ((TH1F*) sd->o["hPseudoMass"+sid+"_st"+sSi])           ->Fill(mass);
   //((TH1F*)    o["hPseudoMass"+sid])                     ->Fill(mass);
} //}}}


/** */
void CnipolHists::FillPreProcess(ChannelEvent *ch)
{ //{{{
   DrawObjContainer *sd = d["preproc"];

   ((TH2F*) sd->o["hTimeVsEnergyA"])->Fill(ch->GetEnergyA(), ch->GetTime());
} //}}}


/** */
void CnipolHists::PreFill(string sid)
{
   char dName[256];
   char hName[256];
   //string sSi("  ");
   DrawObjContainer        *oc;
   DrawObjContainerMapIter  isubdir;

   for (int i=1; i<=NSTRIP; i++) {

      sprintf(dName, "channel%02d", i);
      oc = d.find(dName)->second;

      sprintf(hName, "hSpinVsDelim%s_st%02d", sid.c_str(), i);
      ((TH1*) oc->o[hName])->SetBins(ndelim, 0, ndelim, NUM_SPIN_STATES, -1.5, 1.5);
   }
}


/** */
void CnipolHists::PostFill() { }


/** */
void CnipolHists::SaveAllAs(TCanvas &c, std::string pattern, string path)
{ //{{{
   //Warning("SaveAllAs", "executing...");
   DrawObjContainer::SaveAllAs(c, pattern, path);

   //DrawObjContainerMapIter isubd;

   //for (isubd=d.begin(); isubd!=d.end(); ++isubd) {

   //   // Temporary test only
   //   if (isubd->first.find("channel28") == string::npos) continue;

   //   string parentPath = path;
   //   path += "/" + isubd->first;
   //   printf("path: %s\n", path.c_str());
   //   //isubd->second.SaveAllAs(c, pattern, path);
   //   path = parentPath;
   //}

   string sSi("  ");
   string dName;
   string cName;
   string fName;
   string sid = "_cut2";

   DrawObjContainer *oc;
   DrawObjContainerMapIter isubd;

   // Draw superimposed histos
   TH1* h1 = (TH1*) d["preproc"]->o["hTimeVsEnergyA"];
   TH1* h2 = (TH1*) d["preproc"]->o["hFitMeanTimeVsEnergyA"];

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

   fName = path + "/preproc/c_combo.png";
   printf("path: %s\n", fName.c_str());

   c.SetName(cName.c_str());
   c.SetTitle(cName.c_str());

   if (TPRegexp(pattern).MatchB(fName.c_str())) {
      c.SaveAs(fName.c_str());
   } else {
      //Warning("SaveAllAs", "Histogram %s name does not match pattern. Skipped", fName.c_str());
   }

   // Draw superimposed for all channels
   for (UShort_t i=1; i<=NSTRIP; i++) {

      //if (i+1 != 28) continue;

      sprintf(&sSi[0], "%02d", i);
      dName = "channel"+sSi;
      cName = "c_combo_st"+sSi;

      oc = d.find(dName)->second;

      h1 = (TH1*) oc->o["hTimeVsEnergyA"+sid+"_st"+sSi];
      h2 = (TH1*) oc->o["hFitMeanTimeVsEnergyA"+sid+"_st"+sSi];

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

      fName = path + "/" + dName + "/" + cName + ".png";
      //printf("Saving %s\n", fName.c_str());

      c.SetName(cName.c_str());
      c.SetTitle(cName.c_str());
      //c.SaveAs(fName.c_str());

      if (TPRegexp(pattern).MatchB(fName.c_str())) {
         c.SaveAs(fName.c_str());
      } else {
         //Warning("SaveAllAs", "Histogram %s name does not match pattern. Skipped", fName.c_str());
      }
   }
} //}}}
