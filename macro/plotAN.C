

using namespace std;


void plotAN()
{
   string   fFileName("~/cnipol_results/pp_ANvsSqrts");
   ifstream infile(fFileName.c_str());
   string   strName, strDummy;
   Double_t sqrtS, imR5, imR5ErrStat, imR5ErrTot, r5, r5ErrStat, r5ErrTot;

   // loop over the files
   while ( file.good() )
      //printf("file: %s\n", fileId.c_str());
      file >> strName >> sqrtS >> imR5 >> strDummy >> imR5ErrStat >> r5 >> strDummy >> r5ErrStat
           >>  imR5 >> strDummy >> imR5ErrTot >> r5 >> strDummy >> r5ErrTot;

      
   }

}
