
#ifndef AnaFillExternResult_h
#define AnaFillExternResult_h

#include <map>
#include <time.h>

#include "TObject.h"
#include "TGraphErrors.h"

#include "Asym.h"


class AnaFillResult;


/** */
class AnaFillExternResult : public TObject
{
public:
  friend class AnaFillResult;

protected:

   TGraphErrors *fBluIntensGraph;
   TGraphErrors *fYelIntensGraph;
   time_t        fTimeEventLumiOn;
   time_t        fTimeEventLumiOff;

public:

protected:

   void ReadInfo(std::ifstream &file);

public:

   AnaFillExternResult();
   AnaFillExternResult(std::ifstream &file);
   ~AnaFillExternResult();

   TGraphErrors* GetGrBluIntens() const;
   TGraphErrors* GetGrYelIntens() const;

   void Print(const Option_t* opt="") const;
   //void PrintAsPhp(FILE *f=stdout) const;

   void Process();

   ClassDef(AnaFillExternResult, 1)
};

#endif
