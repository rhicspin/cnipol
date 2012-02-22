#!/bin/bash

i_d=`ps -ef | grep rpoldaemon | grep -v grep | awk '{ print $2}'`

if [ "x$i_d" != "x" ] ; then
    exit 0 
else
    exit 1
fi

