
#include <getopt.h>

#include "MAsymAnaInfo.h"

#include "TString.h"
#include "TSystem.h"

#include "AsymGlobals.h"
#include "MseMeasInfo.h"
#include "MeasInfo.h"

ClassImp(MAsymAnaInfo)

using namespace std;


/** */
MAsymAnaInfo::MAsymAnaInfo() : AnaInfo(),
   fMListFileName("")
{
   Init();
}


/** */
MAsymAnaInfo::~MAsymAnaInfo()
{
}


void   MAsymAnaInfo::SetMListFileName(std::string listName) { fMListFileName = listName; }
string MAsymAnaInfo::GetMListFileName() { return fMListFileName; }
string MAsymAnaInfo::GetMListFullPath() { return GetResultsDir() + "/runXX/lists/" + fMListFileName; }
string MAsymAnaInfo::GetExternInfoPath() { return GetResultsDir() + "/runXX/"; }


/** */
void MAsymAnaInfo::ProcessOptions(int argc, char **argv)
{ //{{{
   int option_index = 0;

   static struct option long_options[] = {
      {"log",                 optional_argument,   0,   'l'},
      {"sfx",                 required_argument,   0,   AnaInfo::OPTION_SUFFIX},
      {"graph",               no_argument,         0,   AnaInfo::MODE_GRAPH},
      //{"no-graph",            no_argument,         0,   AnaInfo::MODE_NO_GRAPH},
      //{"mode-graph",          no_argument,         0,   AnaInfo::MODE_GRAPH},
      //{"mode-no-graph",       no_argument,         0,   AnaInfo::MODE_NO_GRAPH},

      {"copy",                no_argument,         0,   AnaInfo::FLAG_COPY},
      {"copy-results",        no_argument,         0,   AnaInfo::FLAG_COPY},
      {"update-db",           no_argument,         0,   AnaInfo::FLAG_UPDATE_DB},
      {"no-update-db",        no_argument,         0,   AnaInfo::FLAG_NO_UPDATE_DB},
      {"use-db",              no_argument,         0,   AnaInfo::FLAG_USE_DB},
      {"thumb",               no_argument,         0,   AnaInfo::FLAG_CREATE_THUMBS},

      {"meas-list",           required_argument,   0,   MAsymAnaInfo::OPTION_MLIST},
      {0, 0, 0, 0}
   };

   int c;

   while ((c = getopt_long(argc, argv, "?hl::gm:", long_options, &option_index)) != -1)
   {
      switch (c) {

      case '?':
      case 'h':
         PrintUsage();
         exit(0);

      case 'l':
         fFileStdLogName = (optarg != 0 ? optarg : "");
         break;

      case 'g':
      case AnaInfo::MODE_GRAPH:
         fModes |= AnaInfo::MODE_GRAPH;
         break;

      //case AsymAnaInfo::MODE_NO_GRAPH:
      //   fModes &= ~AsymAnaInfo::MODE_GRAPH;
      //   break;

      case AnaInfo::OPTION_SUFFIX:
         fSuffix = optarg;
         break;

      case AnaInfo::FLAG_USE_DB:
         fFlagUseDb    = kTRUE;
         fFlagUpdateDb = kTRUE;
         break;

      case AnaInfo::FLAG_CREATE_THUMBS:
         fFlagCreateThumbs = kTRUE;
         break;

      case AnaInfo::FLAG_COPY:
         fFlagCopyResults = kTRUE;
         break;

      case AnaInfo::FLAG_UPDATE_DB:
         fFlagUpdateDb = kTRUE;
         break;

      case 'm':
      case MAsymAnaInfo::OPTION_MLIST:
         SetMListFileName(optarg);
         break;

      default:
         Error("ProcessOptions", "Invalid option provided");
         PrintUsage();
         exit(0);
      }
   }
} //}}}


/** */
void MAsymAnaInfo::VerifyOptions()
{ //{{{
   // The run name must be specified
   if (fMListFileName.empty()) {
      Error("VerifyOptions", "File name has to be specified");
      PrintUsage();
      exit(0);
   }

   TString tmpFileName(fMListFileName.c_str());

   // file exist?
   if ( !gSystem->FindFile( (GetResultsDir() + "/runXX/lists/").c_str(), tmpFileName ) )
	{
      Error("VerifyOptions",
		   "File list \"%s\" not found\n", GetMListFullPath().c_str());
      exit(0);
	}

   fOutputName = fMListFileName;

   // The output dir is created here
   AnaInfo::VerifyOptions();
} //}}}


/** */
void MAsymAnaInfo::Print(const Option_t* opt) const
{ //{{{
   Info("Print", "Print members:");
   PrintAsPhp();
} //}}}


/** */
void MAsymAnaInfo::PrintAsPhp(FILE *f) const
{ //{{{
   AnaInfo::PrintAsPhp(f);
   fprintf(f, "$rc['fMListFileName']               = \"%s\";\n", fMListFileName.c_str());
   fprintf(f, "\n");
} //}}}


/** */
void MAsymAnaInfo::PrintUsage()
{ //{{{
   cout << endl;

   AnaInfo::PrintUsage();

   cout << "Options:" << endl;
   cout << " -h, -?                               : Print this help" << endl;
   cout << " -m, --meas-list <file_name>          : Name of run with raw data in $CNIPOL_RESULTS_DIR directory" << endl;
   cout << endl;
} //}}}
