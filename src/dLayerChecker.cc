#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>



#include "dLayerChecker.h"


int main (int argc, char *argv[])
{
	
	bool optgiven=false;
	fitresult=0;
	runfit=false;
	
	
	// for get option
	int c;
	extern char *optarg;
	
	
	
	datadir = getenv("CONFDIR");
	asymdir = getenv("ASYMDIR");
	if ( datadir == NULL )
	{
		cerr << "environment CONFDIR is not defined" << endl;
		exit(-1);
	}
	
	
	
	while ((c = getopt(argc, argv, "?fz:h"))!=-1)
	{
		switch (c)
		{
			case 'h':
			case '?':
				cout<<"This program compares the dead layers from two data files"<<endl;
				cout<<"Usage of " << argv[0] <<endl;
				cout << " -f <run #>        : check dead layer consitency " <<endl;
				cout<<"or"<<endl;
				cout << " -z <run #>        : check dead layer fit status" <<endl;
				exit(-1);
			case 'f':
				sprintf(runid, optarg);
	    			// if ifile lack of suffix ".data", attach ".data"
				strcat(configfile, datadir);
				strcat(configfile,     "/");
				strcat(configfile,   runid);
				strcat(configfile,   ".config.dat");
				fprintf(stdout,"Input config file : %s\n",configfile);
				optgiven=true;
				break;
			case 'z':
				sprintf(runid, optarg);
				strcat(configfile, asymdir);
				strcat(configfile,     "/dlayer/");
				strcat(configfile,   runid);
				strcat(configfile,   ".temp.dat");
				fprintf(stdout,"Input dlayer file : %s\n",configfile);
				runfit=true;
				checkChi2(configfile);
				break;
			default:
				fprintf(stdout,"Invalid Option \n");
				cout<<"run with option -h for help"<<endl;
				exit(-1);
		}
	}
	
	
	if(runfit==true)
	{
		if(fitresult==1)
		{
			cout<<"dlayer fit is successful"<<endl;
		}
		else
		{
			cout<<"dlayer fit is NOT successful"<<endl;
		}
		return fitresult;
	}
	
	if(optgiven==false)
	{
		cout<<"This program compares the dead layers from two data files"<<endl;
		cout<<"Usage of " << argv[0] <<endl;
		cout << " -f <run #>        : input run # " <<endl;
		return -1;
	}
	
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
	
	if(readDLayer(configfile2)!=1)
	{
		return -1;
	}
	for(unsigned short ii=0;ii<num_detectors;ii++)
	{
		deadwidth[1][ii]=tempwidth[ii];
	}
	
	
	
	
	cout<<endl<<endl;
	cout<<"dead layers from this run"<<endl;
	for(unsigned short ii=0;ii<num_detectors;ii++)
	{
		cout<<deadwidth[0][ii]<<endl;
	}
	
	cout<<endl<<endl;
	
	cout<<"dead layers from previous file"<<endl;
	for(unsigned short ii=0;ii<num_detectors;ii++)
	{
		cout<<deadwidth[1][ii]<<endl;
	}
	
	cout<<endl<<endl;
	
	
	
	
	dead_layers_consistent=true;

	
	for(unsigned short ii=0;ii<num_detectors;ii++)
	{
		if(fabs(deadwidth[0][ii]-deadwidth[1][ii])>3.)
		{
			dead_layers_consistent=false;
			break;
		}
	}
		
	if(dead_layers_consistent==true)
	{
		cout<<"finished: dead layers are consistent"<<endl;
		return 1;
	}
	else
	{
		cout<<"finished: dead layers are NOT consistent"<<endl;
		return 0;
	}
	
}




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
			rundb.RunID = strtod(s.c_str(),NULL);
      //printf("%8.3f\n",rundb.RunID);
			match = MatchBeam(RUNID,rundb.RunID);
			if (match){
				if(thisrun==false)
				{
					if (RUNID==rundb.RunID) break;
				}
				else
				{
					if (RUNID<rundb.RunID) break;
				}
			}
		}else{
			if (match){
				if (str.find("CONFIG")              ==1) {rundb.config_file_s         = GetVariables(str);}
				if (str.find("MASSCUT")             ==1) rundb.masscut_s             = GetVariables(str);
				if (str.find("TSHIFT")              ==1) rundb.tshift_s              = GetVariables(str);
				if (str.find("ENERGY_CALIB")        ==1) rundb.calib_file_s          = GetVariables(str);
				if (str.find("INJ_TSHIFT")          ==1) rundb.inj_tshift_s          = GetVariables(str);
				if (str.find("RUN_STATUS")          ==1) rundb.run_status_s          = GetVariables(str);
				if (str.find("MEASUREMENT_TYPE")    ==1) rundb.measurement_type_s    = GetVariables(str);
				if (str.find("DEFINE_SPIN_PATTERN") ==1) rundb.define_spin_pattern_s = GetVariables(str);
				if (str.find("COMMENT")             ==1) rundb.comment_s             = GetVariables(str);
				if (str.find("DisableStrip")        ==1){
					rundb.disable_strip_s     = GetVariables(str);
					StripHandler(atoi(rundb.disable_strip_s.c_str()), 1);}
					if (str.find("EnableStrip")         ==1) {
						rundb.enable_strip_s      = GetVariables(str);
						StripHandler(atoi(rundb.enable_strip_s.c_str()),-1);}
			}
		}

	} // end-of-while(getline-loop)
	
	runinfo.NDisableStrip = FindDisableStrip();
	
	if(thisrun==false)
	{
		sprintf(configfile2, "%s/%s", datadir, rundb.config_file_s.c_str());
	}
	
}




