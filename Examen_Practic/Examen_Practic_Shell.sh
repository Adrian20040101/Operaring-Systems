#!/bin/bash

if [ $# -ne 1 ]; then
   echo "You need exactly one argument (a directory name)!"
fi

grep -rc "" --include='*' $1
