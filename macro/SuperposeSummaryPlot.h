// Description : Header file for summary plotting macros
//               f.i. OfflinePol.C, DlayerMonitor.C..
// Date Created: June 28, 2006
// Author      : I. Nakagawa
// 

#ifndef SUPERPOSE_SUMMARY_PLOT_H
#define SUPERPOSE_SUMMARY_PLOT_H

//
// Class name  : 
// Method name : SuperposeComments(Int_t Mode)
//
// Description : Print optional comments on the current plot
// Input       : 
// Return      : 
//
Int_t
SuperposeComments(Int_t Mode, TH2D *frame){


  // may need to restore following if statements in the future.
  //  if ((Mode==10)||(Mode==50)||(Mode==60)||(Mode==90)){
    ArrayAndText(frame, 7563.,7957.,41, "200GeV");
    ArrayAndText(frame, 7991.,8061.,32, "62GeV");
    ArrayAndText(frame, 8092.,8124.,25, "500GeV");
    //  }


  return 0;

}

//
// Class name  : 
// Method name : ArrayAndText(TH2D frame, Float_t xmin, Float_t xmax, Int_t Color, Char_t *txt)
//
// Description : Draw Array and text on the current frame
// Input       : Float_t xmin, Float_t xmax, Int_t Color, Char_t *txt
// Return      : 
//
Int_t
ArrayAndText(TH2D*frame, Float_t xmin, Float_t xmax, Int_t Color, Char_t *txt){

  Float_t scale=0.1;
  Float_t ymin=frame->GetYaxis()->GetXmin() ;
  Float_t ymax=frame ->GetYaxis()->GetXmax() ;
  Float_t yint=ymax-ymin;
  TArrow * ar = new TArrow(xmin,ymax-yint*scale,xmax,ymax-yint*scale,0.02F,"|>"); 
  ar->SetLineWidth(6);
  ar->SetLineColor(Color);
  ar->SetFillColor(Color);
  ar->Draw("same");

  Float_t xtxt = xmin+(xmax-xmin)/2.;
  TText *tx = new TText(xtxt,ymax-yint*scale,txt);
  tx->SetTextAlign(21);
  tx->SetTextColor(13);
  tx->Draw("same");

  return 0;

}			


#endif
