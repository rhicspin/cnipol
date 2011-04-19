#if !defined(EXPAND_MY_SSQLS_STATICS)
#   define MYSQLPP_SSQLS_NO_STATICS
#endif

#ifndef MseRunPeriod_h
#define MseRunPeriod_h

#include <stdio.h>

#include "mysql++.h"
#include "ssqls.h"

#include "TObject.h"

sql_create_6(MseRunPeriod, 1, 2,
   mysqlpp::sql_datetime,          start_time,
   mysqlpp::sql_smallint,          polarimeter_id,
   mysqlpp::sql_varchar,           alpha_calib_run_name,
   mysqlpp::sql_varchar,           dl_calib_run_name,
   mysqlpp::sql_varchar,           disabled_channels,
   mysqlpp::sql_varchar,           disabled_bunches)


class MseRunPeriodX : public MseRunPeriod
{
public:
   // default constructor[13]
   MseRunPeriodX() : MseRunPeriod() { }
 
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
