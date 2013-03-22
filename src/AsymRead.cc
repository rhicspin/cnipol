//  Asymmetry Analysis of RHIC pC Polarimeter
//  file name :   AsymRead.cc
//
//  Author    :   Itaru Nakagawa
//                Dmitri Smirnov
//
//  Creation  :   10/17/2005
//

#include "AsymRead.h"

#include <sstream>

#include "mysql++.h"

#include "TRandom.h"
#include "TStopwatch.h"

#include "AsymCalculator.h"
#include "AsymProcess.h"
#include "AsymRecover.h"
#include "AsymRoot.h"
#include "AnaInfo.h"
#include "TargetInfo.h"

using namespace std;

/** */
RawDataProcessor::RawDataProcessor() : fFileName(""), fFile(0), fMem(0),
   fFileStream(), fSeenRecords()
{
   fclose(fFile);
   delete[] fMem; // why is it here?
   fFileStream.close();
}


/** */
RawDataProcessor::RawDataProcessor(string fname) : fFileName(fname), fFile(0),
   fMem(0),
   fFileStream(fFileName.c_str(), ios::binary), fSeenRecords()
{
   FILE *fFile = fopen(fFileName.c_str(), "r");

   // reading the data till its end ...
   if (!fFile) {
      printf("ERROR: %s file not found. Force exit.\n", fFileName.c_str());
      exit(-1);
   } else
      printf("\nFound file %s\n", fFileName.c_str());

   // Create a BLOB with file content
   printf("Reading into memory...\n");
   TStopwatch sw;

   fFileStream.seekg(0, ios::end);
   fMemSize = fFileStream.tellg(); // in bytes
   printf("fileSize: %d\n", fMemSize);
   fFileStream.seekg(0, ios::beg);

   fMem = new char[fMemSize];
   fFileStream.read(fMem, fMemSize);
   fFileStream.close();

   sw.Stop();
   printf("Stopped reading into memory: Real: %f s and CPU: %f s time\n", sw.RealTime(), sw.CpuTime());
}


/** */
RawDataProcessor::~RawDataProcessor()
{
   if (fFile) fclose(fFile);
   delete[] fMem;
   //fFileStream.close();
}


/** */
void RawDataProcessor::ReadRecBegin(MseMeasInfoX &mseMeasInfo)
{
   cout << endl;
   Info("ReadRecBegin", "Start reading begin record from data file...");

   recordBeginStruct *recBegin;

   //int nRecs = fread(&recBegin, sizeof(recBegin), 1, fp);

   // Take the very first record from the raw data file
   recBegin = (recordBeginStruct*) fMem;

   // We'd better check the record type...
   //if (nRecs == 1 && (recBegin.header.type & REC_TYPEMASK) == REC_BEGIN) {

   cout << "Begin of data stream version: " << recBegin->version << endl;
   cout << "Comment: "                      << recBegin->comment << endl;
   cout << "Time Stamp: "                   << ctime(&recBegin->header.timestamp.time);
   cout << "Unix Time Stamp: "              << recBegin->header.timestamp.time << endl;

   gMeasInfo->fStartTime         = recBegin->header.timestamp.time;
   gMeasInfo->fDataFormatVersion = recBegin->version;
   gMeasInfo->fPolBeam           = (recBegin->header.type & REC_MASK_BEAM)   >> 16;
   gMeasInfo->fPolStream         = (recBegin->header.type & REC_MASK_STREAM) >> 20;

   //printf("fPolBeam:   %#x\n", gMeasInfo->fPolBeam);
   //printf("fPolStream: %#x, %#x, %#x\n", gMeasInfo->fPolStream, (UInt_t) recBegin->header.type, REC_MASK_STREAM);

   int polId = gMeasInfo->GetPolarimeterId(gMeasInfo->fPolBeam, gMeasInfo->fPolStream);

   if (polId < 0)
      Warning("ReadRecBegin(MseMeasInfoX &mseMeasInfo)", "Cannot read polarimeter ID from data record. Will guess it from file name or user option");

   stringstream sstr;

   // First, try to get polarimeter id from the data
   if (polId >= 0) {
      sstr << gMeasInfo->fPolId;
      gRunDb.fPolId = gMeasInfo->fPolId;

   // if failed, get id from the file name
   } else if (gMeasInfo->GetPolarimeterId() >= 0) {
      sstr.str("");
      sstr << gMeasInfo->fPolId;
      gRunDb.fPolId = gMeasInfo->fPolId;

   // see if the polarimeter id was set by command line argument
   } else if (gMeasInfo->fPolId >= 0) {
      sstr.str("");
      sstr << gMeasInfo->fPolId;
      gRunDb.fPolId = gMeasInfo->fPolId;

   } else { // cannot proceed
      Error("ReadRecBegin(MseMeasInfoX &mseMeasInfo)", "Unknown polarimeter ID");
      exit(-1);
   }

   sstr.str("");
   sstr << gMeasInfo->fStartTime;
   gRunDb.timeStamp = gMeasInfo->fStartTime; // should be always defined in raw data
   //gRunDb.Print();

   mseMeasInfo.polarimeter_id = gRunDb.fPolId;
   mysqlpp::DateTime dt(gMeasInfo->fStartTime);
   mseMeasInfo.start_time     = dt;

   //} else
      //printf("ERROR: Cannot read REC_BEGIN record\n");

   Info("ReadRecBegin(MseMeasInfoX &mseMeasInfo)", "Finished reading begin record from data file");

   // Not really used now
   // Configure colliding bunch patterns for PHENIX-BRAHMS and STAR
   PrepareCollidingBunchPattern();
}


/**
 * This method is supposed to read all information about the measurement from
 * raw data. It does not read events.
 */
