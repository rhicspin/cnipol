
#define EXPAND_MY_SSQLS_STATICS

#include "TSystem.h"

#include "MseFillProfileNew.h"


using namespace std;


/** */
MseFillProfileNewX::MseFillProfileNewX() : MseFillProfileNew()
{
   Init();
}


/** */
MseFillProfileNewX::MseFillProfileNewX(const mysqlpp::sql_int_unsigned &p1, const mysqlpp::sql_tinyint &p2, const mysqlpp::sql_tinyint &p3) : MseFillProfileNew(p1, p2, p3)
{
   Init();
   fill = p1;
   polarimeter_id = p2;
   target_orient = p3;
}


/** */
MseFillProfileNewX::MseFillProfileNewX(const mysqlpp::Row& row) : MseFillProfileNew(row) { }


/** */
MseFillProfileNewX::~MseFillProfileNewX()
{ }


/** */
void MseFillProfileNewX::Init()
{
   fill            = 0;
   polarimeter_id  = -1;
   target_orient   = -1;
   target_id       = -1;
   prof_r          = -1;
   prof_r_err      = -1;
   prof_r0         = -1;
   prof_r0_err     = -1;
   prof_rslope     = -1;
   prof_rslope_err = -1;
   prof_pmax       = -1;
   prof_pmax_err   = -1;
}


/** */
void MseFillProfileNewX::Print(const Option_t* opt) const
{
   gSystem->Info("MseFillProfileNewX::Print", "Print members:");
   PrintAsPhp();
}


/** */
void MseFillProfileNewX::PrintAsPhp(FILE *f) const
{
   fprintf(f, "$rc['fill']           = %d;\n", fill         );
   fprintf(f, "$rc['prof_r']         = %f;\n", prof_r       );
   fprintf(f, "$rc['prof_r_err       = %f;\n", prof_r_err   );
   fprintf(f, "$rc['prof_pmax']      = %f;\n", prof_pmax    );
   fprintf(f, "$rc['prof_pmax_err']  = %f;\n", prof_pmax_err);
   fprintf(f, "\n");
}


/** */
void MseFillProfileNewX::SetValues(const AnaFillResult &afr, EPolarimeterId polId, ETargetOrient tgtOrient)
{
   fill           = afr.GetFillId();
   polarimeter_id = polId;
   target_orient  = tgtOrient;
   target_id      = afr.GetPCTarget(polId).fTargetId;

   prof_r          = afr.GetPCProfR(polId, tgtOrient).first;
   prof_r_err      = afr.GetPCProfR(polId, tgtOrient).second;
   prof_r0         = afr.GetPCProfR0(polId, tgtOrient).first;
   prof_r0_err     = afr.GetPCProfR0(polId, tgtOrient).second;
   prof_rslope     = afr.GetPCProfRSlope(polId, tgtOrient).first;
   prof_rslope_err = afr.GetPCProfRSlope(polId, tgtOrient).second;
}
