
#define EXPAND_MY_SSQLS_STATICS

#include "TString.h"
#include "TSystem.h"

#include "MseFillProfile.h"


using namespace std;


/** */
MseFillProfileX::MseFillProfileX() : MseFillProfile()
{
   Init();
}


MseFillProfileX::~MseFillProfileX()
{
}


/** */
MseFillProfileX::MseFillProfileX(const mysqlpp::sql_int_unsigned &p1) : MseFillProfile(p1)
{
   Init();
	fill = p1;
}


void MseFillProfileX::Init()
{
   fill                 = 0;
   profile_blue_h       = -1;
   profile_blue_h_err   = -1;
   profile_blue_v       = -1;
   profile_blue_v_err   = -1;
   profile_yellow_h     = -1;
   profile_yellow_h_err = -1;
   profile_yellow_v     = -1;
   profile_yellow_v_err = -1;
   pmax_blue_h          = -1;
   pmax_blue_h_err      = -1;
   pmax_blue_v          = -1;
   pmax_blue_v_err      = -1;
   pmax_yellow_h        = -1;
   pmax_yellow_h_err    = -1;
   pmax_yellow_v        = -1;
   pmax_yellow_v_err    = -1;
}


/** */
void MseFillProfileX::Print(const Option_t* opt) const
{
   gSystem->Info("MseFillProfileX::Print", "Print members:");
   PrintAsPhp();
}


/** */
void MseFillProfileX::PrintAsPhp(FILE *f) const
{ //{{{
   fprintf(f, "$rc['fill']                    = %d;\n", fill              );
   fprintf(f, "$rc['profile_blue_h']          = %f;\n", profile_blue_h    );
   fprintf(f, "$rc['profile_blue_v']          = %f;\n", profile_blue_v    );
   fprintf(f, "$rc['profile_yellow_h']        = %f;\n", profile_yellow_h  );
   fprintf(f, "$rc['profile_yellow_v']        = %f;\n", profile_yellow_v  );
   fprintf(f, "\n");
} //}}}


/** */
void MseFillProfileX::SetValues(AnaFillResult &fr)
{ //{{{

   if (fr.fPolProfRs.find(kBLUE_RING) != fr.fPolProfRs.end() ) {

      TgtOrient2ValErrMap mapByTgt = fr.fPolProfRs[kBLUE_RING];

      if (mapByTgt.find(kTARGET_H) != mapByTgt.end() ) {

         profile_blue_h     = fr.fPolProfRs[kBLUE_RING][kTARGET_H].first;     //iTgtRes->second.first; 
         profile_blue_h_err = fr.fPolProfRs[kBLUE_RING][kTARGET_H].second;    //iTgtRes->second.second; 
         pmax_blue_h        = fr.fPolProfPMaxs[kBLUE_RING][kTARGET_H].first;
         pmax_blue_h_err    = fr.fPolProfPMaxs[kBLUE_RING][kTARGET_H].second;
      }

      if (mapByTgt.find(kTARGET_V) != mapByTgt.end() ) {

         profile_blue_v     = fr.fPolProfRs[kBLUE_RING][kTARGET_V].first;     //iTgtRes->second.first; 
         profile_blue_v_err = fr.fPolProfRs[kBLUE_RING][kTARGET_V].second;    //iTgtRes->second.second; 
         pmax_blue_v        = fr.fPolProfPMaxs[kBLUE_RING][kTARGET_V].first;
         pmax_blue_v_err    = fr.fPolProfPMaxs[kBLUE_RING][kTARGET_V].second;
      }
   }

   if (fr.fPolProfRs.find(kYELLOW_RING) != fr.fPolProfRs.end() ) {

      TgtOrient2ValErrMap mapByTgt = fr.fPolProfRs[kYELLOW_RING];

      if (mapByTgt.find(kTARGET_H) != mapByTgt.end() ) {

         profile_yellow_h     = fr.fPolProfRs[kYELLOW_RING][kTARGET_H].first;     //iTgtRes->second.first; 
         profile_yellow_h_err = fr.fPolProfRs[kYELLOW_RING][kTARGET_H].second;    //iTgtRes->second.second; 
         pmax_yellow_h        = fr.fPolProfPMaxs[kYELLOW_RING][kTARGET_H].first;
         pmax_yellow_h_err    = fr.fPolProfPMaxs[kYELLOW_RING][kTARGET_H].second;
      }

      if (mapByTgt.find(kTARGET_V) != mapByTgt.end() ) {

         profile_yellow_v     = fr.fPolProfRs[kYELLOW_RING][kTARGET_V].first;     //iTgtRes->second.first; 
         profile_yellow_v_err = fr.fPolProfRs[kYELLOW_RING][kTARGET_V].second;    //iTgtRes->second.second; 
         pmax_yellow_v        = fr.fPolProfPMaxs[kYELLOW_RING][kTARGET_V].first;
         pmax_yellow_v_err    = fr.fPolProfPMaxs[kYELLOW_RING][kTARGET_V].second;
      }
   }
} //}}}


/** */
void MseFillProfileX::Streamer(TBuffer &buf)
{
   //TString tstr;
   //short   smallint;
   //time_t  tmp_time_t;

   if (buf.IsReading()) {
   } else {
   }
}


/** */
TBuffer & operator<<(TBuffer &buf, MseFillProfileX *&rec)
{
   if (!rec) return buf;
   //printf("operator<<(TBuffer &buf, AnaInfo *rec) : \n");
   rec->Streamer(buf);
   return buf;
}


/** */
TBuffer & operator>>(TBuffer &buf, MseFillProfileX *&rec)
{
   //printf("operator<<(TBuffer &buf, AnaInfo *rec) : \n");
   rec->Streamer(buf);
   return buf;
}
