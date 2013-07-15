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

# If last git commit is already in svn, use svn revision id.
SVN_ID_LINE=`git log -1 --pretty=%B | grep "^git-svn-id: "`
if [ $? = 0 ]; then
	SVNVERSION=`echo $SVN_ID_LINE | cut -d" " -f2 | cut -d"@" -f2`
	echo "${SVNVERSION}G\""
	exit 0
fi

git rev-parse HEAD | xargs echo -n
echo "\";"
