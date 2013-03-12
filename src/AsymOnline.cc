#include "AsymOnline.h"

#include <cstring>

ClassImp(OnlineMachineParams)


OnlineMachineParams::OnlineMachineParams() : TObject(), RecordMachineParams()
{
}


OnlineMachineParams & OnlineMachineParams::operator=(const RecordMachineParams &rec)
{
   this->header.len                = rec.header.len;
   this->header.type               = rec.header.type;
   this->header.num                = rec.header.num;
   this->header.timestamp          = rec.header.timestamp;

   this->fCavity197MHzVoltage[0]   = rec.fCavity197MHzVoltage[0];
   this->fCavity197MHzVoltage[1]   = rec.fCavity197MHzVoltage[1];
   this->fSnakeCurrents[0]         = rec.fSnakeCurrents[0];
   this->fSnakeCurrents[1]         = rec.fSnakeCurrents[1];
   this->fStarRotatorCurrents[0]   = rec.fStarRotatorCurrents[0];
   this->fStarRotatorCurrents[1]   = rec.fStarRotatorCurrents[1];
   this->fPhenixRotatorCurrents[0] = rec.fPhenixRotatorCurrents[0];
   this->fPhenixRotatorCurrents[1] = rec.fPhenixRotatorCurrents[1];

   return *this;
}


ClassImp(OnlineWcm)


OnlineWcm::OnlineWcm() : TObject(), RecordWcm()
{
}


OnlineWcm & OnlineWcm::operator=(const RecordWcm &rec)
{
   this->header.len       = rec.header.len;
   this->header.type      = rec.header.type;
   this->header.num       = rec.header.num;
   this->header.timestamp = rec.header.timestamp;

   this->fWcmBeamM        = rec.fWcmBeamM;
   this->fBunchLengthM    = rec.fBunchLengthM;

   memcpy(this->fFillDataM, rec.fFillDataM, 360*sizeof(Float_t));
   memcpy(this->fBunchDataM, rec.fBunchDataM, 360*sizeof(Double_t));
   memcpy(this->fProfileHeaderM, rec.fProfileHeaderM, 32*sizeof(Double_t));

   return *this;
}
