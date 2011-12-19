
{
   gROOT->Reset();
   gROOT->Clear();

   gSystem->AddIncludePath("-I$HOME/rootmacros");
   gSystem->AddIncludePath("-I$CNIPOL_DIR/inc");

   //TString macroPath = "";
	//macroPath += "./:";
	//macroPath += "~/root_macros:";
   //macroPath += "$CNIPOL_DIR/anapc:";

   //gROOT->SetMacroPath(macroPath);

   gSystem->Load("libutils.so"); 
   gSystem->Load("libcnipol.so");
   gSystem->Load("libcniana.so");

   gROOT->LoadMacro("plotPolarFromFile.C+");
}
