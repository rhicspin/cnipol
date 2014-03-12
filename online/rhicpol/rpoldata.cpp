#include "rpoldata.h"


/** */
RecordHeaderStruct& RecordHeaderStruct::operator=(const RecordHeaderStruct &rec)
{
   len                = rec.len;
   type               = rec.type;
   num                = rec.num;
   timestamp          = rec.timestamp;

   return *this;
}


void RecordHeaderStruct::Streamer(TBuffer &R__b)
{
   // Stream an object of class RecordHeaderStruct.

   //UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      //Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      R__b >> len;
      R__b >> type;
      R__b >> num;
      if (sizeof(long) >= sizeof(time_t))
         R__b >> timestamp.delim;
      else
         R__b >> timestamp.time;

      //R__b.StreamObject(&(timestamp),typeid(Timestamp));
      //R__b.CheckByteCount(R__s, R__c, RecordHeaderStruct::IsA());
   } else {
      //R__c = R__b.WriteVersion(RecordHeaderStruct::IsA(), kTRUE);
      R__b << len;
      R__b << type;
      R__b << num;
      if (sizeof(long) >= sizeof(time_t))
         R__b << timestamp.delim;
      else
         R__b << timestamp.time;

      //R__b.StreamObject(&(timestamp),typeid(Timestamp));
      //R__b.SetByteCount(R__c, kTRUE);
   }
}


RecordMachineParams& RecordMachineParams::operator=(const RecordMachineParams &rec)
{
   header = rec.header;

   fCavity197MHzVoltage[0]   = rec.fCavity197MHzVoltage[0];
   fCavity197MHzVoltage[1]   = rec.fCavity197MHzVoltage[1];
   fSnakeCurrents[0]         = rec.fSnakeCurrents[0];
   fSnakeCurrents[1]         = rec.fSnakeCurrents[1];
   fStarRotatorCurrents[0]   = rec.fStarRotatorCurrents[0];
   fStarRotatorCurrents[1]   = rec.fStarRotatorCurrents[1];
   fPhenixRotatorCurrents[0] = rec.fPhenixRotatorCurrents[0];
   fPhenixRotatorCurrents[1] = rec.fPhenixRotatorCurrents[1];

   return *this;
}


void RecordMachineParams::Streamer(TBuffer &R__b)
{
   // Stream an object of class RecordMachineParams.

   //UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      //Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      header.Streamer(R__b);
      R__b.ReadStaticArray((int*)fCavity197MHzVoltage);
      R__b.ReadStaticArray((float*)fSnakeCurrents);
      R__b.ReadStaticArray((float*)fStarRotatorCurrents);
      R__b.ReadStaticArray((float*)fPhenixRotatorCurrents);
      //R__b.CheckByteCount(R__s, R__c, RecordMachineParams::IsA());
   } else {
      //R__c = R__b.WriteVersion(RecordMachineParams::IsA(), kTRUE);
      header.Streamer(R__b);
      R__b.WriteArray(fCavity197MHzVoltage, 2);
      R__b.WriteArray(fSnakeCurrents, 2);
      R__b.WriteArray(fStarRotatorCurrents, 2);
      R__b.WriteArray(fPhenixRotatorCurrents, 2);
      //R__b.SetByteCount(R__c, kTRUE);
   }
}


RecordTargetParams& RecordTargetParams::operator=(const RecordTargetParams &rec)
{
   header = rec.header;

   fVelocity             = rec.fVelocity;
   fProfileStartPosition = rec.fProfileStartPosition;
   fProfileEndPosition   = rec.fProfileEndPosition;
   fProfilePeakPosition  = rec.fProfilePeakPosition;

   return *this;
}


void RecordTargetParams::Streamer(TBuffer &R__b)
{
   // Stream an object of class RecordMachineParams.

   //UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      //Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      header.Streamer(R__b);
      R__b >> fVelocity;
      R__b >> fProfileStartPosition;
      R__b >> fProfileEndPosition;
      R__b >> fProfilePeakPosition;
      //R__b.CheckByteCount(R__s, R__c, RecordMachineParams::IsA());
   } else {
      //R__c = R__b.WriteVersion(RecordMachineParams::IsA(), kTRUE);
      header.Streamer(R__b);
      R__b << fVelocity;
      R__b << fProfileStartPosition;
      R__b << fProfileEndPosition;
      R__b << fProfilePeakPosition;
      //R__b.SetByteCount(R__c, kTRUE);
   }
}


