#if !defined(EXPAND_MY_SSQLS_STATICS)
#   define MYSQLPP_SSQLS_NO_STATICS
#endif

#ifndef MseRunPeriod_h
#define MseRunPeriod_h

#include <stdio.h>

#include "mysql++.h"
#include "ssqls.h"

#include "TObject.h"

sql_create_18(MseRunPeriod, 1, 2,
   mysqlpp::sql_datetime,          start_time,
   mysqlpp::sql_smallint,          polarimeter_id,
   mysqlpp::sql_varchar,           alpha_calib_run_name,
   mysqlpp::sql_tinyint_unsigned,  alpha_source_count,
   mysqlpp::sql_varchar,           dl_calib_run_name,
   mysqlpp::sql_varchar,           disabled_channels,
   mysqlpp::sql_varchar,           disabled_bunches,
   mysqlpp::sql_float,             cut_proto_slope,
   mysqlpp::sql_float,             cut_proto_offset,
   mysqlpp::sql_tinyint_unsigned,  cut_proto_width,
   mysqlpp::sql_float,             cut_proto_adc_min,
   mysqlpp::sql_float,             cut_proto_adc_max,
   mysqlpp::sql_float,             cut_proto_tdc_min,
   mysqlpp::sql_float,             cut_proto_tdc_max,
   mysqlpp::sql_float,             cut_pulser_adc_min,
   mysqlpp::sql_float,             cut_pulser_adc_max,
   mysqlpp::sql_float,             cut_pulser_tdc_min,
   mysqlpp::sql_float,             cut_pulser_tdc_max)


class MseRunPeriodX : public MseRunPeriod
{
public:
   // default constructor[13]
   MseRunPeriodX();
 
   // for-comparison constructor[14]
   MseRunPeriodX(const mysqlpp::sql_datetime &p1);
 
   // full creation constructor
	MseRunPeriodX(const mysqlpp::sql_datetime &p1, const mysqlpp::sql_smallint &p2) :
      MseRunPeriod(p1, p2)
   { }
 
   // population constructor[15]
   MseRunPeriodX(const mysqlpp::Row& row) : MseRunPeriod(row)
   { }
 
   void Init();
   void Print(const Option_t* opt="") const;
   void PrintAsPhp(FILE *f=stdout) const;
};

#endif
