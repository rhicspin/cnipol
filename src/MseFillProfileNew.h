#if !defined(EXPAND_MY_SSQLS_STATICS)
#   define MYSQLPP_SSQLS_NO_STATICS
#endif

#ifndef MseFillProfileNew_h
#define MseFillProfileNew_h

#include <stdio.h>

#include "mysql++.h"
#include "ssqls.h"

#include "TObject.h"

#include "RunConfig.h"
#include "AnaFillResult.h"


sql_create_8(MseFillProfileNew, 3, 0,
   mysqlpp::sql_int_unsigned, fill,
   mysqlpp::sql_tinyint,      polarimeter_id,
   mysqlpp::sql_tinyint,      target_orient,
   mysqlpp::sql_tinyint,      target_id,
   mysqlpp::sql_float,        prof_r,
   mysqlpp::sql_float,        prof_r_err,
   mysqlpp::sql_float,        prof_pmax,
   mysqlpp::sql_float,        prof_pmax_err)


class MseFillProfileNewX : public MseFillProfileNew
{
public:

   MseFillProfileNewX();
   MseFillProfileNewX(const mysqlpp::sql_int_unsigned &p1, const mysqlpp::sql_tinyint &p2, const mysqlpp::sql_tinyint &p3); // for-comparison constructor
   MseFillProfileNewX(const mysqlpp::Row& row); // population constructor

   ~MseFillProfileNewX();
 
   void Init();
   void Print(const Option_t* opt="") const;
   void PrintAsPhp(FILE *f=stdout) const;
   void SetValues(const AnaFillResult &fr, EPolarimeterId polId, ETargetOrient tgtOrient);
};

#endif
