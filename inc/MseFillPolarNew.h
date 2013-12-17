#if !defined(EXPAND_MY_SSQLS_STATICS)
#   define MYSQLPP_SSQLS_NO_STATICS
#endif

#ifndef MseFillPolarNew_h
#define MseFillPolarNew_h

#include <stdio.h>
#include <set>

#include "mysql++.h"
#include "ssqls.h"

#include "RunConfig.h"
#include "AnaFillResult.h"


class MseFillPolarNewX;

typedef std::set<MseFillPolarNewX>            MseFillPolarNewXSet;
typedef MseFillPolarNewXSet::iterator         MseFillPolarNewXSetIter;
typedef MseFillPolarNewXSet::const_iterator   MseFillPolarNewXSetConstIter;


sql_create_12(MseFillPolarNew, 3, 0,
   mysqlpp::sql_int_unsigned, fill,
   mysqlpp::sql_tinyint,      polarimeter_id,
   mysqlpp::sql_tinyint,      ring_id,
   mysqlpp::sql_datetime,     start_time,
   mysqlpp::sql_smallint,     type,
   mysqlpp::sql_float,        beam_energy,
   mysqlpp::sql_float,        polar,
   mysqlpp::sql_float,        polar_err,
   mysqlpp::sql_float,        polar_p0,
   mysqlpp::sql_float,        polar_p0_err,
   mysqlpp::sql_float,        polar_slope,
   mysqlpp::sql_float,        polar_slope_err)


class MseFillPolarNewX : public MseFillPolarNew
{
public:

   MseFillPolarNewX();
   MseFillPolarNewX(const mysqlpp::sql_int_unsigned &p1, const mysqlpp::sql_tinyint &p2, const mysqlpp::sql_tinyint &p3); // for-comparison constructor
   MseFillPolarNewX(const mysqlpp::Row& row); // population constructor

   ~MseFillPolarNewX();
 
   void Init();
   void Print(const Option_t* opt="") const;
   void PrintAsPhp(FILE *f=stdout) const;
   void SetValues(const AnaFillResult &afr);
   void SetValuesPC(const AnaFillResult &afr, EPolarimeterId polId);
   void SetValuesHJ(const AnaFillResult &afr, ERingId ringId);
};

#endif
