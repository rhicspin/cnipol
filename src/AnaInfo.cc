
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
   UPDATE           = 0;      // UPDATE
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
