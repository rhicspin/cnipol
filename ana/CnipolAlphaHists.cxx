/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolAlphaHists.h"
#include "ChannelEventId.h"

ClassImp(CnipolAlphaHists)

using namespace std;


/** Default constructor. */
CnipolAlphaHists::CnipolAlphaHists() : DrawObjContainer()
{
   BookHists();
}


/** */
CnipolAlphaHists::CnipolAlphaHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
}


/** Default destructor. */
CnipolAlphaHists::~CnipolAlphaHists()
{
}


void CnipolAlphaHists::BookHists(std::string cutid)
{ //{{{
   char hName[256];

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
   ((TH2F*) o["hICoef"])->GetXaxis()->SetTitle("Channel");
   ((TH2F*) o["hICoef"])->GetYaxis()->SetTitle("Integral Calib, keV/ADC");

   //o["hICoefDisp"] = new TH1F("hICoefDisp", "hICoefDisp", 40, 6, 9);
   o["hICoefDisp"] = new TH1F("hICoefDisp", "hICoefDisp", 100, 0, 1);
   ((TH2F*) o["hICoefDisp"])->SetBit(TH1::kCanRebin);
   ((TH2F*) o["hICoefDisp"])->GetXaxis()->SetTitle("Integral Calib, keV/ADC");

   o["hAmpltdW"]   = new TH1F("hAmpltdW", "hAmpltdW", 72, 1, 73);
   ((TH1F*) o["hAmpltdW"])->SetOption("E");
   ((TH2F*) o["hAmpltdW"])->GetXaxis()->SetTitle("Channel");
   ((TH2F*) o["hAmpltdW"])->GetYaxis()->SetTitle("Alpha Peak Width, % (A)");

   o["hIntgrlW"]   = new TH1F("hIntgrlW", "hIntgrlW", 72, 1, 73);
   ((TH1F*) o["hIntgrlW"])->SetOption("E");
   ((TH2F*) o["hIntgrlW"])->GetXaxis()->SetTitle("Channel");
   ((TH2F*) o["hIntgrlW"])->GetYaxis()->SetTitle("Alpha Peak Width, % (I)");

   o["Detector1_Events"]   = new TH1F("Detector1_Events", "Detector1_Events", 12, 1, 12);
   ((TH1F*) o["Detector1_Events"])->SetOption("E");
   ((TH2F*) o["Detector1_Events"])->GetXaxis()->SetTitle("Channel");
   ((TH2F*) o["Detector1_Events"])->GetYaxis()->SetTitle("# of Events");

   o["Detector2_Events"]   = new TH1F("Detector2_Events", "Detector2_Events", 12, 13, 24);
   ((TH1F*) o["Detector2_Events"])->SetOption("E");
   ((TH2F*) o["Detector2_Events"])->GetXaxis()->SetTitle("Channel");
   ((TH2F*) o["Detector2_Events"])->GetYaxis()->SetTitle("# of Events");

   o["Detector3_Events"]   = new TH1F("Detector3_Events", "Detector3_Events", 12, 25, 36);
   ((TH1F*) o["Detector3_Events"])->SetOption("E");
   ((TH2F*) o["Detector3_Events"])->GetXaxis()->SetTitle("Channel");
   ((TH2F*) o["Detector3_Events"])->GetYaxis()->SetTitle("# of Events");

   o["Detector4_Events"]   = new TH1F("Detector4_Events", "Detector4_Events", 12, 37, 48);
   ((TH1F*) o["Detector4_Events"])->SetOption("E");
   ((TH2F*) o["Detector4_Events"])->GetXaxis()->SetTitle("Channel");
   ((TH2F*) o["Detector4_Events"])->GetYaxis()->SetTitle("# of Events");

   o["Detector5_Events"]   = new TH1F("Detector5_Events", "Detector5_Events", 12, 49, 60);
   ((TH1F*) o["Detector5_Events"])->SetOption("E");
   ((TH2F*) o["Detector5_Events"])->GetXaxis()->SetTitle("Channel");
   ((TH2F*) o["Detector5_Events"])->GetYaxis()->SetTitle("# of Events");

   o["Detector6_Events"]   = new TH1F("Detector6_Events", "Detector6_Events", 12, 61, 72);
   ((TH1F*) o["Detector6_Events"])->SetOption("E");
   ((TH2F*) o["Detector6_Events"])->GetXaxis()->SetTitle("Channel");
   ((TH2F*) o["Detector6_Events"])->GetYaxis()->SetTitle("# of Events");

    o["Detector1_Events_Dsp"]   = new TH1F("Detector1_Events_Dsp", "Detector1_Events_Dsp", 20000, 0, 20000);
   ((TH1F*) o["Detector1_Events"])->SetOption("E");
   ((TH2F*) o["Detector1_Events"])->GetXaxis()->SetTitle("Events");
   ((TH2F*) o["Detector1_Events"])->GetYaxis()->SetTitle("#");

   o["Detector2_Events_Dsp"]   = new TH1F("Detector2_Events_Dsp", "Detector2_Events_Dsp", 20000, 0, 20000);
   ((TH1F*) o["Detector2_Events"])->SetOption("E");
   ((TH2F*) o["Detector2_Events"])->GetXaxis()->SetTitle("Events");
   ((TH2F*) o["Detector2_Events"])->GetYaxis()->SetTitle("#");

   o["Detector3_Events_Dsp"]   = new TH1F("Detector3_Events_Dsp", "Detector3_Events_Dsp", 20000, 0, 20000);
   ((TH1F*) o["Detector3_Events"])->SetOption("E");
   ((TH2F*) o["Detector3_Events"])->GetXaxis()->SetTitle("Events");
   ((TH2F*) o["Detector3_Events"])->GetYaxis()->SetTitle("#");

   o["Detector4_Events_Dsp"]   = new TH1F("Detector4_Events_Dsp", "Detector4_Events_Dsp", 20000, 0, 20000);
   ((TH1F*) o["Detector4_Events"])->SetOption("E");
   ((TH2F*) o["Detector4_Events"])->GetXaxis()->SetTitle("Events");
   ((TH2F*) o["Detector4_Events"])->GetYaxis()->SetTitle("#");

   o["Detector5_Events_Dsp"]   = new TH1F("Detector5_Events_Dsp", "Detector5_Events_Dsp", 20000, 0, 20000);
   ((TH1F*) o["Detector5_Events"])->SetOption("E");
   ((TH2F*) o["Detector5_Events"])->GetXaxis()->SetTitle("Events");
   ((TH2F*) o["Detector5_Events"])->GetYaxis()->SetTitle("#");

   o["Detector6_Events_Dsp"]   = new TH1F("Detector6_Events_Dsp", "Detector6_Events_Dsp", 20000, 0, 20000);
   ((TH1F*) o["Detector6_Events"])->SetOption("E");
   ((TH2F*) o["Detector6_Events"])->GetXaxis()->SetTitle("Events");
   ((TH2F*) o["Detector6_Events"])->GetYaxis()->SetTitle("#");

   char dName[256];
   //string sSi("  ");
   DrawObjContainer *oc;
   DrawObjContainerMapIter isubdir;

   for (int i=0; i<TOT_WFD_CH; i++) {

      //sprintf(&sSi[0], "%02d", i+1);

      //fDir->cd();
      sprintf(dName, "channel%02d", i);

      isubdir = d.find(dName);

      if ( isubdir == d.end()) { // if dir not found
         // Delete existing directory in memory
         //TKey *key = fDir->GetKey(dName);
         //if (key) key->Delete("v");
            //if (((TClass*) io->second->IsA())->InheritsFrom("TH1"))

         oc = new DrawObjContainer();
         oc->fDir = new TDirectoryFile(dName, dName, "", fDir);
      } else {
         oc = isubdir->second;
      }

      //sprintf(hName,"mass_feedback_st%d", i+1);
      //sprintf(hTitle,"%.3f : Invariant Mass for Strip-%d ", gMeasInfo->RUNID, i+1);
      //sprintf(hTitle,"Invariant Mass for Strip-%d ", i+1);
      //feedback.o[hName] = new TH1F(hName, hTitle, 100, 0, 20);
      //( (TH1F*) feedback.o[hName]) -> GetXaxis() -> SetTitle("Mass [GeV/c^2]");
      //( (TH1F*) feedback.o[hName]) -> SetLineColor(2);

      sprintf(hName, "hAmpltd_st%02d", i);
      oc->o[hName] = new TH1F(hName, hName, 255, 0, 255);
      ((TH1F*) oc->o[hName])->GetXaxis()->SetTitle("Amplitude, ADC");

      sprintf(hName, "hAmpltd_cut1_st%02d", i);
      //oc->o[hName] = new TH1F(hName, hName, 35, 165, 200);
      oc->o[hName] = new TH1F(hName, hName, 255, 0, 255);
      ((TH1F*) oc->o[hName])->GetXaxis()->SetTitle("Amplitude, ADC");

      sprintf(hName, "hIntgrl_st%02d", i);
      oc->o[hName] = new TH1F(hName, hName, 255, 0, 255);
      ((TH1F*) oc->o[hName])->GetXaxis()->SetTitle("Integral, ADC");

      sprintf(hName, "hIntgrl_cut1_st%02d", i);
      //oc->o[hName] = new TH1F(hName, hName, 40, 115, 155);
      oc->o[hName] = new TH1F(hName, hName, 255, 0, 255);
      ((TH1F*) oc->o[hName])->GetXaxis()->SetTitle("Integral, ADC");

      sprintf(hName, "hTdc_st%02d", i);
      oc->o[hName] = new TH1F(hName, hName, 80, 0, 80);
      ((TH1F*) oc->o[hName])->GetXaxis()->SetTitle("TDC");

      sprintf(hName, "hTvsA_st%02d", i);
      oc->o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 0, 80);
      ((TH2F*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH2F*) oc->o[hName])->GetXaxis()->SetTitle("Amplitude, ADC");
      ((TH2F*) oc->o[hName])->GetYaxis()->SetTitle("TDC");

      sprintf(hName, "hTvsA_zoom_st%02d", i);
      oc->o[hName] = new TH2F(hName, hName, 70, 130, 200, 30, 20, 50);
      ((TH2F*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH2F*) oc->o[hName])->GetXaxis()->SetTitle("Amplitude, ADC");
      ((TH2F*) oc->o[hName])->GetYaxis()->SetTitle("TDC");

      sprintf(hName, "hTvsI_st%02d", i);
      oc->o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 0, 80);
      ((TH2F*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH2F*) oc->o[hName])->GetXaxis()->SetTitle("Integral, ADC");
      ((TH2F*) oc->o[hName])->GetYaxis()->SetTitle("TDC");

      sprintf(hName, "hIvsA_st%02d", i);
      oc->o[hName] = new TH2F(hName, hName, 255, 0, 255, 255, 0, 255);
      ((TH2F*) oc->o[hName])->SetOption("colz LOGZ");
      ((TH2F*) oc->o[hName])->GetXaxis()->SetTitle("Amplitude, ADC");
      ((TH2F*) oc->o[hName])->GetYaxis()->SetTitle("Integral, ADC");

     

      //sprintf(hName,"t_vs_e_st%d", i);
      //kinema.oc->o[hName] = new TH2F();

      // If this is a new directory then we need to add it to the list
      if ( isubdir == d.end()) {
         d[dName] = oc;
      }
   }
} //}}}


