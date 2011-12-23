
#include "TargetUId.h"


ClassImp(TargetUId)

using namespace std;


/** */
TargetUId::TargetUId() : TObject(), fPolId(kUNKNOWN_POLID), fTargetOrient(kUNKNOWN_ORIENT), fTargetId(0)
{ //{{{
} //}}}


/** */
TargetUId::TargetUId(EPolarimeterId polId, ETargetOrient tgtOrient, UShort_t tgtId) :
   TObject(), fPolId(polId), fTargetOrient(tgtOrient), fTargetId(tgtId)
{ //{{{
} //}}}


/** */
TargetUId::~TargetUId()
{
}


/** */
bool TargetUId::operator<(const TargetUId &tgtuid) const
{ //{{{
   if (fPolId < tgtuid.fPolId) return true;
   if (fPolId > tgtuid.fPolId) return false;

   if (fTargetOrient < tgtuid.fTargetOrient) return true;
   if (fTargetOrient > tgtuid.fTargetOrient) return false;

   if (fTargetId < tgtuid.fTargetId) return true;

   return false;
} //}}}
