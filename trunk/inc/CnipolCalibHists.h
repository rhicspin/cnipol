/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolCalibHists_h
#define CnipolCalibHists_h

#include "TDirectoryFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TSystem.h"

#include "AsymHeader.h"

#include "DrawObjContainer.h"
#include "ChannelEvent.h"


/** */
class CnipolCalibHists : public DrawObjContainer
{
public:


public:

   CnipolCalibHists();
   CnipolCalibHists(TDirectory *dir);
   virtual ~CnipolCalibHists();

   void BookHists(std::string cutid="");
   void PostFill();

   ClassDef(CnipolCalibHists, 1)
};

#endif
