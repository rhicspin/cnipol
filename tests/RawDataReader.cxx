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

int main(void)
{
   save_output(test, output_filename);

   if (!compare(output_filename, stdoe_log, stdoe_log_len))
   {
      std::cerr << "failed" << std::endl;
      return EXIT_FAILURE;
   }

   std::cerr << "passed" << std::endl;
   return EXIT_SUCCESS;
}
