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


/**
 *
 */
class CnipolCalibHists : public DrawObjContainer
{
public:


public:

   CnipolCalibHists();
   CnipolCalibHists(TDirectory *dir);
   ~CnipolCalibHists();

   void CnipolCalibHistsBook();
   //virtual void Print(const Option_t* opt="") const;
   void Print(const Option_t* opt="") const;
   void SaveAllAs(TCanvas &c, std::string path="./");

   ClassDef(CnipolCalibHists, 1)
};

#endif
