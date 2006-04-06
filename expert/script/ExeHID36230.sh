#!/bin/bash
RUNLIST=.cnipol_daemon_run.list;

for f in `cat $RUNLIST | gawk '{print $1}'`
do 
  echo -e -n "$f "
  export RUNID=$f;
  paw -q -b $MACRODIR/HID36230.kumac > /dev/null
  cat dat/HID36230.dat
done


rm -f dat/HID36230.dat
   
