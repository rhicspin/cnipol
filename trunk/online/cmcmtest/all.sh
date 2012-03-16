#!/bin/bash
CMD=./cmcmtest

for n in 4 5 6 9 10 11 14 15 16 19 20 21 ; do
    $CMD $n 1
done ;

for n in 6 7 9 10 12 13 15 16 18 19 21 22 ; do
    $CMD $n 2
done ;

for n in 2 4 6 8 10 12 14 16 18 ; do
    $CMD $n 3
done ;

for n in 2 4 6 8 10 12 14 16 18 ; do
    $CMD $n 4
done ;

for n in 7 9 11 13 15 ; do
    $CMD $n 5
done ;
