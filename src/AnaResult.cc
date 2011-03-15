
#include "AnaResult.h"


/** */
StructAnalysis::StructAnalysis() :
   max_rate(0),
   //TshiftAve(0),
   //wcm_norm_event_rate(0), 
   //UniversalRate(0),
   //A_N[2],
   //P[2],
   //P_sigma_ratio[2],
   //P_sigma_ratio_norm[2],
   //energy_slope[2],        // Slope for energy spectrum (detectors sum) [0]:slope [1]:error
   profile_error(0),          // profile error
   //sinphi[100+MAXDELIM],   // [0]: regular,  [1]: alternative sigma cut [3]: PHENIX bunch [4]:STAR bunch, 100... target pos
   //basym[100+MAXDELIM],    // [0]: regular,  [1]: alternative sigma cut [3]: PHENIX bunch [4]:STAR bunch; 100... target pos
   //anomaly(),
   //unrecog()
   fIntensPolarR(0),
   fIntensPolarRErr(0)
{
   //memset(A_N, 0, 2);
   //memset(P, 0, 2);
   //memset(P_sigma_ratio, 0, 2);
   //memset(P_sigma_ratio_norm, 0, 2);
   //memset(energy_slope, 0, 2);

   //for (int i=0; i!=100+MAXDELIM; i++) {
   //   StructSinPhi tmpSinPhi;
   //   sinphi[i] = tmpSinPhi;
   //   basym[i]  = tmpSinPhi;
   //}
}


/** */
StructAnalysis::~StructAnalysis() { }


/** */
TBuffer & operator<<(TBuffer &buf, StructAnalysis *&rec)
{
   if (!rec) return buf;
   //printf("operator<<(TBuffer &buf, StructAnalysis *rec) : \n");
   rec->Streamer(buf);
   return buf;
}


/** */
TBuffer & operator>>(TBuffer &buf, StructAnalysis *&rec)
{
   //printf("operator>>(TBuffer &buf, StructAnalysis *rec) : \n");
   rec->Streamer(buf);
   return buf;
}


/** */
void StructAnalysis::Streamer(TBuffer &buf)
{
   if (buf.IsReading()) {
      //printf("reading StructAnalysis::Streamer(TBuffer &buf) \n");
      buf >> max_rate;
      buf >> TshiftAve;
      buf >> wcm_norm_event_rate; 
      buf >> UniversalRate;
      buf.ReadFastArray(A_N, 2);
      buf.ReadFastArray(P, 2);
      buf.ReadFastArray(P_sigma_ratio, 2);
      buf.ReadFastArray(P_sigma_ratio_norm, 2);
      buf.ReadFastArray(energy_slope, 2);
      buf >> profile_error;

      for (int i=0; i!=100+MAXDELIM; i++) {
         buf >> sinphi[i].P[0]    >> sinphi[i].P[1];
         buf >> sinphi[i].dPhi[0] >> sinphi[i].dPhi[1];
         buf >> sinphi[i].chi2;
      }

      buf >> fIntensPolarR;
      buf >> fIntensPolarRErr;

   } else {
      //printf("writing StructAnalysis::Streamer(TBuffer &buf) \n");
      buf << max_rate;
      buf << TshiftAve;
      buf << wcm_norm_event_rate; 
      buf << UniversalRate;
      buf.WriteFastArray(A_N, 2);
      buf.WriteFastArray(P, 2);
      buf.WriteFastArray(P_sigma_ratio, 2);
      buf.WriteFastArray(P_sigma_ratio_norm, 2);
      buf.WriteFastArray(energy_slope, 2);
      buf << profile_error;

      for (int i=0; i!=100+MAXDELIM; i++) {
         buf << sinphi[i].P[0]    << sinphi[i].P[1];
         buf << sinphi[i].dPhi[0] << sinphi[i].dPhi[1];
         buf << sinphi[i].chi2;
      }

      buf << fIntensPolarR;
      buf << fIntensPolarRErr;
   }
}


/** */
void StructAnalysis::PrintAsPhp(FILE *f) const
{ //{{{
   fprintf(f, "$rc['max_rate']            = %f;\n",            max_rate);
   fprintf(f, "$rc['TshiftAve']           = %f;\n",            TshiftAve);
   fprintf(f, "$rc['wcm_norm_event_rate'] = %f;\n",            wcm_norm_event_rate);
   fprintf(f, "$rc['UniversalRate']       = %f;\n",            UniversalRate);
   fprintf(f, "$rc['A_N']                 = array(%f, %f);\n", A_N[0], A_N[1]);
   fprintf(f, "$rc['P']                   = array(%f, %f);\n", P[0], P[1]);
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
   fprintf(f, "$rc['fIntensPolarR']       = %f;\n",            fIntensPolarR);
   fprintf(f, "$rc['fIntensPolarRErr']    = %f;\n",            fIntensPolarRErr);

} //}}}
