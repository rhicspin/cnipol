
#include "dLayerChecker.h"

#include <string.h>

#include "AsymDbFile.h"
#include "RunInfo.h"

using namespace std;


void Usage(char* argv[])
{
  cout << endl;
  cout << " This program compares the dead layers from two data files."<<endl;
  cout << " Default output for -z test is " << ofile << endl;
  cout << " Usage of " << argv[0] <<endl;
  cout << " -f <RunID>        : check dead layer consitency between <RunID> and whatever config file";
  cout <<                     " defined in run.db" <<endl;
  cout << " -z <RunID>        : check the quality of dead layer fit for <RunID>" <<endl ;
  cout << " -s                : CONFIG*=<RunID> instead of CONFIG=<RunID> def:off" << endl << endl;
  exit(-1);

  return;
}


int main (int argc, char *argv[])
{
   bool SINGLE_CONFIG_MODE=false;
        bool optgiven=false;
        fitresult=0;
        runfit=false;

        // for get option
        int c;
        extern char *optarg;

        char OperationMode[100];
        int Mode=0;

        confdir = getenv("CONFDIR");
        sharedir = getenv("SHAREDIR");
        tmpdir = getenv("TMPOUTDIR");

        // output file operation
        sprintf(ofile,"%s/rundb.log",tmpdir);
        ofstream out;
        out.open(ofile);
        if (out.fail()) {
          cerr << "\n Cannot create output file " << ofile << "." << endl;
          cerr << " Likely either no " << tmpdir << " directory or write permission problem." << endl;
        }

        if ( confdir == NULL )
        {
                cerr << "environment CONFDIR is not defined" << endl;
                exit(-1);
        }

        while ((c = getopt(argc, argv, "?sf:z:h"))!=-1)
        {
                switch (c)
                {
                        case 'h':
                        case '?':
                          Usage(argv);
                        case 'f':
                                sprintf(runid, optarg);
                                // if ifile lack of suffix ".data", attach ".data"
                                optgiven=true;
                                sprintf(OperationMode,"(Consistency Check Mode)          ");
                                break;
                        case 'z':
                                sprintf(runid, optarg);
                                strcat(dlayerfile, sharedir);
                                strcat(dlayerfile,     "/dlayer/");
                                strcat(dlayerfile,   runid);
                                strcat(dlayerfile,   ".temp.dat");
                                runfit=true;
                                sprintf(OperationMode,"(DeadLayer Fit Quality Check Mode)");
                                Mode=1;
                                getPreviousRun();
                                break;
                        case 's':
                                SINGLE_CONFIG_MODE=true;
                                break;
                        default:
                                fprintf(stdout,"Invalid Option \n");
                                cout<<"run with option -h for help"<<endl;
                                exit(-1);
                }

                // define configfile
                strcat(configfile, confdir);
                strcat(configfile,     "/");
                strcat(configfile,   runid);
                strcat(configfile,   ".config.dat");

        }

        printf("\n");
        printf("******************************************************************************\n");
        printf("****         dLayerChecker Message %s     ****\n", OperationMode);
        printf("******************************************************************************\n");

        // OperationMode == Deadalyer Fit Quality Check
        if (Mode) {
          checkChi2(dlayerfile);
          readDLayer(configfile);
          // if t0 are all same for strips for 90 degrees detectors, then this is horizontal target run.
          if (!(t0_diff_sum[1]+t0_diff_sum[4])) SINGLE_CONFIG_MODE=true;
        }

        if(runfit==true)
        {
                if(fitresult==1)
                {
                   cout<<"dlayer fit is successful.";
                        out << endl << "[" << runid << "]@" << endl;
                        if (SINGLE_CONFIG_MODE) {
                          cout << " Write out in single config mode" << endl;
                          out << "\tCONFIG*=" ;
                        } else {
                          cout << endl;
                          out << "\tCONFIG="  ;
                        }
                        out << runid << ".config.dat;@" << endl << endl;
                }
                else
                {
                        cout<<"dlayer fit is NOT successful"<<endl;
                        out << endl << "[" << runid << "]@" << endl;
                        out << "\tRUN_STATUS*=Suspicious;@" << endl;
                        if (fitresult==0)
                          out << "\tCOMMENT*=\"Unsuccessful deadlayer fit. Don't update config file\";@";
                        if (fitresult==-1)
                          out << "\tCOMMENT*=\"Deadlayer fit failure. Don't update config file\";@";
                        out << endl << endl;
                }
                printf("******************************************************************************\n\n");
                return fitresult;
        }

        if(optgiven==false) Usage(argv);


        getPreviousRun(true);

        for(unsigned short ii=0;ii<num_detectors;ii++)
        {
                tempwidth[ii]=0;
        }

        if(readDLayer(configfile)!=1)
        {
                return -1;
        }
        for(unsigned short ii=0;ii<num_detectors;ii++)
        {
                deadwidth[0][ii]=tempwidth[ii];
        }

        getPreviousRun();

        for(unsigned short ii=0;ii<num_detectors;ii++)
        {
                tempwidth[ii]=0;
        }

        if(readDLayer(configfile2)!=1) { return -1; }

        for(unsigned short ii=0;ii<num_detectors;ii++)
        {
                deadwidth[1][ii]=tempwidth[ii];
        }

        // print out deadlayer thickness of THIS RunID
        cout<< configfile << ": (THIS)" << endl;
        for(unsigned short ii=0;ii<num_detectors;ii++) printf("%10.2f",deadwidth[0][ii]);
        cout<<endl;

        // print out deadlayer thickness of defined configfile in run.db
        cout<< configfile2 << ": (run.db)" << endl;
        for(unsigned short ii=0;ii<num_detectors;ii++) printf("%10.2f",deadwidth[1][ii]);
        cout<<endl;

        // Consistency test between deadlayers in two configulation files
        printf("Consistency limit =%5.1f [ug/cm2]\n", dlayer_consistency );
        dead_layers_consistent=true;
        for(unsigned short ii=0;ii<num_detectors;ii++)
        {
          printf("%10.2f",fabs(deadwidth[0][ii]-deadwidth[1][ii]));
                if(fabs(deadwidth[0][ii]-deadwidth[1][ii])>dlayer_consistency)
                {
                        dead_layers_consistent=false;
                }
        }
        cout << endl;

        if(dead_layers_consistent==true)
        {
                cout<<"finished: dead layers are consistent"<<endl;
                printf("******************************************************************************\n\n");
                return 1;
        }
        else
        {
                cout<<"finished: dead layers are NOT consistent"<<endl;
                printf("******************************************************************************\n\n");
                return 0;
        }

        out.close();
}


