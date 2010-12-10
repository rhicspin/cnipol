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

#include "AsymGlobals.h"
#include "AnaEvent.h"
#include "ChannelEvent.h"
#include "CnipolHists.h"
#include "DeadLayerCalibrator.h"
#include "EventConfig.h"


class Root
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
   CnipolHists          *fCnipolHists;

public:

   Root();
   ~Root();
 
   Int_t  RootFile(char*filename);
   Int_t  RootHistBook(TStructRunInfo runinfo);
   Int_t  RootHistBook2(TDatprocStruct &dproc, StructRunConst &runconst,
                        StructFeedBack &feedback);
   Int_t  DeleteHistogram();
   Int_t  CloseROOTFile();
   void   CreateTrees();
   Bool_t UseCalibFile(std::string cfname="");
   void   SetChannelEvent(processEvent &event);
   void   AddChannelEvent();
   void   PrintEventMap();
   void   UpdateRunConfig();
   void   SaveChannelTrees();
   void   SaveEventTree();
   void   WriteTreeFile();
};

#endif /* ASYM_ROOT_H */
