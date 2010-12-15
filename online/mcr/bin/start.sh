#!/bin/bash

cd /usr/local/polarim/bin
./rpoldaemon.sh -D >>/dev/null &
exit $status 
