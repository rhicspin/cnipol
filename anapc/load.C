/**
 *
 */

{
   gROOT->Reset();
   gROOT->Clear();

   //gSystem->AddIncludePath("-I/home/dsmirnov/root_macros");
   //gSystem->AddIncludePath("-I/home/dsmirnov/root_macros/utils");
   //gSystem->AddIncludePath("-I./include/SRT_D0");
   gSystem->AddIncludePath("-I$CNIDIR/inc");
   gSystem->AddIncludePath("-I$CNIDIR/src");
   gSystem->AddIncludePath("-I$CNIDIR/macro");

   //gSystem->AddLinkedLibs("$SRT_PUBLIC_CONTEXT/shlib/Linux2.4-GCC_3_4/libd0_util.so");

   TString macroPath = "";
	macroPath += "./:";
	macroPath += "~/root_macros:";
   macroPath += "$CNIDIR:";
   macroPath += "$CNIDIR/anapc:";

   gROOT->SetMacroPath(macroPath);

   gSystem->Load("libcniana.so");
   gSystem->Load("libcnipol.so");

   //gROOT->LoadMacro("fit_functions.C");

   // Choose one of the following macros:
   //gROOT->LoadMacro("analyze.C+");
   //gROOT->LoadMacro("test.C+");
   gROOT->LoadMacro("macro/KinFit.C+");
   gROOT->LoadMacro("analyzeCalib.C+");
   //gROOT->LoadMacro("analyzeDeadLayer.C+");
   gROOT->LoadMacro("createRunInfo.C+");
}
