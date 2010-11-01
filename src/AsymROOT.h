#ifndef ASYM_ROOT_H
#define ASYM_ROOT_H

#include <iostream>

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TTree.h"
#include "TLine.h"

#include "Asym.h"
#include "AnaEvent.h"
#include "ChannelEvent.h"
#include "EventConfig.h"


class Root
{
private:

protected:

   TFile             *rootfile;
   TFile             *fOutTreeFile;

   UInt_t             fTreeFileId;

   TTree             *fRawEventTree;
   TTree             *fAnaEventTree;
   vector<TTree*>     fChannelEventTrees;

   AnaEvent          *fAnaEvent;
   ChannelEvent      *fChannelEvent;
   ChannelData       *fChannelData;
   //ChannelEventMap   fChannelEvents;
   ChannelEventSet    fChannelEvents;
  
public:

   EventConfig       *fEventConfig;

public:

   Root();
   ~Root();
 
   int  RootFile(char*filename);
   int  RootHistBook(StructRunInfo runinfo);
   int  RootHistBook2(datprocStruct dproc, StructRunConst runconst, StructFeedBack feedback);
   int  DeleteHistogram();
   int  CloseROOTFile();
   void CreateTrees();
   void AddChannelEvent(processEvent &event);
   void PrintEventMap();
   void SaveChannelTrees();
   void SaveEventTree();
   void WriteTreeFile();

};

#endif /* ASYM_ROOT_H */
