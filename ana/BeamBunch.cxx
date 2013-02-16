
#include <ios>
#include <iostream>

#include "BeamBunch.h"

ClassImp(BeamBunch)

using namespace std;


/** */
BeamBunch::BeamBunch() : TObject(),
   fIsFilled(kFALSE),
   fIsUserDisabled(kFALSE),
   fBunchSpin(kSPIN_NULL), fLength(-1)
{
}


/** */
BeamBunch::~BeamBunch() { }


Bool_t     BeamBunch::GetFilled() const             { return fIsFilled; }
Bool_t     BeamBunch::IsFilled() const              { return fIsFilled; }
Bool_t     BeamBunch::GetUserDisabled() const       { return fIsUserDisabled; }
ESpinState BeamBunch::GetBunchSpin() const          { return fBunchSpin; }
ESpinState BeamBunch::GetSpin() const               { return fBunchSpin; }
Float_t    BeamBunch::GetLength() const             { return fLength; }

void       BeamBunch::SetFilled(Bool_t yesno)       { fIsFilled = yesno; }
void       BeamBunch::SetUserDisabled(Bool_t yesno) { fIsUserDisabled = yesno; }
void       BeamBunch::SetBunchSpin(ESpinState ss)   { fBunchSpin = ss; }
void       BeamBunch::SetLength(Float_t len)        { fLength = len; }


/** */
void BeamBunch::PrintAsPhp(FILE *f) const
{
   fprintf(f, "$rc['fIsFilled']            = %d;\n", fIsFilled);
   fprintf(f, "$rc['fIsUserDisabled']      = %d;\n", fIsUserDisabled);
   fprintf(f, "$rc['fBunchSpin']           = %d;\n", fBunchSpin);
   fprintf(f, "$rc['fLength']              = %d;\n", fLength);
}


/** */
ostream& operator<<(ostream &os, const BeamBunch &bb)
{
   os << "array(";
   os << "'fIsFilled' => "        << bb.fIsFilled  << ", ";
   os << "'fIsUserDisabled' => "  << bb.fIsUserDisabled << ", ";
   os << "'fBunchSpin' => '"      << bb.fBunchSpin << "', ";
   os << "'fLength' => "          << bb.fLength;
   os << ")";

   return os;
}
