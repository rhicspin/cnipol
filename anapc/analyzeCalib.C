
#include "analyzeCalib.h"

using namespace std;


void analyzeCalib(string runName, Long64_t nEvents)
{
   analyzeCalib_initialize(runName);
   //analyzeCalib_book_histograms();
   //analyzeCalib_fill_histograms(nEvents);
   //analyzeCalib_fit_histograms();
   //analyzeCalib_restore_channels();
   //analyzeCalib_fill_histograms_derivative();
   analyzeCalib_finalize();
}


void analyzeCalib_initialize(string runName)
{
   gStyle->SetPalette(1);
   gStyle->SetOptFit(1111);
   gStyle->SetOptStat("emroui");
   gStyle->SetStatX(0.98);
   gStyle->SetStatY(0.98);
   gStyle->SetStatW(0.17);
   gStyle->SetStatH(0.12);

   gOutFile = new TFile("./out.root", "recreate");
   //gH = new CnipolCalibHists(gOutFile);
   //gH->fDir = gOutFile;

   gChain = new TChain("AnaEventTree");

   const char* outDir = getenv("CNIPOL_RESULTS_DIR");

   gRunName = runName;
   gOutDir.reserve(255);
   sprintf(&gOutDir[0], "%s/%s", outDir, gRunName.c_str());

   string fileName("", 255);
   sprintf(&fileName[0], "%s/%s_tree_00.root", gOutDir.c_str(), gRunName.c_str());

   gChain->AddFile(fileName.c_str());
   gChain->SetBranchAddress("AnaEvent", &gAnaEvent);

   sprintf(&fileName[0], "%s/%s.root", gOutDir.c_str(), gRunName.c_str());

   gInFile = new TFile(fileName.c_str(), "READ");

   if (!gInFile) exit(-1);

   gH = new CnipolCalibHists();
   gH->ReadFromDir(gInFile);

   ec = (EventConfig*) gInFile->FindObjectAny("EventConfig");
   //ec->Print();
   //ec->fCalibrator->fChannelCalibs.clear();
}


