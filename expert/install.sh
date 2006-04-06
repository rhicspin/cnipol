#!/bin/bash

# install paw macros:
if [ -d macro ] ; then
	cd macro
	./install.sh
	cd ..
fi

if [ -d script ] ; then
	cd script
	./install.sh
	cd ..
fi




