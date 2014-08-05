#include <string>
#include <vector>

#include <opencdev.h>

#include "AsymCommon.h"

class BiasCurrentUtil
{
public:

   static std::string          GetBiasCurrentLoggerName(EPolarimeterId polId);
   static EPolarimeterId       ParseLoggerPolId(const std::string &cell_name);
   static std::vector<double>  FillBiasCurrentMeanValue(opencdev::mean_result_t &bias_mean_value, EPolarimeterId polId);
};
