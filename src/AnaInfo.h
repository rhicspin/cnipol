/**
 *
 * 24 Dec, 2010 - Dmitri Smirnov
 *    - Created class
 *
 */

#ifndef AnaInfo_h
#define AnaInfo_h

#include <iostream>
#include <bitset>
#include <string>

#include "TBuffer.h"


/** */
class TDatprocStruct
{
public:

   enum Mode {MODE_ALPHA=0x010000, MODE_NORMAL=0x020000,
              MODE_NO_NORMAL=0x80020000, MODE_SCALER=0x040000,
              MODE_RAW=0x080000, MODE_BUNCH=0x100000, MODE_FULL=0x0f0000};

   // Constraint parameter for Data processing 
   int              enel;               // lower kinetic energy threshold (keV)
   int              eneu;               // upper kinetic energy threshold (keV)
   int              widthl;             // lower banana cut (ns)
   int              widthu;             // upper banana cut (ns)
   ULong_t          fModes;
   int              FEEDBACKMODE;       // fit 12C peak first and feedback tshift and sigma
   int              RAWHISTOGRAM;       // Fill raw histograms
   int              CMODE;              // Calibration mode 0:off 1:on
   int              DMODE;              // dead layer study mode 0:off 1:on
   int              TMODE;              // T0 study mode 0:off 1:on
   int              AMODE;              // A0,A1 study (signal Amp vs. Int) mode 0:off 1:on
   int              BMODE;              // create banana curve (E-T) plots 0:off 1:on
   int              ZMODE;              // with/out T0 subtraction 0:with 1:without
   int              MESSAGE;            // message mode 1: exit just after run begin
   int              CBANANA;            // constant width banana cut :1, <sigma> Mass Cut :2
   int              UPDATE;             // 1: keep update of the histogram
   int              UPDATE_DB;          // 0: default, 1: update info in database
   UShort_t         QUICK_MODE;         // 0: default, 1: quick mode
   int              MMODE;              // mass mode
   int              NTMODE;             // if 1 store NTUPLEv
   int              RECONFMODE;         // if 1 reconfigure from file
   int              RAMPMODE;           // if 1 prepare the histograms for ramp measurement
   int              STUDYMODE;          // if 1 study mode
   std::bitset<3>   SAVETREES;          // bitmask telling which ROOT trees to save
   float            MassSigma;          // banana curve cut within <MassSigma> away from the 12C mass
   float            MassSigmaAlt;       // banana curve alternative cut within
                                        // <MassSigmaAlt> away from the 12C mass
   float            OneSigma;           // 1-sigma of 12C mass distribution in [keV]
   float            tshift;             // time shift in [ns]
   float            inj_tshift;         // time shift in [ns] for injection w.r.t. flattop
   float            dx_offset;          // additional deadlayer offset [ug/cm2]
   float            WCMRANGE;           // Wall Current Monitor process Fill range
   float            MassLimit;          // Lower Mass limit for peak position adjustment fit
   UInt_t           nEventsProcessed;   // number of events processed from raw data file
   UInt_t           nEventsTotal;       // number of total events in raw data file
   UInt_t           thinout;            // Every <thinout> event to be feed into feedback routine
   float            reference_rate;     // Expected universal rate for given target
   float            target_count_mm;    // Target count/mm conversion
   time_t           procDateTime;       // Date/time when processing started
   Double_t         procTimeReal;       // Time in seconds to process input raw file
   Double_t         procTimeCpu;        // Time in seconds to process input raw file
   std::string      userCalibFile;    // Calibration file pass by user as argument

public:

   TDatprocStruct();
   ~TDatprocStruct();

   void Print(const Option_t* opt="") const;
   void Streamer(TBuffer &buf);
};

TBuffer & operator<<(TBuffer &buf, TDatprocStruct *&rec);
TBuffer & operator>>(TBuffer &buf, TDatprocStruct *&rec);

#endif
