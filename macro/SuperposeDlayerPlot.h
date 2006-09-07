// Description : Header file for summary plotting macros
//               DlayerAnalyzer.C
// Date Created: Sept.06, 2006
// Author      : I. Nakagawa
// 

#ifndef SUPERPOSE_DLAYER_PLOT_H
#define SUPERPOSE_DLAYER_PLOT_H

StructConfigFileDB cfg;

//
// Class name  : DlayerAnalyzer
// Method name : GetConfigFileDB(Char_t * filename);
//
// Description : 
// Input       : 
// Return      : Int_t NDATA
//
Int_t
DlayerAnalyzer::GetConfigFileDB(Char_t * DATAFILE){

  ifstream fin;
  fin.open(DATAFILE,ios::in);
  if (fin.fail()){
    cerr << "ERROR: " << DATAFILE << " doesn't exist. Force exit." << endl;
    exit(-1);
  }


  // skip first three lines
  Char_t buff[300];
  for (int i=0;i<3; i++) fin.getline(buff,sizeof(buff),'\n');


  Int_t i=0;
  while (!fin.eof()){

    fin >> cfg.RunID[i] >> cfg.cRunID[i] >> cfg.Dl[i] >> cfg.DlE[i] >> cfg.ReadRate[i];
  

    // rate correction
    if (opt.RateCorrection) cfg.Dl[i] -= RateCorrection(cfg.ReadRate[i]);


    // dummy
    cfg.dx[i]=0;

    ++i;

    if (i>N-1){
      cerr << "WARNING : input data exceed the size of array " << N << endl;
      cerr << "          Ignore beyond line " << N << endl;
      break;
    } // if-(i>N)

  } // end-of-while(!fin.eof())


  fin.close();


  return i-1;

}

//
// Class name  : DlayerAnalyzer
// Method name : PlotConfigFile(Int_t Mode, Int_t Color, Char_t * text)
//
// Description : Superpose deadlayer thickness based on loaded config files in run.db
// Input       : Int_t Mode, Int_t Color, Char_t * text
// Return      : 
//
TGraphErrors *
DlayerAnalyzer::PlotConfigFile(Int_t Mode, Int_t Color, Char_t * text){

  Char_t Beam[]= Color==4 ? "Blue" : "Yellow";
  Char_t filename[100];
  sprintf(filename,"summary/ConfigDB_%s_FTP.dat",Beam);
  cout << filename << endl;

  // get data 
  Int_t ndata = GetConfigFileDB(filename);

  // superpose plots
  TGraphErrors* tg = new TGraphErrors(ndata, cfg.RunID, cfg.Dl, cfg.dx, cfg.dx);

  tg -> SetMarkerStyle(22);
  tg -> SetMarkerSize(1.2);
  tg -> SetMarkerColor(24);
  tg -> SetLineColor(24);
  //  tg -> Draw("P");

  return tg;
}








#endif
