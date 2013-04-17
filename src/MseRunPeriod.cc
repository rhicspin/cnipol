
#define EXPAND_MY_SSQLS_STATICS

#include "TSystem.h"

#include "MseRunPeriod.h"

using namespace std;


MseRunPeriodX::MseRunPeriodX() : MseRunPeriod()
{
   Init();
}


MseRunPeriodX::MseRunPeriodX(const mysqlpp::sql_datetime &p1) : MseRunPeriod(p1)
{
   Init();
}

void MseRunPeriodX::Init()
{
   start_time           = mysqlpp::DateTime("0000-00-00 00:00:00");
   polarimeter_id       = -1;
   alpha_calib_run_name = "";
   alpha_source_count   = 0;
   dl_calib_run_name    = "";
   disabled_channels    = "";
   disabled_bunches     = "";
   cut_proto_slope      = -0.25;
   cut_proto_offset     = 75;
   cut_proto_width      = 20;
   cut_proto_adc_min    = 0;
   cut_proto_adc_max    = 255;
   cut_proto_tdc_min    = 0;
   cut_proto_tdc_max    = 255;
   cut_pulser_adc_min   = 255;
   cut_pulser_adc_max   = 0;
   cut_pulser_tdc_min   = 255;
   cut_pulser_tdc_max   = 0;
}


/** */
void MseRunPeriodX::Print(const Option_t* opt) const
{
   gSystem->Info("MseRunPeriodX::Print", "Print members:");
   PrintAsPhp();
}


/** */
void MseRunPeriodX::PrintAsPhp(FILE *f) const
{
   fprintf(f, "$rc['start_time']                     = %d;\n", (int) time_t(start_time));
   fprintf(f, "$rc['polarimeter_id']                 = %d;\n", (short) polarimeter_id);
   fprintf(f, "$rc['alpha_calib_run_name']           = \"%s\";\n", string(alpha_calib_run_name).c_str());
   fprintf(f, "$rc['alpha_source_count']             = %d;\n",     (unsigned char) alpha_source_count);
   fprintf(f, "$rc['dl_calib_run_name']              = \"%s\";\n", dl_calib_run_name.data());
   fprintf(f, "$rc['disabled_channels']              = \"%s\";\n", disabled_channels.c_str());
   fprintf(f, "$rc['disabled_bunches']               = \"%s\";\n", disabled_bunches.c_str());
   fprintf(f, "$rc['cut_proto_slope']                = %f;\n", cut_proto_slope);
   fprintf(f, "$rc['cut_proto_offset']               = %f;\n", cut_proto_offset);
   fprintf(f, "$rc['cut_proto_width']                = %d;\n", (unsigned char) cut_proto_width);
   fprintf(f, "$rc['cut_proto_adc_min']              = %f;\n", cut_proto_adc_min);
   fprintf(f, "$rc['cut_proto_adc_max']              = %f;\n", cut_proto_adc_max);
   fprintf(f, "$rc['cut_proto_tdc_min']              = %f;\n", cut_proto_tdc_min);
   fprintf(f, "$rc['cut_proto_tdc_max']              = %f;\n", cut_proto_tdc_max);
   fprintf(f, "$rc['cut_pulser_adc_min']             = %f;\n", cut_pulser_adc_min);
   fprintf(f, "$rc['cut_pulser_adc_max']             = %f;\n", cut_pulser_adc_max);
   fprintf(f, "$rc['cut_pulser_tdc_min']             = %f;\n", cut_pulser_tdc_min);
   fprintf(f, "$rc['cut_pulser_tdc_max']             = %f;\n", cut_pulser_tdc_max);
   fprintf(f, "\n");
}
