#if !defined(EXPAND_MY_SSQLS_STATICS)
#   define MYSQLPP_SSQLS_NO_STATICS
#endif

#ifndef MseRunInfo_h
#define MseRunInfo_h

#include "mysql++.h"
#include "ssqls.h"

sql_create_5(MseRunInfo, 1, 5,
   mysqlpp::sql_varchar,  run_name,
   mysqlpp::sql_tinyint,  polarimeter_id,
   mysqlpp::sql_datetime, start_time,
   mysqlpp::sql_datetime, stop_time,
   mysqlpp::sql_float,    beam_energy)

#endif
