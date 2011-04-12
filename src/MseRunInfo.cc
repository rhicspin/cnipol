
#define EXPAND_MY_SSQLS_STATICS

#include "MseRunInfo.h"

using namespace std;

MseRunInfoX::MseRunInfoX(const mysqlpp::sql_varchar &p1) : MseRunInfo(p1)
{
   Init();
}

void MseRunInfoX::Init()
{
   polarimeter_id       = -1;
   start_time           = mysqlpp::DateTime("0000-00-00 00:00:00");
   stop_time            = mysqlpp::DateTime("0000-00-00 00:00:00");
   beam_energy          = 0;
   nevents_total        = 0;
   nevents_processed    = 0;
   polarization         = 0;
   polarization_error   = 0;
   phase                = 0;
   phase_error          = 0;
   profile_ratio        = 0;
   profile_ratio_error  = 0;
   target_orient        = '-';
   target_id            = -1;
   asym_version         = "";
   alpha_calib_run_name = "";
   dl_calib_run_name    = "";
   measurement_type     = -1;
   disabled_channels    = "";
   disabled_bunches     = "";
   ana_start_time       = mysqlpp::DateTime("0000-00-00 00:00:00");
   ana_duration         = 0;
}


/** */
void MseRunInfoX::Print()
{
   cout << " " << run_name << endl;
   cout << "  " << polarimeter_id << endl;
   cout << "   " << start_time << endl;
   cout << "    " << stop_time << endl;
   cout << "     " << beam_energy << endl;
   cout << "     " << nevents_total << endl;
   cout << "     " << nevents_processed << endl;
   cout << "     " << polarization << endl;
   cout << "     " << polarization_error << endl;
   cout << "     " << phase << endl;
   cout << "     " << phase_error << endl;
   cout << "     " << profile_ratio << endl;
   cout << "     " << profile_ratio_error << endl;
   cout << "      " << target_orient << endl;
   cout << "       " << target_id << endl;
   cout << "       " << asym_version << endl;
   cout << "        " << alpha_calib_run_name << endl;
   cout << "         " << dl_calib_run_name << endl;
   cout << "         " << measurement_type << endl;
   cout << "         " << disabled_channels << endl;
   cout << "         " << disabled_bunches << endl;
   cout << "      " << ana_start_time << endl;
   cout << "      " << ana_duration << endl;
}
