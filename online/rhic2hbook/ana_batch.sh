#!/bin/bash
#
# Apr  9, 2012 - Dmitri Smirnov
#    - This script runs online analysis
#

FILE_RUNS=$1

export POLDIR=/usr/local/polarim
export LOGDIR=$POLDIR/log

echo CNIPOL_DIR: $CNIPOL_DIR
echo FILE_RUNS:  $FILE_RUNS
echo LOGDIR:     $LOGDIR

exec < $FILE_RUNS

while read run_name
do
   echo "run: " $run_name
   $CNIPOL_DIR/online/rhic2hbook/ana.sh $run_name
done
