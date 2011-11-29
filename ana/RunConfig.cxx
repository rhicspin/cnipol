
#include <limits.h>

#include "RunConfig.h"

#include "TSystem.h"

#include "AsymHeader.h"

using namespace std;


ostream& operator<<(ostream &os, const ESpinState &ss)
{ //{{{
   if      (ss == kSPIN_UP)   os << "+";
   else if (ss == kSPIN_DOWN) os << "-";
   else if (ss == kSPIN_NULL) os << ".";
   else                       os << "*";

   return os;
} //}}}


/** */
RunConfig::RunConfig() : TObject(), fPolarimeters(), fMeasTypes(),
   fTargetOrients(), fBeamEnergies(), fSpinStates()
{ //{{{
   fPolarimeters.insert(kB1U);
   fPolarimeters.insert(kY1D);
   fPolarimeters.insert(kB2D);
   fPolarimeters.insert(kY2U);

   fMeasTypes.insert(kMEASTYPE_UNKNOWN);
   fMeasTypes.insert(kMEASTYPE_ALPHA  );
   fMeasTypes.insert(kMEASTYPE_SWEEP  );
   fMeasTypes.insert(kMEASTYPE_FIXED  );
   fMeasTypes.insert(kMEASTYPE_RAMP   );
   fMeasTypes.insert(kMEASTYPE_EMIT   );

   fTargetOrients.insert(kTARGET_H);
   fTargetOrients.insert(kTARGET_V);

   fBeams.insert(kBLUE_BEAM);
   fBeams.insert(kYELLOW_BEAM);

   fRings.insert(kBLUE_RING);
   fRings.insert(kYELLOW_RING);

   fBeamEnergies.insert(kINJECTION);
   fBeamEnergies.insert(kBEAM_ENERGY_100);
   fBeamEnergies.insert(kFLATTOP);

   fSpinStates.insert(kSPIN_DOWN);
   fSpinStates.insert(kSPIN_NULL);
   fSpinStates.insert(kSPIN_UP);
} //}}}


/** */
RunConfig::~RunConfig()
{
}


/** */
string RunConfig::AsString(EPolarimeterId polId)
{ //{{{
   switch (polId) {
   case kB1U:
	   return "B1U";
   case kY1D:
	   return "Y1D";
   case kB2D:
	   return "B2D";
   case kY2U:
	   return "Y2U";
   default:
      return "UNK";
   }
} //}}}


/** */
string RunConfig::AsString(EMeasType measType)
{ //{{{
   switch (measType) {
   case kMEASTYPE_ALPHA:
	   return "alpha";
   case kMEASTYPE_SWEEP:
	   return "sweep";
   case kMEASTYPE_FIXED:
	   return "fixed";
   case kMEASTYPE_RAMP:
	   return "ramp";
   case kMEASTYPE_EMIT:
      return "emit";
   default:
      return "UNK";
   }
} //}}}


/** */
string RunConfig::AsString(ETargetOrient targetOrient)
{ //{{{
   switch (targetOrient) {
   case kTARGET_H:
	   return "H";
   case kTARGET_V:
	   return "V";
   default:
      return "UNK";
   }
} //}}}


/** */
string RunConfig::AsString(EBeamId beamId)
{ //{{{
   switch (beamId) {
   case kBLUE_BEAM:
	   return "BLU";
   case kYELLOW_BEAM:
	   return "YEL";
   default:
      return "UNK";
   }
} //}}}


/** */
string RunConfig::AsString(ERingId ringId)
{ //{{{
   switch (ringId) {
   case kBLUE_RING:
	   return "BLU";
   case kYELLOW_RING:
	   return "YEL";
   default:
      return "UNK";
   }
} //}}}


/** */
string RunConfig::AsString(EBeamEnergy beamEnergy)
{ //{{{
   switch (beamEnergy) {
   case kINJECTION:
	   return "024";
   case kBEAM_ENERGY_100:
	   return "100";
   case kFLATTOP:
	   return "250";
   default:
      return "UNK";
   }
} //}}}


/** */
string RunConfig::AsString(ESpinState spinState)
{ //{{{
   switch (spinState) {
   case kSPIN_DOWN:
	   return "down";
   case kSPIN_NULL:
	   return "null";
   case kSPIN_UP:
	   return "up";
   default:
      return "UNK";
   }
} //}}}


/** */
UShort_t RunConfig::AsIndex(ESpinState spinState)
{ //{{{
   switch (spinState) {
   case kSPIN_DOWN:
	   return 1;
   case kSPIN_NULL:
	   return 2;
   case kSPIN_UP:
	   return 0;
   default:
      return 3;
   }
} //}}}


/** */
Color_t RunConfig::AsColor(EBeamId beamId)
{ //{{{
   switch (beamId) {
   case kBLUE_BEAM:
	   return kBlue-4;
   case kYELLOW_BEAM:
	   return kOrange;
   default:
      return kBlack;
   }
} //}}}


/** */
Color_t RunConfig::AsColor(ERingId ringId)
{ //{{{
   switch (ringId) {
   case kBLUE_RING:
	   return kBlue-4;
   case kYELLOW_RING:
	   return kOrange;
   default:
      return kBlack;
   }
} //}}}


/** */
Color_t RunConfig::AsColor(EPolarimeterId polId)
{ //{{{
   switch (polId) {
   case kB1U:
   case kB2D:
	   return kBlue-4;
   case kY1D:
   case kY2U:
	   return kOrange;
   default:
      return kBlack;
   }
} //}}}


/** */
Color_t RunConfig::AsColor(ESpinState spin)
{ //{{{
   switch (spin) {
   case kSPIN_DOWN:
	   return kGreen;
   case kSPIN_NULL:
	   return kBlack;
   case kSPIN_UP:
	   return kRed;
   default:
      return kBlack;
   }
} //}}}


/** */
EBeamId RunConfig::GetBeamId(EPolarimeterId polId)
{ //{{{
   switch (polId) {
   case kB1U:
	   return kBLUE_BEAM;
   case kY1D:
	   return kYELLOW_BEAM;
   case kB2D:
	   return kBLUE_BEAM;
   case kY2U:
	   return kYELLOW_BEAM;
   default:
      return kUNKNOWN_BEAM;
   }
} //}}}


/** */
ERingId RunConfig::GetRingId(EPolarimeterId polId)
{ //{{{
   switch (polId) {
   case kB1U:
	   return kBLUE_RING;
   case kY1D:
	   return kYELLOW_RING;
   case kB2D:
	   return kBLUE_RING;
   case kY2U:
	   return kYELLOW_RING;
   default:
      return kUNKNOWN_RING;
   }
} //}}}


/** */
EStreamId RunConfig::GetStreamId(EPolarimeterId polId)
{ //{{{
   switch (polId) {
   case kB1U:
	   return kUPSTREAM;
   case kY1D:
	   return kDOWNSTREAM;
   case kB2D:
	   return kDOWNSTREAM;
   case kY2U:
	   return kUPSTREAM;
   default:
      return kUNKNOWN_STREAM;
   }
} //}}}


/** */
UShort_t RunConfig::GetDetectorId(UShort_t chId)
{ //{{{
   if (chId >= 1 && chId <= N_SILICON_CHANNELS) 
      return UShort_t( (chId - 1) / NSTRIP_PER_DETECTOR) + 1;
   
   gSystem->Error("   RunConfig::GetDetectorId", "Channel id is not valid");
   return USHRT_MAX;
} //}}}
