/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "Event.h"

ClassImp(Event)

using namespace std;

/** Default constructor. */
AnaEvent::AnaEvent() : TObject()
{
};

/** Default destructor. */
AnaEvent::~AnaEvent()
{
};

/**
 *
 */
void AnaEvent::Print(const Option_t* opt) const
{
   opt = "";

   printf("AnaEvent:\n");
}
