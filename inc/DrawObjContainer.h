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
#include "TH1.h"
#include "TH2F.h"
#include "TDirectory.h"
#include "TObject.h"
#include "TKey.h"
#include "TSystem.h"

class DrawObjContainer;

typedef std::map<std::string, TObject*> ObjMap;
typedef std::map<std::string, TObject*>::iterator ObjMapIter;
typedef std::map<std::string, DrawObjContainer> DrawObjContainerMap;
typedef std::map<std::string, DrawObjContainer>::iterator DrawObjContainerMapIter;

/**
 *
 */
class DrawObjContainer : public TObject
{
public:

   //TFile  *f;
   TDirectory          *fDir;
   ObjMap               o;
   DrawObjContainerMap  d;

public:

   DrawObjContainer();
   DrawObjContainer(TDirectory *dir);
   ~DrawObjContainer();

   void ReadFromDir();
   void ReadFromDir(TDirectory *dir);
   //virtual void Print(const Option_t* opt="") const;
   void Print(const Option_t* opt="") const;
   void SaveAllAs(TCanvas &c, std::string path="./");
   void Draw(Option_t* option = "")
      { ((TObject*)this)->Draw(option); }
   void Draw(TCanvas &c);
   Int_t Write(const char* name = 0, Int_t option = 0, Int_t bufsize = 0);
   Int_t Write(const char* name = 0, Int_t option = 0, Int_t bufsize = 0) const;
   void Delete();

   ClassDef(DrawObjContainer, 1)
};

#endif
