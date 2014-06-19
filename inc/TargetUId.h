
#ifndef TargetUId_h
#define TargetUId_h

#include <iostream>
#include <stdio.h>
#include <vector>

#include "TObject.h"

#include "AsymCommon.h"


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
   TargetUId(const TargetUId& target);
   ~TargetUId();

   void Print(const Option_t* opt="") const;
   TargetUId& operator= (const TargetUId &tgt);
   bool IsValid();
   bool IsCompletelyUnknown();

   friend std::ostream& operator<<(std::ostream& os, TargetUId &tgt);

   ClassDef(TargetUId, 1)
};


bool operator==(const TargetUId& lhs, const TargetUId& rhs);
inline bool operator!=(const TargetUId& lhs, const TargetUId& rhs){return !operator==(lhs,rhs);} 
bool operator< (const TargetUId& lhs, const TargetUId& rhs);
inline bool operator> (const TargetUId& lhs, const TargetUId& rhs){return  operator< (rhs,lhs);} 
inline bool operator<=(const TargetUId& lhs, const TargetUId& rhs){return !operator> (lhs,rhs);} 
inline bool operator>=(const TargetUId& lhs, const TargetUId& rhs){return !operator< (lhs,rhs);}


#endif
