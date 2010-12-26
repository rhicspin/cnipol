#ifndef ASYM_ROOT_H
#define ASYM_ROOT_H

#include <iostream>
#include <math.h>
#include <vector>

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TTree.h"
#include "TLine.h"

#include "Asym.h"
#include "AlphaCalibrator.h"
#include "AnaEvent.h"
#include "AsymGlobals.h"
#include "AnaInfo.h"
#include "RunInfo.h"

#include "ChannelEvent.h"
#include "CnipolCalibHists.h"
#include "CnipolHists.h"
#include "DeadLayerCalibrator.h"
#include "DrawObjContainer.h"
#include "EventConfig.h"

struct StructRunConst;
struct StructFeedBack;
struct processEvent;


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
   //CnipolHists          *fCnipolHists;
   DrawObjContainer     *fHists;

public:

   AsymRoot();
   ~AsymRoot();
 
   Int_t  RootFile(char*filename);
   Int_t  BookHists(TStructRunInfo runinfo);
	Int_t  BookHists2(TDatprocStruct &dproc, StructRunConst &runconst,
             StructFeedBack &feedback);
   Int_t  DeleteHistogram();
   Int_t  CloseROOTFile();
   void   CreateTrees();
   Bool_t UseCalibFile(std::string cfname="");
   void   SetChannelEvent(processEvent &event);
   void   ProcessEvent() {};
   void   PostProcess();
   void   AddChannelEvent();
   void   PrintEventMap();
   void   UpdateRunConfig();
   void   SaveChannelTrees();
   void   SaveEventTree();
   void   WriteTreeFile();
};

#endif /* ASYM_ROOT_H */
