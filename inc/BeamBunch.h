#ifndef BeamBunch_h
#define BeamBunch_h

#include <map>

#include "TObject.h"

#include "RunConfig.h"


class BeamBunch;

typedef std::map<UShort_t, BeamBunch>                 BeamBunchMap;
typedef std::map<UShort_t, BeamBunch>::iterator       BeamBunchIter;
typedef std::map<UShort_t, BeamBunch>::const_iterator BeamBunchIterConst;


/** */
class BeamBunch : public TObject
{
public:

   Bool_t     fIsFilled;
   Bool_t     fIsUserDisabled;
   ESpinState fBunchSpin;
   Float_t    fLength;         // in ns

public:

   BeamBunch();
   ~BeamBunch();

   Bool_t     GetFilled() const;
   Bool_t     IsFilled() const;
   Bool_t     GetUserDisabled() const;
   ESpinState GetBunchSpin() const;
   ESpinState GetSpin() const;
   Float_t    GetLength() const;
   void       SetFilled(Bool_t yesno);
   void       SetUserDisabled(Bool_t yesno);
   void       SetBunchSpin(ESpinState ss);
   void       SetLength(Float_t len);
   void       PrintAsPhp(FILE *f=stdout) const;

   friend ostream& operator<<(ostream &os, const BeamBunch &bb);

   ClassDef(BeamBunch, 1)
};

#endif
