//  Some general utility routines for root macros
//  file name :   Utility.h
//
//  Author    :   Itaru Nakagawa
//  Creation  :   01/26/2007
//


//
// Class name  : 
// Method name : 
// Description : 
// Input       : 
// Return      : 
//
Float_t RATIO_SQRT(Float_t x, Float_t y){
  return sqrt(x*x+y*y);
}

//
// Class name  : 
// Method name : QuadraticSumSQRT(Float_t x, Float_t y)
// Description : Calculate quadratic sum 
// Input       : 
// Return      : 
//
Float_t QuadraticSumSQRT(Float_t x, Float_t y){
  return sqrt(x*x+y*y);
}



//
// Class name  : 
// Method name : 
// Description : Calculate quadratic error propagation for x/y
// Input       : 
// Return      : 
//
Float_t QuadraticDivideError(Float_t x, Float_t y, Float_t dx, Float_t dy){

  if (x*y==0) {
    cerr << "QuadraticDivideError():ERROR! Divided by zero" << endl;
    exit(-1);
  }

  return x/y*sqrt(dx*dx/x/x+dy*dy/y/y);
}


//
// Class name  : 
// Method name : StandardDeviation(Int_t n; Float_t Y[], Float_t y[]);
// Description : Calcualte standard deviation
// Input       : (Int_t Mask, Char *RunStatus)
// Return      : 1 if true, 0 if false
//
Float_t StandardDeviation(Int_t n, Float_t Y[], Float_t y[]){

  Float_t sdev=0;
  for (Int_t i=0;i<n;i++) sdev += (Y[i] - y[i])*(Y[i] - y[i]);
  
  return sqrt(sdev/Float_t(n-1));
}

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
// 	         Bit 2 - Bad-;      
//               Bit 1 - Junk;
// 	         Bit 0 - N/A-;     
// 
//               31 : mask RunStatus == "N/A-","Junk","Bad","BadP","Tune" 
//               19 : mask RunStatus == "N/A-","Junk","Tune" 
//
// Input       : (Int_t Mask, Char *RunStatus)
// Return      : 1 if true, 0 if false
//
Int_t
RunStatusFilter(Int_t Mask, Char_t *RunStatus){

  Int_t test=1;
  if (Mask>>0&1) test *= strcmp(RunStatus,"N/A");
  if (Mask>>1&1) test *= strcmp(RunStatus,"Junk");
  if (Mask>>2&1) test *= strcmp(RunStatus,"Bad-");
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
// Method name : GetMax(Float_t *x, Int_t N)
//
// Description : Calculate maximum out of array x[] 
// Input       : 
// Return      : Float_t Max
//
Float_t 
GetMax(Float_t *x, Int_t N){
  Float_t Max=x[0];
  for (Int_t i=1; i<N; i++) Max = x[i]>Max ? x[i] : Max;
  return Max;
}

//
// Class name  : 
// Method name : GetMax(Float_t *x, Int_t N)
//
// Description : Calculate maximum out of array x[] 
// Input       : Float_t *x, Int_t N, Int_t &iMax)
// Return      : Float_t Max and index "iMax"
//
Float_t 
GetMax(Float_t *x, Int_t N, Int_t &iMax){
  Float_t Max=x[0];
  for (Int_t i=1; i<N; i++) {
    if (x[i]>Max){
      Max = x[i]; iMax = i;
    }else{
      Max = Max;
    }
  }
  return Max;
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
// Method name : GetScale(Int_t *x, Int_t N, Float_t margin, Float& min, Float& max);
//
// Description : Calculate min and max from array x(N)
// Input       : 
// Return      : Float_t min, Float_t max
//
void
GetScale(Int_t *x, Int_t N, Float_t margin, Float_t & min, Float_t & max){

  min = max = Float_t(x[0]);
  for (Int_t i=0; i<N; i++) {
    if (x[i]) {
      if (x[i]<min) min=Float_t(x[i]);
      if (x[i]>max) max=Float_t(x[i]);
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

// Return Minimum and Maximum from array A[N]. Same as GetMinMax() function. But
// GetMinMaxOption takes prefix value which forces min, max to be prefix when the
// absolute min,max are smaller than prefix.
void GetScalePrefix(float prefix, int N, float A[], float margin, float &min, float &max){
  GetScale(A, N, margin, min, max);
  if ( fabs(min)<prefix ) min = -prefix;
  if ( fabs(max)<prefix ) max =  prefix;
  return ;
}




//
// Class name  : 
// Method name : DrawLine()
//
// Description : DrawLines in TH1F histogram
//             : Assumes x=x0=x1, y0=0, y1=y1
// Input       : TH1F * h, float x, float y1, int color
// Return      : 
//
void 
DrawLine(TH1F * h, float x, float y1, int color, int lwidth){

  TLine * l = new TLine(x, 0, x, y1);
  l -> SetLineStyle(2);
  l -> SetLineColor(color);
  l -> SetLineWidth(lwidth);
  h -> GetListOfFunctions()->Add(l);

  return;
}


//
// Class name  : 
// Method name : DrawLine()
//
// Description : DrawLines in TH2F histogram
//             : Assumes  (x1,x2) y=y0=y1
// Input       : TH2F * h, float x0, float x1, float y, int color, int lstyle
// Return      : 
//
void 
DrawLine(TH2F * h, float x0, float x1, float y, int color, int lstyle, int lwidth){

  TLine * l = new TLine(x0, y, x1, y);
  l -> SetLineStyle(lstyle);
  l -> SetLineColor(color);
  l -> SetLineWidth(lwidth);
  h -> GetListOfFunctions()->Add(l);

  return;
}


//
// Class name  : 
// Method name : DrawText(TH2F * h, float x, float y, int color, char * text)
//
// Description : draw text on histogram. 
//             : 
// Input       : TH2F * h, float x, float y, int color, char * text
// Return      : 
//
void 
DrawText(TH2F * h, float x, float y, int color, char * text){

  TText * t = new TText(x, y, text);
  t->SetTextColor(color);
  t->SetTextSize(0.1);
  t->SetTextAlign(21);
  h->GetListOfFunctions()->Add(t);

  return;
}