RecordWcm::RecordWcm() : fFillDataM(), fWcmBeamM(0), fBunchDataM(), fBunchLengthM(0),
   fProfileHeaderM()//, fProfileDataM(0)
{
   //memset(fFillDataM,      0, 360*sizeof(Float_t));
   //memset(fBunchDataM,     0, 360*sizeof(Double_t));
   //memset(fProfileHeaderM, 0, 32 *sizeof(Double_t));
}


RecordWcm& RecordWcm::operator=(const RecordWcm &rec)
{
   header = rec.header;

   fWcmBeamM     = rec.fWcmBeamM;
   fBunchLengthM = rec.fBunchLengthM;

   memcpy(fFillDataM,  rec.fFillDataM, 360*sizeof(Float_t));
   memcpy(fBunchDataM, rec.fBunchDataM, 360*sizeof(Double_t));
   memcpy(fProfileHeaderM, rec.fProfileHeaderM, 32*sizeof(Double_t));

   return *this;
}


void RecordWcm::Print()
{
   for (short i=0; i<360; i++) printf("fBunchDataM[%02d]: %8.5f\n", i, fBunchDataM[i]);
   for (short i=0; i<32;  i++) printf("fProfileHeaderM[%02d]: %8.5f\n", i, fProfileHeaderM[i]);
}


/** */
void TRecordConfigRhicStruct::Streamer(TBuffer &buf)
{
   if (buf.IsReading()) {
      //printf("reading TRecordConfigRhicStruct::Streamer(TBuffer &buf) \n");
      //if (!this) { printf("reading error: this=0 \n"); exit(-1); }
      buf >> header.len;
      buf >> data.WFDTUnit;
      //Int_t = tmp;
      //buf >> tmp; data.CSR.split = tmp
      buf >> data.CSR.WFDMode;
      //printf("data.CSR: %#x, %#x, %#x, %#x, %#x\n", data.CSR.WFDMode, data.CSR.reg,
      //       data.CSR.split.iDiv, data.CSR.split.Mode, (data.CSR.WFDMode&0x0C));
      buf >> data.NumChannels;
      // Now we know the number of channels saved;
      for (int i=0; i!=data.NumChannels; i++) {
         buf >> data.chan[i].t0;
         buf >> data.chan[i].ecoef;
         buf >> data.chan[i].edead;
         buf >> data.chan[i].A0;
         buf >> data.chan[i].A1;
         buf >> data.chan[i].acoef;
         buf >> data.chan[i].dwidth;
      }
   } else {
      //printf("writing TRecordConfigRhicStruct::Streamer(TBuffer &buf) \n");
      //if (!this) { printf("error this=0 \n"); exit(-1); }
      // this one has to go first so we know how much memmory to allocate when reading
      buf << data.NumChannels;
      buf << header.len ;
      buf << data.WFDTUnit ;
      buf << data.CSR.WFDMode; // split, WFDMode, reg - union
      //printf("data.CSR: %#x, %#x, %#x, %#x, %#x\n", data.CSR.WFDMode, data.CSR.reg,
      //       data.CSR.split.iDiv, data.CSR.split.Mode, (data.CSR.WFDMode&0x0C));
      buf << data.NumChannels;
      for (int i=0; i!=data.NumChannels; i++) {
         buf << data.chan[i].t0;
         buf << data.chan[i].ecoef;
         buf << data.chan[i].edead;
         buf << data.chan[i].A0;
         buf << data.chan[i].A1;
         buf << data.chan[i].acoef;
         buf << data.chan[i].dwidth;
      }
   }
}


/** */
void TRecordConfigRhicStruct::Print(const Option_t* opt) const
{
   //long len = header.len;
   //         gConfigInfo = (recordConfigRhicStruct *)
   //                      malloc(sizeof(recordConfigRhicStruct) +
   //                      (rec.cfg.data.NumChannels - 1) * sizeof(SiChanStruct));
   int nRecords  = (header.len - sizeof(recordHeaderStruct)) / sizeof(configRhicDataStruct);
   int nChannels = data.NumChannels;

   printf("nChannels: %d %d\n", nChannels, nRecords);

   //for (int i=0; i!=nRecords; i++) {
   for (int i=0; i!=nChannels; i++) {
      printf("%02d, acoef: %f,", i, data.chan[i].acoef);
      printf(" %f, TOFLength: %f\n", data.chan[i].t0, data.chan[i].TOFLength);

      printf(" acoef: %f, %f, TOFLength: %f\n", chanconf[i].acoef, chanconf[i].t0, chanconf[i].TOFLength);
   }
}
