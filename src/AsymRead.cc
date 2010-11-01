//  Asymmetry Analysis of RHIC pC Polarimeter
//  file name :   AsymRead.cc
//
//
//  Author    :   Itaru Nakagawa
//  Creation  :   10/17/2005
//

#include "AsymROOTGlobals.h"

#include "AsymRead.h"

#define REC_BEAMMASK 0x00030000
void PrintPattern(char*);
void DecodeTargetID(polDataStruct poldat);
void PrepareCollidingBunchPattern();


// =================
// read loop routine
// =================
int readloop()
{
   long int * pointer ;
   int rval;
   int nev;
   long int Nevent, i;
   int flag;     // exit from while when flag==1
   int THINOUT = Flag.feedback ? dproc.thinout : Nskip;
 
   static int READ_FLAG=0;
 
   // Common structure for the data format
   static union {
       recordHeaderStruct     header;
       recordBeginStruct      begin;
       recordPolAdoStruct     polado;
       recordTagAdoStruct     tagado;
       recordBeamAdoStruct    beamado;
       recordConfigRhicStruct cfg;
       recordReadWaveStruct   all;
       recordReadWave120Struct all120;
       recordReadATStruct      at;
       recordWFDV8ArrayStruct  wfd;
       recordEndStruct        end;
       recordScalersStruct    scal;
       recordWcmAdoStruct     wcmado;
       char                   buffer[BSIZE*sizeof(int)];
       recordDataStruct       data;
   } rec;
 
   recordConfigRhicStruct  *cfginfo;
   polDataStruct poldat;
   beamDataStruct beamdat;
   targetDataStruct tgtdat1;
   targetDataStruct tgtdat2;
   wcmDataStruct wcmdat;
   pCTargetStruct pctdat;
   processEvent event;
   AsymRecover recover;
 
   int bid,pat;
   char pattern[1];
   int nreadsi;  // number of Si already read
   int nreadbyte;
 
   FILE *fp;
 
   // -------------------------------------------------
   flag = 0;
 
   // reading the data till its end ...
   if ((fp = fopen(datafile,"r")) == NULL) {
     printf("ERROR: %s file not found. Fource exit.\n",datafile);;
     exit(-1);
   }
 
   while (flag==0) {

       if (fread(&rec.header, sizeof(recordHeaderStruct), 1, fp)!=1){
           break;
       }
 
       if (feof(fp)) {
           fprintf(stdout,"Expected end of file\n");
           break;
       }
 
       if (rec.header.len > BSIZE*sizeof(int)){
           fprintf(stdout,"Not enough buffer d: %d byte b: %d byte\n",
                   rec.header.len, sizeof(rec));
           break;
       }
 
       // Read rest of the structure
       rval = fread(&rec.begin.version, rec.header.len -
                    sizeof(recordHeaderStruct),1,fp);
       if (feof(fp)) {
           perror("Unexpected end of file");
           exit(1);
           break;
       }
 
       if (rval != 1) {
           fprintf(stdout,"IO error in stream (header) %s\n",strerror(errno));
           continue;
       }
 
       //ds
       //printf("type: %#X\n", (rec.header.type & REC_TYPEMASK));
 
       // Distinguish between blue & yellow beam.
       // Presently, it overwrites pre-defined runinfo.RHICBeam
       // in GetPolarimetryID_and_RHICBeam(char RunID[]).
       // Could be used as cross check for the future
       switch (rec.header.type & REC_BEAMMASK) {
       case REC_YELLOW:
         runinfo.RHICBeam=1 ;
         break;
       case REC_BLUE:
         runinfo.RHICBeam=0 ;
         break;
       }
 
       //=============================================================
       //                          Main Switch
       //=============================================================
       switch (rec.header.type & REC_TYPEMASK) {
       case REC_BEGIN:
         if (!ReadFlag.RECBEGIN){
           cout << "Begin of data stream Ver: " <<rec.begin.version<<endl;
           cout << "Comment: " << rec.begin.comment <<endl;
           cout << "Time Stamp: " << ctime(&rec.begin.header.timestamp.time);
           cout << "Unix Time Stamp: " << rec.begin.header.timestamp.time << endl;
           runinfo.StartTime=rec.begin.header.timestamp.time ;
 
           // Configure colliding bunch patterns for PHENIX-BRAHMS and STAR
           PrepareCollidingBunchPattern();
 
           ReadFlag.RECBEGIN = 1;
         }
         break;
 
       case REC_POLADO:
           memcpy(&poldat, &rec.polado.data,sizeof(poldat));
           DecodeTargetID(poldat);
           break;
 
       case REC_TAGADO:
           memcpy(&tgtdat1, &rec.tagado.data[0], sizeof(tgtdat1));
           memcpy(&tgtdat2, &rec.tagado.data[1], sizeof(tgtdat2));
           break; // later
 
       case REC_PCTARGET:
         if (!ReadFlag.PCTARGET){
           ndelim  = (rec.header.len - sizeof(rec.header))/(4*sizeof(long));
           pointer = (long *)&rec.buffer[sizeof(rec.header)] ;
           --pointer;  i=0;
 
           printf("    index   total   x-pos   y-pos \n");
           for (int k=0; k<ndelim ; k++) {
               tgt.Linear[k][1] = *++pointer ; // Horizontal target
               tgt.Rotary[k][1] = *++pointer ;
               tgt.Linear[k][0] = *++pointer ; // Vertical target
               tgt.Rotary[k][0] = *++pointer ;
 
               // force 0 for +/-1 tiny readout as target position.
               if (abs(tgt.Rotary[k][1])<=1) tgt.Rotary[k][1]=0;
 
               // identify Horizontal or Vertical target from the first target rotary position readout.
               if (k==0) {
                 // From Run09 on, the horizontal/vertical targets are identified by linear motion
                 long int tgt_identifyV = runinfo.RUNID < 10040 ? tgt.Rotary[k][0] : tgt.Linear[k][0];
                 long int tgt_identifyH = runinfo.RUNID < 10040 ? tgt.Rotary[k][1] : tgt.Linear[k][1];
                   if (((!tgt.Rotary[k][0])&&(!tgt.Rotary[k][1]))||((tgt.Rotary[k][0])&&(tgt.Rotary[k][1]))){
                       cerr << "ERROR: no target rotary info. Don't know H/V target" << endl;
                       runinfo.target='-';
                   }
                   if (tgt_identifyV) {
                       tgt.VHtarget = 0;
                       runinfo.target = 'V';
                       cout << "Vertical Target in finite position" << endl;
                   } else if (tgt_identifyH) {
                       tgt.VHtarget = 1;
                       runinfo.target = 'H';
                       cout << "Horizontal Target in finite position" << endl;
                   }else{
                     cout << "Warning: Target infomation cannot be recognized.." << endl;
                   }
 
                   tgt.x = tgt.Rotary[k][tgt.VHtarget] * dproc.target_count_mm;
                   tgt.Time[i] = k;
                   tgt.X[i] = tgt.x;
                   printf("@%8d%8d%7.1f%7.1f\n",
                          i, k, tgt.Rotary[k][0]*dproc.target_count_mm, tgt.Rotary[k][1]*dproc.target_count_mm);
               } else {

                 TgtIndex[k] = i;

                 if ((tgt.Rotary[k][1] != tgt.Rotary[k-1][1])||(tgt.Rotary[k][0] != tgt.Rotary[k-1][0])) {
                   TgtIndex[k] = ++i ;
                   tgt.X[TgtIndex[k]] = tgt.Rotary[k][tgt.VHtarget] * dproc.target_count_mm;
                   tgt.Time[TgtIndex[k]] = float(k);
                   tgt.Interval[TgtIndex[k-1]] = tgt.Time[TgtIndex[k]] - tgt.Time[TgtIndex[k-1]];
                   ++nTgtIndex;

                   if (nTgtIndex>TGT_OPERATION) //runinfo.TgtOperation=" scan";
                      strcpy(runinfo.TgtOperation, " scan");

                   printf("@%8d%8d%7.1f%7.1f\n",
                          i, k, tgt.Rotary[k][0]*dproc.target_count_mm, tgt.Rotary[k][1]*dproc.target_count_mm);
                 }
               }
               // target position array including static target motion
               tgt.all.x[k] = tgt.Rotary[k][tgt.VHtarget] * dproc.target_count_mm ;
 
           } // end-of-for(ndelim)-loop
 
           printf("Number of Delimiters :%4d\n",ndelim);
           ReadFlag.PCTARGET = 1;
         }
 
         // define target histograms
         tgtHistBook();
 
         // disable 90 degrees detectors for horizontal target 0x2D={10 1101}
         if (tgt.VHtarget) mask.detector = 0x2D;
 
         break;
 
 
       case REC_WCMADO:
         if (!ReadFlag.WCMADO){
           fprintf(stdout,"Reading WCM information\n");
           memcpy(&wcmdat, &rec.wcmado.data,sizeof(wcmdat));
           for (bid=0;bid<120;bid++){
               wcmdist[bid] = wcmdat.fillDataM[bid*3];
               runinfo.WcmSum += wcmdist[bid]*fillpat[bid];
           }
           runinfo.WcmAve  =  runinfo.WcmSum/float(runinfo.NActiveBunch);
           ReadFlag.WCMADO = 1;
         }
         break;
 
       case REC_BEAMADO:
         if (!ReadFlag.BEAMADO){
           fprintf(stdout,"Reading Beam Information\n");
           memcpy(&beamdat, &rec.beamado.data, sizeof(beamdat));
           runinfo.BeamEnergy = beamdat.beamEnergyM;
           fprintf(stdout,"Beam Energy : %8.2f\n",runinfo.BeamEnergy);
           fprintf(stdout,"RHIC Beam : %1d\n",runinfo.RHICBeam);
 
           // Add inj_tshift for injection measurements
           if (runinfo.BeamEnergy < 30) dproc.tshift+=dproc.inj_tshift;
 
           for (bid=0;bid<120;bid++) {
               pat = beamdat.polarizationFillPatternS[bid*3];
               if (pat>0) {
                   spinpat[bid] = 1;
               } else if (pat<0) {
                   spinpat[bid] = -1;
               } else {
                   spinpat[bid] = 0;
               }
               pat = beamdat.measuredFillPatternM[bid*3];
               if (pat>0) {
                   fillpat[bid] = 1;
                   runinfo.NFilledBunch++;
               } else if (pat<0) {
                   fillpat[bid] = -1;
                   runinfo.NFilledBunch++;
               } else {
                   fillpat[bid] = 0;
               }
           }
 
           char pattern[3][5] ;
 
           //===================================================================
           //   Print Spin Pattern and Recover Spin Pattern by User Defined ones
           //===================================================================
           cout << "Spin Pattern Used : " << endl;
           PrintPattern("spin");
           if (Flag.spin_pattern>=0) {
             recover.OverwriteSpinPattern(Flag.spin_pattern);
             PrintPattern("spin");
           }
 
 
           //===================================================================
           //   Print Fill Pattern and Recover Fill Pattern by User Defined ones
           //===================================================================
           cout << "Fill Pattern Used : " << endl;
           PrintPattern("fill");
           if (Flag.fill_pattern>=0) {
             recover.OverwriteFillPattern(Flag.fill_pattern);
             PrintPattern("fill");
           }
 
           //===========================================================
           //       Masking on bad bunches
           //===========================================================
           if (runinfo.NDisableBunch) {
             recover.MaskFillPattern();
             cout << "Masked Fill Pattern : " << endl;
             PrintPattern("fill");
           }
 
           // Active Bunch
           for (int k=0; k<NBUNCH; k++) {
             ActiveBunch[k]=fillpat[k];
             cout << ActiveBunch[k] ;
           }
           cout << endl;
           runinfo.NActiveBunch = runinfo.NFilledBunch - runinfo.NDisableBunch;
 
           ReadFlag.BEAMADO = 1;
 
         }
 
         break;
 
       case REC_END:
          //ds: useful info should be always reported
          //if (!Flag.feedback){
          fprintf(stdout,"End of data stream \n");
          fprintf(stdout,"End Time: %s\n",
                  ctime(&rec.end.header.timestamp.time));
          fprintf(stdout,"%ld records found\n", Nread);
          fprintf(stdout,"Total number of counts \n");
          fprintf(stdout,"%ld Carbons are found in\n",Nevcut);
          fprintf(stdout,"%ld Total events of intended fill pattern\n",
                  Nevtot);
          fprintf(stdout,"Data Comment: %s\n", rec.end.comment);
          runinfo.StopTime=rec.end.header.timestamp.time;
          //}

          if (end_process(cfginfo)!=0) {
            fprintf(stdout, "Error at end process \n");
          }

          break;
 
       case REC_READRAW:
           break; // later
 
       case REC_READSUB:
           break; // later
 
       case REC_READALL:
           break; // later
 
       case REC_WFDV8SCAL:
           break; // later
 
       case REC_SCALERS:
         //ds: useful info should be always reported
         //if (!Flag.feedback){
           fprintf(stdout,"Scaler readtime \n");
           fprintf(stdout,"Scaler End Time: %s\n",
                   ctime(&rec.scal.header.timestamp.time));
         //}
         break;
 
       case REC_READAT:
 
         //ds: Skip this record if already read enough events specified by user
         if (gMaxEventsUser >= 0 && Nread >= gMaxEventsUser) break;
 
         // Display configuration and configure active strips just once
         if (!READ_FLAG){
 
           // Configure Active Strip Map
           ConfigureActiveStrip(mask.detector);
 
           if (printConfig(cfginfo)!=0) {
               perror(" error in printing configuration");
               exit(1);
           }
           READ_FLAG=1;
           if (rundb.run_status_s=="Junk") {
               cout << "\n This is a JUNK run. Force quit. Remove RUN_STATUS=Junk from run.db to process.\n\n";
               exit(-1);
           }
         }
 
         event.delim = rec.header.timestamp.delim;
         nreadsi     = 0;  // number of Si already read
         nreadbyte   = 0;  // number of bite already read
 
         recordReadATStruct *ATPtr;
 
         for (int i=0; i< rec.header.len - sizeof(recordHeaderStruct);) {
 
            ATPtr = (recordReadATStruct *) (&rec.data.rec[i]);
            event.stN = ATPtr -> subhead.siNum; // si number
            Nevent = ATPtr -> subhead.Events + 1;
            i += sizeof(subheadStruct) + Nevent*sizeof(ATStruct);
            if (i > rec.header.len - sizeof(recordHeaderStruct)) {
               cout << "Broken record "<< rec.header.num << "("
                    << rec.header.len << " bytes). Last subhead: siNum= "
                    << event.stN << " Events= " << Nevent << endl;
               break;
            }
 
            //ds:
            //printf("Nevent(s): %d, si number: %d\n", Nevent, event.stN);
 
            for (int j=0; j<Nevent; j++, Nread++) {

               //ds: Skip events if already read enough events specified by user
               if (gMaxEventsUser >= 0 && Nread >= gMaxEventsUser) break;

               if ( Nread%THINOUT == 0) {

                   event.amp    = ATPtr -> data[j].a;
                   event.tdc    = ATPtr -> data[j].t;
                   event.intg   = ATPtr -> data[j].s;
                   event.bid    = ATPtr -> data[j].b;
                   event.tdcmax = ATPtr -> data[j].tmax;
                   event.rev0   = ATPtr -> data[j].rev0;
                   event.rev    = ATPtr -> data[j].rev;

                   // Finer Target Position Resolution by counting stepping moter
                   // This feature became available after Run06
                   if (runinfo.Run>=6){
                      cntr.revolution=event.delim*512 + event.rev*2 + event.rev0 ;
                      if (cntr.revolution>runinfo.MaxRevolution)
                          runinfo.MaxRevolution = cntr.revolution;

                      if ((event.stN==72)&&(event.delim!=tgt.eventID)){
                          tgt.x += dproc.target_count_mm * (float)tgt.vector ;
                          tgt.vector=-1;
                      }

                      if ((event.stN==72)||(event.stN==73)) {
                          switch (event.stN){
                          case 72:
                              tgt.eventID=event.delim;
                              ++cntr.tgtMotion;
                              break;
                          case 73:
                              tgt.vector = 1;
                              break;
                          }// switch (event.stN)
 
                      }// event.stN==72,73
 
                   }// if (runinfo.Run>=6)
 
                   if (dproc.SAVETREES.any()) { gMyRoot.AddChannelEvent(event); }
 
                   //cout << " i " << i
                   //  << " Nevent " << Nevent
                   //  << " St " << event.stN
                   //  << " amp " << event.amp
                   //  << " tdc " << event.tdc
                   //  << " bid " << event.bid
                   //  << " rev0 " << event.rev0
                   //  << " rev  " << event.rev
                   //  << " revolution #=" << cntr.revolution << endl;
 
                   // Raw histograms
                   //ds: Why don't we save raw histograms in regular passes?
                   //if ((!Flag.feedback)&&(dproc.RAWHISTOGRAM)) {
                   if (event.stN < 72){
                      bunch_dist_raw -> Fill(event.bid);
                      strip_dist_raw -> Fill(event.stN);
                      tdc_raw        -> Fill(event.tdc);
                      adc_raw        -> Fill(event.amp);
                      tdc_vs_adc_raw -> Fill(event.amp,event.tdc);

                      if (fillpat[event.bid]!=1) tdc_vs_adc_false_bunch_raw -> Fill(event.amp,event.tdc);
                   }
                   //}
 
                   // process event for following case:
                   //      fill pattern = 1
                   //      Calibration mode = 1
                   if ((fillpat[event.bid]==1)||(dproc.CMODE==1)||(event.stN>=72)) {
                      // process only if strip is active
                      if (runinfo.ActiveStrip[event.stN]){
 
                         // Event Processing
                         if (event_process(&event,cfginfo)!=0)
                            fprintf(stdout,"Error event process Si:%d Ev:%d\n", nreadsi, j);
                      }
                   }
               }
 
               if (Nread%10000==0) {

                  //ds: Can we simplify this by leaving only one report?
                  //if (Flag.feedback){
                  //   printf("Feedback Mode Ncounts = %ld \r", Nread) ;
                  //} else {
                     printf("%.3f : Proccesing  Ncounts   = %ld \r", runinfo.RUNID, Nread);
                  //}

                  fflush(stdout);
               }

            } // for loop over Nread
         }
         break;
 
       case REC_RHIC_CONF:
         if (!ReadFlag.RHICCONF){
           fprintf(stdout,"Read configure information\n");
           cfginfo = (recordConfigRhicStruct *)
               malloc(sizeof(recordConfigRhicStruct)+
                      (rec.cfg.data.NumChannels-1)*sizeof(SiChanStruct));
 
           memcpy(cfginfo, &rec.cfg, sizeof(recordConfigRhicStruct)+
                  (rec.cfg.data.NumChannels-1)*sizeof(SiChanStruct));
           // when we mandatory provide cfg info
           if (dproc.RECONFMODE==1) {
               reConfig(cfginfo);
           }
 
           // calculate Run Constants
           calcRunConst(cfginfo);
           if (dproc.MESSAGE == 1) exit(0);
           ReadFlag.RHICCONF = 1;
         }
         
         gMyRoot.fEventConfig->fConfigInfo = cfginfo;
         //printf("fConfigInfo: %#x\n", gMyRoot.fEventConfig->fConfigInfo);

         break;
 
       default:    // unknown record
           fprintf(stdout,"Encountered Unknown Record 0x%x\n",
                   rec.header.type & REC_TYPEMASK);
           break;
       }
   }
 
   //fprintf(stdout,"End of loop after %d events.\n", Nread);

   gMyRoot.fEventConfig->fRunInfo = &runinfo;
   //gMyRoot.fEventConfig->PrintAsPhp();
 
   // Some incompleted run don't even have REC_READAT flag. Force printConfig.
   if ((!Nread)&&(!READ_FLAG)) {
     printConfig(cfginfo);
     if (rundb.run_status_s=="Junk") {
       cout << "\n This is a JUNK run. Force quit. Remove RUN_STATUS=Junk from run.db to process.\n\n";
       exit(-1);
     }
   }
 
   return(0);
}