int readDLayer(char *infile)
{
	
	double deadwidth[strips_per_detector*num_detectors + 10];
	
	int countstrip=0;
	int countdetector=0;
	
	int st,strip;
	float t0,acoef,edead,ecoef,A0,A1,iasigma;

	fprintf(stdout,"**********************************\n");
	fprintf(stdout,"** Configuration is overwritten **\n");
	fprintf(stdout,"**********************************\n");

	ifstream configFile;

	configFile.open(infile);
	
	if (!configFile) {
		cerr << "failed to open Config File : " << infile << endl;
		return -1;
	}

	cout << "Reading configuration info from : " << infile <<endl;

    
	char temp[13][20];
	char *tempchar, *stripchar, *T0char;
    
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
			
			
			

// 			cfginfo.data.chan[stripn-1].edead = edeadn;
// 			cfginfo.data.chan[stripn-1].ecoef = ecn;
// 			cfginfo.data.chan[stripn-1].t0 = t0n;
// 			cfginfo.data.chan[stripn-1].A0 = a0n;
// 			cfginfo.data.chan[stripn-1].A1 = a1n;
// 			cfginfo.data.chan[stripn-1].acoef = ealphn;
// 			cfginfo.data.chan[stripn-1].dwidth = dwidthn;
// 			cfginfo.data.chan[stripn-1].pede = peden;
// 			cfginfo.data.chan[stripn-1].C[0] = c0n;
// 			cfginfo.data.chan[stripn-1].C[1] = c1n;
// 			cfginfo.data.chan[stripn-1].C[2] = c2n;
// 			cfginfo.data.chan[stripn-1].C[3] = c3n;
// 			cfginfo.data.chan[stripn-1].C[4] = c4n;
// 			
// 			cout<<"wrote to cfginfo"<<endl;

// 			cout << " Strip " << stripn;
// 			cout << " Ecoef " << ecn;
// 			cout << " T0 " << t0n;
// 			cout << " A0 " << a0n;
// 			cout << " A1 " << a1n;
// 			cout << " Acoef " << ealphn;
// 			cout << " Dwidth " << dwidthn;
// 			cout << " Pedestal " << peden << endl;
		}
        
		linen ++;
	}


	configFile.close();
	
	
	for(unsigned short detector=0;detector<num_detectors;detector++)
	{
		countstrip=0;
		for(unsigned short strip=0;strip<strips_per_detector;strip++)
		{
			if(isStripAlive(strips_per_detector*detector + strip)==true)
			{
				countstrip++;
// 				tempwidth[detector]+=cfginfo.data.chan[strips_per_detector*detector + strip].dwidth;
				tempwidth[detector]+=deadwidth[strips_per_detector*detector + strip];
			}
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
	for(int jj=0;jj<runinfo.NDisableStrip;jj++)
	{
		if((int)strp==runinfo.DisableStrip[jj])
		{
			return false;
		}
	}
	
	return true;
}


// int MatchBeam(double ThisRunID, double RunID)
// {
// 
// 	int match=0;
// 
// 	int ThisRun = int((ThisRunID-int(ThisRunID))*1e3);
// 	int Run     = int((RunID-int(RunID))*1e3);
// 
// 	if ((ThisRun>=100)&&(Run>=100)) match=1;
// 	if ((ThisRun<= 99)&&(Run<=99))  match=1;
// 
// 	return match;
// }


// string GetVariables(string str)
// {
// 
// 	string::size_type begin = str.find("=")+ 1;
// 	string::size_type end = str.find(";");
// 	string::size_type length = end - begin ;
// 
// 	string s = str.substr(begin,length);
// 	return s;
// 
// }


// int StripHandler(int st, int flag)
// {
// 
// 	static int Initiarize = 1;
// 	if (Initiarize) for (int i=0; i<NSTRIP; i++) ProcessStrip[i]=0;
// 
// 	ProcessStrip[st-1] += flag;
// 
// 	Initiarize=0;
// 
// 	return 0;
// }


// int FindDisableStrip()
// {
// 
// 	int NDisableStrip=0;
// 	for (int i=0;i<NSTRIP; i++) {
// 		if (ProcessStrip[i]>0) {
// 			runinfo.DisableStrip[NDisableStrip] = i;
// 			NDisableStrip++;
// 		}
// 	}
// 
// 	return NDisableStrip;
// 
// }


void checkChi2(char *infile)
{
	ifstream configFile;
	fitresult=1;

	configFile.open(infile);
	
	if (!configFile) {
		cerr << "failed to open Config File : " << infile << endl;
		exit(-1);
	}
	
	char buffer[300];
	float chi2;
	char *fitstatus;
	
	int stripcount =1;
	
	while (stripcount<NSTRIP) {
        
		configFile.getline(buffer, sizeof(buffer), '\n'); 
		
		strtok(buffer," ");
		for(short ii=0;ii<7;ii++)
		{
			strtok(NULL," ");
		}
		chi2=atof(strtok(NULL," "));
		strtok(NULL," ");
		fitstatus=strtok(NULL," ");
		
		if(chi2>chi2max)
		{
			cout<<"chi2 is too large for strip "<<stripcount<<endl;
			fitresult=0;
			break;
		}
		
		string ss=fitstatus;
		
		if(ss!="SUCCESSFUL")
		{
			cout<<fitstatus<<endl;
			cout<<"fit status not successful for strip "<<stripcount<<endl;
			fitresult=0;
			break;
		}
		
		stripcount++;
	}
	
	
}