void analyzeCalib_fill_histograms(Long64_t nEvents)
{ //{{{
   gRandom = new TRandom();

   Int_t maxEvents = gChain->GetEntries();

   if (nEvents < 0 || nEvents > maxEvents) nEvents = maxEvents;

   Long64_t nbytes = 0;

   for (Long64_t i=0; i<nEvents; i++) {

      nbytes += gChain->GetEntry(i);

      //printf("num. of events: %lld\n", nEvents);

      //int nChannels = gAnaEvent->fChannels.size();
      //printf("num. of channels: %d\n", nChannels);

      //gAnaEvent->Print();

      ChannelDataMap::iterator mi;
      ChannelDataMap::iterator mb = gAnaEvent->fChannels.begin();
      ChannelDataMap::iterator me = gAnaEvent->fChannels.end();

      string  sSi("  ");

      for (mi=mb; mi!=me; mi++) {

         //if (mi->first+1 != 54) continue;

         //float energy  = gAnaEvent->GetEnergy(ec, *mi);
         //float energyQ = gAnaEvent->GetEnergyIntegral(ec, *mi);
         //float time    = gAnaEvent->GetTime(ec, *mi);

         float amplitude  = mi->second.fAmpltd;
         float integral   = mi->second.fIntgrl;
         float tdc        = mi->second.fTdc;
         //float stime      = gAnaEvent->fEventId.fRevolutionId + gAnaEvent->fEventId.fBunchId;

         ((TH1F*) gH->o["hAmpltd"])->Fill(amplitude);
         ((TH1F*) gH->o["hIntgrl"])->Fill(integral);
         ((TH1F*) gH->o["hTdc"])->Fill(tdc);
         ((TH2F*) gH->o["hTvsA"])->Fill(amplitude, tdc);
         ((TH2F*) gH->o["hTvsI"])->Fill(integral, tdc);
         ((TH2F*) gH->o["hIvsA"])->Fill(amplitude, integral);

         // This is dangerous but we know what we are doing
         sprintf(&sSi[0], "%02d", mi->first+1);

         ((TH1F*) gH->d["channel"+sSi].o["hAmpltd_st"+sSi])->Fill(amplitude);
         ((TH1F*) gH->d["channel"+sSi].o["hIntgrl_st"+sSi])->Fill(integral);
         ((TH1F*) gH->d["channel"+sSi].o["hTdc_st"+sSi])->Fill(tdc);
         ((TH2F*) gH->d["channel"+sSi].o["hTvsA_st"+sSi])->Fill(amplitude, tdc);
         ((TH2F*) gH->d["channel"+sSi].o["hTvsA_zoom_st"+sSi])->Fill(amplitude, tdc);
         ((TH2F*) gH->d["channel"+sSi].o["hTvsI_st"+sSi])->Fill(integral, tdc);
         ((TH2F*) gH->d["channel"+sSi].o["hIvsA_st"+sSi])->Fill(amplitude, integral);

         //if (tdc >=15 && tdc <= 50 && amplitude >= 130 && amplitude <= 210)
         if (tdc >=15 && tdc <= 50) {
            ((TH1F*) gH->o["hAmpltd_cut1"])->Fill(amplitude);
            ((TH1F*) gH->d["channel"+sSi].o["hAmpltd_cut1_st"+sSi])->Fill(amplitude);
         }

         //if (tdc >=12 && tdc <= 16 && integral >= 100)
         //if (tdc >=12 && tdc <= 16) 
         if (tdc >=12 && tdc <= 30) {
            ((TH1F*) gH->o["hIntgrl_cut1"])->Fill(integral);
            ((TH1F*) gH->d["channel"+sSi].o["hIntgrl_cut1_st"+sSi])->Fill(integral);
         }

         //hEventSeq->Fill(stime);
         //printf("e: %f\n", energy);
         //hAmpltd->Fill(energy);
         //hIntgrl->Fill(energyQ);
         //hTdc->Fill(time);

         //hTvsE->Fill(energy, time);
         //hTvsQ->Fill(energyQ, time);

         //hEvsQ->Fill(energy, energyQ);
      }
   }

   // Adjust axis ranges
   Int_t  maxBinA = ((TH1F*) gH->o["hAmpltd_cut1"])->GetMaximumBin();
   Double_t xminA = ((TH1F*) gH->o["hAmpltd_cut1"])->GetXaxis()->GetXmin();
   Double_t xmaxA = ((TH1F*) gH->o["hAmpltd_cut1"])->GetXaxis()->GetXmax();
   xminA = maxBinA - 50 < xminA ? xminA : maxBinA - 50;
   xmaxA = maxBinA + 50 > xmaxA ? xmaxA : maxBinA + 50;

   ((TH1F*) gH->o["hAmpltd_cut1"])->SetAxisRange(xminA, xmaxA);

   Int_t  maxBinI  = ((TH1F*) gH->o["hIntgrl_cut1"])->GetMaximumBin();
   Double_t xminI = ((TH1F*) gH->o["hIntgrl_cut1"])->GetXaxis()->GetXmin();
   Double_t xmaxI = ((TH1F*) gH->o["hIntgrl_cut1"])->GetXaxis()->GetXmax();
   xminI = maxBinI - 50 < xminI ? xminI : maxBinI - 50;
   xmaxI = maxBinI + 50 > xmaxI ? xmaxI : maxBinI + 50;

   ((TH1F*) gH->o["hIntgrl_cut1"])->SetAxisRange(xminI, xmaxI);

   string  sSi("  ");

   for (int i=1; i<=NSTRIP; i++) {
      sprintf(&sSi[0], "%02d", i);

      //maxBin  = ((TH1F*) gH->d["channel"+sSi].o["hAmpltd_cut1_st"+sSi])->GetMaximumBin();
      //xmin = ((TH1F*) gH->d["channel"+sSi].o["hAmpltd_cut1_st"+sSi])->GetXaxis()->GetXmin();
      //xmax = ((TH1F*) gH->d["channel"+sSi].o["hAmpltd_cut1_st"+sSi])->GetXaxis()->GetXmax();
      //xmin = maxBin - 50 < xmin ? xmin : maxBin - 50;
      //xmax = maxBin + 50 > xmax  ? xmax : maxBin + 50;

      ((TH1F*) gH->d["channel"+sSi].o["hAmpltd_cut1_st"+sSi])->SetAxisRange(xminA, xmaxA);
      ((TH1F*) gH->d["channel"+sSi].o["hIntgrl_cut1_st"+sSi])->SetAxisRange(xminI, xmaxI);
   }
}
// }}}


