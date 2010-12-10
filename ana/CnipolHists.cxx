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
}


CnipolHists::CnipolHists(TDirectory *dir) : DrawObjContainer(dir)
{
   CnipolHistsBookOld();
   CnipolHistsBook();
   CnipolHistsBook("_cut1");
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
   kinema.fDir    = fDir;
   bunch.fDir     = fDir;
   errdet.fDir    = fDir;
   asymmetry.fDir = fDir;

   run.o["rate_vs_delim"] = new TH2F();

   feedback.o["mdev_feedback"] = new TH2F();

   kinema.o["energy_spectrum_all"] = new TH1F();        // energy spectrum for all detector sum

   char hName[256];
   char hTitle[256];

   for (int i=0; i<TOT_WFD_CH; i++) {
      sprintf(hName,"mass_feedback_st%d", i+1);
      //sprintf(hTitle,"%.3f : Invariant Mass for Strip-%d ", runinfo.RUNID, i+1);
      sprintf(hTitle,"Invariant Mass for Strip-%d ", i+1);
      feedback.o[hName] = new TH1F(hName, hTitle, 100, 0, 20);
      ( (TH1F*) feedback.o[hName]) -> GetXaxis() -> SetTitle("Mass [GeV/c^2]");
      ( (TH1F*) feedback.o[hName]) -> SetLineColor(2);

      sprintf(hName,"t_vs_e_st%d", i+1);
      kinema.o[hName] = new TH2F();

      sprintf(hName,"t_vs_e_yescut_st%d", i+1);
      kinema.o[hName] = new TH2F();

      sprintf(hName,"mass_vs_e_ecut_st%d", i+1);
      kinema.o[hName] = new TH2F();  // Mass vs. 12C Kinetic Energy

      sprintf(hName,"mass_nocut_st%d", i+1);
      kinema.o[hName] = new TH1F();     // invariant mass without banana cut

      sprintf(hName,"mass_yescut_st%d", i+1);
      kinema.o[hName] = new TH1F();    // invariant mass with banana cut
   }

   for (int i=0; i<NDETECTOR; i++) {
      sprintf(hName,"energy_spectrum_det%d", i+1);
      kinema.o[hName] = new TH1F(); // energy spectrum per detector
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
   d["Kinema"]    = kinema;
   d["Bunch"]     = bunch;
   d["ErrDet"]    = errdet;
   d["Asymmetry"] = asymmetry;
}


void CnipolHists::CnipolHistsBook(string cutid)
{
   char hName[256];

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
   o[hName] = new TH2F(hName, hName, 255, 0, 1530, 100, 0, 100);
   ((TH2F*) o[hName])->SetOption("colz LOGZ");
   ((TH2F*) o[hName])->GetXaxis()->SetTitle("Deposited Energy, keV");
   ((TH2F*) o[hName])->GetYaxis()->SetTitle("Time, ns");

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

   sprintf(hName, "hPseudoMass%s", cutid.c_str());
   o[hName] = new TH1D(hName, hName, 100, 0, 6);

   char dName[256];
   //string sSi("  ");

   DrawObjContainer *oc;
   DrawObjContainerMapIter isubdir;

   for (int i=0; i<TOT_WFD_CH; i++) {

      //sprintf(&sSi[0], "%02d", i+1);

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

      // Time vs Energy from amplitude
      sprintf(hName, "hTimeVsEnergyA%s_st%02d", cutid.c_str(), i+1);
      oc->o[hName] = new TH2F(hName, hName, 255, 0, 1530, 100, 0, 100);
      ((TH2F*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH2F*) oc->o[hName])->GetXaxis()->SetTitle("Deposited Energy, keV");
      ((TH2F*) oc->o[hName])->GetYaxis()->SetTitle("Time, ns");

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
      oc->o[hName] = new TH1D(hName, hName, 100, 0, 6);

      if ( isubdir == d.end()) {
         d[dName] = *oc;
         delete oc;
      }
   }
};

/**
 *
 */
void CnipolHists::Print(const Option_t* opt) const
{
   opt = "";

   //printf("CnipolHists:\n");
}


/**
 *
 */
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
      if (sname.find("channel") == string::npos) continue;
      isubd->second.Write();
   }

   return 0;
}