void RawDataProcessor::ReadMeasInfo(MseMeasInfoX &mseMeasInfo)
{
   cout << endl;
   Info("ReadMeasInfo", "Start reading run info from data file...");

   TStopwatch sw;

   recordHeaderStruct *mHeader;

   char *mSeek = fMem;

   while (true)
   {
      if (mSeek > fMem + fMemSize - 1) break;
      //if (nRecs != 1) break;
      //printf("Currently consider record: %0#10x, len: %ld\n", (UInt_t) header.type, header.len);

      mHeader = (recordHeaderStruct*) mSeek;
      //printf("Currently consider record: %0#10x, len: %ld (MEM)\n", (UInt_t) mHeader->type, mHeader->len);

      // REC_BEAMADO
      if ((mHeader->type & REC_TYPEMASK) == REC_BEAMADO)
      {
         printf("Reading REC_BEAMADO record... size = %ld\n", mHeader->len);

         recordBeamAdoStruct *rec = (recordBeamAdoStruct*) mHeader;

         gMeasInfo->SetBeamEnergy(rec->data.beamEnergyM);
         printf("Beam energy: %8.2f\n", gMeasInfo->GetBeamEnergyReal());
         printf("RHIC beam:   %8d\n",   gMeasInfo->fPolBeam);

         ProcessRecord( (recordBeamAdoStruct&) *rec);

         mSeek = mSeek + mHeader->len;
         continue;
      }

      // REC_MACHINEPARAMS
      if ((mHeader->type & REC_TYPEMASK) == REC_MACHINEPARAMS)
      {
         printf("Reading REC_MACHINEPARAMS record... size= %ld\n", mHeader->len);

         // Extract voltages, currents, etc
         RecordMachineParams *rec = (RecordMachineParams*) mHeader;
         ProcessRecord( (RecordMachineParams&) *rec);

         mSeek = mSeek + mHeader->len;
         continue;
      }

      // REC_TARGETPARAMS
      if ((mHeader->type & REC_TYPEMASK) == REC_TARGETPARAMS)
      {
         printf("Reading REC_TARGETPARAMS record... size= %ld\n", mHeader->len);

         // Extract target info
         RecordTargetParams *rec = (RecordTargetParams*) mHeader;
         ProcessRecord( (RecordTargetParams&) *rec);

         mSeek = mSeek + mHeader->len;
         continue;
      }

      // REC_MEASTYPE
      if ((mHeader->type & REC_TYPEMASK) == REC_MEASTYPE)
      {
         printf("Reading REC_MEASTYPE record... size = %ld\n", mHeader->len);

         recordMeasTypeStruct *rec = (recordMeasTypeStruct*) mHeader;
         ProcessRecord( (recordMeasTypeStruct&) *rec);

         mSeek = mSeek + mHeader->len;
         continue;
      }

      // REC_POLADO
      if ((mHeader->type & REC_TYPEMASK) == REC_POLADO)
      {
         printf("Reading REC_POLADO record... size = %ld\n", mHeader->len);

         recordPolAdoStruct *rec = (recordPolAdoStruct*) mHeader;
         ProcessRecord( (recordPolAdoStruct&) *rec, mseMeasInfo);

         mSeek = mSeek + mHeader->len;
         continue;
      }

      // REC_PCTARGET
      if ((mHeader->type & REC_TYPEMASK) == REC_PCTARGET)
      {
         // Do not process this record for calibration runs. May contain
         // invalid ndelim info
         printf("Reading REC_PCTARGET record... size = %ld\n", mHeader->len);

         recordpCTagAdoStruct *rec = (recordpCTagAdoStruct*) mHeader;
         ProcessRecord( (recordpCTagAdoStruct&) *rec, mseMeasInfo);

         mSeek = mSeek + mHeader->len;
         continue;
      }

      // REC_WCMADO
      if ((mHeader->type & REC_TYPEMASK) == REC_WCMADO)
      {
         printf("Reading REC_WCMADO record... size = %ld\n", mHeader->len);

         recordWcmAdoStruct *rec = (recordWcmAdoStruct*) mHeader;
         ProcessRecord( (recordWcmAdoStruct &) *rec);

         mSeek = mSeek + mHeader->len;
         continue;
      }

      // REC_WCM_NEW
      if ((mHeader->type & REC_TYPEMASK) == REC_WCM_NEW)
      {
         printf("Reading REC_WCM_NEW record... size = %ld\n", mHeader->len);

         RecordWcm *rec = (RecordWcm*) mHeader;
         ProcessRecord( (RecordWcm &) *rec);

         mSeek = mSeek + mHeader->len;
         continue;
      }

      // REC_WFDV8SCAL
      if ((mHeader->type & REC_TYPEMASK) == REC_WFDV8SCAL &&
          fSeenRecords.find(REC_WFDV8SCAL) == fSeenRecords.end())
      {
         printf("Reading REC_WFDV8SCAL record... size = %ld\n", mHeader->len);

         recordWFDV8ArrayStruct *rec = (recordWFDV8ArrayStruct*) mHeader;
         ProcessRecord( (recordWFDV8ArrayStruct &) *rec);

         fSeenRecords.insert(REC_WFDV8SCAL);

         mSeek = mSeek + mHeader->len;
         continue;
      }

      // REC_COUNTRATE
      if ((mHeader->type & REC_TYPEMASK) == REC_COUNTRATE)
      {
         printf("Reading REC_COUNTRATE record... size = %ld\n", mHeader->len);

         recordCountRate *rec = (recordCountRate*) mHeader;
         ProcessRecord( (recordCountRate &) *rec);

         mSeek = mSeek + mHeader->len;
         continue;
      }

      // REC_RHIC_CONF
      if ((mHeader->type & REC_TYPEMASK) == REC_RHIC_CONF)
      {
         printf("Reading REC_RHIC_CONF record... size = %ld\n", mHeader->len);

         recordConfigRhicStruct *rec = (recordConfigRhicStruct*) mHeader;
         ProcessRecord( (recordConfigRhicStruct &) *rec);

         mSeek = mSeek + mHeader->len;
         continue;
      }

      // REC_TAGADO:
      if ((mHeader->type & REC_TYPEMASK) == REC_TAGADO)
      {
         printf("Reading REC_TAGADO record... size = %ld\n", mHeader->len);
         //targetDataStruct *rec = (targetDataStruct*) mHeader;
         //targetDataStruct  tgtdat1;
         //targetDataStruct  tgtdat2;
         //memcpy(&tgtdat1, &rec.tagado.data[0], sizeof(tgtdat1));
         //memcpy(&tgtdat2, &rec.tagado.data[1], sizeof(tgtdat2));
         mSeek = mSeek + mHeader->len;
         continue;
      }

      // REC_SCALERS
      if ((mHeader->type & REC_TYPEMASK) == REC_SCALERS)
      {
         printf("Reading REC_SCALERS record... size = %ld\n", mHeader->len);
         //recordScalersStruct *rec = (recordScalersStruct*) mHeader;
         //ProcessRecord( (recordScalersStruct &) *rec);
         Info("ReadMeasInfo", "Not implemented yet");
         mSeek = mSeek + mHeader->len;
         continue;
      }

      // REC_READRAW, REC_READSUB, REC_READALL
      if ((mHeader->type & REC_TYPEMASK) == REC_READRAW)
      {
         printf("Reading REC_READRAW record... size = %ld\n", mHeader->len);
         Info("ReadMeasInfo", "Not implemented yet");
         mSeek = mSeek + mHeader->len;
         continue;
      }

      // REC_END:
      if ((mHeader->type & REC_TYPEMASK) == REC_END)
      {
         printf("Reading REC_END record... size = %ld\n", mHeader->len);
         recordEndStruct *rec = (recordEndStruct*) mHeader;
         gMeasInfo->fStopTime = rec->header.timestamp.time;
         mSeek = mSeek + mHeader->len;
         continue;
      }

      mSeek = mSeek + mHeader->len;
   }

   sw.Stop();

   Info("ReadMeasInfo", "Stopped reading run info from data file: %f s, %f s\n", sw.RealTime(), sw.CpuTime());
}


