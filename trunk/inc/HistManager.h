/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef HistManager_h
#define HistManager_h

#include "TObject.h"


/**
 *
 */
class HistManager : public TObject
{
public:


public:

   HistManager();
   ~HistManager();

   //virtual void Print(const Option_t* opt="") const;
   void Print(const Option_t* opt="") const;

   ClassDef(HistManager, 1)
};

#endif