void CnipolHists::Fill(ChannelEvent *ch, string cutid)
{
   UChar_t chId = ch->fEventId.fChannelId;
   string sSi("  ");
   sprintf(&sSi[0], "%02d", chId+1);

   DrawObjContainer &os = d["channel"+sSi];

   ((TH2F*) os.o["hTvsA"+cutid+"_st"+sSi])         ->Fill(ch->fChannel.fAmpltd, ch->fChannel.fTdc);
   ((TH2F*)    o["hTvsA"+cutid])                   ->Fill(ch->fChannel.fAmpltd, ch->fChannel.fTdc);
   ((TH2F*) os.o["hTvsI"+cutid+"_st"+sSi])         ->Fill(ch->fChannel.fIntgrl, ch->fChannel.fTdc);
   ((TH2F*)    o["hTvsI"+cutid])                   ->Fill(ch->fChannel.fIntgrl, ch->fChannel.fTdc);
   ((TH2F*) os.o["hTimeVsEnergyA"+cutid+"_st"+sSi])->Fill(ch->GetEnergyA(), ch->GetTime());
   ((TH2F*)    o["hTimeVsEnergyA"+cutid])          ->Fill(ch->GetEnergyA(), ch->GetTime());

   float t0 = 25;
   float energy = 0.5 * MASS_12C * CARBON_PATH_DISTANCE * CARBON_PATH_DISTANCE /
                 ((ch->GetTime()+t0) * (ch->GetTime()+t0) * C_CMNS * C_CMNS);

   //float Mass = t*t*e*runconst.T2M * k2G;
   float mass = ch->GetTime() * ch->GetTime() * ch->GetEnergyA() *
               (2*C_CMNS*C_CMNS/CARBON_PATH_DISTANCE/CARBON_PATH_DISTANCE) *k2G;

   ((TH2F*) os.o["hDLVsEnergyA"+cutid+"_st"+sSi])         ->Fill(ch->GetEnergyA(), energy - ch->GetEnergyA());
   ((TH2F*)    o["hDLVsEnergyA"+cutid])                   ->Fill(ch->GetEnergyA(), energy - ch->GetEnergyA());
   ((TH2F*) os.o["hDLVsTotalEnergy"+cutid+"_st"+sSi])     ->Fill(energy, energy - ch->GetEnergyA());
   ((TH2F*)    o["hDLVsTotalEnergy"+cutid])               ->Fill(energy, energy - ch->GetEnergyA());
   ((TH2F*) os.o["hDLVsTime"+cutid+"_st"+sSi])            ->Fill(ch->GetTime(), energy - ch->GetEnergyA());
   ((TH2F*)    o["hDLVsTime"+cutid])                      ->Fill(ch->GetTime(), energy - ch->GetEnergyA());
   ((TH2F*) os.o["hTotalEnergyVsEnergyA"+cutid+"_st"+sSi])->Fill(ch->GetEnergyA(), energy);
   ((TH2F*)    o["hTotalEnergyVsEnergyA"+cutid])          ->Fill(ch->GetEnergyA(), energy);

   ((TH1F*) os.o["hPseudoMass"+cutid+"_st"+sSi])          ->Fill(mass);
   ((TH1F*)    o["hPseudoMass"+cutid])                    ->Fill(mass);
}


/**
 *
 */
void CnipolHists::SaveAllAs(TCanvas &c, string path)
{
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
   string cutId = "_cut1";

   DrawObjContainer *oc;
   DrawObjContainerMapIter isubd;

   for (UShort_t i=1; i<=NSTRIP; i++) {

      //if (i+1 != 28) continue;

      sprintf(&sSi[0], "%02d", i);
      dName = "channel"+sSi;
      cName = "c_combo_st"+sSi;

      oc = &d.find(dName)->second;

      TH1* h1 = (TH1*) oc->o["hTimeVsEnergyA"+cutId+"_st"+sSi];
      TH1* h2 = (TH1*) oc->o["hFitMeanTimeVsEnergyA"+cutId+"_st"+sSi];

      c.cd();

      h1->Draw();
      h2->Draw("same");

      fName = path + "/" + dName + "/" + cName + ".png";
      printf("path: %s\n", fName.c_str());

      c.SetName(cName.c_str());
      c.SetTitle(cName.c_str());
      c.SaveAs(fName.c_str());
   }
}
