void rootlogon()
{
   char *cnipol_path = "../../";
   gSystem->AddIncludePath(Form(" -I%sinc/ -I%scontrib/polarim-online/include/ -I%scontrib/root-helper/ ", cnipol_path, cnipol_path, cnipol_path));
   gROOT->ProcessLine(Form(".L %sbuild/src/libcnipol.so", cnipol_path));
}