/** */
void CnipolAlphaHists::Fill(ChannelEvent *ch, string sid)
{ //{{{
   UChar_t      chId = ch->GetChannelId();
   ChannelData &data = ch->fChannel;

   string sSi("  ");
   sprintf(&sSi[0], "%02d", chId);

   DrawObjContainer *sd = d["channel"+sSi];

   if (sid == "cut1") {
   //if (data.fTdc >=15 && data.fTdc <= 50 && data.fAmpltd >= 130 && data.fAmpltd <= 210)
   //if (data.fTdc >=15 && data.fTdc <= 50) {
      ((TH1F*) o["hAmpltd_cut1"])->Fill(data.fAmpltd);
      ((TH1F*) sd->o["hAmpltd_cut1_st"+sSi])->Fill(data.fAmpltd);
   //}

   //if (data.fTdc >=12 && data.fTdc <= 16 && data.fIntgrl >= 100)
   //if (data.fTdc >=12 && data.fTdc <= 16) 
   //if (data.fTdc >=12 && data.fTdc <= 30) {
      ((TH1F*) o["hIntgrl_cut1"])->Fill(data.fIntgrl);
      ((TH1F*) sd->o["hIntgrl_cut1_st"+sSi])->Fill(data.fIntgrl);
   //}
   } else {

      ((TH1F*) o["hAmpltd"])->Fill(data.fAmpltd);
      ((TH1F*) o["hIntgrl"])->Fill(data.fIntgrl);
      ((TH1F*) o["hTdc"])   ->Fill(data.fTdc);
      ((TH2F*) o["hTvsA"])  ->Fill(data.fAmpltd, data.fTdc);
      ((TH2F*) o["hTvsI"])  ->Fill(data.fIntgrl, data.fTdc);
      ((TH2F*) o["hIvsA"])  ->Fill(data.fAmpltd, data.fIntgrl);

      ((TH1F*) sd->o["hAmpltd_st"+sSi])   ->Fill(data.fAmpltd);
      ((TH1F*) sd->o["hIntgrl_st"+sSi])   ->Fill(data.fIntgrl);
      ((TH1F*) sd->o["hTdc_st"+sSi])      ->Fill(data.fTdc);
      ((TH2F*) sd->o["hTvsA_st"+sSi])     ->Fill(data.fAmpltd, data.fTdc);
      ((TH2F*) sd->o["hTvsA_zoom_st"+sSi])->Fill(data.fAmpltd, data.fTdc);
      ((TH2F*) sd->o["hTvsI_st"+sSi])     ->Fill(data.fIntgrl, data.fTdc);
      ((TH2F*) sd->o["hIvsA_st"+sSi])     ->Fill(data.fAmpltd, data.fIntgrl);
   }
} //}}}


