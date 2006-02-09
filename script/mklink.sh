#!/bin/bash


cd $DATADIR
echo "$DATADIR: making data  links ..."
ln.sh
cd ..

cd $ONLINEDIR/hbook
echo  "$ONLINEDIR/hbook: making hbook links ..."
ln.sh
cd ..

cd $ONLINEDIR/log
echo "$ONLINEDIR/log: making  log  links ..."
ln.sh


