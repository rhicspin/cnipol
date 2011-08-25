/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef DrawObjContainer_h
#define DrawObjContainer_h

#include <map>
#include <iostream>

#include "TCanvas.h"
#include "TClass.h"
#include "TDirectory.h"
#include "TH1.h"
#include "TH2F.h"
#include "THStack.h"
#include "TKey.h"
#include "TObject.h"
#include "TPaveStats.h"
#include "TPRegexp.h"
#include "TSystem.h"

//#include "ChannelEvent.h"

class ChannelEvent;
class DrawObjContainer;
class EventConfig;

typedef std::map<std::string, TObject*> ObjMap;
typedef std::map<std::string, TObject*>::iterator ObjMapIter;
typedef std::map<std::string, TObject*>::const_iterator ObjMapConstIter;
typedef std::map<std::string, DrawObjContainer*> DrawObjContainerMap;
typedef std::map<std::string, DrawObjContainer*>::iterator DrawObjContainerMapIter;
typedef std::map<std::string, DrawObjContainer*>::const_iterator DrawObjContainerMapConstIter;


/** */
class DrawObjContainer : public TObject
{
protected:

	std::string          fSignature;

public:

   //TFile  *f;
   TDirectory          *fDir;
   ObjMap               o;
   DrawObjContainerMap  d;

public:

   DrawObjContainer();
   DrawObjContainer(TDirectory *dir);
   virtual ~DrawObjContainer();

   std::string  GetSignature();
   void         SetSignature(std::string signature);
   void         SetDir(TDirectory *dir);
   void         ReadFromDir();
   void         ReadFromDir(TDirectory *dir);
   void         Add(DrawObjContainer* oc);
   //virtual void Print(const Option_t* opt="") const;
   virtual void Fill(EventConfig &rc);
   virtual void Fill(EventConfig &rc, DrawObjContainer &oc);
   virtual void FillPreProcess(ChannelEvent *ch);
   void         Print(const Option_t* opt="") const;

   virtual void PreFillPassOne();                        // should be used
   virtual void FillPassOne(ChannelEvent *ch);           // should be used
   virtual void FillDerivedPassOne();                    // should be used
   virtual void PostFillPassOne(DrawObjContainer *oc=0); // should be used

   virtual void PreFill(std::string sid="");
   virtual void Fill(ChannelEvent *ch, std::string cutid="");
   virtual void FillDerived();
   virtual void FillDerived(DrawObjContainer &oc) {}          // special processing for dependant histograms
   virtual void PostFill();
   virtual void PostFill(DrawObjContainer *oc) {}          // special processing for dependant histograms
   virtual void SaveAllAs(TCanvas &c, std::string pattern="^.*$", std::string path="./", Bool_t thumbs=kFALSE);
   virtual void SaveHStackAs(TCanvas &c, THStack &hstack, std::string path="./");
   void         Draw(Option_t* option = "") { ((TObject*)this)->Draw(option); }
   void         Draw(TCanvas &c);
   Int_t        Write(const char* name = 0, Int_t option = 0, Int_t bufsize = 0);
   Int_t        Write(const char* name = 0, Int_t option = 0, Int_t bufsize = 0) const;
   void         Delete(Option_t* option="");
   virtual void UpdateLimits();

   ClassDef(DrawObjContainer, 1)
};

#endif
