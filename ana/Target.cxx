
#include "Target.h"


ClassImp(Target)

using namespace std;


/** */
Target::Target() : TObject(), fUId(kUNKNOWN_POLID, kUNKNOWN_ORIENT, 0), fThickness(0)
{
}


/** */
Target::Target(EPolarimeterId polId, ETargetOrient tgtOrient, UShort_t tgtId) :
   TObject(), fUId(polId, tgtOrient, tgtId), fThickness(0)
{
}


/** */
Target::~Target()
{
}


/** */
void Target::Print(const Option_t* opt) const
{
   fUId.Print();
}


/** */
bool Target::operator<(const Target &tgt) const
{
   if (fUId < tgt.fUId) return true;
   return false;
}
