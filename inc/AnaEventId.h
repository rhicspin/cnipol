/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef AnaEventId_h
#define AnaEventId_h

#include "TObject.h"

/**
 *
 */
class AnaEventId : public TObject
{
public:

   UInt_t  fRevolutionId;
   UInt_t  fBunchId;

public:

   AnaEventId();
   ~AnaEventId();

   virtual void Print(const Option_t* opt="") const;

   ClassDef(AnaEventId, 1)
};

#endif
