#!/bin/bash

echo -n "const char *ASYM_VERSION = \""

git diff --quiet
UNSTAGED=$?

if [ $UNSTAGED = 129 ]; then
        echo "not in git\";"
        exit 0
fi

git diff --cached --quiet
STAGED=$?

if [ $STAGED = 1 ] || [ $UNSTAGED = 1 ]; then
        echo "dirty working tree\";"
        exit 0
fi

git rev-parse HEAD | xargs echo -n
echo "\";"
