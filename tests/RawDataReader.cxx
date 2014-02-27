#include "common.h"

#include "AsymRoot.h"
#include "RawDataReader.h"

// generated using xxd -i
#include "RawDataReader_reference.h"

const char output_filename[] = "stdoe.log";

void test()
{
   RawDataReader rawDataReader("17068.202.data");

   gAsymAnaInfo = new AsymAnaInfo();
   gMeasInfo = new MeasInfo();
   gAsymRoot = new AsymRoot();
   gAsymRoot->CreateRootFile("/dev/null");

   MseMeasInfoX mseMeasInfoX("run123");
   rawDataReader.ReadRecBegin(mseMeasInfoX);
   rawDataReader.ReadMeasInfo(mseMeasInfoX);

   delete gAsymRoot;
   delete gMeasInfo;
   delete gAsymAnaInfo;
}

void fail()
{
   FILE *fp = fopen("golden_reference.log", "w");
   fwrite(stdoe_log, 1, stdoe_log_len, fp);
   fclose(fp);
   system(Form("diff golden_reference.log %s", output_filename));
}

int main(void)
{
   save_output(test, output_filename);

   unsigned long size;
   char *buf = read_file(output_filename, &size);

   if ((size != stdoe_log_len) || (memcmp(buf, stdoe_log, stdoe_log_len) != 0))
   {
      std::cerr << "failed" << std::endl;
      fail();
      return EXIT_FAILURE;
   }
   delete[] buf;
   std::cerr << "passed" << std::endl;

   return EXIT_SUCCESS;
}
