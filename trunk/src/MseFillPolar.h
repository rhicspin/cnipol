#if !defined(EXPAND_MY_SSQLS_STATICS)
#   define MYSQLPP_SSQLS_NO_STATICS
#endif

#ifndef MseFillPolar_h
#define MseFillPolar_h

#include <stdio.h>

#include "mysql++.h"
#include "ssqls.h"

#include "TBuffer.h"
#include "TObject.h"

#include "RunConfig.h"
#include "AnaFillResult.h"


sql_create_15(MseFillPolar, 1, 2,
   mysqlpp::sql_int_unsigned,      fill,
   mysqlpp::sql_datetime,          start_time,
   mysqlpp::sql_float,             beam_energy,
   mysqlpp::sql_float,             polar_blue_hjet,
   mysqlpp::sql_float,             polar_blue_hjet_err,
   mysqlpp::sql_float,             polar_yellow_hjet,
   mysqlpp::sql_float,             polar_yellow_hjet_err,
   mysqlpp::sql_float,             polar_blue_1,
   mysqlpp::sql_float,             polar_blue_1_err,
   mysqlpp::sql_float,             polar_blue_2,
   mysqlpp::sql_float,             polar_blue_2_err,
   mysqlpp::sql_float,             polar_yellow_1,
   mysqlpp::sql_float,             polar_yellow_1_err,
   mysqlpp::sql_float,             polar_yellow_2,
   mysqlpp::sql_float,             polar_yellow_2_err)


class MseFillPolarX : public MseFillPolar
{
public:
   // default constructor[13]
   MseFillPolarX();
   ~MseFillPolarX();
 
   // for-comparison constructor[14]
   MseFillPolarX(const mysqlpp::sql_int_unsigned &p1);
 
   // population constructor[15]
   MseFillPolarX(const mysqlpp::Row& row) : MseFillPolar(row)
   { }
 
   void Init();
   void Print(const Option_t* opt="") const;
   void PrintAsPhp(FILE *f=stdout) const;
   void SetValues(AnaFillResult &fr);
   void Streamer(TBuffer &buf);
};

TBuffer & operator<<(TBuffer &buf, MseFillPolarX *&rec);
TBuffer & operator>>(TBuffer &buf, MseFillPolarX *&rec);

#endif