//
// Class name  :
// Method name : void PrintPattern(char* Mode)
//
// Description : print out spin (Mode=0), fill (Mode=1) pattern
// Input       : Mode
// Return      :
//             :
//
void
PrintPattern(char * Mode){

  char pattern[3][5] ;
  sprintf(pattern[0],"-");
  sprintf(pattern[1],".");
  sprintf(pattern[2],"+");

  for (int i=0;i<RHIC_MAX_FILL;i++) {

    if (i%10 == 0) cout << " ";

    if (Mode=="spin") {
      cout << pattern[spinpat[i]+1];
    }else if (Mode=="fill") {
      cout << pattern[fillpat[i]+1];
    }

  }

  cout <<endl;

  return;

}




//
// Class name  :
// Method name : calcRunConst
//
// Description : caluclate Run Constants
// Input       : recordConfigRhicStruct *cfginfo
// Return      :
//             :
//
void
calcRunConst(recordConfigRhicStruct *cfginfo){

  // Determine the TDC count unit (ns/channel)
  runconst.Ct = cfginfo->data.WFDTUnit/2.;

  // ToF Distance [cm]
  float L = cfginfo->data.TOFLength;

  // C12 Mass
  float m = MASS_12C;

  // speed of light
  float c = C_CMNS;

  // Calculate kinematic constants for the Run
  runconst.E2T = M_SQRT1_2/c * sqrt(m) * L;
  runconst.M2T = L/2/M_SQRT2/c/sqrt(m);         // [ns/keV^(1/2)]
  runconst.T2M = 2*c*c/L/L;

  printf("Kinematic Const. E2T =%10.2f\n",runconst.E2T);
  printf("Kinematic Const. M2T =%10.2f\n",runconst.M2T);
  printf("Kinematic Const. T2M =%10.2f\n",runconst.T2M);

  return;
}


