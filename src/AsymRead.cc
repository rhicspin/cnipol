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
   delete[] fMem;
   fFileStream.close();
}


/** */
RawDataProcessor::RawDataProcessor(string fname) : fFileName(fname), fFile(0),
   fMem(0),
   fFileStream(fFileName.c_str(), ios::binary), fSeenRecords()
{ //{{{
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
   printf("Stopped reading into memory: %f s, %f s\n", sw.RealTime(), sw.CpuTime());
} //}}}


/** */
RawDataProcessor::~RawDataProcessor()
{
}


/** */
void RawDataProcessor::ReadRecBegin(MseMeasInfoX* run)
{ //{{{
   cout << endl;
   Info("ReadRecBegin", "Start reading begin record from data file...");

   recordBeginStruct *recBegin;

   //int nRecs = fread(&recBegin, sizeof(recBegin), 1, fp);

   recBegin = (recordBeginStruct*) fMem;

   //if (nRecs == 1 && (recBegin.header.type & REC_TYPEMASK) == REC_BEGIN) {

   cout << "Begin of data stream Ver: " << recBegin->version << endl;
   cout << "Comment: "                  << recBegin->comment << endl;
   cout << "Time Stamp: "               << ctime(&recBegin->header.timestamp.time);
   cout << "Unix Time Stamp: "          << recBegin->header.timestamp.time << endl;

   gMeasInfo->fStartTime = recBegin->header.timestamp.time;
   gMeasInfo->fPolBeam   = (recBegin->header.type & REC_MASK_BEAM) >> 16;
   gMeasInfo->fPolStream = (recBegin->header.type & REC_MASK_STREAM) >> 20;

   //printf("fPolBeam:   %#x\n", gMeasInfo->fPolBeam);
   //printf("fPolStream: %#x, %#x, %#x\n", gMeasInfo->fPolStream, (UInt_t) recBegin->header.type, REC_MASK_STREAM);

   int polId = gMeasInfo->GetPolarimeterId(gMeasInfo->fPolBeam, gMeasInfo->fPolStream);

   if (polId < 0)
      printf("WARNING: Polarimeter ID will be defined by other means\n");

   stringstream sstr;

   // First, try to get polarimeter id from the data
   if (polId >= 0) {
      sstr << gMeasInfo->fPolId;
      gRunDb.fFields["POLARIMETER_ID"] = sstr.str();
      gRunDb.fPolId = gMeasInfo->fPolId;

   // if failed, get id from the file name
   } else if (gMeasInfo->GetPolarimeterId() >= 0) {
      sstr.str("");
      sstr << gMeasInfo->fPolId;
      gRunDb.fFields["POLARIMETER_ID"] = sstr.str();
      gRunDb.fPolId = gMeasInfo->fPolId;

   // see if the polarimeter id was set by command line argument
   } else if (gMeasInfo->fPolId >= 0) {
      sstr.str("");
      sstr << gMeasInfo->fPolId;
      gRunDb.fFields["POLARIMETER_ID"] = sstr.str();
      gRunDb.fPolId = gMeasInfo->fPolId;

   } else { // cannot proceed

      printf("ERROR: Unknown polarimeter ID\n");
      exit(-1);
   }

   sstr.str("");
   sstr << gMeasInfo->fStartTime;
   gRunDb.fFields["START_TIME"] = sstr.str();
   gRunDb.timeStamp = gMeasInfo->fStartTime; // should be always defined in raw data
   //gRunDb.Print();

   if (run) {
      run->polarimeter_id = gRunDb.fPolId;
      //mysqlpp::DateTime dt(gRunDb.timeStamp);
      mysqlpp::DateTime dt(gMeasInfo->fStartTime);
      run->start_time     = dt;
   }

   //} else
      //printf("ERROR: Cannot read REC_BEGIN record\n");

   Info("ReadRecBegin", "Stopped reading begin record from data file");
} //}}}


/**
 * This method is supposed to read all information about the measurement from
 * raw data. It does not read events.
 */
