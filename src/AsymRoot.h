#ifndef ASYM_ROOT_H
#define ASYM_ROOT_H

#include <iostream>
#include <math.h>
#include <vector>

#include "TFile.h"
#include "TTree.h"

#include "AsymGlobals.h"

#include "AnaEvent.h"
#include "ChannelData.h"
#include "ChannelEvent.h"
#include "DrawObjContainer.h"
#include "EventConfig.h"

//struct StructFeedBack;
//struct processEvent;


class AsymRoot
{

private:

protected:

   TFile                *rootfile;
   TFile                *fOutTreeFile;
   UInt_t                fTreeFileId;
  
public:

   TTree                *fRawEventTree;
   TTree                *fAnaEventTree;
   std::vector<TTree*>   fChannelEventTrees;
   AnaEvent             *fAnaEvent;
   ChannelEvent         *fChannelEvent;
   ChannelData          *fChannelData;
   //ChannelEventMap      fChannelEvents;
   ChannelEventSet       fChannelEvents;
   EventConfig          *fEventConfig;
   DrawObjContainer     *fHists;

public:

   AsymRoot();
   ~AsymRoot();
 
   void   RootFile(std::string filename);
   void   BookHists();
	void   BookHists2(StructFeedBack &feedback);
   void   DeleteHistogram();
   void   Calibrate();
   void   CalibrateFast();
   void   Finalize();
   void   CreateTrees();
   Bool_t UseCalibFile(std::string cfname="");
   void   UpdateRunConfig();
   void   SetChannelEvent(processEvent &event);
   void   SetChannelEvent(ATStruct &at, long delim, unsigned chId);
   void   ProcessEvent() {};
   void   PreProcess();
   void   PostProcess();
   void   FillPreProcess();
   void   FillScallerHists(Long_t *hData, UShort_t chId);
   void   FillTargetHists(Int_t n, Double_t *hData);
   void   FillProfileHists(UInt_t n, Long_t *hData);
   void   ProcessProfileHists();
   void   AddChannelEvent();
   void   PrintEventMap();
   void   UpdateCalibrator();
   void   SaveChannelTrees();
   void   SaveEventTree();
   void   WriteTreeFile();
   void   SaveAs(std::string pattern="^.*$", std::string dir=".");
};

#endif
