/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolCalibHists_h
#define CnipolCalibHists_h

#include "TClass.h"
#include "TDirectoryFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TSystem.h"

#include "AsymHeader.h"

#include "DrawObjContainer.h"
#include "ChannelEvent.h"

//class ChannelEvent;


/** */
class CnipolCalibHists : public DrawObjContainer
{
public:


public:

   CnipolCalibHists();
   CnipolCalibHists(TDirectory *dir);
   virtual ~CnipolCalibHists();

   void CnipolCalibHistsBook(std::string cutid="");
   //virtual void Print(const Option_t* opt="") const;
   void Print(const Option_t* opt="") const;
   void Fill(ChannelEvent *ch, std::string cutid="");
   void FillPreProcess(ChannelEvent *ch);
   void PostFill();
   //void SaveAllAs(TCanvas &c, std::string pattern="*", std::string path="./");

   ClassDef(CnipolCalibHists, 1)
};

#endif
