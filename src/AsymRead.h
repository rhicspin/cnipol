/**
 *
 * 15 Oct, 2010 - Dmitri Smirnov
 *    - Modified readloop to take an object of Root class as an argument 
 *
 * 18 Oct, 2010 - Dmitri Smirnov
 *    - Made the Root object global, redefined readloop back
 *
 */

#ifndef ASYM_READ_H
#define ASYM_READ_H

#include <errno.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "rhicpol.h"
#include "rpoldata.h"

#include "AsymGlobals.h"
#include "MeasInfo.h"
#include "MseMeasInfo.h"

#define BSIZE_OFFLINE 0x1000000                 // 16 MByte

class RawDataProcessor
{
public:
   std::string    fFileName;
   FILE          *fFile;
   char          *fMem;
   UInt_t         fMemSize;
   std::ifstream  fFileStream;

private:
   std::set<Int_t> fSeenRecords;

public:
   RawDataProcessor();
   RawDataProcessor(std::string fname);
   ~RawDataProcessor();

   void ReadRecBegin(MseRunInfoX* run=0);
   void ReadRunInfo(MseRunInfoX &runInfo);
   void ReadDataFast();
};

void readloop(MseRunInfoX &run);
void UpdateRunConst(TRecordConfigRhicStruct *ci);
void DecodeTargetID(const polDataStruct &poldat, MseRunInfoX &run);
void ProcessRecordPCTarget(const pCTargetStruct &rec, MseRunInfoX &run);
void PrepareCollidingBunchPattern();

void ProcessRecord(const recordConfigRhicStruct &rec);
void ProcessRecord(const recordPolAdoStruct &rec, MseRunInfoX &runInfo);
void ProcessRecord(const recordpCTagAdoStruct &rec, MseRunInfoX &run);
void ProcessRecord(const recordWFDV8ArrayStruct &rec);
void ProcessRecord(const recordCountRate &rec);
void ProcessRecord(const recordWcmAdoStruct &rec);
void ProcessRecord(const recordBeamAdoStruct &rec);

#endif
