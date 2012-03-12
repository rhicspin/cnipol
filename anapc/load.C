/**
 *
 */

{
   gROOT->Reset();
   gROOT->Clear();

   gSystem->AddIncludePath("-I$HOME/root_macros");
   //gSystem->AddIncludePath("-I/home/dsmirnov/root_macros/utils");
   //gSystem->AddIncludePath("-I./include/SRT_D0");
   gSystem->AddIncludePath("-I$CNIPOL_DIR/online/include");
   gSystem->AddIncludePath("-I$CNIPOL_DIR/inc");
   gSystem->AddIncludePath("-I$CNIPOL_DIR/src");
   gSystem->AddIncludePath("-I$CNIPOL_DIR/macro");

   //gSystem->AddIncludePath("-I$MYSQL_INCLUDE_DIR");
   //gSystem->AddIncludePath("-I$MYSQLPP_INCLUDE_DIR");

   //gSystem->AddLinkedLibs("$SRT_PUBLIC_CONTEXT/shlib/Linux2.4-GCC_3_4/libd0_util.so");

   TString macroPath = "";
	macroPath += "./:";
	macroPath += "~/rootmacros:";
   macroPath += "$CNIPOL_DIR:";
   macroPath += "$CNIPOL_DIR/anapc:";

   //macroPath += "$MYSQL_LIB_DIR:";
   //macroPath += "$MYSQLPP_LIB_DIR:";

   gROOT->SetMacroPath(macroPath);

   gSystem->Load("libutils.so");
   gSystem->Load("libcnipol.so");
   gSystem->Load("libcniana.so");
   //gSystem->Load("libcnipol.so");

   //gSystem->Load("libmysqlpp.so");
   //gSystem->Load("libmysqlclient.so");

   //gROOT->LoadMacro("fit_functions.C");

   // Choose one of the following macros:
   //gROOT->LoadMacro("analyze.C+");
   //gROOT->LoadMacro("test.C+");
   //gROOT->LoadMacro("utils/utils.C+");
   //gROOT->LoadMacro("macro/KinFit.C+");
   //gROOT->LoadMacro("analyzeCalib.C+");
   //gROOT->LoadMacro("analyzeDeadLayer.C+");
   //gROOT->LoadMacro("createRunInfo.C+");
   //gROOT->LoadMacro("anaPulseResponse.C+");
   //gROOT->LoadMacro("manalyze.C+");
   //gROOT->LoadMacro("savePlots.C+");
}
