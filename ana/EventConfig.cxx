/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "EventConfig.h"

#include "AnaInfo.h"
#include "DbEntry.h"
#include "RunInfo.h"
#include "AnaResult.h"
#include "MseRunInfo.h"

ClassImp(EventConfig)

using namespace std;

/**
 * Default constructor.
 * We have to allocate memory for fRunInfo, fAnaInfo, and fDbEntry as they can be
 * read from a file with a streamer. The streamers do not allocate memory by
 * themselves.
 */
EventConfig::EventConfig() : TObject(), fRandom(new TRandom()), fConfigInfo(0),
   fRunInfo(new RunInfo()), fAnaInfo(new AnaInfo()), fDbEntry(new DbEntry()),
   fCalibrator(new Calibrator()), fAnaResult(new AnaResult()), fMseRunInfoX(new MseRunInfoX())
{
}


/** Default destructor. */
EventConfig::~EventConfig()
{
   // there should be more than that...
   //delete fConfigInfo;
}


void EventConfig::Print(const Option_t* opt) const
{
   printf("EventConfig:\n");
   PrintAsPhp();
}


/** */
void EventConfig::PrintAsPhp(FILE *f) const
{ //{{{
   fprintf(f, "<?php\n");

   fprintf(f, "\n// TRecordConfigRhicStruct data\n");
   if (!fConfigInfo) {
      Error("PrintAsPhp", "fConfigInfo not defined");
   } else {
      fprintf(f, "$rc['data']['NumChannels'] = %d;\n", fConfigInfo->data.NumChannels);
   }

   fprintf(f, "\n// RunInfo data\n");
   if (!fRunInfo) {
      Error("PrintAsPhp", "fRunInfo not defined");
   } else {
      fRunInfo->PrintAsPhp(f);
   }

   fprintf(f, "\n// AnaInfo data\n");
   if (!fAnaInfo) {
      Error("PrintAsPhp", "fAnaInfo not defined");
   } else {
      fAnaInfo->PrintAsPhp(f);
   }

   fprintf(f, "\n// DbEntry data\n");
   if (!fDbEntry) {
      Error("PrintAsPhp", "fDbEntry not defined");
   } else {
      fDbEntry->PrintAsPhp(f);
   }

   fprintf(f, "\n// Calibrator data\n");
   if (!fCalibrator) {
      Error("PrintAsPhp", "fCalibrator not defined");
   } else {
      fCalibrator->PrintAsPhp(f);
   }

   fprintf(f, "\n// AnaResult data\n");
   if (!fAnaResult) {
      Error("PrintAsPhp", "fAnaResult not defined");
   } else {
      fAnaResult->PrintAsPhp(f);
   }

   fprintf(f, "\n// MseRunInfoX data\n");
   if (!fMseRunInfoX) {
      Error("PrintAsPhp", "fMseRunInfoX not defined");
   } else {
      fMseRunInfoX->PrintAsPhp(f);
   }
   
   fprintf(f, "?>\n");
} //}}}


/** */
void EventConfig::PrintAsConfig(FILE *f) const
{ //{{{
	fprintf(f, "* Strip t0 ec edead A0 A1 ealph dwidth pede C0 C1 C2 C3 C4\n");
	fprintf(f, "* for the dead layer and T0  : %s\n", fRunInfo->fRunName.c_str());
	fprintf(f, "* for the Am calibration     : %s\n", fDbEntry->alpha_calib_run_name.c_str());
	fprintf(f, "* for the Integral/Amplitude : default\n");
	fprintf(f, "* \n");

   fCalibrator->PrintAsConfig(f);
} //}}}


/** */
float EventConfig::ConvertToEnergy(UShort_t adc, UShort_t chId)
{ 
   return fConfigInfo->data.chan[chId].acoef * adc;
}


/** */
void EventConfig::Streamer(TBuffer &R__b)
{
   // Stream an object of class EventConfig.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> fRandom;
      R__b >> fConfigInfo;
      R__b >> fRunInfo;
      R__b >> fAnaInfo;
      R__b >> fCalibrator;
      R__b >> fAnaResult;
      R__b >> fMseRunInfoX;
      R__b.CheckByteCount(R__s, R__c, EventConfig::IsA());
   } else {
      R__c = R__b.WriteVersion(EventConfig::IsA(), kTRUE);
      TObject::Streamer(R__b);
      R__b << fRandom;
      R__b << fConfigInfo;
      R__b << fRunInfo;
      R__b << fAnaInfo;
      R__b << fCalibrator;
      R__b << fAnaResult;
      R__b << fMseRunInfoX;
      R__b.SetByteCount(R__c, kTRUE);
   }
}


/** */
TBuffer & operator<<(TBuffer &buf, EventConfig *&rec)
{
   if (!rec) return buf;
   //printf("operator<<(TBuffer &buf, EventConfig *rec) : \n");
   rec->Streamer(buf);
   return buf;
}


/** */
TBuffer & operator>>(TBuffer &buf, EventConfig *&rec)
{
   //printf("operator>>(TBuffer &buf, EventConfig *rec) : \n");
   rec->Streamer(buf);
   return buf;
}
