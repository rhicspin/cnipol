/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef Target_h
#define Target_h

#include <stdio.h>
#include <vector>

#include "TObject.h"

#include "TargetUId.h"


class Target;

typedef std::set<Target>      TargetSet;
typedef TargetSet::iterator   TargetSetIter;


/** */
class Target : public TObject
{
public:
   
   TargetUId fUId;
   Double_t  fThickness;

public:

   Target();
   Target(EPolarimeterId polId, ETargetOrient tgtOrient, UShort_t tgtId);
   ~Target();

   void Print(const Option_t* opt="") const;
   bool operator<(const Target &tgtuid) const;

   ClassDef(Target, 1)
};

#endif