void RawDataProcessor::ReadMeasInfo(MseMeasInfoX &MeasInfo)
{ //{{{
   cout << endl;
   Info("ReadMeasInfo", "Start reading run info from data file...");

   TStopwatch sw;

   recordHeaderStruct *mHeader;

   char *mSeek = fMem;

   while (true) {

      //int nRecs = fread(&header, sizeof(recordHeaderStruct), 1, fp);

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
         fprintf(stdout, "Beam energy: %8.2f\n", gMeasInfo->GetBeamEnergy());
         fprintf(stdout, "RHIC beam:   %1d\n", gMeasInfo->fPolBeam);

         ProcessRecord( (recordBeamAdoStruct&) *rec);

         mSeek = mSeek + mHeader->len;
         continue;
      }

      // REC_POLADO
      if ((mHeader->type & REC_TYPEMASK) == REC_POLADO)
      {
         printf("Reading REC_POLADO record... size = %ld\n", mHeader->len);

         recordPolAdoStruct *rec = (recordPolAdoStruct*) mHeader;
         ProcessRecord( (recordPolAdoStruct&) *rec, MeasInfo);

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
         ProcessRecord( (recordpCTagAdoStruct&) *rec, MeasInfo);

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

      mSeek = mSeek + mHeader->len;
   }

   sw.Stop();

   Info("ReadMeasInfo", "Stopped reading run info from data file: %f s, %f s\n", sw.RealTime(), sw.CpuTime());
} //}}}


/** */
void RawDataProcessor::ReadDataFast()
{ //{{{
   cout << endl;
   Info("ReadDataFast", "Start reading events from data file...");

   TStopwatch sw;

   recordHeaderStruct *mHeader;

   UInt_t nTotalEvents = 0;
   UInt_t nReadEvents  = 0;

   char *mSeek = fMem;

   while (true) {

      //int nRecs = fread(&header, sizeof(recordHeaderStruct), 1, fp);

      if (mSeek > fMem + fMemSize - 1) break;
      //if (nRecs != 1) break;

      //printf("Currently consider record: %0#10x, len: %ld\n", (UInt_t) header.type, header.len);

      mHeader = (recordHeaderStruct*) mSeek;

      //mSeek = mSeek + mHeader->len;

      //printf("Currently consider record: %0#10x, len: %ld (MEM)\n", (UInt_t) mHeader->type, mHeader->len);

      if ((mHeader->type & REC_TYPEMASK) != REC_READAT)
      {
         //long offset = header.len - sizeof(recordHeaderStruct);
         //fseek(fp, offset, SEEK_CUR);

         mSeek = mSeek + mHeader->len;
         continue;
      }

      // We are here if rec type is REC_READAT
      //long delim = header.timestamp.delim;
      long delim = mHeader->timestamp.delim;

      //nEvent++;

      //if (nEvent > 1000) break;

      //size_t recSize = header.len - sizeof(recordHeaderStruct);
      size_t recSize = mHeader->len - sizeof(recordHeaderStruct);
      //unsigned char* buffer = new unsigned char[recSize];

      //nRecs = fread(buffer, recSize, 1, fp);

      char *mSeekAT = mSeek;

      mSeekAT += sizeof(recordHeaderStruct);

      mSeek = mSeek + mHeader->len;

      //recordReadATStruct *ATPtrF;
      recordReadATStruct *ATPtr;

      for (UInt_t i=0; i<recSize; ) {

         //ATPtrF     = (recordReadATStruct*) (buffer + i);
         ATPtr     = (recordReadATStruct*) (mSeekAT + i);
         unsigned chId   = ATPtr->subhead.siNum; // si number
         unsigned Events = ATPtr->subhead.Events + 1;

         //if (Events > 100) printf("events: %d, %d\n", siNum, Events);

         i += sizeof(subheadStruct) + Events*sizeof(ATStruct);

         for (unsigned j=0; j<Events; j++, nReadEvents++) {

            //printf("f: %d %d %d\n", ATPtrF->data[j].a, ATPtrF->data[j].t, ATPtrF->data[j].s);
            //printf("m: %d %d %d\n", ATPtr->data[j].a, ATPtr->data[j].t, ATPtr->data[j].s);

            if (gMaxEventsUser > 0 && nTotalEvents >= gMaxEventsUser) break;

            gAsymRoot->SetChannelEvent(ATPtr->data[j], delim, chId);

            // Use all events to fill pulser histograms
            if ( gAnaInfo->HasPulserBit() &&
                 gAsymRoot->fChannelEvent->PassCutEmptyBunch() &&
                 //gAsymRoot->fChannelEvent->PassCutNoise() &&
                 gAsymRoot->fChannelEvent->PassCutSiliconChannel() )
            {
               //gAsymRoot->fHists->d["pulser"]->FillPassOne(gAsymRoot->fChannelEvent);
               gAsymRoot->FillPassOne(kCUT_PASSONE_PULSER);
            }

            // Use only a fraction of events
            if (gRandom->Rndm() > gAnaInfo->fFastCalibThinout) continue;

            if (gAnaInfo->HasPmtBit() &&
                gAsymRoot->fChannelEvent->PassCutPmtChannel() &&
                gAsymRoot->fChannelEvent->PassCutPmtNoise() )
            {
               //gAsymRoot->fHists->d["pmt"]->FillPassOne(gAsymRoot->fChannelEvent);
               gAsymRoot->FillPassOne(kCUT_PASSONE_PMT);
            }

            if ( !gAsymRoot->fChannelEvent->PassCutNoise() )          continue;
            //if ( !gAsymRoot->fChannelEvent->PassCutEnabledChannel() ) continue;
            //if ( !gAsymRoot->fChannelEvent->PassCutPulser() )         continue;
            if ( !gAsymRoot->fChannelEvent->PassCutSiliconChannel() ) continue;
            //if ( !gAsymRoot->fChannelEvent->PassCutDepEnergyTime() ) continue;
            if ( gAsymRoot->fChannelEvent->PassCutEmptyBunch() )      continue;

            gAsymRoot->FillPassOne(kCUT_PASSONE_ALL);
            //gAsymRoot->PrintChannelEvent();

            nTotalEvents++;
         }
      }
   }

   sw.Stop();

   printf("Total events read:     %12d\n", nReadEvents);
   printf("Total events accepted: %12d\n", nTotalEvents);

   Info("ReadDataFast", "Stopped reading events from data file: %f s, %f s\n", sw.RealTime(), sw.CpuTime());
} //}}}


