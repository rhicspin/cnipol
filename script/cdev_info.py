#!/usr/bin/python

import sys
import string
import os



def main():

   #fileAllMeas = open("/eicdata/eic0005/runXX/lists/run13_phys")
   fileAllMeas = open("/eicdata/eic0005/runXX/lists/run13_new")
   allMeass    = fileAllMeas.readlines()
   fills = []

   for meas in allMeass:
      meas = meas.strip()
      fillId, measId = meas.split('.')
      print "meas: %s" % (meas),

      if (fillId not in fills):
         fills.append(fillId)
         print "insert in fills: %s\n" % (fillId),
      else:
         print "already in fills %s\n" % (fillId),

   for fillId in fills:
      cmd = "ssh -Y acnlina \"~/cnipol_trunk/script/get_fill_info.sh %s\" > /eicdata/eic0005/runXX/cdev_info/cdev_%s" % (fillId, fillId)
      print "%s\n" % (cmd),
      os.system(cmd)



if __name__ == '__main__':
    main()