/** */
void CnipolAlphaHists::FillPreProcess(ChannelEvent *ch) { }


/** */
void CnipolAlphaHists::PostFill()
{ //{{{
   // Adjust axis ranges
   Int_t  maxBinA = ((TH1F*) o["hAmpltd_cut1"])->GetMaximumBin();
   Double_t xminA = ((TH1F*) o["hAmpltd_cut1"])->GetXaxis()->GetXmin();
   Double_t xmaxA = ((TH1F*) o["hAmpltd_cut1"])->GetXaxis()->GetXmax();
  

   xminA = maxBinA - 50 < xminA ? xminA : maxBinA - 50;
   xmaxA = maxBinA + 50 > xmaxA ? xmaxA : maxBinA + 50;

   ((TH1F*) o["hAmpltd_cut1"])->SetAxisRange(xminA, xmaxA);

   Int_t  maxBinI = ((TH1F*) o["hIntgrl_cut1"])->GetMaximumBin();
   Double_t xminI = ((TH1F*) o["hIntgrl_cut1"])->GetXaxis()->GetXmin();
   Double_t xmaxI = ((TH1F*) o["hIntgrl_cut1"])->GetXaxis()->GetXmax();
   Int_t tmaxBinI=0;

   Int_t maxBin=0;

   xminI = maxBinI - 50 < xminI ? xminI : maxBinI - 50;
   xmaxI = maxBinI + 50 > xmaxI ? xmaxI : maxBinI + 50;

   ((TH1F*) o["hIntgrl_cut1"])->SetAxisRange(xminI, xmaxI);

   printf("xminA, xmaxA, xminI, xmaxI: %f, %f, %f, %f\n", xminA, xmaxA, xminI, xmaxI);

   string  sSi("  ");

   for (int i=1; i<=NSTRIP; i++) {
      sprintf(&sSi[0], "%02d", i);
      

     
      maxBinA = ((TH1F*) d["channel"+sSi]->o["hAmpltd_st"+sSi])->GetMaximumBin();
      maxBinI - ((TH1F*) d["channel"+sSi]->o["hIntgrl_st"+sSi])->GetMaximumBin();
      if(i==9||i==10){
       for(int j=0;j<=255;j++){
	
	 tmaxBinI=((TH1F*) d["channel"+sSi]->o["hIntgrl_st"+sSi])->GetBinContent(j); 
	 if(tmaxBinI>=maxBinI)maxBinI=tmaxBinI;
       }
      }
	//xmin   = ((TH1F*) d["channel"+sSi].o["hAmpltd_cut1_st"+sSi])->GetXaxis()->GetXmin();
      //xmax   = ((TH1F*) d["channel"+sSi].o["hAmpltd_cut1_st"+sSi])->GetXaxis()->GetXmax();
      //xmin   = maxBin - 50 < xmin ? xmin : maxBin - 50;
      //xmax   = maxBin + 50 > xmax  ? xmax : maxBin + 50;
      xminA=maxBinA-50;
      xmaxA=maxBinA+50;
      xminI=maxBinI-50;
      xmaxI=maxBinI+50;
    
     
      ((TH1F*) d["channel"+sSi]->o["hAmpltd_cut1_st"+sSi])->SetAxisRange(xminA, xmaxA);
      


      ((TH1F*) d["channel"+sSi]->o["hIntgrl_cut1_st"+sSi])->SetAxisRange(xminI, xmaxI);
   }
 
} //}}}


