// Description : Header file for summary plotting macros
//               DlayerAnalyzer.C
// Date Created: Sept.06, 2006
// Author      : I. Nakagawa
// 

#ifndef SUPERPOSE_DLAYER_PLOT_H
#define SUPERPOSE_DLAYER_PLOT_H

StructConfigFileDB cfg;
StructOnlineDlayer online;

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


//
// Class name  : DlayerAnalyzer
// Method name : GetOnlineSummaryDat(Char_t * DATAFILE){
//
// Description : Get data from Online Summary File
// Input       : Char_t * DATAFILE
// Return      : Int_t NDATA
//
Int_t
DlayerAnalyzer::GetOnlineSummaryDat(Char_t * DATAFILE){

  ifstream fin;
  fin.open(DATAFILE,ios::in);
  if (fin.fail()){
    cerr << "ERROR: " << DATAFILE << " doesn't exist. Force exit." << endl;
    exit(-1);
  }


  // skip first three lines
  Char_t line[300],buff[300];
  for (int i=0;i<4; i++) fin.getline(buff,sizeof(buff),'\n');

  Int_t i=0;
  while (!fin.eof()){

    fin.getline(buff,sizeof(buff),'\n');
    if (strlen(buff) > 80) {
      online.RunID[i] = atof(strtok(buff," "));
      for (int j=0; j<3; j++)  strtok(NULL," ");
      online.bunch[i] = atof(strtok(NULL," "));
      online.BZdelay[i] = atof(strtok(NULL," "));
      online.threshold[i][0] = atof(strtok(NULL," "));
      online.threshold[i][1] = atof(strtok(NULL," "));
      online.Nevents[i] = atof(strtok(NULL," "));
      online.A_N[i] = atof(strtok(NULL," "));
      online.Dl[i] = atof(strtok(NULL," "));
      online.config[i] = strtok(NULL," ");
      ++i;
    }

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
// Method name : PlotOnlineDlayerAverage(Int_t Mode, Int_t Color, Char_t * text)
//
// Description : Superpose deadlayer thickness from Online Configulation file
// Input       : Int_t Mode, Int_t Color, Char_t * text
// Return      : TGraph * online_dlayer_plot
//
TGraph *
DlayerAnalyzer::PlotOnlineDlayerAverage(Int_t Mode, Int_t Color, Char_t * text){

  Char_t Beam[]= Color==4 ? "Blue" : "Yellow";
  Char_t filename[100];
  sprintf(filename,"summary/Online_%s.dat",Beam);
  cout << filename << endl;

  // get data 
  Int_t ndata = GetOnlineSummaryDat(filename);

  // superpose plots
  TGraph * online_dlayer_plot = new TGraph(ndata, online.RunID, online.Dl);

  online_dlayer_plot -> SetMarkerStyle(23);
  online_dlayer_plot -> SetMarkerSize(1.2);
  online_dlayer_plot -> SetMarkerColor(35);
  online_dlayer_plot -> SetLineColor(35);

  return online_dlayer_plot;
}








#endif
