/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include <ctime>

#include "EventConfig.h"

#include "AsymAnaInfo.h"
#include "DbEntry.h"
#include "MeasInfo.h"
#include "AnaMeasResult.h"
#include "MseMeasInfo.h"

ClassImp(EventConfig)

using namespace std;

/**
 * This is a persistent class. It contains virtually all information about a
 * single measurement.
 *
 * Default constructor.
 * We have to allocate memory for fMeasInfo and fAnaInfo as they can be
 * read from a file with a streamer. The streamers do not allocate memory by
 * themselves.
 */
EventConfig::EventConfig() : TObject(), fRandom(new TRandom()),
   fMeasInfo(new MeasInfo()), fAnaInfo(new AsymAnaInfo()),
   fCalibrator(new Calibrator(fRandom)), fAnaMeasResult(new AnaMeasResult()),
   fMseMeasInfoX(new MseMeasInfoX())
{
}


/** Default destructor. */
EventConfig::~EventConfig()
{
   // pointers should be deleted properly
}


MeasInfo*      EventConfig::GetMeasInfo()      { return fMeasInfo; }
AsymAnaInfo*   EventConfig::GetAnaInfo()       { return fAnaInfo; }
Calibrator*    EventConfig::GetCalibrator()    { return fCalibrator; }
AnaMeasResult* EventConfig::GetAnaMeasResult() { return fAnaMeasResult; }
MseMeasInfoX*  EventConfig::GetMseMeasInfoX()  { return fMseMeasInfoX; }


/** */
void EventConfig::Print(const Option_t* opt) const
{
   printf("EventConfig:\n");
   PrintAsPhp();
}


/** */
void EventConfig::PrintAsPhp(FILE *f) const
{ //{{{
   fprintf(f, "<?php\n");

   //fprintf(f, "\n// TRecordConfigRhicStruct data\n");
   //if (!fConfigInfo) {
   //   Error("PrintAsPhp", "fConfigInfo not defined");
   //} else {
   //   fprintf(f, "$rc['data']['NumChannels'] = %d;\n", fConfigInfo->data.NumChannels);
   //}

   fprintf(f, "\n// MeasInfo data\n");
   if (!fMeasInfo) {
      Error("PrintAsPhp", "fMeasInfo not defined");
   } else {
      fMeasInfo->PrintAsPhp(f);
   }

   fprintf(f, "\n// AnaInfo data\n");
   if (!fAnaInfo) {
      Error("PrintAsPhp", "fAnaInfo not defined");
   } else {
      fAnaInfo->PrintAsPhp(f);
   }

   fprintf(f, "\n// Calibrator data\n");
   if (!fCalibrator) {
      Error("PrintAsPhp", "fCalibrator not defined");
   } else {
      fCalibrator->PrintAsPhp(f);
   }

   fprintf(f, "\n// AnaMeasResult data\n");
   if (!fAnaMeasResult) {
      Error("PrintAsPhp", "fAnaMeasResult not defined");
   } else {
      fAnaMeasResult->PrintAsPhp(f);
   }

   fprintf(f, "\n// MseMeasInfoX data\n");
   if (!fMseMeasInfoX) {
      Error("PrintAsPhp", "fMseMeasInfoX not defined");
   } else {
      fMseMeasInfoX->PrintAsPhp(f);
   }
   
   fprintf(f, "?>\n\n");
} //}}}


/** */
void EventConfig::PrintAsConfig(FILE *f) const
{ //{{{
	fprintf(f, "* Strip t0 ec edead A0 A1 ealph dwidth pede C0 C1 C2 C3 C4\n");
	fprintf(f, "* for the dead layer and T0  : %s\n", fMeasInfo->GetRunName().c_str());
	fprintf(f, "* for the Am calibration     : %s\n", fAnaInfo->GetAlphaCalibRun().c_str());
	fprintf(f, "* for the Integral/Amplitude : default\n");
	fprintf(f, "* \n");

   fCalibrator->PrintAsConfig(f);
} //}}}


/** */
string EventConfig::GetSignature() const
{ //{{{
   string strSignature = "signature not defined";

   if (fAnaInfo && fMeasInfo) {

      char strMeasStartTime[25];
      time_t measStartTime = fMeasInfo->fStartTime;
      tm *ltime = localtime(&measStartTime);
      strftime(strMeasStartTime, 25, "%c", ltime);

      char strAnaEndTime[25];
      time_t anaEndTime = fAnaInfo->fAnaDateTime + (time_t) fAnaInfo->fAnaTimeReal;
      //printf("***anaEndTime: %lld\n", anaEndTime);
      ltime = localtime(&anaEndTime);
      strftime(strAnaEndTime, 25, "%c", ltime);

      strSignature = fAnaInfo->GetRunName() + ": Recorded " + strMeasStartTime + ", "
                     + " Analyzed " + strAnaEndTime;
                     //+ fAnaInfo->fUserGroup.fRealName + " @ " + strAnaEndTime;

      //if (strSignature.size() != 0) 
      //   strSignature += ", ";

      strSignature += ", Version " + fAnaInfo->fAsymVersion + ", " + fAnaInfo->fUserGroup.fUser;
   }

   return strSignature;
} //}}}


/** */
bool EventConfig::operator<(const EventConfig &mc) const
{ //{{{
   if (fMeasInfo->RUNID < mc.fMeasInfo->RUNID) return true;
   return false;
} //}}}