/** */
//void CnipolAlphaHists::SaveAllAs(TCanvas &c, std::string pattern, string path)
//{ //{{{
//   DrawObjContainer::SaveAllAs(c, pattern, path);
//
//   return;
//   
//   if (gSystem->mkdir(path.c_str()) < 0)
//      printf("WARNING: Perhaps dir already exists: %s\n", path.c_str());
//   else
//      printf("created dir: %s\n", path.c_str());
//
//   ObjMapIter io;
//
//   char cName[256];
//   char fName[256];
//   string option("");
//
//   for (io=o.begin(); io!=o.end(); ++io) {
//
//      if (!io->second) {
//         printf("Object not found\n");
//         continue;
//      }
//
//      string sname(io->second->GetName());
//      //if (sname != "hAmpltdW" && sname != "hIntgrlW") continue;
//      //if (sname.find("hTvsA_st") == string::npos) continue;
//      //if (sname.find("hIntgrl_cut1_st") == string::npos &&
//      //    sname.find("hIntgrlW") == string::npos ) continue;
//
//      sprintf(cName, "c_%s", io->first.c_str());
//      c.cd();
//      c.SetName(cName);
//      c.SetTitle(cName);
//
//      if (((TClass*) io->second->IsA())->InheritsFrom("TH2")) {
//         c.SetLogz(kTRUE);
//         option = "colz";
//         //if (sname.find("hTvsA_st") != string::npos) c.SetLogz(kFALSE);
//      } else {
//         c.SetLogz(kFALSE);
//         option = "";
//      }
//
//      io->second->Draw(option.c_str());
//      //io->second->Print();
//
//      sprintf(fName, "%s/%s.png", path.c_str(), cName);
//      c.SaveAs(fName);
//   }
//
//   DrawObjContainerMapIter isubd;
//
//   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {
//      string parentPath = path;
//      path += "/" + isubd->first;
//      //printf("cd to path: %s\n", path.c_str());
//      isubd->second->SaveAllAs(c, pattern, path);
//      path = parentPath;
//   }
//
//   // Special cases
//   //sprintf(cName, "c_%s", io->first.c_str());
//   //c.cd(); c.SetName(cName); c.SetTitle(cName);
//
//   //io->second->Draw(option.c_str());
//   //sprintf(fName, "%s/%s.png", path.c_str(), cName);
//   //c.SaveAs(fName);
//} //}}}
