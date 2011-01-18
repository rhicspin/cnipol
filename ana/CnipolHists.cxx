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
   CnipolHistsBookOld();
   CnipolHistsBook();
   CnipolHistsBook("_cut1");
   CnipolHistsBook("_cut2");
   BookPreProcess();
   //CnipolHistsBookExtra();
   //CnipolHistsBookExtra("_cut1");
}


CnipolHists::CnipolHists(TDirectory *dir) : DrawObjContainer(dir)
{
   CnipolHistsBookOld();
   CnipolHistsBook();
   CnipolHistsBook("_cut1");
   CnipolHistsBook("_cut2");
   BookPreProcess();
   //CnipolHistsBookExtra();
   //CnipolHistsBookExtra("_cut1");
   //ReadFromDir();
}


/** Default destructor. */
CnipolHists::~CnipolHists()
{
}


void CnipolHists::CnipolHistsBookOld()
{
   DrawObjContainer run;
   DrawObjContainer raw;
   DrawObjContainer feedback;
   DrawObjContainer kinema;
   DrawObjContainer bunch;
   DrawObjContainer errdet;
   DrawObjContainer asymmetry;

   run.fDir       = fDir;
   raw.fDir       = fDir;
   feedback.fDir  = fDir;
   //kinema.fDir    = fDir;
   kinema.fDir = new TDirectoryFile("Kinema2", "Kinema2", "", fDir);
   //kinema.fDir    = Kinema;
   bunch.fDir     = fDir;
   errdet.fDir    = fDir;
   asymmetry.fDir = fDir;

   run.o["rate_vs_delim"] = new TH2F();

   feedback.o["mdev_feedback"] = new TH2F();

   //kinema.o["energy_spectrum_all"] = new TH1F();

   char hName[256];
   char hTitle[256];

   // energy spectrum for all detector sum
   sprintf(hName, "energy_spectrum_all");
   sprintf(hTitle, "%.3f : Energy Spectrum (All Detectors)", gRunInfo.runName.c_str());
   kinema.o[hName] = new TH1F(hName, hTitle, 100, 0, 0.03);
   ((TH1F*) kinema.o[hName])->GetXaxis()->SetTitle("Momentum Transfer [-GeV/c]^2");

   for (int i=1; i<=TOT_WFD_CH; i++) {

      sprintf(hName,"mass_feedback_st%d", i);
      //sprintf(hTitle,"%.3f : Invariant Mass for Strip-%d ", runinfo.RUNID, i+1);
      sprintf(hTitle,"Invariant Mass for Strip-%d ", i);
      feedback.o[hName] = new TH1F(hName, hTitle, 100, 0, 20);
      ( (TH1F*) feedback.o[hName]) -> GetXaxis() -> SetTitle("Mass [GeV/c^2]");
      ( (TH1F*) feedback.o[hName]) -> SetLineColor(2);

      sprintf(hName,"t_vs_e_st%d", i);
      kinema.o[hName] = new TH2F();

      sprintf(hName,"t_vs_e_yescut_st%d", i);
      kinema.o[hName] = new TH2F();

      sprintf(hName,"mass_vs_e_ecut_st%d", i);
      kinema.o[hName] = new TH2F();  // Mass vs. 12C Kinetic Energy

      sprintf(hName,"mass_nocut_st%d", i);
      kinema.o[hName] = new TH1F();     // invariant mass without banana cut

      sprintf(hName,"mass_yescut_st%d", i);
      kinema.o[hName] = new TH1F();    // invariant mass with banana cut
   }

   for (int i=1; i<=NDETECTOR; i++) {
      sprintf(hName,"energy_spectrum_det%d", i);
      // energy spectrum per detector
      kinema.o[hName] = new TH1F(hName, hName, 100, 0, 0.03);
      ((TH1F*) kinema.o[hName])->GetXaxis()->SetTitle("Momentum Transfer [-GeV/c]^2");
   }

   //TF1  * banana_cut_l[NSTRIP][2];     // banana cut low
   //TF1  * banana_cut_h[NSTRIP][2];     // banana cut high
   //TLine  * energy_cut_l[NSTRIP];      // energy cut low
   //TLine  * energy_cut_h[NSTRIP];      // energy cut high

   raw.o["bunch_dist_raw"]              = new TH1F();//"asym_vs_bunch_x45", htitle, NBUNCH, -0.5, NBUNCH-0.5, 100, min, max);
   raw.o["strip_dist_raw"]              = new TH1F();
   raw.o["tdc_raw"]                     = new TH1F();
   raw.o["adc_raw"]                     = new TH1F();
   raw.o["tdc_vs_adc_raw"]              = new TH2F(); //((TH2F*) raw.o["tdc_vs_adc_raw"])->SetOption("LOGZ");
   raw.o["tdc_vs_adc_false_bunch_raw"]  = new TH2F();

   //bunch.o["bunch_dist_raw            = new TH1F();              // counts per bunch (raw)
   bunch.o["bunch_dist"]                = new TH1F();                  // counts per bunch
   bunch.o["wall_current_monitor"]      = new TH1F();        // wall current monitor
   bunch.o["specific_luminosity"]       = new TH1F();         // specific luminosity

   errdet.o["mass_chi2_vs_strip"]       = new TH2F();          // Chi2 of Gaussian Fit on Mass peak
   errdet.o["mass_sigma_vs_strip"]      = new TH2F();         // Mass sigma width vs. strip
   errdet.o["mass_e_correlation_strip"] = new TH2F();    // Mass-energy correlation vs. strip
   errdet.o["mass_pos_dev_vs_strip"]    = new TH2F();       // Mass position deviation vs. strip
   errdet.o["good_carbon_events_strip"] = new TH1I();    // number of good carbon events per strip
   errdet.o["spelumi_vs_bunch"]         = new TH2F();            // Counting rate vs. bunch
   errdet.o["bunch_spelumi"]            = new TH1F();               // Counting rate per bunch hisogram
   errdet.o["asym_bunch_x45"]           = new TH1F();              // Bunch asymmetry histogram for x45
   errdet.o["asym_bunch_x90"]           = new TH1F();              // Bunch asymmetry histogram for x90
   errdet.o["asym_bunch_y45"]           = new TH1F();              // Bunch asymmetry histogram for y45

   asymmetry.o["asym_vs_bunch_x45"]     = new TH2F();//"asym_vs_bunch_x45", htitle, NBUNCH, -0.5, NBUNCH-0.5, 100, min, max);
   asymmetry.o["asym_vs_bunch_x90"]     = new TH2F();
   asymmetry.o["asym_vs_bunch_y45"]     = new TH2F();
   asymmetry.o["asym_sinphi_fit"]       = new TH2F();
   asymmetry.o["scan_asym_sinphi_fit"]  = new TH2F();

   d["Run"]       = run;
   d["Raw"]       = raw;
   d["FeedBack"]  = feedback;
   d["Kinema2"]   = kinema;
   d["Bunch"]     = bunch;
   d["ErrDet"]    = errdet;
   d["Asymmetry"] = asymmetry;

   // 
   sprintf(hName, "hKinEnergyA_oo");
   o[hName] = new TH1F(hName, hName, 25, 22.5, 1172.2);
   ((TH1F*) o[hName])->GetXaxis()->SetTitle("Kinematic Energy, keV");
}