/** */
void RawDataProcessor::ReadDataPassOne(MseMeasInfoX &mseMeasInfo)
{
   cout << endl;
   Info("ReadDataPassOne", "Start reading events from data file...");

   TStopwatch sw;

   gMeasInfo->fNEventsProcessed = 0;
   gMeasInfo->fNEventsTotal     = 0;

   recordHeaderStruct *mHeader;
   char *mSeek = fMem;

   while (true) {

      if (mSeek > fMem + fMemSize - 1) break;
      //if (nRecs != 1) break;
      //printf("Currently consider record: %0#10x, len: %ld\n", (UInt_t) header.type, header.len);

      mHeader = (recordHeaderStruct*) mSeek;
      //printf("Currently consider record: %0#10x, len: %ld (MEM)\n", (UInt_t) mHeader->type, mHeader->len);

      if ((mHeader->type & REC_TYPEMASK) != REC_READAT)
      {
         mSeek = mSeek + mHeader->len;
         continue;
      }

      // We are here if rec type is REC_READAT
      long    delim   = mHeader->timestamp.delim;
      size_t  recSize = mHeader->len - sizeof(recordHeaderStruct);
      char   *mSeekAT = mSeek;

      mSeekAT += sizeof(recordHeaderStruct);
      mSeek    = mSeek + mHeader->len;

      recordReadATStruct *ATPtr;

      for (UInt_t i=0; i<recSize; )
      {
                  ATPtr   = (recordReadATStruct*) (mSeekAT + i);
         unsigned chId    = ATPtr->subhead.siNum; // si number
         unsigned nEvents = ATPtr->subhead.Events + 1;

         // count the total number of event records in raw file
         gMeasInfo->fNEventsTotal += nEvents;

         //if (nEvents > 100) printf("nEvents: %d, %d\n", siNum, nEvents);

         i += sizeof(subheadStruct) + nEvents*sizeof(ATStruct);

         for (unsigned iEvent=0; iEvent<nEvents; iEvent++)
         {
            //printf("m: %d %d %d\n", ATPtr->data[iEvent].a, ATPtr->data[iEvent].t, ATPtr->data[iEvent].s);

            if (gMaxEventsUser > 0 && gMeasInfo->fNEventsProcessed >= gMaxEventsUser) break;

            gAsymRoot->SetChannelEvent(ATPtr->data[iEvent], delim, chId);

            // Use only a fraction of events
            if (gRandom->Rndm() > gAsymAnaInfo->fThinout) continue;

            if ( gAsymRoot->fChannelEvent->PassCutSiliconChannel() )
            {
               // Fill hists with raw data
               //gAsymRoot->FillPassOne(kCUT_PASSONE_RAW);
               gAsymRoot->FillPassOne(kCUT_PASSONE);

               // The same but empty bunches only
               if ( gAsymRoot->fChannelEvent->PassCutEmptyBunch() )
                  gAsymRoot->FillPassOne(kCUT_PASSONE_RAW_EB);

               //ds: XXX commented for test
               //// Apply very basic "proto" cuts
               //if ( gAsymRoot->fChannelEvent->PassCutNoise() ) {
               //   gAsymRoot->FillPassOne(kCUT_PASSONE_CALIB);

               //   // The same but empty bunches only
               //   if ( gAsymRoot->fChannelEvent->PassCutEmptyBunch() )
               //      gAsymRoot->FillPassOne(kCUT_PASSONE_CALIB_EB);
               //}
            }

            // Use all events to fill pulser histograms - not valid. thinout is applied
            //if ( gAsymAnaInfo->HasPulserBit() &&
            //     gAsymRoot->fChannelEvent->PassCutEmptyBunch() &&
            //     //gAsymRoot->fChannelEvent->PassCutNoise() &&
            //     gAsymRoot->fChannelEvent->PassCutSiliconChannel() )
            //{
            //   gAsymRoot->FillPassOne(kCUT_PASSONE_PULSER);
            //}

            if ( gAsymAnaInfo->HasPmtBit() && gAsymRoot->fChannelEvent->PassCutPmtChannel() )
                 //gAsymRoot->fChannelEvent->PassCutPmtNoise()
            {
               gAsymRoot->FillPassOne(kCUT_PASSONE_PMT);
            }

            //if ( !gAsymRoot->fChannelEvent->PassCutNoise() )          continue;
            //if ( !gAsymRoot->fChannelEvent->PassCutEnabledChannel() ) continue;
            //if ( !gAsymRoot->fChannelEvent->PassCutPulser() )         continue;
            //if ( !gAsymRoot->fChannelEvent->PassCutSiliconChannel() ) continue;
            //if ( !gAsymRoot->fChannelEvent->PassCutDepEnergyTime() )  continue;
            //if (  gAsymRoot->fChannelEvent->PassCutEmptyBunch() )     continue;

            //gAsymRoot->PrintChannelEvent();

            gMeasInfo->fNEventsProcessed++;

            // fFileStdLogName is empty if the stdout/stderr was not redirected to a file
            //if (gMeasInfo->fNEventsProcessed%50000 == 0 && fFileStdLogName.empty() )
            if (gMeasInfo->fNEventsProcessed%50000 == 0)
            {
               printf("%s: Processed events %u\r", gMeasInfo->GetRunName().c_str(), gMeasInfo->fNEventsProcessed);
               fflush(stdout);
            }
         }
      }
   }

   printf("Total events read:      %12u\n", gMeasInfo->fNEventsTotal);
   printf("Total events processed: %12u\n", gMeasInfo->fNEventsProcessed);

   // Save info into database global object
   mseMeasInfo.stop_time         = mysqlpp::DateTime(gMeasInfo->fStopTime);
   mseMeasInfo.nevents_total     = gMeasInfo->fNEventsTotal;
   mseMeasInfo.nevents_processed = gMeasInfo->fNEventsProcessed;

   sw.Stop();

   Info("ReadDataPassOne", "Stopped reading events from data file: %f s, %f s\n", sw.RealTime(), sw.CpuTime());
}


