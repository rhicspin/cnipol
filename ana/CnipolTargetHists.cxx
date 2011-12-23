/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolTargetHists.h"

#include "AsymGlobals.h"
#include "TargetInfo.h"
#include "RunInfo.h"
#include "AnaInfo.h"
#include "AnaMeasResult.h"


ClassImp(CnipolTargetHists)

using namespace std;

/** Default constructor. */
CnipolTargetHists::CnipolTargetHists() : DrawObjContainer()
{
   BookHists();
}


CnipolTargetHists::CnipolTargetHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
}


/** Default destructor. */
CnipolTargetHists::~CnipolTargetHists()
{
}


/** */
void CnipolTargetHists::BookHists(string sid)
{ //{{{
   fDir->cd();

   if (!sid.empty()) sid = "_" + sid;

   char hName[256];

   // Older histograms from earlier times
   sprintf(hName, "rate_vs_delim");
   o[hName] = new TH2F(hName, hName, 1, 0, 1, 1, 0, 1);
   ((TH1*) o[hName])->SetOption("NOIMG");
   ((TH1*) o[hName])->SetTitle(";Target Position;Carbon in Banana Rate, MHz;");

   sprintf(hName, "tgtx_vs_time");
   o[hName] = new TH2F(hName, hName, 1, 0, 1, 1, 0, 1);
   ((TH1*) o[hName])->SetOption("NOIMG");
   ((TH1*) o[hName])->SetTitle(";Target Position;Duration from Measurement Start, s;");

   // Newer histograms
   sprintf(hName, "hTargetSteps");
   o[hName] = new TH1I(hName, hName, 100, 0, 100);
   ((TH1*) o[hName])->SetTitle(";Target Steps;;");
   ((TH1*) o[hName])->SetBit(TH1::kCanRebin);

   sprintf(hName, "hTargetChanVertYel");
   o[hName] = new TH1F(hName, hName, 255, 0, 255);
   ((TH1*) o[hName])->SetOption("NOIMG");
   ((TH1*) o[hName])->SetTitle(";ADC;;");

   sprintf(hName, "hTargetChanHorzYel");
   o[hName] = new TH1F(hName, hName, 255, 0, 255);
   ((TH1*) o[hName])->SetOption("NOIMG");
   ((TH1*) o[hName])->SetTitle(";ADC;;");

   sprintf(hName, "hTargetChanVertBlu");
   o[hName] = new TH1F(hName, hName, 255, 0, 255);
   ((TH1*) o[hName])->SetOption("NOIMG");
   ((TH1*) o[hName])->SetTitle(";ADC;;");

   sprintf(hName, "hTargetChanHorzBlu");
   o[hName] = new TH1F(hName, hName, 255, 0, 255);
   ((TH1*) o[hName])->SetOption("NOIMG");
   ((TH1*) o[hName])->SetTitle(";ADC;;");

   sprintf(hName, "hTargetHorzLinear");
   o[hName] = new TH1F(hName, hName, 100, 0, 100);
   ((TH1*) o[hName])->SetOption("NOIMG");
   ((TH1*) o[hName])->SetTitle(";Time;Position;");
   //((TH1*) o[hName])->SetBit(TH1::kCanRebin);

   sprintf(hName, "hTargetHorzRotary");
   o[hName] = new TH1F(hName, hName, 100, 0, 100);
   ((TH1*) o[hName])->SetTitle(";Time;Position;");
   //((TH1*) o[hName])->SetBit(TH1::kCanRebin);

   sprintf(hName, "hTargetVertLinear");
   o[hName] = new TH1F(hName, hName, 100, 0, 100);
   ((TH1*) o[hName])->SetTitle(";Time;Position;");
   //((TH1*) o[hName])->SetBit(TH1::kCanRebin);

   sprintf(hName, "hTargetVertRotary");
   o[hName] = new TH1F(hName, hName, 100, 0, 100);
   ((TH1*) o[hName])->SetTitle(";Time;Position;");
   //((TH1*) o[hName])->SetBit(TH1::kCanRebin);
} //}}}


/** */
void CnipolTargetHists::Fill(ChannelEvent *ch, string sid)
{ //{{{
   UChar_t chId  = ch->GetChannelId();
   //UChar_t detId = ch->GetDetectorId();

   //string sDetId(" ");
   //sprintf(&sDetId[0], "%1d", detId);
   //char hName[256];

   //if (!ch->PassCutTargetChannel()) return;

   UChar_t chIndex = chId - gConfigInfo->data.NumChannels + 4;
   //UChar_t chIndex = chId - NSTRIP;

   switch (chIndex) {
   case 1:
      ((TH1*) o["hTargetChanVertYel"])->Fill(ch->fChannel.fAmpltd);
      break;
   case 2:
      ((TH1*) o["hTargetChanHorzYel"])->Fill(ch->fChannel.fAmpltd);
      break;
   case 3:
      ((TH1*) o["hTargetChanVertBlu"])->Fill(ch->fChannel.fAmpltd);
      break;
   case 4:
      ((TH1*) o["hTargetChanHorzBlu"])->Fill(ch->fChannel.fAmpltd);
      break;
   }

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

   ((TH1*) o["hTargetSteps"])->Fill(tstep);

} //}}}


