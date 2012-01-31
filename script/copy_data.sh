#!/bin/sh

LOG="/usr/local/polarim/log/copy_data.log"

date >> ${LOG} 2>&1

#eval `ssh-agent -s` >> ${LOG} 2>&1
#export 
export SSH_AUTH_SOCK=/tmp/ssh-pppCEO1287/agent.1287
export SSH_AGENT_PID=1288

#ssh -Nf rssh >> ${LOG} 2>&1

#env >> ${LOG} 2>&1

rsync -a /usr/local/polarim/data/*.data eic0005:/eicdata/eic0005/run12/data/ >> ${LOG} 2>&1
