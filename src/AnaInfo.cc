
#include "AnaInfo.h"

using namespace std;


// Default Values for Run Condition
TDatprocStruct::TDatprocStruct()
{
   enel             = 400;
   eneu             = 900;    // energy
   widthl           = -30;
   widthu           = 30;     // banana cut width (Constant mode only)
   FEEDBACKMODE     = 0;      // FEEDBACKMODE
   RAWHISTOGRAM     = 0;      // RAWHISTOGRAM
   CMODE            = 0;      // CMODE
   DMODE            = 0;      // DMODE
   TMODE            = 0;      // TMODE
   AMODE            = 0;      // AMODE
   BMODE            = 1;      // BMODE
   ZMODE            = 0;      // ZMODE
   MESSAGE          = 0;      // MESSAGE
   CBANANA          = 2;      // CBANANA
   UPDATE           = 0;
   UPDATE_DB        = 0;
   MMODE            = 1;      // MMODE
   NTMODE           = 0;      // NTMODE
   RECONFMODE       = 1;      // RECONFMODE
   RAMPMODE         = 0;      // RAMPMODE
   STUDYMODE        = 0;      // STUDYMODE
   SAVETREES        = 0;      // SAVETREES
   MassSigma        = 3;      // MassSigma banana curve cut within <MassSigma> away from the 12C mass
   MassSigmaAlt     = 2;      // MassSigma banana curve alternative cut within <MassSigmaAlt> away from the 12C mass
   OneSigma         = 1.5e6;  // 1-sigma of 12C mass peak = > 1.5e6 [keV]
   tshift           = 0;      // tshift: Time shift in [ns]
   inj_tshift       = 0;      // Time shift in [ns] for injection w.r.t. flattop
   dx_offset        = 0;      // additional deadlayer offset [ug/cm2]
   WCMRANGE         = 999.05; // Wall Current Monitor process Fill range +/-5[%]
   MassLimit        = 8;      // Lower Mass limit for peak position adjustment fit default :8 GeV
   nEventsProcessed = 0;      // nEventsProcessed
   nEventsTotal     = 0;      // nEventsTotal
   thinout          = 1;      // <thinout> event rate to be feed into feedback routine
   reference_rate   = 1;      // Expected universal rate for given target
   target_count_mm  = 0.11;   // Target count/mm conversion
   procDateTime     = 0;      // procDateTime date and time
   procTimeReal     = 0;      // procTimeReal
   procTimeCpu      = 0;      // procTimeCpu
   userCalibFile    = "";     // userCalibFile
}


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
      buf >> enel;
      buf >> eneu;
      buf >> widthl;
      buf >> widthu;
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
      buf << CMODE;
      buf << nEventsProcessed;
      buf << nEventsTotal;
      buf << thinout;
      buf << procDateTime << procTimeReal << procTimeCpu;
   }
}
