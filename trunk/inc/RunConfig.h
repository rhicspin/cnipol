
#ifndef RunConfig_h
#define RunConfig_h

#include <set>
#include <string>

#include "TColor.h"
#include "TObject.h"

#include "Asym.h"
#include "Target.h"


class RunConfig : public TObject
{
private:

public:
   std::set<EPolarimeterId> fPolarimeters;
   TargetSet                fTargets;
   std::set<EMeasType>      fMeasTypes;
   std::set<ETargetOrient>  fTargetOrients;
   std::set<EBeamId>        fBeams;
   std::set<ERingId>        fRings;
   std::set<EBeamEnergy>    fBeamEnergies;
   std::set<ESpinState>     fSpinStates;

   RunConfig();
   ~RunConfig();

   static std::string AsString(EPolarimeterId polId);
   static std::string AsString(EMeasType measType);
   static std::string AsString(ETargetOrient targetOrient);
   static std::string AsString(EBeamId beamId);
   static std::string AsString(ERingId ringId);
   static std::string AsString(EBeamEnergy beamEnergy);
   static std::string AsString(ESpinState spinState);
   static UShort_t    AsIndex(ESpinState spinState);
   static Color_t     AsColor(EBeamId beamId);
   static Color_t     AsColor(ERingId ringId);
   static Color_t     AsColor(EPolarimeterId polId);
   static Color_t     AsColor(ESpinState spin);

   static EBeamId     GetBeamId(EPolarimeterId polId);
   static ERingId     GetRingId(EPolarimeterId polId);
   static EStreamId   GetStreamId(EPolarimeterId polId);

   static UShort_t    GetDetectorId(UShort_t chId);
};

#endif
