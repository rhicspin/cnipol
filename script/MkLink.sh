#!/bin/bash

if [ -f $HOME/.cni ] ; then 
     source $HOME/.cni
fi
RAWDATA_LIST=raw_data.list

/usr/local/cnipol/share/bin/mklink.sh --double-pc

#Update raw data file list in $RAWDATA_LIST
cd $DATADIR
ls 9???.???.data          | sed -e 's/.data//' >  $RAWDATA_LIST
ls 1????.???.data 2>/dev/null | sed -e 's/.data//' >> $RAWDATA_LIST
chown e950:e950 $RAWDATA_LIST
cd -

#Update Online Polarization Table
/usr/local/cnipol/share/bin/OnlinePol.sh --update
chown e950:e950 $DATADIR/OnlinePol.dat