/** Read loop routine */
void RawDataProcessor::ReadDataPassTwo(MseMeasInfoX &mseMeasInfo)
{
   cout << endl;
   Info("ReadDataPassTwo", "Start reading events from data file...");

   TStopwatch sw;

   gMeasInfo->fNEventsProcessed = 0;
   gMeasInfo->fNEventsTotal     = 0;

   recordHeaderStruct *mHeader;
   char *mSeek = fMem;

   while (true) {

      if (mSeek > fMem + fMemSize - 1) break;
      //if (nRecs != 1) break;
      //printf("Currently consider record: %0#10x, len: %ld\n", (UInt_t) header.type, header.len);

      mHeader = (recordHeaderStruct*) mSeek;
      //printf("Currently consider record: %0#10x, len: %ld (MEM)\n", (UInt_t) mHeader->type, mHeader->len);

      if ((mHeader->type & REC_TYPEMASK) != REC_READAT)
      {
         mSeek = mSeek + mHeader->len;
         continue;
      }

      // We are here if rec type is REC_READAT
      long    delim   = mHeader->timestamp.delim;
      size_t  recSize = mHeader->len - sizeof(recordHeaderStruct);
      char   *mSeekAT = mSeek;

      mSeekAT += sizeof(recordHeaderStruct);
      mSeek    = mSeek + mHeader->len;

      recordReadATStruct *ATPtr;

      for (UInt_t i=0; i<recSize; )
      {
                  ATPtr   = (recordReadATStruct*) (mSeekAT + i);
         unsigned chId    = ATPtr->subhead.siNum; // si number
         unsigned nEvents = ATPtr->subhead.Events + 1;

         // count the total number of event records in raw file
         gMeasInfo->fNEventsTotal += nEvents;

         //if (nEvents > 100) printf("nEvents: %d, %d\n", siNum, nEvents);

         i += sizeof(subheadStruct) + nEvents*sizeof(ATStruct);

         for (unsigned iEvent=0; iEvent<nEvents; iEvent++)
         {
            //printf("m: %d %d %d\n", ATPtr->data[iEvent].a, ATPtr->data[iEvent].t, ATPtr->data[iEvent].s);

            if (gMaxEventsUser > 0 && gMeasInfo->fNEventsProcessed >= gMaxEventsUser) break;

            gAsymRoot->SetChannelEvent(ATPtr->data[iEvent], delim, chId);

            // Use only a fraction of events
            if (gRandom->Rndm() > gAsymAnaInfo->fThinout) continue;

            //gAsymRoot->PrintChannelEvent();

            gMeasInfo->fNEventsProcessed++;

            // Finer Target Position Resolution by counting stepping moter
            // This feature became available after Run06
            if (gMeasInfo->fRunId >= 6) {
               cntr.revolution = gAsymRoot->fChannelEvent->GetRevolutionId();

               if (cntr.revolution > gMeasInfo->MaxRevolution)
                  gMeasInfo->MaxRevolution = cntr.revolution;

               if (gAsymRoot->fChannelEvent->GetChannelId() == 73 && delim != tgt.eventID) {
                  tgt.x += gAsymAnaInfo->target_count_mm * (float) tgt.vector;
                  tgt.vector=-1;
               }

               if (gAsymRoot->fChannelEvent->GetChannelId() == 73 || gAsymRoot->fChannelEvent->GetChannelId() == 74) {
                   switch (gAsymRoot->fChannelEvent->GetChannelId()) {
                   case 73:
                       tgt.eventID = delim;
                       ++cntr.tgtMotion;
                       break;
                   case 74:
                       tgt.vector = 1;
                       break;
                   }
               }
            }

            if (gAsymAnaInfo->fSaveTrees.any()) { gAsymRoot->AddChannelEvent(); }

            event_process(*gAsymRoot->fChannelEvent);

            // fFileStdLogName is empty if the stdout/stderr was not redirected to a file
            //if (gMeasInfo->fNEventsProcessed%50000 == 0 && fFileStdLogName.empty() )
            if (gMeasInfo->fNEventsProcessed%50000 == 0)
            {
               printf("%s: Processed events %u\r", gMeasInfo->GetRunName().c_str(), gMeasInfo->fNEventsProcessed);
               fflush(stdout);
            }
         }
      }
   }

   // Post processing
   if ( gAsymAnaInfo->HasNormalBit() ) end_process(mseMeasInfo);

   printf("End of data stream \n");
   printf("End time: %s\n", ctime(&gMeasInfo->fStopTime));
   printf("Total events read:      %12u\n", gMeasInfo->fNEventsTotal);
   printf("Total events processed: %12u\n", gMeasInfo->fNEventsProcessed);

   // Save info into database global object
   mseMeasInfo.stop_time         = mysqlpp::DateTime(gMeasInfo->fStopTime);
   mseMeasInfo.nevents_total     = gMeasInfo->fNEventsTotal;
   mseMeasInfo.nevents_processed = gMeasInfo->fNEventsProcessed;

   if (gAsymAnaInfo->HasAlphaBit())
      mseMeasInfo.beam_energy = 0;
   else
      mseMeasInfo.beam_energy = gMeasInfo->GetBeamEnergyReal();

   sw.Stop();

   Info("ReadDataPassTwo", "Stopped reading events from data file: %f s, %f s\n", sw.RealTime(), sw.CpuTime());
}


