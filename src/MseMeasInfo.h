#if !defined(EXPAND_MY_SSQLS_STATICS)
#   define MYSQLPP_SSQLS_NO_STATICS
#endif

#ifndef MseMeasInfo_h
#define MseMeasInfo_h

#include <stdio.h>

#include "mysql++.h"
#include "ssqls.h"

#include "TBuffer.h"
#include "TObject.h"

sql_create_23(MseMeasInfo, 1, 5,
   mysqlpp::sql_varchar,           run_name,
   mysqlpp::sql_smallint,          polarimeter_id,
   mysqlpp::sql_datetime,          start_time,
   mysqlpp::sql_datetime,          stop_time,
   mysqlpp::sql_float,             beam_energy,
   mysqlpp::sql_int_unsigned,      nevents_total,
   mysqlpp::sql_int_unsigned,      nevents_processed,
   mysqlpp::sql_float,             polarization,
   mysqlpp::sql_float,             polarization_error,
   mysqlpp::sql_float,             phase,
   mysqlpp::sql_float,             phase_error,
   mysqlpp::sql_float,             profile_ratio,
   mysqlpp::sql_float,             profile_ratio_error,
   mysqlpp::sql_char,              target_orient,
   mysqlpp::sql_smallint,          target_id,
   mysqlpp::sql_varchar,           asym_version,
   mysqlpp::sql_varchar,           alpha_calib_run_name,
   mysqlpp::sql_varchar,           dl_calib_run_name,
   mysqlpp::sql_smallint,          measurement_type,
   mysqlpp::sql_varchar,           disabled_channels,
   mysqlpp::sql_varchar,           disabled_bunches,
   mysqlpp::sql_datetime,          ana_start_time,
   mysqlpp::sql_int_unsigned,      ana_duration)


class MseMeasInfoX : public MseMeasInfo
{
public:
   // default constructor[13]
   MseMeasInfoX();
   ~MseMeasInfoX();
 
   // for-comparison constructor[14]
   MseMeasInfoX(const mysqlpp::sql_varchar &p1);
 
   // full creation constructor
	MseMeasInfoX(const mysqlpp::sql_varchar &p1, const mysqlpp::sql_smallint &p2,
               const mysqlpp::sql_datetime &p3, const mysqlpp::sql_datetime &p4,
               const mysqlpp::sql_float &p5) : MseMeasInfo(p1, p2, p3, p4, p5)
   { }
 
   // population constructor[15]
   MseMeasInfoX(const mysqlpp::Row& row) : MseMeasInfo(row)
   { }
 
   void Init();
   void Print(const Option_t* opt="") const;
   void PrintAsPhp(FILE *f=stdout) const;
   void Streamer(TBuffer &buf);
};

TBuffer & operator<<(TBuffer &buf, MseMeasInfoX *&rec);
TBuffer & operator>>(TBuffer &buf, MseMeasInfoX *&rec);

#endif
