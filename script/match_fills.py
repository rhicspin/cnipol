
import sys
import string
#import getopt

def main():

   fileAllMeas   = open("run13_phys")
   fileHJetFills = open("run13_phys_hjet")

   allMeass  = fileAllMeas.readlines()
   hJetFills = fileHJetFills.readlines()

   for meas in allMeass:
      meas = meas.strip()
      fillId, measId = meas.split('.')
      #print fillId, measId
      #print "meas: %s\n" % (fillId),

      for hjFill in hJetFills:
         #print "hjFill: " + hjFill,

         hjFill = hjFill.strip()

         if (fillId == hjFill):
            #print "use meas: %s" % (meas),
            print meas
         #else:
         #   print "do not use meas: %s" % (meas),


if __name__ == '__main__':
    main()