// read loop routine
void readloop(MseMeasInfoX &run)
{ //{{{
   cout << endl;
   gSystem->Info("readloop", "Start reading events from data file...");

   TStopwatch sw;

   static int READ_FLAG = 0;

   // Common structure for the data format
   static union {
       recordHeaderStruct      header;
       recordBeginStruct       begin;
       recordTagAdoStruct      tagado;
       recordReadWaveStruct    all;
       recordReadWave120Struct all120;
       recordReadATStruct      at;
       recordEndStruct         end;
       recordScalersStruct     scal;
       char                    buffer[BSIZE_OFFLINE*sizeof(int)];
       recordDataStruct        data;
   } rec;

   //polDataStruct            poldat;
   targetDataStruct         tgtdat1;
   targetDataStruct         tgtdat2;

   processEvent             event;

   FILE *fp;
   int   rval;
   int   recSize;
   int   flag = 0;     // exit from while when flag==1
   //int THINOUT = Flag.feedback ? gAnaInfo->fThinout : Nskip;

   // reading the data till its end ...
   if ((fp = fopen(gAnaInfo->GetRawDataFileName().c_str(), "r")) == NULL) {
      printf("ERROR: %s file not found. Force exit.\n", gAnaInfo->GetRawDataFileName().c_str());
      exit(-1);
   } else
      printf("\nFound file %s\n", gAnaInfo->GetRawDataFileName().c_str());


   while (flag == 0) {

      if (fread(&rec.header, sizeof(recordHeaderStruct), 1, fp) != 1)
         break;

      //printf("Currently consider record:   %0#10x, len: %ld\n", (UInt_t) rec.header.type, rec.header.len);

      if (feof(fp)) {
         fprintf(stdout, "Expected end of file\n");
         break;
      }

      if ( (UInt_t) rec.header.len > BSIZE_OFFLINE*sizeof(int)) {
         fprintf(stdout, "Not enough buffer d: %ld byte b: %u byte\n",
                 rec.header.len, sizeof(rec));
         break;
      }

      recSize = rec.header.len - sizeof(recordHeaderStruct);

      // Skip empty data records
      if (!recSize) continue;

      // Read rest of the structure
      rval = fread(&rec.begin.version, recSize, 1, fp);

      if (feof(fp)) {
         perror("Unexpected end of file");
         exit(1);
         break;
      }

      if (rval != 1) {
         fprintf(stdout, "IO error in stream (header) %s\n", strerror(errno));
         fprintf(stdout, "IO error in stream (header) %s\n", strerror(ferror(fp)));
         continue;
      }

      //printf("type: %#X\n", (rec.header.type & REC_TYPEMASK));

      //Long_t my_i;
      //Long_t *my_pointer;

      // Main Switch
      switch (rec.header.type & REC_TYPEMASK) {

      case REC_BEGIN:
         cout << "Data stream version: " << rec.begin.version << endl;
         cout << "Comment: "             << rec.begin.comment << endl;
         cout << "Date & Time: "         << ctime(&rec.begin.header.timestamp.time);
         cout << "Timestamp: "           << rec.begin.header.timestamp.time << endl;

         gMeasInfo->fStartTime = rec.begin.header.timestamp.time;
         gMeasInfo->fDataFormatVersion = rec.begin.version;

         gMeasInfo->fPolBeam   = (rec.header.type & REC_MASK_BEAM) >> 16;
         gMeasInfo->fPolStream = (rec.header.type & REC_MASK_STREAM) >> 20;

         //printf("fPolBeam:   %#x\n", gMeasInfo->fPolBeam);
         //printf("fPolStream: %#x, %#x, %#x\n", gMeasInfo->fPolStream, rec.header.type, REC_MASK_STREAM);

         // Configure colliding bunch patterns for PHENIX-BRAHMS and STAR
         PrepareCollidingBunchPattern();

         break;

      case REC_TAGADO:
         memcpy(&tgtdat1, &rec.tagado.data[0], sizeof(tgtdat1));
         memcpy(&tgtdat2, &rec.tagado.data[1], sizeof(tgtdat2));
         break; // later

      case REC_END:
         gMeasInfo->fStopTime = rec.end.header.timestamp.time;
         break;

      case REC_READRAW:
      case REC_READSUB:
         break;

      case REC_READALL:
         break;

      //case REC_SCALERS:
      //   fprintf(stdout, "Scaler readtime \n");
      //   fprintf(stdout, "Scaler End Time: %s\n", ctime(&rec.scal.header.timestamp.time));
      //   gMeasInfo->fStopTime = rec.header.timestamp.time;
      //   break;

      case REC_READAT:

        // Display configuration and configure active strips just once
        if (!READ_FLAG) {

           // Configure Active Strip Map
           gMeasInfo->ConfigureActiveStrip(mask.detector);
           gMeasInfo->PrintConfig();

           READ_FLAG = 1;

           if (gRunDb.run_status_s == "Junk") {
              cout << "\n This is a JUNK run. Force quit. Remove RUN_STATUS=Junk from run.db to process.\n\n";
              exit(-1);
           }
        }

        event.delim = rec.header.timestamp.delim;
        //int nreadsi     = 0;  // number of Si already read
        //int nreadbyte   = 0;  // number of bite already read

        recordReadATStruct *ATPtr;
        int Nevent;

        for (UInt_t i=0; i<rec.header.len - sizeof(recordHeaderStruct);) {

           ATPtr     = (recordReadATStruct*) (&rec.data.rec[i]);
           event.stN = ATPtr->subhead.siNum; // si number
           Nevent    = ATPtr->subhead.Events + 1;

           // count the total number of event records in raw file
           gAnaInfo->nEventsTotal += Nevent;

           i += sizeof(subheadStruct) + Nevent*sizeof(ATStruct);

           if (i > rec.header.len - sizeof(recordHeaderStruct)) {
              cout << "Broken record "<< rec.header.num << "("
                   << rec.header.len << " bytes). Last subhead: siNum= "
                   << event.stN << " Events= " << Nevent << endl;
              break;
           }

           //ds:
           //printf("Nevent(s): %d, si number: %d, gMaxEventsUser %d, Nread %d\n", Nevent,
           //       event.stN, gMaxEventsUser, Nread);

           for (int j=0; j<Nevent; j++, Nread++) {

              //ds: Skip events if already read enough events specified by user
              if (gMaxEventsUser > 0 && Nevtot >= gMaxEventsUser) break;

              if (Nread % gAnaInfo->fThinout == 0) {

                 //Nread++;
                 Nevtot++;

                 event.amp    = ATPtr->data[j].a;
                 event.tdc    = ATPtr->data[j].t;
                 event.intg   = ATPtr->data[j].s;
                 event.bid    = ATPtr->data[j].b;
                 event.tdcmax = ATPtr->data[j].tmax;
                 event.rev0   = ATPtr->data[j].rev0;
                 event.rev    = ATPtr->data[j].rev;

                 // Finer Target Position Resolution by counting stepping moter
                 // This feature became available after Run06
                 if (gMeasInfo->Run >= 6) {
                    cntr.revolution = event.delim*512 + event.rev*2 + event.rev0;

                    if (cntr.revolution > gMeasInfo->MaxRevolution)
                       gMeasInfo->MaxRevolution = cntr.revolution;

                    if (event.stN == 72 && event.delim != tgt.eventID) {
                       tgt.x += gAnaInfo->target_count_mm * (float)tgt.vector;
                       tgt.vector=-1;
                    }

                    if (event.stN == 72 || event.stN == 73) {
                        switch (event.stN) {
                        case 72:
                            tgt.eventID = event.delim;
                            ++cntr.tgtMotion;
                            break;
                        case 73:
                            tgt.vector = 1;
                            break;
                        }
                    }
                 }

                 gAsymRoot->SetChannelEvent(event);

                 if (gAnaInfo->fSaveTrees.any()) { gAsymRoot->AddChannelEvent(); }

                 //cout << " i "            << i
                 //     << " Nevent "       << Nevent
                 //     << " St "           << event.stN
                 //     << " amp "          << event.amp
                 //     << " tdc "          << event.tdc
                 //     << " bid "          << event.bid
                 //     << " rev0 "         << event.rev0
                 //     << " rev  "         << event.rev
                 //     << " revolution #=" << cntr.revolution << endl;

                 // Raw histograms
                 //ds: Why don't we save raw histograms in regular passes?
                 //if (!Flag.feedback && gAnaInfo->RAWHISTOGRAM)
                 //if (event.stN < 72) {
                 //   bunch_dist_raw -> Fill(event.bid);
                 //   strip_dist_raw -> Fill(event.stN);
                 //   tdc_raw        -> Fill(event.tdc);
                 //   adc_raw        -> Fill(event.amp);
                 //   tdc_vs_adc_raw -> Fill(event.amp,event.tdc);

                 //   //ds: Don't we need to compare absolute value of gFillPattern[bid] with 1?
                 //   if (fabs(gFillPattern[event.bid]) != 1)
                 //      tdc_vs_adc_false_bunch_raw->Fill(event.amp, event.tdc);
                 //}

                 // process event for following case:
                 //    fill pattern     = 1
                 //    Calibration mode = 1
                 // also process only if strip is active
                 //if (event.stN >= NSTRIP)
                    //printf("channel111: %d, %d\n", event.stN, gMeasInfo->ActiveStrip[event.stN]);
                    //printf("channel111: %d\n", event.stN);

                 //if ( gFillPattern[event.bid] == 1 || gAnaInfo->HasAlphaBit() == 1) // || event.stN >= 72) ) //&&
                 //     //gMeasInfo->ActiveStrip[event.stN] )
                 //{
                    event_process(&event);
                 //}

                 if (Nevtot%50000==0) {

                    //ds: Can we simplify this by leaving only one report?
                    //if (Flag.feedback){
                    //   printf("Feedback Mode Ncounts = %ld \r", Nread) ;
                    //} else {
                       printf("%.3f: Proccesing Ncounts = %u \r", gMeasInfo->RUNID, Nevtot);
                    //}

                    fflush(stdout);
                 }
              }
           }
        }

        break;

      //case REC_SUBRUN:
      //   printf("Processing record: REC_SUBRUN\n");
      //   break;

      default:    // unknown record
         fprintf(stdout, "Encountered Unknown Record \"%#x\"\n",
            (UInt_t) rec.header.type & REC_TYPEMASK);
         break;
      }
   }

   fclose(fp);

   // Post processing
   if (gAnaInfo->HasNormalBit())
      end_process(run);

   fprintf(stdout, "End of data stream \n");
   fprintf(stdout, "End Time: %s\n", ctime(&gMeasInfo->fStopTime));
   fprintf(stdout, "Carbons found: %ld \n", Nevcut);
   //fprintf(stdout, "Data Comment: %s\n", rec.end.comment);
   fprintf(stdout, "Total events in file %d\n", gAnaInfo->nEventsTotal);
   fprintf(stdout, "First %d events processed\n", Nread);
   fprintf(stdout, "%d events saved\n", Nevtot);

   gAnaInfo->nEventsProcessed = Nevtot;

   mysqlpp::DateTime dt(gMeasInfo->fStopTime);
   run.stop_time = dt;

   // Add info to database entry
   stringstream sstr;

   sstr.str(""); sstr << gAnaInfo->nEventsTotal;
   gRunDb.fFields["NEVENTS_TOTAL"] = sstr.str();
   run.nevents_total = gAnaInfo->nEventsTotal;

   sstr.str(""); sstr << gAnaInfo->nEventsProcessed;
   gRunDb.fFields["NEVENTS_PROCESSED"] = sstr.str();
   run.nevents_processed = gAnaInfo->nEventsProcessed;

   sstr.str("");
   sstr << gMeasInfo->GetBeamEnergy();

   if (gAnaInfo->HasAlphaBit()) {
      gRunDb.fFields["BEAM_ENERGY"] = "0";
      run.beam_energy = 0;
   } else {
      gRunDb.fFields["BEAM_ENERGY"] = sstr.str();
      run.beam_energy = gMeasInfo->GetBeamEnergy();
   }

   // Some incompleted run don't even have REC_READAT flag. Force PrintConfig.
   if (!Nread && !READ_FLAG) {
      gMeasInfo->PrintConfig();

      if (gRunDb.run_status_s == "Junk") {
         cout << "\n This is a JUNK run. Force quit. Remove RUN_STATUS=Junk from run.db to process.\n\n";
         exit(-1);
      }
   }

   sw.Stop();

   gSystem->Info("readloop", "Stopped reading events from data file: %f s, %f s\n", sw.RealTime(), sw.CpuTime());
} //}}}