/**
 * Deprecated
 */
void reConfig()
{
    ifstream configFile;
    configFile.open(reConfFile);

    if (!configFile) {
       cout << "Failed to open Config File : " << reConfFile << endl;
       cout << "Proceed with original configuration from raw data file" << endl;
       return;
    }

    printf("**********************************\n");
    printf("** Configuration is overwritten **\n");
    printf("**********************************\n");

    cout << "Reading configuration info from : " << reConfFile <<endl;

    char  *tempchar;
    char   buffer[300];
    float  t0n, ecn, edeadn, a0n, a1n, ealphn, dwidthn, peden;
    float  c0n, c1n, c2n, c3n, c4n;
    int    stripn;
    int    linen = 0;

    while (!configFile.eof()) {

       configFile.getline(buffer, sizeof(buffer), '\n');

       if (strstr(buffer,"Channel")!=0) {

          tempchar = strtok(buffer,"l");
          stripn   = atoi(strtok(NULL, "="));
          t0n      = atof(strtok(NULL, " "));
          ecn      = atof(strtok(NULL, " "));
          edeadn   = atof(strtok(NULL, " "));
          a0n      = atof(strtok(NULL, " "));
          a1n      = atof(strtok(NULL, " "));
          ealphn   = atof(strtok(NULL, " "));
          dwidthn  = atof(strtok(NULL, " ")) + gAsymAnaInfo->dx_offset; // extra thickness
          peden    = atof(strtok(NULL, " "));
          c0n      = atof(strtok(NULL, " "));
          c1n      = atof(strtok(NULL, " "));
          c2n      = atof(strtok(NULL, " "));
          c3n      = atof(strtok(NULL, " "));
          c4n      = atof(strtok(NULL, " "));

          gConfigInfo->data.chan[stripn-1].edead  = edeadn;
          gConfigInfo->data.chan[stripn-1].ecoef  = ecn;
          gConfigInfo->data.chan[stripn-1].t0     = t0n;
          gConfigInfo->data.chan[stripn-1].A0     = a0n;
          gConfigInfo->data.chan[stripn-1].A1     = a1n;
          gConfigInfo->data.chan[stripn-1].acoef  = ealphn;
          gConfigInfo->data.chan[stripn-1].dwidth = dwidthn;
          gConfigInfo->data.chan[stripn-1].pede   = peden;
          gConfigInfo->data.chan[stripn-1].C[0]   = c0n;
          gConfigInfo->data.chan[stripn-1].C[1]   = c1n;
          gConfigInfo->data.chan[stripn-1].C[2]   = c2n;
          gConfigInfo->data.chan[stripn-1].C[3]   = c3n;
          gConfigInfo->data.chan[stripn-1].C[4]   = c4n;

          cout << " Strip "    << stripn;
          cout << " Ecoef "    << ecn;
          cout << " T0 "       << t0n;
          cout << " A0 "       << a0n;
          cout << " A1 "       << a1n;
          cout << " Acoef "    << ealphn;
          cout << " Dwidth "   << dwidthn;
          cout << " Pedestal " << peden    << endl;
       }

       linen ++;
    }

    configFile.close();
}


/** */
void UpdateRunConst(TRecordConfigRhicStruct *ci)
{
   if (!ci) return;

   //ci->Print();

   float Ct = ci->data.WFDTUnit/2.; // Determine the TDC count unit (ns/channel)
   float L  = ci->data.TOFLength;

   //gRunConsts[0] = RunConst();

   for (UShort_t i=1; i<=ci->data.NumChannels; i++) {

      if (gMeasInfo->fDataFormatVersion == 40200 ) // Run 11 version
      {
         L = ci->data.chan[i-1].TOFLength;
      } else
         L = CARBON_PATH_DISTANCE;

      gRunConsts[i] = RunConst(L, Ct);
   }

   // Set values for 0th channel to the average ones
   Float_t sumL      = 0;
   UInt_t  nChannels = 0;

   map<UShort_t, RunConst>::iterator irc = gRunConsts.begin();

   for (; irc!=gRunConsts.end(); ++irc) {
      if (gMeasInfo->IsSiliconChannel(irc->first)) {
         sumL += irc->second.L;
         nChannels++;
      }
   }

   float avrgL = nChannels > 0 ? sumL / nChannels : CARBON_PATH_DISTANCE;

   gRunConsts[0] = RunConst(avrgL, Ct);

   printf("\nAverage RunConst:\n");
   gRunConsts[0].Print();
}


