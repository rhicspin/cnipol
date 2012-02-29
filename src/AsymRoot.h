#ifndef AsymRoot_h
#define AsymRoot_h

#include <iostream>
#include <math.h>
#include <map>
#include <vector>

#include "TFile.h"
#include "TTree.h"

#include "AsymGlobals.h"
#include "MseMeasInfo.h"

#include "AnaEvent.h"
#include "ChannelData.h"
#include "ChannelEvent.h"
#include "DrawObjContainer.h"
#include "EventConfig.h"


class AnaInfo;

enum ECut {kCUT_UNKNOWN, kCUT_NOCUT, kCUT_PASSONE_RAW, kCUT_PASSONE_PULSER, kCUT_PASSONE_PMT, kCUT_PASSONE_ALL,
           kCUT_RAW, kCUT_RAW_EB, kCUT_NOISE, kCUT_CARBON, kCUT_CARBON_EB };

typedef std::map<ECut, std::set<DrawObjContainer*> >   Cut2DrawObjContainerMap;
typedef Cut2DrawObjContainerMap::iterator              Cut2DrawObjContainerMapIter;


class AsymRoot
{

private:

protected:

   TFile                    *fOutRootFile;
   TFile                    *fOutTreeFile;
   UInt_t                    fTreeFileId;
   Cut2DrawObjContainerMap   fHistCuts;    // Map with histograms containers classified by cut categories
  
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
   void         FillPassOne(ECut cut);
   void         FillDerivedPassOne();
   void         PostFillPassOne();
   void         PreFill();
   void         Fill(ECut cut);
   void         FillDerived();
   void         PostFill(MseMeasInfoX &run);
   void         FillScallerHists(Long_t *hData, UShort_t chId);
   void         FillTargetHists(Int_t n, Double_t *hData);
   void         FillProfileHists(UInt_t n, Long_t *hData);
   void         FillRunHists();
   void         AddChannelEvent();
   void         PrintEventMap();
   void         PrintChannelEvent();
   void         UpdateCalibrator();
   void         SaveChannelTrees();
   void         SaveEventTree();
   void         WriteTreeFile();
   void         SaveAs(std::string pattern="^.*$", std::string dir=".");
   EventConfig* GetRunConfig();
   void         GetRunConfigs(MeasInfo *&ri, AnaInfo *&ai, AnaMeasResult *&ar);
};

#endif
