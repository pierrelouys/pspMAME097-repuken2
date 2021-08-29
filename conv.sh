#!/bin/bash

shopt -s nullglob

#list="c mak txt"
list="mak txt"
for ext in $list
do
    echo "$ext"
    for f in `find . -type f -name "*.$ext"`  
    do
	    echo "Processing $f"
        base=$(basename "$f")
        dirna=$(dirname "$f")
        fulldir="./altdir"${dirna#.}"/"
        mkdir -p $fulldir
        echo $base
        nkf -S -w $f > $fulldir"$base"
        #read -p "Press any key to resume ..."
    done
done

shopt -u nullglob

