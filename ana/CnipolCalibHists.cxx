/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolCalibHists.h"

ClassImp(CnipolCalibHists)

using namespace std;

/** Default constructor. */
CnipolCalibHists::CnipolCalibHists() : DrawObjContainer()
{
   CnipolCalibHistsBook();
};



CnipolCalibHists::CnipolCalibHists(TDirectory *dir) : DrawObjContainer(dir)
{
   CnipolCalibHistsBook();
   //ReadFromDir();
};


/** Default destructor. */
CnipolCalibHists::~CnipolCalibHists()
{
};


void CnipolCalibHists::CnipolCalibHistsBook()
{
   char hName[256];
   //char hTitle[256];

   //sprintf(hName, "hEventSeq");
   //hEventSeq = new TH1F(hName, hName, 50000, 0, 35e6);

   fDir->Print();

   o["hAmpltd"]      = new TH1F("hAmpltd", "hAmpltd", 255, 0, 255);
   ((TH2F*) o["hAmpltd"])->GetXaxis()->SetTitle("Amplitude, ADC");
   ((TH2F*) o["hAmpltd"])->GetYaxis()->SetTitle("Events");
   // Amplitude with a cut applied on TDC and then loose cut on amplitude itself
   //o["hAmpltd_cut1"] = new TH1F("hAmpltd_cut1", "hAmpltd_cut1", 35, 165, 200);

   // The axis range will be/can be adjusted later based on the peak position
   o["hAmpltd_cut1"] = new TH1F("hAmpltd_cut1", "hAmpltd_cut1", 255, 0, 255);
   ((TH2F*) o["hAmpltd_cut1"])->GetXaxis()->SetTitle("Amplitude, ADC");
   ((TH2F*) o["hAmpltd_cut1"])->GetYaxis()->SetTitle("Events");

   o["hIntgrl"]      = new TH1F("hIntgrl", "hIntgrl", 255, 0, 255);
   ((TH2F*) o["hIntgrl"])->GetXaxis()->SetTitle("Integral, ADC");
   ((TH2F*) o["hIntgrl"])->GetYaxis()->SetTitle("Events");

   o["hIntgrl_cut1"] = new TH1F("hIntgrl_cut1", "hIntgrl_cut1", 255, 0, 255);
   ((TH2F*) o["hIntgrl_cut1"])->GetXaxis()->SetTitle("Integral, ADC");
   ((TH2F*) o["hIntgrl_cut1"])->GetYaxis()->SetTitle("Events");

   o["hTdc"]         = new TH1F("hTdc",    "hTdc",  80, 0, 80);
   ((TH2F*) o["hTdc"])->GetXaxis()->SetTitle("TDC");
   ((TH2F*) o["hTdc"])->GetYaxis()->SetTitle("Events");

   o["hTvsA"]        = new TH2F("hTvsA",   "hTvsA", 255, 0, 255, 80, 0, 80);
   ((TH2F*) o["hTvsA"])->SetOption("colz LOGZ");
   ((TH2F*) o["hTvsA"])->GetXaxis()->SetTitle("Amplitude, ADC");
   ((TH2F*) o["hTvsA"])->GetYaxis()->SetTitle("TDC");

   o["hTvsI"]        = new TH2F("hTvsI",   "hTvsI", 255, 0, 255, 80, 0, 80);
   ((TH2F*) o["hTvsI"])->SetOption("colz LOGZ");
   ((TH2F*) o["hTvsI"])->GetXaxis()->SetTitle("Integral, ADC");
   ((TH2F*) o["hTvsI"])->GetYaxis()->SetTitle("TDC");

   o["hIvsA"]        = new TH2F("hIvsA",   "hIvsA", 255, 0, 255, 255, 0, 255);
   ((TH2F*) o["hIvsA"])->SetOption("colz LOGZ");
   ((TH2F*) o["hIvsA"])->GetXaxis()->SetTitle("Amplitude, ADC");
   ((TH2F*) o["hIvsA"])->GetYaxis()->SetTitle("Integral, ADC");

   o["hACoef"]   = new TH1F("hACoef", "hACoef", 72, 1, 73);
   ((TH1F*) o["hACoef"])->SetOption("E");
   //((TH1F*) o["hACoef"])->GetYaxis()->SetRangeUser(5, 8);
   ((TH2F*) o["hACoef"])->GetXaxis()->SetTitle("Channel");
   ((TH2F*) o["hACoef"])->GetYaxis()->SetTitle("Amplitude Calib, keV/ADC");

   o["hACoefDisp"] = new TH1F("hACoefDisp", "hACoefDisp", 100, 0, 1);
   ((TH2F*) o["hACoefDisp"])->SetBit(TH1::kCanRebin);
   ((TH2F*) o["hACoefDisp"])->GetXaxis()->SetTitle("Amplitude Calib, keV/ADC");

   o["hICoef"]   = new TH1F("hICoef", "hICoef", 72, 1, 73);
   ((TH1F*) o["hICoef"])->SetOption("E");
   //((TH1F*) o["hICoef"])->GetYaxis()->SetRangeUser(6, 9);
   ((TH2F*) o["hICoef"])->GetXaxis()->SetTitle("Channel");
   ((TH2F*) o["hICoef"])->GetYaxis()->SetTitle("Integral Calib, keV/ADC");

   //o["hICoefDisp"] = new TH1F("hICoefDisp", "hICoefDisp", 40, 6, 9);
   o["hICoefDisp"] = new TH1F("hICoefDisp", "hICoefDisp", 100, 0, 1);
   ((TH2F*) o["hICoefDisp"])->SetBit(TH1::kCanRebin);
   ((TH2F*) o["hICoefDisp"])->GetXaxis()->SetTitle("Integral Calib, keV/ADC");

   o["hAmpltdW"]   = new TH1F("hAmpltdW", "hAmpltdW", 72, 1, 73);
   ((TH1F*) o["hAmpltdW"])->SetOption("E");
   //((TH1F*) o["hAmpltdW"])->GetYaxis()->SetRangeUser(0, 2);
   ((TH2F*) o["hAmpltdW"])->GetXaxis()->SetTitle("Channel");
   ((TH2F*) o["hAmpltdW"])->GetYaxis()->SetTitle("Alpha Peak Width, % (A)");

   o["hIntgrlW"]   = new TH1F("hIntgrlW", "hIntgrlW", 72, 1, 73);
   ((TH1F*) o["hIntgrlW"])->SetOption("E");
   //((TH1F*) o["hIntgrlW"])->GetYaxis()->SetRangeUser(0, 2);
   ((TH2F*) o["hIntgrlW"])->GetXaxis()->SetTitle("Channel");
   ((TH2F*) o["hIntgrlW"])->GetYaxis()->SetTitle("Alpha Peak Width, % (I)");

   char dName[256];

   //for (int i=0; i<TOT_WFD_CH; i++)
   for (int i=1; i<=NSTRIP; i++) {

      DrawObjContainer oc;

      //fDir->cd();
      sprintf(dName, "channel%02d", i);
      oc.fDir = new TDirectoryFile(dName, dName, "", fDir);
      //oc.fDir->Build((TFile*)fDir);
      //oc.fDir->cd();

      //sprintf(hName,"mass_feedback_st%d", i+1);
      //sprintf(hTitle,"%.3f : Invariant Mass for Strip-%d ", runinfo.RUNID, i+1);
      //sprintf(hTitle,"Invariant Mass for Strip-%d ", i+1);
      //feedback.o[hName] = new TH1F(hName, hTitle, 100, 0, 20);
      //( (TH1F*) feedback.o[hName]) -> GetXaxis() -> SetTitle("Mass [GeV/c^2]");
      //( (TH1F*) feedback.o[hName]) -> SetLineColor(2);

      sprintf(hName, "hAmpltd_st%02d", i);
      oc.o[hName] = new TH1F(hName, hName, 255, 0, 255);
      ((TH1F*) oc.o[hName])->GetXaxis()->SetTitle("Amplitude, ADC");

      sprintf(hName, "hAmpltd_cut1_st%02d", i);
      //oc.o[hName] = new TH1F(hName, hName, 35, 165, 200);
      oc.o[hName] = new TH1F(hName, hName, 255, 0, 255);
      ((TH1F*) oc.o[hName])->GetXaxis()->SetTitle("Amplitude, ADC");

      sprintf(hName, "hIntgrl_st%02d", i);
      oc.o[hName] = new TH1F(hName, hName, 255, 0, 255);
      ((TH1F*) oc.o[hName])->GetXaxis()->SetTitle("Integral, ADC");

      sprintf(hName, "hIntgrl_cut1_st%02d", i);
      //oc.o[hName] = new TH1F(hName, hName, 40, 115, 155);
      oc.o[hName] = new TH1F(hName, hName, 255, 0, 255);
      ((TH1F*) oc.o[hName])->GetXaxis()->SetTitle("Integral, ADC");

      sprintf(hName, "hTdc_st%02d", i);
      oc.o[hName] = new TH1F(hName, hName, 80, 0, 80);
      ((TH1F*) oc.o[hName])->GetXaxis()->SetTitle("TDC");

      sprintf(hName, "hTvsA_st%02d", i);
      oc.o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 0, 80);
      ((TH2F*) oc.o[hName])->SetOption("colz LOGZ");
      ((TH2F*) oc.o[hName])->GetXaxis()->SetTitle("Amplitude, ADC");
      ((TH2F*) oc.o[hName])->GetYaxis()->SetTitle("TDC");

      sprintf(hName, "hTvsA_zoom_st%02d", i);
      oc.o[hName] = new TH2F(hName, hName, 70, 130, 200, 30, 20, 50);
      ((TH2F*) oc.o[hName])->SetOption("colz LOGZ");
      ((TH2F*) oc.o[hName])->GetXaxis()->SetTitle("Amplitude, ADC");
      ((TH2F*) oc.o[hName])->GetYaxis()->SetTitle("TDC");

      sprintf(hName, "hTvsI_st%02d", i);
      oc.o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 0, 80);
      ((TH2F*) oc.o[hName])->SetOption("colz LOGZ");
      ((TH2F*) oc.o[hName])->GetXaxis()->SetTitle("Integral, ADC");
      ((TH2F*) oc.o[hName])->GetYaxis()->SetTitle("TDC");

      sprintf(hName, "hIvsA_st%02d", i);
      oc.o[hName] = new TH2F(hName, hName, 255, 0, 255, 255, 0, 255);
      ((TH2F*) oc.o[hName])->SetOption("colz LOGZ");
      ((TH2F*) oc.o[hName])->GetXaxis()->SetTitle("Amplitude, ADC");
      ((TH2F*) oc.o[hName])->GetYaxis()->SetTitle("Integral, ADC");

      //sprintf(hName,"t_vs_e_st%d", i);
      //kinema.oc.o[hName] = new TH2F();

      d[dName] = oc;
   }
};

