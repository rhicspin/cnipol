/**
 *
 * 24 Dec, 2010 - Dmitri Smirnov
 *    - Created class
 *
 */

#ifndef AnaInfo_h
#define AnaInfo_h

#include <cstdlib>
#include <iostream>
#include <bitset>
#include <string>
#include <sys/stat.h>

#include "TBuffer.h"
#include "TString.h"

#include "AsymRunDB.h"


class TStructRunDB;


/** */
class TDatprocStruct
{
public:

   // Various histogramming and running modes
   enum Mode {MODE_ALPHA   = 0x01010000,
              MODE_CALIB   = 0x01000000,
              MODE_GRAPH   = 0x02000000, MODE_NO_GRAPH  = 0x82000000,
              MODE_NORMAL  = 0x00020000, MODE_NO_NORMAL = 0x80020000,
              MODE_SCALER  = 0x00040000,
              MODE_RAW     = 0x00080000,
              MODE_RUN     = 0x00100000,
              MODE_TARGET  = 0x00200000,
              MODE_PROFILE = 0x00400000,
              MODE_FULL    = 0x007f0000};

   // Constraint parameter for Data processing 
   std::string      fRunId;             // Run name
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
   Float_t          fFastCalibThinout;  // Approximate fraction of events to pass
   float            reference_rate;     // Expected universal rate for given target
   float            target_count_mm;    // Target count/mm conversion
   time_t           procDateTime;       // Date/time when processing started
   Double_t         procTimeReal;       // Time in seconds to process input raw file
   Double_t         procTimeCpu;        // Time in seconds to process input raw file
   std::string      userCalibFile;      // Calibration file pass by user as argument - DEPRECATED
   std::string      fAlphaCalibRun;     // Name of the alpha calib run
   std::string      fDlCalibRun;        // Name of the alpha calib run
   std::map<std::string, std::string> fAsymEnv;
   FILE*            fFileRunInfo;
   FILE*            fFileRunConf;
   FILE*            fFileStdLog;
   std::string      fFileStdLogName;
   Bool_t           fFlagCopyResults;

public:

   TDatprocStruct();
   TDatprocStruct(std::string runId);
   ~TDatprocStruct();

   std::string GetOutDir() const;
   std::string GetImageDir() const { return GetOutDir() + "/images"; }
   std::string GetAlphaCalibFile() const;
   std::string GetDlCalibFile() const;
   std::string GetRunInfoFileName() const { return GetOutDir() + "/runconfig.php"; }
   std::string GetRunConfFileName() const { return GetOutDir() + "/config_calib.dat"; }
   std::string GetStdLogFileName() const { return GetOutDir() + "/" + fFileStdLogName; }
   std::string GetRootFileName() const { return GetOutDir() + "/" + fRunId + ".root"; }
   std::string GetRootTreeFileName(UShort_t trid) const;
   FILE*       GetRunInfoFile() const { return fFileRunInfo; }
   FILE*       GetRunConfFile() const { return fFileRunConf; }
   void        ProcessOptions();
   void        Print(const Option_t* opt="") const;
   void        PrintAsPhp(FILE *f) const;
   void        PrintUsage();
   void        Streamer(TBuffer &buf);
   void        Update(TStructRunDB &rundb);
	void        CopyResults();

   inline Bool_t HasAlphaBit();
   inline Bool_t HasCalibBit();
   inline Bool_t HasGraphBit();
   inline Bool_t HasNormalBit();
   inline Bool_t HasScalerBit();
   inline Bool_t HasRawBit();
   inline Bool_t HasRunBit();
   inline Bool_t HasTargetBit();
   inline Bool_t HasProfileBit();

private:
   void Init();
   void MakeOutDir();
};

TBuffer & operator<<(TBuffer &buf, TDatprocStruct *&rec);
TBuffer & operator>>(TBuffer &buf, TDatprocStruct *&rec);

Bool_t TDatprocStruct::HasAlphaBit()  {
   return (fModes & (TDatprocStruct::MODE_ALPHA^TDatprocStruct::MODE_CALIB))  == (TDatprocStruct::MODE_ALPHA^TDatprocStruct::MODE_CALIB);
 }

Bool_t TDatprocStruct::HasCalibBit()   { return (fModes & TDatprocStruct::MODE_CALIB)   == TDatprocStruct::MODE_CALIB; }
Bool_t TDatprocStruct::HasGraphBit()   { return (fModes & TDatprocStruct::MODE_GRAPH)   == TDatprocStruct::MODE_GRAPH; }
Bool_t TDatprocStruct::HasNormalBit()  { return (fModes & TDatprocStruct::MODE_NORMAL)  == TDatprocStruct::MODE_NORMAL; }
Bool_t TDatprocStruct::HasScalerBit()  { return (fModes & TDatprocStruct::MODE_SCALER)  == TDatprocStruct::MODE_SCALER; }
Bool_t TDatprocStruct::HasRawBit()     { return (fModes & TDatprocStruct::MODE_RAW)     == TDatprocStruct::MODE_RAW; }
Bool_t TDatprocStruct::HasRunBit()     { return (fModes & TDatprocStruct::MODE_RUN)     == TDatprocStruct::MODE_RUN; }
Bool_t TDatprocStruct::HasTargetBit()  { return (fModes & TDatprocStruct::MODE_TARGET)  == TDatprocStruct::MODE_TARGET; }
Bool_t TDatprocStruct::HasProfileBit() { return (fModes & TDatprocStruct::MODE_PROFILE) == TDatprocStruct::MODE_PROFILE; }

#endif
