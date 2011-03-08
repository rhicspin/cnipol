#!/bin/csh

set id = `ps -ef | grep rpolado | grep $USER | grep -v grep | awk '{ print $2}'`
kill -s TERM $id
echo "rpolado stopped!"
exit $status