void CnipolHists::CnipolHistsBook(string cutid)
{
   char hName[256];

   //fDir->Print();

   //char  formula[100], fname[100];
   //sprintf(formula, "(%f/sqrt(x+130))-23", RunConst::E2T);

   sprintf(hName, "hTvsA%s", cutid.c_str());
   o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 0, 80);
   ((TH2F*) o[hName])->SetOption("colz LOGZ");
   ((TH2F*) o[hName])->GetXaxis()->SetTitle("Amplitude, ADC");
   ((TH2F*) o[hName])->GetYaxis()->SetTitle("TDC");

   sprintf(hName, "hTvsI%s", cutid.c_str());
   o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 0, 80);
   ((TH2F*) o[hName])->SetOption("colz LOGZ");
   ((TH2F*) o[hName])->GetXaxis()->SetTitle("Integral, ADC");
   ((TH2F*) o[hName])->GetYaxis()->SetTitle("TDC");

   // Time vs Energy from amplitude
   sprintf(hName, "hTimeVsEnergyA%s", cutid.c_str());
   //o[hName] = new TH2F(hName, hName, 255, 0, 1530, 100, 0, 100);
   o[hName] = new TH2F(hName, hName, 255, 0, 2530, 100, 0, 100);
   ((TH2F*) o[hName])->SetOption("colz LOGZ");
   ((TH2F*) o[hName])->GetXaxis()->SetTitle("Deposited Energy, keV");
   ((TH2F*) o[hName])->GetYaxis()->SetTitle("Time, ns");

   //sprintf(fname, "banana_nominal%s", cutid.c_str());
   //TF1 *banana_cut_l = new TF1(fname, formula, 0, 1500);
   //((TH1D*) o[hName])->GetListOfFunctions()->Add(banana_cut_l);

   // TOF vs Kinematic Energy
   sprintf(hName, "hTofVsKinEnergyA%s", cutid.c_str());
   o[hName] = new TH2F(hName, hName, 255, 0, 1785, 100, 20, 120);
   ((TH2F*) o[hName])->SetOption("colz LOGZ");
   ((TH2F*) o[hName])->GetXaxis()->SetTitle("Kinematic Energy, keV");
   ((TH2F*) o[hName])->GetYaxis()->SetTitle("ToF, ns");

   // Kinematic Energy
   sprintf(hName, "hKinEnergyA_o%s", cutid.c_str());
   o[hName] = new TH1F(hName, hName, 25, 22.5, 1172.2);
   ((TH1F*) o[hName])->GetXaxis()->SetTitle("Kinematic Energy, keV");

   // Spin vs Strip Id
   sprintf(hName, "hSpinVsChannel%s", cutid.c_str());
   o[hName] = new TH2I(hName, hName, NSTRIP, 1, NSTRIP+1, NUM_SPIN_STATES, -1.5, 1.5);
   ((TH1*) o[hName])->SetOption("colz");
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Channel Id");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Spin State");

   // Spin vs Bunch Id
   sprintf(hName, "hSpinVsBunch%s", cutid.c_str());
   o[hName] = new TH2I(hName, hName, NBUNCH, 0, NBUNCH, NUM_SPIN_STATES, -1.5, 1.5);
   ((TH1*) o[hName])->SetOption("colz");
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Bunch Id");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Spin State");

   // Time vs Energy from amplitude
   sprintf(hName, "hTimeVsFunnyEnergyA%s", cutid.c_str());
   o[hName] = new TH2F(hName, hName, 255, 0, 1530, 100, 0, 100);
   ((TH2F*) o[hName])->SetOption("colz LOGZ");
   ((TH2F*) o[hName])->GetXaxis()->SetTitle("Funny Energy, keV");
   ((TH2F*) o[hName])->GetYaxis()->SetTitle("Time, ns");

   //sprintf(fname, "banana_nominal_funny%s", cutid.c_str());
   //banana_cut_l = new TF1(fname, formula, 0, 1500);
   //((TH1D*) o[hName])->GetListOfFunctions()->Add(banana_cut_l);

   sprintf(hName, "hFitMeanTimeVsEnergyA%s", cutid.c_str());
   o[hName] = new TH1D(hName, hName, 255, 0, 1530);
   //((TH2F*) o[hName])->SetOption("colz LOGZ");
   ((TH1D*) o[hName])->GetYaxis()->SetRangeUser(0, 100);
   ((TH1D*) o[hName])->GetXaxis()->SetTitle("Deposited Energy, keV");
   ((TH1D*) o[hName])->GetYaxis()->SetTitle("Mean Time, ns");

   sprintf(hName, "hDLVsEnergyA%s", cutid.c_str());
   o[hName] = new TH2F(hName, hName, 100, 100, 1300, 100, -400, 1100);
   ((TH2F*) o[hName])->SetOption("colz");
   //((TH2F*) o[hName])->GetYaxis()->SetRangeUser(0, 20000);

   sprintf(hName, "hDLVsTotalEnergy%s", cutid.c_str());
   o[hName] = new TH2F(hName, hName, 100, 100, 1600, 100, -200, 600);
   ((TH2F*) o[hName])->SetOption("colz");

   sprintf(hName, "hDLVsTime%s", cutid.c_str());
   o[hName] = new TH2F(hName, hName, 100, 0, 100, 100, -200, 600);
   ((TH2F*) o[hName])->SetOption("colz");

   sprintf(hName, "hTotalEnergyVsEnergyA%s", cutid.c_str());
   o[hName] = new TH2F(hName, hName, 100, 100, 1300, 100, 100, 1600);
   ((TH2F*) o[hName])->SetOption("colz");

   sprintf(hName, "hTof%s", cutid.c_str());
   o[hName] = new TH1D(hName, hName, 100, 0, 100);

   sprintf(hName, "hPseudoMass%s", cutid.c_str());
   o[hName] = new TH1D(hName, hName, 100, 0, 20);

   char dName[256];
   //string sSi("  ");
   DrawObjContainer *oc;
   DrawObjContainerMapIter isubdir;

   for (int i=0; i<TOT_WFD_CH; i++) {

      //sprintf(&sSi[0], "%02d", i+1);

      //fDir->cd();
      sprintf(dName, "channel%02d", i+1);

      isubdir = d.find(dName);

      if ( isubdir == d.end()) { // if dir not found
         oc = new DrawObjContainer();
         oc->fDir = new TDirectoryFile(dName, dName, "", fDir);
      } else {
         oc = &isubdir->second;
      }

      sprintf(hName, "hTvsA%s_st%02d", cutid.c_str(), i+1);
      oc->o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 0, 80);
      ((TH2F*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH2F*) oc->o[hName])->GetXaxis()->SetTitle("Amplitude, ADC");
      ((TH2F*) oc->o[hName])->GetYaxis()->SetTitle("TDC");

      sprintf(hName, "hTvsI%s_st%02d", cutid.c_str(), i+1);
      //sprintf(hName, "hTvsI", i+1);
      oc->o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 0, 80);
      ((TH2F*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH2F*) oc->o[hName])->GetXaxis()->SetTitle("Integral, ADC");
      ((TH2F*) oc->o[hName])->GetYaxis()->SetTitle("TDC");

      //sprintf(fname, "banana_nominal%s_st%02d", cutid.c_str(), i+1);
      //banana_cut_l = new TF1(fname, formula, 0, 1500);

      // Time vs Energy from amplitude
      sprintf(hName, "hTimeVsEnergyA%s_st%02d", cutid.c_str(), i+1);
      oc->o[hName] = new TH2F(hName, hName, 255, 0, 1530, 100, 0, 100);
      ((TH2F*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH2F*) oc->o[hName])->GetXaxis()->SetTitle("Deposited Energy, keV");
      ((TH2F*) oc->o[hName])->GetYaxis()->SetTitle("Time, ns");
      //((TH2F*) oc->o[hName])->GetListOfFunctions()->Add(banana_cut_l);

      // TOF vs Kinematic Energy
      sprintf(hName, "hTofVsKinEnergyA%s_st%02d", cutid.c_str(), i+1);
      oc->o[hName] = new TH2F(hName, hName, 255, 0, 1785, 100, 20, 120);
      ((TH2F*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH2F*) oc->o[hName])->GetXaxis()->SetTitle("Kinematic Energy, keV");
      ((TH2F*) oc->o[hName])->GetYaxis()->SetTitle("ToF, ns");

      sprintf(hName, "hFitMeanTimeVsEnergyA%s_st%02d", cutid.c_str(), i+1);
      oc->o[hName] = new TH1D(hName, hName, 255, 0, 1530);
      //((TH2F*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH1D*) oc->o[hName])->GetYaxis()->SetRangeUser(0, 100);
      ((TH1D*) oc->o[hName])->GetXaxis()->SetTitle("Deposited Energy, keV");
      ((TH1D*) oc->o[hName])->GetYaxis()->SetTitle("Mean Time, ns");

      sprintf(hName, "hDLVsEnergyA%s_st%02d", cutid.c_str(), i+1);
      oc->o[hName] = new TH2F(hName, hName, 100, 100, 1300, 100, -400, 1100);
      ((TH2F*) oc->o[hName])->SetOption("colz");
      //((TH2F*) oc->o[hName])->GetYaxis()->SetRangeUser(0, 20000);

      sprintf(hName, "hDLVsTotalEnergy%s_st%02d", cutid.c_str(), i+1);
      oc->o[hName] = new TH2F(hName, hName, 100, 100, 1600, 100, -200, 600);
      ((TH2F*) oc->o[hName])->SetOption("colz");

      sprintf(hName, "hDLVsTime%s_st%02d", cutid.c_str(), i+1);
      oc->o[hName] = new TH2F(hName, hName, 100, 0, 100, 100, -200, 600);
      ((TH2F*) oc->o[hName])->SetOption("colz");

      sprintf(hName, "hTotalEnergyVsEnergyA%s_st%02d", cutid.c_str(), i+1);
      oc->o[hName] = new TH2F(hName, hName, 100, 100, 1300, 100, 100, 1600);
      ((TH2F*) oc->o[hName])->SetOption("colz");

      sprintf(hName, "hPseudoMass%s_st%02d", cutid.c_str(), i+1);
      oc->o[hName] = new TH1D(hName, hName, 100, 0, 20);

      if ( isubdir == d.end()) {
         d[dName] = *oc;
         delete oc;
      }
   }

   //delete banana_cut_l;
}


