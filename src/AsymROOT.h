#ifndef ASYM_ROOT_H
#define ASYM_ROOT_H

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TTree.h"
#include "TLine.h"

#include "Asym.h"
#include "AnaEvent.h"
#include "ChannelEvent.h"


class Root
{
private:

protected:

   TTree        *fRawEventTree;

public:

   TFile           *rootfile;
   ChannelEvent    *fChannelEvent;
   ChannelEventMap  fChannelEvents;
  
public:

   Root();
   ~Root();
 
   int  RootFile(char*filename);
   int  RootHistBook(StructRunInfo runinfo);
   int  RootHistBook2(datprocStruct dproc, StructRunConst runconst, StructFeedBack feedback);
   int  DeleteHistogram();
   int  CloseROOTFile();
   void CreateTree();
   void AddChannelEvent(processEvent &event);
   void FillTree();
   void PrintEventMap();
   void SaveChannelTrees();
   void SaveEventTree();

};

#endif /* ASYM_ROOT_H */
