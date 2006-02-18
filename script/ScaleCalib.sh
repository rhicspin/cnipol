#!/bin/bash

file=yellow_calib_0513c
cat Calib/$file.temp.dat  | awk '{print $1, $2*2, $3, $4, $5, $6, $7, $8}' > Calib/$file.times2.temp.dat
cat Calib/$file.temp.dat  | awk '{print $1, $2*5, $3, $4, $5, $6, $7, $8}' > Calib/$file.times5.temp.dat

file=blue_calib_0513c
cat Calib/$file.temp.dat  | awk '{print $1, $2*2, $3, $4, $5, $6, $7, $8}' > Calib/$file.times2.temp.dat
cat Calib/$file.temp.dat  | awk '{print $1, $2*5, $3, $4, $5, $6, $7, $8}' > Calib/$file.times5.temp.dat


