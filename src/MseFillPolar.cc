
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
void MseFillPolarX::SetValues(AnaFillResult &fr)
{ //{{{
   start_time = mysqlpp::DateTime(fr.GetStartTime());

   if (fr.fHJPolars.find(kBLUE_RING) != fr.fHJPolars.end() ) {
      polar_blue_hjet     = fr.fHJPolars[kBLUE_RING].first; 
      polar_blue_hjet_err = fr.fHJPolars[kBLUE_RING].second; 
   }

   if (fr.fHJPolars.find(kYELLOW_RING) != fr.fHJPolars.end() ) {
      polar_yellow_hjet     = fr.fHJPolars[kYELLOW_RING].first; 
      polar_yellow_hjet_err = fr.fHJPolars[kYELLOW_RING].second; 
   }


   if (fr.fPCPolars.find(kB1U) != fr.fPCPolars.end() ) {
      polar_blue_1       = fr.fPCPolars[kB1U].first; 
      polar_blue_1_err   = fr.fPCPolars[kB1U].second; 
   }

   if (fr.fPCPolars.find(kY1D) != fr.fPCPolars.end() ) {
      polar_yellow_1     = fr.fPCPolars[kY1D].first; 
      polar_yellow_1_err = fr.fPCPolars[kY1D].second; 
   }

   if (fr.fPCPolars.find(kB2D) != fr.fPCPolars.end() ) {
      polar_blue_2       = fr.fPCPolars[kB2D].first; 
      polar_blue_2_err   = fr.fPCPolars[kB2D].second; 
   }

   if (fr.fPCPolars.find(kY2U) != fr.fPCPolars.end() ) {
      polar_yellow_2     = fr.fPCPolars[kY2U].first; 
      polar_yellow_2_err = fr.fPCPolars[kY2U].second; 
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
