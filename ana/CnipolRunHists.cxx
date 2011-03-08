/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolRunHists.h"

#include "AsymGlobals.h"


ClassImp(CnipolRunHists)

using namespace std;

/** Default constructor. */
CnipolRunHists::CnipolRunHists() : DrawObjContainer()
{
   BookHists();
}


CnipolRunHists::CnipolRunHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
}


/** Default destructor. */
CnipolRunHists::~CnipolRunHists()
{
}


/** */
void CnipolRunHists::BookHists(string sid)
{ //{{{
   TDirectory *dir;

   //dir = new TDirectoryFile("run", "run", "", fDir);
   dir = Run;
   DrawObjContainer *run = new DrawObjContainer(dir);

   //dir = new TDirectoryFile("raw", "raw", "", fDir);
   dir = Raw;
   DrawObjContainer *raw = new DrawObjContainer(dir);

   //dir = new TDirectoryFile("feedback", "feedback", "", fDir);
   //dir = Feedback;
   //DrawObjContainer *feedback = new DrawObjContainer(dir);

   //DrawObjContainer *kinema;

   dir = Bunch;
   DrawObjContainer *bunch = new DrawObjContainer(dir);

   //DrawObjContainer *errdet;

   dir = Asymmetry;
   DrawObjContainer *asymmetry = new DrawObjContainer(dir);

   //feedback->fDir  = fDir;
   ////kinema->fDir    = fDir;
   //kinema->fDir = new TDirectoryFile("Kinema2", "Kinema2", "", fDir);
   ////kinema->fDir    = Kinema;
   //errdet->fDir    = fDir;

   run->o["rate_vs_delim"] = rate_vs_delim;
   run->o["tgtx_vs_time"]  = tgtx_vs_time; //new TH2F();

   //feedback->o["mdev_feedback"] = new TH2F();

   //kinema->o["energy_spectrum_all"] = new TH1F();

   char hName[256];
   //char hTitle[256];

   // energy spectrum for all detector sum
   //sprintf(hName, "energy_spectrum_all");
   //sprintf(hTitle, "%.3f : Energy Spectrum (All Detectors)", gRunInfo.runName.c_str());
   //kinema->o[hName] = new TH1F(hName, hTitle, 100, 0, 0.03);
   //((TH1F*) kinema->o[hName])->GetXaxis()->SetTitle("Momentum Transfer [-GeV/c]^2");

   //for (int i=1; i<=TOT_WFD_CH; i++) {

   //   sprintf(hName,"mass_feedback_st%d", i);
   //   //sprintf(hTitle,"%.3f : Invariant Mass for Strip-%d ", runinfo->RUNID, i+1);
   //   sprintf(hTitle,"Invariant Mass for Strip-%d ", i);
   //   feedback->o[hName] = new TH1F(hName, hTitle, 100, 0, 20);
   //   ( (TH1F*) feedback->o[hName]) -> GetXaxis() -> SetTitle("Mass [GeV/c^2]");
   //   ( (TH1F*) feedback->o[hName]) -> SetLineColor(2);

   //   sprintf(hName,"t_vs_e_st%d", i);
   //   kinema->o[hName] = new TH2F();

   //   sprintf(hName,"t_vs_e_yescut_st%d", i);
   //   kinema->o[hName] = new TH2F();

   //   sprintf(hName,"mass_vs_e_ecut_st%d", i);
   //   kinema->o[hName] = new TH2F();  // Mass vs. 12C Kinetic Energy

   //   sprintf(hName,"mass_nocut_st%d", i);
   //   kinema->o[hName] = new TH1F();     // invariant mass without banana cut

   //   sprintf(hName,"mass_yescut_st%d", i);
   //   kinema->o[hName] = new TH1F();    // invariant mass with banana cut
   //}

   //for (int i=1; i<=NDETECTOR; i++) {
   //   sprintf(hName,"energy_spectrum_det%d", i);
   //   // energy spectrum per detector
   //   kinema->o[hName] = new TH1F(hName, hName, 100, 0, 0.03);
   //   ((TH1F*) kinema->o[hName])->GetXaxis()->SetTitle("Momentum Transfer [-GeV/c]^2");
   //}

   //TF1  * banana_cut_l[NSTRIP][2];     // banana cut low
   //TF1  * banana_cut_h[NSTRIP][2];     // banana cut high
   //TLine  * energy_cut_l[NSTRIP];      // energy cut low
   //TLine  * energy_cut_h[NSTRIP];      // energy cut high

   raw->o["bunch_dist_raw"]              = bunch_dist_raw;
   raw->o["strip_dist_raw"]              = strip_dist_raw;
   raw->o["tdc_raw"]                     = tdc_raw;
   raw->o["adc_raw"]                     = adc_raw;
   raw->o["tdc_vs_adc_raw"]              = tdc_vs_adc_raw;
   raw->o["tdc_vs_adc_false_bunch_raw"]  = tdc_vs_adc_false_bunch_raw;

   bunch->o["bunch_dist"]                = bunch_dist;
   bunch->o["wall_current_monitor"]      = wall_current_monitor;
   bunch->o["specific_luminosity"]       = specific_luminosity;

   //errdet->o["mass_chi2_vs_strip"]       = new TH2F();          // Chi2 of Gaussian Fit on Mass peak
   //errdet->o["mass_sigma_vs_strip"]      = new TH2F();         // Mass sigma width vs. strip
   //errdet->o["mass_e_correlation_strip"] = new TH2F();    // Mass-energy correlation vs-> strip
   //errdet->o["mass_pos_dev_vs_strip"]    = new TH2F();       // Mass position deviation vs. strip
   //errdet->o["good_carbon_events_strip"] = new TH1I();    // number of good carbon events per strip
   //errdet->o["spelumi_vs_bunch"]         = new TH2F();            // Counting rate vs. bunch
   //errdet->o["bunch_spelumi"]            = new TH1F();               // Counting rate per bunch hisogram
   //errdet->o["asym_bunch_x45"]           = new TH1F();              // Bunch asymmetry histogram for x45
   //errdet->o["asym_bunch_x90"]           = new TH1F();              // Bunch asymmetry histogram for x90
   //errdet->o["asym_bunch_y45"]           = new TH1F();              // Bunch asymmetry histogram for y45

   asymmetry->o["asym_vs_bunch_x45"]    = asym_vs_bunch_x45;
   asymmetry->o["asym_vs_bunch_x90"]    = asym_vs_bunch_x90;
   asymmetry->o["asym_vs_bunch_y45"]    = asym_vs_bunch_y45;
   asymmetry->o["asym_sinphi_fit"]      = asym_sinphi_fit;
   asymmetry->o["scan_asym_sinphi_fit"] = scan_asym_sinphi_fit;

   d["Run"]       = run;
   d["Raw"]       = raw;
   //d["FeedBack"]  = feedback;
   //d["Kinema2"]   = kinema;
   d["Bunch"]     = bunch;
   //d["ErrDet"]    = errdet;
   d["Asymmetry"] = asymmetry;

   //
   sprintf(hName, "hKinEnergyA_oo");
   o[hName] = new TH1F(hName, hName, 25, 22.5, 1172.2);
   ((TH1F*) o[hName])->GetXaxis()->SetTitle("Kinematic Energy, keV");

   // 
   //sprintf(hName, "hSpinVsChannel%s", sid.c_str());
   sprintf(hName, "hTargetSteps");
   o[hName] = new TH1I(hName, hName, 100, 0, 100);
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Target Steps");
   ((TH1*) o[hName])->SetBit(TH1::kCanRebin);

} //}}}


/** */
void CnipolRunHists::Print(const Option_t* opt) const
{ //{{{
   opt = "";

   //printf("CnipolHists:\n");
   DrawObjContainer::Print();
} //}}}


/** */
void CnipolRunHists::Fill(ChannelEvent *ch, string sid)
{ //{{{
   //UChar_t chId  = ch->GetChannelId();
   //UChar_t detId = ch->GetDetectorId();
   //UShort_t delim = ch->GetDelimiterId();

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

   ((TH1*) o["hTargetSteps"])->Fill(tstep);
} //}}}
