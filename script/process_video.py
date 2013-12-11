#!/usr/bin/python

import sys
import string
import os
import re
import MySQLdb as mdb
from datetime import datetime, timedelta


gPolId2DirMap = {0: 'bluTop', 1: 'yel1', 2: 'blu2', 3: 'yel2'}


def main():

   vFileInfos = {}

   for polId, dir in gPolId2DirMap.iteritems():
      vFileInfos[polId] = getListOfVideoFiles(dir)

   dbCursor = connectDB()

   fileCniMeass = open("/usr/local/polarim/runXX/lists/run13_target_3")
   cniMeass     = fileCniMeass.readlines()

   for measId in cniMeass:

      measId = measId.strip()
      fillId, measCount = measId.split('.')
      print
      print "measId: {0}:\t".format(measId),

      sqlQuery = "SELECT run_name, polarimeter_id, start_time FROM `run_info` WHERE run_name LIKE {0} LIMIT 1".format(measId)
      dbCursor.execute(sqlQuery)
      measInfo = dbCursor.fetchone()
      print measInfo

      if (measInfo is None): continue

      polId = measInfo['polarimeter_id']

      if polId not in gPolId2DirMap: continue

      foundVFileInfo = copyVideoFile(measInfo, vFileInfos[polId])

      if (foundVFileInfo is None): continue


def connectDB():

   try:
      con = mdb.connect('localhost', 'cnipol', '(n!P0l', 'cnipol')
      cur = con.cursor(mdb.cursors.DictCursor)

   except mdb.Error, e:
      print "Error %d: %s" % (e.args[0], e.args[1])
      sys.exit(1)

   return cur


def getListOfVideoFiles(dir):

   cmd = "ssh acnlina \"find /cfs/n/Video/" + dir + " -type f -name '*.avi' -printf '%f\\n' | sort\""
   print "%s\n" % (cmd),
   list = os.popen(cmd, "r")

   vFileInfos = []

   while 1:

      vFileName = list.readline()
      vFileName = vFileName.strip()
      print vFileName

      if not vFileName: break

      # File name example: yel1.2013-05-14-09.57.12.09h53m30s.avi
      m = re.match("(\S+)\.(\d{4})-(\d{2})-(\d{2})-(\d{2})\.(\d{2})\.(\d{2})\.(\d{2})h(\d{2})m(\d{2})s\.avi", vFileName)

      if m is None: continue

      meas_start_time  = datetime( int(m.group(2)), int(m.group(3)), int(m.group(4)), int(m.group(5)), int(m.group(6)), int(m.group(7)) )
      video_start_time = datetime( int(m.group(2)), int(m.group(3)), int(m.group(4)), int(m.group(8)), int(m.group(9)), int(m.group(10)) )

      vFileInfo = ( vFileName, meas_start_time, video_start_time )
      vFileInfos.append( vFileInfo )

   return vFileInfos


def copyVideoFile(measInfo, vFileInfos):

   outFile = "/usr/local/polarim/video/" + measInfo['run_name'] + ".avi"

   if os.path.isfile(outFile):
      print "File " + outFile + " already exists. Skipping..."
      return None

   minDeltaSeconds = 1e8
   foundVFileInfo  = None

   for vFileInfo in vFileInfos:

      #print measInfo, vFileInfo

      measInfo_start_time = measInfo['start_time']
      vFileName, meas_start_time, video_start_time = vFileInfo

      measVideoDelta        = (meas_start_time - video_start_time)
      measVideoDeltaSeconds = measVideoDelta.seconds + measVideoDelta.days * 24 * 3600
      measDelta             = abs(measInfo_start_time - meas_start_time)
      measDeltaSeconds      = measDelta.seconds + measDelta.days * 24 * 3600

      if (measDeltaSeconds < 20 and measVideoDeltaSeconds >= 0 and measVideoDeltaSeconds < minDeltaSeconds):
         minDeltaSeconds = measVideoDeltaSeconds
         foundVFileInfo  = vFileInfo

   if foundVFileInfo is None: return foundVFileInfo

   vFileName, meas_start_time, video_start_time = foundVFileInfo
   polId = measInfo['polarimeter_id']

   cmd = "rsync -auv acnlina:/cfs/n/Video/" + gPolId2DirMap[polId] + "/" + vFileName + " /usr/local/polarim/video/"
   print "%s\n" % (cmd),
   os.system(cmd)

   skipStart = str(minDeltaSeconds)
   cmd = "ffmpeg -y -ss "+skipStart+" -r 30 -i /usr/local/polarim/video/" + \
      vFileName + " -t 60 -b:v 1024k -vcodec mpeg4 -acodec copy /usr/local/polarim/video/" + measInfo['run_name'] + ".avi"
   print "%s\n" % (cmd),
   os.system(cmd)

   cmd = "rm /usr/local/polarim/video/" + vFileName
   print "%s\n" % (cmd),
   os.system(cmd)

   return foundVFileInfo


if __name__ == '__main__':
    main()
