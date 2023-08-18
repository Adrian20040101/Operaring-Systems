#!/bin/bash

if [ $# -ne 1 ]; then
echo "You have to pass one argument, which is the name of the directory"
else
grep -hr --include="*.txt" .  $1
fi