/**
 *
 */
void CnipolCalibHists::Print(const Option_t* opt) const
{
   opt = "";

   //printf("CnipolCalibHists:\n");
}


/**
 *
 */
void CnipolCalibHists::SaveAllAs(TCanvas &c, string path)
{
   DrawObjContainer::SaveAllAs(c, path);

   return;
   
   if (gSystem->mkdir(path.c_str()) < 0)
      printf("WARNING: Perhaps dir already exists: %s\n", path.c_str());
   else
      printf("created dir: %s\n", path.c_str());

   ObjMapIter io;

   char cName[256];
   char fName[256];
   string option("");

   for (io=o.begin(); io!=o.end(); ++io) {

      if (!io->second) {
         printf("Object not found\n");
         continue;
      }

      string sname(io->second->GetName());
      //if (sname != "hAmpltdW" && sname != "hIntgrlW") continue;
      //if (sname.find("hTvsA_st") == string::npos) continue;
      //if (sname.find("hIntgrl_cut1_st") == string::npos &&
      //    sname.find("hIntgrlW") == string::npos ) continue;

      sprintf(cName, "c_%s", io->first.c_str());
      c.cd();
      c.SetName(cName);
      c.SetTitle(cName);

      if (((TClass*) io->second->IsA())->InheritsFrom("TH2")) {
         c.SetLogz(kTRUE);
         option = "colz";
         //if (sname.find("hTvsA_st") != string::npos) c.SetLogz(kFALSE);
      } else {
         c.SetLogz(kFALSE);
         option = "";
      }

      io->second->Draw(option.c_str());
      //io->second->Print();

      sprintf(fName, "%s/%s.png", path.c_str(), cName);
      c.SaveAs(fName);
   }

   DrawObjContainerMapIter isubd;

   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {
      string parentPath = path;
      path += "/" + isubd->first;
      //printf("cd to path: %s\n", path.c_str());
      isubd->second.SaveAllAs(c, path);
      path = parentPath;
   }

   // Special cases
   //sprintf(cName, "c_%s", io->first.c_str());
   //c.cd(); c.SetName(cName); c.SetTitle(cName);

   //io->second->Draw(option.c_str());
   //sprintf(fName, "%s/%s.png", path.c_str(), cName);
   //c.SaveAs(fName);
}
