#!/usr/bin/env python

"""
A script to download run periods from MySQL database and output them as a
C-style array.
"""

import os
import datetime
import MySQLdb

def to_mysqlpp(key, val):
    if isinstance(val, datetime.datetime):
        unix_time = "MK_UNIXTIME_NY_TZ(%i, %i, %i, %i, %i, %i)" \
            % (val.year, val.month, val.day, val.hour, val.minute, val.second)
        return "%s /* %s */" % (unix_time, val)
    elif isinstance(val, basestring):
        return "\"%s\"" % val
    elif type(val) is float:
        return str(val)
    elif key == "polarimeter_id":
        m = {0: "kB1U", 1: "kY1D", 2: "kB2D", 3: "kY2U"}
        return m[val]
    else:
        return str(val)

db = MySQLdb.connect(
    host=os.environ["CNIPOL_DB_HOST"],
    user=os.environ["CNIPOL_DB_USER"],
    passwd=os.environ["CNIPOL_DB_PASSWORD"],
    db=os.environ["CNIPOL_DB_NAME"],
    )
cur = db.cursor()
cur.execute("SELECT * FROM `run_period` ORDER BY `start_time` ASC, `polarimeter_id` ASC;")
res = cur.fetchall()
cols = [d[0] for d in cur.description]
for row in res:
    print "{"
    for key, val in zip(cols, row):
        if key == "dl_calib_run_name": # this field is in db but not in MseRunPeriod
            continue
        print "   .%s = %s," % (key, to_mysqlpp(key, val))
    print "},"
