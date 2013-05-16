
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
   data.get("value", (int*) arg);
}


/** */
int main(int argc, char** argv)
{
   // Hook Action button
   cdevSystem &defSystem = cdevSystem::defaultSystem();

   int fillId = 0;

   cdevCallback cb(handleGetAsync, &fillId);

   cdevDevice &device = cdevDevice::attachRef("ringSpec.blue"); // the source of the fill number
   //cdevData data;
   device.sendCallback("monitorOn fillNumberM", NULL, cb);

   // Infinite loop
   for (;;) {

      defSystem.pend(2);
   }

   return 0;
}