// Description : Decorde target infomation.
//             : presently the target ID is assumed to be appear at the last of
//             : character string poldat.targetIdS.
// Input       : polDataStruct poldat
void DecodeTargetID(const polDataStruct &poldat, MseMeasInfoX &mseMeasInfo)
{
   cout << endl;
   cout << "target ID = " << poldat.targetIdS << endl;
   //cout << "startTimeS = " << poldat.startTimeS << endl;
   //cout << "stopTimeS = " << poldat.stopTimeS << endl;

   gRunDb.fFields["TARGET_ID"] = poldat.targetIdS;

   // initiarization
   gMeasInfo->fTargetId = '-';

   string str(poldat.targetIdS);

   if (str.find("Background") < str.size())   gMeasInfo->fTargetId = 'B';
   if (str.find("1")         == str.size()-1) gMeasInfo->fTargetId = '1';
   if (str.find("2")         == str.size()-1) gMeasInfo->fTargetId = '2';
   if (str.find("3")         == str.size()-1) gMeasInfo->fTargetId = '3';
   if (str.find("4")         == str.size()-1) gMeasInfo->fTargetId = '4';
   if (str.find("5")         == str.size()-1) gMeasInfo->fTargetId = '5';
   if (str.find("6")         == str.size()-1) gMeasInfo->fTargetId = '6';
   if (str.find("7")         == str.size()-1) gMeasInfo->fTargetId = '7';
   if (str.find("8")         == str.size()-1) gMeasInfo->fTargetId = '-';

   if (str.find('V') != string::npos) {
      gMeasInfo->fTargetOrient = 'V';
      mseMeasInfo.target_orient       = 'V';
   }

   if (str.find('H') != string::npos) {
      gMeasInfo->fTargetOrient = 'H';
      mseMeasInfo.target_orient       = 'H';
   }

   //cout << "target id str: " << str << " " << mseMeasInfo.target_orient << endl;

   stringstream sstr;
   //int target_id;
   sstr << gMeasInfo->fTargetId;
   //sstr >> target_id;
   sstr >> mseMeasInfo.target_id;
   //mseMeasInfo.target_id = target_id;

   // Restore Vertical or Horizontal target information
   // in case this information isn't decorded correctly
   // within REC_PCTARGET routine. If the target is horizontal,
   // then mask 90 degree detector.
   //if (gMeasInfo->fTargetOrient == '-') {

   //   if (str.find("Vert") == 0 || str.find("V") == 0) {
   //      gMeasInfo->fTargetOrient = 'V';
   //      mseMeasInfo.target_orient       = 'V';
   //      tgt.VHtarget            = 0;
   //   }

   //   if (str.find("Horz") == 0 || str.find("H") == 0) {
   //      gMeasInfo->fTargetOrient = 'H';
   //      mseMeasInfo.target_orient       = 'H';
   //      tgt.VHtarget            = 1;
   //      // This is too late to reconfigure strip mask because this routine is
   //      // executed at the end of event loop. Too bad. /* March 5,'09 IN */
   //      //      mask.detector = 0x2D;
   //      //      gMeasInfo->ConfigureActiveStrip(mask.detector);
   //   }
   //}
}


/** */
void ProcessRecordPCTarget(const pCTargetStruct &rec, MseMeasInfoX &mseMeasInfo)
{
   const long* pointer = (const long*) &rec;

   //--rec;
   //UShort_t i = 0;

   // copy data to a linear array
   Double_t* linRec = new Double_t[gNDelimeters*4 + 8]; // there 2 under and ooverflow bins

   for (Int_t k=0; k<gNDelimeters; k++) {

      *(linRec + k + 1                 ) = *pointer++; // Horizontal target
      *(linRec + k + (gNDelimeters+2)  ) = *pointer++;
      *(linRec + k + (gNDelimeters+2)*2) = *pointer++; // Vertical target
      *(linRec + k + (gNDelimeters+2)*3) = *pointer++;

      //printf("%8d %8d %12.3f %12.3f %12.3f %12.3f\n", k, gNDelimeters, *(linRec + k + 1), *(linRec + k + (gNDelimeters+2)  ), *(linRec + k + (gNDelimeters+2)*2), *(linRec + k + (gNDelimeters+2)*3) );
   }

   gAsymRoot->FillTargetHists(gNDelimeters, linRec);
   //gAsymRoot->AnalyzeTargetMoves();

   delete [] linRec;

   //
   tgt.fNDelim = gNDelimeters;

   //--pointer;
   UShort_t i = 0;

   printf("   index    total        x-pos        y-pos\n");

   pointer = (const long*) &rec;

   for (int k=0; k<gNDelimeters; k++) {

      tgt.Linear[k][1] = *pointer++; // Horizontal target
      tgt.Rotary[k][1] = *pointer++;
      tgt.Linear[k][0] = *pointer++; // Vertical target
      tgt.Rotary[k][0] = *pointer++;

      // force 0 for +/-1 tiny readout as target position.
      if (abs(tgt.Rotary[k][1]) <= 1) tgt.Rotary[k][1] = 0;

      // identify Horizontal or Vertical target from the first target
      // rotary position readout.
      if (k == 0) {
         // From Run09 on, the horizontal/vertical targets are
         // identified by linear motion
         long int tgt_identifyV = gMeasInfo->RUNID < 10040 ? tgt.Rotary[k][0] : tgt.Linear[k][0];
         long int tgt_identifyH = gMeasInfo->RUNID < 10040 ? tgt.Rotary[k][1] : tgt.Linear[k][1];

         if ( ( !tgt.Rotary[k][0] && !tgt.Rotary[k][1] ) ||
              (  tgt.Rotary[k][0] &&  tgt.Rotary[k][1] ) )
         {
            cout << "ERROR: no target rotary info. Don't know H/V target" << endl;
            //gMeasInfo->fTargetOrient = '-';
         }

         if (tgt_identifyV) {
            tgt.VHtarget           = 0;
            //gMeasInfo->fTargetOrient = 'V';
            //mseMeasInfo.target_orient      = 'V';
            cout << "Vertical Target in finite position - ???" << endl;

         } else if (tgt_identifyH) {
            tgt.VHtarget           = 1;
            //gMeasInfo->fTargetOrient = 'H';
            //mseMeasInfo.target_orient      = 'H';
            cout << "Horizontal Target in finite position - ???" << endl;

         } else {
            cout << "Warning: Target infomation cannot be recognized - ???" << endl;
         }

         tgt.x       = tgt.Rotary[k][tgt.VHtarget] * gAsymAnaInfo->target_count_mm;
         tgt.Time[i] = k;
         tgt.X[i]    = tgt.x;

         printf("%8d %8d %8d %8d %12.3f %12.3f %12.3f\n", i, k, nTgtIndex,
                 TgtIndex[k], tgt.X[TgtIndex[k]],
                 tgt.Rotary[k][0]*gAsymAnaInfo->target_count_mm,
                 tgt.Rotary[k][1]*gAsymAnaInfo->target_count_mm);
      } else {

         TgtIndex[k] = i;

         if (tgt.Rotary[k][1] != tgt.Rotary[k-1][1] ||
             tgt.Rotary[k][0] != tgt.Rotary[k-1][0] )
         {
            TgtIndex[k]                 = ++i;
            tgt.X[TgtIndex[k]]          = tgt.Rotary[k][tgt.VHtarget] * gAsymAnaInfo->target_count_mm;
            tgt.Time[TgtIndex[k]]       = float(k);
            tgt.Interval[TgtIndex[k-1]] = tgt.Time[TgtIndex[k]] - tgt.Time[TgtIndex[k-1]];
            ++nTgtIndex;

            printf("%8d %8d %8d %8d %12.3f %12.3f %12.3f\n", i, k, nTgtIndex,
               TgtIndex[k], tgt.X[TgtIndex[k]],
               tgt.Rotary[k][0]*gAsymAnaInfo->target_count_mm,
               tgt.Rotary[k][1]*gAsymAnaInfo->target_count_mm);

            //++i;
         }
      }

      // target position array including static target motion
      tgt.all.x[k] = tgt.Rotary[k][tgt.VHtarget] * gAsymAnaInfo->target_count_mm ;
   }

   printf("Number of delimiters: %4d\n", gNDelimeters);
   printf("nTgtIndex: %d\n", nTgtIndex);

   tgt.Print();

   // define target histograms (hbook)
   //tgtHistBook();

   // disable 90 degrees detectors for horizontal target 0x2D={10 1101}
   //if (tgt.VHtarget) mask.detector = 0x2D;
}