/** */
void UpdateRunConst(TRecordConfigRhicStruct *ci)
{ //{{{
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
} //}}}


// Description : Decorde target infomation.
//             : presently the target ID is assumed to be appear at the last of
//             : character string poldat.targetIdS.
// Input       : polDataStruct poldat
void DecodeTargetID(const polDataStruct &poldat, MseMeasInfoX &run)
{ //{{{
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
      run.target_orient       = 'V';
   }

   if (str.find('H') != string::npos) {
      gMeasInfo->fTargetOrient = 'H';
      run.target_orient       = 'H';
   }

   //cout << "target id str: " << str << " " << run.target_orient << endl;

   stringstream sstr;
   //int target_id;
   sstr << gMeasInfo->fTargetId;
   //sstr >> target_id;
   sstr >> run.target_id;
   //run.target_id = target_id;

   // Restore Vertical or Horizontal target information
   // in case this information isn't decorded correctly
   // within REC_PCTARGET routine. If the target is horizontal,
   // then mask 90 degree detector.
   //if (gMeasInfo->fTargetOrient == '-') {

   //   if (str.find("Vert") == 0 || str.find("V") == 0) {
   //      gMeasInfo->fTargetOrient = 'V';
   //      run.target_orient       = 'V';
   //      tgt.VHtarget            = 0;
   //   }

   //   if (str.find("Horz") == 0 || str.find("H") == 0) {
   //      gMeasInfo->fTargetOrient = 'H';
   //      run.target_orient       = 'H';
   //      tgt.VHtarget            = 1;
   //      // This is too late to reconfigure strip mask because this routine is
   //      // executed at the end of event loop. Too bad. /* March 5,'09 IN */
   //      //      mask.detector = 0x2D;
   //      //      gMeasInfo->ConfigureActiveStrip(mask.detector);
   //   }
   //}
} //}}}


