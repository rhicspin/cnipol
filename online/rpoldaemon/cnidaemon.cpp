
/********************************************************
 ********************************************************/

/**
 * Jan 30, 2012 - Dmitri Smirnov
 *    -
 */

#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdarg.h>
#include <cdevRequestObject.h>
#include <cdevCallback.h>
#include <cdevSystem.h>

#include "globals.h"
#include "rhicpol.h"
#include "rcdev.h"



// this function removes \n from the standard ctime function output


/** Asynchronous get handler */
void handleGetAsync(int status, void* arg, cdevRequestObject& req, cdevData& data)
{
   cdevDevice& device = req.device();
   //data.get("value", &fillNumber);
   data.get("value", (double*) arg);
   printf("value: %f\n", *((double*) arg));
}

/** */
int main(int argc, char** argv)
{
   // Hook Action button

   setbuf(stdout, NULL);

   //long fillId = 0;

   cdevData data;
   cdevDevice &device = cdevDevice::attachRef("ringSpec.blue"); // the source of the fill number
   //cdevDevice &device = cdevDevice::attachRef("wcm.blue"); // the source of the fill number

   //int value = 0;
   double value = 0;

   cdevSystem &defSystem = cdevSystem::defaultSystem();
   //cdevCallback cb(handleGetAsync, &fillId);
   cdevCallback cb(handleGetAsync, &value);

   //device.sendCallback("monitorOn fillNumberM", data, cb);
   //device.sendCallback("monitorOn wcmBeamM", NULL, cb);

   device.send("get fillNumberM", NULL, data);
   data.get("value", &value);

   printf("value my: %f \n", value);

	return 0;


   // Infinite loop
   for (int i=0; ; i++) {

      printf("value2: %f - %d\n", value, i);

      defSystem.pend();
      //defSystem.poll();
   }

   return 0;
}
