#!/bin/bash

function print_version_definition()
{
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

	# check if there were staged changes
	git diff --cached --quiet
	STAGED=$?

	git describe --abbrev=0 --tags --exact 2> /dev/null
	UNSTABLE=$?

	if [ $STAGED = 0 ] && [ $UNSTAGED = 0 ]; then
		if [ $UNSTABLE = 128 ]; then
			FLAGS="M"
		fi

		TAG=`git describe --abbrev=0 --tags 2> /dev/null`
		echo -n "${TAG}${FLAGS}"
	else
		echo -n "dirty"
	fi
	echo -n ";"
	git rev-parse HEAD | xargs echo -n
	echo "\";"
}

DEFINITION_STR=`print_version_definition`
FILE_CONTENTS=`cat $2 2> /dev/null`
if [ "$DEFINITION_STR" != "$FILE_CONTENTS" ]; then
	echo $DEFINITION_STR > $2
fi
