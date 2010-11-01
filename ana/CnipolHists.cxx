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
   CnipolHistsBook();
};



CnipolHists::CnipolHists(TDirectory *dir) : DrawObjContainer(dir)
{
   CnipolHistsBook();
   ReadFromDir();
};


/** Default destructor. */
CnipolHists::~CnipolHists()
{
};


void CnipolHists::CnipolHistsBook()
{
   char hName[256];
   char hTitle[256];

   DrawObjContainer run;
   DrawObjContainer raw;
   DrawObjContainer feedback;
   DrawObjContainer kinema;
   DrawObjContainer bunch;
   DrawObjContainer errdet;
   DrawObjContainer asymmetry;

   run.o["rate_vs_delim"] = new TH2F();

   feedback.o["mdev_feedback"] = new TH2F();

   kinema.o["energy_spectrum_all"] = new TH1F();        // energy spectrum for all detector sum

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

   raw.o["bunch_dist_raw"] = new TH1F();//"asym_vs_bunch_x45", htitle, NBUNCH, -0.5, NBUNCH-0.5, 100, min, max);
   raw.o["strip_dist_raw"] = new TH1F();
   raw.o["tdc_raw"] = new TH1F();
   raw.o["adc_raw"] = new TH1F();
   raw.o["tdc_vs_adc_raw"] = new TH2F();
   raw.o["tdc_vs_adc_false_bunch_raw"] = new TH2F();

   //bunch.o["bunch_dist_raw = new TH1F();              // counts per bunch (raw)
   bunch.o["bunch_dist"] = new TH1F();                  // counts per bunch
   bunch.o["wall_current_monitor"] = new TH1F();        // wall current monitor
   bunch.o["specific_luminosity"] = new TH1F();         // specific luminosity

   errdet.o["mass_chi2_vs_strip"] = new TH2F();          // Chi2 of Gaussian Fit on Mass peak
   errdet.o["mass_sigma_vs_strip"] = new TH2F();         // Mass sigma width vs. strip
   errdet.o["mass_e_correlation_strip"] = new TH2F();    // Mass-energy correlation vs. strip
   errdet.o["mass_pos_dev_vs_strip"] = new TH2F();       // Mass position deviation vs. strip
   errdet.o["good_carbon_events_strip"] = new TH1I();    // number of good carbon events per strip
   errdet.o["spelumi_vs_bunch"] = new TH2F();            // Counting rate vs. bunch
   errdet.o["bunch_spelumi"] = new TH1F();               // Counting rate per bunch hisogram
   errdet.o["asym_bunch_x45"] = new TH1F();              // Bunch asymmetry histogram for x45
   errdet.o["asym_bunch_x90"] = new TH1F();              // Bunch asymmetry histogram for x90
   errdet.o["asym_bunch_y45"] = new TH1F();              // Bunch asymmetry histogram for y45

   asymmetry.o["asym_vs_bunch_x45"] = new TH2F();//"asym_vs_bunch_x45", htitle, NBUNCH, -0.5, NBUNCH-0.5, 100, min, max);
   asymmetry.o["asym_vs_bunch_x90"] = new TH2F();
   asymmetry.o["asym_vs_bunch_y45"] = new TH2F();
   asymmetry.o["asym_sinphi_fit"] = new TH2F();
   asymmetry.o["scan_asym_sinphi_fit"] = new TH2F();

   d["Run"]       = run;
   d["Raw"]       = raw;
   d["FeedBack"]  = feedback;
   d["Kinema"]    = kinema;
   d["Bunch"]     = bunch;
   d["ErrDet"]    = errdet;
   d["Asymmetry"] = asymmetry;
};

/**
 *
 */
void CnipolHists::Print(const Option_t* opt) const
{
   opt = "";

   //printf("CnipolHists:\n");
}
