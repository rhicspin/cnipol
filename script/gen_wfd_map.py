#!/usr/bin/env python

from __future__ import with_statement

CHANNEL_TAG = "Channel"

def read_ini(filepath):
    with open(filepath) as fp:
        lines = fp.readlines();
    result = {}
    for line in lines:
        if line.startswith(CHANNEL_TAG):
            line = line.split()[0]
            line = line[len(CHANNEL_TAG):]
            channel_id, addr = line.split("=")
            channel_id = int(channel_id)
            result[channel_id] = addr
    return result

def f(offset):
    def _((ch, addr)):
        tup = map(int, addr.split("."))
        wfd = tup[1] + tup[0] - offset
        return wfd
    return _

ch_map = {}
ch_map.update(read_ini("crate3.ini"))
ch_map.update(read_ini("crate4.ini"))
print map(f(4), ch_map.items())

ch_map = {}
ch_map.update(read_ini("crate5.ini"))
ch_map.update(read_ini("crate6.ini"))
print map(f(6), ch_map.items())
