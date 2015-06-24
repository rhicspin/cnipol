#ifndef RunPeriod_h
#define RunPeriod_h

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

struct RunPeriod
{
   time_t start_time;
   int polarimeter_id;
   const char* alpha_calib_run_name;
   int alpha_source_count;
   const char* disabled_channels;
   const char* disabled_bunches;
   double cut_proto_slope;
   double cut_proto_offset;
   double cut_proto_width;
   double cut_proto_adc_min;
   double cut_proto_adc_max;
   double cut_proto_tdc_min;
   double cut_proto_tdc_max;
   double cut_pulser_adc_min;
   double cut_pulser_adc_max;
   double cut_pulser_tdc_min;
   double cut_pulser_tdc_max;
};

const struct RunPeriod* find_run_period(time_t start_time, int polId);

#ifdef __cplusplus
}
#endif

#endif
