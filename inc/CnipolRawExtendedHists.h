/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolRawExtendedHists_h
#define CnipolRawExtendedHists_h

#include "TDirectoryFile.h"

#include "AsymHeader.h"

#include "ChannelEvent.h"
#include "DrawObjContainer.h"


/**
 *
 */
class CnipolRawExtendedHists : public DrawObjContainer
{
public:


public:

   CnipolRawExtendedHists();
   CnipolRawExtendedHists(TDirectory *dir);
   ~CnipolRawExtendedHists();

   void  BookHists();
   void  Fill(ChannelEvent *ch);
   void  FillDerived();

   ClassDef(CnipolRawExtendedHists, 1)
};

#endif
