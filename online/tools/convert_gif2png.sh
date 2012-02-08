#!/bin/bash

FILE_PATTERN=$1
FILE_LIST=(`ls ${FILE_PATTERN}`)

echo "Will process ${#FILE_LIST[@]} file(s)"

for file_name in ${FILE_LIST[@]}
do
   file_name=${file_name%.gif} 
   convert ${file_name}.gif[0] ${file_name}.a.png
   convert ${file_name}.gif[1] ${file_name}.b.png
   echo "${file_name} "
done

echo
