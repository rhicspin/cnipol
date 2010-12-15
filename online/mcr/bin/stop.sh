#!/bin/bash

i_d=`ps -ef | grep rpoldaemon | grep $USER | grep -v grep | awk '{ print $2}'`
kill -s TERM $i_d
echo "rpoldaemon stopped!"
exit $status
