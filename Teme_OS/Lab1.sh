#!/bin/bash

u=($1 $2 $3)
v=($4 $5 $6)

if [ $# -ne 6 ]; then
   echo "You need to give 6 parameters - 3 for vector u, 3 for vector v"
else

product=0
for ((i=0; i<${#u[@]}; i++)); do
  product=$(($product + u[i] * v[i]))
done

if [ $product -eq 0 ]; then
  echo "ja"
else
  echo "nein"
fi
fi