/**
 * The size of hData is 4*n+4
 */
void CnipolTargetHists::Fill(Int_t n, Double_t* hData)
{ //{{{
   ((TH1*) o["hTargetHorzLinear"])->SetBins(n, 0, n);
   ((TH1*) o["hTargetHorzLinear"])->SetContent(hData);

   ((TH1*) o["hTargetHorzRotary"])->SetBins(n, 0, n);
   ((TH1*) o["hTargetHorzRotary"])->SetContent(hData + (n+2) - 1);

   ((TH1*) o["hTargetVertLinear"])->SetBins(n, 0, n);
   ((TH1*) o["hTargetVertLinear"])->SetContent(hData + 2*(n+2) - 1);

   ((TH1*) o["hTargetVertRotary"])->SetBins(n, 0, n);
   ((TH1*) o["hTargetVertRotary"])->SetContent(hData + 3*(n+2) - 1);
} //}}}


/** */
void CnipolTargetHists::PostFill()
{ //{{{
   //char  htitle[100];
   float dx[MAXDELIM], y[MAXDELIM], dy[MAXDELIM];
   int   X_index = gRunInfo->Run >= 6 ? nTgtIndex : gNDelimeters;
 
   float xmin, xmax;
   float margin = 0.02;

   //GetMinMax(nTgtIndex, tgt.X, margin, xmin, xmax);
   GetMinMax(X_index, tgt.X, margin, xmin, xmax);

   printf("xmin, xmax: %f, %f, %d, %d\n", xmin, xmax, X_index, gNDelimeters);
 
   // Make rate _vs deliminter plots
   for (int i=0; i<X_index; i++) {

     UInt_t count = ((TH1*) o["hTargetSteps"])->GetBinContent(i+1);

     dx[i] = 0;
     // y[i] = tgt.Interval[i] ? float(cntr.good[i]) / tgt.Interval[i] * MHz : 0 ;
     //dy[i] = tgt.Interval[i] ? float( sqrt( double(cntr.good[i]) ) ) / tgt.Interval[i] * MHz : 0;

      y[i] = tgt.Interval[i] ? float(count) / tgt.Interval[i] * MHz : 0 ;
     dy[i] = tgt.Interval[i] ? float(sqrt(count)) / tgt.Interval[i] * MHz : 0;
   }
 
   gAnaMeasResult->max_rate = GetMax(X_index, y);

   float ymin, ymax;

   GetMinMax(X_index, y, margin, ymin, ymax);

   //sprintf(htitle,"%.3f : Rate vs Taret Postion", gRunInfo->RUNID);
 
   ((TH1*) o["rate_vs_delim"])->SetBins(100, xmin, xmax, 100, ymin, ymax);
 
   //TGraphErrors *rate_delim = new TGraphErrors(nTgtIndex, tgt.X, y, dx, dy);
   TGraphErrors *rate_delim = new TGraphErrors(X_index, tgt.X, y, dx, dy);

   rate_delim->SetMarkerStyle(20);
   rate_delim->SetMarkerColor(kBlue);
   ((TH1*) o["rate_vs_delim"])->GetListOfFunctions()->Add(rate_delim, "P");
	//delete rate_delim;
 
   // Target Position vs Time
   //sprintf(htitle,"%.3f : Taret Postion vs. Time", gRunInfo->RUNID);
 
   //TH2F *tgtx_vs_time = new TH2F("tgtx_vs_time", htitle, 10, xmin, xmax, 10, 0.5, gRunInfo->fRunTime*1.2);

   ((TH1*) o["tgtx_vs_time"])->SetBins(10, xmin, xmax, 10, 0.5, gRunInfo->fRunTime*1.2);
 
   //delete gAsymRoot.fHists->d["run"]->d["Run"]->o["tgtx_vs_time"];
   //gAsymRoot.fHists->d["run"]->d["Run"]->o["tgtx_vs_time"] = tgtx_vs_time;
 
   //TGraph *tgtx_time = new TGraph(nTgtIndex, tgt.X, tgt.Time);
   TGraph *tgtx_time = new TGraph(X_index, tgt.X, tgt.Time);
   tgtx_time->SetMarkerStyle(20);
   tgtx_time->SetMarkerColor(kBlue);
   ((TH1*) o["tgtx_vs_time"])->GetListOfFunctions()->Add(tgtx_time, "P");
	//delete tgtx_time;
} //}}}
