
//
// Class name  : 
// Method name : RunStatusFilter(Int_t Mask, Char *RunStatus){
//
// Description : Filter out RunStatus
//	         Bit 7 - Good 
//               Bit 6 - Rcvd;
//	         Bit 5 - Susp;
//               Bit 4 - Tune;     
//               Bit 3 - BadP;     
// 	         Bit 2 - Bad;      
//               Bit 1 - Junk;
// 	         Bit 0 - N/A-;     
// Input       : (Int_t Mask, Char *RunStatus)
// Return      : 1 if true, 0 if false
//
Int_t
RunStatusFilter(Int_t Mask, Char_t *RunStatus){

  Int_t test=1;
  if (Mask>>0&1) test *= strcmp(RunStatus,"N/A");
  if (Mask>>1&1) test *= strcmp(RunStatus,"Junk");
  if (Mask>>2&1) test *= strcmp(RunStatus,"Bad");
  if (Mask>>3&1) test *= strcmp(RunStatus,"BadP");
  if (Mask>>4&1) test *= strcmp(RunStatus,"Tune");
  if (Mask>>5&1) test *= strcmp(RunStatus,"Susp");
  if (Mask>>6&1) test *= strcmp(RunStatus,"Rcvd");
  if (Mask>>7&1) test *= strcmp(RunStatus,"Good");

  return test;

}



//
// Class name  : 
// Method name : MeasTypeFilter(Int_t Mask, Char *MeasType){
//
// Description : Filter out RunStatus
//	         Bit 7 - NORM;
//               Bit 6 -     ;
//	         Bit 5 -     ;
//               Bit 4 -     ;     
//               Bit 3 -     ;  
// 	         Bit 2 - PHYS;      
//               Bit 1 - TUNE;
// 	         Bit 0 - PROF;     
// Input       : (Int_t Mask, Char *MeasType)
// Return      : 1 if true, 0 if false
//
Int_t
MeasTypeFilter(Int_t Mask, Char_t *MeasType){

  Int_t test=1;
  if (Mask>>0&1) test *= strcmp(MeasType,"PROF");
  if (Mask>>1&1) test *= strcmp(MeasType,"TUNE");
  if (Mask>>2&1) test *= strcmp(MeasType,"PHYS");
  if (Mask>>7&1) test *= strcmp(MeasType,"NORM");

  return test;

}



//
// Class name  : 
// Method name : GetScale(Float_t *x, Int_t N, Float_t margin, Float& min, Float& max);
//
// Description : Calculate min and max from array x(N)
// Input       : 
// Return      : Float_t min, Float_t max
//
void
GetScale(Float_t *x, Int_t N, Float_t margin, Float_t & min, Float_t & max){

  min = max = x[0];
  for (Int_t i=0; i<N; i++) {
    if (x[i]) {
      if (x[i]<min) min=x[i];
      if (x[i]>max) max=x[i];
    }
  }

  Float_t interval = max - min;
  min -= interval*margin;
  max += interval*margin;

  return ;

}

//
// Class name  : 
// Method name : GetScale(Float_t *x, Int_t N, Float_t margin, Float& min, Float& max);
//
// Description : Calculate min and max from array x(N)
// Input       : 
// Return      : Float_t min, Float_t max
//
Float_t 
GetScalePrefixRange(Float_t Range, Float_t *x, Int_t N, Float_t margin, Float_t & min, Float_t & max){
 
  GetScale(x, N, 0, min, max);
  Float_t mean = (max + min)/2 ;
  
  min = min < mean - Range/2 ? min : mean - Range/2 ;
  max = max > mean + Range/2 ? max : mean + Range/2 ;
  
  return fabs(max - min);

}
