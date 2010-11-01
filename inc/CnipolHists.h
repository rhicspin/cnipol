/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolHists_h
#define CnipolHists_h

#include "TH2F.h"

#include "AsymHeader.h"

#include "DrawObjContainer.h"


/**
 *
 */
class CnipolHists : public DrawObjContainer
{
public:


public:

   CnipolHists();
   CnipolHists(TDirectory *dir);
   ~CnipolHists();

   void CnipolHistsBook();
   //virtual void Print(const Option_t* opt="") const;
   void Print(const Option_t* opt="") const;

   ClassDef(CnipolHists, 1)
};

#endif
