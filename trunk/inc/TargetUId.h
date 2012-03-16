/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef TargetUId_h
#define TargetUId_h

#include <stdio.h>
#include <vector>

#include "TObject.h"

#include "Asym.h"


class TargetUId;

typedef std::map<TargetUId, ValErrPair>        TargetUId2ValErrMap;
typedef TargetUId2ValErrMap::iterator          TargetUId2ValErrMapIter;
typedef TargetUId2ValErrMap::const_iterator    TargetUId2ValErrMapConstIter;


/** */
class TargetUId : public TObject
{
public:
   
   EPolarimeterId fPolId;
   ETargetOrient  fTargetOrient;
   UShort_t       fTargetId;

public:

   TargetUId();
   TargetUId(EPolarimeterId polId, ETargetOrient tgtOrient, UShort_t tgtId);
   ~TargetUId();

   void Print(const Option_t* opt="") const;
   bool operator<(const TargetUId &tgtuid) const;

   ClassDef(TargetUId, 1)
};

#endif
