#!/bin/bash

echo -n "const char *ASYM_VERSION = \""

git diff --quiet
UNSTAGED=$?

if [ $UNSTAGED = 129 ]; then
	SVNVERSION=`svnversion $1`
	if [ $SVNVERSION = "exported" ]; then
		echo "not in repo\""
		exit 0
	else
		echo "${SVNVERSION}\";"
		exit 0
	fi
fi

git diff --cached --quiet
STAGED=$?

if [ $STAGED = 1 ] || [ $UNSTAGED = 1 ]; then
        echo "dirty working tree\";"
        exit 0
fi

git rev-parse HEAD | xargs echo -n
echo "\";"
