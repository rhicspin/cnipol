
#include <string>

#include "AnaMeasResult.h"

ClassImp(AnaMeasResult)

using namespace std;


/** */
AnaMeasResult::AnaMeasResult() : TObject(),
   fStartTime(0),
   max_rate(0),
   TshiftAve(0),
   wcm_norm_event_rate(0), 
   UniversalRate(0),
   //A_N[2],
   //P[2],
   fAnaPower(),
   fPolar(),
   fAvrgPMAsym(),
   fFitResAsymPhi(),       fFitResPolarPhi(),
   fFitResAsymBunchX90(),  fFitResAsymBunchX45(), fFitResAsymBunchY45(),
   fAsymX90(), fAsymX45(), fAsymX45T(), fAsymX45B(), fAsymY45(),
   fFitResProfilePvsI(),   fFitResEnergySlope(), fFitResPseudoMass(),
   //P_sigma_ratio[2],
   //P_sigma_ratio_norm[2],
   //energy_slope[2],        // Slope for energy spectrum (detectors sum) [0]:slope [1]:error
   profile_error(0),          // profile error
   //sinphi[100+MAXDELIM],   // [0]: regular,  [1]: alternative sigma cut [3]: PHENIX bunch [4]:STAR bunch, 100... target pos
   //basym[100+MAXDELIM],    // [0]: regular,  [1]: alternative sigma cut [3]: PHENIX bunch [4]:STAR bunch; 100... target pos
   //anomaly(),
   //unrecog()
   fProfilePolarMax(),
   fProfilePolarR(),
   fPmtV1T0(0),
   fPmtV1T0Err(0),
   fPmtS1T0(0),
   fPmtS1T0Err(0)
{
   A_N[0] = 0; A_N[1] = 0;
   P[0]   = 0; P[1]   = 0;
   //memset(A_N, 0, 2);
   //memset(P, 0, 2);
   //memset(P_sigma_ratio, 0, 2);
   //memset(P_sigma_ratio_norm, 0, 2);
   //memset(energy_slope, 0, 2);

   for (int i=0; i!=100+MAXDELIM; i++) {
      StructSinPhi tmpSinPhi;
      sinphi[i] = tmpSinPhi;
      basym[i]  = tmpSinPhi;
   }
}


/** */
AnaMeasResult::~AnaMeasResult() { }


/** */
void AnaMeasResult::PrintAsPhp(FILE *f) const
{ //{{{
   fprintf(f, "$rc['max_rate']            = %f;\n",            max_rate);
   fprintf(f, "$rc['TshiftAve']           = %f;\n",            TshiftAve);
   fprintf(f, "$rc['wcm_norm_event_rate'] = %f;\n",            wcm_norm_event_rate);
   fprintf(f, "$rc['UniversalRate']       = %f;\n",            UniversalRate);
   fprintf(f, "$rc['A_N']                 = array(%f, %f);\n", A_N[0], A_N[1]);
   fprintf(f, "$rc['P']                   = array(%f, %f);\n", P[0], P[1]);
   fprintf(f, "$rc['fAnaPower']           = %s;\n",            PairAsPhpArray(fAnaPower).c_str());
   fprintf(f, "$rc['fPolar']              = %s;\n",            PairAsPhpArray(fPolar).c_str());
   fprintf(f, "$rc['fAvrgPMAsym']         = %s;\n",            PairAsPhpArray(fAvrgPMAsym).c_str());
   // Skip fit results fFitResAsymPhi fFitResPolarPhi ...
   fprintf(f, "$rc['fAsymX90']            = %s;\n",            MapAsPhpArray<string, ValErrPair>(fAsymX90).c_str());
   fprintf(f, "$rc['fAsymX45']            = %s;\n",            MapAsPhpArray<string, ValErrPair>(fAsymX45).c_str());
   fprintf(f, "$rc['fAsymX45T']           = %s;\n",            MapAsPhpArray<string, ValErrPair>(fAsymX45T).c_str());
   fprintf(f, "$rc['fAsymX45B']           = %s;\n",            MapAsPhpArray<string, ValErrPair>(fAsymX45B).c_str());
   fprintf(f, "$rc['fAsymY45']            = %s;\n",            MapAsPhpArray<string, ValErrPair>(fAsymY45).c_str());
   fprintf(f, "$rc['P_sigma_ratio']       = array(%f, %f);\n", P_sigma_ratio[0], P_sigma_ratio[1]);
   fprintf(f, "$rc['P_sigma_ratio_norm']  = array(%f, %f);\n", P_sigma_ratio_norm[0], P_sigma_ratio_norm[1]);
   fprintf(f, "$rc['energy_slope']        = array(%f, %f);\n", energy_slope[0], energy_slope[1]);
   fprintf(f, "$rc['profile_error']       = %f;\n",            profile_error);

   fprintf(f, "$rc['sinphi']              = array(");
   //for (int i=0; i!=100+MAXDELIM; i++)
   for (int i=0; i!=1; i++) {

      fprintf(f, "%d => array ('P' => %f, 'PErr' => %f, 'dPhi' => %f, 'dPhiErr' => %f, 'chi2' => %f)\n",
         i, sinphi[i].P[0], sinphi[i].P[1], sinphi[i].dPhi[0], sinphi[i].dPhi[1], sinphi[i].chi2);
   }

   fprintf(f, ");\n");
   fprintf(f, "$rc['fProfilePolarMax']    = %s;\n",            PairAsPhpArray(fProfilePolarMax).c_str());
   fprintf(f, "$rc['fProfilePolarR']      = %s;\n",            PairAsPhpArray(fProfilePolarR).c_str());

   fprintf(f, "$rc['fPmtV1T0']            = %f;\n",            fPmtV1T0);
   fprintf(f, "$rc['fPmtV1T0Err']         = %f;\n",            fPmtV1T0Err);
   fprintf(f, "$rc['fPmtS1T0']            = %f;\n",            fPmtS1T0);
   fprintf(f, "$rc['fPmtS1T0Err']         = %f;\n",            fPmtS1T0Err);

} //}}}


/** */
ValErrPair AnaMeasResult::GetPCPolar() const
{ //{{{
   ValErrPair val_err(0, -1);

   if (fFitResPolarPhi.Get()) {
      val_err.first  = fFitResPolarPhi->Value(0);
      val_err.second = fFitResPolarPhi->FitResult::Error(0);
   } else {
      Error("GetPCPolar", "No valid fit result found");
   }

   return val_err;
} //}}}
