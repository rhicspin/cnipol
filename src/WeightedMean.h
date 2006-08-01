#ifndef WeightedMean_H
#define WeightedMean_H

const int N=NSTRIP;


int Usage(char *argv[]);
int Example(char *argv[]);
int getData();
float WeightedMeanError(float A[N], float dA[N], float Ave, int NDAT);
float WeightedMean(float A[N], float dA[N], int NDAT);
void calcWeightedMean(float A[N], float dA[N], int NDAT, float &Ave, float &dAve);
void PrintResults(int Mode);



/*

//
// Class name  : 
// Method name : Mean
//
// Description : calculate Average
// Input       : double A[N], int NDAT
// Return      : Average
//
double
Mean(double A[N], int NDAT){
  double mean = 0;
  for (int i=0; i<N ; i++) mean += A[i];
  return mean/NDAT;
}
*/

#endif /* WM_H */
