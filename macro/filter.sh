#!/bin/bash

ifile=$1
ofile=tgt.dat

cat $ifile | grep @ | gawk '{printf("%6d%10d%8d%8d\n", $2, $3, $4, $5)}' > x.dat
echo "*Index  delimiter  x-rot    y-rot" > $ofile
$INSTALLDIR/interval >> $ofile
rm -f x.dat

