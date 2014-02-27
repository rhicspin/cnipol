#include "assert.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"

#include "common.h"

/**
 * This routine will run testfunc() saving all its output
 * to a file.
 */
void save_output(void (*testfunc)(), const char *output_filename)
{
   // enable redirect to file
   int sout = dup(fileno(stdout));
   int serr = dup(fileno(stderr));
   freopen(output_filename, "w", stdout);
   dup2(fileno(stdout), fileno(stderr));

   (*testfunc)();

   // disable redirect to file
   fflush(stdout);
   fflush(stderr);
   dup2(sout,fileno(stdout));
   dup2(serr,fileno(stderr));
   close(sout);
   close(serr);
}

char *read_file(const char *filename, unsigned long *size)
{
   // read output
   FILE *fp = fopen(filename, "r");
   fseek(fp, 0, SEEK_END);
   *size = ftell(fp);
   fseek(fp, 0, SEEK_SET);
   char *buf = new char[*size];
   assert(fread(buf, 1, *size, fp) == *size);
   fclose(fp);
   return buf;
}

bool compare(const char *output_filename, const unsigned char *reference_buf, unsigned long reference_len)
{
   bool retval = true;
   unsigned long size;
   char *buf = read_file(output_filename, &size);

   if ((size != reference_len) || (memcmp(buf, reference_buf, reference_len) != 0))
   {
      const char *filename = "golden_reference.log";
      FILE *fp = fopen(filename, "w");
      fwrite(reference_buf, 1, reference_len, fp);
      fclose(fp);
      const unsigned N = 256;
      char cmd[N];
      snprintf(cmd, N, "diff %s %s", filename, output_filename);
      system(cmd);
      retval = false;
   }
   delete[] buf;
   return retval;
}
