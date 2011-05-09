
#define EXPAND_MY_SSQLS_STATICS

#include "MseRunPeriod.h"

using namespace std;

MseRunPeriodX::MseRunPeriodX(const mysqlpp::sql_datetime &p1) : MseRunPeriod(p1)
{
   Init();
}

void MseRunPeriodX::Init()
{
   start_time           = mysqlpp::DateTime("0000-00-00 00:00:00");
   polarimeter_id       = -1;
   alpha_calib_run_name = "";
   dl_calib_run_name    = "";
   disabled_channels    = "";
   disabled_bunches     = "";
   cut_proto_slope      = 0.;
   cut_proto_offset     = 0.;
}


/** */
void MseRunPeriodX::Print(const Option_t* opt) const
{
   PrintAsPhp();
}


/** */
void MseRunPeriodX::PrintAsPhp(FILE *f) const
{ //{{{
   fprintf(f, "$rc['start_time']                     = %d;\n", (int) time_t(start_time));
   fprintf(f, "$rc['polarimeter_id']                 = %d;\n", (short) polarimeter_id);
   fprintf(f, "$rc['alpha_calib_run_name']           = \"%s\";\n", string(alpha_calib_run_name).c_str());
   fprintf(f, "$rc['dl_calib_run_name']              = \"%s\";\n", dl_calib_run_name.data());
   fprintf(f, "$rc['disabled_channels']              = \"%s\";\n", disabled_channels.c_str());
   fprintf(f, "$rc['disabled_bunches']               = \"%s\";\n", disabled_bunches.c_str());
   fprintf(f, "$rc['cut_proto_slope']                = %f;\n", cut_proto_slope);
   fprintf(f, "$rc['cut_proto_offset']               = %f;\n", cut_proto_offset);
} //}}}