void getPreviousRun(bool thisrun){

        double RUNID=strtod(runid, NULL);

        // Read run.db
        readdb(RUNID);

        if(thisrun==false)
        {
                sprintf(configfile2, "%s/%s", confdir, gRunDb.config_file_s.c_str());
        }

        return;
}


int readDLayer(char *infile)
{
        double deadwidth[strips_per_detector*num_detectors + 10];
        double t0[strips_per_detector*num_detectors + 10];
        int countstrip=0;

        ifstream configFile;

        configFile.open(infile);

        if (!configFile) {
                cerr << "failed to open Config File : " << infile << endl;
                return -1;
        }

        char *tempchar;

        char buffer[300];
        int stripn;
        float t0n, ecn, edeadn, a0n, a1n, ealphn, dwidthn, peden;
        float c0n, c1n, c2n, c3n, c4n;

        int linen=0;
        while (!configFile.eof()) {

                configFile.getline(buffer, sizeof(buffer), '\n');
                if (strstr(buffer,"Channel")!=0) {

                        tempchar = strtok(buffer,"l");
                        stripn = atoi(strtok(NULL, "="));
                        t0n = atof(strtok(NULL," "));
                        ecn = atof(strtok(NULL," "));
                        edeadn = atof(strtok(NULL," "));
                        a0n = atof(strtok(NULL," "));
                        a1n = atof(strtok(NULL," "));
                        ealphn = atof(strtok(NULL," "));
                        dwidthn = atof(strtok(NULL," "));
                        peden = atof(strtok(NULL," "));
                        c0n = atof(strtok(NULL," "));
                        c1n = atof(strtok(NULL," "));
                        c2n = atof(strtok(NULL," "));
                        c3n = atof(strtok(NULL," "));
                        c4n = atof(strtok(NULL," "));

                        deadwidth[stripn-1]=dwidthn;
                        t0[stripn-1]=t0n;
                }

                linen ++;
        }

        configFile.close();

        for(unsigned short detector=0;detector<num_detectors;detector++)
        {
          // for detector active/non-active test
          float t0_ref=0;

                countstrip = 0;
      t0_diff_sum[detector] = 0;

                for(unsigned short strip=0;strip<strips_per_detector;strip++)
                  {
                        if(isStripAlive(strips_per_detector*detector + strip)==true)
                        {
                                countstrip++;
                                tempwidth[detector]+=deadwidth[strips_per_detector*detector + strip];
                        }

                        // reference t0 is always the first strip in detector. calculate diff.
                        if (!strip) t0_ref = t0[strips_per_detector*detector + strip];
                        t0_diff_sum[detector] += fabs(t0_ref - t0[strips_per_detector*detector + strip]);

                }
                if(countstrip!=0)
                {
                        tempwidth[detector]=(tempwidth[detector]/((float)countstrip));
                }
                else
                {
                        cout<<"warning: detector "<<detector<<" has no live strips"<<endl;
                }

        }

        return 1;
}


