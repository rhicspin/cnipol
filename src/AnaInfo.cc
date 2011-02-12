
#include "AnaInfo.h"

using namespace std;


// Default Values for Run Condition
TDatprocStruct::TDatprocStruct() :
   enel             (400),
   eneu             (900),   
   widthl           (-30),
   widthu           (3),    
   fModes           (MODE_NORMAL),
   FEEDBACKMODE     (0),     
   RAWHISTOGRAM     (0),     
   CMODE            (0),     
   DMODE            (0),     
   TMODE            (0),     
   AMODE            (0),     
   BMODE            (1),     
   ZMODE            (0),     
   MESSAGE          (0),     
   CBANANA          (2),     
   UPDATE           (0),
   UPDATE_DB        (0),
   QUICK_MODE       (0),
   MMODE            (1),     
   NTMODE           (0),     
   RECONFMODE       (1),     
   RAMPMODE         (0),     
   STUDYMODE        (0),     
   SAVETREES        (0),     
   MassSigma        (3),     
   MassSigmaAlt     (2),     
   OneSigma         (1.5e6), 
   tshift           (0),     
   inj_tshift       (0),     
   dx_offset        (0),     
   WCMRANGE         (999.05),
   MassLimit        (8),     
   nEventsProcessed (0),     
   nEventsTotal     (0),     
   thinout          (1),     
   reference_rate   (1),     
   target_count_mm  (0.11),  
   procDateTime     (0),     
   procTimeReal     (0),     
   procTimeCpu      (0),     
   userCalibFile    ("")    
{ }


/** */
TDatprocStruct::~TDatprocStruct() { }


/** */
TBuffer & operator<<(TBuffer &buf, TDatprocStruct *&rec)
{
   if (!rec) return buf;
   //printf("operator<<(TBuffer &buf, TDatprocStruct *rec) : \n");
   rec->Streamer(buf);
   return buf;
}


/** */
TBuffer & operator>>(TBuffer &buf, TDatprocStruct *&rec)
{
   //if (!rec) return buf;
   //printf("operator<<(TBuffer &buf, TDatprocStruct *rec) : \n");
   // if object has been created already delete it
   free(rec);
   rec = (TDatprocStruct *) realloc(rec, sizeof(TDatprocStruct));
   rec->Streamer(buf);
   return buf;
}


/** */
void TDatprocStruct::Print(const Option_t* opt) const
{
   cout
   << "enel             = " << enel             << endl
   << "eneu             = " << eneu             << endl
   << "widthl           = " << widthl           << endl
   << "widthu           = " << widthu           << endl
   << "fModes           = " << hex << showbase << fModes << endl << dec << noshowbase
   << "FEEDBACKMODE     = " << FEEDBACKMODE     << endl
   << "RAWHISTOGRAM     = " << RAWHISTOGRAM     << endl
   << "CMODE            = " << CMODE            << endl
   << "DMODE            = " << DMODE            << endl
   << "TMODE            = " << TMODE            << endl
   << "AMODE            = " << AMODE            << endl
   << "BMODE            = " << BMODE            << endl
   << "ZMODE            = " << ZMODE            << endl
   << "MESSAGE          = " << MESSAGE          << endl
   << "CBANANA          = " << CBANANA          << endl
   << "UPDATE           = " << UPDATE           << endl
   << "UPDATE_DB        = " << UPDATE_DB        << endl
   << "QUICK_MODE       = " << QUICK_MODE       << endl
   << "MMODE            = " << MMODE            << endl
   << "NTMODE           = " << NTMODE           << endl
   << "RECONFMODE       = " << RECONFMODE       << endl
   << "RAMPMODE         = " << RAMPMODE         << endl
   << "STUDYMODE        = " << STUDYMODE        << endl
   << "SAVETREES        = " << SAVETREES        << endl
   << "MassSigma        = " << MassSigma        << endl
   << "MassSigmaAlt     = " << MassSigmaAlt     << endl
   << "OneSigma         = " << OneSigma         << endl
   << "tshift           = " << tshift           << endl
   << "inj_tshift       = " << inj_tshift       << endl
   << "dx_offset        = " << dx_offset        << endl
   << "WCMRANGE         = " << WCMRANGE         << endl
   << "MassLimit        = " << MassLimit        << endl
   << "nEventsProcessed = " << nEventsProcessed << endl
   << "nEventsTotal     = " << nEventsTotal     << endl
   << "thinout          = " << thinout          << endl
   << "reference_rate   = " << reference_rate   << endl
   << "target_count_mm  = " << target_count_mm  << endl
   << "procDateTime     = " << procDateTime     << endl
   << "procTimeReal     = " << procTimeReal     << endl
   << "procTimeCpu      = " << procTimeCpu      << endl
   << "userCalibFile    = " << userCalibFile    << endl;
}


/** */
void TDatprocStruct::Streamer(TBuffer &buf)
{
   if (buf.IsReading()) {
      //printf("reading TDatprocStruct::Streamer(TBuffer &buf) \n");

      unsigned long uLong = 0;

      buf >> enel;
      buf >> eneu;
      buf >> widthl;
      buf >> widthu;
      buf >> fModes;
      buf >> CMODE;
      buf >> nEventsProcessed;
      buf >> nEventsTotal;
      buf >> thinout;
      buf >> procDateTime >> procTimeReal >> procTimeCpu;
   } else {
      //printf("writing TDatprocStruct::Streamer(TBuffer &buf) \n");
      buf << enel;
      buf << eneu;
      buf << widthl;
      buf << widthu;
      buf << fModes;
      buf << CMODE;
      buf << nEventsProcessed;
      buf << nEventsTotal;
      buf << thinout;
      buf << procDateTime << procTimeReal << procTimeCpu;
   }
}
