
#define EXPAND_MY_SSQLS_STATICS

#include "TString.h"
#include "TSystem.h"

#include "MseMeasInfo.h"


using namespace std;


/** */
MseMeasInfoX::MseMeasInfoX() : MseMeasInfo()
{
   Init();
}


MseMeasInfoX::~MseMeasInfoX()
{
}


/** */
MseMeasInfoX::MseMeasInfoX(const mysqlpp::sql_varchar &p1) : MseMeasInfo(p1)
{
   Init();
}

void MseMeasInfoX::Init()
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
void MseMeasInfoX::Print(const Option_t* opt) const
{
   gSystem->Info("MseMeasInfoX::Print", "Print members:");
   PrintAsPhp();
}


/** */
void MseMeasInfoX::PrintAsPhp(FILE *f) const
{ //{{{
   fprintf(f, "$rc['run_name']                       = \"%s\";\n", run_name.c_str()          );
   fprintf(f, "$rc['polarimeter_id']                 = %d;\n", (short) polarimeter_id    );
   fprintf(f, "$rc['start_time']                     = %d;\n", (int) time_t(start_time)        );
   fprintf(f, "$rc['stop_time']                      = %d;\n", (int) time_t(stop_time)         );
   fprintf(f, "$rc['beam_energy']                    = %f;\n", beam_energy       );
   fprintf(f, "$rc['nevents_total']                  = %d;\n", nevents_total     );
   fprintf(f, "$rc['nevents_processed']              = %d;\n", nevents_processed );
   fprintf(f, "$rc['polarization']                   = %f;\n", polarization      );
   fprintf(f, "$rc['polarization_error']             = %f;\n", polarization_error);
   fprintf(f, "$rc['phase']                          = %f;\n", phase             );
   fprintf(f, "$rc['phase_error']                    = %f;\n", phase_error       );
   fprintf(f, "$rc['profile_ratio']                  = %f;\n", profile_ratio     );
   fprintf(f, "$rc['profile_ratio_error']            = %f;\n", profile_ratio_error);
   fprintf(f, "$rc['target_orient']                  = \"%s\";\n", target_orient.c_str()     );
   fprintf(f, "$rc['target_id']                      = %d;\n", target_id         );
   fprintf(f, "$rc['asym_version']                   = \"%s\";\n", asym_version.c_str()      );
   //cout << "|" << alpha_calib_run_name << "|" << endl;
   fprintf(f, "$rc['alpha_calib_run_name']           = \"%s\";\n", string(alpha_calib_run_name).c_str());
   fprintf(f, "$rc['dl_calib_run_name']              = \"%s\";\n", dl_calib_run_name.data() );
   fprintf(f, "$rc['measurement_type']               = %d;\n", measurement_type  );
   fprintf(f, "$rc['disabled_channels']              = \"%s\";\n", disabled_channels.c_str() );
   fprintf(f, "$rc['disabled_bunches']               = \"%s\";\n", disabled_bunches.c_str()  );
   fprintf(f, "$rc['ana_start_time']                 = %d;\n", (int) time_t(ana_start_time)    );
   fprintf(f, "$rc['ana_duration']                   = %d;\n", ana_duration      );
   fprintf(f, "\n");
} //}}}


/** */
void MseMeasInfoX::Streamer(TBuffer &buf)
{ //{{{
   TString tstr;
   //short   smallint;
   time_t  tmp_time_t;

   if (buf.IsReading()) {
      //printf("reading AnaInfo::Streamer(TBuffer &buf) \n");
      buf >> tstr; run_name = tstr.Data();
      buf >> polarimeter_id; 
      buf >> tmp_time_t; start_time = mysqlpp::sql_datetime(tmp_time_t); 
      buf >> tmp_time_t; stop_time = mysqlpp::sql_datetime(tmp_time_t); 
      buf >> beam_energy; 
      buf >> nevents_total; 
      buf >> nevents_processed; 
      buf >> tstr; target_orient = tstr.Data();
      buf >> target_id;
      //buf >> procDateTime >> procTimeReal >> procTimeCpu;
      //buf >> tstr; fAlphaCalibRun = tstr.Data();
      //buf >> tstr; fDlCalibRun = tstr.Data();
   } else {
      //printf("writing AnaInfo::Streamer(TBuffer &buf) \n");
      tstr = run_name; buf << tstr;
      buf << polarimeter_id;
      buf << time_t(start_time); 
      buf << time_t(stop_time); 
      buf << beam_energy; 
      buf << nevents_total; 
      buf << nevents_processed; 
      tstr = target_orient; buf << tstr;
      //buf << (Char_t) target_orient;
      buf << target_id;
      //buf << procDateTime << procTimeReal << procTimeCpu;
      //tstr = fAlphaCalibRun; buf << tstr;
      //tstr = fDlCalibRun;    buf << tstr;
   }
} //}}}


/** */
TBuffer & operator<<(TBuffer &buf, MseMeasInfoX *&rec)
{
   if (!rec) return buf;
   //printf("operator<<(TBuffer &buf, AnaInfo *rec) : \n");
   rec->Streamer(buf);
   return buf;
}


/** */
TBuffer & operator>>(TBuffer &buf, MseMeasInfoX *&rec)
{
   //printf("operator<<(TBuffer &buf, AnaInfo *rec) : \n");
   rec->Streamer(buf);
   return buf;
}
