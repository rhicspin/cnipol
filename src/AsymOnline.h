#ifndef AsymOnline_h
#define AsymOnline_h

#include "TObject.h"

#include "rpoldata.h"


class OnlineMachineParams : public TObject, public RecordMachineParams
{
public:

   OnlineMachineParams();

   OnlineMachineParams & operator=(const RecordMachineParams &rec);

   ClassDef(OnlineMachineParams, 1)
};


class OnlineWcm : public TObject, public RecordWcm
{
public:

   OnlineWcm();

   OnlineWcm & operator=(const RecordWcm &rec);

   ClassDef(OnlineWcm, 1)
};


#endif