/** */
void CnipolHists::CnipolHistsBookExtra(string cutid)
{ //{{{
   //char hName[256];

   //TH2F* hData = (TH2F*) o["hTimeVsEnergyA"+cutid];

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
{
   char hName[256];
   string dName = "preproc";

   DrawObjContainer *oc;
   DrawObjContainerMapIter isubdir;

   isubdir = d.find(dName);

   if ( isubdir == d.end()) { // if dir not found
      oc = new DrawObjContainer();
      oc->fDir = new TDirectoryFile(dName.c_str(), dName.c_str(), "", fDir);
   } else {
      oc = &isubdir->second;
   }

   sprintf(hName, "hTimeVsEnergyA");
   //oc->o[hName] = new TH2F(hName, hName, 255, 0, 1530, 100, 0, 100);
   oc->o[hName] = new TH2F(hName, hName, 255, 0, 2550, 100, 0, 100);
   ((TH2F*) oc->o[hName])->SetOption("colz LOGZ");
   ((TH2F*) oc->o[hName])->GetXaxis()->SetTitle("Deposited Energy, keV");
   ((TH2F*) oc->o[hName])->GetYaxis()->SetTitle("Time, ns");

   sprintf(hName, "hFitMeanTimeVsEnergyA");
   //oc->o[hName] = new TH1D(hName, hName, 255, 0, 1530);
   oc->o[hName] = new TH1D(hName, hName, 255, 0, 2550);
   ((TH1D*) oc->o[hName])->GetYaxis()->SetRangeUser(0, 100);
   ((TH1D*) oc->o[hName])->GetXaxis()->SetTitle("Deposited Energy, keV");
   ((TH1D*) oc->o[hName])->GetYaxis()->SetTitle("Mean Time, ns");

   if ( isubdir == d.end()) {
      d[dName] = *oc;
      delete oc;
   }
}


/** */
void CnipolHists::Print(const Option_t* opt) const
{
   opt = "";

   //printf("CnipolHists:\n");
}


/** */
Int_t CnipolHists::Write(const char* name, Int_t option, Int_t bufsize)
{
   if (!fDir) {
      printf("ERROR: CnipolHists::Write(): fDir not defined\n");
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
          sname.find("preproc") != string::npos ||
          sname.find("Kinema2") != string::npos )
      {//continue;
         isubd->second.Write();
      }
   }

   return 0;
}


