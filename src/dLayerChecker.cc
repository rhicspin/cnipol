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
	
	for(unsigned short strip=0;strip<num_strips;strip++)
	{
		is_strip_alive[strip]=true;
		for(unsigned short nn=0;nn<num_dead_strips;nn++)
		{
			if(dead_strips[nn]==strip)
			{
				is_strip_alive[strip]=false;
				break;
			}
		}
	}
	
	
	
	
	// for get option
	int c;
	extern char *optarg;
	
	
	
	char * datadir = getenv("DATADIR");
	if ( datadir == NULL )
	{
		cerr << "environment DATADIR is not defined" << endl;
		cerr << "e.g. export DATADIR=$HOME/2005/data" << endl;
		exit(-1);
	}

	char ifile[32];
	
	while ((c = getopt(argc, argv, "?f:g:h"))!=-1)
	{
		switch (c)
		{
			case 'h':
			case '?':
				cout<<"This program compares the dead layers from two data files"<<endl;
				cout<<"Usage of " << argv[0] <<endl;
				cout << " -f <filename>        : first input data file name " <<endl;
				cout << " -g <filename>        : second input data file name " <<endl;
				exit(-1);
			case 'f':
				sprintf(ifile, optarg);
	    			// if ifile lack of suffix ".data", attach ".data"
				if (strstr(ifile,".data")==NULL) strcat(ifile,".data"); 
				strcat(datafile, datadir);
				strcat(datafile,     "/");
				strcat(datafile,   ifile);
				fprintf(stdout,"Input data file : %s\n",datafile);
				break;
			case 'g':
				sprintf(ifile, optarg);
	    			// if ifile lack of suffix ".data", attach ".data"
				if (strstr(ifile,".data")==NULL) strcat(ifile,".data"); 
				strcat(datafile2, datadir);
				strcat(datafile2,     "/");
				strcat(datafile2,   ifile);
				fprintf(stdout,"Input data file 2 : %s\n",datafile2);
				break;
			default:
				fprintf(stdout,"Invalid Option \n");
				cout<<"run with option -h for help"<<endl;
				exit(-1);
		}
	}
	
	for(unsigned short ii=0;ii<num_detectors;ii++)
	{
		tempwidth[ii]=0;
	}
	
	if(readDLayer(datafile)!=1)
	{
		return -1;
	}
	for(unsigned short ii=0;ii<num_detectors;ii++)
	{
		deadwidth[0][ii]=tempwidth[ii];
	}
	
	
	
	for(unsigned short ii=0;ii<num_detectors;ii++)
	{
		tempwidth[ii]=0;
	}
	
	if(readDLayer(datafile2)!=1)
	{
		return -1;
	}
	for(unsigned short ii=0;ii<num_detectors;ii++)
	{
		deadwidth[1][ii]=tempwidth[ii];
	}
	
	
	
	
	cout<<endl<<endl;
	cout<<"dead layers from first file"<<endl;
	for(unsigned short ii=0;ii<num_detectors;ii++)
	{
		cout<<deadwidth[0][ii]<<endl;
	}
	
	cout<<endl<<endl;
	
	cout<<"dead layers from second file"<<endl;
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

int readDLayer(char *infile)
{
	int flag=0;     // exit from while when flag==1
	int rval;
	
	int countstrip=0;
	int countdetector=0;
	
	 // Common structure for the data format
	static union 
	{
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
	
	FILE *fp;
	
	// reading the data till its end ...
	if ((fp = fopen(infile,"r")) == NULL)
	{
		printf("ERROR: %s file not found. Fource exit.\n",infile);;
		exit(-1);
	} 
	
	while(flag==0)
	{
		if (fread(&rec.header, sizeof(recordHeaderStruct), 1, fp)!=1)
		{
			break;
		}
		
		if (feof(fp))
		{
			fprintf(stdout,"Expected end of file\n");
			break;
		}
		
        
		if (rec.header.len > BSIZE*sizeof(int))
		{
			fprintf(stdout,"Not enough buffer d: %d byte b: %d byte\n",
				rec.header.len, sizeof(rec));
			break;
		}
		 
		// Read rest of the structure
		rval = fread(&rec.begin.version, rec.header.len - 
				sizeof(recordHeaderStruct),1,fp);
		
		
		if (feof(fp)) 
		{
			perror("Unexpected end of file");
			exit(-1);
			break;
		}
		
		switch (rec.header.type & REC_TYPEMASK) 
		{
			case REC_RHIC_CONF:
				if (/*!ReadFlag.RHICCONF*/1)
				{
					fprintf(stdout,"Read configure information\n");
					cfginfo = (recordConfigRhicStruct *)malloc(sizeof(recordConfigRhicStruct)+(rec.cfg.data.NumChannels-1)*sizeof(SiChanStruct));
					
					memcpy(cfginfo, &rec.cfg, sizeof(recordConfigRhicStruct)+(rec.cfg.data.NumChannels-1)*sizeof(SiChanStruct));
					
					//channels <--> strips
					if(cfginfo->data.NumChannels!=num_strips)
					{
						cout<<"wrong number of channels in cfginfo"<<endl;
						exit(-1);
					}
					
					
					for(unsigned short detector=0;detector<num_detectors;detector++)
					{
						countstrip=0;
						for(unsigned short strip=0;strip<strips_per_detector;strip++)
						{
							if(is_strip_alive[strips_per_detector*detector + strip]==true)
							{
								countstrip++;
								tempwidth[detector]+=cfginfo->data.chan[strips_per_detector*detector + strip].dwidth;
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
					
				}
				break;
		}
		
	}
	
// 	cout<<rec.header.len<<endl;
// 	cout<<rec.header.type<<endl;
// 	cout<<rec.cfg.data.NumChannels<<endl;
	
	return 1;
}
