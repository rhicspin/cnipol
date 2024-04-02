#!/bin/csh

#set dir=`pwd`
source /star/u/zchang/.login
source /star/u/zchang/.cshrc

source /star/u/zchang/run22/cnipol/script/r22setup.csh 22 > /dev/null

#set MY_REMOTE_HOME_DIR=/home/cfsd/zchang

#set CNIPOL_REMOTE_BLUE_DATA_DIR=/home/blue/20$RUN_ID/data
#set CNIPOL_REMOTE_YELLOW_DATA_DIR=/home/yellow/20$RUN_ID/data
setenv CNIPOL_ONLINE_DIR /gpfs02/eic/eichome/cnipol/root
#exec /star/u/zchang/run22/cnipol/script/runAsymR22.sh

#set fill=32923
#set names = ("304" "305" "306" "307" "308" "309" "310" "311" "312" "313" "314" "315" "316" "317")
#set names = ("005" "205" "206")
set fill=32960
set names = ("204" "005" "104")
set opt=""
#set opt="--spin-pattern 13 --fill-pattern 1"
#set names = ("305" "304" "105")
#set opt="--spin-pattern 3 --fill-pattern 1"
@ cntr=0
foreach nn ($names)
  echo $fill.$nn.data
  if ( $cntr == 4) then
     wait
     @ cntr=0
  endif
  #/usr/bin/nohup sleep 10 &
  echo $cntr
  /usr/bin/nohup ./runtest.csh $fill.$nn $opt>& /dev/null &
  #./runtest.csh $fill.$nn $opt
  @ cntr += 1
end

wait