void analyzeCalib_fit_histograms()
{
   string    sSi("  ");
   //float     chi2_max  = 0;
   Double_t  stat_max  = 0;
   TH1F     *hAmp      = 0;
   TH1F     *hInt      = 0;
   TF1      *fitfunc   = new TF1("fitfunc", "gaus");
   TFitResultPtr frp;

   fitfunc->SetLineColor(2);
   fitfunc->SetLineWidth(3);

   for (int i=1; i<=NSTRIP; i++) {
   //for (int i=33; i<34; i++)

      sprintf(&sSi[0], "%02d", i);

      // Amplitude
      // Just for shorthand
      hAmp = (TH1F*) gH->d["channel"+sSi].o["hAmpltd_cut1_st"+sSi];
      frp = analyzeCalib_fit_histograms(hAmp, fitfunc);

      int   ndf  = fitfunc->GetNDF();
      float amp  = fitfunc->GetParameter(1);
      float eamp = fitfunc->GetParError(1);
      float chi2 = fitfunc->GetChisquare();
      float cal, ecal;

      if (ndf <= 0) {
         ndf = -1;
         amp = eamp = cal = ecal = chi2 = 0;
      } else {
         cal  = (ALPHA_KEV/amp) * ATTEN;
         //ecal = cal * eamp/amp;
         ecal = cal * eamp;
      }

      float aCoef    = -2;
      float aCoefErr = -2;
      float aChi2    = -2;

      if (frp) {
         //frp->Print();
         aCoef    = (ALPHA_KEV/fitfunc->GetParameter(1)) * ATTEN;
         aCoefErr = aCoef * fitfunc->GetParameter(2)/fitfunc->GetParameter(1);
         aChi2    = fitfunc->GetChisquare();
      }

      //if (frp)
         //((TH1F*) gH->o["hAmpltdW"])->SetBinContent(i, frp->Value(2));
      ((TH1F*) gH->o["hAmpltdW"])->SetBinContent(i, 100*fitfunc->GetParameter(2)/fitfunc->GetParameter(1));
      ((TH1F*) gH->o["hAmpltdW"])->SetBinError(i, 100*fitfunc->GetParError(2)/(fitfunc->GetParameter(1)));

      ((TH1F*) gH->o["hACoef"])->SetBinContent(i, aCoef);
      ((TH1F*) gH->o["hACoef"])->SetBinError(i, aCoefErr);

      ((TH1F*) gH->o["hACoefDisp"])->Fill(aCoef);

      // Integral
      hInt = (TH1F*) gH->d["channel"+sSi].o["hIntgrl_cut1_st"+sSi];
      frp = analyzeCalib_fit_histograms(hInt, fitfunc);

            ndf      = 0;
      float iCoef    = -2;
      float iCoefErr = -2;
      float iChi2    = -2;

      if (frp) {
         //frp->Print();
         ndf      = fitfunc->GetNDF();
         iCoef    = (ALPHA_KEV/fitfunc->GetParameter(1)) * ATTEN;
         iCoefErr = iCoef * fitfunc->GetParameter(2)/fitfunc->GetParameter(1);
         iChi2    = fitfunc->GetChisquare();
      }

      //if (frp)
         //((TH1F*) gH->o["hIntgrlW"])->SetBinContent(i, frp->Value(2));
      ((TH1F*) gH->o["hIntgrlW"])->SetBinContent(i, 100*fitfunc->GetParameter(2)/fitfunc->GetParameter(1));
      ((TH1F*) gH->o["hIntgrlW"])->SetBinError(i, 100*fitfunc->GetParError(2)/(fitfunc->GetParameter(1)));

      ((TH1F*) gH->o["hICoef"])->SetBinContent(i, iCoef);
      ((TH1F*) gH->o["hICoef"])->SetBinError(i, iCoefErr);

      ((TH1F*) gH->o["hICoefDisp"])->Fill(iCoef);

      ChannelCalib tmpChCalib;
      tmpChCalib.fACoef    = aCoef;
      tmpChCalib.fACoefErr = aCoefErr;
      tmpChCalib.fICoef    = iCoef;
      tmpChCalib.fICoefErr = iCoefErr;

      //tmpChCalib.Print();

      ec->fCalibrator->fChannelCalibs[i] = tmpChCalib;

      printf("%6.4f %6.4f %6.2f %6.2f %4.1f %3d  SUCCESSFUL\n",
              cal, ecal, amp, eamp, chi2/ndf, ndf);
      //fprintf(pf0,"%2d %6.4f %6.4f %6.2f %6.2f %4.1f %3d SUCCESSFUL\n",
      //        i,cal[i],ecal[i],amp[i],eamp[i],chi2[i]/ndf,ndf);

      Double_t stat = hAmp->GetEntries();
      if (stat > stat_max) stat_max = stat;
      //if (chi2 > chi2_max) chi2_max = chi2;
   }
}


