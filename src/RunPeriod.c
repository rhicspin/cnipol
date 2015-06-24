#include <stdint.h>

#include "RunPeriod.h"

/*
 * Code borrowed from
 * http://stackoverflow.com/questions/10538444/do-you-know-of-a-c-macro-to-compute-unix-time-and-date
 */
#define _UNIXTIME_FDAY(year) \
    (((year) % 400) == 0 ? 29LL : \
        (((year) % 100) == 0 ? 28LL : \
            (((year) % 4) == 0 ? 29LL : \
                28LL)))

#define _UNIXTIME_YDAY(year, month, day) \
    ( \
        /* January */    (int64_t)day \
        /* February */ + ((month) >=  2 ? 31LL : 0LL) \
        /* March */    + ((month) >=  3 ? _UNIXTIME_FDAY(year) : 0LL) \
        /* April */    + ((month) >=  4 ? 31LL : 0LL) \
        /* May */      + ((month) >=  5 ? 30LL : 0LL) \
        /* June */     + ((month) >=  6 ? 31LL : 0LL) \
        /* July */     + ((month) >=  7 ? 30LL : 0LL) \
        /* August */   + ((month) >=  8 ? 31LL : 0LL) \
        /* September */+ ((month) >=  9 ? 31LL : 0LL) \
        /* October */  + ((month) >= 10 ? 30LL : 0LL) \
        /* November */ + ((month) >= 11 ? 31LL : 0LL) \
        /* December */ + ((month) >= 12 ? 30LL : 0LL) \
    )

#define MK_UNIXTIME(year, month, day, hour, minute, second) \
    ( (int64_t)second \
    + (int64_t)minute * 60LL \
    + (int64_t)hour * 3600LL \
    + (_UNIXTIME_YDAY(year, month, day) - 1) * 86400LL \
    + ((int64_t)year - 1970LL) * 31536000LL \
    + ((int64_t)year - 1969LL) / 4LL * 86400LL \
    - ((int64_t)year - 1901LL) / 100LL * 86400LL \
    + ((int64_t)year - 1601LL) / 400LL * 86400LL )

// Convert New York time to UTC UNIX timestamp
#define MK_UNIXTIME_NY_TZ(year, month, day, hour, minute, second) \
   (MK_UNIXTIME(year, month, day, hour, minute, second) + 18000LL)


#define kB1U 0
#define kY1D 1
#define kB2D 2
#define kY2U 3

static const struct RunPeriod run_period_list[] = {
#include "RunPeriod.inc"
};

const struct RunPeriod* find_run_period(time_t start_time, int polId)
{
   const struct RunPeriod *res = NULL;
   int i, size = sizeof(run_period_list) / sizeof(run_period_list[0]);

   for(i = 0; i < size; i++) {
      if (run_period_list[i].polarimeter_id != polId)
         continue;
      if (run_period_list[i].start_time < start_time) {
         if ((res == NULL) || (res->start_time < run_period_list[i].start_time))
            res = &run_period_list[i];
      }
   }

   return res;
}