void CnipolHists::Fill(ChannelEvent *ch, string cutid)
{
   UChar_t chId  = ch->GetChannelId();
   UChar_t detId = ch->GetDetectorId();

   // by detector id
   //DrawObjContainer &sd1 = d["Kinema"];

   //string sDetId(" ");
   //sprintf(&sDetId[0], "%1d", detId);
   char hName[256];
   
   if (cutid == "_cut2") { // fill these if only pass the carbon mass cut
      ((TH1F*) d["Kinema2"].o["energy_spectrum_all"])->Fill( fabs(ch->GetMandelstamT()) );
      sprintf(hName,"energy_spectrum_det%d", detId);
      ((TH1F*) d["Kinema2"].o[hName])->Fill( fabs(ch->GetMandelstamT()) );
   }

   // by channel id
   string sSi("  ");
   sprintf(&sSi[0], "%02d", chId);

   DrawObjContainer &sd = d["channel"+sSi];
   //sd = d["channel"+sSi];

   ((TH2F*) sd.o["hTvsA"+cutid+"_st"+sSi])                 ->Fill(ch->fChannel.fAmpltd, ch->fChannel.fTdc);
   ((TH2F*)    o["hTvsA"+cutid])                           ->Fill(ch->fChannel.fAmpltd, ch->fChannel.fTdc);
   ((TH2F*) sd.o["hTvsI"+cutid+"_st"+sSi])                 ->Fill(ch->fChannel.fIntgrl, ch->fChannel.fTdc);
   ((TH2F*)    o["hTvsI"+cutid])                           ->Fill(ch->fChannel.fIntgrl, ch->fChannel.fTdc);
   ((TH2F*) sd.o["hTimeVsEnergyA"+cutid+"_st"+sSi])        ->Fill(ch->GetEnergyA(), ch->GetTime());
   ((TH2F*)    o["hTimeVsEnergyA"+cutid])                  ->Fill(ch->GetEnergyA(), ch->GetTime());

   Float_t tof = ch->GetTimeOfFlightEstimate();

   ((TH2F*) sd.o["hTofVsKinEnergyA"+cutid+"_st"+sSi])      ->Fill(ch->GetKinEnergyAEstimate(), tof);
   ((TH2F*)    o["hTofVsKinEnergyA"+cutid])                ->Fill(ch->GetKinEnergyAEstimate(), tof);

   ((TH1F*)    o["hKinEnergyA_o"+cutid])                   ->Fill(ch->GetKinEnergyAEstimate());
   
   UChar_t bId = ch->GetBunchId();

   ((TH1*)     o["hSpinVsChannel"+cutid])                  ->Fill(ch->GetChannelId(), spinpat[bId]);
   ((TH1*)     o["hSpinVsBunch"+cutid])                    ->Fill(bId, spinpat[bId]);

   ((TH2F*)    o["hTimeVsFunnyEnergyA"+cutid])             ->Fill(ch->GetFunnyEnergyA(), ch->GetTime());

   float t0 = 25;
   // XXX has to be changed
   float energy = 0.5 * MASS_12C * CARBON_PATH_DISTANCE * CARBON_PATH_DISTANCE /
                 ((ch->GetTime()+t0) * (ch->GetTime()+t0) * C_CMNS * C_CMNS);

   ((TH2F*) sd.o["hDLVsEnergyA"+cutid+"_st"+sSi])          ->Fill(ch->GetEnergyA(), energy - ch->GetEnergyA());
   ((TH2F*)    o["hDLVsEnergyA"+cutid])                    ->Fill(ch->GetEnergyA(), energy - ch->GetEnergyA());
   ((TH2F*) sd.o["hDLVsTotalEnergy"+cutid+"_st"+sSi])      ->Fill(energy, energy - ch->GetEnergyA());
   ((TH2F*)    o["hDLVsTotalEnergy"+cutid])                ->Fill(energy, energy - ch->GetEnergyA());
   ((TH2F*) sd.o["hDLVsTime"+cutid+"_st"+sSi])             ->Fill(ch->GetTime(), energy - ch->GetEnergyA());
   ((TH2F*)    o["hDLVsTime"+cutid])                       ->Fill(ch->GetTime(), energy - ch->GetEnergyA());
   ((TH2F*) sd.o["hTotalEnergyVsEnergyA"+cutid+"_st"+sSi]) ->Fill(ch->GetEnergyA(), energy);
   ((TH2F*)    o["hTotalEnergyVsEnergyA"+cutid])           ->Fill(ch->GetEnergyA(), energy);

   ((TH1F*)    o["hTof"+cutid])->Fill(tof);

   Float_t mass = ch->GetCarbonMassEstimate();
   //cout << "mass: " << mass << endl;

   ((TH1F*) sd.o["hPseudoMass"+cutid+"_st"+sSi])           ->Fill(mass);
   ((TH1F*)    o["hPseudoMass"+cutid])                     ->Fill(mass);
}


