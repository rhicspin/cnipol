#ifndef AsymOnline_h
#define AsymOnline_h

#include "TObject.h"

#include "rpoldata.h"

class RecordHeaderStruct_t : public TObject{
 public:
  int len;           // total length = header size + data size
  int type;          // record type, see above
  int num;           // record number
  int time;      // local UNIX time in most cases
  int delim;     // when read from memory

  RecordHeaderStruct_t & operator = (const RecordHeaderStruct &hd){
    len = hd.len;
    type = hd.type;
    num = hd.type;
    time = hd.timestamp.time;
    delim = hd.timestamp.delim;
    return *this;
  }
 private:
  ClassDef(RecordHeaderStruct_t, 1);
};


class RecordMachineParams_t : public TObject{
 public:
  RecordHeaderStruct_t header;
  int   fCavity197MHzVoltage[N_BEAMS];
  float fSnakeCurrents[N_BEAMS];
  float fStarRotatorCurrents[N_BEAMS];
  float fPhenixRotatorCurrents[N_BEAMS];
  RecordMachineParams_t & operator = (const RecordMachineParams &par){
    header = par.header;
    for(int ib = 0; ib < N_BEAMS; ib++){
      fCavity197MHzVoltage[ib] = par.fCavity197MHzVoltage[ib];
      fSnakeCurrents[ib] = par.fSnakeCurrents[ib];
      fStarRotatorCurrents[ib] = par.fStarRotatorCurrents[ib];
      fPhenixRotatorCurrents[ib] = par.fPhenixRotatorCurrents[ib];
    }
    return *this;
  }
 private:
  ClassDef(RecordMachineParams_t, 1);
};


class RecordTargetParams_t : public TObject{
 public:
  RecordHeaderStruct_t header;
  int fVelocity;
  int fProfileStartPosition;
  int fProfileEndPosition;
  int fProfilePeakPosition;
  RecordTargetParams_t & operator = (const RecordTargetParams &par){
    header = par.header;
    fVelocity = par.fVelocity;
    fProfileStartPosition = par.fProfileStartPosition;
    fProfileEndPosition = par.fProfileEndPosition;
    fProfilePeakPosition = par.fProfilePeakPosition;
    return *this;
  }
 private:
  ClassDef(RecordTargetParams_t, 1);
};

#endif
