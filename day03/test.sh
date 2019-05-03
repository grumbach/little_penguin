#!/bin/sh

CHECKPATCH="/sources/linux/scripts/checkpatch.pl"

if [[ -e "$CHECKPATCH" ]]; then
    echo "$CHECKPATCH found";
else
    echo "$CHECKPATCH not found"
    exit 1
fi

ERROR=`$CHECKPATCH --no-tree --strict -f $1 | grep -C10 "ERROR"`

if [[ -n $ERROR ]]; then
    echo "norm KO"
    echo $ERROR
else
    echo "norm OK"
fi