/** */
void ProcessRecordPCTarget(const pCTargetStruct &rec, MseMeasInfoX &run)
{ //{{{
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
            //run.target_orient      = 'V';
            cout << "Vertical Target in finite position - ???" << endl;

         } else if (tgt_identifyH) {
            tgt.VHtarget           = 1;
            //gMeasInfo->fTargetOrient = 'H';
            //run.target_orient      = 'H';
            cout << "Horizontal Target in finite position - ???" << endl;

         } else {
            cout << "Warning: Target infomation cannot be recognized - ???" << endl;
         }

         tgt.x       = tgt.Rotary[k][tgt.VHtarget] * gAnaInfo->target_count_mm;
         tgt.Time[i] = k;
         tgt.X[i]    = tgt.x;

         printf("%8d %8d %8d %8d %12.3f %12.3f %12.3f\n", i, k, nTgtIndex,
                 TgtIndex[k], tgt.X[TgtIndex[k]],
                 tgt.Rotary[k][0]*gAnaInfo->target_count_mm,
                 tgt.Rotary[k][1]*gAnaInfo->target_count_mm);
      } else {

         TgtIndex[k] = i;

         if (tgt.Rotary[k][1] != tgt.Rotary[k-1][1] ||
             tgt.Rotary[k][0] != tgt.Rotary[k-1][0] )
         {
            TgtIndex[k]                 = ++i;
            tgt.X[TgtIndex[k]]          = tgt.Rotary[k][tgt.VHtarget] * gAnaInfo->target_count_mm;
            tgt.Time[TgtIndex[k]]       = float(k);
            tgt.Interval[TgtIndex[k-1]] = tgt.Time[TgtIndex[k]] - tgt.Time[TgtIndex[k-1]];
            ++nTgtIndex;

            printf("%8d %8d %8d %8d %12.3f %12.3f %12.3f\n", i, k, nTgtIndex,
               TgtIndex[k], tgt.X[TgtIndex[k]],
               tgt.Rotary[k][0]*gAnaInfo->target_count_mm,
               tgt.Rotary[k][1]*gAnaInfo->target_count_mm);

            //++i;
         }
      }

      // target position array including static target motion
      tgt.all.x[k] = tgt.Rotary[k][tgt.VHtarget] * gAnaInfo->target_count_mm ;
   }

   printf("Number of delimiters: %4d\n", gNDelimeters);
   printf("nTgtIndex: %d\n", nTgtIndex);

   tgt.Print();

   // define target histograms (hbook)
   //tgtHistBook();

   // disable 90 degrees detectors for horizontal target 0x2D={10 1101}
   //if (tgt.VHtarget) mask.detector = 0x2D;
} //}}}


