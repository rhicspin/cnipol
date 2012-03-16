#!/bin/bash

ps x | grep cnipol_daemon.sh | grep -v "grep" | grep -v "emacs" | grep -v "vi" | grep "fast-offline"
if [ $? -eq 1 ] ; then
    echo "cnipol_daemon.sh --fast-offline is not running. Execute ..."
    cnipol_daemon.sh --fast-offline > $TMPOUTDIR/cnipol_daemon.log
fi






 