TFitResultPtr analyzeCalib_fit_histograms(TH1 *h, TF1 *f)
{
   TFitResultPtr frp = 0;

   h->Print();

   if (h->Integral() < 10) {
      f->SetParameters(0, 1, 0);
      f->SetParError(2, 0);
      return frp;
   }

   int   mbin = h->GetMaximumBin();
   float norm = h->GetBinContent(mbin);
   float mean = h->GetBinCenter(mbin);
   float xmin = h->GetXaxis()->GetXmin();
   float xmax = h->GetXaxis()->GetXmax();
   //float ymin = h->GetYaxis()->GetXmin();
   //float ymax = h->GetYaxis()->GetXmax();
   //float intg = h->Integral();
   float expectedSigma = 0.7;

   //h->SetAxisRange(apeak-awidth,apeak+awidth);
   f->SetParameters(norm, mean, expectedSigma);
   f->SetParLimits(0, 0.8*norm, 1.2*norm); // norm
   //f->SetParLimits(0, ymin, ymax); // norm
   //f->SetParLimits(1, mean-5*expectedSigma, mean+5*expectedSigma); // mean
   f->SetParLimits(1, xmin, xmax); // mean
   f->SetParLimits(2, 0.1, 3*expectedSigma); // sigma

   //h->Fit(f, "", "", apeak-awidth, apeak+awidth);
   frp = h->Fit(f, "BMS"); // B: use limits, M: improve fit, S: return frp

   return frp;
}


void analyzeCalib_restore_channels()
{
   Double_t aMean = ((TH1F*) gH->o["hACoefDisp"])->GetMean();
   Double_t aRMS  = ((TH1F*) gH->o["hACoefDisp"])->GetRMS();
   Double_t iMean = ((TH1F*) gH->o["hICoefDisp"])->GetMean();
   Double_t iRMS  = ((TH1F*) gH->o["hICoefDisp"])->GetRMS();

   ChannelCalibMap &chans = ec->fCalibrator->fChannelCalibs;

   ChannelCalibMap::iterator mi, mi2;
   ChannelCalibMap::iterator mb  = chans.begin();
   ChannelCalibMap::iterator me  = chans.end();

   for (mi=mb; mi!=me; mi++) {

      ChannelCalib &ch = mi->second;

      if ( ch.fACoef >= 0 && ch.fACoefErr >= 0 && fabs(aMean - ch.fACoef) > 5*aRMS ) {
         //ch.fACoef    = -1;
         ch.fACoefErr = -1;
      }// else
       //  ch.fACoefErr = -1;

      if ( ch.fICoef >= 0 && ch.fICoefErr >= 0 && fabs(iMean - ch.fICoef) > 5*iRMS ) {
         //ch.fICoef    = -1;
         ch.fICoefErr = -1;
      }
   }

   // Now assign detector average
   // Ampltitude
   for (mi=mb; mi!=me; mi++) {

      ChannelCalib &ch = mi->second;
      if (ch.fACoefErr >= 0) continue;

      short iDet = floor((mi->first-1)/12);
      //printf("iDet: %d\n", iDet);

      float detAve = 0;
      short nChDet = 0;

      for (mi2=mb; mi2!=me; mi2++) {
         ChannelCalib &ch2 = mi2->second;
         short iDet2 = floor((mi2->first-1)/12);
         if (iDet == iDet2 && ch2.fACoefErr >=0) {
            //printf("iDet2: %d\n", iDet2);
            detAve += ch2.fACoef;
            nChDet++;
         }
      }

      detAve /= nChDet;

      ch.fACoef = detAve;
   }

   // Now assign detector average
   // Integral
   for (mi=mb; mi!=me; mi++) {

      ChannelCalib &ch = mi->second;
      if (ch.fICoefErr >= 0) continue;

      short iDet = floor((mi->first-1)/12);
      //printf("iDet: %d\n", iDet);

      float detAve = 0;
      short nChDet = 0;

      for (mi2=mb; mi2!=me; mi2++) {
         ChannelCalib &ch2 = mi2->second;
         short iDet2 = floor((mi2->first-1)/12);
         if (iDet == iDet2 && ch2.fICoefErr >=0) {
            //printf("iDet2: %d\n", iDet2);
            detAve += ch2.fICoef;
            nChDet++;
         }
      }

      detAve /= nChDet;

      ch.fICoef = detAve;
   }
}


//float analyzeCalib_restore_channels_average(UShort_t chId)
//{
//}


void analyzeCalib_finalize()
{
   TCanvas c("cName", "cName", 800, 600);

   string path("", 255);
   sprintf(&path[0], "%s/images", gOutDir.c_str());

   //gH->SaveAllAs(c, path.c_str());
   //gH->Write();
   //gOutFile->Close();

   // Save config info to php file
   string fileName("", 255);
   sprintf(&fileName[0], "%s/runconfig.php", gOutDir.c_str());

   FILE *f = fopen(fileName.c_str(), "w");
   ec->PrintAsPhp(f);
   ec->PrintAsPhp(stdout);

   // Delete from and Write updated run config object to root file
   //gInFile->cd();
   //gInFile->Delete("EventConfig;1");
   //ec->Write("EventConfig");
   //gInFile->Close();
}
