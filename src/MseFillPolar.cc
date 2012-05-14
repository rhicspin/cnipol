
#define EXPAND_MY_SSQLS_STATICS

#include "TString.h"
#include "TSystem.h"

#include "MseFillPolar.h"


using namespace std;


/** */
MseFillPolarX::MseFillPolarX() : MseFillPolar()
{
   Init();
}


MseFillPolarX::~MseFillPolarX()
{
}


/** */
MseFillPolarX::MseFillPolarX(const mysqlpp::sql_int_unsigned &p1) : MseFillPolar(p1)
{
   Init();
	fill = p1;
}


void MseFillPolarX::Init()
{
   fill                  =  0; 
   start_time            = mysqlpp::DateTime("0000-00-00 00:00:00");
   type                  =  0;
   beam_energy           =  0; 
   polar_blue_hjet       = -1; 
   polar_blue_hjet_err   = -1; 
   polar_yellow_hjet     = -1; 
   polar_yellow_hjet_err = -1; 
   polar_blue_1          = -1; 
   polar_blue_1_err      = -1; 
   polar_blue_2          = -1; 
   polar_blue_2_err      = -1; 
   polar_yellow_1        = -1; 
   polar_yellow_1_err    = -1; 
   polar_yellow_2        = -1; 
   polar_yellow_2_err    = -1; 
}


/** */
void MseFillPolarX::Print(const Option_t* opt) const
{
   gSystem->Info("MseFillPolarX::Print", "Print members:");
   PrintAsPhp();
}


/** */
void MseFillPolarX::PrintAsPhp(FILE *f) const
{ //{{{
   fprintf(f, "$rc['fill']                    = %d;\n", fill              );
   fprintf(f, "$rc['type']                    = %d;\n", type              );
   fprintf(f, "$rc['beam_energy']             = %d;\n", beam_energy       );
   fprintf(f, "$rc['polar_blue_hjet']         = %f;\n", polar_blue_hjet        );
   fprintf(f, "$rc['polar_blue_hjet_err']     = %f;\n", polar_blue_hjet_err    );
   fprintf(f, "$rc['polar_yellow_hjet']       = %f;\n", polar_yellow_hjet      );
   fprintf(f, "$rc['polar_yellow_hjet_err']   = %f;\n", polar_yellow_hjet_err  );
   fprintf(f, "$rc['polar_blue_1']            = %f;\n", polar_blue_1      );
   fprintf(f, "$rc['polar_blue_1_err']        = %f;\n", polar_blue_1_err  );
   fprintf(f, "$rc['polar_blue_2']            = %f;\n", polar_blue_2      );
   fprintf(f, "$rc['polar_blue_2_err']        = %f;\n", polar_blue_2_err  );
   fprintf(f, "$rc['polar_yellow_1']          = %f;\n", polar_yellow_1    );
   fprintf(f, "$rc['polar_yellow_1_err']      = %f;\n", polar_yellow_1_err);
   fprintf(f, "$rc['polar_yellow_2']          = %f;\n", polar_yellow_2    );
   fprintf(f, "$rc['polar_yellow_2_err']      = %f;\n", polar_yellow_2_err);
   fprintf(f, "\n");
} //}}}


/** */
void MseFillPolarX::SetValues(AnaFillResult &afr)
{ //{{{
   start_time  = mysqlpp::DateTime(afr.GetStartTime());
   type        = afr.GetFillType();
   beam_energy = afr.GetFlattopEnergy();

   if (afr.fHJPolars.find(kBLUE_RING) != afr.fHJPolars.end() ) {
      polar_blue_hjet     = afr.fHJPolars[kBLUE_RING].first; 
      polar_blue_hjet_err = afr.fHJPolars[kBLUE_RING].second; 
   }

   if (afr.fHJPolars.find(kYELLOW_RING) != afr.fHJPolars.end() ) {
      polar_yellow_hjet     = afr.fHJPolars[kYELLOW_RING].first; 
      polar_yellow_hjet_err = afr.fHJPolars[kYELLOW_RING].second; 
   }


   if (afr.fPCPolars.find(kB1U) != afr.fPCPolars.end() ) {
      polar_blue_1       = afr.fPCPolars[kB1U].first; 
      polar_blue_1_err   = afr.fPCPolars[kB1U].second; 
   }

   if (afr.fPCPolars.find(kY1D) != afr.fPCPolars.end() ) {
      polar_yellow_1     = afr.fPCPolars[kY1D].first; 
      polar_yellow_1_err = afr.fPCPolars[kY1D].second; 
   }

   if (afr.fPCPolars.find(kB2D) != afr.fPCPolars.end() ) {
      polar_blue_2       = afr.fPCPolars[kB2D].first; 
      polar_blue_2_err   = afr.fPCPolars[kB2D].second; 
   }

   if (afr.fPCPolars.find(kY2U) != afr.fPCPolars.end() ) {
      polar_yellow_2     = afr.fPCPolars[kY2U].first; 
      polar_yellow_2_err = afr.fPCPolars[kY2U].second; 
   }

} //}}}


/** */
void MseFillPolarX::Streamer(TBuffer &buf)
{
   //TString tstr;
   //short   smallint;
   //time_t  tmp_time_t;

   if (buf.IsReading()) {
   } else {
   }
}


/** */
TBuffer & operator<<(TBuffer &buf, MseFillPolarX *&rec)
{
   if (!rec) return buf;
   //printf("operator<<(TBuffer &buf, AnaInfo *rec) : \n");
   rec->Streamer(buf);
   return buf;
}


/** */
TBuffer & operator>>(TBuffer &buf, MseFillPolarX *&rec)
{
   //printf("operator<<(TBuffer &buf, AnaInfo *rec) : \n");
   rec->Streamer(buf);
   return buf;
}
