#include <cstdlib>
#include <iostream>

#include "common.h"

// generated using xxd -i
#include "asym_reference.h"

const char *output_filename = "asym_test.log";

int asym_main(int argc, char *argv[]);

void test()
{
   char *argv[] = {NULL, "--alpha", "-r", "150213a.blu2.alpha0", "-l", ""};
   asym_main(6, argv);
}

int main(void)
{
   save_output(test, output_filename);

   if (!compare(output_filename, asym_test_log, asym_test_log_len))
   {
      std::cerr << "failed" << std::endl;
      return EXIT_FAILURE;
   }

   std::cerr << "passed" << std::endl;
   return EXIT_SUCCESS;
}