// Method name : PrepareCollidingBunchPattern(gMeasInfo->fPolBeam)
// Description : Configure phx.bunchpat[] and str.bunchpat[] arrays only for colliding bunches
void PrepareCollidingBunchPattern()
{ //{{{
   for (int i=0; i<NBUNCH; i++){
      phx.bunchpat[i] = 1; // PHENIX bunch patterns
      str.bunchpat[i] = 1; // STAR bunch patterns
   }

   if (gMeasInfo->fPolBeam == 1) { // Yellow Beam
      for (int j=31; j<40; j++) str.bunchpat[j] = phx.bunchpat[j+40] = 0;
   } else if (gMeasInfo->fPolBeam == 2){ // Blue Beam
      for (int j=31; j<40; j++) phx.bunchpat[j] = str.bunchpat[j+40] = 0;
   }

   cout << "===== Colliding Bunch pattern =======" << endl;
   cout << " IP2, IP8:  " ; for (int i=0; i<NBUNCH; i++) cout << phx.bunchpat[i] ; cout << endl;
   cout << " IP6, IP10: " ; for (int i=0; i<NBUNCH; i++) cout << str.bunchpat[i] ; cout << endl;
   cout << "=====================================" << endl;
} //}}}


/** */
void ProcessRecord(const recordConfigRhicStruct &rec)
{ //{{{
   gConfigInfo = (recordConfigRhicStruct *) malloc(sizeof(recordConfigRhicStruct) +
             (rec.data.NumChannels - 1) * sizeof(SiChanStruct));

   //XXX printf("TTT: %d\n", rec.data.NumChannels);

   memcpy(gConfigInfo, &rec, sizeof(recordConfigRhicStruct) +
          (rec.data.NumChannels - 1) * sizeof(SiChanStruct));

   // when we mandatory provide cfg info -- derpecated
   //if (gAnaInfo->RECONFMODE == 1) {
   //   reConfig(gConfigInfo);
   //}

   // Recalculate Run constants
   UpdateRunConst(gConfigInfo);
} //}}}


