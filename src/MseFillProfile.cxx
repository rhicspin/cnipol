
#define EXPAND_MY_SSQLS_STATICS

#include "TString.h"
#include "TSystem.h"

#include "MseFillProfile.h"


using namespace std;


/** */
MseFillProfileX::MseFillProfileX() : MseFillProfile()
{
   Init();
}


MseFillProfileX::~MseFillProfileX()
{
}


/** */
MseFillProfileX::MseFillProfileX(const mysqlpp::sql_int_unsigned &p1) : MseFillProfile(p1)
{
   Init();
	fill = p1;
}


void MseFillProfileX::Init()
{
   fill                 = 0;
   profile_blue_h       = -1;
   profile_blue_h_err   = -1;
   profile_blue_v       = -1;
   profile_blue_v_err   = -1;
   profile_yellow_h     = -1;
   profile_yellow_h_err = -1;
   profile_yellow_v     = -1;
   profile_yellow_v_err = -1;
   pmax_blue_h          = -1;
   pmax_blue_h_err      = -1;
   pmax_blue_v          = -1;
   pmax_blue_v_err      = -1;
   pmax_yellow_h        = -1;
   pmax_yellow_h_err    = -1;
   pmax_yellow_v        = -1;
   pmax_yellow_v_err    = -1;
}


/** */
void MseFillProfileX::Print(const Option_t* opt) const
{
   gSystem->Info("MseFillProfileX::Print", "Print members:");
   PrintAsPhp();
}


/** */
void MseFillProfileX::PrintAsPhp(FILE *f) const
{
   fprintf(f, "$rc['fill']                    = %d;\n", fill              );
   fprintf(f, "$rc['profile_blue_h']          = %f;\n", profile_blue_h    );
   fprintf(f, "$rc['profile_blue_v']          = %f;\n", profile_blue_v    );
   fprintf(f, "$rc['profile_yellow_h']        = %f;\n", profile_yellow_h  );
   fprintf(f, "$rc['profile_yellow_v']        = %f;\n", profile_yellow_v  );
   fprintf(f, "\n");
}


/** */
void MseFillProfileX::SetValues(const AnaFillResult &afr)
{
   profile_blue_h       = afr.GetPCProfR(kBLUE_RING, kTARGET_H).first;     //iTgtRes->second.first; 
   profile_blue_h_err   = afr.GetPCProfR(kBLUE_RING, kTARGET_H).second;    //iTgtRes->second.second; 
   pmax_blue_h          = afr.GetPCProfPMax(kBLUE_RING, kTARGET_H).first;
   pmax_blue_h_err      = afr.GetPCProfPMax(kBLUE_RING, kTARGET_H).second;

   profile_blue_v       = afr.GetPCProfR(kBLUE_RING, kTARGET_V).first;     //iTgtRes->second.first; 
   profile_blue_v_err   = afr.GetPCProfR(kBLUE_RING, kTARGET_V).second;    //iTgtRes->second.second; 
   pmax_blue_v          = afr.GetPCProfPMax(kBLUE_RING, kTARGET_V).first;
   pmax_blue_v_err      = afr.GetPCProfPMax(kBLUE_RING, kTARGET_V).second;

   profile_yellow_h     = afr.GetPCProfR(kYELLOW_RING, kTARGET_H).first;     //iTgtRes->second.first; 
   profile_yellow_h_err = afr.GetPCProfR(kYELLOW_RING, kTARGET_H).second;    //iTgtRes->second.second; 
   pmax_yellow_h        = afr.GetPCProfPMax(kYELLOW_RING, kTARGET_H).first;
   pmax_yellow_h_err    = afr.GetPCProfPMax(kYELLOW_RING, kTARGET_H).second;

   profile_yellow_v     = afr.GetPCProfR(kYELLOW_RING, kTARGET_V).first;     //iTgtRes->second.first; 
   profile_yellow_v_err = afr.GetPCProfR(kYELLOW_RING, kTARGET_V).second;    //iTgtRes->second.second; 
   pmax_yellow_v        = afr.GetPCProfPMax(kYELLOW_RING, kTARGET_V).first;
   pmax_yellow_v_err    = afr.GetPCProfPMax(kYELLOW_RING, kTARGET_V).second;
}
