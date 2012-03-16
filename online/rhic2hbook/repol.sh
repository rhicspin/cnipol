
#!/bin/bash

run=$1;


tgt=`grep 'target used' /usr/local/polarim/log/an$run.log | awk '{print $3}'`

if [ $tgt = "vertical" ]; then
  echo 0 > $LOGDIR/repol.dat
elif [ $tgt = "horizonal" ]; then # NOTE MISSPELLING
  echo 1 > $LOGDIR/repol.dat
else
  echo -1 > $LOGDIR/repol.dat
fi
 
grep 'Trueman' /usr/local/polarim/log/an$run.log  | awk '{print $7}' >> $LOGDIR/repol.dat

