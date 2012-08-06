
#define EXPAND_MY_SSQLS_STATICS

#include "TSystem.h"

#include "MseFillPolarNew.h"


using namespace std;


/** */
MseFillPolarNewX::MseFillPolarNewX() : MseFillPolarNew()
{
   Init();
}


/** */
MseFillPolarNewX::MseFillPolarNewX(const mysqlpp::sql_int_unsigned &p1, const mysqlpp::sql_tinyint &p2, const mysqlpp::sql_tinyint &p3) : MseFillPolarNew(p1, p2, p3)
{
   Init();
   fill = p1;
   polarimeter_id = p2;
   ring_id = p3;
}


/** */
MseFillPolarNewX::MseFillPolarNewX(const mysqlpp::Row& row) : MseFillPolarNew(row) { }


/** */
MseFillPolarNewX::~MseFillPolarNewX()
{ }


/** */
void MseFillPolarNewX::Init()
{ //{{{
   fill            = 0;
   polarimeter_id  = -1;
   ring_id         = -1;
   start_time      = mysqlpp::DateTime("0000-00-00 00:00:00");
   type            = -1;
   beam_energy     = 0;
   polar           = -1;
   polar_err       = -1;
   polar_p0        = -1;
   polar_p0_err    = -1;
   polar_slope     = -1;
   polar_slope_err = -1;
} //}}}


/** */
void MseFillPolarNewX::Print(const Option_t* opt) const
{ //{{{
   gSystem->Info("MseFillPolarNewX::Print", "Print members:");
   PrintAsPhp();
} //}}}


/** */
void MseFillPolarNewX::PrintAsPhp(FILE *f) const
{ //{{{
   fprintf(f, "$rc['fill']                    = %d;\n", fill           );
   fprintf(f, "$rc['polarimeter_id']          = %d;\n", (int) polarimeter_id );
   fprintf(f, "$rc['ring_id']                 = %d;\n", (int) ring_id        );
   //fprintf(f, "$rc['start_time']              = %d;\n", start_time     );
   fprintf(f, "$rc['type']                    = %d;\n", type           );
   fprintf(f, "$rc['beam_energy']             = %f;\n", beam_energy    );
   fprintf(f, "$rc['polar']                   = %f;\n", polar          );
   fprintf(f, "$rc['polar_err']               = %f;\n", polar_err      );
   fprintf(f, "$rc['polar_p0']                = %f;\n", polar_p0       );
   fprintf(f, "$rc['polar_p0_err']            = %f;\n", polar_p0_err   );
   fprintf(f, "$rc['polar_slope']             = %f;\n", polar_slope    );
   fprintf(f, "$rc['polar_slope_err']         = %f;\n", polar_slope_err);
   fprintf(f, "\n");
} //}}}


/** */
void MseFillPolarNewX::SetValues(const AnaFillResult &afr)
{ //{{{
   fill        = afr.GetFillId();
   start_time  = mysqlpp::DateTime(afr.GetLumiOnTime());
   type        = afr.GetFillType();
   beam_energy = afr.GetFlattopEnergy();
} //}}}


/** */
void MseFillPolarNewX::SetValuesPC(const AnaFillResult &afr, EPolarimeterId polId)
{ //{{{
   SetValues(afr);

   polarimeter_id = polId;
   ring_id        = RunConfig::GetRingId(polId);

   ValErrPair ve;

   ve = afr.GetPCPolar(polId);
   polar     = ve.first;
   polar_err = ve.second;

   ve = afr.GetPCPolarP0(polId);
   polar_p0     = ve.first;
   polar_p0_err = ve.second;

   ve = afr.GetPCPolarSlope(polId);
   polar_slope     = ve.first;
   polar_slope_err = ve.second;
} //}}}


/** */
void MseFillPolarNewX::SetValuesHJ(const AnaFillResult &afr, ERingId ringId)
{ //{{{
   SetValues(afr);

   polarimeter_id = kHJET;
   ring_id        = ringId;

   ValErrPair ve = afr.GetHJPolar(ringId);
   polar     = ve.first;
   polar_err = ve.second;
} //}}}