//
// Class name  :
// Method name : DecodeTargetID(polDataStruct poldat)
//
// Description : Decorde target infomation.
//             : presently the target ID is assumed to be appear at the last of
//             : character string poldat.targetIdS.
// Input       : polDataStruct poldat
// Return      :
//             :
//
void
DecodeTargetID(polDataStruct poldat){

  cout << endl << "target ID = " << poldat.targetIdS << endl;

  // initiarization
  runinfo.targetID='-';

  string str(poldat.targetIdS);
  if (str.find("Background") < str.size())   runinfo.targetID='B';
  if (str.find("1")         == str.size()-1) runinfo.targetID='1';
  if (str.find("2")         == str.size()-1) runinfo.targetID='2';
  if (str.find("3")         == str.size()-1) runinfo.targetID='3';
  if (str.find("4")         == str.size()-1) runinfo.targetID='4';
  if (str.find("5")         == str.size()-1) runinfo.targetID='5';
  if (str.find("6")         == str.size()-1) runinfo.targetID='6';
  if (str.find("7")         == str.size()-1) runinfo.targetID='7';
  if (str.find("8")         == str.size()-1) runinfo.targetID='-';

  // Restore Vertical or Horizontal target information
  // in case this information isn't decorded correctly
  // within REC_PCTARGET routine. If the target is horizontal,
  // then mask 90 degree detector.
  if (runinfo.target=='-') {
    if ((str.find("Vert") == 0)||(str.find("V") == 0)) {runinfo.target='V'; tgt.VHtarget=0;}
    if ((str.find("Horz") == 0)||(str.find("H") == 0)) {
      runinfo.target='H';
      tgt.VHtarget=1;
      // This is too late to reconfigure strip mask because this routine is
      // executed at the end of event loop. Too bad. /* March 5,'09 IN */
      //      mask.detector = 0x2D;
      //      ConfigureActiveStrip(mask.detector);
    }
  }

  return;

}




