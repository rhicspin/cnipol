#ifndef ASYM_RUNDB_H
#define ASYM_RUNDB_H


//const char * GetVariables(string str);
string GetVariables(string str);
//const char* GetVariables(string str);
float GetVariablesFloat(string str);
int ContinueScan(double ThisRunID, double RunID);
int MatchBeam(double ThisRunID, double RunID);
void PrintRunDB();
int StripHandler(int, int);
StructRunDB rundb;
static int ProcessStrip[NSTRIP];
int FindDisableStrip();


#endif