bool isStripAlive(unsigned short strp)
{
   for(int jj=0;jj<gRunInfo->NDisableStrip;jj++) {
      if((int)strp==gRunInfo->fDisabledChannels[jj]) {
         return false;
      }
   }

   return true;
}


void checkChi2(char *infile)
{
        ifstream DlayerFile;
        fitresult=1;

        DlayerFile.open(infile);

        if (!DlayerFile) {
                cerr << "failed to open DlayerFit File : " << infile << endl;
                fitresult=-1;
                return ;
        }

        char buffer[300];
        float chi2;
        char *fitstatus;

        int counter=0;
        while (counter<NSTRIP) {

                DlayerFile.getline(buffer, sizeof(buffer), '\n');
                if (strchr(buffer,' ')==NULL) break;

                // get Strip #, chi2 and fitting status from buffer
                int stripID=atoi(strtok(buffer," "));

                for(short ii=0;ii<7;ii++)   strtok(NULL," ");
                chi2=atof(strtok(NULL," "));
                strtok(NULL," ");
                fitstatus=strtok(NULL," ");

                  // ignore if strip is disabled in run.db
                  if(isStripAlive(stripID)==true) {

                    // test chi2
                    if(chi2>chi2max)
                      {
                        cout<< "chi2 = "<< chi2 << " exceeds limit " << chi2max << " in strip #" << stripID+1 <<endl;
                        fitresult=0;
                        break;
                      }

                    // test fitting status
                    string ss=fitstatus;
                    if(ss!="SUCCESSFUL")
                      {
                        cout<<fitstatus<<endl;
                        cout<<"fit status not successful for strip "<<stripID<<endl;
                        fitresult=0;
                        break;
                      }

                  }// end-if-isStripAlive()

                  counter++;
        }

        return;
}


/*
void getPreviousRun(bool thisrun)
{

        double RUNID=strtod(runid, NULL);
        // run DB file
        char *dbfile="run.db";
        FILE * in_file;
        if ((in_file = fopen(dbfile,"r")) == NULL) {
                printf("ERROR: %s file not found. Force exit.\n",dbfile);;
                exit(-1);
        }

        string s;
        char * line = NULL;
        size_t len = 0;
        ssize_t read;
        int match=0;
        while ((read = getline(&line, &len, in_file)) != -1){

                string str(line);
                if (str[0] == '[') { // Get Run Number
                        s = str.substr(1,8);
                        sprintf(runid2, "%s", s.c_str());
                        gRunDb.RunID = strtod(s.c_str(),NULL);
      //printf("%8.3f\n",gRunDb.RunID);
                        match = MatchBeam(RUNID,gRunDb.RunID);
                        if (match){
                                if(thisrun==false)
                                {
                                        if (RUNID==gRunDb.RunID) break;
                                }
                                else
                                {
                                        if (RUNID<gRunDb.RunID) break;
                                }
                        }
                }else{
                        if (match){
                                if(str.find("*=")==-1 || RUNID==gRunDb.RunID)
                                {
                                if (str.find("CONFIG")              ==1) {gRunDb.config_file_s         = GetVariables(str);}
                                if (str.find("MASSCUT")             ==1) gRunDb.masscut_s             = GetVariables(str);
                                if (str.find("TSHIFT")              ==1) gRunDb.tshift_s              = GetVariables(str);
                                if (str.find("ENERGY_CALIB")        ==1) gRunDb.calib_file_s          = GetVariables(str);
                                if (str.find("INJ_TSHIFT")          ==1) gRunDb.inj_tshift_s          = GetVariables(str);
                                if (str.find("RUN_STATUS")          ==1) gRunDb.run_status_s          = GetVariables(str);
                                if (str.find("MEASUREMENT_TYPE")    ==1) gRunDb.measurement_type_s    = GetVariables(str);
                                if (str.find("DEFINE_SPIN_PATTERN") ==1) gRunDb.define_spin_pattern_s = GetVariables(str);
                                if (str.find("COMMENT")             ==1) gRunDb.comment_s             = GetVariables(str);
                                if (str.find("DisableStrip")        ==1){
                                        gRunDb.disable_strip_s     = GetVariables(str);
                                        StripHandler(atoi(gRunDb.disable_strip_s.c_str()), 1);}
                                        if (str.find("EnableStrip")         ==1) {
                                                gRunDb.enable_strip_s      = GetVariables(str);
                                                StripHandler(atoi(gRunDb.enable_strip_s.c_str()),-1);}
                                }
                        }
                }

        } // end-of-while(getline-loop)

        gRunInfo->NDisableStrip = FindDisableStrip();

        if(thisrun==false)
        {
                sprintf(configfile2, "%s/%s", confdir, gRunDb.config_file_s.c_str());
        }
}
*/