//
// Class name  :
// Method name : PrepareCollidingBunchPattern(runinfo.RHICBeam)
//
// Description : Configure phx.bunchpat[] and str.bunchpat[] arrays only for colliding bunches
//             :
// Input       :
// Return      :
//             :
//
void
PrepareCollidingBunchPattern(){


  for (int i=0; i<NBUNCH; i++){
    phx.bunchpat[i] = 1; // PHENIX bunch patterns
    str.bunchpat[i] = 1; // STAR bunch patterns
  }

  if (runinfo.RHICBeam==1) { // Yellow Beam
    for (int j=31; j<40; j++) str.bunchpat[j] = phx.bunchpat[j+40] = 0;
  }else if (runinfo.RHICBeam==0){ // Blue Beam
    for (int j=31; j<40; j++) phx.bunchpat[j] = str.bunchpat[j+40] = 0;
  }

  cout << "===== Colliding Bunch pattern =======" << endl;
  cout << " IP2,IP8 : " ; for (int i=0; i<NBUNCH; i++) cout << phx.bunchpat[i] ; cout << endl;
  cout << " IP6,IP10: " ; for (int i=0; i<NBUNCH; i++) cout << str.bunchpat[i] ; cout << endl;
  cout << "=====================================" << endl;

  return;

}
