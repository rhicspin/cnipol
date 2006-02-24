#!/bin/bash

LINE=$1
FILE=$2

head -n $LINE $FILE | tail -n 1 

