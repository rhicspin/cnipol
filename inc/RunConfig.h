
#ifndef RunConfig_h
#define RunConfig_h

#include <set>
#include <string>

#include "TColor.h"

enum EPolarimeterId {kB1U = 0, kY1D = 1, kB2D = 2, kY2U = 3};

enum EMeasType {kMEASTYPE_UNKNOWN = 0x00,
                kMEASTYPE_ALPHA   = 0x01,
                kMEASTYPE_SWEEP   = 0x02,
                kMEASTYPE_FIXED   = 0x04,
                kMEASTYPE_RAMP    = 0x08,
                kMEASTYPE_EMIT    = 0x10};

enum ETargetOrient {kTARGET_H = 0, kTARGET_V = 1};

enum EBeamEnergy {kINJECTION = 24, kBEAM_ENERGY_100 = 100, kFLATTOP = 250};

typedef std::set<EBeamEnergy>::iterator IterBeamEnergy;


class RunConfig
{
private:

public:
   std::set<EPolarimeterId> fPolarimeters;
   std::set<EMeasType>      fMeasTypes;
   std::set<ETargetOrient>  fTargetOrients;
   std::set<EBeamEnergy>    fBeamEnergies;

   RunConfig();
   ~RunConfig();

   static std::string AsString(EPolarimeterId polId);
   static std::string AsString(EMeasType measType);
   static std::string AsString(ETargetOrient targetOrient);
   static std::string AsString(EBeamEnergy beamEnergy);
   static Color_t     AsColor(EPolarimeterId polId);

};

#endif
