#!/bin/bash

ifile=$1
ofile=tbin.dat
odfile=tdbin.dat

cat $ifile | grep EnergyBin  | sed -e '{s/EnergyBin://}'  > $ofile
cat $ifile | grep EnergydBin | sed -e '{s/EnergydBin://}' > $odfile



