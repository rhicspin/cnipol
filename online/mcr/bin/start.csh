#!/bin/csh 

cd /usr/local/polarim/bin
# this is wrong - we need to start server depending on our IP
./rpoldaemon.sh -B &

exit $status 
