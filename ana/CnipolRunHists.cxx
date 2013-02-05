/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolRunHists.h"

#include "TMath.h"

#include "AsymGlobals.h"
#include "MeasInfo.h"


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
void CnipolRunHists::BookHists()
{
   TH1* hist;

   fDir->cd();

   //TDirectory *dir;
   //dir = new TDirectoryFile("feedback", "feedback", "", fDir);
   //dir = Feedback;
   //DrawObjContainer *feedback = new DrawObjContainer(dir);

   //DrawObjContainer *kinema;
   //DrawObjContainer *errdet;

   //Asymmetry->SetMother(fDir);
   //TDirectory *dir = new TDirectoryFile("Asymmetry", "Asymmetry", "", fDir);
   //DrawObjContainer *asymmetry = new DrawObjContainer((TDirectory*) Asymmetry);
   //DrawObjContainer *asymmetry = new DrawObjContainer(dir);

   //feedback->fDir  = fDir;
   ////kinema->fDir    = fDir;
   //kinema->fDir = new TDirectoryFile("Kinema2", "Kinema2", "", fDir);
   ////kinema->fDir    = Kinema;
   //errdet->fDir    = fDir;


   //feedback->o["mdev_feedback"] = new TH2F();

   //kinema->o["energy_spectrum_all"] = new TH1F();

   char hName[256];
   //char hTitle[256];

   // energy spectrum for all detector sum
   //sprintf(hName, "energy_spectrum_all");
   //sprintf(hTitle, "%.3f : Energy Spectrum (All Detectors)", gMeasInfo->runName.c_str());
   //kinema->o[hName] = new TH1F(hName, hTitle, 100, 0, 0.03);
   //((TH1F*) kinema->o[hName])->GetXaxis()->SetTitle("Momentum Transfer [-GeV/c]^2");

   //for (int i=1; i<=TOT_WFD_CH; i++) {

   //   sprintf(hName,"mass_feedback_st%d", i);
   //   //sprintf(hTitle,"%.3f : Invariant Mass for Strip-%d ", gMeasInfo->RUNID, i+1);
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

   //for (int i=1; i<=N_DETECTORS; i++) {
   //   sprintf(hName,"energy_spectrum_det%d", i);
   //   // energy spectrum per detector
   //   kinema->o[hName] = new TH1F(hName, hName, 100, 0, 0.03);
   //   ((TH1F*) kinema->o[hName])->GetXaxis()->SetTitle("Momentum Transfer [-GeV/c]^2");
   //}

   //TF1  * banana_cut_l[NSTRIP][2];     // banana cut low
   //TF1  * banana_cut_h[NSTRIP][2];     // banana cut high
   //TLine  * energy_cut_l[NSTRIP];      // energy cut low
   //TLine  * energy_cut_h[NSTRIP];      // energy cut high
 
   sprintf(hName, "wall_current_monitor");
   o[hName] = new TH1F(hName, hName, N_BUNCHES, 1, N_BUNCHES+1);
   ((TH1*) o[hName])->SetTitle("; Bunch Id; Number of Protons, x10^{9};");
   ((TH1*) o[hName])->SetFillColor(kGray);
 
   sprintf(hName, "specific_luminosity");
   o[hName] = new TH1F(hName, hName, N_BUNCHES, 1, N_BUNCHES+1);
   ((TH1*) o[hName])->SetTitle("; Bunch Id; Number of Protons, x10^{9};");
   ((TH1*) o[hName])->SetFillColor(kGray);

   //errdet->o["mass_chi2_vs_strip"]       = new TH2F();    // Chi2 of Gaussian Fit on Mass peak
   //errdet->o["mass_sigma_vs_strip"]      = new TH2F();    // Mass sigma width vs. strip
   //errdet->o["mass_e_correlation_strip"] = new TH2F();    // Mass-energy correlation vs-> strip
   //errdet->o["mass_pos_dev_vs_strip"]    = new TH2F();    // Mass position deviation vs. strip
   //errdet->o["good_carbon_events_strip"] = new TH1I();    // number of good carbon events per strip
   //errdet->o["spelumi_vs_bunch"]         = new TH2F();    // Counting rate vs. bunch
   //errdet->o["bunch_spelumi"]            = new TH1F();    // Counting rate per bunch hisogram
   //errdet->o["asym_bunch_x45"]           = new TH1F();    // Bunch asymmetry histogram for x45
   //errdet->o["asym_bunch_x90"]           = new TH1F();    // Bunch asymmetry histogram for x90
   //errdet->o["asym_bunch_y45"]           = new TH1F();    // Bunch asymmetry histogram for y45

   //asymmetry->o["asym_vs_bunch_x90"]    = asym_vs_bunch_x90;
   //asymmetry->o["asym_vs_bunch_y45"]    = asym_vs_bunch_y45;
   //o["asym_sinphi_fit"]      = new 

   hist = new TH2F("asym_sinphi_fit", "asym_sinphi_fit", 1, 0, 2*M_PI, 1, -1, 1);
   o["asym_sinphi_fit"] = hist;
   asym_sinphi_fit = (TH2F*) hist;

   hist = new TH2F("scan_asym_sinphi_fit", "scan_asym_sinphi_fit", 1, 0, 2*M_PI, 1, -1, 1);
   o["scan_asym_sinphi_fit"] = hist;
   scan_asym_sinphi_fit = (TH2F*) hist;

   sprintf(hName,  "asym_vs_bunch_x45");
   o[hName] = new TH2F(hName, hName, NBUNCH, 0, NBUNCH, 1, 0, 1);
   ((TH1*) o[hName])->SetTitle("; Raw Asymmetry X45; Counts weighted by error;");

   //sprintf(hName,  "asym_sinphi_fit");
   //sprintf(hTitle, "asym_sinphi_fit");
   //o[hName] = new TH2F(hName, hTitle, 1, 0, 1, 1, 0, 1);

   //d["FeedBack"]  = feedback;
   //d["Kinema2"]   = kinema;
   //d["ErrDet"]    = errdet;
   //d["Asymmetry"] = asymmetry;

}


/** */
void CnipolRunHists::Fill(ChannelEvent *ch)
{
   //UChar_t chId  = ch->GetChannelId();
   //UChar_t detId = ch->GetDetectorId();
   //UShort_t delim = ch->GetDelimiterId();
}


/** */
void CnipolRunHists::PostFill()
{
   float min, max;
   float margin=0.2;
   float prefix=0.028;

   //DrawHorizLine(asym_vs_bunch_x90, -0.5, NBUNCH-0.5, 0, 1, 1, 1);

   GetMinMaxOption(prefix, NBUNCH, gBunchAsym.Ax45[0], margin, min, max);
   ((TH1*) o["asym_vs_bunch_x45"])->SetBins(NBUNCH, -0.5, NBUNCH-0.5, 100, min, max);
   DrawHorizLine( (TH1*) o["asym_vs_bunch_x45"], -0.5, NBUNCH-0.5, 0, 1, 1, 1);
}


/** */
void CnipolRunHists::Fill(MeasInfo &ri)
{
   map<UShort_t, Float_t>::iterator ib;

   for (ib=ri.fWallCurMon.begin(); ib!=ri.fWallCurMon.end(); ++ib)
   {
      ((TH1*) o["wall_current_monitor"])->SetBinContent(ib->first, ib->second);
   }
}