// Method name : PrepareCollidingBunchPattern(gMeasInfo->fPolBeam)
// Description : Configure phx.bunchpat[] and str.bunchpat[] arrays only for colliding bunches
void PrepareCollidingBunchPattern()
{
   for (int i=0; i<NBUNCH; i++) {
      phx.bunchpat[i] = 1; // PHENIX bunch patterns
      str.bunchpat[i] = 1; // STAR bunch patterns
   }

   if (gMeasInfo->fPolBeam == 1) { // Yellow Beam
      for (int j=31; j<40; j++) str.bunchpat[j] = phx.bunchpat[j+40] = 0;
   } else if (gMeasInfo->fPolBeam == 2){ // Blue Beam
      for (int j=31; j<40; j++) phx.bunchpat[j] = str.bunchpat[j+40] = 0;
   }

   cout << "===== Colliding Bunch pattern =======" << endl;
   cout << " IP2, IP8:  " ; for (int i=0; i<NBUNCH; i++) cout << phx.bunchpat[i]; cout << endl;
   cout << " IP6, IP10: " ; for (int i=0; i<NBUNCH; i++) cout << str.bunchpat[i]; cout << endl;
   cout << "=====================================" << endl;
}


/** */
void ProcessRecord(const recordConfigRhicStruct &rec)
{
   gConfigInfo = (recordConfigRhicStruct *) malloc(sizeof(recordConfigRhicStruct) +
             (rec.data.NumChannels - 1) * sizeof(SiChanStruct));

   //XXX printf("TTT: %d\n", rec.data.NumChannels);

   memcpy(gConfigInfo, &rec, sizeof(recordConfigRhicStruct) +
          (rec.data.NumChannels - 1) * sizeof(SiChanStruct));

   // when we mandatory provide cfg info -- derpecated
   //if (gAsymAnaInfo->RECONFMODE == 1) {
   //   reConfig(gConfigInfo);
   //}

   // Recalculate Run constants
   UpdateRunConst(gConfigInfo);
}


/** */
void ProcessRecord(const recordMeasTypeStruct &rec)
{
   //UInt_t size = (rec.header.len - sizeof(rec.header))/(sizeof(long));
   gMeasInfo->SetMeasType(rec.type);

   //printf("recordMeasTypeStruct\n");
   //printf("len, size: %ld, %d\n", rec.header.len, size);
   //printf("type: %d\n", measType);
}


/** */
void ProcessRecord(const recordPolAdoStruct &rec, MseMeasInfoX &mseMeasInfo)
{
   if (!gAsymAnaInfo->HasAlphaBit()) DecodeTargetID( (polDataStruct &) rec.data, mseMeasInfo);
}


/** */
void ProcessRecord(const recordpCTagAdoStruct &rec, MseMeasInfoX &mseMeasInfo)
{
   if (!gAsymAnaInfo->HasTargetBit() || gAsymAnaInfo->HasAlphaBit()) return;

   gNDelimeters  = (rec.header.len - sizeof(rec.header)) / sizeof(pCTargetStruct);

   //long *pointer = (long *) &rec.buffer[sizeof(rec.header)];
   ProcessRecordPCTarget((pCTargetStruct &) rec.data, mseMeasInfo);

}


