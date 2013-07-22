#!/bin/bash

echo -n "const char *ASYM_VERSION = \""

# check if git is in PATH
which git 1> /dev/null 2> /dev/null
if [ $? != 1 ]; then
	git diff --quiet
	UNSTAGED=$?
	GIT_FOUND=1
else
	UNSTAGED=0
	GIT_FOUND=0
fi

# if this is not a git repo or git is not found fallback to svn
if [ $UNSTAGED = 129 ] || [ $GIT_FOUND = 0 ]; then
	SVNVERSION=`svnversion $1`
	if [ $SVNVERSION = "exported" ]; then
		echo "not in repo\""
		exit 0
	else
		echo "${SVNVERSION}\";"
		exit 0
	fi
fi

# check if there were staged changes
git diff --cached --quiet
STAGED=$?

if [ $STAGED = 1 ] || [ $UNSTAGED = 1 ]; then
	FLAGS="M"
else
	# check if last git commit is pushed to svn
	SVN_ID_LINE=`git log -1 --pretty=%B | grep "^git-svn-id: "`
	if [ $? != 0 ]; then
		FLAGS="M"
	fi
fi

# Take last svn revision id.
SVN_ID_LINE=`git log --pretty=%B | grep "^git-svn-id: "  | head -n 1`
if [ $? = 0 ]; then
	SVNVERSION=`echo $SVN_ID_LINE | cut -d" " -f2 | cut -d"@" -f2`
	echo "${SVNVERSION}${FLAGS}\";"
	exit 0
fi

echo "unknown\";"
