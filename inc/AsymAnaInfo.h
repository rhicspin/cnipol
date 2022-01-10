#ifndef AsymAnaInfo_h
#define AsymAnaInfo_h

#include <iostream>
#include <bitset>
#include <string>
#include <sys/stat.h>

#include "TObject.h"
#include "TSystem.h"

#include "AsymCommon.h"
#include "AnaInfo.h"

class MseMeasInfoX;


/**
 * This class takes care of the command line options specific to the 'asym'
 * executable. The common options are taken care of by the AnaInfo class.
 */
class AsymAnaInfo : public AnaInfo
{
public:

   // Various histogramming and running modes
   enum Mode {MODE_ALPHA             = 0x00011000,
              MODE_NORMAL            = 0x00020000, MODE_NO_NORMAL    = 0x40020000,
              MODE_SCALER            = 0x00040000,
              MODE_RAW               = 0x00080000,
              MODE_RUN               = 0x00100000,
              MODE_TARGET            = 0x00200000,
              MODE_PROFILE           = 0x00400000,
              MODE_ASYM              = 0x00800000,
              MODE_KINEMA            = 0x08000000,
              MODE_PMT               = 0x00000100,
              MODE_PULSER            = 0x00000200,
              MODE_STUDIES           = 0x00000400,
              MODE_STM               = 0x01800000, //zchang
              MODE_NOISE               = 0x02800000, //zchang noise study
              MODE_NO_GAIN_CORRECTION = 0x00000800};

   enum Option {OPTION_AC_DIPOLE_PERIOD = 0x20000004};

   // Constraint parameter for data processing 
   std::string      fRunName;           // Run name
   ULong_t          fAsymModes;
   std::bitset<3>   fSaveTrees;         //! bitmask telling which ROOT trees to save
   std::bitset<N_DETECTORS>   fDisabledDetectors;   //! bitmask with disabled detector indeces
   float            tshift;             // time shift in [ns]
   Float_t          fThinout;           // Approximate fraction of events to read
   UInt_t           fMaxEventsUser;     // Max number of events requested by user
   float            reference_rate;     // Expected universal rate for given target
   float            target_count_mm;    // Target count/mm conversion
   std::string      fAlphaCalibRun;     // Name of alpha calib run
   FILE            *fFileRunConf;       //!
   uint32_t         fAcDipolePeriod;    //! AC dipole magnet period in revolutions
public:

   AsymAnaInfo();
   ~AsymAnaInfo();

   void        SetRunName(std::string runName);
   std::string GetRunName() const;
   std::string GetRawDataFileName() const;
   virtual std::string GetAnaInfoFileName() const;
   std::string GetRunConfFileName() const;
   FILE*       GetRunConfFile() const;
   std::string GetAlphaCalibFile() const;
   std::string GetRootTreeFileName(UShort_t trid) const;
   void        ProcessOptions(int argc, char **argv);
   void        VerifyOptions();
   void        Print(const Option_t* opt="") const;
   void        PrintAsPhp(FILE *f=stdout) const;
   void        PrintUsage();
   void        Update(MseMeasInfoX& run);

   std::string GetAlphaCalibRun() const;
   Bool_t      HasAlphaBit() const;
   Bool_t      HasNormalBit() const;
   Bool_t      HasScalerBit() const;
   Bool_t      HasRawBit() const;
   Bool_t      HasRunBit() const;
   Bool_t      HasTargetBit() const;
   Bool_t      HasProfileBit() const;
   Bool_t      HasAsymBit() const;
   Bool_t      HasKinematBit() const;
   Bool_t      HasPmtBit() const;
   Bool_t      HasPulserBit() const;
   Bool_t      HasStudiesBit() const;
   Bool_t      HasSTMBit() const; //zchang
   Bool_t      HasNoiseBit() const; //zchang noise study
   Bool_t      HasNoGainCorrectionBit() const;

   ClassDef(AsymAnaInfo, 7)
};

#endif
