#!/bin/bash

grep '' studenten.csv | while read line; do
  #Extract the name and first semester grade from the current line
  name=$(echo "$line" | awk -F ',' '{print $1}')
  grade1=$(echo "$line" | awk -F ',' '{print $2}')

  #Calculate the average grade for the student
  grades=$(grep "^$name," notenSommer.csv | awk -F ',' '{for(i=2;i<=NF;i++) print $i}')
  total=0
  count=0
  for grade in $grades; do
    total=$((total + grade))
    count=$((count + 1))
  done
  if [ "$count" -gt 0 ]; then
    avg=$((total / count))
    annual_avg=$(( (grade1 + avg)/2 ))
    #Substitute the name and the initial grade with the name and the annual grade, also -i is used to perform an 
    #in-place edit and not display anything in the console
    sed -i "s/^$name,$grade1/$name,$annual_avg/" studenten.csv
  fi
done
