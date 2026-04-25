#!/bin/bash

while read line
do
    ./format $line >> ../catalog.c
done < ../catalog.txt