/** */
void ProcessRecord(const recordWFDV8ArrayStruct &rec)
{
   Int_t s1=0, s2=0, s3=0, s4=0;
   //float hist[1536];

   UShort_t chId = rec.siNum + 1;

   // Consider only silicon channels
   if (chId > NSTRIP) return;

   //for (int i=0; i<1536; i++) {
   //   //hist[i] = rec.hist[i];
   //   //printf("i, hist[i]: %4d, %8.3f\n", i, hist[i]);
   //}

   for (int i=0; i< 128; i++) s1 += rec.hist[i];        // bunch hist
   for (int i=0; i< 128; i++) s2 += rec.hist[i+128];    // unpol energy
   for (int i=0; i< 128; i++) s3 += rec.hist[i+256];    // pol+ energy
   for (int i=0; i< 128; i++) s4 += rec.hist[i+384];    // pol- energy

   //HPAKAD( 200 + chId, &hist[0]);
   //HPAKAD( 300 + chId, &hist[128]);
   //HPAKAD( 400 + chId, &hist[256]);
   //HPAKAD( 500 + chId, &hist[384]);
   //HPAKAD(1300 + chId, &hist[128]);
   //HPAKAD(1400 + chId, &hist[256]);
   //HPAKAD(1500 + chId, &hist[384]);

   //if (sipar_.ifine == 0) {
   //   //HPAKAD(600 + chId, &hist[512]);
   //   //HPAKAD(1600 + chId, &hist[512]);
   //} else {
   //   //HPAKAD(700 + chId, &hist[512]);
   //   //HPAKAD(1700 + chId, &hist[512]);
   //}

   printf("Si%02d : %12ld %12ld %12ld %12ld %12ld    %12d  %12d  %12d  %12d",
          rec.siNum + 1, rec.scalers[0], rec.scalers[1],
          rec.scalers[2], rec.scalers[3], rec.scalers[4],
          s1, s2, s3, s4);

   gAsymRoot->FillScallerHists((Long_t*) rec.hist, chId);

   // Check for hardware counts compatibility. Mark channel as bad if something is wrong
   if (s2 < rec.scalers[2] || s3 != rec.scalers[0] ||
       s4 != rec.scalers[1] || s1 < s2 + s3 + s4)
   {
      printf(" <- !!!");
      //sipar_.mark[rec.siNum]++;
   }

   printf("\n");

   Int_t cnt = 0;

   cnt += rec.scalers[0] + rec.scalers[1] + rec.scalers[2];

   //for (int i=0; i<3; i++) {
   //   //sscal_[rec.siNum * 3+i] += rec.scalers[i];
   //}
}


/** */
void ProcessRecord(const recordCountRate &rec)
{
   UInt_t size = (rec.header.len - sizeof(rec.header))/(sizeof(long));
   Long_t *pointer = (Long_t *) rec.data;
   //Double_t *pointer = (Double_t *) &rec.buffer[sizeof(rec.header)];

   printf("len, size: %ld, %d\n", rec.header.len, size);

   //for (UInt_t i=0; i<size; i++) {
   //   printf("countrate: i: %d, %ld\n", i, *(pointer+i));
   //}

   gAsymRoot->FillProfileHists(size, pointer);
}


/** */
void ProcessRecord(const RecordMachineParams &rec)
{
    gMeasInfo->SetMachineParams(rec);
}


/** */
void ProcessRecord(const RecordTargetParams &rec)
{
    gMeasInfo->SetTargetParams(rec);
}


/** */
void ProcessRecord(const recordWcmAdoStruct &rec)
{
   for (int bid=1; bid<=N_BUNCHES; bid++) {

      float data = rec.data.fillDataM[(bid-1)*3];

      // save non-zero bunches only
      if (!data) continue;

      gMeasInfo->fWallCurMon[bid] = data;
      //gMeasInfo->fWallCurMonSum += gMeasInfo->fWallCurMon[bid] * gFillPattern[bid];
      gMeasInfo->fWallCurMonSum += gMeasInfo->fWallCurMon[bid];
   }

   //gMeasInfo->fWallCurMonAve = gMeasInfo->fWallCurMonSum/float(gMeasInfo->NActiveBunch);
   gMeasInfo->fWallCurMonAve = gMeasInfo->fWallCurMonSum / gMeasInfo->fWallCurMon.size();

   gAsymRoot->FillRunHists();
}


/** */
void ProcessRecord(const RecordWcm &rec)
{
   for (int bid=1; bid<=N_BUNCHES; bid++)
   {
      BeamBunchIter  ibb = gMeasInfo->fBeamBunches.find(bid);
      BeamBunch     &bb  = ibb->second;

      bb.SetLength((Float_t) rec.fBunchDataM[(bid-1)*3]);
   }
}


/** */
void ProcessRecord(const recordBeamAdoStruct &rec)
{
   //memcpy(&beamdat, &rec.beamado.data, sizeof(beamdat));
   //beamDataStruct beamdat;

   //gMeasInfo->SetBeamEnergy(beamdat.beamEnergyM);
   //fprintf(stdout, "Beam Energy: %8.2f\n", gMeasInfo->GetBeamEnergyReal());
   //fprintf(stdout, "RHIC Beam:   %1d\n", gMeasInfo->fPolBeam);

   for (int bid=0; bid<N_BUNCHES; bid++) {

      BeamBunchIter  ibb = gMeasInfo->fBeamBunches.find(bid+1);
      BeamBunch     &bb  = ibb->second;

      int pat = rec.data.polarizationFillPatternS[bid*3];

      if (pat > 0) {
         bb.SetBunchSpin(kSPIN_UP);
      } else if (pat < 0) {
         bb.SetBunchSpin(kSPIN_DOWN);
      } else {
         bb.SetBunchSpin(kSPIN_NULL);
      }

      pat = rec.data.measuredFillPatternM[bid*3];

      if (pat > 0) {
         bb.SetFilled(kTRUE);
      } else {
         bb.SetFilled(kFALSE);
      }
   }

   AsymRecover recover;

   if (Flag.spin_pattern >= 0) {
      cout << "\nSpin pattern has been overwritten" << endl;
      recover.OverwriteSpinPattern(Flag.spin_pattern);
   }

   if (Flag.fill_pattern >= 0) {
      cout << "\nFill pattern has been overwritten" << endl;
      recover.OverwriteFillPattern(Flag.fill_pattern);
   }

   // Mask bad/disabled bunches
   //if (gMeasInfo->NDisableBunch) {
   //   cout << "\nFill pattern has been overwritten again" << endl;
   //   recover.MaskFillPattern();
   //}

   // Print active Bunch
   //cout << "\nActive bunches: " << endl;

   //for (int i=0; i<NBUNCH; i++) {
   //   if (i%10 == 0) cout << " ";
   //   ActiveBunch[i] = gFillPattern[i];
   //   cout << ActiveBunch[i];
   //}

   //cout << endl;

   gMeasInfo->PrintBunchPatterns();
}
