#!/bin/csh

set id = `ps -ef | grep rpoldaemon | grep $USER | grep -v grep | awk '{ print $2}'`

if ( $id != "" ) then
    exit 0 
else
    exit 1
endif