/** */
void ProcessRecord(const recordPolAdoStruct &rec, MseMeasInfoX &MeasInfo)
{ //{{{
   if (!gAnaInfo->HasAlphaBit()) DecodeTargetID( (polDataStruct &) rec.data, MeasInfo);
} //}}}


/** */
void ProcessRecord(const recordpCTagAdoStruct &rec, MseMeasInfoX &run)
{ //{{{
   if (!gAnaInfo->HasTargetBit() || gAnaInfo->HasAlphaBit()) return;

   gNDelimeters  = (rec.header.len - sizeof(rec.header)) / sizeof(pCTargetStruct);

   //long *pointer = (long *) &rec.buffer[sizeof(rec.header)];
   ProcessRecordPCTarget((pCTargetStruct &) rec.data, run);

} //}}}


/** */
void ProcessRecord(const recordWFDV8ArrayStruct &rec)
{ //{{{
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
} //}}}


/** */
void ProcessRecord(const recordCountRate &rec)
{ //{{{
   UInt_t size = (rec.header.len - sizeof(rec.header))/(sizeof(long));
   Long_t *pointer = (Long_t *) rec.data;
   //Double_t *pointer = (Double_t *) &rec.buffer[sizeof(rec.header)];

   printf("len, size: %ld, %d\n", rec.header.len, size);

   //for (UInt_t i=0; i<size; i++) {
   //   printf("countrate: i: %d, %ld\n", i, *(pointer+i));
   //}

   gAsymRoot->FillProfileHists(size, pointer);
} //}}}


/** */
void ProcessRecord(const recordWcmAdoStruct &rec)
{ //{{{
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
} //}}}


/** */
void ProcessRecord(const recordBeamAdoStruct &rec)
{ //{{{
   //memcpy(&beamdat, &rec.beamado.data, sizeof(beamdat));
   //beamDataStruct beamdat;

   //gMeasInfo->SetBeamEnergy(beamdat.beamEnergyM);
   //fprintf(stdout, "Beam Energy: %8.2f\n", gMeasInfo->GetBeamEnergy());
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
   if (gMeasInfo->NDisableBunch) {
      cout << "\nFill pattern has been overwritten again" << endl;
      recover.MaskFillPattern();
   }

   // Print active Bunch
   //cout << "\nActive bunches: " << endl;

   //for (int i=0; i<NBUNCH; i++) {
   //   if (i%10 == 0) cout << " ";
   //   ActiveBunch[i] = gFillPattern[i];
   //   cout << ActiveBunch[i];
   //}

   //cout << endl;

   gMeasInfo->PrintBunchPatterns();
} //}}}
