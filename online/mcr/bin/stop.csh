#!/bin/csh

set id = `ps -ef | grep rpoldaemon | grep $USER | grep -v grep | awk '{ print $2}'`
kill -s TERM $id
echo "rpoldaemon stopped!"
exit $status