/** */
void CnipolHists::FillPreProcess(ChannelEvent *ch)
{
   UChar_t chId = ch->fEventId.fChannelId;

   DrawObjContainer &sd = d["preproc"];

   ((TH2F*) sd.o["hTimeVsEnergyA"])->Fill(ch->GetEnergyA(), ch->GetTime());
}


/** */
void CnipolHists::PostFill()
{
}


/** */
void CnipolHists::SaveAllAs(TCanvas &c, string path)
{
   //Warning("SaveAllAs", "executing...");
   DrawObjContainer::SaveAllAs(c, path);

   //DrawObjContainerMapIter isubd;

   //for (isubd=d.begin(); isubd!=d.end(); ++isubd) {

   //   // Temporary test only
   //   if (isubd->first.find("channel28") == string::npos) continue;

   //   string parentPath = path;
   //   path += "/" + isubd->first;
   //   printf("path: %s\n", path.c_str());
   //   //isubd->second.SaveAllAs(c, path);
   //   path = parentPath;
   //}

   string sSi("  ");
   string dName;
   string cName;
   string fName;
   string cutId = "_cut2";

   DrawObjContainer *oc;
   DrawObjContainerMapIter isubd;

   // Draw superimposed histos
   TH1* h1 = (TH1*) d["preproc"].o["hTimeVsEnergyA"];
   TH1* h2 = (TH1*) d["preproc"].o["hFitMeanTimeVsEnergyA"];

   c.cd();
   char *l = strstr(h1->GetOption(), "LOGZ");
   //printf("XXX1: set logz %s\n", ((TH1*)io->second)->GetOption());
   if (l) { c.SetLogz(kTRUE);
      //printf("XXX2: set logz \n");
   } else { c.SetLogz(kFALSE); }

   h1->Draw();
   h2->Draw("same");

   fName = path + "/preproc/c_combo.png";
   printf("path: %s\n", fName.c_str());

   c.SetName(cName.c_str());
   c.SetTitle(cName.c_str());
   c.SaveAs(fName.c_str());

   return;

   // Draw superimposed for all channels
   for (UShort_t i=1; i<=NSTRIP; i++) {

      //if (i+1 != 28) continue;

      sprintf(&sSi[0], "%02d", i);
      dName = "channel"+sSi;
      cName = "c_combo_st"+sSi;

      oc = &d.find(dName)->second;

      h1 = (TH1*) oc->o["hTimeVsEnergyA"+cutId+"_st"+sSi];
      h2 = (TH1*) oc->o["hFitMeanTimeVsEnergyA"+cutId+"_st"+sSi];

      c.cd();

      char *l = strstr(h1->GetOption(), "LOGZ");
      //printf("XXX1: set logz %s\n", ((TH1*)io->second)->GetOption());
      if (l) { c.SetLogz(kTRUE);
         //printf("XXX2: set logz \n");
      } else { c.SetLogz(kFALSE); }

      h1->Draw();
      h2->Draw("same");

      fName = path + "/" + dName + "/" + cName + ".png";
      printf("path: %s\n", fName.c_str());

      c.SetName(cName.c_str());
      c.SetTitle(cName.c_str());
      c.SaveAs(fName.c_str());
   }
}
