#!/bin/bash

bgx_update() {
    #echo bgx_update called...
    bgxoldgrp=${bgxgrp}
    bgxgrp=""
    ((bgxcount = 0))
    bgxjobs=" $(jobs -pr | tr '\n' ' ')"
    #echo bgxjobs $bgxjobs
    for bgxpid in ${bgxoldgrp} ; do
        echo "${bgxjobs}" | grep " ${bgxpid} " >/dev/null 2>&1
        #echo "${bgxjobs}" | grep " ${bgxpid} "
        if [[ $? -eq 0 ]] ; then
            bgxgrp="${bgxgrp} ${bgxpid}"
            ((bgxcount = bgxcount + 1))
        fi
    done
}


bgx_limit() {
    bgxmax=$1 ; shift
    bgx_update
    while [[ ${bgxcount} -ge ${bgxmax} ]] ; do
        sleep 1
        bgx_update
    done
    if [[ "$1" != "-" ]] ; then
        #nice -n 5 $* &
        ionice -c2 -n3 $* &
        #$* &
        bgxgrp="${bgxgrp} $!"
        # to avoid clashing while updating run database
        sleep 2
    fi
}


bgx_wait() {
   while [[ ${bgxcount} -ne 0 ]] ; do
       #echo bgxcount ${bgxcount} oldcount ${oldcount}
       oldcount=${bgxcount}
       while [[ ${oldcount} -eq ${bgxcount} ]] ; do
           sleep 1
           #bgxgrp=${group1} ; bgx_update ; group1=${bgxgrp}
           bgx_update;
       done
       #echo 9 $(date | awk '{print $4}') '[' ${group1} ']'
   done
}
