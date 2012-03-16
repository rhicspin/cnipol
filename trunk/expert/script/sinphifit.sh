#!/bin/bash

FILE1=dat/Total_HID36230.dat
FILE2=dat/asym_analysis_summary.dat


NLINE=`wc $FILE1 | gawk '{print $1}'` 
for (( i=1; i<$NLINE; i++ )) ; 
do

  RUNID=`line.sh $i $FILE1 | gawk '{print $1}'` ;
  P1=`line.sh $i $FILE1 | gawk '{print $2}'` ;
  dP1=`line.sh $i $FILE1 | gawk '{print $3}'` ;
  phi1=`line.sh $i $FILE1 | gawk '{print $4}'` ;
  dphi1=`line.sh $i $FILE1 | gawk '{print $5}'` ;
  P2=`grep $RUNID $FILE2 | gawk '{print $11}'`;
  dP2=`grep $RUNID $FILE2 | gawk '{print $12}'`;
  phi2=`grep $RUNID $FILE2 | gawk '{print $13}'`;
  dphi2=`grep $RUNID $FILE2 | gawk '{print $14}'`;
  
  echo -e -n "$RUNID ";
  echo $P1 $P2 | gawk '{printf("%7.2f",$2/$1)}';
  echo $dP1 $dP2 | gawk '{printf("%7.2f",$2/$1)}';
  echo $phi1 $phi2 | gawk '{printf("%7.2f",$2/$1)}';
  echo $dphi1 $dphi2 | gawk '{printf("%7.2f",$2/$1)}';
  echo " ";

done
