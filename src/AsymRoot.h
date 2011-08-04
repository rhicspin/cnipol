#ifndef AsymRoot_h
#define AsymRoot_h

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


class AnaInfo;


class AsymRoot
{

private:

protected:

   TFile                *fOutRootFile;
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
 
   void         CreateRootFile(std::string filename);
   void         BookHists();
	void         BookHists2(StructFeedBack &feedback);
   void         DeleteHistogram();
   void         Calibrate();
   void         CalibrateFast();
   void         Finalize();
   void         CreateTrees();
   //Bool_t       UseCalibFile(std::string cfname=""); // Deprecated
   void         UpdateRunConfig(); // Deprecated
   void         SetChannelEvent(processEvent &event);
   void         SetChannelEvent(ATStruct &at, long delim, unsigned chId);
   void         ProcessEvent() {};
   void         PreProcess();
   void         PostProcess();
   void         FillPassOne();
   void         PostFillPassOne();
   void         FillScallerHists(Long_t *hData, UShort_t chId);
   void         FillTargetHists(Int_t n, Double_t *hData);
   void         FillProfileHists(UInt_t n, Long_t *hData);
   void         FillRunHists();
   void         ProcessProfileHists();
   void         AddChannelEvent();
   void         PrintEventMap();
   void         PrintChannelEvent();
   void         UpdateCalibrator();
   void         SaveChannelTrees();
   void         SaveEventTree();
   void         WriteTreeFile();
   void         SaveAs(std::string pattern="^.*$", std::string dir=".");
   EventConfig* GetRunConfig();
   void         GetRunConfigs(RunInfo *&ri, AnaInfo *&ai, AnaResult *&ar);
};

#endif
