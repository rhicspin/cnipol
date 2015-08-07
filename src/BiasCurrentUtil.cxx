#include <cmath>
#include <map>
#include <string>
#include <vector>

#include <opencdev.h>

#include <TSystem.h>

#include "BiasCurrentUtil.h"
#include "RunConfig.h"

using std::map;
using std::string;
using std::vector;


string BiasCurrentUtil::GetBiasCurrentLoggerName(EPolarimeterId polId)
{
   switch(RunConfig::GetBeamId(polId))
   {
      case kBLUE_BEAM:
         return "RHIC/Polarimeter/Blue/biasReadbacks";
      case kYELLOW_BEAM:
         return "RHIC/Polarimeter/Yellow/biasReadbacks";
      default:
          throw "Unknown beam type";
   }
}


EPolarimeterId BiasCurrentUtil::ParseLoggerPolId(const std::string &cell_name)
{
#define CHAR_PAIR(c1, c2) (((uint16_t)c1) << 8) | ((uint16_t)c2)
   switch(CHAR_PAIR(cell_name[0], cell_name[10]))
   {
   case CHAR_PAIR('b', '1'):
      return kB1U;
   case CHAR_PAIR('y', '1'):
      return kY1D;
   case CHAR_PAIR('b', '2'):
      return kB2D;
   case CHAR_PAIR('y', '2'):
      return kY2U;
   default:
      throw "Can't parse polarimeter";
   }
#undef CHAR_PAIR
}


vector<double> BiasCurrentUtil::FillBiasCurrentMeanValue(opencdev::mean_result_t &bias_mean_value, EPolarimeterId polId)
{
   vector<double> result(N_DETECTORS, NAN);

   for(map<string, double>::const_iterator it = bias_mean_value.begin(); it != bias_mean_value.end(); it++) {
      const string &key = it->first;
      double value = it->second;
      gSystem->Info("BiasCurrentUtil::FillBiasCurrentMeanValue", "Mean %s equals to %f", key.c_str(), value);

      EPolarimeterId parsed_polId = ParseLoggerPolId(key);
      if (parsed_polId != polId)
      {
         continue;
      }

      if (fabs(value) > 100)
      {
         gSystem->Error("BiasCurrentUtil::FillBiasCurrentMeanValue", "Bias cureent value is too big - Ignoring");
      }
      else
      {
         int det_id = key[15] - '1';
         result.at(det_id) = value;
      }
   }

   return result;
}
