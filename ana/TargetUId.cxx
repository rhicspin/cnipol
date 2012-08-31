
#include "RunConfig.h"
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
TargetUId::TargetUId(const TargetUId& tgt) :
   TObject(), fPolId(tgt.fPolId), fTargetOrient(tgt.fTargetOrient), fTargetId(tgt.fTargetId)
{ //{{{
} //}}}


/** */
TargetUId::~TargetUId()
{ //{{{
} //}}}


/** */
void TargetUId::Print(const Option_t* opt) const
{ //{{{
   cout << (TargetUId&)*this;
} //}}}


/** */
bool TargetUId::IsValid()
{ //{{{
   if (fPolId == kUNKNOWN_POLID || fTargetOrient == kUNKNOWN_ORIENT || fTargetId == 0) return false;
   return true;
} //}}}


/** */
bool TargetUId::IsCompletelyUnknown()
{ //{{{
   if (fPolId == kUNKNOWN_POLID && fTargetOrient == kUNKNOWN_ORIENT && fTargetId == 0) return true;
   return false;
} //}}}


/** */
TargetUId& TargetUId::operator= (const TargetUId &tgt)
{ //{{{
   fPolId        = tgt.fPolId;
   fTargetOrient = tgt.fTargetOrient;
   fTargetId     = tgt.fTargetId;

   return *this;
} //}}}


/** */
ostream& operator<<(ostream& os, TargetUId &tgt)
{ //{{{
   os << RunConfig::AsString(tgt.fPolId) << " " << RunConfig::AsString(tgt.fTargetOrient) << " " << tgt.fTargetId;
   return os;
} //}}}


/** */
bool operator<(const TargetUId &lhs, const TargetUId &rhs)
{ //{{{
   if (lhs.fPolId < rhs.fPolId) return true;
   if (lhs.fPolId > rhs.fPolId) return false;

   if (lhs.fTargetOrient < rhs.fTargetOrient) return true;
   if (lhs.fTargetOrient > rhs.fTargetOrient) return false;

   if (lhs.fTargetId < rhs.fTargetId) return true;

   return false;
} //}}}


/** */
bool operator==(const TargetUId &lhs, const TargetUId &rhs)
{ //{{{
   if (lhs.fPolId == rhs.fPolId && lhs.fTargetOrient == rhs.fTargetOrient && lhs.fTargetId == rhs.fTargetId) return true;
   return false;
} //}}}
