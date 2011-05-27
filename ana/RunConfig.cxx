
#include "RunConfig.h"

using namespace std;


/** */
RunConfig::RunConfig() : fPolarimeters(), fMeasTypes()
{
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

   fBeamEnergies.insert(kINJECTION);
   fBeamEnergies.insert(kBEAM_ENERGY_100);
   fBeamEnergies.insert(kFLATTOP);
}


/** */
RunConfig::~RunConfig()
{
}


/** */
string RunConfig::AsString(EPolarimeterId polId)
{
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
}


string RunConfig::AsString(EMeasType measType)
{
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
}


/** */
string RunConfig::AsString(ETargetOrient targetOrient)
{
   switch (targetOrient) {
   case kTARGET_H:
	   return "H";
   case kTARGET_V:
	   return "V";
   default:
      return "UNK";
   }
}


/** */
string RunConfig::AsString(EBeamEnergy beamEnergy)
{
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
}


Color_t RunConfig::AsColor(EPolarimeterId polId)
{
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
}
