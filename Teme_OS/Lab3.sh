#!/bin/bash

for file in "$@"; do
   if [ ! -f "$file" ]; then
      echo "You need to type a file"
      break
   fi
done

awk '{for(i=1;i<NF;i++) if($i == $(i+1)) print FNR " " $i}' $@
