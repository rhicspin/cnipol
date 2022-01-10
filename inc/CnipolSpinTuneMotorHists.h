
#ifndef CnipolSpinTuneMotorHists_h
#define CnipolSpinTuneMotorHists_h

#include <string>

#include "TDirectoryFile.h"

//#include "SpinTuneMotorHeader.h"

#include "ChannelEvent.h"
#include "DrawObjContainer.h"


/**
 *
 */
class CnipolSpinTuneMotorHists : public DrawObjContainer
{

public:

   CnipolSpinTuneMotorHists();
   CnipolSpinTuneMotorHists(TDirectory *dir);
   ~CnipolSpinTuneMotorHists();

   void BookHists();
   //void PreFill();
   void Fill(ChannelEvent *ch);
   void FillDerived();
   //void FillDerived(DrawObjContainer &oc);
   void PostFill();

   ClassDef(CnipolSpinTuneMotorHists, 1)
};

#endif
