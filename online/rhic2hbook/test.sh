#!/bin/bash
#
# Apr  9, 2012 - Dmitri Smirnov
#    - Polarimeter id can be recognized automatically for standard run notation ?????.P??
#

source /usr/local/cnipol_trunk/setup.sh
source $CNIPOL_DIR/online/setup_online.sh

$CNIPOL_DIR/online/rhic2hbook/ana.sh $1 $2
