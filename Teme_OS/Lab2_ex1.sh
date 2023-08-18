#!/bin/bash

if [ $# -ne 2 ];
then
echo "You need to write two arguments. The first one is the word, the second one is the file"
else

#wir haben die kleine Buchstabe in einem Buffer gespeichert, und wir setzen das Wort das gegeben wird,
# bevor die Buchstabe. '\1' zeigt dass es sich um das erste Buffer handelt

sed -E "s/([a-z])/$1\1/g" $2
fi
