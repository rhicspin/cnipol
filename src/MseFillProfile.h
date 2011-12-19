#if !defined(EXPAND_MY_SSQLS_STATICS)
#   define MYSQLPP_SSQLS_NO_STATICS
#endif

#ifndef MseFillProfile_h
#define MseFillProfile_h

#include <stdio.h>

#include "mysql++.h"
#include "ssqls.h"

#include "TBuffer.h"
#include "TObject.h"

#include "RunConfig.h"
#include "AnaFillResult.h"


sql_create_17(MseFillProfile, 1, 2,
   mysqlpp::sql_int_unsigned,      fill,
   mysqlpp::sql_float,             profile_blue_h,
   mysqlpp::sql_float,             profile_blue_h_err,
   mysqlpp::sql_float,             profile_blue_v,
   mysqlpp::sql_float,             profile_blue_v_err,
   mysqlpp::sql_float,             profile_yellow_h,
   mysqlpp::sql_float,             profile_yellow_h_err,
   mysqlpp::sql_float,             profile_yellow_v,
   mysqlpp::sql_float,             profile_yellow_v_err,
   mysqlpp::sql_float,             pmax_blue_h,
   mysqlpp::sql_float,             pmax_blue_h_err,
   mysqlpp::sql_float,             pmax_blue_v,
   mysqlpp::sql_float,             pmax_blue_v_err,
   mysqlpp::sql_float,             pmax_yellow_h,
   mysqlpp::sql_float,             pmax_yellow_h_err,
   mysqlpp::sql_float,             pmax_yellow_v,
   mysqlpp::sql_float,             pmax_yellow_v_err)


class MseFillProfileX : public MseFillProfile
{
public:
   // default constructor[13]
   MseFillProfileX();
   ~MseFillProfileX();
 
   // for-comparison constructor[14]
   MseFillProfileX(const mysqlpp::sql_int_unsigned &p1);
 
   // population constructor[15]
   MseFillProfileX(const mysqlpp::Row& row) : MseFillProfile(row)
   { }
 
   void Init();
   void Print(const Option_t* opt="") const;
   void PrintAsPhp(FILE *f=stdout) const;
   void SetValues(AnaFillResult &fr);
   void Streamer(TBuffer &buf);
};

TBuffer & operator<<(TBuffer &buf, MseFillProfileX *&rec);
TBuffer & operator>>(TBuffer &buf, MseFillProfileX *&rec);

#endif
