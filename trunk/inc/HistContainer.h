/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef HistContainer_h
#define HistContainer_h

#include "TObject.h"


/**
 *
 */
class HistContainer : public TObject
{
public:

public:

   HistContainer();
   ~HistContainer();

   //virtual void Print(const Option_t* opt="") const;
   void Print(const Option_t* opt="") const;

   ClassDef(HistContainer, 1)
};

#endif
