/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef AnaEvent_h
#define AnaEvent_h

#include "TObject.h"

class AnaEvent;

typedef std::vector<AnaEvent> AnaEventVec;

/**
 *
 */
class AnaEvent : public TObject
{
public:

   GntChannelVec fChannels;

public:

   AnaEvent();
   ~AnaEvent();

   virtual void Print(const Option_t* opt="") const;

   ClassDef(AnaEvent, 1)
};

#endif
